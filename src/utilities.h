//
// Created by annikura on 01.06.17.
//

#ifndef TERM_PROJECT_UTILITIES_H
#define TERM_PROJECT_UTILITIES_H

#include <vector>
#include <cstdint>
#include <cstdlib>
#include <iostream>

template <typename T>
std::vector<uint8_t> serialize(const T & obj) {
    std::vector<uint8_t> ret(sizeof(obj));
    *reinterpret_cast<T*>(&ret[0]) = obj;
    //std::cerr << sizeof(obj) << " " << ret.size() << "\n";
    return std::move(ret);
}


template <typename T>
size_t serialize(const T & obj, std::vector<uint8_t> & dest, size_t index) {
    //std::vector<uint8_t> ret(sizeof(obj));
    dest.resize(index + sizeof(obj));
    *reinterpret_cast<T*>(&dest[index]) = obj;
    //std::cerr << dest.size() << "\n";
    return index + sizeof(obj);
}


template <typename T>
T deserialize(const std::vector<uint8_t> & src, size_t index) {
    //if (index == 0)
    //    std::cerr << "woop!" << sizeof(T) << " " << src.size() << "\n";
    assert(index + sizeof(T) <= src.size());
    return *reinterpret_cast<T*>(const_cast<uint8_t *>(&src[index]));
}

#endif //TERM_PROJECT_UTILITIES_H
