/* vim:set ts=4 sw=4 sts=4 et: */

#include <cstdio>
#include <vector>
#include "string_util.h"

using namespace std;

string StringUtil::format(const string& fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    string buf = vformat(fmt.c_str(), ap);
    va_end(ap);
    return buf;
}

string StringUtil::format(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    string buf = vformat(fmt, ap);
    va_end(ap);
    return buf;
}

string StringUtil::vformat(const char* fmt, va_list ap) {
    size_t size = 1024;
    char stackbuf[1024];
    vector<char> dynamicbuf;
    char *buf = &stackbuf[0];

    while (1) {
        int needed = vsnprintf(buf, size, fmt, ap);
        if (needed <= (int)size && needed >= 0)
            return string(buf, (size_t)needed);
        size = (needed > 0) ? (needed+1) : (size*2);
        dynamicbuf.resize(size);
        buf = &dynamicbuf[0];
    }
}

