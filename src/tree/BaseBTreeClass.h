//
// Created by annikura on 16.05.17.
//

#ifndef TERM_PROJECT_BASEBTREECLASS_H
#define TERM_PROJECT_BASEBTREECLASS_H

#include "../node/InnerNode.h"
#include "../node/LeafNode.h"
#include "../node/Node.h"

template <typename Key, typename Value, typename NodeStorage, typename ValueStorage>
class BaseBTreeClass {
protected:
    typedef Node<Key, Value, NodeStorage, ValueStorage> BNode;

    BNode root;
    const std::function<bool(const Key &, const Key &)> comparator;

    BaseBTreeClass(std::function<bool(const Key &, const Key &)> cmp, size_t root_id) :
            comparator(cmp),
            root(LeafNode<Key, Value, ValueStorage>()) {
        root.setId(root_id);
        root.commit();
    }

public:
    virtual Value get(const Key & key) const = 0;
    virtual void set(const Key & key, const Value & value) = 0;
    virtual void del(const Key & key) = 0;
    virtual std::vector<std::pair<Key, Value>> & getRange(const Key & left, const Key & right, std::vector<std::pair<Key, Value>> & dest) const = 0;
};


#endif //TERM_PROJECT_BASEBTREECLASS_H
