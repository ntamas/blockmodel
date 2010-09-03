/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef BLOCKMODEL_VECTOR_HPP
#define BLOCKMODEL_VECTOR_HPP

#include <cassert>
#include <cstring>

/// Templatized general dense vector class
template <class T>
class Vector {
private:
    /// Storage area
    T* m_data;

    /// Number of elements
    int m_length;

public:
    /// Constructor that creates a null vector of the given size
    Vector(int length = 0) :
        m_data(0), m_length(length) {
        m_data = new T[m_length];
    }

    /// Copy constructor
    Vector(const Vector& other) :
        m_data(0), m_length(other.m_length) {
        m_data = new T[m_length];
        memcpy(m_data, other.m_data, numBytes());
    }

    /// Destructor
    ~Vector() {
        if (m_data)
            delete[] m_data;
    }

    /// Returns the number of bytes occupied by this vector
    size_t numBytes() const {
        return m_length * sizeof(T);
    }

    /// Returns the length of this vector
    size_t size() const {
        return m_length;
    }

    /// Returns the element at the given index of the vector
    T& operator()(int index) {
        assert(m_data);
        assert(index >= 0 && index < m_length);
        return m_data[index];
    }
};

/*************************************************************************/

#endif

