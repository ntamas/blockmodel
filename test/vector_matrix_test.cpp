/* vim:set ts=4 sw=4 sts=4 et: */

#include <igraph/cpp/matrix.h>
#include <igraph/cpp/vector.h>

#include "test_common.cpp"

using namespace igraph;

int test_matrix_vector_multiplication() {
    igraph_real_t vdata[] = {1, 2, 3, 4};
    Vector v(4, vdata);
    igraph_real_t mdata[] = {
        16,  5,  9,  4,
         2, 11,  7, 14,
         3, 10,  6, 15,
        13,  8, 12,  1
    }; // this is the TRANSPOSED matrix!
    Matrix m(4, 4);
    m = mdata;

    /* vector * matrix */
    {
        igraph_real_t expdata[] = {69, 101, 101, 69};
        Vector exp(4, expdata);
        if (exp != v * m)
            return 1;
    }

    /* matrix * vector */
    {
        igraph_real_t expdata[] = {81, 89, 89, 81};
        Vector exp(4, expdata);
        if (exp != m * v)
            return 2;
    }

    /* vector *= const */
    {
        igraph_real_t expdata[] = {2, 4, 6, 8};
        Vector exp(4, expdata);
        v *= 2;
        if (exp != v)
            return 3;
    }

    /* vector * const */
    {
        igraph_real_t expdata[] = {1, 2, 3, 4};
        Vector exp(4, expdata);
        if (exp != v * 0.5)
            return 4;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    CHECK(test_matrix_vector_multiplication);
}

