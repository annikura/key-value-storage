#ifndef TERM_PROJECT_UTILITIES_H
#define TERM_PROJECT_UTILITIES_H

#include <vector>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cassert>

template <typename T>
std::vector<uint8_t> serialize(const T & obj) {
    std::vector<uint8_t> ret(sizeof(obj));
    *reinterpret_cast<T*>(&ret[0]) = obj;
    return std::move(ret);
}


template <typename T>
size_t serialize(const T & obj, std::vector<uint8_t> & dest, size_t index) {
    dest.resize(index + sizeof(obj));
    *reinterpret_cast<T*>(&dest[index]) = obj;
    return index + sizeof(obj);
}


template <typename T>
T deserialize(const std::vector<uint8_t> & src, size_t index) {
    assert(index + sizeof(T) <= src.size());
    return *reinterpret_cast<T*>(const_cast<uint8_t *>(&src[index]));
}

template <typename T>
struct is_journalized {
    template <typename U>
    static char test(typename U::journal*);

    template <typename U>
    static int test(U* x);

    static const bool value = (sizeof(test<T>(nullptr)) == 1);
};

#endif //TERM_PROJECT_UTILITIES_H
