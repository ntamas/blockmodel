/* vim:set ts=4 sw=4 sts=4 et: */

#include <cmath>
#include <csignal>
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
    MetropolisHastingsStrategy m_mcmc;

    /// Best log-likelihood found so far
    double m_bestLogL;

    /// Best model found so far
    UndirectedBlockmodel m_bestModel;

    /// Flag to note whether we have to dump the best state when possible
    bool m_dumpBestStateFlag;

public:
    LOGGING_FUNCTION(debug, 2);
    LOGGING_FUNCTION(info, 1);
    LOGGING_FUNCTION(error, 0);

    /// Constructor
    BlockmodelCmdlineApp() : m_pGraph(0), m_pModel(0),
        m_bestLogL(-std::numeric_limits<double>::max()),
        m_bestModel(), m_dumpBestStateFlag(false) {}

    /// Destructor
    ~BlockmodelCmdlineApp() {
        if (m_pModel)
            delete m_pModel;
    }

    /// Dumps the best state found so far and clears the dump flag
    void dumpBestState() {
        clog << "TODO: implement state dumping\n";
        m_dumpBestStateFlag = false;
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

    /// Dumps the best state of the model to a file on the next occasion
    /**
     * This function is called by the SIGUSR1 signal handler to signal that
     * we should dump the best state as soon as possible. We cannot dump
     * it here directly as we might be modifying it at the same time.
     */
    void raiseDumpBestStateFlag() {
        m_dumpBestStateFlag = true;
    }

    /// Runs a single block of the Markov chain Monte Carlo process
    /**
     * The sampled log-likelihoods are collected in the given vector.
     * The vector is cleared at the start of the process.
     */
    void runBlock(long numSamples, Vector& samples) {
        double logL;

        samples.clear();
        while (numSamples > 0) {
            m_mcmc.step();

            logL = m_pModel->getLogLikelihood();
            if (m_bestLogL < logL) {
                // Store the best model and log-likelihood
                m_bestModel = *m_pModel;
                m_bestLogL = logL;
            }
            samples.push_back(logL);

            if (m_mcmc.getStepCount() % m_args.logPeriod == 0 && !isQuiet()) {
                clog << '[' << setw(6) << m_mcmc.getStepCount() << "] "
                     << setw(12) << logL << "\t(" << m_bestLogL << ")\t"
                     << (m_mcmc.wasLastProposalAccepted() ? '*' : ' ')
                     << setw(8) << m_mcmc.getAcceptanceRatio()
                     << '\n';
            }

            if (m_dumpBestStateFlag)
                dumpBestState();

            numSamples--;
        }
    }

    /// Runs the sampling until hell freezes over
    void runUntilHellFreezesOver() {
        Vector dummy;

        while (1)
            runBlock(1000, dummy);
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
        m_mcmc.getRNG()->init_genrand(m_args.randomSeed);

        m_pModel = new UndirectedBlockmodel(m_pGraph.get(), m_args.numGroups);
        m_pModel->randomize(*m_mcmc.getRNG());

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

        m_bestModel = *m_pModel;
        m_bestLogL = m_pModel->getLogLikelihood();
        m_mcmc.setModel(m_pModel);

        info(">> starting Markov chain");
        bool converged = false;
        double prevEstEntropy = 0.0, estEntropy;
        Vector samples(m_args.blockSize);

        // Run the Markov chain until convergence
        while (!converged) {
            runBlock(m_args.blockSize, samples);
            estEntropy = samples.sum() / samples.size();
            debug(">> estimated entropy of distribution: %.4f", estEntropy);
            if (std::fabs(estEntropy - prevEstEntropy) < 1.0)
                converged = true;
            prevEstEntropy = estEntropy;
        }

        if (m_args.numSamples > 0) {
            /* taking a finite number of samples */
            info(">> convergence condition satisfied, taking %d samples", m_args.numSamples);
            samples.reserve(m_args.numSamples);
            runBlock(m_args.numSamples, samples);
        } else {
            /* leave the Markov chain running anyway */
            info(">> convergence condition satisfied, leaving the chain running anyway");
            samples.reserve(m_args.numSamples);
            runUntilHellFreezesOver();
        }

        return 0;
    }
};

// Global app instance. It is here because we need it in the signal handler
BlockmodelCmdlineApp app;

/// Signal handler called for SIGUSR1
void handleSIGUSR1(int signum) {
    app.raiseDumpBestStateFlag();
}

int main(int argc, char** argv) {
#ifdef SIGUSR1
    signal(SIGUSR1, handleSIGUSR1);
#endif
    // Register the signal handler for SIGUSR1
    return app.run(argc, argv);
}
