//
// Created by annikura on 21.05.17.
//

#ifndef TERM_PROJECT_TEST_BTREE_H
#define TERM_PROJECT_TEST_BTREE_H

#include "../src/tree/BTree.h"

void genIntPairs(size_t n, size_t seed, std::vector<std::pair<int, int>> & arr) {
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

TEST(BTreePositive, SimpleAdd1) {
    typedef std::string key_t;
    typedef std::string value_t;
    typedef std::map<size_t, std::vector<uint8_t>> v_storage;
    typedef BTree<key_t, value_t, v_storage, v_storage> tree_t;

    key_t key = "foo";
    value_t value = "x";

    tree_t tree((std::less<key_t>()));
    tree.set(key, value);
    EXPECT_EQ(value, tree.get(key));
}

TEST(BTreePositive, SimpleAdd2) {
    typedef std::string key_t;
    typedef std::string value_t;
    typedef std::map<size_t, std::vector<uint8_t>> v_storage;
    typedef BTree<key_t, value_t, v_storage, v_storage> tree_t;

    key_t key1 = "foo", key2 = "temp";
    value_t value1 = "x", value2 = "res";

    tree_t tree((std::less<key_t>()));
    tree.set(key1, value1);
    tree.set(key2, value2);
    EXPECT_EQ(value2, tree.get(key2));
    EXPECT_EQ(value1, tree.get(key1));
}

TEST(BTreePositive, BigSmallAdd) {
    typedef int key_t;
    typedef int value_t;
    typedef std::map<size_t, std::vector<uint8_t>> v_storage;
    typedef BTree<key_t, value_t, v_storage, v_storage> tree_t;

    tree_t tree((std::less<key_t>()));

    size_t n = 100;
    std::vector<std::pair<int, int>> arr;
    genIntPairs(n, 179239, arr);
    for (size_t index = 0; index < n; index++)
        tree.set(arr[index].first, arr[index].second);
    std::sort(arr.begin(), arr.end());
    for (size_t index = 0; index < n; index++)
        EXPECT_EQ(arr[index].second, tree.get(arr[index].first));
}



TEST(BTreePositive, BranchedAdd) {
    typedef int key_t;
    typedef int value_t;
    typedef std::map<size_t, std::vector<uint8_t>> v_storage;
    typedef BTree<key_t, value_t, v_storage, v_storage> tree_t;

    tree_t tree((std::less<key_t>()), 2);

    size_t n = 4;
    std::vector<std::pair<int, int>> arr;
    genIntPairs(n, 179239, arr);

    for (size_t index = 0; index < n; index++) {
        tree.set(arr[index].first, arr[index].second);
    }
    std::sort(arr.begin(), arr.end());
    for (size_t index = 0; index < n; index++)
        EXPECT_EQ(arr[index].second, tree.get(arr[index].first));
}


TEST(BTreePositive, BigAdd) {
    typedef int key_t;
    typedef int value_t;
    typedef std::map<size_t, std::vector<uint8_t>> v_storage;
    typedef BTree<key_t, value_t, v_storage, v_storage> tree_t;

    tree_t tree((std::less<key_t>()));

    size_t n = 10000;
    std::vector<std::pair<int, int>> arr;
    genIntPairs(n, 179239, arr);
    for (size_t index = 0; index < n; index++)
        tree.set(arr[index].first, arr[index].second);
    std::sort(arr.begin(), arr.end());
    for (size_t index = 0; index < n; index++)
        EXPECT_EQ(arr[index].second, tree.get(arr[index].first));
}

TEST(BTreePositive, SimpleDel) {
    typedef int key_t;
    typedef int value_t;
    typedef std::map<size_t, std::vector<uint8_t>> v_storage;
    typedef BTree<key_t, value_t, v_storage, v_storage> tree_t;

    tree_t tree((std::less<key_t>()));

    size_t n = 1;
    std::vector<std::pair<int, int>> arr;
    genIntPairs(n, 179, arr);
    for (size_t index = 0; index < n; index++)
        tree.set(arr[index].first, arr[index].second);
    std::sort(arr.begin(), arr.end());
    for (size_t index = 0; index < n; index++) {
        tree.del(arr[index].first);
        ASSERT_THROW(tree.get(arr[index].first), std::runtime_error);
    }
}


TEST(BTreePositive, SmallDel) {
    typedef int key_t;
    typedef int value_t;
    typedef std::map<size_t, std::vector<uint8_t>> v_storage;
    typedef BTree<key_t, value_t, v_storage, v_storage> tree_t;

    tree_t tree((std::less<key_t>()));

    size_t n = 5;
    std::vector<std::pair<int, int>> arr;
    genIntPairs(n, 179239, arr);
    for (size_t index = 0; index < n; index++)
        tree.set(arr[index].first, arr[index].second);
    std::sort(arr.begin(), arr.end());
    for (size_t index = 0; index < n; index++) {
        tree.del(arr[index].first);
        ASSERT_THROW(tree.get(arr[index].first), std::runtime_error);
    }
}

TEST(BTreePositive, BranchedDel) {
    typedef int key_t;
    typedef int value_t;
    typedef std::map<size_t, std::vector<uint8_t>> v_storage;
    typedef BTree<key_t, value_t, v_storage, v_storage> tree_t;

    tree_t tree((std::less<key_t>()), 2);

    size_t n = 10000;
    std::vector<std::pair<int, int>> arr;
    genIntPairs(n, 179239, arr);
    for (size_t index = 0; index < n; index++)
        tree.set(arr[index].first, arr[index].second);
    std::sort(arr.begin(), arr.end());
    for (size_t index = 0; index < n; index++) {
        tree.del(arr[index].first);
        ASSERT_THROW(tree.get(arr[index].first), std::runtime_error);
    }
}

TEST(BTreePositive, BigDelInSortOrder) {
    typedef int key_t;
    typedef int value_t;
    typedef std::map<size_t, std::vector<uint8_t>> v_storage;
    typedef BTree<key_t, value_t, v_storage, v_storage> tree_t;

    tree_t tree((std::less<key_t>()));

    size_t n = 10000;
    std::vector<std::pair<int, int>> arr;
    genIntPairs(n, 179239, arr);
    for (size_t index = 0; index < n; index++)
        tree.set(arr[index].first, arr[index].second);
    std::sort(arr.begin(), arr.end());
    for (size_t index = 0; index < n; index++) {
        tree.del(arr[index].first);
        ASSERT_THROW(tree.get(arr[index].first), std::runtime_error);
    }
}

TEST(BTreePositive, BigDelInReverseOrder) {
    typedef int key_t;
    typedef int value_t;
    typedef std::map<size_t, std::vector<uint8_t>> v_storage;
    typedef BTree<key_t, value_t, v_storage, v_storage> tree_t;

    tree_t tree((std::less<key_t>()));

    size_t n = 10000;
    std::vector<std::pair<int, int>> arr;
    genIntPairs(n, 239179, arr);
    for (size_t index = 0; index < n; index++)
        tree.set(arr[index].first, arr[index].second);
    std::sort(arr.begin(), arr.end());
    std::reverse(arr.begin(), arr.end());
    for (size_t index = 0; index < n; index++) {
        tree.del(arr[index].first);
        ASSERT_THROW(tree.get(arr[index].first), std::runtime_error);
    }
}

TEST(BTreePositive, BigDelShuffled) {
    typedef int key_t;
    typedef int value_t;
    typedef std::map<size_t, std::vector<uint8_t>> v_storage;
    typedef BTree<key_t, value_t, v_storage, v_storage> tree_t;

    tree_t tree((std::less<key_t>()), 2);

    size_t n = 1000;
    std::vector<std::pair<int, int>> arr;
    genIntPairs(n, 239179, arr);
    for (size_t index = 0; index < n; index++)
        tree.set(arr[index].first, arr[index].second);
    for (size_t index = 0; index < n / 2; index++) {
        tree.del(arr[index].first);
        ASSERT_THROW(tree.get(arr[index].first), std::runtime_error);
    }
    for (size_t index = n / 2; index < n ; index++) {
        EXPECT_EQ(arr[index].second, tree.get(arr[index].first));
    }
}


TEST(BTreePositive, GetRange) {
    typedef int key_t;
    typedef int value_t;
    typedef std::map<size_t, std::vector<uint8_t>> v_storage;
    typedef BTree<key_t, value_t, v_storage, v_storage> tree_t;

    tree_t tree((std::less<key_t>()), 10);

    size_t n = 1000;
    std::vector<std::pair<int, int>> arr;
    genIntPairs(n, 239179, arr);
    for (size_t index = 0; index < n; index++)
        tree.set(arr[index].first, arr[index].second);
    std::sort(arr.begin(), arr.end());
    for (size_t index = 0; index < n; index++) {
        size_t l = rand() % n, r = rand() % n;
        if (l > r)
            std::swap(l, r);
        std::vector<std::pair<int, int>> ret;
        tree.getRange(arr[l].first, arr[r].first, ret);
        for (size_t index = l; index < r; index++)
            EXPECT_EQ(arr[index], ret[index - l]);
    }
}


TEST(BTreePositive, Brute) {
    typedef int key_t;
    typedef int value_t;
    typedef std::map<size_t, std::vector<uint8_t>> v_storage;
    typedef BTree<key_t, value_t, v_storage, v_storage> tree_t;

    for (size_t times = 0; times < 100; times++) {
        tree_t tree((std::less<key_t>()), 64);

        size_t n = 1000 + rand() % 1000;
        std::vector<std::pair<int, int>> arr;

        genIntPairs(n, rand(), arr);
        for (size_t index = 0; index < n; index++)
            tree.set(arr[index].first, arr[index].second);
        std::sort(arr.begin(), arr.end());
        for (size_t index = 0; index < n; index++) {
            int t = rand() % 3;

            if (t == 2) {
                size_t index = rand() % arr.size();
                EXPECT_EQ(arr[index].second, tree.get(arr[index].first));
            }
            if (t == 1) {
                size_t index = rand() % arr.size();
                tree.del(arr[index].first);
                ASSERT_THROW(tree.get(arr[index].first), std::runtime_error);
                arr.erase(arr.begin() + index);
            }
            if (t == 0) {
                size_t l = rand() % arr.size(), r = rand() % arr.size();
                if (l > r)
                    std::swap(l, r);
                std::vector<std::pair<int, int>> ret;
                tree.getRange(arr[l].first, arr[r].first, ret);
                for (size_t index = l; index < r; index++)
                    EXPECT_EQ(arr[index], ret[index - l]);
            }
        }
    }
}

#endif //TERM_PROJECT_TEST_BTREE_H
