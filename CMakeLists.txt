cmake_minimum_required(VERSION 3.7)
project(term_project)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES src/library.cpp
                 src/library.h
                 src/node/BaseNodeClass.h
                 src/node/InnerNode.h
                 src/node/LeafNode.h
                 src/io/IOStorage.cpp
                 src/io/IOStorage.h
                 src/node/Node.h
                 src/tree/BaseBTreeClass.h
                 src/tree/BTree.h)

set(TEST_SOURCES tests/test_all.cpp)

add_executable(test_all ${SOURCE_FILES} tests/test_all.cpp tests/test_leaf.h tests/test_inner.h tests/test_btree.h tests/test_file_array.h tests/test_io_storage.h)
add_library(term_project ${SOURCE_FILES} src/io/IOStorage.h src/utilities.h src/io/FileArray.h src/io/IOStorage.cpp)

target_link_libraries(test_all gtest)