#include <stdio.h>
#include "aids.hpp"

using namespace aids;

void parse_db(String_View db) {
    while (db.count > 0) {
        auto line = db.chop_by_delim('\n');
        line = line.trim();
        if (line.count == 0) {
            continue;
        }
        println(stdout, "\t|\t", line, "\t|\t");
    }
}

int main() {
    auto db = read_file_as_string_view("db.txt").value_or({});

    parse_db(db);
}
