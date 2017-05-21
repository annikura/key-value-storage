//
// Created by annikura on 21.05.17.
//

#ifndef TERM_PROJECT_TEST_INNER_H
#define TERM_PROJECT_TEST_INNER_H

#include <map>
#include <gtest/gtest.h>
#include "../src/node/InnerNode.h"

TEST(InnerCreation, ParametersTest) {
    InnerNode<int> node;

    EXPECT_EQ(0, node.size());
    EXPECT_EQ(false, node.isLeaf());
    EXPECT_EQ(-1, node.getId());
}

TEST(InnerSetters, Simple) {
    InnerNode<int>node;
    size_t id = 74, child = 88;

    node.setId(id);
    EXPECT_EQ(id, node.getId());
}

TEST(InnerAdd, Unary) {
    InnerNode<std::string> node;
    size_t child_id = 10;
    std::string key = "10x";

    node.addKey(key, child_id, node.find(key, std::less<std::string>()));

    EXPECT_EQ(key, node.getKey(0));
    EXPECT_EQ(child_id, node.getChild(0));
    EXPECT_EQ(0, node.find(key, std::less<std::string>()));
}

TEST(InnerAdd, SeveralInOrder) {
    InnerNode<std::string> node;

    for (char i = 'a'; i <= 'z'; i++) {
        std::string s;
        s += i;
        node.addKey(s, i - 'a' + 1, node.find(s, std::less<std::string>()));
    }

    for (char i = 'a'; i <= 'z'; i++) {
        std::string s;
        s += i;
        EXPECT_EQ(i - 'a', node.find(s, std::less<std::string>()));
        EXPECT_EQ(s, node.getKey(i - 'a'));
        EXPECT_EQ(i - 'a' + 1, node.getChild(i - 'a'));
    }
}

TEST(InnerAdd, SeveralInReverseOrder) {
    InnerNode<std::string> node;

    for (char i = 'z'; i >= 'a'; i--) {
        std::string s;
        s += i;
        node.addKey(s, i - 'a' + 1, node.find(s, std::less<std::string>()));
    }

    for (char i = 'a'; i <= 'z'; i++) {
        std::string s;
        s += i;
        EXPECT_EQ(i - 'a', node.find(s, std::less<std::string>()));
        EXPECT_EQ(s, node.getKey(i - 'a'));
        EXPECT_EQ(i - 'a' + 1, node.getChild(i - 'a'));
    }
}

TEST(InnerAdd, BigRandom1) {
    InnerNode<int> node;
    srand(179239);
    int max = 0;
    std::map<int, size_t> map;

    size_t n = 10000;
    for (size_t i = 0; i < n; i++) {
        int key = rand();
        size_t child = rand();
        while (map.count(key) > 0)
            key = rand();
        map[key] = child;

        node.addKey(key, child, node.find(key, std::less<size_t>()));

        max = std::max(max, key);
        EXPECT_EQ(max, node.getMax());
    }

    for (auto & el: map) {
    EXPECT_EQ(el.first, node.getKey(node.find(el.first, std::less<size_t>())));
    EXPECT_EQ(el.second, node.getChild(node.find(el.first, std::less<size_t>())));
    }
}

TEST(InnerAdd, BigRandom2) {
    InnerNode<int> node;
    srand(239179);
    int max = 0;
    std::map<int, size_t> map;

    size_t n = 10000;
    for (size_t i = 0; i < n; i++) {
        int key = rand();
        size_t child = rand();
        while (map.count(key) > 0)
            key = rand();
        map[key] = child;

        node.addKey(key, child, node.find(key, std::less<size_t>()));

        max = std::max(max, key);
        EXPECT_EQ(max, node.getMax());
    }

    for (auto & el: map) {
    EXPECT_EQ(el.first, node.getKey(node.find(el.first, std::less<size_t>())));
    EXPECT_EQ(el.second, node.getChild(node.find(el.first, std::less<size_t>())));
    }
}


TEST(InnerGetChild, Big1) {
    InnerNode<int> node;
    srand(179239);
    int max = 0;
    std::map<int, size_t> map;

    size_t n = 10000;
    for (size_t i = 0; i < n; i++) {
        int key = rand();
        size_t child = rand();
        while (map.count(key) > 0)
            key = rand();
        map[key] = child;

        node.addKey(key, child, node.find(key, std::less<size_t>()));

        max = std::max(max, key);
        EXPECT_EQ(max, node.getMax());
    }

    for (auto & el: map) {
        if (rand() % 2) {
            size_t child = rand();
            map[el.first] = child;
            node.setChild(node.find(el.first, std::less<int>()), child);
        }
    }

    for (auto & el: map) {
        EXPECT_EQ(el.first, node.getKey(node.find(el.first, std::less<size_t>())));
        EXPECT_EQ(el.second, node.getChild(node.find(el.first, std::less<size_t>())));
    }
}

TEST(InnerGetChild, Big2) {
    InnerNode<int> node;
    srand(239179);
    int max = 0;
    std::map<int, size_t> map;

    size_t n = 10001;
    for (size_t i = 0; i < n; i++) {
        int key = rand();
        size_t child = rand();
        while (map.count(key) > 0)
            key = rand();
        map[key] = child;

        node.addKey(key, child, node.find(key, std::less<size_t>()));

        max = std::max(max, key);
        EXPECT_EQ(max, node.getMax());
    }

    for (auto & el: map) {
        if (rand() % 2) {
            size_t child = rand();
            map[el.first] = child;
            node.setChild(node.find(el.first, std::less<int>()), child);
        }
    }

    for (auto & el: map) {
        EXPECT_EQ(el.first, node.getKey(node.find(el.first, std::less<size_t>())));
        EXPECT_EQ(el.second, node.getChild(node.find(el.first, std::less<size_t>())));
    }
}

TEST(InnerDeleteKey, Big1) {
    InnerNode<int> node;
    srand(179239);
    int max = 0;
    std::map<int, size_t> map;

    size_t n = 10000;
    for (size_t i = 0; i < n; i++) {
        int key = rand();
        size_t child = rand();
        while (map.count(key) > 0)
            key = rand();
        map[key] = child;

        node.addKey(key, child, node.find(key, std::less<size_t>()));

        max = std::max(max, key);
        EXPECT_EQ(max, node.getMax());
    }

    for (auto & el: map) {
        if (rand() % 2) {
            node.deleteKey(node.find(el.first, std::less<int>()));
            map.erase(map.find(el.first));
        }
    }

    for (auto & el: map) {
        EXPECT_EQ(el.first, node.getKey(node.find(el.first, std::less<size_t>())));
        EXPECT_EQ(el.second, node.getChild(node.find(el.first, std::less<size_t>())));
    }
}

TEST(InnerDeleteKey, Big2) {
    InnerNode<int> node;
    srand(239179);
    int max = 0;
    std::map<int, size_t> map;

    size_t n = 10001;
    for (size_t i = 0; i < n; i++) {
        int key = rand();
        size_t child = rand();
        while (map.count(key) > 0)
            key = rand();
        map[key] = child;

        node.addKey(key, child, node.find(key, std::less<size_t>()));

        max = std::max(max, key);
        EXPECT_EQ(max, node.getMax());
    }

    for (auto & el: map) {
        if (rand() % 2) {
            node.deleteKey(node.find(el.first, std::less<int>()));
            map.erase(map.find(el.first));
        }
    }

    for (auto & el: map) {
        EXPECT_EQ(el.first, node.getKey(node.find(el.first, std::less<size_t>())));
        EXPECT_EQ(el.second, node.getChild(node.find(el.first, std::less<size_t>())));
    }
}

TEST(InnerSplit, Simple) {
    InnerNode<int> node;

    int key1 = 1, key2 = 2;
    size_t child1 = 3, child2 = 4;
    size_t id1 = 444, id2 = 555;
    node.addKey(key1, child1, 0);
    node.addKey(key2, child2, 1);

    std::tuple<InnerNode<int>,
            InnerNode<int>> ret = node.split(id1, id2);
    EXPECT_EQ(1, std::get<0>(ret).size());
    EXPECT_EQ(id1, std::get<0>(ret).getId());
    EXPECT_EQ(key1, std::get<0>(ret).getKey(0));
    EXPECT_EQ(child1, std::get<0>(ret).getChild(0));

    EXPECT_EQ(1, std::get<1>(ret).size());
    EXPECT_EQ(id2, std::get<1>(ret).getId());
    EXPECT_EQ(key2, std::get<1>(ret).getKey(0));
    EXPECT_EQ(child2, std::get<1>(ret).getChild(0));
}


TEST(InnerJoin, Simple) {
    InnerNode<int> node;

    int key1 = 1, key2 = 2;
    size_t child1 = 3, child2 = 4;
    size_t id1 = 444, id2 = 555;
    node.addKey(key1, child1, 0);
    node.addKey(key2, child2, 1);

    std::tuple<InnerNode<int>,
            InnerNode<int>> ret = node.split(id1, id2);

    InnerNode<int> new_node = std::get<0>(ret);
    new_node.join(std::get<1>(ret));

    EXPECT_EQ(2, new_node.size());
    EXPECT_EQ(id1, new_node.getId());
    EXPECT_EQ(key1, new_node.getKey(0));
    EXPECT_EQ(child1, new_node.getChild(0));
    EXPECT_EQ(key2, new_node.getKey(1));
    EXPECT_EQ(child2, new_node.getChild(1));
}

TEST(InnerSplitJoin, BigOdd) {
    InnerNode<int> node;
    srand(239179);
    int max = 0;
    size_t id1 = 444, id2 = 555;
    std::map<int, size_t> map;

    size_t n = 10000;
    for (size_t i = 0; i < n; i++) {
        int key = rand();
        size_t child = rand();
        while (map.count(key) > 0)
            key = rand();
        map[key] = child;

        node.addKey(key, child, node.find(key, std::less<size_t>()));

        max = std::max(max, key);
        EXPECT_EQ(max, node.getMax());
    }


    std::tuple<InnerNode<int>,
            InnerNode<int>> ret = node.split(id1, id2);
    InnerNode<int> new_node = std::get<0>(ret);
    new_node.join(std::get<1>(ret));

    EXPECT_EQ(n, new_node.size());
    EXPECT_EQ(id1, new_node.getId());

    for (auto & el: map) {
        EXPECT_EQ(el.first, node.getKey(node.find(el.first, std::less<size_t>())));
        EXPECT_EQ(el.second, node.getChild(node.find(el.first, std::less<size_t>())));
    }
}


TEST(InnerSplitJoin, BigEven) {
    InnerNode<int> node;
    srand(179239);
    int max = 0;
    size_t id1 = 444, id2 = 555;
    std::map<int, size_t> map;

    size_t n = 10001;
    for (size_t i = 0; i < n; i++) {
        int key = rand();
        size_t child = rand();
        while (map.count(key) > 0)
            key = rand();
        map[key] = child;

        node.addKey(key, child, node.find(key, std::less<size_t>()));

        max = std::max(max, key);
        EXPECT_EQ(max, node.getMax());
    }


    std::tuple<InnerNode<int>,
            InnerNode<int>> ret = node.split(id1, id2);
    InnerNode<int> new_node = std::get<0>(ret);
    new_node.join(std::get<1>(ret));

    EXPECT_EQ(n, new_node.size());
    EXPECT_EQ(id1, new_node.getId());

    for (auto & el: map) {
        EXPECT_EQ(el.first, node.getKey(node.find(el.first, std::less<size_t>())));
        EXPECT_EQ(el.second, node.getChild(node.find(el.first, std::less<size_t>())));
    }
}


#endif //TERM_PROJECT_TEST_INNER_H
            std::map<size_t, int> map;
