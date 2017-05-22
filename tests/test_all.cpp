//
// Created by annikura on 21.05.17.
//
#include "gtest/gtest.h"

#include "test_leaf.h"
#include "test_inner.h"
#include "test_btree.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}