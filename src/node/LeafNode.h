#ifndef TERM_PROJECT_LEAFNODE_H
#define TERM_PROJECT_LEAFNODE_H

#include <cassert>
#include <iostream>
#include "BaseNodeClass.h"
#include "../utilities.h"

template <typename Key, typename Value, typename ValueStorage>
class LeafNode : public BaseNodeClass<Key>{
protected:
    typedef LeafNode<Key, Value,  ValueStorage> leaf_node_t;
    typedef BaseNodeClass<Key> super_t;

    std::vector<size_t> values;
    size_t next = -1, prev = -1;
    static ValueStorage value_storage;
public:
// ===== Base node methods =====
    LeafNode();
    LeafNode(const leaf_node_t & other);
    LeafNode(leaf_node_t && other);
    LeafNode & operator=(const LeafNode & other);
    bool isLeaf() const override;

    std::tuple<leaf_node_t, leaf_node_t> split(size_t id_l, size_t id_r) const;

// ===== Binary mode =====

    LeafNode(const std::vector<uint8_t> & src, size_t beg);
    size_t toBinary(std::vector<uint8_t> & res, size_t index) const override;

// ===== Leaf node methods =====

    size_t getNext() const;
    size_t getPrev() const;
    void setNext(size_t id);
    void setPrev(size_t id);

    Value getValue(size_t index) const;
    void setValue(const Value & value, size_t index);

    void addKey(size_t  index, size_t val_id, const Key & key, const Value & value);
    void deleteKey(size_t index);

    void join(const leaf_node_t & other);
};

template <typename Key, typename Value, typename ValueStorage>
ValueStorage LeafNode<Key, Value, ValueStorage>::value_storage;

template <typename Key, typename Value, typename ValueStorage>
LeafNode<Key, Value, ValueStorage>::LeafNode() :
        next(-1),
        prev(-1),
        super_t::BaseNodeClass() { }


template <typename Key, typename Value, typename ValueStorage>
LeafNode<Key, Value, ValueStorage>::LeafNode(const leaf_node_t & other) :
        values(other.values),
        next(other.next),
        prev(other.prev)
{
    this->keys = other.keys;
    this->id = other.id;
    this->is_deleted = other.is_deleted;
}

template <typename Key, typename Value, typename ValueStorage>
LeafNode<Key, Value, ValueStorage>::LeafNode(leaf_node_t && other) :
        values(std::move(other.values)),
        next(other.next),
        prev(other.prev)
{
    this->keys = std::move(other.keys);
    this->id = other.id;
    this->is_deleted = other.is_deleted;
}

template <typename Key, typename Value, typename ValueStorage>
LeafNode<Key, Value, ValueStorage> & LeafNode<Key, Value, ValueStorage>::operator=(const LeafNode & other) {
    this->keys = other.keys;
    this->values = other.values;
    this->id = other.id;
    this->is_deleted = other.is_deleted;
    this->next = other.next;
    this->prev = other.prev;
    return *this;
}

template <typename Key, typename Value, typename ValueStorage>
bool LeafNode<Key, Value, ValueStorage>::isLeaf() const {
    assert(!this->is_deleted);
    return true;
}

template <typename Key, typename Value, typename ValueStorage>
std::tuple<LeafNode<Key, Value, ValueStorage>,
           LeafNode<Key, Value, ValueStorage>>
           LeafNode<Key, Value, ValueStorage>::split(size_t id_l, size_t id_r) const {
    assert(!this->is_deleted);
    leaf_node_t left, right;
    left.setId(id_l);       // setting given ids
    right.setId(id_r);

    left.next = id_r;       // setting links in the list of leaves
    left.prev = prev;
    right.prev = id_l;
    right.next = next;

    size_t middle = this->keys.size() / 2;            // splitting arrays of values and keys by the middle
    left.keys.insert(left.keys.begin(), this->keys.begin(), this->keys.begin() + middle);         // and copying the parts into the corresponding nodes
    left.values.insert(left.values.begin(), this->values.begin(), this->values.begin() + middle);
    right.keys.insert(right.keys.begin(), this->keys.begin() + middle, this->keys.end());
    right.values.insert(right.values.begin(), this->values.begin() + middle, this->values.end());

    return std::make_tuple(left, right);
}

// ===== Binary mode =====

template <typename Key, typename Value, typename ValueStorage>
LeafNode<Key, Value, ValueStorage>::LeafNode(const std::vector<uint8_t> & src, size_t beg)
        : super_t::BaseNodeClass(src, beg)
{
    assert(beg + sizeof(next) + sizeof(prev) <= src.size());
    next = deserialize<size_t>(src, beg);
    beg += sizeof(next);
    prev = deserialize<size_t>(src, beg);
    beg += sizeof(prev);
    assert(beg + this->size() * sizeof(size_t) <= src.size());
    for (size_t index = 0; index < this->size(); index++)
        values.push_back(deserialize<size_t>(src, beg + index * sizeof(size_t)));
}


template <typename Key, typename Value, typename ValueStorage>
size_t LeafNode<Key, Value, ValueStorage>::toBinary(std::vector<uint8_t> & res, size_t index) const {
    index = super_t::toBinary(res, index);
    index = serialize(next, res, index);
    index = serialize(prev, res, index);

    for (size_t i = 0; i < this->size(); i++)
        index = serialize(values[i], res, index);
    return index;
}

// ===== Leaf node methods =====

template <typename Key, typename Value, typename ValueStorage>
size_t LeafNode<Key, Value, ValueStorage>::getNext() const {
    assert(!this->is_deleted);
    return next;
}

template <typename Key, typename Value, typename ValueStorage>
size_t LeafNode<Key, Value, ValueStorage>::getPrev() const {
    assert(!this->is_deleted);
    return prev;
}

template <typename Key, typename Value, typename ValueStorage>
void LeafNode<Key, Value, ValueStorage>::setNext(size_t id) {
    assert(!this->is_deleted);
    next = id;
}

template <typename Key, typename Value, typename ValueStorage>
void LeafNode<Key, Value, ValueStorage>::setPrev(size_t id) {
    assert(!this->is_deleted);
    prev = id;
}

template <typename Key, typename Value, typename ValueStorage>
Value LeafNode<Key, Value, ValueStorage>::getValue(size_t index) const {
    assert(index < values.size());
    assert(!this->is_deleted);
    return deserialize<Value>(value_storage.find(values[index])->second, 0);
}

template <typename Key, typename Value, typename ValueStorage>
void LeafNode<Key, Value, ValueStorage>::setValue(const Value & value, size_t index) {
    assert(index < values.size());
    assert(!this->is_deleted);
    auto it = this->value_storage.find(values[index]);
    if (it != this->value_storage.end())
        this->value_storage.erase(it);
    this->value_storage.insert(std::make_pair(values[index], serialize(value)));
}

template <typename Key, typename Value, typename ValueStorage>
void LeafNode<Key, Value, ValueStorage>::addKey(size_t index, size_t val_id, const Key & key, const Value & value) {
    assert(index <= this->keys.size());
    assert(!this->is_deleted);
    this->keys.insert(this->keys.begin() + index, key);
    values.insert(values.begin() + index, val_id);
    setValue(value, index);
}

template <typename Key, typename Value, typename ValueStorage>
void LeafNode<Key, Value, ValueStorage>::deleteKey(size_t index) {
    assert(index < this->size());
    assert(!this->is_deleted);
    value_storage.erase(value_storage.find(values[index]));
    values.erase(values.begin() + index);
    this->keys.erase(this->keys.begin() + index);
}


template <typename Key, typename Value, typename ValueStorage>
void  LeafNode<Key, Value, ValueStorage>::join(const leaf_node_t & other) {
    assert(!this->is_deleted);
    assert(!other.is_deleted);
    this->keys.insert(this->keys.end(), other.keys.begin(), other.keys.end());
    this->values.insert(this->values.end(), other.values.begin(), other.values.end());

    next = other.next;
}


#endif //TERM_PROJECT_LEAFNODE_H
