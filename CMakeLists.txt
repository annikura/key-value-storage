cmake_minimum_required(VERSION 3.7)
project(term_project)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES src/library.cpp
                 src/library.h
                 src/node/BaseNodeClass.h
                 src/node/InnerNode.h
                 src/node/LeafNode.h
                 src/node/Node.h
                 src/tree/BaseBTreeClass.h
                 src/tree/BTree.h)

set(TEST_SOURCES tests/test_all.cpp)

add_executable(test_all ${SOURCE_FILES} tests/test_all.cpp tests/test_leaf.h tests/test_inner.h tests/test_btree.h)
add_library(term_project ${SOURCE_FILES} src/io/IOBaseClass.h src/utilities.cpp src/utilities.h)

target_link_libraries(test_all gtest)