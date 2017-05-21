//
// Created by annikura on 16.05.17.
//

#ifndef TERM_PROJECT_BTREE_H
#define TERM_PROJECT_BTREE_H

#include <map>
#include <unordered_map>
#include "BaseBTreeClass.h"

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
class BTree : protected BaseBTreeClass<Key, Value, NodeStorage, ValueStorage>{
protected:
    typedef Node<Key, Value, NodeStorage, ValueStorage> BNode;

    const size_t node_size;

    static size_t next_node_id;
    static size_t next_value_id;

    std::unordered_map<size_t, BNode> buffer;
    std::unordered_map<size_t, BNode> changes;

    size_t genNodeId();
    size_t genValueId();

    void saveNode(const BNode & node);
    void delNode(BNode & node);

    BNode getNode(size_t id);
    BNode getNode(size_t index) const; // Doesn't buffer nodes

    const Key & normalizeNodeShortage(BNode & node, size_t index);
    const Key & normalizeNodeOverflow(BNode & node, size_t index);

    Key set(BNode node, const Key & key, const Value & value);
    Key del(BNode node, const Key & key);

    size_t findKey(const BNode & node, const Key & key) const;
    void insertIntoList(const BNode & node);

    void findNode(const Key & key, BNode & node) const;
public:
    BTree(std::function<bool(const Key &, const Key &)> cmp, size_t node_sz = 512);
    const Value get(const Key & key) const override;
    void set(const Key & key, const Value & value) override;
    void del(const Key & key) override ;
    std::vector<std::pair<Key, Value>> & getRange(const Key & left, const Key & right, std::vector<std::pair<Key, Value>> & dest) const override;
};

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
size_t BTree<Key, Value, NodeStorage, ValueStorage>::genNodeId() {
    return this->new_node_id++;
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
size_t BTree<Key, Value, NodeStorage, ValueStorage>::genValueId() {
    return this->new_value_id++;
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
void BTree<Key, Value, NodeStorage, ValueStorage>::saveNode(const BNode & node) {
    changes[node.getId()] = node;
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
void BTree<Key, Value, NodeStorage, ValueStorage>::delNode(BNode & node) {
    node.setDeleted();
    saveNode(node);
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
Node<Key, Value, NodeStorage, ValueStorage> BTree<Key, Value, NodeStorage, ValueStorage>::getNode(size_t id) {
    auto it = changes.find(id);
    if (it != changes.end())
        return it->second;
    it = buffer.find(id);
    if (it != buffer.end())
        return it->second;
    return buffer[id] = BNode(id);
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
Node<Key, Value, NodeStorage, ValueStorage> BTree<Key, Value, NodeStorage, ValueStorage>::getNode(size_t id) const {  // Doesn't buffer nodes
    auto it = changes.find(id);
    if (it != changes.end())
        return it->second;
    it = buffer.find(id);
    if (it != buffer.end())
        return it->second;
    return BNode(id);
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
const Key & BTree<Key, Value, NodeStorage, ValueStorage>::normalizeNodeShortage(BNode & node, size_t index) {
    assert(node.size() > 1);
    if (index == 0)
        index++;
    BNode  left = getNode(node.getChild(index - 1));
    BNode right = getNode(node.getChild(index));
    left.join(right);
    if (left.size() > node_size) {
        std::tuple<BNode, BNode> pair = left.split(left.getId(), right.getId());
        if (left.isLeaf()) {
            insertIntoList(left);
            insertIntoList(right);
        }
        node.updateKey(index - 1, pair[0].getMax());

        saveNode(pair[0]);
        saveNode(pair[1]);
    }
    else {
        node.deleteKey(index - 1);
        node.setChild(index, left.getId());
        right.setDeleted();

        saveNode(left);
        saveNode(right);
    }

    saveNode(node);
    return node.getMax();
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
const Key & BTree<Key, Value, NodeStorage, ValueStorage>::normalizeNodeOverflow(BNode & node, size_t index) {
    BNode child = getNode(node.getChild(index));
    std::tuple<BNode, BNode> pair = child.split(genNodeId(), child.getId());
    node.addKey(pair[0].getMax(), pair[0].getId(), index);

    saveNode(pair[0]);
    saveNode(pair[1]);
    saveNode(node);

    return node.getMax();
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
Key BTree<Key, Value, NodeStorage, ValueStorage>::set(BNode node, const Key & key, const Value & value) {
    if (node.isLeaf()) {
        size_t index = findKey(node, key);
        if (index == -1)
            node.addKey(node.find(key, this->comparator), genValueId(), key, value);
        else
            node.setValue(value, node.find(key, this->comparator));
        saveNode(node);
        return node.getMax();
    }

    size_t index = node.find(key, this->comparator);
    if (index > node.size()) --index;
    Key mx_key = set(BNode(node.getChild(index)), key, value);

    node.updateKey(mx_key, index);
    BNode child = getNode(node.getChild(index));
    if (child.size() == node_size * 2)
        normalizeNodeOverflow(node, index);
    else
        saveNode(node);

    return node.getMax();
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
Key BTree<Key, Value, NodeStorage, ValueStorage>::del(BNode node, const Key & key) {
    if (node.isLeaf()) {
        size_t index = findKey(node, key);
        if (index == -1)
            throw std::runtime_error("del: This key does not exist");
        else
            node.deleteKey(node.find(key, this->comparator));
        saveNode(node);
        return node.getMax();
    }

    size_t index = node.find(key, this->comparator);
    if (index > node.size())
        throw std::runtime_error("del: This key does not exist");
    Key mx_key = del(BNode(node.getChild(index)), key);

    node.updateKey(mx_key, index);
    BNode child = getNode(node.getChild(index));
    if (child.size() + 1 == node_size)
        normalizeNodeShortage(node, index);
    else
        saveNode(node);

    return node.getMax();
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
size_t BTree<Key, Value, NodeStorage, ValueStorage>::findKey(const BNode & node, const Key & key) const {
    size_t index = node.find(key, this->comparator);
    if (index >= node.size())
        return -1;
    Key found_key = node.getKey(index);
    return (this->comparator(found_key, key) == this->comparator(key, found_key)) ? index : -1;
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
void BTree<Key, Value, NodeStorage, ValueStorage>::insertIntoList(const BNode & node) {
    if (node.getNext() != -1) {
        BNode v = getNode(node.getNext());
        v.setPrev(node.getId());
        saveNode(v);
    }
    if (node.getPrev() != -1) {
        BNode v = getNode(node.getPrev());
        v.setNext(node.getId());
        saveNode(v);
    }
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
void BTree<Key, Value, NodeStorage, ValueStorage>::findNode(const Key & key, BNode & node) const {
    if (node.isLeaf())
        return;
    size_t index = node.find(key, this->comparator);
    if (index > node.size())
        throw std::runtime_error("find: This key does not exist");
    node = BNode(node.getChild(index));
    findNode(key, node);
}

// public methods

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
BTree<Key, Value, NodeStorage, ValueStorage>::BTree(std::function<bool(const Key &, const Key &)> cmp, size_t node_sz)
        : node_size(node_sz),
        BTree::BTree(cmp, genNodeId()){ }

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
const Value BTree<Key, Value, NodeStorage, ValueStorage>::get(const Key & key) const {
    BNode node(this->root);
    findNode(key, node);
    size_t index = findKey(node, key);
    if (index == -1)
        throw std::runtime_error("get: This key does not exist");
    else
        return node.getValue(index);
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
void BTree<Key, Value, NodeStorage, ValueStorage>::set(const Key & key, const Value & value) {
    set(this->root, key, value);

    if (this->root.size() == node_size * 2) {
        std::tuple<BNode, BNode> pair = this->root.split(genNodeId(), genNodeId());
        size_t root_id = this->root.getId();
        this->root = BNode(InnerNode<Key>());
        this->root.setId(root_id);

        this->root.addKey(pair[0].getMax(), pair[0].getId(), 0);
        this->root.addKey(pair[1].getMax(), pair[1].getId(), 1);

        saveNode(pair[0]);
        saveNode(pair[1]);
        saveNode(this->root);
    }

    for (BNode & node: changes)
        node.commit();

    buffer.clear();
    changes.clear();
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
void BTree<Key, Value, NodeStorage, ValueStorage>::del(const Key & key) {
    del(this->root, key);

    if (this->root.size() == 1) {
        size_t child = this->root.getChild(0);

        this->root.setDeleted();
        saveNode(this->root);

        this->root = getNode(child);
    }

    for (BNode & node: changes)
        node.commit();

    buffer.clear();
    changes.clear();

}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
std::vector<std::pair<Key, Value>> & BTree<Key, Value, NodeStorage, ValueStorage>::getRange(const Key & left, const Key & right, std::vector<std::pair<Key, Value>> & dest) const {
    BNode node = this->root;
    findNode(node, left);
    while (1) {
        size_t l = node.find(left, this->comparator);
        size_t r = node.find(right, this->comparator);
        if (l == r)
            break;
        for (size_t index = l; index < r; ++index)
            dest.push_back(std::make_pair(node.getKey(index), node.getValue(index)));
        if (node.getNext() == -1)
            break;
    }
    return dest;
}

#endif //TERM_PROJECT_BTREE_H
