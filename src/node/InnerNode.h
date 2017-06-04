#ifndef TERM_PROJECT_INNERNODE_H
#define TERM_PROJECT_INNERNODE_H

#include "BaseNodeClass.h"
#include "../utilities.h"

template <typename Key>
class InnerNode : public BaseNodeClass<Key>{
protected:
    typedef InnerNode<Key> inner_node_t;
    typedef BaseNodeClass<Key> super_t;

    std::vector<size_t> children;
public:

// ===== Base node methods =====

    InnerNode();
    InnerNode(const std::vector<uint8_t> & src, size_t beg);
    InnerNode(const inner_node_t & other);
    InnerNode(inner_node_t && other);
    InnerNode &operator=(const InnerNode & other) = default;

    bool isLeaf() const override;

    std::tuple<inner_node_t, inner_node_t> split(size_t id_l, size_t id_r) const;
    size_t toBinary(std::vector<uint8_t> & res, size_t index) const override;

// ===== Inner node methods =====

    const Key & deleteKey(size_t index);
    const Key & updateKey(const Key & key, size_t index);
    const Key & addKey(const Key & key, size_t child_id, size_t index);

    size_t getChild(size_t index) const;
    void setChild(size_t index, size_t new_child);
    void setKey(size_t index, const Key & new_key);

    void join(const inner_node_t & other);
};

template <typename Key>
InnerNode<Key>::InnerNode() :
        super_t::BaseNodeClass() { }

template <typename Key>
InnerNode<Key>::InnerNode(const inner_node_t & other) :
        children(other.children)
{
    this->id = other.id;
    this->is_deleted = other.is_deleted;
    this->keys = other.keys;
}

template <typename Key>
InnerNode<Key>::InnerNode(inner_node_t && other) :
        children(std::move(other.children))
{
    assert(other.is_deleted);
    this->id = other.id;
    this->is_deleted = other.is_deleted;
    this->keys = other.keys;
}


template <typename Key>
bool InnerNode<Key>::isLeaf() const {
    assert(!this->is_deleted);
    return false;
}

template <typename Key>
std::tuple<InnerNode<Key>, InnerNode<Key>> InnerNode<Key>::split(size_t id_l, size_t id_r) const {
    assert(!this->is_deleted);
    inner_node_t left, right;
    left.setId(id_l);       // setting given ids
    right.setId(id_r);

    auto middle = this->keys.size() / 2;            // splitting arrays of values and keys by the middle
    left.keys.insert(left.keys.begin(), this->keys.begin(), this->keys.begin() + middle);         // and copying the parts into the corresponding nodes
    left.children.insert(left.children.begin(), this->children.begin(), this->children.begin() + middle);
    right.keys.insert(right.keys.begin(), this->keys.begin() + middle, this->keys.end());
    right.children.insert(right.children.begin(), this->children.begin() + middle, this->children.end());

    return std::make_tuple(left, right);
}


template <typename Key>
InnerNode<Key>::InnerNode(const std::vector<uint8_t> & src, size_t beg)
        : super_t::BaseNodeClass(src, beg)
{
    assert(beg + this->size() * sizeof(size_t) <= src.size());
    for (size_t index = 0; index < this->size(); index++)
        children.push_back(deserialize<size_t>(src, beg + index * sizeof(size_t)));
}

template <typename Key>
size_t InnerNode<Key>::toBinary(std::vector<uint8_t> & res, size_t index) const {
    index = super_t::toBinary(res, index);

    for (size_t i = 0; i < this->size(); i++)
        index = serialize(children[i], res, index);
    return index;
}

// ===== Inner node methods =====

template  <typename Key>
const Key & InnerNode<Key>::deleteKey(size_t index) {
    assert(!this->is_deleted);
    assert(index < this->size());
    this->keys.erase(this->keys.begin() + index);
    this->children.erase(this->children.begin() + index);
    return this->getMax();
}

template  <typename Key>
const Key & InnerNode<Key>::updateKey(const Key & key, size_t index) {
    assert(!this->is_deleted);
    assert(index < this->size());
    this->keys[index] = key;
    return this->getMax();
}

template  <typename Key>
const Key & InnerNode<Key>::addKey(const Key & key, size_t child_id, size_t index) {
    assert(!this->is_deleted);
    assert(index <= this->size());
    this->keys.insert(this->keys.begin() + index, key);
    this->children.insert(this->children.begin() + index, child_id);
    return this->getMax();
}

template <typename Key>
size_t InnerNode<Key>::getChild(size_t index) const {
    assert(!this->is_deleted);
    assert(index < this->size());
    return children[index];
}

template <typename Key>
void InnerNode<Key>::setChild(size_t index, size_t new_child) {
    assert(!this->is_deleted);
    assert(index < this->size());
    children[index] = new_child;
}

template <typename Key>
void InnerNode<Key>::setKey(size_t index, const Key & new_key) {
    assert(!this->is_deleted);
    assert(index < this->size());
    this->keys = new_key;
}

template <typename Key>
void InnerNode<Key>::join(const inner_node_t & other) {
    assert(!this->is_deleted);
    assert(!other.is_deleted);
    this->keys.insert(this->keys.end(), other.keys.begin(), other.keys.end());
    this->children.insert(this->children.end(), other.children.begin(), other.children.end());
}

#endif //TERM_PROJECT_INNERNODE_H
