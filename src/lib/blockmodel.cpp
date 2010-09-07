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

double UndirectedBlockmodel::getLogLikelihood() const {
    double result = 0.0;

    for (int i = 0; i < m_numTypes; i++) {
        int count1 = m_typeCounts[i];
        if (count1 == 0)
            continue;

        for (int j = 0; j < m_numTypes; j++) {
            int count2 = m_typeCounts[j];
            double prob;

            if (count2 == 0)
                continue;

            if (i == j) {
                if (count2 == 1)
                    continue;
                prob = m_edgeCounts(i, i) / count1 / (count2 - 1);
                result += count1 * (count2 - 1) * binary_entropy(prob);
            } else {
                prob = m_edgeCounts(i, j) / count1 / count2;
                result += count1 * count2 * binary_entropy(prob);
            }
        }
    }

    return result;
}

double UndirectedBlockmodel::getProbability(int type1, int type2) const {
    double count1 = m_typeCounts[type1];
    double count2 = m_typeCounts[type2];

    if (count1 == 0 || count2 == 0)
        return 0;

    if (type1 == type2) {
        if (count2 == 1)
            return 0.0;
        return m_edgeCounts(type1, type2) / count1 / (count2 - 1);
    }

    return m_edgeCounts(type1, type2) / count1 / count2;
}

Matrix UndirectedBlockmodel::getProbabilities() const {
    Matrix result(m_numTypes, m_numTypes);
    getProbabilities(result);
    return result;
}

void UndirectedBlockmodel::getProbabilities(Matrix& result) const {
    result.resize(m_numTypes, m_numTypes);

    for (int i = 0; i < m_numTypes; i++) {
        int count1 = m_typeCounts[i];

        if (count1 == 0) {
            /* numerator is strongly zero */
            for (int j = 0; j < m_numTypes; j++)
                result(i, j) = 0;
            continue;
        }

        for (int j = 0; j < m_numTypes; j++) {
            int count2 = m_typeCounts[j];

            if (count2 == 0) {
                /* numerator is strongly zero */
                result(i, j) = 0;
                continue;
            } else if (i == j) {
                result(i, j) = (count2 == 1) ? 0 :
                               ((double)m_edgeCounts(i, j)) / count1 / (count2-1);
            } else {
                result(i, j) = ((double)m_edgeCounts(i, j)) / count1 / count2;
            }
        }
    }
}

void UndirectedBlockmodel::randomize(MersenneTwister& rng) {
    for (Vector::iterator it = m_types.begin(); it != m_types.end(); it++)
        *it = rng.randint(m_numTypes);
    recountEdges();
}

void UndirectedBlockmodel::recountEdges() {
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

void UndirectedBlockmodel::setType(long index, int newType) {
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
}

void UndirectedBlockmodel::setTypes(const Vector& types) {
    m_types = types;
    recountEdges();
}
