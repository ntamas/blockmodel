/* vim:set ts=4 sw=4 sts=4 et: */

#include <cmath>
#include <cstdarg>
#include <iomanip>
#include <iostream>
#include <limits>
#include <block/blockmodel.h>
#include <block/optimization.h>
#include <igraph/cpp/graph.h>

#include "cmd_arguments.h"

using namespace igraph;
using namespace std;

CommandLineArguments args;

#define LOGGING_FUNCTION(funcname, level) \
    void funcname(const char* format, ...) { \
        va_list arglist;                     \
        if (args.verbosity < level)          \
            return;                          \
        va_start(arglist, format);           \
        vfprintf(stderr, format, arglist);   \
        va_end(arglist);                     \
    }

LOGGING_FUNCTION(debug, 2);
LOGGING_FUNCTION(info, 1);
LOGGING_FUNCTION(error, 0);

int main(int argc, char** argv) {
    args.parse(argc, argv);

    if (args.numGroups <= 0) {
        error("Automatic group count detection not supported yet :(\n");
        return 1;
    }

    // Graph graph = Graph::Full(5) + Graph::Full(5);
    Graph graph = Graph::GRG(100, 0.2);

    UndirectedBlockmodel model(&graph, args.numGroups);
    model.randomize();

    if (args.initMethod == GREEDY) {
        GreedyStrategy greedy;
        greedy.setModel(&model);

        info(">> running greedy initialization\n");
        model.getTypes().print();

        while (greedy.step()) {
            double logL = model.getLogLikelihood();
            if (args.verbosity > 0) {
                clog << '[' << setw(6) << greedy.getStepCount() << "] "
                     << setw(12) << logL << "\t(" << logL << ")\n";
            }
            model.getTypes().print();
        }

        return 0;
    }

    MetropolisHastingsStrategy mcmc;
    mcmc.setModel(&model);

    double logL;
    double bestLogL = -std::numeric_limits<double>::max();

    info(">> starting Markov chain\n");

    while (1) {
        mcmc.step();

        logL = model.getLogLikelihood();
        if (bestLogL < logL)
            bestLogL = logL;

        if (mcmc.getStepCount() % args.logPeriod == 0 && args.verbosity >= 1) {
            clog << '[' << setw(6) << mcmc.getStepCount() << "] "
                 << setw(12) << logL << "\t(" << bestLogL << ")\t"
                 << (mcmc.wasLastProposalAccepted() ? '*' : ' ')
                 << setw(8) << mcmc.getAcceptanceRatio()
                 << '\n';
        }

        if (isnan(logL))
            return 1;
    }

	return 0;
}
