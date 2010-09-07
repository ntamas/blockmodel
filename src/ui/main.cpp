/* vim:set ts=4 sw=4 sts=4 et: */

#include <cmath>
#include <iomanip>
#include <iostream>
#include <block/blockmodel.h>
#include <block/optimization.h>
#include <igraph/cpp/graph.h>

#include "cmd_arguments.h"

using namespace igraph;
using namespace std;

int main(int argc, char** argv) {
    CommandLineArguments args(argc, argv);
    Graph graph = Graph::Full(5) + Graph::Full(5);

	clog << "Stochastic blockmodeling\n\n";

    UndirectedBlockmodel model(&graph, args.numGroups);
    model.randomize();

    MCMCStrategy mcmc;
    mcmc.setModel(&model);

    while (1) {
        clog << '[' << setw(6) << mcmc.getStepCount() << "] "
             << setw(15) << setprecision(2) << model.getLogLikelihood() << '\n';
        model.getTypes().print();
        if (isnan(model.getLogLikelihood())) {
            return 1;
        }
        mcmc.step();
    }

	return 0;
}
