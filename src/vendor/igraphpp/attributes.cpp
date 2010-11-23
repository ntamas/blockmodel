/* vim:set ts=4 sw=4 sts=4 et: */

#include <igraph/igraph_attributes.h>
#include <igraph/igraph_interface.h>
#include <igraph/cpp/attributes.h>
#include <igraph/cpp/error.h>
#include <igraph/cpp/ptr_vector.h>
#include <igraph/cpp/str_vector.h>
#include <igraph/cpp/vector.h>

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

AttributeValueVector AttributeHolder::getEdgeAttribute(const std::string& attribute) const {
    EdgeAttributeMap::const_iterator it = m_edgeAttributes.find(attribute);
    if (it == m_edgeAttributes.end())
        return AttributeValueVector();
    return it->second;
}

AttributeValue AttributeHolder::getGraphAttribute(const std::string& attribute) const {
    GraphAttributeMap::const_iterator it = m_graphAttributes.find(attribute);
    if (it == m_graphAttributes.end())
        return AttributeValue();
    return it->second;
}

AttributeValueVector AttributeHolder::getVertexAttribute(const std::string& attribute) const {
    VertexAttributeMap::const_iterator it = m_vertexAttributes.find(attribute);
    if (it == m_vertexAttributes.end())
        return AttributeValueVector();
    return it->second;
}

AttributeValue AttributeHolder::getVertexAttribute(const std::string& attribute,
        long int index) const {
    VertexAttributeMap::const_iterator it = m_vertexAttributes.find(attribute);
    if (it == m_vertexAttributes.end())
        return AttributeValueVector();
    return it->second[index];
}

bool AttributeHolder::hasEdgeAttribute(const std::string& attribute) const {
    return m_edgeAttributes.find(attribute) != m_edgeAttributes.end();
}

bool AttributeHolder::hasGraphAttribute(const std::string& attribute) const {
    return m_graphAttributes.find(attribute) != m_graphAttributes.end();
}

bool AttributeHolder::hasVertexAttribute(const std::string& attribute) const {
    return m_vertexAttributes.find(attribute) != m_vertexAttributes.end();
}

void AttributeHolder::setGraphAttribute(const std::string& attribute,
        const AttributeValue& value) {
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

    template <typename AttrMapType>
    static int add_vertices_edges_helper(AttrMapType& attrs, long int lastIndex,
            long int numNewEntries, igraph_vector_ptr_t* attr_vector_ptr) {
        // Extend the vertex attribute vectors by numNewEntries new elements
        for (AttributeHolder::VertexAttributeMap::iterator it = attrs.begin();
                it != attrs.end(); it++) {
            it->second.resize(lastIndex + numNewEntries);
        }

        // Do we have attributes with the newly added vertices? If not, return.
        if (attr_vector_ptr == 0)
            return IGRAPH_SUCCESS;

        PtrVector<igraph_attribute_record_t*> attrRecords(attr_vector_ptr);
        long int i, j;

        // For each attribute record...
        for (PtrVector<igraph_attribute_record_t*>::iterator it = attrRecords.begin();
                it != attrRecords.end(); it++) {
           igraph_attribute_record_t* record = (*it);

            // Do we have an attribute with this name? If not, add it.
            if (attrs.find(record->name) == attrs.end())
                attrs[record->name].resize(lastIndex + numNewEntries);

            AttributeValueVector& vec = attrs[record->name];
            switch (record->type) {
                case IGRAPH_ATTRIBUTE_NUMERIC:
                    {
                        Vector values(static_cast<igraph_vector_t*>(
                                    const_cast<void*>(record->value)
                        ), false);
                        for (i = 0, j = lastIndex; i < numNewEntries; i++, j++)
                            vec[j] = values[i];
                    }
                    break;

                case IGRAPH_ATTRIBUTE_STRING:
                    {
                        StrVector values(static_cast<igraph_strvector_t*>(
                                    const_cast<void*>(record->value)), false);
                        for (i = 0, j = lastIndex; i < numNewEntries; i++, j++)
                            vec[j] = std::string(values[i]);
                    }
                    break;

                default:
                    // Unsupported attribute type, just continue
                    // TODO: show a warning?
                    break;
            }
        }

        return IGRAPH_SUCCESS;
    }

    static int add_vertices(igraph_t *graph, long int nv, igraph_vector_ptr_t *attr) {
        AttributeHolder& holder = *(static_cast<AttributeHolder*>(graph->attr));
        AttributeHolder::VertexAttributeMap& attrs = holder.m_vertexAttributes;
        return add_vertices_edges_helper(attrs, igraph_vcount(graph), nv, attr);
    }

    /***********************************************************************/

    static int add_edges(igraph_t *graph, const igraph_vector_t *edges,
                  igraph_vector_ptr_t *attr) {
        AttributeHolder& holder = *(static_cast<AttributeHolder*>(graph->attr));
        AttributeHolder::EdgeAttributeMap& attrs = holder.m_edgeAttributes;
        return add_vertices_edges_helper(attrs, igraph_ecount(graph),
                igraph_vector_size(edges), attr);
    }

    /***********************************************************************/

    static int get_numeric_graph_attr(const igraph_t *graph, const char *name,
            igraph_vector_t *value) {
        AttributeHolder& holder = *(static_cast<AttributeHolder*>(graph->attr));
        AttributeHolder::GraphAttributeMap& attrs = holder.m_graphAttributes;
        AttributeHolder::GraphAttributeMap::const_iterator it;

        it = attrs.find(name);
        if (it == attrs.end())
            IGRAPH_ERROR("Unknown attribute", IGRAPH_EINVAL);

        IGRAPH_CHECK(igraph_vector_resize(value, 1));
        VECTOR(*value)[0] = *it->second.as<igraph_real_t>();

        return IGRAPH_SUCCESS;
    }

    /***********************************************************************/

    static int get_string_graph_attr(const igraph_t *graph, const char *name,
            igraph_strvector_t *value) {
        AttributeHolder& holder = *(static_cast<AttributeHolder*>(graph->attr));
        AttributeHolder::GraphAttributeMap& attrs = holder.m_graphAttributes;
        AttributeHolder::GraphAttributeMap::const_iterator it;

        it = attrs.find(name);
        if (it == attrs.end())
            IGRAPH_ERROR("Unknown attribute", IGRAPH_EINVAL);

        IGRAPH_CHECK(igraph_strvector_resize(value, 1));
        IGRAPH_CHECK(igraph_strvector_set(value, 0, *it->second.as<const char*>()));

        return IGRAPH_SUCCESS;
    }

    /***********************************************************************/

    template <typename T>
    static int gettype_helper(const igraph_t *graph, igraph_attribute_type_t *type,
            const char* name, const T& map) {
        typename T::const_iterator it = map.find(name);

        if (it == map.end())
            IGRAPH_ERROR("Unknown attribute", IGRAPH_EINVAL);

        const typename T::mapped_type& values = it->second;
        igraph_attribute_type_t result = IGRAPH_ATTRIBUTE_DEFAULT;

        /* Infer the attribute type by evaluating the types of the items */
        for (typename T::mapped_type::const_iterator it = values.begin();
                it != values.end(); it++) {
            // const any& item = *it;
            // const std::type_info& type_info = item.type();

            // TODO
        }

        if (result == IGRAPH_ATTRIBUTE_DEFAULT)
            result = IGRAPH_ATTRIBUTE_NUMERIC;

        *type = result;
        return IGRAPH_SUCCESS;
    }

    static int gettype(const igraph_t *graph, igraph_attribute_type_t *type,
            igraph_attribute_elemtype_t elemtype, const char *name) {
        AttributeHolder& holder = *(static_cast<AttributeHolder*>(graph->attr));
        switch (elemtype) {
        case IGRAPH_ATTRIBUTE_GRAPH:
            // TODO
            *type = IGRAPH_ATTRIBUTE_NUMERIC;
            return IGRAPH_SUCCESS;

        case IGRAPH_ATTRIBUTE_VERTEX:
            return gettype_helper(graph, type, name, holder.m_vertexAttributes);

        case IGRAPH_ATTRIBUTE_EDGE:
            return gettype_helper(graph, type, name, holder.m_edgeAttributes);

        default:
            IGRAPH_ERROR("Unknown attribute element type", IGRAPH_EINVAL);
        }
    }

    /***********************************************************************/

    template <typename T>
    static igraph_bool_t has_attr_helper(const T& map, const char* name) {
        return map.find(name) != map.end();
    }

    static igraph_bool_t has_attr(const igraph_t *graph,
            igraph_attribute_elemtype_t type, const char *name) {
        AttributeHolder& holder = *(static_cast<AttributeHolder*>(graph->attr));

        switch (type) {
            case IGRAPH_ATTRIBUTE_GRAPH:
                return has_attr_helper(holder.m_graphAttributes, name);

            case IGRAPH_ATTRIBUTE_VERTEX:
                return has_attr_helper(holder.m_vertexAttributes, name);

            case IGRAPH_ATTRIBUTE_EDGE:
                return has_attr_helper(holder.m_edgeAttributes, name);

            default:
                IGRAPH_ERROR("Unknown attribute element type", IGRAPH_EINVAL);
        }
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
    &AttributeHandlerImpl::has_attr,
    &AttributeHandlerImpl::gettype,
    &AttributeHandlerImpl::get_numeric_graph_attr,
    &AttributeHandlerImpl::get_string_graph_attr,
    /* get_numeric_vertex_attr= */ 0,
    /* get_string_vertex_attr= */ 0,
    /* get_numeric_edge_attr= */ 0,
    /* get_string_edge_attr= */ 0
};

void AttributeHandler::attach() {
    igraph_i_set_attribute_table(&cpp_attribute_handler);
}

}            // end of namespace

