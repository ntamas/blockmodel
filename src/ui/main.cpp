/* vim:set ts=4 sw=4 sts=4 et: */

#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <block/blockmodel.h>
#include <block/optimization.h>
#include <igraph/cpp/graph.h>

#include "cmd_arguments.h"

using namespace igraph;
using namespace std;

int main(int argc, char** argv) {
    CommandLineArguments args(argc, argv);
    // Graph graph = Graph::Full(5) + Graph::Full(5);
    Graph graph = Graph::GRG(100, 0.2);

	clog << "Stochastic blockmodeling\n\n";

    UndirectedBlockmodel model(&graph, args.numGroups);
    model.randomize();

    MetropolisHastingsStrategy mcmc;
    // GibbsSamplingStrategy mcmc;
    mcmc.setModel(&model);

    double logL;
    double bestLogL = -std::numeric_limits<double>::max();

    while (1) {
        mcmc.step();

        logL = model.getLogLikelihood();
        if (bestLogL < logL)
            bestLogL = logL;

        if (mcmc.getStepCount() % args.logPeriod == 0) {
            clog << '[' << setw(6) << mcmc.getStepCount() << "] "
                 << setw(12) << logL << "\t(" << bestLogL << ")\t"
                 << (mcmc.wasLastProposalAccepted() ? '*' : ' ')
                 << setw(8) << mcmc.getAcceptanceRatio()
                 << '\n';
        }

        if (logL == 0.0) {
            model.getTypes().print();
            break;
        }
        if (isnan(logL))
            return 1;
    }

	return 0;
}
