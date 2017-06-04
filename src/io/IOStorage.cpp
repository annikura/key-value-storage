#include <cassert>
#include <iostream>
#include "IOStorage.h"

typename IOStorage::ProxyIt IOStorage::find(size_t id) {
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
    storage.writeElement(pair.second[0], position);
    table.writeElement(position, pair.first);
}

void IOStorage::erase(const IOStorage::ProxyIt & proxy) {
    FileArray stack(stack_file, sizeof(size_t));
    stack.pushBack(proxy.getPosition());
    FileArray table(table_file, sizeof(size_t));
    table.writeElement<size_t>(-1, proxy.getId());
}
