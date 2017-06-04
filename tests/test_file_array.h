#ifndef TERM_PROJECT_TEST_FILE_ARRAY_H
#define TERM_PROJECT_TEST_FILE_ARRAY_H

#include <algorithm>
#include "gtest/gtest.h"
#include "../src/io/FileArray.h"

TEST(FileArray, Simple) {
    FileArray file("file", 4, true);
    EXPECT_EQ(0, file.size());
}

TEST(FileArray, SimpleAdd) {
    FileArray file("file", sizeof(int), true);

    file.pushBack((int) 5);
    int dest;
    file.readElement(dest, 0);
    EXPECT_EQ(1, file.size());
    EXPECT_EQ(5, dest);
}

TEST(FileArray, SimpleAdd2) {
    FileArray file("file", sizeof(uint8_t), true);
    uint8_t val1 = 15, val2 = 28, x1, x2;

    file.pushBack(val1);
    file.pushBack(val2);

    file.readElement(x1, 0);
    file.readElement(x2, 1);
    EXPECT_EQ(2, file.size());
    EXPECT_EQ(x1, val1);
    EXPECT_EQ(x2, val2);
}

TEST(FileArray, SimpleDel) {
    FileArray file("file", sizeof(uint8_t), true);
    uint8_t val1 = 15, val2 = 28, x1, x2;

    file.pushBack(val1);
    file.popBack();
    file.pushBack(val2);
    file.pushBack(val1);

    file.readElement(x1, 1);
    file.readElement(x2, 0);
    EXPECT_EQ(2, file.size());
    EXPECT_EQ(x1, val1);
    EXPECT_EQ(x2, val2);
}

TEST(FileArray, SimpleDel1) {
    FileArray file("file", sizeof(uint8_t), true);
    uint8_t val1 = 15, val2 = 28;

    file.pushBack(val1);
    file.pushBack(val2);
    file.popBack();
    file.popBack();

    EXPECT_EQ(0, file.size());
}

TEST(FileArray, SimpleBack) {
    FileArray file("file", sizeof(uint8_t), true);
    uint8_t val1 = 15, val2 = 28;

    file.pushBack(val1);
    file.pushBack(val2);
    file.popBack();
    file.popBack();

    EXPECT_EQ(0, file.size());
}

TEST(FileArray, BigAdd) {
    srand(179239);

    FileArray file ("file", sizeof(int), true);
    std::vector<std::pair<int, size_t>> vec;
    size_t n = 10000;

    for (size_t i = 0; i < n; i++) {
        vec.push_back(std::make_pair(rand(), i));
        file.pushBack(vec.back().first);
    }

    std::sort(vec.begin(), vec.end());
    for (size_t i = 0; i < n; i++) {
        int x = -1;
        file.readElement(x, vec[i].second);
        EXPECT_EQ(vec[i].first, x);
    }
}

TEST(FileArray, BigWriteElement) {
    srand(179239);

    FileArray file ("file", sizeof(int), true);
    std::vector<std::pair<int, size_t>> vec;
    size_t n = 10000;

    for (size_t i = 0; i < n; i++) {
        vec.push_back(std::make_pair(rand(), i));
        file.pushBack(vec.back().first);
    }

    for (size_t i = 0; i < n; i += 2){
        vec[i].first = rand();
        file.writeElement(vec[i].first, i);
    }

    std::sort(vec.begin(), vec.end());
    for (size_t i = 0; i < n; i++) {
        int x = -1;
        file.readElement(x, vec[i].second);
        EXPECT_EQ(vec[i].first, x);
    }
}

TEST(FileArray, BigDel) {
    srand(179239);

    FileArray file ("file", sizeof(int), true);
    std::vector<std::pair<int, size_t>> vec;
    size_t n = 10000, m = n - n / 2;

    for (size_t i = 0; i < n; i++) {
        vec.push_back(std::make_pair(rand(), i));
        file.pushBack(vec.back().first);
    }

    for (size_t i = 0; i < n / 2; i++)
        file.popBack(), vec.pop_back();

    EXPECT_EQ(m, file.size());

    std::sort(vec.begin(), vec.end());
    for (size_t i = 0; i < m; i++) {
        int x = -1;
        file.readElement(x, vec[i].second);
        EXPECT_EQ(vec[i].first, x);
    }
}

TEST(FileArray, BigDel1) {
    srand(179239);

    FileArray file ("file", sizeof(int), true);
    std::vector<std::pair<int, size_t>> vec;
    size_t n = 10000, m = n - n / 2, cnt = 107;

    for (size_t i = 0; i < n; i++) {
        vec.push_back(std::make_pair(rand(), i));
        file.pushBack(vec.back().first);
    }
    for (size_t i = 0; i < n / 2; i++)
        file.popBack(), vec.pop_back();

    EXPECT_EQ(m, file.size());
    for (size_t i = 0; i < cnt; i++) {
        vec.push_back(std::make_pair(rand(), m + i));
        file.pushBack(vec.back().first);
    }

    EXPECT_EQ(m + cnt, file.size());
    std::sort(vec.begin(), vec.end());
    for (size_t i = 0; i < m + cnt; i++) {
        int x = -1;
        file.readElement(x, vec[i].second);
        EXPECT_EQ(vec[i].first, x);
    }
}


TEST(FileArray, BigDelToEmpty) {
    srand(179239);

    FileArray file ("file", sizeof(int), true);
    size_t n = 10000;

    for (size_t i = 0; i < n; i++)
        file.pushBack(rand());
    for (size_t i = 0; i < n; i++)
        file.popBack();

    EXPECT_EQ(0, file.size());
}



#endif //TERM_PROJECT_TEST_FILE_ARRAY_H
