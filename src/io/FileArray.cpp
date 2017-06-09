#include "FileArray.h"

void FileArray::popBack() {
    write(--sz, 0, sizeof(sz));
}

FileArray::FileArray(std::string filename, size_t blk_sz, bool rewrite) : N(blk_sz) {
    std::system(("touch " + filename).c_str());
    //std::cerr << clock() << "\n";
    //std::cerr << (long double)clock() / CLOCKS_PER_SEC << "\n";
    //size_t file_size = getFileSize(filename);
    //std::cerr << (long double)clock() / CLOCKS_PER_SEC << "\n";

    //fs.exceptions(std::ios_base::badbit | std::ios_base::failbit);
    fs.open(filename, std::ios_base::out | std::ios_base::in | std::ios_base::binary);
    //std::cerr << (long double)clock() / CLOCKS_PER_SEC << "\n";

    if (rewrite) { // || file_size < sizeof(sz)) {
        write<size_t>(0, 0, sizeof(sz));
        sz = 0;
    } else {
        sz = size();
    }
    std::cerr << "fuck\n";
}
FileArray::~FileArray() {
    fs.close();
}

size_t FileArray::size() {
    if (sz == -1) {
        fs.seekg(0, std::ios_base::beg);
        fs.read(reinterpret_cast<char *>(&sz), sizeof(sz));
        if (fs.eof())
            sz = 0;
    }
    return sz;
}

size_t FileArray::countPosition(size_t id) {
    return sizeof(size_t) + id * N;
}
