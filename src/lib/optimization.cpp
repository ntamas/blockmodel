/* vim:set ts=4 sw=4 sts=4 et: */

#include <algorithm>
#include <cmath>
#include <block/optimization.h>
#include <igraph/cpp/graph.h>

using namespace igraph;

namespace {
    double log_1_minus_x(double x) {
        return std::log(1.0 - x);
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
    double logL = m_pModel->getLogLikelihood();
    Vector oldTypeCounts(m_pModel->getTypeCounts());
    Vector newTypes(n);

    m_stepCount++;

    // Calculate log(1-P)
    Matrix logP_minus_log1P = m_pModel->getProbabilities();
    Matrix log1P(logP_minus_log1P);
    std::transform(logP_minus_log1P.begin(), logP_minus_log1P.end(),
            log1P.begin(), log_1_minus_x);

    // Calculate logP_minus_log1P
    std::transform(logP_minus_log1P.begin(), logP_minus_log1P.end(),
            logP_minus_log1P.begin(), log);
    logP_minus_log1P -= log1P;

    // Okay, so the local contribution of vertex i to the log-likelihood is
    // given as follows, assuming that the type of vertex i is t_i,
    // n_k is the number of neighbors of vertex i with type k and
    // N_k is the total number of vertices with type k
    //
    // sum_k (n_k * log(p(t_i, k)) + (N_k - n_k) * log(1-p(t_i, k)))
    //
    // This can be re-written as:
    //
    // sum_k (N_k * log(1-p(t_i, k)) + n_k * (log(p(t_i, k)) - log(1-p(t_i, k))))
    //
    // That's why I wrote above that we will need logP - log(1-P). Switching to
    // vector notations:
    //
    // N * log(1-P) + n * (log(P) - log(1-P))
    //
    // is what we are looking for. This gives us a vector and we simply have
    // to select the maximum element to get the new type of vertex i.
    //
    // Well, there's a small glitch here: if vertex i is currently in group t_i,
    // we have to decrease component t_i of N by 1; or, subtract row t_i of
    // log(1-P) from the final vector.
    //
    // TODO: or column? Think about it. It doesn't matter for undirected
    // blockmodels, but it may matter for directed ones.

    // For each vertex...
    for (i = 0; i < n; i++) {
        // First we calculate the number of neighbors of type k for vertex i,
        // which we will denote with neiCountByType(k).
        Vector neiCountByType(k);
        Vector neis = graph->neighbors(i);
        for (Vector::iterator it = neis.begin(); it != neis.end(); it++) {
            neiCountByType[m_pModel->getType(*it)]++;
        }

        // We already have logP - log1P, so all we need is two matrix-vector
        // products and an addition.
        neiCountByType = logP_minus_log1P * neiCountByType;
        neiCountByType += (log1P * oldTypeCounts);
        // We might get 'nan' values after this step in neiCountByType if
        // an infinity or negative infinity (occurring in logP_minus_log1P
        // or log1P) was multiplied by zero. We don't care, though, nan
        // values will never be selected as maxima anyway.

        // Now the correction mentioned above
        neiCountByType -= log1P.getRow(m_pModel->getType(i));

        // Find the maximum element
        newTypes[i] = 
            std::max_element(neiCountByType.begin(), neiCountByType.end()) -
            neiCountByType.begin();
    }

    // TODO: rewrite the above to use a single matrix-matrix multiplication,
    // maybe that's faster?
    if (newTypes != m_pModel->getTypes()) {
        m_pModel->setTypes(newTypes);
        return (m_pModel->getLogLikelihood() > logL);
    }

    return false;
}

/*************************************************************************/

bool MetropolisHastingsStrategy::step() {
    Graph* graph = m_pModel->getGraph();
    int i = m_pRng->randint(graph->vcount());
    int oldType = m_pModel->getType(i);
    int newType = m_pRng->randint(m_pModel->getNumTypes());
    double logL = m_pModel->getLogLikelihood();

    m_pModel->setType(i, newType);
    double newLogL = m_pModel->getLogLikelihood();

    m_stepCount++;

    m_lastProposalAccepted = true;
    if (newLogL < logL) {
        double p = std::exp(newLogL - logL);
        if (m_pRng->random() > p) {
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
    int i = m_pRng->randint(graph->vcount());
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
    logLs.binsearch(m_pRng->random() * logLs.back(), &k);
    m_pModel->setType(i, k);

    return true;
}

