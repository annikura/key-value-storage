#ifndef TERM_PROJECT_IOBASECLASS_H
#define TERM_PROJECT_IOBASECLASS_H

#include <cstdio>
#include <string>
#include <utility>
#include <ios>
#include <fstream>
#include <vector>
#include "FileArray.h"

class IOStorage {
private:

    const std::string storage_file;
    const std::string table_file;
    const std::string stack_file;

    const size_t block_size;
protected:
    class ProxyIt {
    protected:
        size_t id;
                const std::string & file;
        const size_t & block_sz;
    public:

        size_t first;
        std::vector<uint8_t> second;

        ProxyIt(size_t id, size_t position, const std::string & filename, const size_t & blk_sz) :
                id(id),
                first(position),
                file(filename),
                block_sz(blk_sz) { };

        ProxyIt *operator->() {
            FileArray fs(file, block_sz);
            assert(first != -1);
            second.resize(block_sz);
            fs.readElement(second[0], first);
            return this;
        }

        size_t getPosition() const {
            return first;
        }

        size_t getId() const {
            return id;
        }

        bool operator==(const ProxyIt & other) const {
            return this->first == other.first;
        };

        bool operator!=(const ProxyIt & other) const {
            return !this->operator==(other);
        };
    };

public:
    IOStorage(const std::string & stor_name,
              const std::string & stck_name,
              const std::string & tbl_name,
              size_t blck_size, bool rewrite = true);
    ~IOStorage();
    ProxyIt find(size_t id) const;
    void insert(std::pair<size_t, std::vector<uint8_t>>);
    void erase(const ProxyIt & getter);

    ProxyIt end() { return  ProxyIt(-1, -1, storage_file, block_size); };
};


#endif //TERM_PROJECT_IOBASECLASS_H
