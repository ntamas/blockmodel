/* vim:set ts=4 sw=4 sts=4 et: */

#include <cmath>
#include <cstdarg>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <block/blockmodel.h>
#include <block/optimization.h>
#include <igraph/cpp/graph.h>

#include "cmd_arguments.h"

using namespace igraph;
using namespace std;

#define LOGGING_FUNCTION(funcname, level) \
    void funcname(const char* format, ...) { \
        va_list arglist;                     \
        if (m_args.verbosity < level)          \
            return;                          \
        va_start(arglist, format);           \
        vfprintf(stderr, format, arglist);   \
        va_end(arglist);                     \
    }

class BlockmodelCmdlineApp {
private:
    /// Parsed command line arguments
    CommandLineArguments m_args;

    /// Graph being analyzed by the UI
    Graph* m_pGraph;

    /// Blockmodel being fitted to the graph
    UndirectedBlockmodel* m_pModel;

    /// Markov chain Monte Carlo strategy to optimize the model
    MetropolisHastingsStrategy mcmc;

public:
    LOGGING_FUNCTION(debug, 2);
    LOGGING_FUNCTION(info, 1);
    LOGGING_FUNCTION(error, 0);

    /// Constructor
    BlockmodelCmdlineApp() : m_pGraph(0), m_pModel(0) {}

    /// Destructor
    ~BlockmodelCmdlineApp() {
        if (m_pGraph)
            delete m_pGraph;
        if (m_pModel)
            delete m_pModel;
    }
    
    /// Returns whether we are running in quiet mode
    bool isQuiet() {
        return m_args.verbosity < 1;
    }

    /// Returns whether we are running in verbose mode
    bool isVerbose() {
        return m_args.verbosity > 2;
    }

    /// Runs the user interface
    int run(int argc, char** argv) {
        m_args.parse(argc, argv);

        if (m_args.numGroups <= 0) {
            error("Automatic group count detection not supported yet :(\n");
            return 1;
        }

        // Graph graph = Graph::Full(5) + Graph::Full(5);
        m_pGraph = new Graph(Graph::GRG(100, 0.2));
        m_pModel = new UndirectedBlockmodel(m_pGraph, m_args.numGroups);
        m_pModel->randomize();

        if (m_args.initMethod == GREEDY) {
            GreedyStrategy greedy;
            greedy.setModel(m_pModel);

            info(">> running greedy initialization\n");
            m_pModel->getTypes().print();

            while (greedy.step()) {
                double logL = m_pModel->getLogLikelihood();
                if (!isQuiet()) {
                    clog << '[' << setw(6) << greedy.getStepCount() << "] "
                         << setw(12) << logL << "\t(" << logL << ")\n";
                }
                m_pModel->getTypes().print();
            }

            return 0;
        }

        mcmc.setModel(m_pModel);

        double logL;
        double bestLogL = -std::numeric_limits<double>::max();

        info(">> starting Markov chain\n");

        {
            Graph graph2;
            graph2 = *m_pGraph;
        }

        while (1) {
            mcmc.step();

            logL = m_pModel->getLogLikelihood();
            if (bestLogL < logL)
                bestLogL = logL;

            if (mcmc.getStepCount() % m_args.logPeriod == 0 && !isQuiet()) {
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
};

int main(int argc, char** argv) {
    BlockmodelCmdlineApp app;
    return app.run(argc, argv);
}
