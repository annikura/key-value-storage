cmake_minimum_required(VERSION 3.7)
project(term_project)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -O3)
set(CMAKE_BUILD_TYPE Release)

set(SOURCE_FILES
        key_value_storage.h
                 src/node/BaseNodeClass.h
                 src/node/InnerNode.h
                 src/node/LeafNode.h
                 src/io/IOStorage.cpp
                 src/io/IOStorage.h
                 src/node/Node.h
                 src/tree/BaseBTreeClass.h
        src/tree/BTree.h
                 src/io/FileArray.h
        src/io/FileArray.cpp src/io/Journal.cpp src/io/Journal.h)

set (TEST_SOURCES
        tests/test_all.cpp
        tests/test_leaf.h
        tests/test_inner.h
        tests/test_btree.h
        tests/test_file_array.h
        tests/test_io_storage.h
        tests/test-key-value-storage.h)


add_executable(test_all ${SOURCE_FILES} ${TEST_SOURCES})
add_executable(main ${SOURCE_FILES} src/main.cpp)

add_library(term_project ${SOURCE_FILES})

target_link_libraries(test_all gtest)