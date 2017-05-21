//
// Created by annikura on 13.05.17.
//

#ifndef TERM_PROJECT_INNERNODE_H
#define TERM_PROJECT_INNERNODE_H

#include "BaseNodeClass.h"

template <typename Key>
class InnerNode : public BaseNodeClass<Key, InnerNode<Key>>{
protected:
    typedef InnerNode<Key> inner_node_t;
    typedef BaseNodeClass<Key, inner_node_t> super_t;

    std::vector<size_t> children;
public:

// ===== Base node methods =====

    InnerNode();
    InnerNode(const inner_node_t & other);
    InnerNode(inner_node_t && other);

    bool isLeaf() const override;

    std::tuple<inner_node_t, inner_node_t> split(size_t id_l, size_t id_r) const override;

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

    auto middle_it = this->keys.begin() + this->keys.size() / 2 + 1;            // splitting arrays of values and keys by the middle
    left.keys.insert(left.keys.begin(), this->keys.begin(), middle_it);         // and copying the parts into the corresponding nodes
    left.children.insert(left.children.begin(), this->children.begin(), middle_it);
    right.keys.insert(right.keys.begin(), middle_it, this->keys.end());
    right.children.insert(right.children.begin(), middle_it, this->children.end());

    return {left, right};
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
    assert(index < this->size());
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
    assert(other.is_deleted);
    this->keys.insert(this->keys.end(), other.keys.begin(), other.keys.end());
    this->children.insert(this->children.end(), other.children.begin(), other.children.end());
}

#endif //TERM_PROJECT_INNERNODE_H
