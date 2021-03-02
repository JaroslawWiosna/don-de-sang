#include "3rd_party/aids-patched.hpp"
#include "date.hpp"

#include <stdio.h>

using namespace aids;

enum class Gender {
    Unknown,
    Female,
    Male,
};

struct Donation {
   Gender gender{Gender::Unknown};
   struct tm t{};
   unsigned int amount{450};
   unsigned int sum{};
};

void print1(FILE *stream, Donation don) {
    print(stream, '{');

    char buf[255] = {};
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
    Gender gender{Gender::Unknown};
    while (db_file.count > 0) {
        auto line = db_file.chop_by_delim('\n');
        line = line.trim();
        if (line.count == 0) {
            continue;
        }
        if ("male"_sv == line) {
            gender = Gender::Male;
            continue;
        } else if ("female"_sv == line) {
            gender = Gender::Female;
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

        donations.push(Donation{gender, tm, amount, {}});
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
    Treshold thd_female[] = {{5000}, {10000}, {15000}};
    Treshold thd_male[] = {{6000}, {12000}, {18000}};
    constexpr size_t thd_female_size = (sizeof(thd_female) / sizeof(thd_female[0]));
    static_assert(3 == thd_female_size);
    constexpr size_t thd_male_size = (sizeof(thd_male) / sizeof(thd_male[0]));
    static_assert(3 == thd_male_size);

    static_assert(thd_female_size == thd_male_size);
    constexpr size_t thd_size = thd_female_size; // or thd_male_size, whatever

    Treshold (*thd)[thd_size] = nullptr;
    
    for (size_t i{}; i < d.size; ++i) {
        if (Gender::Female == d.data[i].gender) {
            thd = &thd_female;
        } else if (Gender::Male == d.data[i].gender) {
            thd = &thd_male;
        } 
        print(stdout, i+1, ". ", d.data[i]);
        for (size_t j{}; j < thd_size; ++j) {
            if (!thd[j]->passed && d.data[i].sum >= thd[j]->thr) {
                thd[j]->origin = &d.data[0].t;
                thd[j]->last = &d.data[i].t;
                print(stdout, 
                        thd[j]->thr, 
                        " treshold reached! It took ", 
                        operator-(*thd[j]->last, *thd[j]->origin).c_str());
                thd[j]->passed = true;
            
                print(stdout, '\n');
            }
        }
    }
}

#define ASSERT_EQ_STR(expected, actual)                     \
    do {                                                    \
        if (strcmp(expected, actual)) {                     \
            panic("ERROR: expected ", expected              \
                "\n       actual   ", actual, '\n');        \
        }                                                   \
    } while(0);

#define EXPECT_EQ_STR(expected, actual)                     \
    do {                                                    \
        if (strcmp(expected, actual)) {                     \
            print(stderr, "ERROR: expected ", expected      \
                "\n       actual   ", actual, '\n');        \
        }                                                   \
    } while(0);

int tests() {
    Date begin = iso8601("1970-01-02"_sv);
    Date end = iso8601("1970-01-03"_sv);

    Duration period = end - begin;
    puts(period.c_str());
    EXPECT_EQ_STR("1 days", period.c_str());

    {
        auto expected = iso8601("1970-01-09"_sv);
        auto actual = (begin + 1_week);
        {
            Duration period = expected - begin;
            puts(period.c_str());
        }
        {
            Duration period = actual - begin;
            puts(period.c_str());
        }
        assert(expected == actual);
    }
    return 0;
}

int main() {
    tests();
    auto db_file = read_file_as_string_view("db.txt").value_or();
    auto dons = parse_db_file(db_file);
    sanity_check(dons);
    sum(dons);

    summary(dons);
    return 0;
}
