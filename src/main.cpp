#include <cstdlib>
#include <climits>
#include "../key_value_storage.h"

int main() {
    srand(179239);

    size_t init_size = rand() % 100000 + 100;
    size_t number_of_operations = init_size, max_range = 100;
    size_t set_cnt = 0, del_cnt = 0, range_cnt = 0, get_cnt = 0;
    std::vector<int> arr(init_size, INT_MAX);
    KeyValueStorage<int, int> storage("storage");

    for (size_t i = 0; i < init_size; i++) {
        storage.set(i, i);
        arr[i] = i;
    }
    std::cout << "A tree containing " << init_size << " elements has been created" << std::endl;

    for (size_t i = 0; i < number_of_operations; i++) {
        int operation_type = rand() % 4;

        if (operation_type == 0) {                  // set element
            int new_element_key = rand() % init_size, new_element_value = rand();
            storage.set(new_element_key, new_element_value);

            set_cnt++;
            continue;
        }
        if (operation_type == 1) {                  // del element
            int del_key = rand() % init_size;
            if (arr[del_key] == INT_MAX)
                continue;

            storage.del(del_key);
            arr[del_key] = INT_MAX;
            del_cnt++;
            continue;
        }
        if (operation_type == 2) {                   // range query
            int left = rand() % (init_size - max_range), right = left + rand() % max_range;
            std::vector<std::pair<int, int>> result;
            storage.getRange(left, right, result);
            range_cnt++;
            continue;
        }
        if (operation_type == 3) {                     // get query
            int get_key = rand() % init_size;
            if (arr[get_key] == INT_MAX)
                continue;
            storage.get(get_key);
            get_cnt++;
            continue;
        }
    }
    std::cout << get_cnt + range_cnt + del_cnt + set_cnt << " queries have been processed:" << std::endl
              << get_cnt << " get queries;" << std::endl
              << range_cnt << " range queries;" << std::endl
              << del_cnt << " del queries;" << std::endl
              << set_cnt << " set queries;" << std::endl;
}