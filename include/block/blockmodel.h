/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef BLOCKMODEL_BLOCKMODEL_H
#define BLOCKMODEL_BLOCKMODEL_H

#include <block/matrix.hpp>
#include <igraph/cpp/graph.h>

using namespace igraph;

/// Class representing an undirected blockmodel
class UndirectedBlockmodel {
private:
    /// Pointer to a graph to which this model will be fitted
    Graph* m_pGraph;

    /// The number of types in the model
    int m_numTypes;

    /// Vector storing the vertex types
    Vector m_types;

    /// Vector storing the number of vertices of a given type
    Vector m_typeCounts;

    /// Matrix storing the number of edges between pairs of vertex types
    /**
     * Due to some optimizations, the diagonal of the matrix actually stores
     * twice the number of edges going between vertices of the same type.
     */
    Matrix<long> m_edgeCounts;

public:
    /// Constructs a new undirected blockmodel to be fitted to the given graph
    UndirectedBlockmodel(Graph* graph, int numTypes)
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
    Matrix<long> getEdgeCounts() const {
        return m_edgeCounts;
    }

    /// Returns the log-likelihood of the model
    double getLogLikelihood() const;

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
    Matrix<double> getProbabilities() const;

    /// Returns the estimated probability matrix
    void getProbabilities(Matrix<double>& result) const;

    /// Returns how many vertices have the given type
    long getTypeCount(long index) const {
        return m_typeCounts[index];
    }

    /// Returns the whole type count vector
    Vector getTypeCounts() const {
        return m_typeCounts;
    }

    /// Returns the whole type vector
    Vector getTypes() const {
        return m_types;
    }

    /// Sets the type of a single vertex
    void setType(long index, long newType);

    /// Sets the types of multiple vertices
    void setTypes(const Vector& types);

private:
    /// Recounts the edges and updates m_typeCounts and m_edgeCounts
    void recountEdges();
};

#endif
