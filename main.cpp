#include <stdio.h>
#include <time.h>
#include "aids.hpp"

using namespace aids;

struct Donation {
   struct tm t{};
   String_View desc{};
};

void print1(FILE *stream, Donation don) {
    print(stream, '{');

    char buf[255];
    strftime(buf, sizeof(buf), "%d %b %Y", &don.t);
    print(stream, buf);
    print(stream, ", ");
    print(stream, don.desc);
    print(stream, '}');
    print(stream, '\n');
}

template <typename T>
void print1(FILE *stream, Dynamic_Array<T> d) {
    for (size_t i{}; i < d.size; ++i) {
        print(stream, i+1, ". ", d.data[i]);
    } 
}

void sanity_check(Dynamic_Array<Donation> d) {
    long long max{};
    for (size_t i{}; i < d.size; ++i) {
        char buf[255];
        strftime(buf, sizeof(buf), "%s", &d.data[i].t);
        long long curr = atoll(buf);
        if (curr < max) {
            panic("Donations are not sorted");
        }
        max = curr;
    }  
}

Dynamic_Array<Donation> parse_db_file(String_View db_file) {
    Dynamic_Array<Donation> donations{};
    while (db_file.count > 0) {
        auto line = db_file.chop_by_delim('\n');
        line = line.trim();
        if (line.count == 0) {
            continue;
        }
        auto date = line.chop_word();
        line = line.trim();
        auto desc = line.chop_word();

        struct tm tm;

        memset(&tm, 0, sizeof(struct tm)); 
        char buf0[255];
        memcpy(buf0, date.data, date.count);
        strptime(buf0, "%Y-%m-%d", &tm);

        donations.push({tm, desc});
    }
    return donations;
}

int main() {
    auto db_file = read_file_as_string_view("db.txt").value_or({});

    auto donations = parse_db_file(db_file);
    sanity_check(donations);
    println(stdout, donations);
}
