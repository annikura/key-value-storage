#ifndef TERM_PROJECT_TEST_IO_STORAGE_H
#define TERM_PROJECT_TEST_IO_STORAGE_H

#include "gtest/gtest.h"
#include "../src/io/IOStorage.h"

static std::string directory = "tests/test_files/";

class IOStorage_f : public testing::Test{
public:
    void setUp() {
    }

    void TearDown() {
        system(("rm " + directory + "storage").c_str());
        system(("rm " + directory + "stack").c_str());
        system(("rm " + directory + "table").c_str());
    }
};

TEST_F(IOStorage_f, Simple) {
    IOStorage storage(directory + "storage", directory + "stack", directory + "table", 1);
    EXPECT_EQ(storage.end(), storage.find(0));
}

TEST_F(IOStorage_f, SimpleAdd) {
    IOStorage storage(directory + "storage", directory + "stack", directory + "table", 1);
    storage.insert(std::make_pair(0, std::vector<uint8_t>(1, 9)));
    auto res = storage.find(0);
    EXPECT_EQ(std::vector<uint8_t>(1, 9), res->second);
    EXPECT_EQ(0, res.getId());
    EXPECT_EQ(0, res.getPosition());
    EXPECT_EQ(storage.end(), storage.find(1));
}

TEST_F(IOStorage_f, SimpleAdd1) {
    IOStorage storage(directory + "storage", directory + "stack", directory + "table", 1);
    storage.insert(std::make_pair(0, std::vector<uint8_t>(1, 9)));
    storage.insert(std::make_pair(1, std::vector<uint8_t>(1, 8)));
    storage.insert(std::make_pair(2, std::vector<uint8_t>(1, 11)));
    EXPECT_EQ(std::vector<uint8_t>(1, 9), storage.find(0)->second);
    EXPECT_EQ(std::vector<uint8_t>(1, 8), storage.find(1)->second);
    EXPECT_EQ(std::vector<uint8_t>(1, 11), storage.find(2)->second);
    EXPECT_EQ(storage.end(), storage.find(3));
}

TEST_F(IOStorage_f, SimpleDel) {
    IOStorage storage(directory + "storage", directory + "stack", directory + "table", 1);
    storage.insert(std::make_pair(0, std::vector<uint8_t>(1, 9)));
    storage.erase(storage.find(0));
    EXPECT_EQ(storage.end(), storage.find(0));
}


TEST_F(IOStorage_f, SimpleDel1) {
    IOStorage storage(directory + "storage", directory + "stack", directory + "table", 1);
    storage.insert(std::make_pair(0, std::vector<uint8_t>(1, 9)));
    storage.insert(std::make_pair(1, std::vector<uint8_t>(1, 11)));
    storage.erase(storage.find(0));
    EXPECT_EQ(storage.end(), storage.find(0));
    EXPECT_EQ(std::vector<uint8_t>(1, 11), storage.find(1)->second);
}

TEST_F(IOStorage_f, BigAdd) {
    size_t sz = 512;
    size_t n = 2000;
    IOStorage storage(directory + "storage", directory + "stack", directory + "table", sz);
    std::vector<std::vector<uint8_t>> vec(n);

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < sz; j++)
            vec[i].push_back(i + j);
        storage.insert(std::make_pair(i, vec[i]));
    }

    for (size_t i = 0; i < n; i++)
        EXPECT_EQ(vec[i], storage.find(i)->second);
}

TEST_F(IOStorage_f, BigDel) {
    size_t sz = 1;
    IOStorage storage(directory + "storage", directory + "stack", directory + "table", sz);
    size_t n = 1;
    std::vector<std::vector<uint8_t>> vec(n);

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < sz; j++)
            vec[i].push_back(rand());
        storage.insert(std::make_pair(i, vec[i]));
    }

    for (size_t i = 0; i < n; i += 2)
        storage.erase(storage.find(i));

    for (size_t i = 0; i < n; i++)
        if (i % 2)
            EXPECT_EQ(vec[i], storage.find(i)->second);
        else
            EXPECT_EQ(storage.end(), storage.find(i));
}


TEST(TimeConsumer, Simple) {
    system("touch file");
    system("rm -f file");
    system("touch file");

    size_t n = 2000, sz = 4096;
    for (size_t i = 0; i < n; i++) {
        std::fstream file;
        file.exceptions(std::ios_base::failbit | std::ios_base::badbit);
        file.open("file", std::ios_base::in | std::ios_base::out | std::ios_base::app   );
        std::vector<uint8_t> vec(sz, i);
        file.write(reinterpret_cast<char *>(&vec[0]), sz);
        file.seekg(0);
        file.flush();
        file.seekg(std::ios_base::end);
        file.close();
    }
}
#endif //TERM_PROJECT_TEST_IO_STORAGE_H
