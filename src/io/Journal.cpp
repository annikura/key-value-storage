#include <cassert>
#include <iostream>
#include "Journal.h"

void Journal::readNextPosition() {                  // first (sizeof(size_t) * 2 + 1) bytes are reserved for
                                                    // storing 'next_position + next_position + check_byte'.
    size_t var1, var2;                              // So if fist 'next_position' is unequal to the second one,
    uint8_t check_byte;                             // then the problem happened when journal was writing one of them.
    read(var1, 0);                                  // Let's then have a check byte. It will be equal to 1 if we have already
    read(var2);                                     // written the first number and have started to write the second one
    read(check_byte);                               // When we have finished to write the second number, lets set it back to zero.

    next_position = (var1 == var2 || check_byte == 1 ? var1 : var2);
}

void Journal::writeNextPosition(size_t value) {
    file.seekp(0, std::ios_base::beg);
    next_position = value;

    write(next_position, 0);
    write<uint8_t>(1, sizeof(next_position) * 2);            // setting check byte to 1
    write(next_position, sizeof(next_position));
    write<uint8_t>(0);                                       // setting check byte back to 0

    file.sync();
}

Journal::Journal(const std::string & filename, bool create) {
    file.exceptions(std::ios_base::badbit | std::ios_base::failbit);
    if (create) {
        try {
            system(("touch " + journalName(filename)).c_str());
            file.open(journalName(filename), std::ios_base::in | std::ios_base::out);
            next_position = 2 * sizeof(size_t) + 1;

            write(next_position, 0);
            write(next_position);
            write<uint8_t>(0);                              // setting check byte to 0

            file.sync();
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

Journal::Journal(const std::string & restoring_file) {      // constructor which restores given file according to the journal
    std::fstream recovering_file;
    recovering_file.exceptions(std::ios_base::badbit | std::ios_base::failbit);
    try {
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
        read(position);
        read(data_size);

        std::vector<uint8_t> data(data_size);
        read(data[0], -1, data_size);

        recovering_file.seekp(position);
        recovering_file.write(reinterpret_cast<char *>(&data[0]), data_size);
        recovering_file.sync();
    }
    writeNextPosition(sizeof(size_t) * 2 + 1);                  // we have restored everything,
                                                                // lets reset journal back to empty
}

void Journal::journalize(size_t beg, char * const data, size_t size) {
    assert(file.is_open());
    write(beg, next_position);
    write(size);
    write(*data, -1, size);
    file.sync();
    writeNextPosition(file.tellp());                            // so we change the next position pointer
                                                                // only if we have successfully wrote the journal record
}

void Journal::deleteJournal(const std::string & filename) {
    system(("rm -f " + journalName(filename) + "\n").c_str());
}

std::string Journal::journalName(const std::string & filename) {
    return filename + ".journal";
}