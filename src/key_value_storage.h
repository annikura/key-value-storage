#ifndef TERM_PROJECT_LIBRARY_H
#define TERM_PROJECT_LIBRARY_H

#include "tree/BTree.h"
#include "io/IOStorage.h"


template <typename Key, typename Value>
class ValueStorage : public IOStorage {
    const static std::string type_prefix;
public:
    ValueStorage() : IOStorage(type_prefix + "^value_storage",
                               type_prefix + "^value_stack",
                               type_prefix + "^value_table",
                               sizeof(Value)) {};
};

template <typename Key, typename Value>
const std::string ValueStorage<Key, Value>::type_prefix = "data/" +
                                                          (std::string)typeid(Key).name() + "^" +
                                                          (std::string)typeid(Value).name();

template <typename Key, typename Value>
class NodeStorage : public IOStorage {
    static const size_t ELEMENTS_IN_NODE = 512;
    static const std::string type_prefix;
public:
    NodeStorage() : IOStorage( type_prefix + "^node_storage",
                               type_prefix + "^node_stack",
                               type_prefix + "^node_table",
                               sizeof(size_t) * (2 + 2 + ELEMENTS_IN_NODE) +  // id + size + next + prev + values * Elements_in_block
                               sizeof(Key) * ELEMENTS_IN_NODE) {};            // keys
};


template <typename Key, typename Value>
const std::string NodeStorage<Key, Value>::type_prefix = "data/" +
                                                         (std::string)typeid(Key).name() + "^" +
                                                         (std::string)typeid(Value).name();

template <typename Key, typename Value, typename Comparator = std::less<Key>>
class KeyValueStorage : public BTree<Key, Value, NodeStorage<Key, Value>, ValueStorage<Key, Value>> {
private:
    static size_t active_trees;
public:
    KeyValueStorage() : BTree<Key, Value, NodeStorage<Key, Value>, ValueStorage<Key, Value>>::BTree(Comparator()) {
        ++active_trees;
    }

    ~KeyValueStorage() {
        --active_trees;
    }
};

template <typename Key, typename Value, typename Comparator>
size_t KeyValueStorage<Key, Value, Comparator>::active_trees = 0;


#endif