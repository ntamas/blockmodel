/* vim:set ts=4 sw=4 sts=4 et: */

#include <cstdlib>
#include <block/blockmodel.h>
#include <igraph/cpp/graph.h>
#include <mtwister/mt.h>

#include "test_common.cpp"

using namespace igraph;

int test_setType() {
    Graph graph = Graph::GRG(100, 0.2);
    UndirectedBlockmodel model(&graph, 5);

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
    /* Disjoint union of two full graphs with loops */
    Graph graph = Graph::Full(5) + Graph::Full(5);
    UndirectedBlockmodel model(&graph, 2);

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
    Graph graph = Graph::Full(5) + Graph::Full(5);
    UndirectedBlockmodel model(&graph, 2);

    for (int i = 0; i < 10; i++)
        model.setType(i, i / 5);

    Matrix expected(2, 2);
    expected(0, 0) = expected(1, 1) = 1.0;

    if (model.getProbabilities() != expected)
        return 1;

    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            if (model.getProbability(i, j) != expected(i, j))
                return 100 + 10 * i + j;

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

int test_getTotalEdgesFromAffectedGroups() {
    /* Disjoint union of two full graphs */
    Graph graph = Graph::Full(5) + Graph::Full(5);
    UndirectedBlockmodel model(&graph, 2);
    Vector counts0, counts1;

    for (int i = 0; i < 10; i++)
        model.setType(i, i / 5);

    /* Smoke testing first */
    {
        PointMutation mutation(0, 0, 1);

        model.getTotalEdgesFromAffectedGroups(mutation, counts0, counts1);
        if (counts0[0] != 6 || counts0[1] != 24)
            return 1;
        if (counts1[0] != 24 || counts1[1] != 15)
            return 2;

        mutation.to = 0;
        model.getTotalEdgesFromAffectedGroups(mutation, counts0, counts1);
        if (counts0[0] != 10 || counts0[1] != 25)
            return 3;
        if (counts0 != counts1)
            return 4;
    }

    /* Try five groups, do many random mutations */
    MersenneTwister rng;
    model = UndirectedBlockmodel(&graph, 5);
    for (int i = 0; i < 10; i++)
        model.setType(i, i / 2);
    for (int i = 0; i < 10000; i++) {
        int from = rng.randint(10);
        PointMutation mutation(from, model.getType(from), rng.randint(5));
        model.getTotalEdgesFromAffectedGroups(mutation, counts0, counts1);
        model.setType(from, mutation.to);

        for (int j = 0; j < 5; j++) {
            if (model.getTotalEdgesBetweenGroups(j, mutation.from) !=
                    counts0[j])
                return 5;
            if (model.getTotalEdgesBetweenGroups(j, mutation.to) !=
                    counts1[j])
                return 6;
        }
    }

    return 0;
}

int main(int argc, char* argv[]) {
    srand(time(0));

    CHECK(test_setType);
    CHECK(test_getProbabilities);
    CHECK(test_getLogLikelihood);
    CHECK(test_getTotalEdgesFromAffectedGroups);

    return 0;
}
