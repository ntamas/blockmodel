/* vim:set ts=4 sw=4 sts=4 et: */

#include <cmath>
#include <cstdarg>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
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
        fprintf(stderr, "\n");               \
        va_end(arglist);                     \
    }

class BlockmodelCmdlineApp {
private:
    /// Parsed command line arguments
    CommandLineArguments m_args;

    /// Graph being analyzed by the UI
    std::auto_ptr<Graph> m_pGraph;

    /// Blockmodel being fitted to the graph
    UndirectedBlockmodel* m_pModel;

    /// Markov chain Monte Carlo strategy to optimize the model
    MetropolisHastingsStrategy mcmc;

    /// Best log-likelihood found so far
    double m_bestLogL;

    /// Whether the Markov chain has converged already or not
    bool m_converged;

public:
    LOGGING_FUNCTION(debug, 2);
    LOGGING_FUNCTION(info, 1);
    LOGGING_FUNCTION(error, 0);

    /// Constructor
    BlockmodelCmdlineApp() : m_pGraph(0), m_pModel(0),
        m_bestLogL(-std::numeric_limits<double>::max()) {}

    /// Destructor
    ~BlockmodelCmdlineApp() {
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

    /// Loads a graph from the given file
    /**
     * If the name of the file is "-", the file is assumed to be the
     * standard input.
     */
    std::auto_ptr<Graph> loadGraph(const std::string& filename) {
        std::auto_ptr<Graph> result;
        FILE* fptr = stdin;

        if (filename != "-") {
            fptr = fopen(filename.c_str(), "r");
            if (fptr == NULL) {
                std::ostringstream oss;
                oss << "File not found: " << filename;
                throw std::runtime_error(oss.str());
            }
        }

        result.reset(new Graph(Graph::ReadEdgelist(fptr)));
        return result;
    }

    /// Runs a single block of the Markov chain Monte Carlo process
    /**
     * The sampled log-likelihoods are collected in the given vector.
     * The vector is cleared at the start of the process.
     */
    void runBlock(Vector& samples) {
        samples.clear();
        while (1) {
            double logL;
            mcmc.step();

            logL = m_pModel->getLogLikelihood();
            if (m_bestLogL < logL)
                m_bestLogL = logL;
            samples.push_back(logL);

            if (mcmc.getStepCount() % m_args.logPeriod == 0 && !isQuiet()) {
                clog << '[' << setw(6) << mcmc.getStepCount() << "] "
                     << setw(12) << logL << "\t(" << m_bestLogL << ")\t"
                     << (mcmc.wasLastProposalAccepted() ? '*' : ' ')
                     << setw(8) << mcmc.getAcceptanceRatio()
                     << '\n';
            }

            if (mcmc.getStepCount() % m_args.blockSize == 0) {
                // Block ended
                debug(">> optimization block ended here");
                return;
            }
        }
    }

    /// Runs the user interface
    int run(int argc, char** argv) {
        m_args.parse(argc, argv);

        if (m_args.numGroups <= 0) {
            error("Automatic group count detection not supported yet :(");
            return 1;
        }

        info(">> loading graph: %s", m_args.inputFile.c_str());
        m_pGraph = loadGraph(m_args.inputFile);

        debug(">> using random seed: %lu", m_args.randomSeed);
        mcmc.getRNG()->init_genrand(m_args.randomSeed);

        m_pModel = new UndirectedBlockmodel(m_pGraph.get(), m_args.numGroups);
        m_pModel->randomize(*mcmc.getRNG());

        if (m_args.initMethod == GREEDY) {
            GreedyStrategy greedy;
            greedy.setModel(m_pModel);

            info(">> running greedy initialization");

            while (greedy.step()) {
                double logL = m_pModel->getLogLikelihood();
                if (!isQuiet()) {
                    clog << '[' << setw(6) << greedy.getStepCount() << "] "
                         << setw(12) << logL << "\t(" << logL << ")\n";
                }
            }
        }

        mcmc.setModel(m_pModel);

        info(">> starting Markov chain");
        bool converged = false;
        double prevEstEntropy = 0.0;
        Vector samples(m_args.blockSize);

        // Run the Markov chain until convergence
        while (!converged) {
            double estEntropy;
            runBlock(samples);
            estEntropy = samples.sum() / samples.size();
            debug(">> estimated entropy of distribution: %.4f", estEntropy);
            if (std::fabs(estEntropy - prevEstEntropy) < 1.0)
                converged = true;
            prevEstEntropy = estEntropy;
        }

        info(">> chain seems to have converged to stationary state");
        m_pModel->getTypes().print();
        return 0;
    }
};

int main(int argc, char** argv) {
    BlockmodelCmdlineApp app;
    return app.run(argc, argv);
}
