/* vim:set ts=4 sw=4 sts=4 et: */

#include <cstdlib>
#include <igraph/cpp/graph.h>
#include <block/blockmodel.h>
#include <block/optimization.h>

#include "test_common.cpp"

using namespace igraph;

int test_two_rings() {
    Graph graph = Graph::Ring(5) + Graph::Ring(5);
    UndirectedBlockmodel model(&graph, 2);
    Vector types(10);
    GreedyStrategy greedy;

    /* First, set up the optimal configuration and see if we stay there */
    for (int i = 0; i < 10; i++)
        types[i] = i/5;
    model.setTypes(types);

    greedy.optimize(model);
    if (model.getTypes() != types)
        return 1;

    /* Change one element and see if we converge back */
    model.setType(0, 1);
    greedy.optimize(model);
    if (model.getTypes() != types)
        return 2;

    return 0;
}

int test_four_almost_cliques() {
    Graph graph = Graph::Full(4) + Graph::Full(4) +
                  Graph::Full(4) + Graph::Full(4);
    UndirectedBlockmodel model(&graph, 4);
    Vector types(16);
    GreedyStrategy greedy;

    /* Remove one edge from each clique */
    Vector edges(8);
    edges[0] =  0; edges[1] =  1; edges[2] =  5; edges[3] =  6;
    edges[4] = 10; edges[5] = 11; edges[6] = 15; edges[7] = 12;
    graph.deleteEdges(EdgeSelector::Pairs(edges));

    /* Every clique is colored differently */
    for (int i = 0; i < 16; i++)
        types[i] = i / 4;
    model.setTypes(types);

    /* Check if we stay in this configuration */
    greedy.optimize(model);
    if (model.getTypes() != types)
        return 1;

    /* Mutate one color in each clique */
    for (int i = 0; i < 4; i++)
        model.setType(i*4 + rand() % 4, rand() % 4);
    model.setTypes(types);

    /* Check if we stay in this configuration */
    greedy.optimize(model);
    if (model.getTypes() != types) {
        model.getTypes().print();
        return 2;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    srand(time(0));

    CHECK(test_two_rings);
    CHECK(test_four_almost_cliques);
    return 0;
}

