#include <iostream>
#include <iomanip>
#include "IOStorage.h"

IOStorage::IOStorage(const std::string & stor_name,
                     const std::string & stck_name,
                     const std::string & tbl_name,
                     size_t blck_size, bool rewrite) :
    storage_file(stor_name),
    stack_file(stck_name),
    table_file(tbl_name),
    block_size(blck_size)
{
    FileArray  name_f(storage_file, block_size, rewrite);
    FileArray stack_f(stack_file, sizeof(size_t), rewrite);
    FileArray table_f(table_file, sizeof(size_t), rewrite);

    if (!rewrite) {
        auto f = Journal(storage_file);
        f = Journal(stack_file);
        f = Journal(table_file);
    }
}

IOStorage::~IOStorage() {
    system(("rm -f " + storage_file + " " + table_file + " " + stack_file + "\n").c_str());
    Journal::deleteJournal(storage_file);
    Journal::deleteJournal(table_file);
    Journal::deleteJournal(stack_file);
}


typename IOStorage::ProxyIt IOStorage::find(size_t id) const {
    FileArray table(table_file, sizeof(size_t));
    if (table.size() <= id)
        return ProxyIt(-1, -1, storage_file, block_size);
    size_t position;
    table.readElement(position, id);
    return ProxyIt(id, position, storage_file, block_size);
}

void IOStorage::insert(std::pair<size_t, std::vector<uint8_t>> pair) {
    FileArray stack(stack_file, sizeof(size_t));
    FileArray table(table_file, sizeof(size_t));
    FileArray storage(storage_file, block_size);

    assert(pair.second.size() <= block_size);
    pair.second.resize(block_size);

    size_t position = storage.size();
    if (stack.size() > 0) {
        stack.back(position);
        stack.popBack();
    }

    storage.writeElement(pair.second[0], position, pair.second.size());
    table.writeElement(position, pair.first);
}

void IOStorage::erase(const IOStorage::ProxyIt & proxy) {
    FileArray stack(stack_file, sizeof(size_t));
    stack.pushBack(proxy.getPosition());
    FileArray table(table_file, sizeof(size_t));
    table.writeElement<size_t>(-1, proxy.getId());
}