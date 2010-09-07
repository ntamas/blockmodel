/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef IGRAPHPP_EDGE_SELECTOR_H
#define IGRAPHPP_EDGE_SELECTOR_H

#include <igraph/cpp/types.h>
#include <igraph/cpp/vector.h>

namespace igraph {

/// C++-style wrapper around an igraph_es_t
class EdgeSelector {
private:
    /// The igraph_es_t instance encapsulated by the wrapper
    igraph_es_t m_es;

public:
    /*****************************/
    /* Constructors, destructors */
    /*****************************/

    /// Constructs an empty edge selector
    EdgeSelector() {
        IGRAPH_TRY(igraph_es_none(&m_es));
    }

    /// Constructs an edge selector that selects a single edge
    EdgeSelector(integer_t eid) {
        IGRAPH_TRY(igraph_es_1(&m_es, eid));
    }

    /// Constructs an edge selector t hat handles a vector as an edge selector
    EdgeSelector(Vector* vector) {
        IGRAPH_TRY(igraph_es_vector(&m_es, vector->c_vector()));
    }

    /// Constructs a wrapper that wraps the given igraph_es_t instance
    /**
     * The ownership of the wrapped instance is stolen by the wrapper.
     * The caller should not destroy the graph on its own, ever;
     * the wrapper should be destroyed instead.
     */
    EdgeSelector(igraph_es_t es) : m_es(es) {}

    /// Destroys the edge selector
    ~EdgeSelector() {
        igraph_es_destroy(&m_es);
    }

    /******************/
    /* Static methods */
    /******************/

    /// Creates an edge selector from multiple edges defined by their endpoints
    static EdgeSelector Pairs(const Vector& vector, bool directed=true) {
        igraph_es_t es;
        IGRAPH_TRY(igraph_es_pairs(&es, vector.c_vector(), directed));
        return EdgeSelector(es);
    }

    /********************/
    /* Instance methods */
    /********************/

    /// Returns a pointer to the encapsulated igraph_es_t instance
    igraph_es_t* c_es() {
        return &m_es;
    }

    /// Returns a pointer to the encapsulated igraph_es_t instance (const)
    const igraph_es_t* c_es() const {
        return &m_es;
    }

    /// Checks whether an edge selector includes all edges
    bool is_all() {
        return igraph_es_is_all(&m_es);
    }

    /*************/
    /* Operators */
    /*************/

    /*****************/
    /* Private stuff */
    /*****************/

private:
    /// Assignment operator (intentionally unimplemented)
    EdgeSelector& operator=(const EdgeSelector&);
};

}       // end of namespaces

#endif  // IGRAPHPP_EDGE_SELECTOR_H

