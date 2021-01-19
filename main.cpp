#include <stdio.h>
#include <time.h>
#include "aids.hpp"

using namespace aids;

struct Donation {
   struct tm t{};
   unsigned int amount{450};
   unsigned int sum{};
};

void print1(FILE *stream, Donation don) {
    print(stream, '{');

    char buf[255];
    strftime(buf, sizeof(buf), "%d %b %Y", &don.t);
    print(stream, buf);
    print(stream, ", ");
    print(stream, don.amount);
    print(stream, ", ");
    print(stream, don.sum);
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
        desc = desc.trim();
        unsigned int amount{};
        if ("450ml"_sv == desc) {
            amount = 450;
        } else if ("KP"_sv == desc) {
            amount = 450;
        } else if ("sep"_sv == desc) {
            amount = 500;
        } else {
            panic("Unknown desc `", desc , '`');
        }

        struct tm tm;

        memset(&tm, 0, sizeof(struct tm)); 
        char buf0[255];
        memcpy(buf0, date.data, date.count);
        strptime(buf0, "%Y-%m-%d", &tm);

        donations.push(Donation{tm, amount, {}});
    }
    return donations;
}

void sum(Dynamic_Array<Donation> d) {
    unsigned int acc{};
    for (size_t i{}; i < d.size; ++i) {
        acc += d.data[i].amount;
        d.data[i].sum = acc;
    }
}

struct Treshold {
    unsigned int thr{};
    bool passed{};
    struct tm* origin{};
    struct tm* last{};

    long long diff() {
        char buf1[255];
        strftime(buf1, sizeof(buf1), "%s", origin);
        
        char buf2[255];
        strftime(buf2, sizeof(buf2), "%s", last);
        
        return atoll(buf2) - atoll(buf1);
    };
};

void summary(Dynamic_Array<Donation> d) {
    Treshold thd1{6000};
    Treshold thd2{12000};
    Treshold thd3{18000};
    for (size_t i{}; i < d.size; ++i) {
        print(stdout, i+1, ". ", d.data[i]);
        if (!thd1.passed && d.data[i].sum >= thd1.thr) {
            thd1.origin = &d.data[0].t;
            thd1.last = &d.data[i].t;
            print(stdout, thd1.thr, " treshold reached! It took ", (long)thd1.diff() / 86400, " days.");
            thd1.passed = true;
            
            print(stdout, '\n');
        }
        if (!thd2.passed && d.data[i].sum >= thd2.thr) {
            thd2.origin = &d.data[0].t;
            thd2.last = &d.data[i].t;
            print(stdout, thd2.thr, " treshold reached! It took ", (long)thd2.diff() / 86400, " days.");
            thd2.passed = true;
            
            print(stdout, '\n');
        }
        if (!thd3.passed && d.data[i].sum >= thd3.thr) {
            thd3.origin = &d.data[0].t;
            thd3.last = &d.data[i].t;
            print(stdout, thd3.thr, " treshold reached! It took ", (long)thd3.diff() / 86400, " days.");
            thd3.passed = true;
            
            print(stdout, '\n');
        }
    }
}

int main() {
    auto db_file = read_file_as_string_view("db.txt").value_or({});
    auto dons = parse_db_file(db_file);
    sanity_check(dons);
    sum(dons);

    summary(dons);
}
