//
// Created by annikura on 01.06.17.
//

#ifndef TERM_PROJECT_IOBASECLASS_H
#define TERM_PROJECT_IOBASECLASS_H

#include <cstdio>
#include <string>
#include <utility>
#include <ios>
#include <fstream>
#include <vector>

class IOBaseClass {
protected:
    class BaseGetter {
    protected:
        size_t first;
        std::vector<uint8_t> second;
    public:
        BaseGetter() : first(-1) { };
        BaseGetter(size_t position) : first(position) { };

        BaseGetter *operator->() {

            return this;
        }

        bool operator==(const BaseGetter & other) {
            return this->first == other.first;
        };
    };

    void init_file(std::string file) {
        std::fstream fs;
        fs.exceptions(std::ios_base::badbit | std::ios_base::failbit);

        fs.open(file, std::ios_base::out | std::ios_base::binary);
        size_t zero = 0;
        fs.write(reinterpret_cast<char *>(&zero), sizeof(zero));

        fs.sync();
        fs.close();
    }

private:
    std::string storage_name;
    std::string stack_name;
    std::string table_name;

public:
    IOBaseClass(std::string storage, std::string stack, std::string table) :
            storage_name(storage),
            stack_name(stack),
            table_name(table)
    {
        init_file(storage);
        init_file(stack);
        init_file(table);
    };

    BaseGetter find(size_t id);
    BaseGetter insert(std::pair<size_t, std::vector<uint8_t>>);
    BaseGetter erase(const BaseGetter & getter);

    BaseGetter end() { return  BaseGetter(); };
};

typename IOBaseClass::BaseGetter IOBaseClass::find(size_t id) {

}

typename IOBaseClass::BaseGetter IOBaseClass::insert(std::pair<size_t, std::vector<uint8_t>>) {

}

typename IOBaseClass::BaseGetter IOBaseClass::erase(const IOBaseClass::BaseGetter & getter) {

}

#endif //TERM_PROJECT_IOBASECLASS_H
