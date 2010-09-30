/* vim:set ts=4 sw=4 sts=4 et: */

#include <cmath>
#include <block/blockmodel.h>

using namespace igraph;

namespace {
    double binary_entropy(double prob) {
        if (prob <= 0 || prob >= 1)
            return 0.0;
        return prob * std::log(prob) + (1 - prob) * std::log(1 - prob);
    }
}

/***************************************************************************/

void Blockmodel::recountEdges() {
    if (m_pGraph == NULL)
        return;

    long n = m_pGraph->vcount();
    Vector edgelist = m_pGraph->getEdgelist();
    Vector::const_iterator it = edgelist.begin();

    m_typeCounts.fill(0);
    for (long i = 0; i < n; i++) {
        m_typeCounts[m_types[i]]++;
    }

    m_edgeCounts.fill(0);
    while (it != edgelist.end()) {
        long type1 = m_types[*(it++)];
        long type2 = m_types[*(it++)];
        m_edgeCounts(type1,type2) += 1;
        m_edgeCounts(type2,type1) += 1;
    }
}

void Blockmodel::setGraph(igraph::Graph* graph) {
    size_t oldSize = m_types.size();

    m_pGraph = graph;
    if (m_pGraph != NULL) {
        m_types.resize(m_pGraph->vcount());
        for (; oldSize < m_types.size(); oldSize++)
            m_types[oldSize] = 0;
        recountEdges();
    }
}

void Blockmodel::setNumTypes(int numTypes) {
    if (numTypes <= 0)
        throw std::runtime_error("must have at least one type");

    m_numTypes = numTypes;
    m_typeCounts.resize(numTypes);
    if (m_pGraph != NULL)
        m_typeCounts[0] += m_pGraph->vcount() - m_typeCounts.sum();

    m_edgeCounts.resize(numTypes, numTypes);
    recountEdges();
}

void Blockmodel::setType(long index, int newType) {
    // Save the old type
    int oldType = m_types[index];
    // Get the neighbors of the affected vertex
    Vector neighbors = m_pGraph->neighbors(index);
    // Adjust the edge counts and the type counts
    // Here we assume that there are no loop edges
    m_typeCounts[oldType]--; m_typeCounts[newType]++;
    for (Vector::const_iterator it = neighbors.begin();
         it != neighbors.end(); it++) {
        long otherType = m_types[*it];
        m_edgeCounts(oldType, otherType)--;
        m_edgeCounts(otherType, oldType)--;
        m_edgeCounts(newType, otherType)++;
        m_edgeCounts(otherType, newType)++;
    }
    // Set the type of the vertex to the new type
    m_types[index] = newType;
    // Invalidate the log-likelihood cache
    invalidateCache();
}

void Blockmodel::setTypes(const Vector& types) {
    m_types = types;
    // Invalidate the log-likelihood cache
    invalidateCache();
    // Recount the edges
    recountEdges();
}

/***************************************************************************/

Graph UndirectedBlockmodel::generate(MersenneTwister& rng) const {
    long int n = m_types.size();
    const Matrix probs = getProbabilities();
    Graph graph(n);
    Vector edges;

    for (long int v1 = 0; v1 < n; v1++) {
        int type1 = m_types[v1];
        for (long int v2 = v1+1; v2 < n; v2++) {
            if (rng.random() < probs(type1, m_types[v2])) {
                edges.push_back(v1);
                edges.push_back(v2);
            }
        }
    }

    graph.addEdges(edges);
    return graph;
}

double UndirectedBlockmodel::getLogLikelihood() const {
    if (m_logLikelihood <= 0)
        return m_logLikelihood;

    double den, result = 0.0;

    for (int i = 0; i < m_numTypes; i++) {
        if (m_typeCounts[i] == 0)
            continue;

        for (int j = i+1; j < m_numTypes; j++) {
            den = getTotalEdgesBetweenGroups(i, j);
            if (den == 0)
                continue;
            result += den * binary_entropy(m_edgeCounts(i, j) / den);
        }

        den = getTotalEdgesBetweenGroups(i, i);
        if (den == 0)
            continue;
        result += den * binary_entropy(m_edgeCounts(i, i) / 2 / den);
    }

    m_logLikelihood = result;
    return result;
}

long int UndirectedBlockmodel::getTotalEdgesBetweenGroups(int type1, int type2) const {
    double count1 = m_typeCounts[type1];
    double count2 = m_typeCounts[type2];

    if (type1 == type2)
        return count1 * (count2 - 1) / 2;

    return count1 * count2;
}

double UndirectedBlockmodel::getProbability(int type1, int type2) const {
    if (m_pGraph == NULL)
        return m_probabilities(type1, type2);

    double possibleEdges = getTotalEdgesBetweenGroups(type1, type2);

    if (possibleEdges == 0)
        return 0;

    if (type1 == type2)
        // m_edgeCounts counts edges between the same group twice
        possibleEdges *= 2;

    return m_edgeCounts(type1, type2) / possibleEdges;
}

Matrix UndirectedBlockmodel::getProbabilities() const {
    Matrix result(m_numTypes, m_numTypes);
    getProbabilities(result);
    return result;
}

void UndirectedBlockmodel::getProbabilities(Matrix& result) const {
    if (m_pGraph == NULL) {
        result = m_probabilities;
        return;
    }

    result.resize(m_numTypes, m_numTypes);

    for (int i = 0; i < m_numTypes; i++) {
        double den;

        if (m_typeCounts[i] == 0) {
            /* numerator is strongly zero */
            for (int j = i; j < m_numTypes; j++) {
                result(i, j) = result(j, i) = 0;
            }
            continue;
        }

        for (int j = i+1; j < m_numTypes; j++) {
            den = getTotalEdgesBetweenGroups(i, j);
            result(i, j) = result(j, i) =
                (den == 0) ? 0 : (m_edgeCounts(i, j) / den);
        }

        den = getTotalEdgesBetweenGroups(i, i);
        result(i, i) = (den == 0) ? 0 : (m_edgeCounts(i, i) / 2 / den); 
    }
}

void UndirectedBlockmodel::randomize(MersenneTwister& rng) {
    for (Vector::iterator it = m_types.begin(); it != m_types.end(); it++)
        *it = rng.randint(m_numTypes);
    // Invalidate the log-likelihood cache
    invalidateCache();
    // Recount the edges
    recountEdges();
}

void UndirectedBlockmodel::setNumTypes(int numTypes) {
    Blockmodel::setNumTypes(numTypes);
    if (m_pGraph != NULL)
        m_probabilities.resize(0, 0);
    else
        m_probabilities.resize(numTypes, numTypes);
}

void UndirectedBlockmodel::setProbabilities(const Matrix& p) {
    if (!p.isSymmetric())
        throw new std::runtime_error("probabilities matrix must be symmetric");
    m_probabilities = p;
}
