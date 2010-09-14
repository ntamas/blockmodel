/* vim:set ts=4 sw=4 sts=4 et: */

#include <cmath>
#include <cstdlib>
#include <vector>
#include <block/math.hpp>
#include <mtwister/mt.h>

#include "test_common.cpp"

int test_moving_average() {
    MovingAverage<int> avg(200);
    MersenneTwister rng;

    std::vector<int> vec(1200);
    std::vector<int>::iterator it, it2;

    for (it = vec.begin() + 200; it != vec.end(); it++)
        *it = rng.randint(5000);

    for (it = vec.begin(), it2 = it + 200; it2 != vec.end(); it++, it2++) {
        double expected = std::accumulate(it, it2, 0) / 200.0;
        if (!ALMOST_EQUALS(expected, avg.value(), 1e-3))
            return 1;
        avg.push_back(*it2);
    }

    return 0;
}

int main(int argc, char* argv[]) {
    CHECK(test_moving_average);

    return 0;
}

