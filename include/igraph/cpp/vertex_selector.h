/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef IGRAPHPP_VERTEX_SELECTOR_H
#define IGRAPHPP_VERTEX_SELECTOR_H

#include <igraph/cpp/types.h>
#include <igraph/cpp/vector.h>

namespace igraph {

/// C++-style wrapper around an igraph_vs_t
class VertexSelector {
private:
    /// The igraph_vs_t instance encapsulated by the wrapper
    igraph_vs_t m_vs;

public:
    /*****************************/
    /* Constructors, destructors */
    /*****************************/

    /// Constructs an empty vertex selector
    VertexSelector() {
        IGRAPH_TRY(igraph_vs_none(&m_vs));
    }

    /// Constructs a vertex selector that selects a single vertex
    VertexSelector(integer_t vid) {
        IGRAPH_TRY(igraph_vs_1(&m_vs, vid));
    }

    /// Constructs a vertex selector that selects a range of vertices
    VertexSelector(integer_t from, integer_t to) {
        IGRAPH_TRY(igraph_vs_seq(&m_vs, from, to));
    }

    /// Constructs a vertex selector that handles a vector as a vertex selector
    VertexSelector(Vector* vector) {
        IGRAPH_TRY(igraph_vs_vector(&m_vs, vector->c_vector()));
    }

    /// Constructs a wrapper that wraps the given igraph_vs_t instance
    /**
     * The ownership of the wrapped instance is stolen by the wrapper.
     * The caller should not destroy the vertex selector on its own, ever;
     * the wrapper should be destroyed instead.
     */
    VertexSelector(igraph_vs_t vs) : m_vs(vs) {}

    /// Destroys the vertex selector
    ~VertexSelector() {
        igraph_vs_destroy(&m_vs);
    }

    /******************/
    /* Static methods */
    /******************/

    /// Creates a vertex selector that selects all vertices
    static VertexSelector All() {
        igraph_vs_t vs;
        IGRAPH_TRY(igraph_vs_all(&vs));
        return VertexSelector(vs);
    }

    /********************/
    /* Instance methods */
    /********************/

    /// Returns a pointer to the encapsulated igraph_vs_t instance
    igraph_vs_t* c_vs() {
        return &m_vs;
    }

    /// Returns a pointer to the encapsulated igraph_vs_t instance (const)
    const igraph_vs_t* c_vs() const {
        return &m_vs;
    }

    /*************/
    /* Operators */
    /*************/

    /*****************/
    /* Private stuff */
    /*****************/

private:
    /// Assignment operator (intentionally unimplemented)
    VertexSelector& operator=(const VertexSelector&);
};

}       // end of namespaces

#endif  // IGRAPHPP_VERTEX_SELECTOR_H

