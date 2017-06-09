//
// Created by annikura on 21.05.17.
//

#ifndef TERM_PROJECT_TEST_LEAF_H
#define TERM_PROJECT_TEST_LEAF_H

#include <map>
#include "gtest/gtest.h"
#include "../src/node/LeafNode.h"

TEST(LeafCreation, ParametersTest) {
    LeafNode<int, int, std::map<size_t, std::vector<uint8_t>>> node;

    EXPECT_EQ(0, node.size());
    EXPECT_EQ(-1, node.getNext());
    EXPECT_EQ(-1, node.getPrev());
    EXPECT_EQ(true, node.isLeaf());
    EXPECT_EQ(-1, node.getId());
}


TEST(LeafSetters, Simple) {
    LeafNode<int, int, std::map<size_t, std::vector<uint8_t>>> node;
    size_t next = 100, prev = 519, id = 74;

    node.setNext(next);
    node.setPrev(prev);
    node.setId(id);

    EXPECT_EQ(next, node.getNext());
    EXPECT_EQ(prev, node.getPrev());
    EXPECT_EQ(id, node.getId());
}

TEST(LeafAdd, Unary) {
    LeafNode<std::string, std::string, std::map<size_t , std::vector<uint8_t>>> node;
    size_t value_id = 0;
    std::string key = "10x", value = "25x";

    node.addKey(0, value_id, key, value);

    EXPECT_EQ(key, node.getKey(0));
    EXPECT_EQ(value, node.getValue(0));
    EXPECT_EQ(0, node.find(key, std::less<std::string>()));
}

TEST(LeafAdd, SeveralInOrder) {
    LeafNode<std::string, char, std::map<size_t , std::vector<uint8_t>>> node;

    for (char i = 'a'; i <= 'z'; i++) {
        std::string s;
        s += i;
        node.addKey(node.find(s, std::less<std::string>()), i - 'a', s, i);
    }

    for (char i = 'a'; i <= 'z'; i++) {
        std::string s;
        s += i;
        EXPECT_EQ(i - 'a', node.find(s, std::less<std::string>()));
        EXPECT_EQ(s, node.getKey(i - 'a'));
        EXPECT_EQ(i, node.getValue(i - 'a'));
    }
}


TEST(LeafAdd, SeveralInReverseOrder) {
    LeafNode<std::string, char, std::map<size_t, std::vector<uint8_t>>> node;

    for (char i = 'z'; i >= 'a'; i--) {
        std::string s;
        s += i;
        node.addKey(node.find(s, std::less<std::string>()), abs(i - 'z'), s, i);
    }

    for (char i = 'a'; i <= 'z'; i++) {
        std::string s;
        s += i;
        EXPECT_EQ(i - 'a', node.find(s, std::less<std::string>()));
        EXPECT_EQ(s, node.getKey(i - 'a'));
        EXPECT_EQ(i, node.getValue(i - 'a'));
    }
}

TEST(LeafAdd, BigRandom1) {
    LeafNode<int, int, std::map<size_t, std::vector<uint8_t>>> node;
    srand(179239);
    int max = 0;

    size_t n = 10000;
    std::map<size_t, int> map;
    for (size_t i = 0; i < n; i++) {
        int key = rand(), val = rand();
        while (map.count(key) > 0)
            key = rand();
        map[key] = val;

        node.addKey(node.find(key, std::less<int>()), i, key, val);
        max = std::max(max, key);
        EXPECT_EQ(max, node.getMax());
    }

    for (auto & el: map) {
        EXPECT_EQ(el.first, node.getKey(node.find(el.first, std::less<int>())));
        EXPECT_EQ(el.second, node.getValue(node.find(el.first, std::less<int>())));
    }
}


TEST(LeafAdd, BigRandom2) {
    LeafNode<int, int, std::map<size_t , std::vector<uint8_t>>> node;
    srand(239179);

    size_t n = 10000;
    std::map<size_t, int> map;
    for (size_t i = 0; i < n; i++) {
        int key = rand(), val = rand();
        while (map.count(key) > 0)
            key = rand();
        map[key] = val;

        node.addKey(node.find(key, std::less<int>()), i, key, val);
    }

    for (auto & el: map) {
        EXPECT_EQ(el.first, node.getKey(node.find(el.first, std::less<int>())));
        EXPECT_EQ(el.second, node.getValue(node.find(el.first, std::less<int>())));
    }
}

TEST(LeafSetValue, Big1) {
    LeafNode<int, int, std::map<size_t , std::vector<uint8_t>>> node;
    srand(179239);

    size_t n = 10000;
    std::map<int, int> map;
    for (size_t i = 0; i < n; i++) {
        int key = rand(), val = rand();
        while (map.count(key) > 0)
            key = rand();
        map[key] = val;

        node.addKey(node.find(key, std::less<int>()), i, key, val);
    }

    for (auto & el: map) {
        if (rand() % 2) {
            int val = rand();
            map[el.first] = val;
            node.setValue(val, node.find(el.first, std::less<int>()));
        }
    }

    for (auto & el: map) {
        EXPECT_EQ(el.first, node.getKey(node.find(el.first, std::less<int>())));
        EXPECT_EQ(el.second, node.getValue(node.find(el.first, std::less<int>())));
    }
}


TEST(LeafSetValue, Big2) {
    LeafNode<int, int, std::map<size_t, std::vector<uint8_t>>> node;
    srand(239179);

    size_t n = 10000;
    std::map<int, int> map;
    for (size_t i = 0; i < n; i++) {
        int key = rand(), val = rand();
        while (map.count(key) > 0)
            key = rand();
        map[key] = val;

        node.addKey(node.find(key, std::less<int>()), i, key, val);
    }

    for (auto & el: map) {
        if (rand() % 2) {
            int val = rand();
            map[el.first] = val;
            node.setValue(val, node.find(el.first, std::less<int>()));
        }
    }

    for (auto & el: map) {
        EXPECT_EQ(el.first, node.getKey(node.find(el.first, std::less<int>())));
        EXPECT_EQ(el.second, node.getValue(node.find(el.first, std::less<int>())));
    }
}

TEST(LeafSplit, Simple) {
    LeafNode<int, int, std::map<size_t , std::vector<uint8_t>>> node;

    int key1 = 1, key2 = 2, val1 = 3, val2 = 4;
    size_t id1 = 444, id2 = 555;
    size_t prev = 100, next = 200;
    node.addKey(0, 0, key1, val1);
    node.addKey(1, 1, key2, val2);
    node.setPrev(prev);
    node.setNext(next);

    std::tuple<LeafNode<int, int, std::map<size_t , std::vector<uint8_t>>>,
               LeafNode<int, int, std::map<size_t , std::vector<uint8_t>>>> ret = node.split(id1, id2);
    EXPECT_EQ(1, std::get<0>(ret).size());
    EXPECT_EQ(id1, std::get<0>(ret).getId());
    EXPECT_EQ(id2, std::get<0>(ret).getNext());
    EXPECT_EQ(prev, std::get<0>(ret).getPrev());
    EXPECT_EQ(key1, std::get<0>(ret).getKey(0));
    EXPECT_EQ(val1, std::get<0>(ret).getValue(0));

    EXPECT_EQ(1, std::get<1>(ret).size());
    EXPECT_EQ(id2, std::get<1>(ret).getId());
    EXPECT_EQ(id1, std::get<1>(ret).getPrev());
    EXPECT_EQ(next, std::get<1>(ret).getNext());
    EXPECT_EQ(key2, std::get<1>(ret).getKey(0));
    EXPECT_EQ(val2, std::get<1>(ret).getValue(0));
}


TEST(LeafJoin, Simple) {
    LeafNode<int, int, std::map<size_t , std::vector<uint8_t>>> node;

    int key1 = 1, key2 = 2, val1 = 3, val2 = 4;
    size_t id1 = 444, id2 = 555;
    size_t prev = 100, next = 200;
    node.addKey(0, 0, key1, val1);
    node.addKey(1, 1, key2, val2);
    node.setPrev(prev);
    node.setNext(next);

    std::tuple<LeafNode<int, int, std::map<size_t , std::vector<uint8_t>>>,
            LeafNode<int, int, std::map<size_t , std::vector<uint8_t>>>> ret = node.split(id1, id2);

    LeafNode<int, int, std::map<size_t , std::vector<uint8_t>>> new_node = std::get<0>(ret);
    new_node.join(std::get<1>(ret));

    EXPECT_EQ(2, new_node.size());
    EXPECT_EQ(id1, new_node.getId());
    EXPECT_EQ(prev, new_node.getPrev());
    EXPECT_EQ(next, new_node.getNext());
    EXPECT_EQ(key1, new_node.getKey(0));
    EXPECT_EQ(val1, new_node.getValue(0));
    EXPECT_EQ(key2, new_node.getKey(1));
    EXPECT_EQ(val2, new_node.getValue(1));
}

#endif //TERM_PROJECT_TEST_LEAF_H
