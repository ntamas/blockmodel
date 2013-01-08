/* vim:set ts=4 sw=4 sts=4 et: */

#include <cstdlib>
#include <block/blockmodel.h>
#include <igraph/cpp/graph.h>
#include <igraph/cpp/generators/full.h>
#include <igraph/cpp/generators/grg.h>
#include <mtwister/mt.h>

#include "test_common.cpp"

using namespace igraph;

int test_setType() {
    Graph graph = *grg_game(100, 0.2);
    UndirectedBlockmodel model = Blockmodel::create<UndirectedBlockmodel>(&graph, 5);

    Vector types;
    Vector typeCounts;
    Matrix edgeCounts;

    /* Set a few types randomly */
    for (int i = 0; i < 10000; i++) {
        model.setType(rand() % 100, rand() % 5);
    }

    /* Get the type count vector and the type vector */
    types = model.getTypes();
    typeCounts = model.getTypeCounts();
    if (typeCounts.size() != 5)
        return 1;

    /* Check the validity of the type count vector */
    for (int i = 0; i < 5; i++) {
        if (std::count(types.begin(), types.end(), i) != typeCounts[i])
            return 100+i;
    }

    /* Check the validity of the edge count matrix */
    edgeCounts = model.getEdgeCounts();
    if (edgeCounts.sum() != 2 * graph.ecount())
        return 2;

    return 0;
}

int test_getLogLikelihood() {
    /* Disjoint union of two full graphs */
    Graph graph = *full(5) + *full(5);
    UndirectedBlockmodel model = Blockmodel::create<UndirectedBlockmodel>(&graph, 2);

    for (int i = 0; i < 10; i++)
        model.setType(i, i / 5);

    if (model.getLogLikelihood() != 0.0)
        return 1;

    /* Pathological case: no edge between vertices of type 1 */
    for (int i = 0; i < 10; i++)
        model.setType(i, (i == 3) ? 1 : 0);
    if (!ALMOST_EQUALS(model.getLogLikelihood(), -30.913270946, 1e-8))
        return 2;

    /* Even more pathological case: no vertices of type 1 */
    for (int i = 0; i < 10; i++)
        model.setType(i, 0);
    if (!ALMOST_EQUALS(model.getLogLikelihood(), -30.913270946, 1e-8))
        return 3;

    return 0;
}

int test_getProbabilities() {
    /* Disjoint union of two full graphs */
    Graph graph = *full(5) + *full(5);
    UndirectedBlockmodel model = Blockmodel::create<UndirectedBlockmodel>(&graph, 2);

    for (int i = 0; i < 10; i++)
        model.setType(i, i / 5);

    Matrix expected(2, 2);
    expected(0, 0) = expected(1, 1) = 1.0;

    if (model.getProbabilities() != expected)
        return 1;

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++)
            if (model.getProbability(i, j) != expected(i, j))
                return 100 + 10 * i + j;
        if (model.getProbabilitiesFromGroup(i) != expected.getRow(i))
            return 100 + 10 * i + 9;
    }

    /* Pathological case: no edge between vertices of type 1 */
    for (int i = 0; i < 10; i++)
        model.setType(i, (i == 3) ? 1 : 0);

    expected(0, 0) = expected(0, 1) = expected(1, 0) = 4/9.0;
    expected(1, 1) = 0;
    if (model.getProbabilities().maxdifference(expected) > 1e-8)
        return 2;

    /* Even more pathological case: no vertices of type 1 */
    for (int i = 0; i < 10; i++)
        model.setType(i, 0);

    expected(0, 0) = 4/9.0;
    expected(0, 1) = expected(1, 0) = expected(1, 1) = 0;
    if (model.getProbabilities().maxdifference(expected) > 1e-8)
        return 3;

    return 0;
}

int test_getTotalAndActualEdgesFromAffectedGroups() {
    /* Disjoint union of two full graphs */
    Graph graph = *full(5) + *full(5);
    UndirectedBlockmodel model = Blockmodel::create<UndirectedBlockmodel>(&graph, 2);
    Vector counts0, counts1, counts2, counts3;

    for (int i = 0; i < 10; i++)
        model.setType(i, i / 5);

    /* Smoke testing first */
    {
        PointMutation mutation(0, 0, 1);

        model.getTotalEdgesFromAffectedGroupsAfter(mutation, counts0, counts1);
        if (counts0[0] != 12 || counts0[1] != 24)
            return 1;
        if (counts1[0] != 24 || counts1[1] != 30)
            return 2;
        model.getEdgeCountsFromAffectedGroupsAfter(mutation, counts0, counts1);
        if (counts0[0] != 12 || counts0[1] != 4)
            return 3;
        if (counts1[0] !=  4 || counts1[1] != 20)
            return 4;

        mutation.to = 0;
        model.getTotalEdgesFromAffectedGroupsAfter(mutation, counts0, counts1);
        if (counts0[0] != 20 || counts0[1] != 25)
            return 5;
        if (counts0 != counts1)
            return 6;
        model.getEdgeCountsFromAffectedGroupsAfter(mutation, counts0, counts1);
        if (counts0[0] != 20 || counts0[1] != 0)
            return 7;
        if (counts1 != counts0)
            return 8;
    }

    /* Try five groups, do many random mutations */
    MersenneTwister rng;
    model = Blockmodel::create<UndirectedBlockmodel>(&graph, 5);
    for (int i = 0; i < 10; i++)
        model.setType(i, i / 2);
    for (int i = 0; i < 10000; i++) {
        int from = rng.randint(10);
        PointMutation mutation(from, model.getType(from), rng.randint(5));
        model.getTotalEdgesFromAffectedGroupsAfter(mutation, counts0, counts1);
        model.getEdgeCountsFromAffectedGroupsAfter(mutation, counts2, counts3);
        model.performMutation(mutation);

        for (int j = 0; j < 5; j++) {
            if (model.getTotalEdgesBetweenGroups(j, mutation.from) !=
                    counts0[j])
                return 9;
            if (model.getTotalEdgesBetweenGroups(j, mutation.to) !=
                    counts1[j])
                return 10;
            if (model.getEdgeCount(j, mutation.from) != counts2[j])
                return 11;
            if (model.getEdgeCount(j, mutation.to) != counts3[j])
                return 12;
        }
    }

    return 0;
}

int test_getLogLikelihoodIncrease() {
    /* Disjoint union of two full graphs */
    Graph graph = *full(5) + *full(5);
    UndirectedBlockmodel model = Blockmodel::create<UndirectedBlockmodel>(&graph, 5);
    MersenneTwister rng;
    double predictedLogL;

    /* Try five groups, do many random mutations */
    for (int i = 0; i < 10; i++)
        model.setType(i, i / 2);
    for (int i = 0; i < 10000; i++) {
        int from = rng.randint(10);
        PointMutation mutation(from, model.getType(from), rng.randint(5));
        while (mutation.from == mutation.to)
            mutation.to = rng.randint(5);

        predictedLogL = model.getLogLikelihood() +
            model.getLogLikelihoodIncrease(mutation);
        model.performMutation(mutation);

        if (!ALMOST_EQUALS(model.getLogLikelihood(), predictedLogL, 1e-3)) {
            std::cout << "Step #" << (i+1) << '\n'
                      << "Predicted logL = " << predictedLogL << '\n'
                      << "Actual logL    = " << model.getLogLikelihood() << '\n';
            return 1;
        }
    }

    return 0;
}

int main(int argc, char* argv[]) {
    srand(time(0));

    CHECK(test_setType);
    CHECK(test_getProbabilities);
    CHECK(test_getLogLikelihood);
    CHECK(test_getTotalAndActualEdgesFromAffectedGroups);
    CHECK(test_getLogLikelihoodIncrease);

    return 0;
}
