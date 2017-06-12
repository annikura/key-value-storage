#ifndef TERM_PROJECT_LIBRARY_H
#define TERM_PROJECT_LIBRARY_H

#include "src/tree/BTree.h"
#include "src/io/IOStorage.h"

template <typename Key, typename Value>
class ValueStorage : public IOStorage {
    const static std::string type_prefix;
public:
    ValueStorage(const std::string & name, bool rewrite = true) :
            IOStorage(type_prefix + "^value_storage" + "~" + name,
                      type_prefix + "^value_stack" + "~" + name,
                      type_prefix + "^value_table"+ "~" + name,
                      sizeof(Value), rewrite) {};
};

template <typename Key, typename Value>
const std::string ValueStorage<Key, Value>::type_prefix = "key_value_storage_data/" +
                                                          (std::string)typeid(Key).name() + "^" +
                                                          (std::string)typeid(Value).name();

template <typename Key, typename Value>
class NodeStorage : public IOStorage {
    static const size_t ELEMENTS_IN_NODE = 512;
    static const std::string type_prefix;
public:
    NodeStorage(const std::string & name, bool rewrite = true) :
            IOStorage( type_prefix + "^node_storage" + "~" + name,
                       type_prefix + "^node_stack" + "~" + name,
                       type_prefix + "^node_table" + "~" + name,
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
    KeyValueStorage(const std::string & name, bool restore = false) :
            BTree<Key, Value, NodeStorage<Key, Value>, ValueStorage<Key, Value>>::BTree(Comparator(), name, !restore) {

    }
};

template <typename Key, typename Value, typename Comparator>
size_t KeyValueStorage<Key, Value, Comparator>::active_trees = 0;


#endif