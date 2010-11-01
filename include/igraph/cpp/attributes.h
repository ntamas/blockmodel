/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef IGRAPHPP_ATTRIBUTES_H
#define IGRAPHPP_ATTRIBUTES_H

#include <map>
#include <vector>
#include <igraph/cpp/any.hpp>

namespace igraph {

struct AttributeHandlerImpl;

/// Attribute holder class for graphs
class AttributeHolder {
private:
    /// Typedef for attribute values
    typedef any AttributeValue;

    /// Typedef for a vector that stores attributes
    typedef std::vector<AttributeValue> AttributeValueVector;

    /// Typedef for the graph attribute storage
    typedef std::map<std::string, AttributeValue> GraphAttributeMap;

    /// Typedef for the vertex attribute storage
    typedef std::map<std::string, AttributeValueVector> VertexAttributeMap;

    /// Typedef for the edge attribute storage
    typedef std::map<std::string, AttributeValueVector> EdgeAttributeMap;

    /// Storage for the graph attributes
    GraphAttributeMap m_graphAttributes;
    
    /// Storage for the vertex attributes
    VertexAttributeMap m_vertexAttributes;

    /// Storage for the edge attributes
    EdgeAttributeMap m_edgeAttributes;

public:
    /// Returns a reference to the value of the given graph attribute
    any& getGraphAttributeReference(const std::string& attribute);

    /// Returns a copy of the value of the given graph attribute
    any getGraphAttribute(const std::string& attribute) const;

    /// Checks whether a given edge attribute exists
    bool hasEdgeAttribute(const std::string& attribute) const;

    /// Checks whether a given graph attribute exists
    bool hasGraphAttribute(const std::string& attribute) const;

    /// Checks whether a given vertex attribute exists
    bool hasVertexAttribute(const std::string& attribute) const;

    /// Sets the value of a given graph attribute
    void setGraphAttribute(const std::string& attribute, const any& value);

    friend class AttributeHandlerImpl;

private:
    /// Copies the graph attributes from another holder
    void copyGraphAttributesFrom(const AttributeHolder& other);

    /// Copies the vertex attributes from another holder
    void copyVertexAttributesFrom(const AttributeHolder& other);

    /// Copies the edge attributes from another holder
    void copyEdgeAttributesFrom(const AttributeHolder& other);
};

/// Attribute handler to be attached to igraph's attribute handler interface
struct AttributeHandler {
    /// Attaches the attribute handler to igraph
    static void attach();
};

}               // end of namespaces

#endif          // IGRAPHPP_ATTRIBUTES_H

