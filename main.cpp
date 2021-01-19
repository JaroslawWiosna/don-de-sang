#include <stdio.h>
#include <time.h>
#include "aids.hpp"

using namespace aids;

struct Db {
   struct tm t{};
   String_View desc{};
};

void parse_db_file(String_View db_file) {
    Db db{};
    while (db_file.count > 0) {
        auto line = db_file.chop_by_delim('\n');
        line = line.trim();
        if (line.count == 0) {
            continue;
        }
        auto date = line.chop_word();
        line = line.trim();
        auto desc = line.chop_word();

        println(stdout, "\t|\t date: ", date, " ", "desc: ", desc, "\t|");
    }
}

int main() {
    auto db_file = read_file_as_string_view("db.txt").value_or({});

    parse_db_file(db_file);
}
