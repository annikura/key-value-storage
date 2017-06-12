#ifndef TERM_PROJECT_FILEARRAY_H
#define TERM_PROJECT_FILEARRAY_H

#include <cstdio>
#include <ios>
#include <fstream>
#include "../utilities.h"
#include "Journal.h"

class FileArray {
private:
    std::fstream fs;
    size_t sz = -1;
    const size_t N;
    Journal journal;
public:
    FileArray(const std::string & filename, size_t blk_sz, bool rewrite=false);
    ~FileArray();
    size_t size();
    size_t countPosition(size_t id);
    template <typename T>
    void pushBack(const T &src, size_t obj_size = -1);
    void popBack();
    template <typename T>
    void back(T & dest);
    template <typename T>
    void readElement(T &dest, size_t index);
    template <typename T>
    void writeElement(const T & src, size_t index, size_t obj_size = -1);
    template <typename T>
    void write(const T & src, size_t beg = -1, size_t blk_sz = -1);
};

template <typename T>
void FileArray::pushBack(const T &src, size_t obj_size) {
    if (obj_size == -1) obj_size = N;
    write(src, -1, obj_size);
    write(++sz, 0, sizeof(sz));
}

template <typename T>
void FileArray::back(T & dest) {
    readElement(dest, sz - 1);
}

template <typename T>
void FileArray::readElement(T &dest, size_t index) {
    assert(index < size());
    fs.seekg(countPosition(index), std::ios_base::beg);
    fs.read(reinterpret_cast<char *>(&dest), N);
}

template <typename T>
void FileArray::writeElement(const T & src, size_t index, size_t obj_size) {
    if (obj_size == -1) obj_size = N;
    if (index == sz)
        pushBack(src, obj_size);
    else
        write(src, countPosition(index), obj_size);
}

template <typename T>
void FileArray::write(const T & src, size_t beg, size_t blk_sz) {
    if (beg == -1) beg = countPosition(sz);
    if (blk_sz == -1) blk_sz = N;

    if (sz != -1 && beg != sz * N + sizeof(size_t)) {
        std::vector<uint8_t> tmp(blk_sz);
        fs.seekg(beg, std::ios_base::beg);
        //std::cerr << beg << " " <<  sz << "\n";
        fs.read(reinterpret_cast<char *>(&tmp[0]), blk_sz);
        journal.journalize(beg, reinterpret_cast<char *>(&tmp[0]), blk_sz);
    }
    fs.seekp(beg, std::ios_base::beg);
    fs.write(reinterpret_cast<char *>(const_cast<T*>(&src)), blk_sz);

}


#endif //TERM_PROJECT_FILEARRAY_H
