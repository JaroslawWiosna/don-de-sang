#ifndef DATE_HPP_
#define DATE_HPP_

#include "3rd_party/aids-patched.hpp"
#include <time.h>

//TODO(#1): using namespace in header file is not smart
using namespace aids;

struct Date {
    struct tm t{};
};

bool operator==(Date lhs, Date rhs) {
    return (0 == memcmp((void*)&lhs.t, (void*)&rhs.t, sizeof(lhs.t)));
}

// NOTE: issue `man 1 date` and find `iso-8601` for details
Date iso8601(String_View sv) {
    struct tm t{};
    memset(&t, 0, sizeof(struct tm)); 
    char buf0[255];
    memcpy(buf0, sv.data, sv.count);
    strptime(buf0, "%Y-%m-%d", &t);
    return {t};
}

struct Duration {
    long long dur{};

    char *c_str() {
        long long days = dur / 86400;
        int years = days / 365;
        days = days % 365;
        char *res = (char*)malloc(256);
        sprintf(res, "%d years and %lld days", years, days);
        return res;
    }
};

Duration operator-(const struct tm end, const struct tm begin){
    char buf1[255];
    strftime(buf1, sizeof(buf1), "%s", &begin);

    char buf2[255];
    strftime(buf2, sizeof(buf2), "%s", &end);
        
    return {atoll(buf2) - atoll(buf1)};
}

Duration operator-(Date end, Date begin){
    return end.t - begin.t;
}

Duration operator ""_day(unsigned long long int i) {
    return {60 * 60 * 24 * (long long int)i};    
}

Date operator+(Date date, Duration duration) {
    char buf[255];
    strftime(buf, sizeof(buf), "%s", &date.t);
    long long res = atoll(buf) + duration.dur;

    struct tm t{};
    memset(&t, 0, sizeof(struct tm)); 
    char buf0[255];
    sprintf(buf0, "%lld", res);
    strptime(buf0, "%s", &t);
    return {t};
}

#endif // DATE_HPP_
