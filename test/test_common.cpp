#include <algorithm>
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

template <typename T>
int ALMOST_EQUALS_VECTOR(const T& vec1, const T& vec2, double eps) {
    T diff = vec1 - vec2;
    std::transform(diff.begin(), diff.end(), std::fabs);
    return (diff.max() <= eps);
}

