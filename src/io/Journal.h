#ifndef TERM_PROJECT_JOURNAL_H
#define TERM_PROJECT_JOURNAL_H


#include <fstream>
#include <vector>

class Journal {
private:
    std::fstream file;
    size_t next_position = -1;
    template <typename T>
    void read(T & obj, size_t pos = -1, size_t size = sizeof(T)) {
        if (pos != -1) file.seekg(pos, std::ios_base::beg);
        file.read(reinterpret_cast<char *>(&obj), size);
    }

    template <typename T>
    void write(const T & obj, size_t pos = -1, size_t size = sizeof(T)) {
        if (pos != -1) file.seekp(pos, std::ios_base::beg);
        file.write(reinterpret_cast<const char *>(&obj), size);
    }

    static std::string journalName(const std::string & filename);
    void readNextPosition();
    void writeNextPosition(size_t value);
public:
    Journal(const std::string & filename, bool create);
    Journal(const std::string & restoring_file);

    void journalize(size_t beg, char * const data, size_t size);
    static void deleteJournal(const std::string & filename);
};


#endif //TERM_PROJECT_JOURNAL_H
