/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef IGRAPHPP_STR_VECTOR_H
#define IGRAPHPP_STR_VECTOR_H

#include <iostream>
#include <string>
#include <igraph/cpp/error.h>
#include <igraph/cpp/types.h>

namespace igraph {

/// C++-style wrapper around an igraph_strvector_t
class StrVector {
public:
    typedef char*& reference;
    typedef char* const& const_reference;
    typedef char** iterator;
    typedef char** const const_iterator;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef const char* value_type;
    typedef char** pointer;

private:
    /// The igraph_strvector_t instance encapsulated by the wrapper
    igraph_strvector_t m_strvector;

    /// Whether we own the given igraph_strvector_t instance or not
    bool m_owner;

public:
    /*****************************/
    /* Constructors, destructors */
    /*****************************/

    /// Constructs a string vector
    explicit StrVector(long int length = 0) : m_owner(true) {
        IGRAPH_TRY(igraph_strvector_init(&m_strvector, length));
    }

    /// Constructs a wrapper that wraps the given \c igraph_strvector_t instance
    /**
     * \param  own  specifies whether we own the given \c igraph_strvector_t instance
     *              or not. If own is false, the encapsulated \c igraph_strvector_t
     *              will not be destroyed when the \c Vector wrapper is destroyed.
     */
    StrVector(igraph_strvector_t vector, bool own=true) : m_strvector(vector), m_owner(own) {}

    /// Constructs a wrapper that refers to the same vector as the given igraph_strvector_t* pointer
    StrVector(igraph_strvector_t* vector, bool own=false) : m_strvector(*vector), m_owner(own) {}

    /// Copy constructor
    StrVector(const StrVector& other) : m_owner(true) {
        IGRAPH_TRY(igraph_strvector_copy(&m_strvector, &other.m_strvector));
    }

    /// Destroys the vector
    ~StrVector() {
        if (m_owner)
            igraph_strvector_destroy(&m_strvector);
    }

    /********************/
    /* Instance methods */
    /********************/

    /// Adds a new element to the end of the vector
    void add(const char* value) {
        IGRAPH_TRY(igraph_strvector_add(&m_strvector, value));
    }

    /// Adds a new element to the end of the vector
    void add(const std::string& value) {
        add(value.c_str());
    }

    /// Appends another string vector to this one
    void append(const StrVector& from) {
        IGRAPH_TRY(igraph_strvector_append(&m_strvector, &from.m_strvector));
    }

    /// Returns an iterator pointing to the first element of the vector
    iterator begin() {
        return m_strvector.data;
    }

    /// Returns an iterator pointing to the first element of the vector (const)
    const_iterator begin() const {
        return m_strvector.data;
    }

    /// Removes all the elements from the string vector
    /**
     * Note that this function sets the length of the vector to zero, but it
     * does not allocate previously used memory -- it is still kept in case
     * the vector has to grow again.
     */
    void clear() {
        igraph_strvector_clear(&m_strvector);
    }

    /// Returns a pointer to the internal igraph_strvector_t
    igraph_strvector_t* c_strvector() {
        return &m_strvector;
    }

    /// Returns a const pointer to the internal igraph_strvector_t
    const igraph_strvector_t* c_strvector() const {
        return &m_strvector;
    }

    /// Returns whether the vector is empty
    bool empty() const {
        return size() == 0;
    }

    /// Returns an iterator pointing after the last element of the vector
    iterator end() {
        return m_strvector.data + size();
    }

    /// Returns an iterator pointing after the last element of the vector (const)
    const_iterator end() const {
        return m_strvector.data + size();
    }

    /// Returns an element of the vector
    const char* get(long int idx) {
        char* result;
        igraph_strvector_get(&m_strvector, idx, &result);
        return result;
    }

    /// Prints the vector to the standard output
    void print() const {
        for (iterator it = begin(); it != end(); it++)
            std::cout << *it << '\n';
    }

    /// Removes the given element from the vector
    void remove(long int elem) {
        igraph_strvector_remove(&m_strvector, elem);
    }

    /// Resizes the vector
    void resize(long int newsize) {
        IGRAPH_TRY(igraph_strvector_resize(&m_strvector, newsize));
    }

    /// Sets an element of the vector
    void set(long int idx, const char* value) {
        IGRAPH_TRY(igraph_strvector_set(&m_strvector, idx, value));
    }

    /// Sets an element of the vector
    void set(long int idx, const std::string& value) {
        set(idx, value.c_str());
    }

    /// Returns the size of the vector
    size_t size() const {
        return igraph_strvector_size(&m_strvector);
    }

    /*************/
    /* Operators */
    /*************/

    /// Assignment operator; intentionally left unimplemented (yet)
    StrVector& operator=(const StrVector& other);

    /// Returns the element with the given index
    value_type operator[](long int index) {
        return STR(m_strvector, index);
    }
};

}       // end of namespaces

#endif  // IGRAPHPP_VECTOR_H

