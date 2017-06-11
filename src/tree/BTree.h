#ifndef TERM_PROJECT_BTREE_H
#define TERM_PROJECT_BTREE_H

#include <map>
#include <unordered_map>
#include <set>
#include "BaseBTreeClass.h"

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
class BTree : protected BaseBTreeClass<Key, Value, NodeStorage, ValueStorage>{
protected:
    typedef Node<Key, Value, ValueStorage> BNode;

    const size_t node_size;

    //static size_t next_node_id;
    size_t next_node_id = 1;
    static size_t next_value_id;

    std::unordered_map<size_t, BNode> buffer;
    std::unordered_map<size_t, BNode> changes;

    NodeStorage node_storage;
    ValueStorage value_storage;

    size_t genNodeId();
    size_t genValueId();

    void saveNode(const BNode & node);
    void delNode(BNode & node);
    void commit(const BNode & node);

    BNode getNode(size_t id);
    BNode getNode(size_t index) const; // Doesn't buffer nodes

    void normalizeNodeShortage(BNode & node, size_t index);
    void normalizeNodeOverflow(BNode & node, size_t index);

    Key set(BNode node, const Key & key, const Value & value);
    Key del(BNode node, const Key & key);

    size_t findKey(const BNode & node, const Key & key) const;
    void insertIntoList(const BNode & node);

    void findNode(const Key & key, BNode & node) const;
public:
    void print(BNode node = InnerNode<Key>()) const;

    BTree(std::function<bool(const Key &, const Key &)> cmp, size_t node_sz = 256);
    const Value get(const Key & key) const override;
    void set(const Key & key, const Value & value) override;
    void del(const Key & key) override ;
    std::vector<std::pair<Key, Value>> & getRange(const Key & left, const Key & right, std::vector<std::pair<Key, Value>> & dest) const override;
};

//template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
//NodeStorage BTree<Key, Value, NodeStorage, ValueStorage>::storage;

//template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
//size_t BTree<Key, Value, NodeStorage, ValueStorage>::next_node_id = 0;

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
size_t BTree<Key, Value, NodeStorage, ValueStorage>::next_value_id = 0;

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
size_t BTree<Key, Value, NodeStorage, ValueStorage>::genNodeId() {
    return next_node_id++;
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
void BTree<Key, Value, NodeStorage, ValueStorage>::print(BNode node) const {
    if (node.getId() == -1) {
        node = this->root;
        std::cerr << "------------------------\n";
    }
    std::cerr << "=======\n";
    std::cerr << "Id: " << node.getId() << " Size: " << node.size() << "\n";
    if (node.isLeaf()) {
        std::cerr << "Prev: " << node.getPrev() << " Next: " << node.getNext() << "\n";
        std::cerr << "Key    Value\n";
        for (size_t index = 0; index < node.size(); index++)
            std::cerr << node.getKey(index) <<  " " << node.getValue(index, value_storage) << std::endl;
    }
    else {
        std::cerr << "Key    Child\n";
        for (size_t index = 0; index < node.size(); index++)
            std::cerr << node.getKey(index) <<  " " << node.getChild(index) << std::endl;
    }
    std::cerr << "=======\n";
    if (!node.isLeaf()) {
        for (size_t index = 0; index < node.size(); index++)
            print(getNode(node.getChild(index)));
    }
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
size_t BTree<Key, Value, NodeStorage, ValueStorage>::genValueId() {
    return this->next_value_id++;
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
void BTree<Key, Value, NodeStorage, ValueStorage>::saveNode(const BNode & node) {
    assert(node.size() <= node_size * 2);
    changes[node.getId()] = node;
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
void BTree<Key, Value, NodeStorage, ValueStorage>::delNode(BNode & node) {
    node.setDeleted();
    saveNode(node);
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
void BTree<Key, Value, NodeStorage, ValueStorage>::commit(const BNode & node) {
    auto index = this->node_storage.find(node.getId());
    if (index != this->node_storage.end())
        this->node_storage.erase(index);
    this->node_storage.insert(std::make_pair(node.getId(), node.toBinary()));
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
Node<Key, Value, ValueStorage> BTree<Key, Value, NodeStorage, ValueStorage>::getNode(size_t id) {
    auto it = changes.find(id);
    if (it != changes.end())
        return it->second;
    it = buffer.find(id);
    if (it != buffer.end())
        return it->second;
    if (this->node_storage.find(id) == node_storage.end())
        std::cerr << id << " =(\n";
    return buffer[id] = BNode(this->node_storage.find(id)->second);
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
Node<Key, Value, ValueStorage> BTree<Key, Value, NodeStorage, ValueStorage>::getNode(size_t id) const {  // Doesn't buffer nodes
    auto it = changes.find(id);
    if (it != changes.end())
        return it->second;
    it = buffer.find(id);
    if (it != buffer.end())
        return it->second;
    return BNode(this->node_storage.find(id)->second);
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
void BTree<Key, Value, NodeStorage, ValueStorage>::normalizeNodeShortage(BNode & node, size_t index) {
    assert(node.size() > 1);
    if (index == 0)
        index++;
    BNode  left = getNode(node.getChild(index - 1));
    BNode right = getNode(node.getChild(index));
    left.join(right);
    if (left.size() >= node_size * 2) {
        std::tuple<BNode, BNode> pair = left.split(left.getId(), right.getId());
        saveNode(std::get<0>(pair));
        saveNode(std::get<1>(pair));

        if (left.isLeaf()) {
            insertIntoList(std::get<0>(pair));
            insertIntoList(std::get<1>(pair));
        }
        node.updateKey(std::get<0>(pair).getMax(), index - 1);
    }
    else {
        saveNode(left);
        saveNode(right);

        if (left.isLeaf())
            insertIntoList(left);
        node.setChild(index, left.getId());
        node.deleteKey(index - 1);
        delNode(right);
    }

    saveNode(node);
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
void  BTree<Key, Value, NodeStorage, ValueStorage>::normalizeNodeOverflow(BNode & node, size_t index) {
    BNode child = getNode(node.getChild(index));
    std::tuple<BNode, BNode> pair = child.split(genNodeId(), child.getId());
    node.addKey(std::get<0>(pair).getMax(), std::get<0>(pair).getId(), index);

    saveNode(std::get<0>(pair));
    saveNode(std::get<1>(pair));

    if (child.isLeaf()) {
        insertIntoList(std::get<0>(pair));
        insertIntoList(std::get<1>(pair));
    }

    saveNode(node);
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
Key BTree<Key, Value, NodeStorage, ValueStorage>::set(BNode node, const Key & key, const Value & value) {
    if (node.isLeaf()) {
        size_t index = findKey(node, key);
        if (index == -1) {
            node.addKey(node.find(key, this->comparator), genValueId(), key, value, value_storage);
        }
        else {
            node.setValue(value, node.find(key, this->comparator), value_storage);
        }
        saveNode(node);
        return node.getMax();
    }
    size_t index = node.find(key, this->comparator);
    if (index >= node.size()) --index;
    Key mx_key = set(getNode(node.getChild(index)), key, value);
    node.updateKey(std::move(mx_key), index);
    BNode child = getNode(node.getChild(index));
    if (child.size() == node_size * 2) {
        normalizeNodeOverflow(node, index);
    }
    else {
        saveNode(node);
    }

    return node.getMax();
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
Key BTree<Key, Value, NodeStorage, ValueStorage>::del(BNode node, const Key & key) {
    if (node.isLeaf()) {
        size_t index = findKey(node, key);
        if (index == -1)
            throw std::runtime_error("del: This key does not exist");
        else
            node.deleteKey(node.find(key, this->comparator), value_storage);
        saveNode(node);
        return node.size() > 0 ? node.getMax() : key;
    }

    size_t index = node.find(key, this->comparator);
    if (index >= node.size())
        throw std::runtime_error("del: This key does not exist");
    Key mx_key = del(getNode(node.getChild(index)), key);

    node.updateKey(mx_key, index);
    BNode child = getNode(node.getChild(index));
    if (child.size() + 1 == node_size)
        normalizeNodeShortage(node, index);
    else
        saveNode(node);

    return node.size() > 0 ? node.getMax() : mx_key;
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
    if (index >= node.size())
        throw std::runtime_error("find: This key does not exist");
    node = getNode(node.getChild(index));
    findNode(key, node);
}

// public methods

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
BTree<Key, Value, NodeStorage, ValueStorage>::BTree(std::function<bool(const Key &, const Key &)> cmp, size_t node_sz)
        : node_size(node_sz),
        BTree::BaseBTreeClass(cmp, 0){ commit(this->root); }

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
const Value BTree<Key, Value, NodeStorage, ValueStorage>::get(const Key & key) const {
    BNode node(this->root);
    findNode(key, node);
    size_t index = findKey(node, key);
    if (index == -1)
        throw std::runtime_error("get: This key does not exist");
    else
        return node.getValue(index, value_storage);
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
void BTree<Key, Value, NodeStorage, ValueStorage>::set(const Key & key, const Value & value) {
    set(this->root, key, value);
    this->root = getNode(this->root.getId());

    if (this->root.size() == node_size * 2) {
        std::tuple<BNode, BNode> pair = this->root.split(genNodeId(), genNodeId());

        size_t root_id = this->root.getId();
        this->root = BNode(InnerNode<Key>());

        this->root.setId(root_id);

        this->root.addKey(std::get<0>(pair).getMax(), std::get<0>(pair).getId(), 0);
        this->root.addKey(std::get<1>(pair).getMax(), std::get<1>(pair).getId(), 1);

        saveNode(std::get<0>(pair));
        saveNode(std::get<1>(pair));
        saveNode(this->root);
    }
    std::set<size_t> tmp;
    for (auto & node: changes) {
        tmp.insert(node.first);
    }
    for (auto & el: tmp) {
        commit(changes[el]);
    }

    changes.clear();
    buffer.clear();
}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
void BTree<Key, Value, NodeStorage, ValueStorage>::del(const Key & key) {
    del(this->root, key);
    this->root = getNode(this->root.getId());

    if (!this->root.isLeaf() && this->root.size() == 1) {
        size_t root_id = this->root.getId();
        BNode child = getNode(this->root.getChild(0));

        this->root = child;
        this->root.setId(root_id);

        delNode(child);
        saveNode(child);
    }

    std::set<size_t> tmp;
    for (auto & node: changes)
        tmp.insert(node.first);
    for (auto & el: tmp)
        commit(changes[el]);

    buffer.clear();
    changes.clear();

}

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
std::vector<std::pair<Key, Value>> & BTree<Key, Value, NodeStorage, ValueStorage>::getRange(const Key & left, const Key & right, std::vector<std::pair<Key, Value>> & dest) const {
    BNode node = this->root;
    findNode(left, node);
    while (1) {
        size_t l = node.find(left, this->comparator);
        size_t r = node.find(right, this->comparator);
        if (l == r)
            break;
        for (size_t index = l; index < r; ++index)
            dest.push_back(std::make_pair(node.getKey(index), node.getValue(index, value_storage)));
        if (node.getNext() == -1 || r < node.size())
            break;
        node = getNode(node.getNext());
    }
    return dest;
}

#endif //TERM_PROJECT_BTREE_H
