/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef IGRAPHPP_VECTOR_H
#define IGRAPHPP_VECTOR_H

#include <igraph/cpp/types.h>

namespace igraph {

/// C++-style wrapper around an igraph_vector_t
class Vector {
public:
    typedef igraph_real_t* iterator;
    typedef const igraph_real_t* const_iterator;

private:
    /// The igraph_vector_t instance encapsulated by the wrapper
    igraph_vector_t m_vector;

public:
    /*****************************/
    /* Constructors, destructors */
    /*****************************/

    /// Constructs an empty vector
    Vector(long length = 0) {
        IGRAPH_TRY(igraph_vector_init(&m_vector, length));
    }

    /// Copy constructor
    Vector(const Vector& other) {
        IGRAPH_TRY(igraph_vector_copy(&m_vector, &other.m_vector));
    }

    /// Destroys the vector
    ~Vector() {
        igraph_vector_destroy(&m_vector);
    }

    /********************/
    /* Instance methods */
    /********************/

    /// Returns an iterator pointing to the first element of the vector
    iterator begin() {
        return &(VECTOR(m_vector)[0]);
    }

    /// Returns an iterator pointing to the first element of the vector (const)
    const_iterator begin() const {
        return &(VECTOR(m_vector)[0]);
    }

    /// Returns a const pointer to the internal igraph_vector_t
    const igraph_vector_t* c_vector() const {
        return &m_vector;
    }

    /// Returns a pointer to the internal igraph_vector_t
    igraph_vector_t* c_vector() {
        return &m_vector;
    }

    /// Returns an iterator pointing after the last element of the vector
    iterator end() {
        return &(VECTOR(m_vector)[size()]);
    }

    /// Returns an iterator pointing after the last element of the vector (const)
    const_iterator end() const {
        return &(VECTOR(m_vector)[size()]);
    }

    /// Fills the vector with the given item
    void fill(igraph_real_t element) {
        igraph_vector_fill(&m_vector, element);
    }

    /// Prints the vector to the standard output
    void print() const {
        igraph_vector_print(&m_vector);
    }

    /// Returns the size of the vector
    size_t size() const {
        return igraph_vector_size(&m_vector);
    }

    /*************/
    /* Operators */
    /*************/

    /// Assignment operator: copies the given vector to this one
    Vector& operator=(const Vector& other) {
        IGRAPH_TRY(igraph_vector_update(&m_vector, &other.m_vector));
        return *this;
    }

    /// Equality check: returns true if the two vectors are equal
    bool operator==(const Vector& other) const {
        return igraph_vector_is_equal(&m_vector, &other.m_vector);
    }

    /// Returns the element with the given index
    igraph_real_t& operator[](long int index) {
        return VECTOR(m_vector)[index];
    }

    /// Returns the element with the given index (const variant)
    igraph_real_t operator[](long int index) const {
        return VECTOR(m_vector)[index];
    }
};

}       // end of namespaces

#endif  // IGRAPHPP_VECTOR_H

