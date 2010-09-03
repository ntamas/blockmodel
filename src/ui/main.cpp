/* vim:set ts=4 sw=4 sts=4 et: */

#include <block/blockmodel.h>
#include <iostream>

#include "cmd_arguments.h"

using namespace std;

int main(int argc, char** argv) {
    CommandLineArguments args(argc, argv);
    Graph graph(10);

	clog << "Stochastic blockmodeling\n\n";

    for (int i = 0; i < 5; i++) {
        for (int j = i+1; j < 5; j++) {
            graph.addEdge(i, j);
            graph.addEdge(i+5, j+5);
        }
    }
    graph.addEdge(0, 5);

    UndirectedBlockmodel model(&graph, 2);

	return 0;
}
