/* vim:set ts=4 sw=4 sts=4 et: */

#include <igraph/igraph_attributes.h>
#include <igraph/cpp/attributes.h>
#include <igraph/cpp/error.h>

namespace igraph {

/***************************************************************************/

void AttributeHolder::copyGraphAttributesFrom(const AttributeHolder& other) {
    m_graphAttributes = other.m_graphAttributes;
}

void AttributeHolder::copyVertexAttributesFrom(const AttributeHolder& other) {
    m_vertexAttributes = other.m_vertexAttributes;
}

void AttributeHolder::copyEdgeAttributesFrom(const AttributeHolder& other) {
    m_edgeAttributes = other.m_edgeAttributes;
}

any AttributeHolder::getGraphAttribute(const std::string& attribute) const {
    GraphAttributeMap::const_iterator it = m_graphAttributes.find(attribute);
    if (it == m_graphAttributes.end())
        return any();
    return it->second;
}

any& AttributeHolder::getGraphAttributeReference(const std::string& attribute) {
    return m_graphAttributes[attribute];
}

bool AttributeHolder::hasGraphAttribute(const std::string& attribute) const {
    return m_graphAttributes.find(attribute) != m_graphAttributes.end();
}

void AttributeHolder::setGraphAttribute(const std::string& attribute,
        const any& value) {
    m_graphAttributes[attribute] = value;
}

/***************************************************************************/

struct AttributeHandlerImpl {
    static int init(igraph_t *graph, igraph_vector_ptr_t *attr) {
        IGRAPHPP_TRY_NEW(graph->attr, AttributeHolder);
        return IGRAPH_SUCCESS;
    }

    static void destroy(igraph_t *graph) {
        if (graph->attr)
            delete static_cast<AttributeHolder*>(graph->attr);
        graph->attr = NULL;
    }

    static int copy(igraph_t *to, const igraph_t *from,
            igraph_bool_t ga, igraph_bool_t va, igraph_bool_t ea) {
        IGRAPHPP_TRY_NEW(to->attr, AttributeHolder);

        if (from->attr == NULL)
            return IGRAPH_SUCCESS;

        const AttributeHolder& from0 = *(static_cast<AttributeHolder*>(from->attr));
        AttributeHolder& to0 = *(static_cast<AttributeHolder*>(to->attr));

        if (ga)
            to0.copyGraphAttributesFrom(from0);
        if (va)
            to0.copyVertexAttributesFrom(from0);
        if (ea)
            to0.copyEdgeAttributesFrom(from0);

        return IGRAPH_SUCCESS;
    }

    /***********************************************************************/

    static int add_vertices(igraph_t *graph, long int nv, igraph_vector_ptr_t *attr) {
        // TODO
        return IGRAPH_SUCCESS;
    }

    /***********************************************************************/

    static int add_edges(igraph_t *graph, const igraph_vector_t *edges,
                  igraph_vector_ptr_t *attr) {
        // TODO
        return IGRAPH_SUCCESS;
    }
};


/***************************************************************************/

static igraph_attribute_table_t cpp_attribute_handler = {
    &AttributeHandlerImpl::init,
    &AttributeHandlerImpl::destroy,
    &AttributeHandlerImpl::copy,
    &AttributeHandlerImpl::add_vertices,
    /* permute_vertices = */ 0,
    /* combine_vertices = */ 0,
    &AttributeHandlerImpl::add_edges,
    /* permute_edges = */ 0,
    /* combine_edges = */ 0,
    /* get_info = */ 0,
    /* has_attr = */ 0,
    /* gettype = */ 0,
    /* get_numeric_graph_attr= */ 0,
    /* get_string_graph_attr= */ 0,
    /* get_numeric_vertex_attr= */ 0,
    /* get_string_vertex_attr= */ 0,
    /* get_numeric_edge_attr= */ 0,
    /* get_string_edge_attr= */ 0
};

void AttributeHandler::attach() {
    igraph_i_set_attribute_table(&cpp_attribute_handler);
}

}            // end of namespace

