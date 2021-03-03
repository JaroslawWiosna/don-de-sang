#ifndef DATE_HPP_
#define DATE_HPP_

#include "3rd_party/aids-patched.hpp"
#include <ctime>

//TODO(#1): using namespace in header file is not smart
using namespace aids;

struct Date {
    struct tm t{};
};

void print1(FILE *stream, Date date) {
    print(stream, '{',
        "tm_sec == ", date.t.tm_sec,
        ", tm_min == ", date.t.tm_min,
        ", tm_hour == ", date.t.tm_hour,
        ", tm_mday == ", date.t.tm_mday,
        ", tm_mon == ", date.t.tm_mon,
        ", tm_year == ", date.t.tm_year,
        ", tm_wday == ", date.t.tm_wday,
        ", tm_yday == ", date.t.tm_yday,
        ", tm_isdst == ", date.t.tm_isdst,
        ", tm_gmtoff == ", date.t.tm_gmtoff, 
        ", tm_zone == ", date.t.tm_zone,
        '}');
}

bool operator==(Date lhs, Date rhs) {
    return (lhs.t.tm_year == rhs.t.tm_year) 
        && (lhs.t.tm_mon == rhs.t.tm_mon) 
        && (lhs.t.tm_mday == rhs.t.tm_mday);
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
        const int RES_CAPACITY{256};
        char *res = (char*)malloc(RES_CAPACITY);
        if (years > 0) {
            snprintf(res, RES_CAPACITY, "%d years and %lld days", years, days);
        } else {
            snprintf(res, RES_CAPACITY, "%lld days", days);
        }
        return res;
    }
};

Duration operator-(struct tm end, struct tm begin){
    auto b = mktime(&begin);
    auto e = mktime(&end);

    return {(long long)difftime(e, b)};
}

Duration operator-(Date end, Date begin){
    return end.t - begin.t;
}

Duration operator ""_day(unsigned long long int i) {
    return {60 * 60 * 24 * (long long int)i};    
}

Duration operator ""_week(unsigned long long int i) {
    return {60 * 60 * 24 * 7 * (long long int)i};    
}

Date operator+(Date date, Duration duration) {
    auto tm = mktime(&date.t);
    tm += duration.dur;
    struct tm *lt = localtime(&tm);
    struct tm result{};
    memcpy(&result, lt, sizeof(result));
    return {result};
}

#endif // DATE_HPP_
