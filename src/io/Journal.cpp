#include <cassert>
#include <iostream>
#include "Journal.h"

void Journal::readNextPosition() {
    file.seekg(0, std::ios_base::beg);
    size_t var1, var2;
    uint8_t check_byte;
    file.read(reinterpret_cast<char *>(&var1), sizeof(var1));
    file.read(reinterpret_cast<char *>(&var2), sizeof(var2));
    file.read(reinterpret_cast<char *>(&check_byte), sizeof(check_byte));

    if (var1 == var2 || check_byte == 1)
        next_position = var1;
    else
        next_position = var2;
}

void Journal::writeNextPosition(size_t value) {
    file.seekp(0, std::ios_base::beg);
    uint8_t check_byte = 1;
    file.write(reinterpret_cast<char *>(&next_position), sizeof(next_position));
    file.seekp(sizeof(next_position) * 2, std::ios_base::beg);
    file.write(reinterpret_cast<char *>(&check_byte), sizeof(check_byte));
    file.seekp(sizeof(next_position), std::ios_base::beg);
    file.write(reinterpret_cast<char *>(&next_position), sizeof(next_position));
    check_byte = 0;
    file.write(reinterpret_cast<char *>(&check_byte), sizeof(check_byte));
    file.sync();
}

Journal::Journal(const std::string & filename, bool create) {
    file.exceptions(std::ios_base::badbit | std::ios_base::failbit);
    if (create) {
        try {
            system(("touch " + journalName(filename)).c_str());
            file.open(journalName(filename), std::ios_base::in | std::ios_base::out);
            size_t zero = 0;
            file.write(reinterpret_cast<char *>(&zero), sizeof(size_t));
            file.write(reinterpret_cast<char *>(&zero), sizeof(size_t));
            file.write(reinterpret_cast<char *>(&zero), 1);
            file.sync();
            next_position = 2 * sizeof(size_t) + 1;
        }
        catch (std::ios_base::failure &) {
            throw std::ios_base::failure("failed to initialize Storage filesystem");
        }
    }
    else {
        file.open(journalName(filename));
        readNextPosition();
    }
}

Journal::Journal(const std::string & restoring_file) { // constructor which restores given file according to the journal
    std::fstream recovering_file;
    try {
        recovering_file.exceptions(std::ios_base::badbit | std::ios_base::failbit);
        recovering_file.open(restoring_file);

        file.exceptions(std::ios_base::badbit | std::ios_base::failbit);
        file.open(journalName(restoring_file));
        readNextPosition();
    }
    catch (std::ios_base::failure &) {
        throw std::ios_base::failure("Required storage doesn't exist");
    }

    file.seekg(sizeof(size_t) * 2 + 1, std::ios_base::beg);
    while (file.tellg() < next_position) {
        size_t position, data_size;
        file.read(reinterpret_cast<char *>(&position), sizeof(position));
        file.read(reinterpret_cast<char *>(&data_size), sizeof(data_size));

        std::vector<uint8_t> data(data_size);
        file.read(reinterpret_cast<char *>(&data[0]), data_size);

        recovering_file.seekp(position);
        recovering_file.write(reinterpret_cast<char *>(&data[0]), data_size);
    }
    writeNextPosition(sizeof(size_t) * 2 + 1);
}

void Journal::journalize(size_t beg, char * const data, size_t size) {
    assert(file.is_open());
    file.seekp(next_position, std::ios_base::beg);
    file.write(reinterpret_cast<char *>(&beg), sizeof(beg));
    file.write(reinterpret_cast<char *>(&size), sizeof(size));
    file.write(data, size);
    file.sync();
    writeNextPosition(file.tellp());
}

void Journal::deleteJournal(const std::string & filename) {
    system(("rm -f " + journalName(filename) + "\n").c_str());
}

std::string Journal::journalName(const std::string & filename) {
    return filename + ".journal";
}