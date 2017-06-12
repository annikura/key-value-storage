#include "gtest/gtest.h"
#include <chrono>

#define DNDEBUG

#include "test_leaf.h"
#include "test_inner.h"
#include "test_btree.h"
#include "test_file_array.h"
#include "test_io_storage.h"
#include "test-key-value-storage.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}