#include <cstdlib>
#include "key_value_storage.h"

int main() {
    srand(179239);

    size_t n = rand() % 100 + 100;
    KeyValueStorage<int, int> storage;

    for (size_t i = 0; i < n; i++)
        storage.set(i, i);
}