//
// Created by annikura on 01.06.17.
//

#ifndef TERM_PROJECT_IOBASECLASS_H
#define TERM_PROJECT_IOBASECLASS_H

#include <cstdio>
#include <string>
#include <utility>
#include <ios>
#include <fstream>
#include <vector>

class IOStorage {
private:
    const std::string storage_file;
    const std::string table_file;
    const std::string stack_file;

    const size_t block_size;
protected:
    class Getter {
    protected:
        size_t first;
        std::vector<uint8_t> second;

        const std::string & file;
        const size_t & block_sz;
    public:
        Getter(size_t position, const std::string & filename, const size_t & blk_sz) :
                first(position),
                file(filename),
                block_sz(blk_sz) { };

        Getter *operator->() {
            assert(first != -1);
            File fs(file);
            second.resize(block_sz);
            fs.read(second[0], block_sz);
            return this;
        }

        bool operator==(const Getter & other) {
            return this->first == other.first;
        };
    };

public:

    IOStorage(std::string str_name, std::string stck_name, std::string tbl_name, size_t blck_size) :
            storage_file(str_name),
            stack_file(stck_name),
            table_file(tbl_name),
            block_size(blck_size)
    {
        File  name_f(storage_file);
        File stack_f(stack_file);
        File table_f(table_file);

        name_f.write<size_t>(0);
        table_f.write<size_t>(0);
        stack_f.write<size_t>(0);
    };

    Getter find(size_t id);
    Getter insert(std::pair<size_t, std::vector<uint8_t>>);
    Getter erase(const Getter & getter);

    Getter end() { return  Getter(-1, storage_file, block_size); };
};

typename IOStorage::Getter IOStorage::find(size_t id) {
    File table(table_file);
    size_t count;
    table.read(count, 0);
    if (count <= id)
        return Getter(-1, storage_file, block_size);
    size_t position;
    table.read(position, sizeof(size_t) * (id + 1));
    return Getter(position, storage_file, block_size);
}

typename IOStorage::Getter IOStorage::insert(std::pair<size_t, std::vector<uint8_t>> pair) {

}

typename IOStorage::Getter
IOStorage::erase(const IOStorage::Getter & getter) {

}

#endif //TERM_PROJECT_IOBASECLASS_H
