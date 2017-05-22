//
// Created by annikura on 13.05.17.
//

#ifndef TERM_PROJECT_BASENODECLASS_H
#define TERM_PROJECT_BASENODECLASS_H


#include <cstdio>
#include <algorithm>
#include <tuple>
#include <functional>
#include <vector>
#include <cassert>

template <typename Key>
class BaseNodeClass {
protected:
    size_t id;
    bool is_deleted = false;
    std::vector<Key> keys;
public:
    BaseNodeClass() : id(-1), is_deleted(false) { };

    virtual size_t size() const {
        assert(!is_deleted);
        return keys.size();
    }
    virtual size_t getId() const {
        return id;
    }
    virtual void setId(size_t new_id) {
        assert(!is_deleted);
        id = new_id;
    }
    virtual bool isLeaf() const = 0;

    virtual const Key & getKey(size_t index) const {
        assert(!is_deleted);
        assert(index < size());
        return keys[index];
    }

    virtual void setDeleted() {
        assert(!is_deleted);
        is_deleted = true;
    }

    virtual size_t find(const Key & key, std::function<bool(const Key &, const Key &)> cmp) const {
        assert(!is_deleted);
        return std::lower_bound(this->keys.begin(), this->keys.end(), key, cmp) - this->keys.begin();
    }

    virtual const Key & getMax() const {
        assert(!is_deleted);
        assert(keys.size() > 0);
        return keys[keys.size() - 1];
    }
};


#endif //TERM_PROJECT_BASENODECLASS_H
