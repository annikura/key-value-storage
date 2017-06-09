#ifndef TERM_PROJECT_TEST_KEY_VALUE_STORAGE_H
#define TERM_PROJECT_TEST_KEY_VALUE_STORAGE_H

#include "gtest/gtest.h"
#include "../src/key_value_storage.h"

inline void genIntPairs(size_t n, size_t seed, std::vector<std::pair<int, int>> & arr) {
    srand(seed);
    std::set<int> used;
    for (size_t i = 0; i < n; i++) {
        int key = rand();
        while (used.find(key) != used.end())
            key = rand();
        used.insert(key);
        arr.push_back(std::make_pair(key, rand()));
    }
}



TEST(KeyValueStorage, Brute) {
    KeyValueStorage<int, int> tree;

    size_t n = 1007;
    std::vector<std::pair<int, int>> arr;

    genIntPairs(n, 179, arr);
    for (size_t index = 0; index < n; index++) {
        tree.set(arr[index].first, arr[index].second);
    //    std::cerr << index << "\n";
    }
    /*
    std::sort(arr.begin(), arr.end());
    for (size_t index = 0; index < n / 2; index++) {
        int t = rand() % 2;
        if (t == 1) {
            size_t ind = rand() % arr.size();
            EXPECT_EQ(arr[ind].second, tree.get(arr[ind].first));
        }
        if (t == 0) {
            size_t ind = rand() % arr.size();
            tree.del(arr[ind].first);
            ASSERT_THROW(tree.get(arr[ind].first), std::runtime_error);
            arr.erase(arr.begin() + ind);
        }
    }
     */
}

#endif //TERM_PROJECT_TEST_KEY_VALUE_STORAGE_H
