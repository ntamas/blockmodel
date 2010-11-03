/* vim:set ts=4 sw=4 sts=4 et: */

#include <igraph/igraph_conversion.h>
#include <igraph/igraph_foreign.h>
#include <igraph/igraph_games.h>
#include <igraph/igraph_operators.h>
#include <igraph/igraph_structural.h>
#include <igraph/cpp/edge_selector.h>
#include <igraph/cpp/graph.h>
#include <igraph/cpp/vertex_selector.h>
#include <memory>

namespace igraph {

/// Destructor
Graph::~Graph() {
    if (m_pGraph) {
        igraph_destroy(m_pGraph);
        delete m_pGraph;
    }
}

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

Graph Graph::ReadNCOL(FILE* instream, bool names,
        AddWeights weights, bool directed) {
    std::auto_ptr<igraph_t> result(new igraph_t);
    IGRAPH_TRY(igraph_read_graph_ncol(result.get(), instream, 0, names, weights, directed));
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
    Vector edge(2);
    edge[0] = source; edge[1] = target;
    addEdges(edge);
}

void Graph::addEdges(const Vector& edges) {
    assert(m_pGraph);
    IGRAPH_TRY(igraph_add_edges(m_pGraph, edges.c_vector(), 0));
}

void Graph::addVertex() { addVertices(1); }

void Graph::addVertices(long numVertices) {
    assert(m_pGraph);
    IGRAPH_TRY(igraph_add_vertices(m_pGraph, numVertices, 0));
}

Vector Graph::degree(const VertexSelector& vids, NeighborMode mode, bool loops) const {
    Vector result;
    degree(&result, vids, mode, loops);
    return result;
}

void Graph::degree(Vector* result, const VertexSelector& vids,
                   NeighborMode mode, bool loops) const {
    assert(m_pGraph);
    IGRAPH_TRY(igraph_degree(m_pGraph, result->c_vector(), *vids.c_vs(),
                mode, loops));
}

void Graph::deleteEdges(const EdgeSelector& es) {
    assert(m_pGraph);
    IGRAPH_TRY(igraph_delete_edges(m_pGraph, *es.c_es()));
}

any Graph::getAttribute(const std::string& attribute) const {
    return getAttributeHolder()->getGraphAttribute(attribute);
}

void Graph::getEdgelist(Vector* result, bool bycol) const {
    assert(m_pGraph);
    IGRAPH_TRY(igraph_get_edgelist(m_pGraph, result->c_vector(), bycol));
}

Vector Graph::getEdgelist(bool bycol) const {
    Vector result;
    getEdgelist(&result, bycol);
    return result;
}

bool Graph::isSimple() const {
    bool_t result;
    IGRAPH_TRY(igraph_is_simple(m_pGraph, &result));
    return result;
}

bool Graph::hasAttribute(const std::string& attribute) const {
    return getAttributeHolder()->hasGraphAttribute(attribute);
}

void Graph::neighbors(Vector* result, long int vertex, NeighborMode mode) const {
    assert(m_pGraph);
    IGRAPH_TRY(igraph_neighbors(m_pGraph, result->c_vector(), vertex, mode));
}

Vector Graph::neighbors(long int vertex, NeighborMode mode) const {
    Vector result;
    neighbors(&result, vertex, mode);
    return result;
}

void Graph::setAttribute(const std::string& attribute, const any& value) {
    return getAttributeHolder()->setGraphAttribute(attribute, value);
}

void Graph::simplify(bool multiple, bool loops) {
    // TODO: last argument (attribute combination)
    assert(m_pGraph);
    IGRAPH_TRY(igraph_simplify(m_pGraph, multiple, loops, 0));
}

void Graph::writeEdgelist(FILE* outstream) const {
    assert(m_pGraph);
    IGRAPH_TRY(igraph_write_graph_edgelist(m_pGraph, outstream));
}

/*************/
/* Operators */
/*************/

Graph Graph::operator+(const Graph& other) const {
    std::auto_ptr<igraph_t> result(new igraph_t);
    IGRAPH_TRY(igraph_disjoint_union(result.get(), m_pGraph, other.m_pGraph));
    return Graph(result.release());
}

any& Graph::operator[](const std::string& attribute) {
    return getAttributeHolder()->getGraphAttributeReference(attribute);
}

/***************************************************************************/

AttributeHolder* Graph::getAttributeHolder() {
    return static_cast<AttributeHolder*>(m_pGraph->attr);
}

const AttributeHolder* Graph::getAttributeHolder() const {
    return static_cast<AttributeHolder*>(m_pGraph->attr);
}

}         // end of namespaces

