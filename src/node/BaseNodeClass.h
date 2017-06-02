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
#include "../utilities.h"

template <typename Key>
class BaseNodeClass {
protected:
    size_t id;
    bool is_deleted = false;
    std::vector<Key> keys;
public:
    BaseNodeClass() : id(-1), is_deleted(false) { };
    BaseNodeClass(const std::vector<uint8_t> & src, size_t & beg) : BaseNodeClass()
    {
        size_t size;
        assert(beg + sizeof(size) <= src.size());
        id = deserialize<size_t>(src, beg);
        beg += sizeof(id);
        size = deserialize<size_t>(src, beg);
        beg += sizeof(size);
        //std::cerr << "BaseClass:" << beg << " " << size << " " << src.size() << "\n";
        assert(beg + size * sizeof(Key) <= src.size());
        for (size_t index = 0; index < size; index++) {
            keys.push_back(deserialize<Key>(src, beg));
            beg += sizeof(Key);
            //std::cerr << keys.back() << " ";
        }
        //std::cerr << "\n";
    }

    virtual size_t size() const {
//        assert(!is_deleted);
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
    virtual size_t toBinary(std::vector<uint8_t> & res, size_t index) const {
        index = serialize(this->id, res, index);
        index = serialize(this->size(), res, index);
        for (size_t i = 0; i < size(); i++)
            index = serialize(keys[i], res, index);
        return index;
    }

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
        return keys.back();
    }
};


#endif //TERM_PROJECT_BASENODECLASS_H
