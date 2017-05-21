//
// Created by annikura on 16.05.17.
//

#ifndef TERM_PROJECT_LEAFNODE_H
#define TERM_PROJECT_LEAFNODE_H

#include <cassert>
#include "BaseNodeClass.h"

template <typename Key, typename Value, typename ValueStorage>
class LeafNode : public BaseNodeClass<Key, LeafNode<Key, Value, ValueStorage>>{
protected:
    typedef LeafNode<Key, Value,  ValueStorage> leaf_node_t;
    typedef BaseNodeClass<Key, leaf_node_t> super_t;

    std::vector<size_t > values;
    size_t next = -1, prev = -1;
    static ValueStorage value_storage;
public:
// ===== Base node methods =====
    LeafNode();
    LeafNode(const leaf_node_t & other);
    LeafNode(leaf_node_t && other);

    bool isLeaf() const override;

    std::tuple<leaf_node_t, leaf_node_t> split(size_t id_l, size_t id_r) const override;

// ===== Leaf node methods =====

    size_t getNext() const;
    size_t getPrev() const;
    void setNext(size_t id);
    void setPrev(size_t id);

    Value getValue(size_t index) const;
    void setValue(const Value & value, size_t index);

    const Key & addKey(size_t  index, size_t val_id, const Key & key, const Value & value); // both are returning new max key in the node
    const Key & deleteKey(size_t index); // if there is no key left, return the deleted key

    void join(const leaf_node_t & other);
};

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
bool LeafNode<Key, Value, ValueStorage>::isLeaf() const {
    assert(!this->is_deleted);
    return true;
}

/*
template <typename Key, typename Value, typename ValueStorage>
void LeafNode<Key, Value, ValueStorage>::commit() {
    if (this->is_deleted)
        this->node_storage.erase(this->id);
    else
        this->node_storage.insert_or_assign(this->id, *this);
}
*/

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

    auto middle_it = this->keys.begin() + this->keys.size() / 2 + 1;            // splitting arrays of values and keys by the middle
    left.keys.insert(left.keys.begin(), this->keys.begin(), middle_it);         // and copying the parts into the corresponding nodes
    left.values.insert(left.values.begin(), this->values.begin(), middle_it);
    right.keys.insert(right.keys.begin(), middle_it, this->keys.end());
    right.values.insert(right.values.begin(), middle_it, this->values.end());

    return {left, right};
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
    return value_storage.find(values[index])->second;
}

template <typename Key, typename Value, typename ValueStorage>
void LeafNode<Key, Value, ValueStorage>::setValue(const Value & value, size_t index) {
    assert(index < values.size());
    assert(!this->is_deleted);
    value_storage.insert_or_assign(values[index], value);
}

template <typename Key, typename Value, typename ValueStorage>
const Key & LeafNode<Key, Value, ValueStorage>::addKey(size_t index, size_t val_id, const Key & key, const Value & value) {
    assert(index <= this->keys.size());
    assert(!this->is_deleted);
    this->keys.insert(this->keys.begin() + index, key);
    values.insert(values.begin() + index, val_id);
    setValue(index, value);
    return this->getMax();
}

template <typename Key, typename Value, typename ValueStorage>
const Key & LeafNode<Key, Value, ValueStorage>::deleteKey(size_t index) {
    assert(index < this->size());
    assert(!this->is_deleted);
    value_storage.erase(values[index]);
    values.erase(values.begin() + index);
    if (this->size() > 1) {
        this->keys.erase(this->keys.begin() + index);
        return this->getMax();
    }
    else {
        Key key = this->keys[0];
        this->keys.clear();
        return key;
    }
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
