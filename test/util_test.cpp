/* vim:set ts=4 sw=4 sts=4 et: */

#include <vector>
#include <block/util.hpp>

#include "test_common.cpp"

int test_rank_vector() {
    const int N = 2, M = 5;

    double data[][M] = {
        { 2, 4, 10, 6, 1 },
        { 2, 2, 2, 1, 1 }
    };
    double expected[][M] = {
        { 2, 3, 5, 4, 1 },
        { 4, 4, 4, 1.5, 1.5 }
    };

    for (int i = 0; i < N; i++) {
        std::vector<double> result(M);
        std::vector<double> expected_result(M);
        std::copy(expected[i], expected[i] + M, expected_result.begin());
        rank_vector(data[i], data[i] + M, result.begin());
        if (expected_result != result) {
            std::cout << "Observed: ";
            for (int j = 0; j < M; j++)
                std::cout << result[j] << ' ';
            std::cout << '\n' << "Expected: ";
            for (int j = 0; j < M; j++)
                std::cout << expected_result[j] << ' ';
            std::cout << '\n';
            return i+1;
        }
    }

    return 0;
}

int main(int argc, char* argv[]) {
    CHECK(test_rank_vector);
}
        

