/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef BLOCKMODEL_OPTIMIZATION_H
#define BLOCKMODEL_OPTIMIZATION_H

#include <block/blockmodel.h>

/// Greedy optimization strategy for an undirected blockmodel
/**
 * This strategy takes a blockmodel and repeatedly moves vertices between
 * groups in a way that maximizes the local contribution of a vertex to
 * the global likelihood.
 */
class GreedyStrategy {
private:
    bool step(UndirectedBlockmodel& model);
public:
    void optimize(UndirectedBlockmodel& model);
};

#endif
