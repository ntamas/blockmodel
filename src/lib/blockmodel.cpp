/* vim:set ts=4 sw=4 sts=4 et: */

#include <cmath>
#include <block/blockmodel.h>

namespace {
    double binary_entropy(double prob) {
        if (prob <= 0 || prob >= 1)
            return 0.0;
        return prob * std::log(prob) + (1 - prob) * std::log(prob);
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

Matrix<double> UndirectedBlockmodel::getProbabilities() const {
    Matrix<double> result(m_numTypes, m_numTypes);
    getProbabilities(result);
    return result;
}

void UndirectedBlockmodel::getProbabilities(Matrix<double>& result) const {
    result.resize(m_numTypes, m_numTypes);

    for (int i = 0; i < m_numTypes; i++) {
        int count1 = m_typeCounts[i];
        if (count1 == 0)
            continue;

        for (int j = 0; j < m_numTypes; j++) {
            int count2 = m_typeCounts[j];
            if (count2 == 0)
                continue;

            if (i == j)
                result(i, j) = ((double)m_edgeCounts(i, j)) / count1 / (count2-1);
            else
                result(i, j) = ((double)m_edgeCounts(i, j)) / count1 / count2;
        }
    }
}

void UndirectedBlockmodel::setType(long index, long newType) {
    // Save the old type
    long oldType = m_types[index];
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

void UndirectedBlockmodel::recountEdges() {
    long n = m_pGraph->vcount();
    Vector edgelist = m_pGraph->getEdgelist();
    Vector::const_iterator it = edgelist.begin();

    m_typeCounts.fill(0);
    for (long i = 0; i < n; i++) {
        m_typeCounts[m_types[i]]++;
    }

    while (it != edgelist.end()) {
        long type1 = m_types[*(it++)];
        long type2 = m_types[*(it++)];
        m_edgeCounts(type1,type2) += 1;
        m_edgeCounts(type2,type1) += 1;
    }
}
