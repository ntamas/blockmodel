/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef BLOCKMODEL_OPTIMIZATION_H
#define BLOCKMODEL_OPTIMIZATION_H

#include <block/blockmodel.h>
#include <block/math.h>
#include <mtwister/mt.h>

/// Abstract optimization strategy class
class OptimizationStrategy {
    typedef UndirectedBlockmodel Model;

protected:
    /// Pointer to the model being optimized
    Model* m_pModel;

public:
    /// Constructs an optimization strategy not attached to any model
    OptimizationStrategy() : m_pModel(0) {}

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

/// Markov chain Monte Carlo sampling for an undirected blockmodel
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
class MCMCStrategy : public OptimizationStrategy {
private:
    /// The random number generator used by the MCMC sampler
    MersenneTwister m_rng;

    /// The number of steps taken
    int m_stepCount;

    /// The moving average that tracks the acceptance ratio
    MovingAverage<bool> m_acceptanceRatio;

public:
    /// Constructor
    MCMCStrategy() : OptimizationStrategy(), m_stepCount(0), m_acceptanceRatio(1000) {
    }

    /// Returns the acceptance ratio
    float getAcceptanceRatio() const {
        return m_acceptanceRatio.value();
    }

    /// Returns the number of steps taken so far
    int getStepCount() const {
        return m_stepCount;
    }

    /// Advances the Markov chain by one step
    bool step();
};

#endif
