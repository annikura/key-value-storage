#ifndef TERM_PROJECT_FILEARRAY_H
#define TERM_PROJECT_FILEARRAY_H

#include <cstdio>
#include <ios>
#include <fstream>

class FileArray {
private:
    std::fstream fs;
    size_t sz = -1;
    const size_t N;
public:
    FileArray(std::string filename, size_t blk_sz, bool rewrite=false) : N(blk_sz) {
        fs.exceptions(std::ios_base::badbit | std::ios_base::failbit);
        fs.open(filename, std::ios_base::out | std::ios_base::in | std::ios_base::binary);
        if (rewrite) {
            write<size_t>(0, 0, sizeof(sz));
            sz = 0;
        } else {
            sz = size();
        }
    }
    ~FileArray() {
        fs.close();
    }

    size_t size() {
        if (sz == -1) {
            fs.seekg(0, std::ios_base::beg);
            fs.read(reinterpret_cast<char *>(&sz), sizeof(sz));
            if (fs.eof())
                sz = 0;
        }
        return sz;
    }

    size_t countPosition(size_t id) {
        return sizeof(size_t) + id * N;
    }

    template <typename T>
    void pushBack(const T &src) {
        write(src);
        write(++sz, 0, sizeof(sz));
    }

    void popBack() {
        write(--sz, 0, sizeof(sz));
    }

    template <typename T>
    void back(T & dest) {
        readElement(dest, sz - 1);
    }

    template <typename T>
    void readElement(T &dest, size_t index) {
        assert(index < size());
        fs.seekg(countPosition(index), std::ios_base::beg);
        fs.read(reinterpret_cast<char *>(&dest), N);
    }

    template <typename T>
    inline void writeElement(const T & src, size_t index) {
        if (index == sz)
            pushBack(src);
        else
            write(src, countPosition(index));
    }

    template <typename T>
    inline void write(const T & src, size_t beg = -1, size_t blk_sz = -1) {
        if (beg == -1) beg = countPosition(sz);
        if (blk_sz == -1) blk_sz = N;
        fs.seekp(beg, std::ios_base::beg);
        fs.write(reinterpret_cast<char *>(const_cast<T*>(&src)), blk_sz);
        fs.sync();
        fs.flush();
    }
};


#endif //TERM_PROJECT_FILEARRAY_H
