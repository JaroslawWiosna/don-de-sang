#ifndef DATE_HPP_
#define DATE_HPP_

#include "3rd_party/aids-patched.hpp"
#include <time.h>

//TODO(#1): using namespace in header file is not smart
using namespace aids;

struct Date {
    String_View sv{};
};

// NOTE: issue `man 1 date` and find `iso-8601` for details
Date load_date_from_iso8601(String_View sv) {
    Date res{};
    res.sv = sv;
    return res;
}

struct Duration {
    long long dur{};

    char *c_str() {
        char *res = (char*)malloc(256);
        sprintf(res, "%lld days", dur / 86400);
        return res;
    }
};

Duration operator-(Date end, Date begin){
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm)); 
    char buf0[255];
    memcpy(buf0, begin.sv.data, begin.sv.count);
    strptime(buf0, "%Y-%m-%d", &tm);

    char buf1[255];
    strftime(buf1, sizeof(buf1), "%s", &tm);

    memcpy(buf0, end.sv.data, end.sv.count);
    strptime(buf0, "%Y-%m-%d", &tm);

    char buf2[255];
    strftime(buf2, sizeof(buf2), "%s", &tm);
        
    return {atoll(buf2) - atoll(buf1)};
}

Duration operator-(const struct tm end, const struct tm begin){
    char buf1[255];
    strftime(buf1, sizeof(buf1), "%s", &begin);

    char buf2[255];
    strftime(buf2, sizeof(buf2), "%s", &end);
        
    return {atoll(buf2) - atoll(buf1)};
}

#endif // DATE_HPP_
