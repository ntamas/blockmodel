/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef IGRAPHPP_GRAPH_H
#define IGRAPHPP_GRAPH_H

#include <cassert>
#include <igraph/igraph_constructors.h>
#include <igraph/igraph_datatype.h>
#include <igraph/igraph_interface.h>
#include <igraph/cpp/edge_selector.h>
#include <stdexcept>
#include <vector>

#include <igraph/cpp/types.h>
#include <igraph/cpp/vector.h>

namespace igraph {

/// C++-style wrapper around an igraph_t object
class Graph {
private:
    /// The igraph_t instance encapsulated by the wrapper
    igraph_t* m_pGraph;

public:
    /*****************************/
    /* Constructors, destructors */
    /*****************************/

    /// Constructs an empty graph
    Graph(long numVertices = 0, bool directed = false) : m_pGraph(0) {
        m_pGraph = new igraph_t;
        IGRAPH_TRY(igraph_empty(m_pGraph, numVertices, directed));
    }
    
    /// Constructs a wrapper that wraps the given igraph_t instance
    /**
     * The ownership of the wrapped instance is stolen by the wrapper.
     * The caller should not destroy the graph on its own, ever;
     * the wrapper should be destroyed instead.
     *
     * This function never throws an exception.
     */
    Graph(igraph_t* graph) throw() : m_pGraph(graph) {
        assert(graph);
    }

    /// Copy constructor
    Graph(const Graph& other) : m_pGraph(0) {
        m_pGraph = new igraph_t;
        IGRAPH_TRY(igraph_copy(m_pGraph, other.m_pGraph));
    }

    /// Destroys the graph
    ~Graph() {
        if (m_pGraph) {
            igraph_destroy(m_pGraph);
            delete m_pGraph;
        }
    }

    /******************/
    /* Static methods */
    /******************/

    /// Constructs a famous graph by name
    static Graph Famous(const std::string& name);

    /// Constructs a full graph
    static Graph Full(integer_t n, bool directed=false, bool loops=false);

    /// Constructs a geometric random graph
    static Graph GRG(integer_t nodes, real_t radius, bool torus=false,
            Vector* x=0, Vector* y=0);

    /// Reads a graph from an edge list file
    static Graph ReadEdgelist(FILE* instream, integer_t n=0,
            bool directed=false);

    /// Constructs a ring graph
    static Graph Ring(integer_t n, bool directed=false, bool mutual=true,
            bool circular=true);

    /********************/
    /* Instance methods */
    /********************/

    /// Adds a single edge to the graph
    void addEdge(integer_t source, integer_t target);

    /// Adds a list of edges to the graph
    void addEdges(const std::vector<integer_t>& edges);

    /// Adds a single vertex to the graph
    void addVertex();

    /// Adds the given number of vertices to the graph
    void addVertices(long numVertices);

    /// Deletes some edges from the graph
    void deleteEdges(const EdgeSelector& es) {
        IGRAPH_TRY(igraph_delete_edges(m_pGraph, *es.c_es()));
    }

    /// Returns the number of edges in the graph
    integer_t ecount() {
        return igraph_ecount(m_pGraph);
    }

    /// Returns the edge list of the graph
    Vector getEdgelist(bool bycol=false);
    /// Returns the edge list of the graph
    void getEdgelist(Vector* result, bool bycol=false);

    /// Returns the neighbors of a vertex
    void neighbors(Vector* result, long int vertex, NeighborMode mode = IGRAPH_OUT);
    /// Returns the neighbors of a vertex
    Vector neighbors(long int vertex, NeighborMode mode = IGRAPH_OUT);

    /// Returns the number of vertices in the graph
    integer_t vcount() {
        return igraph_vcount(m_pGraph);
    }

    /*************/
    /* Operators */
    /*************/

    /// Disjoint union of two graphs
    Graph operator+(const Graph& other) const;

    /// Assignment operator
    Graph& operator=(const Graph& other) {
        if (&other == this)
            return *this;

        igraph_t new_graph;
        IGRAPH_TRY(igraph_copy(&new_graph, other.m_pGraph));

        if (m_pGraph) {
            igraph_destroy(m_pGraph);
        } else {
            m_pGraph = new igraph_t;
        }
        *m_pGraph = new_graph;

        return *this;
    }
};

}       // end of namespaces

#endif  // IGRAPHPP_GRAPH_H
