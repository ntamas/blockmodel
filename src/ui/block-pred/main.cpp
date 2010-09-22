/* vim:set ts=4 sw=4 sts=4 et: */

#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <block/blockmodel.h>
#include <block/io.hpp>
#include <block/optimization.h>
#include <block/prediction.h>
#include <block/util.hpp>
#include <igraph/cpp/graph.h>

#include "../common/logging.h"
#include "../common/string_util.h"
#include "cmd_arguments.h"

using namespace igraph;
using namespace std;

class BlockmodelPredictionApp {
private:
    /// Parsed command line arguments
    CommandLineArguments m_args;

    /// Markov chain Monte Carlo strategy to optimize the model
    MetropolisHastingsStrategy m_mcmc;

    /// Graph that was used to fit the model to
    auto_ptr<Graph> m_pGraph;

public:
    LOGGING_FUNCTION(debug, 2);
    LOGGING_FUNCTION(info, 1);
    LOGGING_FUNCTION(warning, 1);
    LOGGING_FUNCTION(error, 0);

    /// Constructor
    BlockmodelPredictionApp() : m_args(), m_mcmc(), m_pGraph(0) {}

    /// Returns whether we are running in quiet mode
    bool isQuiet() {
        return m_args.verbosity < 1;
    }

    /// Returns whether we are running in verbose mode
    bool isVerbose() {
        return m_args.verbosity > 1;
    }

    /// Reads the model from the disk
    int readModel(UndirectedBlockmodel& model) {
        PlainTextReader<UndirectedBlockmodel> modelReader;

        info(">> loading model: %s", m_args.inputFile.c_str());

        try {
            if (m_args.inputFile == "-")
                modelReader.read(model, cin);
            else {
                ifstream is(m_args.inputFile.c_str());
                modelReader.read(model, is);
            }
        } catch (const runtime_error& ex) {
            error("Cannot read input file: %s", m_args.inputFile.c_str());
            error(ex.what());
            return 1;
        }

        if (!modelReader.getOriginalFilename().empty()) {
            FILE* f = fopen(modelReader.getOriginalFilename().c_str(), "r");
            if (f != NULL) {
                m_pGraph.reset(new Graph(Graph::ReadEdgelist(f)));
                model.setGraph(m_pGraph.get());
                fclose(f);
            } else {
                warning(">> cannot load original graph `%s', continuing anyway",
                        modelReader.getOriginalFilename().c_str());
            }
        }

        return 0;
    }

    /// Runs the user interface
    int run(int argc, char** argv) {
        UndirectedBlockmodel model;
        double logL, bestLogL;
        MersenneTwister* rng = m_mcmc.getRNG();

        m_args.parse(argc, argv);

        if (m_args.sampleCount <= 0)
            return 0;

        if (readModel(model))
            return 1;

        debug(">> using random seed: %lu", m_args.randomSeed);
        rng->init_genrand(m_args.randomSeed);

        m_mcmc.setModel(&model);
        
        // If we have requested only one sample, then always take the one
        // we have in the beginning
        if (m_args.sampleCount == 1)
            m_args.samplingFreq = 1.0;

        if (model.getGraph() == NULL && m_args.sampleCount > 1) {
            error("Loaded model has no associated graph; cannot start sampling.");
            return 2;
        }

        info(">> starting Markov chain");
        bestLogL = model.getLogLikelihood();
        auto_ptr<Predictor> pPredictor(new AveragingPredictor(&model));

        /* Start taking samples */
        while (1) {
            if (rng->random() < m_args.samplingFreq)
                pPredictor->takeSample();

            if (pPredictor->getSampleCount() >= m_args.sampleCount)
                break;

            m_mcmc.step();

            logL = model.getLogLikelihood();
            if (bestLogL < logL)
                bestLogL = logL;

            if (m_mcmc.getStepCount() % m_args.logPeriod == 0 && !isQuiet()) {
                clog << '[' << setw(6) << m_mcmc.getStepCount() << "] "
                     << '(' << setw(6) << pPredictor->getSampleCount() << ") "
                     << setw(12) << logL << "\t(" << bestLogL << ")\t"
                     << (m_mcmc.wasLastProposalAccepted() ? '*' : ' ')
                     << setw(8) << m_mcmc.getAcceptanceRatio()
                     << '\n';
            }
        }

        /* Okay, list the predictions */
        size_t n = model.getVertexCount();

        if (m_args.sort) {
            typedef vector<pair<pair<int, int>, double> > Predictions;
            Predictions preds;
            preds.reserve(n * (n-1) / 2);
            for (size_t v1 = 0; v1 < n; v1++)
                for (size_t v2 = v1+1; v2 < n; v2++)
                    preds.push_back(make_pair(
                                make_pair(v1, v2),
                                pPredictor->predictProbability(v1, v2)
                    ));

            pair_comparator<2> cmp;
            make_heap(preds.begin(), preds.end(), cmp);

            while (!preds.empty()) {
                Predictions::const_iterator it = preds.begin();
                cout << (it->first.first) << '\t' << (it->first.second) << '\t'
                     << it->second << '\n';
                pop_heap(preds.begin(), preds.end(), cmp);
                preds.pop_back();
            }
        } else {
            for (size_t v1 = 0; v1 < n; v1++)
                for (size_t v2 = v1+1; v2 < n; v2++)
                    cout << v1 << '\t' << v2 << '\t'
                         << pPredictor->predictProbability(v1, v2) << '\n';
        }

        return 0;
    }
};

int main(int argc, char** argv) {
    BlockmodelPredictionApp app;

    igraph::AttributeHandler::attach();
    return app.run(argc, argv);
}
