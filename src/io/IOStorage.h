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
            assert(first != -1);
            FileArray fs(file, block_sz);
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
    };

public:

    IOStorage(std::string stor_name, std::string stck_name, std::string tbl_name, size_t blck_size) :
            storage_file(stor_name),
            stack_file(stck_name),
            table_file(tbl_name),
            block_size(blck_size)
    {
        FileArray  name_f(storage_file, block_size, true);
        FileArray stack_f(stack_file, sizeof(size_t), true);
        FileArray table_f(table_file, sizeof(size_t), true);
    };

    ProxyIt find(size_t id);
    void insert(std::pair<size_t, std::vector<uint8_t>>);
    void erase(const ProxyIt & getter);

    ProxyIt end() { return  ProxyIt(-1, -1, storage_file, block_size); };
};


#endif //TERM_PROJECT_IOBASECLASS_H
