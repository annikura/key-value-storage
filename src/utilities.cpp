#include "utilities.h"

size_t getFileSize(const std::string & filename) {
    std::fstream fs;
    fs.exceptions(std::ios_base::badbit | std::ios_base::failbit);
    fs.open(filename, std::ios_base::out | std::ios_base::in | std::ios_base::binary);
    fs.seekg(0, std::ios_base::end);
    size_t ret = fs.tellg();
    fs.close();

    return ret;
}
