cmake_minimum_required(VERSION 3.5)
project(term_project)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES src/library.cpp src/library.h src/node/BaseNodeClass.h src/node/InnerNode.h src/node/LeafNode.h src/tree/BaseBTreeClass.h src/node/Node.h src/tree/BTree.h)
add_library(term_project ${SOURCE_FILES})