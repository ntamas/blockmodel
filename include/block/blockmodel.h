/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef BLOCKMODEL_BLOCKMODEL_H
#define BLOCKMODEL_BLOCKMODEL_H

#include <igraph/cpp/graph.h>
#include <igraph/cpp/matrix.h>
#include <igraph/cpp/vector.h>

/// Class representing an undirected blockmodel
class UndirectedBlockmodel {
private:
    /// Pointer to a graph to which this model will be fitted
    igraph::Graph* m_pGraph;

    /// The number of types in the model
    int m_numTypes;

    /// Vector storing the vertex types
    igraph::Vector m_types;

    /// Vector storing the number of vertices of a given type
    igraph::Vector m_typeCounts;

    /// Matrix storing the number of edges between pairs of vertex types
    /**
     * Due to some optimizations, the diagonal of the matrix actually stores
     * twice the number of edges going between vertices of the same type.
     */
    igraph::Matrix m_edgeCounts;

public:
    /// Constructs a new undirected blockmodel to be fitted to the given graph
    UndirectedBlockmodel(igraph::Graph* graph, int numTypes)
        : m_pGraph(graph), m_numTypes(numTypes), m_types(graph->vcount()),
          m_typeCounts(numTypes), m_edgeCounts(numTypes, numTypes) {
        m_typeCounts[0] = m_pGraph->vcount();
        m_edgeCounts(0,0) = 2 * m_pGraph->ecount();
    }

    /// Returns the number of edges between the two given groups
    long getEdgeCount(long ri, long ci) const {
        return m_edgeCounts(ri, ci);
    }

    /// Returns the whole edge count matrix
    igraph::Matrix getEdgeCounts() const {
        return m_edgeCounts;
    }

    /// Returns a pointer to the graph associated to the model
    igraph::Graph* getGraph() {
        return m_pGraph;
    }

    /// Returns the log-likelihood of the model
    double getLogLikelihood() const;

    /// Returns the number of types in this model
    int getNumTypes() const {
        return m_numTypes;
    }

    /// Returns the estimated probability between the two given types
    double getProbability(int type1, int type2) const {
        double count1 = m_typeCounts[type1];
        double count2 = m_typeCounts[type2];

        if (count1 == 0 || count2 == 0)
            return 0;

        if (type1 == type2)
            return m_edgeCounts(type1, type2) / count1 / (count2 - 1);
        return m_edgeCounts(type1, type2) / count1 / count2;
    }

    /// Returns the estimated probability matrix
    igraph::Matrix getProbabilities() const;

    /// Returns the estimated probability matrix
    void getProbabilities(igraph::Matrix& result) const;

    /// Returns the type of the given vertex
    int getType(long index) const {
        return m_types[index];
    }

    /// Returns how many vertices have the given type
    long getTypeCount(long index) const {
        return m_typeCounts[index];
    }

    /// Returns the whole type count vector
    igraph::Vector getTypeCounts() const {
        return m_typeCounts;
    }

    /// Returns the whole type vector
    igraph::Vector getTypes() const {
        return m_types;
    }

    /// Sets the type of a single vertex
    void setType(long index, int newType);

    /// Sets the types of multiple vertices
    void setTypes(const igraph::Vector& types);

private:
    /// Recounts the edges and updates m_typeCounts and m_edgeCounts
    void recountEdges();
};

#endif
