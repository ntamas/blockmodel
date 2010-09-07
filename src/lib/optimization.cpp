/* vim:set ts=4 sw=4 sts=4 et: */

#include <algorithm>
#include <cmath>
#include <block/optimization.h>
#include <igraph/cpp/graph.h>

using namespace igraph;

namespace {
    double log_1_minus_x(double x) {
        return 1.0 - std::log(x);
    }

    igraph_real_t exp_igraph(igraph_real_t x) {
        return std::exp(x);
    }
}

/*************************************************************************/

bool GreedyStrategy::step() {
    Graph* graph = m_pModel->getGraph();
    long int i, n = graph->vcount();
    int k = m_pModel->getNumTypes();
    Vector newTypes(n);

    m_stepCount++;

    // Calculate logP and log(1-P)
    Matrix logP = m_pModel->getProbabilities();
    Matrix log1P(logP);
    std::transform(logP.begin(), logP.end(), log1P.begin(), log_1_minus_x);
    std::transform(logP.begin(), logP.end(), logP.begin(), log);
    
    // Calculate logP - log(1-P), we will only need this, see explanation
    // below in the main loop
    logP -= log1P;

    // For each vertex...
    for (i = 0; i < n; i++) {
        // Approximate the local contribution of the vertex to the likelihood
        // when it is moved to each of the groups
        //
        // With some math magic, it can be shown that if the group of vertex i
        // is t_i, then the log-likelihood corresponding to the vertex is equal
        // to n * (log p(t_i, :) - log(1-p(t_i, :))) + const in Matlab notation.
        // n is a vector containing the number of neighbors i having type x and
        // the constant does not depend on n.
        //
        // So, first we calculate n, which we will denote with neiCountByType.
        Vector neiCountByType(k);
        Vector neis = graph->neighbors(i);
        for (Vector::iterator it = neis.begin(); it != neis.end(); it++) {
            neiCountByType[m_pModel->getType(*it)]++;
        }

        // We already have logP - log1P, so all we need is a matrix-vector
        // product
        neiCountByType = logP * neiCountByType;

        newTypes[i] = 
            std::max_element(neiCountByType.begin(), neiCountByType.end()) -
            neiCountByType.begin();
    }

    // TODO: rewrite the above to use a single matrix-matrix multiplication,
    // maybe that's faster?
    if (newTypes != m_pModel->getTypes()) {
        m_pModel->setTypes(newTypes);
        return true;
    }

    return false;
}

/*************************************************************************/

bool MetropolisHastingsStrategy::step() {
    Graph* graph = m_pModel->getGraph();
    int i = m_rng.randint(graph->vcount());
    int oldType = m_pModel->getType(i);
    int newType = m_rng.randint(m_pModel->getNumTypes());
    double logL = m_pModel->getLogLikelihood();

    m_pModel->setType(i, newType);
    double newLogL = m_pModel->getLogLikelihood();

    m_stepCount++;

    m_lastProposalAccepted = true;
    if (newLogL < logL) {
        double p = std::exp(newLogL - logL);
        if (m_rng.random() > p) {
            /* reject the proposal, reset the state */
            m_pModel->setType(i, oldType);
            m_lastProposalAccepted = false;
        }
    }

    m_acceptanceRatio.push_back(m_lastProposalAccepted);
    return true;
}

/*************************************************************************/

bool GibbsSamplingStrategy::step() {
    Graph* graph = m_pModel->getGraph();
    int i = m_rng.randint(graph->vcount());
    long int k = m_pModel->getNumTypes();
    Vector logLs(k);

    m_stepCount++;

    // TODO: maybe this can be calculated more efficiently?
    for (int j = 0; j < k; j++) {
        m_pModel->setType(i, j);
        logLs[j] = m_pModel->getLogLikelihood();
    }

    // Subtract the minimum log-likelihood from the log-likelihoods
    logLs -= logLs.min();

    // Run exp(), get cumulative sum
    std::transform(logLs.begin(), logLs.end(), logLs.begin(), exp_igraph);
    std::partial_sum(logLs.begin(), logLs.end(), logLs.begin());

    // Select a new type based on the log-likelihood distribution
    logLs.binsearch(m_rng.random() * logLs.back(), &k);
    m_pModel->setType(i, k);

    return true;
}

