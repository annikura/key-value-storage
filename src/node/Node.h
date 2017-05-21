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
        typename NodeStorage,
        typename ValueStorage
>
class Node{
protected:
    typedef Node<Key, Value, NodeStorage, ValueStorage> node_t;
    typedef InnerNode<Key> inner_node_t;
    typedef LeafNode<Key, Value, ValueStorage> leaf_node_t;

    union {
        inner_node_t innerNode;
        leaf_node_t leafNode;
    };

    enum class NodeType {UNKNOWN, INNER, LEAF} node_type = NodeType::UNKNOWN;

    static NodeStorage storage;
public:
// ===== Base methods =====

    Node(size_t id);

    Node(const leaf_node_t & other);
    Node(leaf_node_t && other);
    Node(const inner_node_t & other);
    Node(inner_node_t && other);

    size_t size() const;
    size_t getId() const;
    void setId(size_t new_id);
    bool isLeaf() const;

    void commit();
    void setDeleted();

    size_t find(const Key & key, std::function<bool(const Key &, const Key &)>) const;

    std::tuple<node_t, node_t> split(size_t left_is, size_t right_id) const;
    void join(const Node & other);
    const Key & getMax() const;
    const Key & getKey(size_t index) const;
    const Key & deleteKey(size_t index);

// ===== Leaf node methods =====

    size_t getNext() const;
    size_t getPrev() const;

    void setNext(size_t id);
    void setPrev(size_t id);

    Value getValue(size_t index) const;

    void setValue(const Value & value, size_t index);

    Key & addKey(size_t index, size_t val_id, const Key & key, const Value & value); // both are returning new max key in the node


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
        typename NodeStorage,
        typename ValueStorage
>
Node<Key, Value, NodeStorage, ValueStorage>::Node(size_t id) {
    *this = storage.find(id)->second;
}

template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
Node<Key, Value, NodeStorage, ValueStorage>::Node(const leaf_node_t & other) :
        node_type(NodeType::LEAF),
        leafNode(other) { }

template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
Node<Key, Value, NodeStorage, ValueStorage>::Node(leaf_node_t && other) :
        node_type(NodeType::LEAF),
        leafNode(std::move(other)) { } // odd move?

template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
Node<Key, Value, NodeStorage, ValueStorage>::Node(const inner_node_t & other) :
        node_type(NodeType::INNER),
        innerNode(other) { }

template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
Node<Key, Value, NodeStorage, ValueStorage>::Node(inner_node_t && other) :
        node_type(NodeType::INNER),
        innerNode(other) { }

template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
size_t Node<Key, Value, NodeStorage, ValueStorage>::size() const {
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
        typename NodeStorage,
        typename ValueStorage
>
size_t Node<Key, Value, NodeStorage, ValueStorage>::getId() const {
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
        typename NodeStorage,
        typename ValueStorage
>
void Node<Key, Value, NodeStorage, ValueStorage>::setId(size_t new_id) {
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
        typename NodeStorage,
        typename ValueStorage
>
bool Node<Key, Value, NodeStorage, ValueStorage>::isLeaf() const {
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
        typename NodeStorage,
        typename ValueStorage
>
void Node<Key, Value, NodeStorage, ValueStorage>::commit() {
    assert(node_type != NodeType::UNKNOWN);
    storage.insert_or_assign(*this);
}

template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
void Node<Key, Value, NodeStorage, ValueStorage>::setDeleted() {
    assert(node_type != NodeType::UNKNOWN);
    if (node_type == NodeType::LEAF)
        leafNode.setDeleted();
    else
        innerNode.setDeleted();
}

template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
size_t Node<Key, Value, NodeStorage, ValueStorage>::find(const Key & key, std::function<bool(const Key &, const Key &)> cmp) const {
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
        typename NodeStorage,
        typename ValueStorage
>
std::tuple<Node<Key, Value, NodeStorage, ValueStorage>,
           Node<Key, Value, NodeStorage, ValueStorage>>
Node<Key, Value, NodeStorage, ValueStorage>::split(size_t left_id, size_t right_id) const {
    assert(node_type != NodeType::UNKNOWN);
    if (node_type == NodeType::LEAF) {
        auto ret = leafNode.split(left_id, right_id);
        return {Node(ret[0]), Node(ret[1])};
    }
    else {
        auto ret = innerNode.split(left_id, right_id);
        return {Node(ret[0]), Node(ret[1])};

    }
}

template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
void Node<Key, Value, NodeStorage, ValueStorage>::join(const Node & other) {
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
        typename NodeStorage,
        typename ValueStorage
>
const Key & Node<Key, Value, NodeStorage, ValueStorage>::getMax() const {
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
        typename NodeStorage,
        typename ValueStorage
>
const Key & Node<Key, Value, NodeStorage, ValueStorage>::getKey(size_t index) const {
    assert(node_type != NodeType::UNKNOWN);
    if (node_type == NodeType::LEAF) {
        return leafNode.getKey(index);
    }
    else {
        return innerNode.getKey(index);
    }
}

// ===== Leaf node methods =====

template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
size_t Node<Key, Value, NodeStorage, ValueStorage>::getNext() const {
    assert(node_type == NodeType::LEAF);
    return leafNode.getNext();
}

template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
size_t Node<Key, Value, NodeStorage, ValueStorage>::getPrev() const {
    assert(node_type == NodeType::LEAF);
    return leafNode.getPrev();
}


template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
void Node<Key, Value, NodeStorage, ValueStorage>::setNext(size_t id) {
    assert(node_type == NodeType::LEAF);
    return leafNode.setNext(id);
}

template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
void Node<Key, Value, NodeStorage, ValueStorage>::setPrev(size_t id) {
    assert(node_type == NodeType::LEAF);
    return leafNode.setPrev(id);
}

template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
Value Node<Key, Value, NodeStorage, ValueStorage>::getValue(size_t index) const {
    assert(node_type == NodeType::LEAF);
    return leafNode.getValue(index);
}

template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
void Node<Key, Value, NodeStorage, ValueStorage>::setValue(const Value & value, size_t index) {
    assert(node_type == NodeType::LEAF);
    leafNode.setValue(value, index);
}

template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
Key & Node<Key, Value, NodeStorage, ValueStorage>::addKey(size_t index, size_t val_id, const Key & key, const Value & value) {
    assert(node_type == NodeType::LEAF);
    return leafNode.addKey(index, val_id, key, value);
}

template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
const Key & Node<Key, Value, NodeStorage, ValueStorage>::deleteKey(size_t index) {
    assert(node_type != NodeType::UNKNOWN);
    if (node_type == NodeType::LEAF)
        return leafNode.deleteKey(index);
    else
        return innerNode.deleteKey(index);
}

// ===== Inner node methods =====


template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
const Key & Node<Key, Value, NodeStorage, ValueStorage>::updateKey(const Key & key, size_t index) {
    assert(node_type == NodeType::INNER);
    return innerNode.updateKey(key, index);
}

template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
const Key & Node<Key, Value, NodeStorage, ValueStorage>::addKey(const Key & key, size_t child_id, size_t index) {
    assert(node_type == NodeType::INNER);
    return innerNode.addKey(key, child_id, index);
}

template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
size_t Node<Key, Value, NodeStorage, ValueStorage>::getChild(size_t index) const {
    assert(node_type == NodeType::INNER);
    return innerNode.getChild(index);
}

template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
void Node<Key, Value, NodeStorage, ValueStorage>::setChild(size_t index, size_t new_child) {
    assert(node_type == NodeType::INNER);
    return innerNode.setChild(index, new_child);
}

template <
        typename Key,
        typename Value,
        typename NodeStorage,
        typename ValueStorage
>
void Node<Key, Value, NodeStorage, ValueStorage>::setKey(size_t index, const Key & new_key) {
    assert(node_type == NodeType::INNER);
    return innerNode.setKey(index, new_key);
}

#endif //TERM_PROJECT_NODE_H
