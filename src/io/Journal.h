#ifndef TERM_PROJECT_JOURNAL_H
#define TERM_PROJECT_JOURNAL_H


#include <fstream>
#include <vector>

class Journal {
private:
    std::fstream file;
    size_t next_position = -1;


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
