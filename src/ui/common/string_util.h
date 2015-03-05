/* vim:set ts=4 sw=4 sts=4 et: */

#include <cstdarg>
#include <string>

#ifndef _STRING_UTIL_H

class StringUtil {
public:
    /// Safer, string-based version of sprintf
    static std::string format(const char* fmt, ...);

    /// Safer, string-based version of vsprintf
    static std::string vformat(const char* fmt, va_list ap);
};

#endif    // _STRING_UTIL_H
