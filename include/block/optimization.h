/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef BLOCKMODEL_OPTIMIZATION_H
#define BLOCKMODEL_OPTIMIZATION_H

#include <memory>
#include <block/blockmodel.h>
#include <block/math.hpp>
#include <mtwister/mt.h>

/// Abstract optimization strategy class
class OptimizationStrategy {
    typedef UndirectedBlockmodel Model;

protected:
    /// Pointer to the model being optimized
    Model* m_pModel;

    /// The number of steps taken
    int m_stepCount;

public:
    /// Constructs an optimization strategy not attached to any model
    OptimizationStrategy() : m_pModel(0), m_stepCount(0) {}

    /// Returns the number of steps taken so far
    int getStepCount() const {
        return m_stepCount;
    }

    /// Runs the optimization procedure
    void optimize() {
        while (step());
    }

    /// Sets the model being optimized
    virtual void setModel(Model* model) {
        m_pModel = model;
    }

    /// Runs one step of the optimization strategy
    /**
     * Returns true if the state changed, false otherwise.
     */
    virtual bool step() = 0;
};

/// Greedy optimization strategy for an undirected blockmodel
/**
 * This strategy takes a blockmodel and repeatedly moves vertices between
 * groups in a way that maximizes the local contribution of a vertex to
 * the global likelihood.
 */
class GreedyStrategy : public OptimizationStrategy {
public:
    bool step();
};

/// Optimization strategy that uses a random number generator
class RandomizedOptimizationStrategy : public OptimizationStrategy {
protected:
    /// The random generator used by the strategy
    std::auto_ptr<MersenneTwister> m_pRng;

public:
    /// Constructor
    RandomizedOptimizationStrategy() : m_pRng(new MersenneTwister()) {}

    /// Returns the random generator used by the strategy
    MersenneTwister* getRNG() {
        return m_pRng.get();
    }

    /// Sets the random generator used by the strategy
    void setRNG(MersenneTwister* pRng) {
        m_pRng.reset(pRng);
    }
};

/// Metropolis-Hastings algorithm for an undirected blockmodel
/**
 * In each step, a vertex is selected randomly and a random new group is
 * proposed. There are two possibilities:
 *
 * 1. The new group improves the log-likelihood of the model. In this case,
 *    the new group is accepted unconditionally.
 *
 * 2. The new group does not improve the log-likelihood of the model. In
 *    this case, the new group is accepted with a probability equal to
 *    the likelihood ratio of the new and the old configuration. If the
 *    new group is rejected, the same sample will be returned.
 */
class MetropolisHastingsStrategy : public RandomizedOptimizationStrategy {
private:
    /// The moving average that tracks the acceptance ratio
    MovingAverage<bool> m_acceptanceRatio;

    /// Whether the last proposal was accepted or not
    bool m_lastProposalAccepted;

public:
    /// Constructor
    MetropolisHastingsStrategy() : RandomizedOptimizationStrategy(),
        m_acceptanceRatio(1000), m_lastProposalAccepted(false) {
    }

    /// Returns the acceptance ratio
    float getAcceptanceRatio() const {
        return m_acceptanceRatio.value();
    }

    /// Advances the Markov chain by one step
    bool step();

    /// Returns whether the last proposal was accepted or not
    bool wasLastProposalAccepted() const {
        return m_lastProposalAccepted;
    }
};

/// Gibbs sampling for an undirected blockmodel
/**
 * In each step, a vertex is selected randomly and a new group is
 * proposed according to the log-likelihood conditioned on all but the
 * selected vertex.
 */
class GibbsSamplingStrategy : public RandomizedOptimizationStrategy {
public:
    /// Constructor
    GibbsSamplingStrategy() : RandomizedOptimizationStrategy() {}

    /// Returns the acceptance ratio
    float getAcceptanceRatio() const { return 1.0; }

    /// Advances the Markov chain by one step
    bool step();

    /// Returns whether the last proposal was accepted or not
    bool wasLastProposalAccepted() const { return true; }
};

#endif
