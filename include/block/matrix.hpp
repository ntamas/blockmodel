/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef BLOCKMODEL_MATRIX_HPP
#define BLOCKMODEL_MATRIX_HPP

#include <cassert>
#include <cstring>
#include <numeric>
#include <vector>

/// Templatized general dense matrix class
template <class T>
class Matrix {
private:
    /// Storage area
    std::vector<T> m_data;

    /// Number of rows
    int m_numRows;

    /// Number of columns
    int m_numCols;

public:
    /*****************************/
    /* Constructors, destructors */
    /*****************************/

    /// Constructor that creates a zero matrix of the given size
    Matrix(int numRows = 0, int numCols = 0) :
        m_data(numRows * numCols), m_numRows(numRows), m_numCols(numCols) {
        m_data.resize(numRows * numCols);
    }

    /// Copy constructor
    Matrix(const Matrix& other) :
        m_data(other.m_data), m_numRows(other.m_numRows), m_numCols(other.m_numCols) {
    }

    /********************/
    /* Instance methods */
    /********************/

    /// Returns the number of bytes occupied by this matrix
    size_t numBytes() const {
        return m_numRows * m_numCols * sizeof(T);
    }

    /// Returns the number of columns in this matrix
    size_t numCols() const {
        return m_numCols;
    }

    /// Returns the number of rows in this matrix
    size_t numRows() const {
        return m_numRows;
    }

    /// Resizes the matrix to have the given number of rows and columns
    void resize(int numRows, int numCols) {
        m_data.resize(numRows * numCols);
    }

    /// Returns the total number of elements in this matrix
    size_t size() const {
        return m_numRows * m_numCols;
    }

    /// Returns the sum of all the elements in this matrix
    T sum() const {
        return std::accumulate(m_data.begin(), m_data.end(), 0);
    }

    /*************/
    /* Operators */
    /*************/

    /// Returns the element at the given row and column of the matrix (const)
    const T& operator()(int row, int col) const {
        assert(row >= 0 && row < m_numRows);
        assert(col >= 0 && col < m_numCols);
        return m_data[row * m_numCols + col];
    }

    /// Returns the element at the given row and column of the matrix
    T& operator()(int row, int col) {
        assert(row >= 0 && row < m_numRows);
        assert(col >= 0 && col < m_numCols);
        return m_data[row * m_numCols + col];
    }

    /// Checks two matrices for equality
    bool operator==(const Matrix& other) const {
        return m_data == other.m_data;
    }

    /// Checks two matrices for inequality
    bool operator!=(const Matrix& other) const {
        return m_data != other.m_data;
    }
};

/*************************************************************************/

#endif
