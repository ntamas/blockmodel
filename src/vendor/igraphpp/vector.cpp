/* vim:set ts=4 sw=4 sts=4 et: */

#include <igraph/cpp/matrix.h>
#include <igraph/cpp/vector.h>

namespace igraph {

Vector Vector::operator*(const Matrix& matrix) {
    Vector result(size());
    igraph_blas_dgemv(1, 1, matrix.c_matrix(), c_vector(),
            0, result.c_vector());
    return result;
}

Vector operator+(igraph_real_t plus, const Vector& vector) {
    return vector * plus;
}

Vector operator-(igraph_real_t minus, const Vector& vector) {
    return (-vector) + minus;
}

Vector operator*(igraph_real_t by, const Vector& vector) {
    return vector * by;
}

}         // end of namespaces
