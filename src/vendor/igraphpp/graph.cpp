/* vim:set ts=4 sw=4 sts=4 et: */

#include <igraph/igraph_conversion.h>
#include <igraph/igraph_foreign.h>
#include <igraph/igraph_games.h>
#include <igraph/igraph_operators.h>
#include <igraph/cpp/graph.h>
#include <memory>

namespace igraph {

/******************/
/* Static methods */
/******************/

Graph Graph::Famous(const std::string& name) {
    std::auto_ptr<igraph_t> result(new igraph_t);
    IGRAPH_TRY(igraph_famous(result.get(), name.c_str()));
    return Graph(result.release());
}

Graph Graph::Full(integer_t nodes, bool directed, bool loops) {
    std::auto_ptr<igraph_t> result(new igraph_t);
    IGRAPH_TRY(igraph_full(result.get(), nodes, directed, loops));
    return Graph(result.release());
}

Graph Graph::GRG(integer_t nodes, real_t radius, bool torus,
        Vector* x, Vector* y) {
    std::auto_ptr<igraph_t> result(new igraph_t);
    IGRAPH_TRY(igraph_grg_game(result.get(), nodes, radius, torus,
                x->c_vector(), y->c_vector()));
    return Graph(result.release());
}

Graph Graph::ReadEdgelist(FILE* instream, integer_t n, bool directed) {
    std::auto_ptr<igraph_t> result(new igraph_t);
    IGRAPH_TRY(igraph_read_graph_edgelist(result.get(), instream, n, directed));
    return Graph(result.release());
}

Graph Graph::Ring(integer_t n, bool directed, bool mutual, bool circular) {
    std::auto_ptr<igraph_t> result(new igraph_t);
    IGRAPH_TRY(igraph_ring(result.get(), n, directed, mutual, circular));
    return Graph(result.release());
}

/********************/
/* Instance methods */
/********************/

void Graph::addEdge(igraph_integer_t source, igraph_integer_t target) {
    igraph_integer_t edge[2] = { source, target };
    addEdges(std::vector<igraph_integer_t>(edge, edge+2));
}

void Graph::addEdges(const std::vector<double>& edges) {
    igraph_vector_t v;

    if (edges.empty())
        return;

    igraph_vector_view(&v, &edges[0], edges.size());
    IGRAPH_TRY(igraph_add_edges(m_pGraph, &v, 0));
}

void Graph::addVertex() { addVertices(1); }

void Graph::addVertices(long numVertices) {
    assert(m_pGraph);
    IGRAPH_TRY(igraph_add_vertices(m_pGraph, numVertices, 0));
}

void Graph::getEdgelist(Vector* result, bool bycol) {
    IGRAPH_TRY(igraph_get_edgelist(m_pGraph, result->c_vector(), bycol));
}

Vector Graph::getEdgelist(bool bycol) {
    Vector result;
    getEdgelist(&result, bycol);
    return result;
}

void Graph::neighbors(Vector* result, long int vertex, NeighborMode mode) {
    IGRAPH_TRY(igraph_neighbors(m_pGraph, result->c_vector(), vertex, mode));
}

Vector Graph::neighbors(long int vertex, NeighborMode mode) {
    Vector result;
    neighbors(&result, vertex, mode);
    return result;
}

/*************/
/* Operators */
/*************/

Graph Graph::operator+(const Graph& other) const {
    std::auto_ptr<igraph_t> result(new igraph_t);
    IGRAPH_TRY(igraph_disjoint_union(result.get(), m_pGraph, other.m_pGraph));
    return Graph(result.release());
}

}         // end of namespaces

