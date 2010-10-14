/* vim:set ts=4 sw=4 sts=4 et: */

#include <cstdlib>
#include <block/blockmodel.h>
#include <igraph/cpp/graph.h>
#include <mtwister/mt.h>

#include "test_common.cpp"

using namespace igraph;

int test_getLogLikelihood() {
    /* Disjoint union of two full graphs */
    Graph graph = Graph::Full(5) + Graph::Full(3);
    DegreeCorrectedUndirectedBlockmodel model =
        Blockmodel::create<DegreeCorrectedUndirectedBlockmodel>(&graph, 2);

    for (int i = 0; i < 8; i++)
        model.setType(i, i / 5);

    if (!ALMOST_EQUALS(model.getLogLikelihood(), -16.4478, 1e-3))
        return 1;

    /* Pathological case: no edge between vertices of type 1 */
    for (int i = 0; i < 8; i++)
        model.setType(i, (i == 3) ? 1 : 0);
    if (!ALMOST_EQUALS(model.getLogLikelihood(), -23.1048, 1e-3))
        return 2;

    /* Even more pathological case: no vertices of type 1 */
    for (int i = 0; i < 8; i++)
        model.setType(i, 0);
    if (!ALMOST_EQUALS(model.getLogLikelihood(), -23.4705, 1e-3))
        return 3;

    return 0;
}

int test_getLogLikelihoodIncrease() {
    /* Disjoint union of two full graphs */
    Graph graph = Graph::Full(5) + Graph::Full(3);
    DegreeCorrectedUndirectedBlockmodel model =
        Blockmodel::create<DegreeCorrectedUndirectedBlockmodel>(&graph, 4);
    MersenneTwister rng;
    double predictedLogL, predictedDiff;

    /* Try four groups, do many random mutations */
    for (int i = 0; i < 8; i++)
        model.setType(i, i / 2);
    for (int i = 0; i < 10000; i++) {
        int from = rng.randint(8);
        PointMutation mutation(from, model.getType(from), rng.randint(4));
        while (mutation.from == mutation.to)
            mutation.to = rng.randint(4);

		predictedDiff = model.getLogLikelihoodIncrease(mutation);
        predictedLogL = model.getLogLikelihood() + predictedDiff;
        mutation.perform(model);

        if (!ALMOST_EQUALS(model.getLogLikelihood(), predictedLogL, 1e-3)) {
            std::cout << "Step #" << (i+1) << '\n'
					  << "Predicted diff = " << predictedDiff << '\n'
                      << "Predicted logL = " << predictedLogL << '\n'
					  << "Actual diff    = "
					  << model.getLogLikelihood() - (predictedLogL - predictedDiff) << '\n'
                      << "Actual logL    = " << model.getLogLikelihood() << '\n';
            return 1;
        }
    }

    return 0;
}

int main(int argc, char* argv[]) {
    srand(time(0));

    CHECK(test_getLogLikelihood);
    CHECK(test_getLogLikelihoodIncrease);

    return 0;
}
