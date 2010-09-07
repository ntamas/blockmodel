#include <cmath>
#include <iostream>

static int test_counter = 0;
#define CHECK(func) {     \
    test_counter++;       \
    int _retval = func(); \
    if (_retval) {        \
        std::cout << '#' << test_counter << ' ' <<   \
               #func << " failed (return code = " << \
               _retval << ")\n";                     \
        return _retval;   \
    } \
}

#define ALMOST_EQUALS(x, y, eps) (std::fabs((x) - (y)) <= (eps))

