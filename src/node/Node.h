//
// Created by annikura on 16.05.17.
//

#ifndef TERM_PROJECT_NODE_H
#define TERM_PROJECT_NODE_H


#include "LeafNode.h"
#include "InnerNode.h"

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
class Node{
protected:
    typedef Node<Key, Value, ValueStorage> node_t;
    typedef InnerNode<Key> inner_node_t;
    typedef LeafNode<Key, Value, ValueStorage> leaf_node_t;

    //union {
        inner_node_t innerNode;
        leaf_node_t leafNode;
    //};

    enum class NodeType {UNKNOWN, INNER, LEAF} node_type = NodeType::UNKNOWN;

public:
// ===== Base methods =====

    Node() { leafNode = leaf_node_t(); };
    Node(const node_t & node);
    Node(const leaf_node_t & other);
    Node(const leaf_node_t && other);
    Node(const inner_node_t & other);
    Node(const inner_node_t && other);
    Node & operator=(const Node & other);
    ~Node();

    size_t size() const;
    size_t getId() const;
    void setId(size_t new_id);
    bool isLeaf() const;

    void setDeleted();

    size_t find(const Key & key, std::function<bool(const Key &, const Key &)>) const;

    std::tuple<node_t, node_t> split(size_t left_is, size_t right_id) const;
    void join(const Node & other);
    const Key & getMax() const;
    const Key & getKey(size_t index) const;
    void deleteKey(size_t index);


// ===== Binary mode methods =====

    explicit Node(const std::vector<uint8_t > & src);
    std::vector<uint8_t> toBinary() const;

// ===== Leaf node methods =====

    size_t getNext() const;
    size_t getPrev() const;

    void setNext(size_t id);
    void setPrev(size_t id);

    Value getValue(size_t index) const;

    void setValue(const Value & value, size_t index);

    void addKey(size_t index, size_t val_id, const Key & key, const Value & value); // both are returning new max key in the node


// ===== Inner node methods =====

    const Key & updateKey(const Key & key, size_t index);
    const Key & addKey(const Key & key, size_t child_id, size_t index);


    size_t getChild(size_t index) const;

    void setChild(size_t index, size_t new_child);
    void setKey(size_t index, const Key & new_key);
};

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
Node<Key, Value, ValueStorage>::Node(const node_t & node) {
    assert(node.node_type != NodeType::UNKNOWN);
    node_type = node.node_type;
    if (node_type == NodeType::LEAF)
        leafNode = node.leafNode;
    else
        innerNode = node.innerNode;
}

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
Node<Key, Value, ValueStorage>::Node(const leaf_node_t & other) :
        node_type(NodeType::LEAF),
        leafNode(other) { }

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
Node<Key, Value, ValueStorage>::Node(const leaf_node_t && other) :
        node_type(NodeType::LEAF),
        leafNode(std::move(other)) { } // odd move?

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
Node<Key, Value, ValueStorage>::Node(const inner_node_t & other) :
        node_type(NodeType::INNER),
        innerNode(other) { }

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
Node<Key, Value, ValueStorage>::Node(const inner_node_t && other) :
        node_type(NodeType::INNER),
        innerNode(std::move(other)) { }

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
Node<Key, Value, ValueStorage> & Node<Key, Value, ValueStorage>::operator=(const node_t & other) {
    assert(other.node_type != NodeType::UNKNOWN);
    node_type = other.node_type;
    if (node_type == NodeType::LEAF)
        leafNode = other.leafNode;
    else
        innerNode = other.innerNode;
    return *this;
}


template <
        typename Key,
        typename Value,
        typename ValueStorage
>
Node<Key, Value, ValueStorage>::~Node() {
}

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
size_t Node<Key, Value, ValueStorage>::size() const {
    assert(node_type != NodeType::UNKNOWN);
    if (node_type == NodeType::LEAF) {
        return leafNode.size();
    }
    else {
        return innerNode.size();
    }
}

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
size_t Node<Key, Value, ValueStorage>::getId() const {
    assert(node_type != NodeType::UNKNOWN);
    if (node_type == NodeType::LEAF) {
        return leafNode.getId();
    }
    else {
        return  innerNode.getId();
    }
}

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
void Node<Key, Value, ValueStorage>::setId(size_t new_id) {
    assert(node_type != NodeType::UNKNOWN);
    if (node_type == NodeType::LEAF) {
        leafNode.setId(new_id);
    }
    else {
        innerNode.setId(new_id);
    }
}

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
bool Node<Key, Value, ValueStorage>::isLeaf() const {
    assert(node_type != NodeType::UNKNOWN);
    if (node_type == NodeType::LEAF) {
        return leafNode.isLeaf();
    }
    else {
        return innerNode.isLeaf();
    }
}

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
void Node<Key, Value, ValueStorage>::setDeleted() {
    assert(node_type != NodeType::UNKNOWN);
    if (node_type == NodeType::LEAF)
        leafNode.setDeleted();
    else
        innerNode.setDeleted();
}

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
size_t Node<Key, Value, ValueStorage>::find(const Key & key, std::function<bool(const Key &, const Key &)> cmp) const {
    assert(node_type != NodeType::UNKNOWN);
    if (node_type == NodeType::LEAF) {
        return leafNode.find(key, cmp);
    }
    else {
        return innerNode.find(key, cmp);
    }
}

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
std::tuple<Node<Key, Value, ValueStorage>,
           Node<Key, Value, ValueStorage>>
Node<Key, Value, ValueStorage>::split(size_t left_id, size_t right_id) const {
    assert(node_type != NodeType::UNKNOWN);
    if (node_type == NodeType::LEAF) {
        std::tuple<leaf_node_t, leaf_node_t> ret = leafNode.split(left_id, right_id);
        return std::make_tuple(Node(std::get<0>(ret)), Node(std::get<1>(ret)));
    }
    else {
        auto ret = innerNode.split(left_id, right_id);
        return std::make_tuple(Node(std::get<0>(ret)), Node(std::get<1>(ret)));
    }
}

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
void Node<Key, Value, ValueStorage>::join(const Node & other) {
    assert(node_type != NodeType::UNKNOWN);
    if (node_type == NodeType::LEAF) {
        leafNode.join(other.leafNode);
    }
    else {
        innerNode.join(other.innerNode);
    }
}

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
const Key & Node<Key, Value, ValueStorage>::getMax() const {
    assert(node_type != NodeType::UNKNOWN);
    if (node_type == NodeType::LEAF) {
        return leafNode.getMax();
    }
    else {
        return innerNode.getMax();
    }

}


template <
        typename Key,
        typename Value,
        typename ValueStorage
>
const Key & Node<Key, Value, ValueStorage>::getKey(size_t index) const {
    assert(node_type != NodeType::UNKNOWN);
    if (node_type == NodeType::LEAF) {
        return leafNode.getKey(index);
    }
    else {
        return innerNode.getKey(index);
    }
}

// ===== Binary mode methods =====

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
Node<Key, Value, ValueStorage>::Node(const std::vector<uint8_t> & src) {
    size_t index = 0;
    node_type = deserialize<NodeType>(src, index);
    index += sizeof(node_type);
    if (node_type == node_t::NodeType::INNER)
        innerNode = inner_node_t(src, index);
    if (node_type == node_t::NodeType::LEAF)
        leafNode = leaf_node_t(src, index);
    assert(node_type != NodeType::UNKNOWN);
}


template <
        typename Key,
        typename Value,
        typename ValueStorage
>
std::vector<uint8_t> Node<Key, Value, ValueStorage>::toBinary() const {
    assert(node_type != NodeType::UNKNOWN);
    std::vector<uint8_t> ret;
    serialize(node_type, ret, ret.size());
    if (node_type == NodeType::LEAF)
        leafNode.toBinary(ret, ret.size());
    else
        innerNode.toBinary(ret, ret.size());
    return std::move(ret);
}


// ===== Leaf node methods =====

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
size_t Node<Key, Value, ValueStorage>::getNext() const {
    assert(node_type == NodeType::LEAF);
    return leafNode.getNext();
}

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
size_t Node<Key, Value, ValueStorage>::getPrev() const {
    assert(node_type == NodeType::LEAF);
    return leafNode.getPrev();
}


template <
        typename Key,
        typename Value,
        typename ValueStorage
>
void Node<Key, Value, ValueStorage>::setNext(size_t id) {
    assert(node_type == NodeType::LEAF);
    return leafNode.setNext(id);
}

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
void Node<Key, Value, ValueStorage>::setPrev(size_t id) {
    assert(node_type == NodeType::LEAF);
    return leafNode.setPrev(id);
}

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
Value Node<Key, Value, ValueStorage>::getValue(size_t index) const {
    assert(node_type == NodeType::LEAF);
    return leafNode.getValue(index);
}

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
void Node<Key, Value, ValueStorage>::setValue(const Value & value, size_t index) {
    assert(node_type == NodeType::LEAF);
    leafNode.setValue(value, index);
}

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
void Node<Key, Value, ValueStorage>::addKey(size_t index, size_t val_id, const Key & key, const Value & value) {
    assert(node_type == NodeType::LEAF);
    leafNode.addKey(index, val_id, key, value);
}

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
void Node<Key, Value, ValueStorage>::deleteKey(size_t index) {
    assert(node_type != NodeType::UNKNOWN);
    if (node_type == NodeType::LEAF)
        leafNode.deleteKey(index);
    else
        innerNode.deleteKey(index);
}

// ===== Inner node methods =====


template <
        typename Key,
        typename Value,
        typename ValueStorage
>
const Key & Node<Key, Value, ValueStorage>::updateKey(const Key & key, size_t index) {
    assert(node_type == NodeType::INNER);
    return innerNode.updateKey(key, index);
}

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
const Key & Node<Key, Value, ValueStorage>::addKey(const Key & key, size_t child_id, size_t index) {
    assert(node_type == NodeType::INNER);
    return innerNode.addKey(key, child_id, index);
}

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
size_t Node<Key, Value, ValueStorage>::getChild(size_t index) const {
    assert(node_type == NodeType::INNER);
    return innerNode.getChild(index);
}

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
void Node<Key, Value, ValueStorage>::setChild(size_t index, size_t new_child) {
    assert(node_type == NodeType::INNER);
    return innerNode.setChild(index, new_child);
}

template <
        typename Key,
        typename Value,
        typename ValueStorage
>
void Node<Key, Value, ValueStorage>::setKey(size_t index, const Key & new_key) {
    assert(node_type == NodeType::INNER);
    return innerNode.setKey(index, new_key);
}

#endif //TERM_PROJECT_NODE_H
