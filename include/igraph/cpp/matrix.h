/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef IGRAPHPP_MATRIX_H
#define IGRAPHPP_MATRIX_H

#include <igraph/igraph_blas.h>
#include <igraph/cpp/types.h>

namespace igraph {

/// C++-style wrapper around an igraph_matrix_t
class Matrix {
public:
    typedef igraph_real_t* iterator;
    typedef const igraph_real_t* const_iterator;

private:
    /// The igraph_matrix_t instance encapsulated by the wrapper
    igraph_matrix_t m_matrix;

public:
    /*****************************/
    /* Constructors, destructors */
    /*****************************/

    /// Constructs an empty matrix
    Matrix(long nrow = 0, long ncol = 0) {
        IGRAPH_TRY(igraph_matrix_init(&m_matrix, nrow, ncol));
    }

    /// Copy constructor
    Matrix(const Matrix& other) {
        IGRAPH_TRY(igraph_matrix_copy(&m_matrix, &other.m_matrix));
    }

    /// Destroys the matrix
    ~Matrix() {
        igraph_matrix_destroy(&m_matrix);
    }

    /********************/
    /* Instance methods */
    /********************/

    /// Returns an iterator pointing to the first element of the matrix
    iterator begin() {
        return &(MATRIX(m_matrix, 0, 0));
    }

    /// Returns an iterator pointing to the first element of the matrix (const)
    const_iterator begin() const {
        return &(MATRIX(m_matrix, 0, 0));
    }

    /// Returns a const pointer to the internal igraph_matrix_t
    const igraph_matrix_t* c_matrix() const {
        return &m_matrix;
    }

    /// Returns a pointer to the internal igraph_matrix_t
    igraph_matrix_t* c_matrix() {
        return &m_matrix;
    }

    /// Returns an iterator pointing after the last element of the matrix
    iterator end() {
        return &(MATRIX(m_matrix, 0, 0)) + size();
    }

    /// Returns an iterator pointing after the last element of the matrix (const)
    const_iterator end() const {
        return &(MATRIX(m_matrix, 0, 0)) + size();
    }

    /// Fills the matrix with the given element
    void fill(igraph_real_t element) {
        igraph_matrix_fill(&m_matrix, element);
    }

    /// Returns the number of columns of the matrix
    size_t ncol() const {
        return igraph_matrix_ncol(&m_matrix);
    }

    /// Returns the number of rows of the matrix
    size_t nrow() const {
        return igraph_matrix_nrow(&m_matrix);
    }

    /// Prints the matrix to the standard output
    void print() const {
        igraph_matrix_print(&m_matrix);
    }

    /// Resizes the matrix
    void resize(long nrow, long ncol) {
        IGRAPH_TRY(igraph_matrix_resize(&m_matrix, nrow, ncol));
    }

    /// Returns the size of the matrix
    size_t size() const {
        return igraph_matrix_size(&m_matrix);
    }

    /// Returns the sum of the elements of the matrix
    igraph_real_t sum() const {
        return igraph_matrix_sum(&m_matrix);
    }

    /*************/
    /* Operators */
    /*************/

    /// Assignment operator: copies the given matrix to this one
    Matrix& operator=(const Matrix& other) {
        IGRAPH_TRY(igraph_matrix_update(&m_matrix, &other.m_matrix));
        return *this;
    }

    /// Equality check: returns true if the two matrices are equal
    bool operator==(const Matrix& other) const {
        return igraph_matrix_is_equal(&m_matrix, &other.m_matrix);
    }

    /// Inequality check: returns true if the two matrices are not equal
    bool operator!=(const Matrix& other) const {
        return !((*this) == other);
    }

    /// Returns the element with the given row and column indices
    igraph_real_t& operator()(long int ri, long int ci) {
        return MATRIX(m_matrix, ri, ci);
    }

    /// Returns the element with the given row and column indices (const variant)
    igraph_real_t& operator()(long int ri, long int ci) const {
        return MATRIX(m_matrix, ri, ci);
    }

    /// Multiplication by vector from right
    Vector operator*(const Vector& v) const {
        Vector result(v.size());
        igraph_blas_dgemv(0, 1, &m_matrix, v.c_vector(),
                0, result.c_vector());
        return result;
    }

    /// In-place addition with constant
    Matrix& operator+=(igraph_real_t plus) {
        igraph_matrix_add_constant(&m_matrix, plus);
        return *this;
    }

    /// In-place addition with matrix
    Matrix& operator+=(const Matrix& other) {
        IGRAPH_TRY(igraph_matrix_add(&m_matrix, &other.m_matrix));
        return *this;
    }

    /// In-place subtraction
    Matrix& operator-=(const Matrix& other) {
        IGRAPH_TRY(igraph_matrix_sub(&m_matrix, &other.m_matrix));
        return *this;
    }

    /// In-place scaling
    Matrix& operator*=(igraph_real_t by) {
        igraph_matrix_scale(&m_matrix, by);
        return *this;
    }

    /// In-place division
    Matrix& operator/=(igraph_real_t by) {
        igraph_matrix_scale(&m_matrix, 1.0/by);
        return *this;
    }
};

}       // end of namespaces

#endif  // IGRAPHPP_VECTOR_H

