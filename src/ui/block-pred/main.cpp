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

    /// Markov chain Monte Carlo strategy to sample the model
    MetropolisHastingsStrategy m_mcmc;

    /// Mapping of vertex IDs to names (if necessary)
    /**
     * \todo use vetex attributes from igraph
     */
    vector<string> m_nameMapping;

    /// Graph that was used to fit the model to
    auto_ptr<Graph> m_pGraph;

    /// The model being sampled
    auto_ptr<UndirectedBlockmodel> m_pModel;

    /// Predictor that is used to calculate the probability of new edges
    auto_ptr<Predictor> m_pPredictor;

public:
    LOGGING_FUNCTION(debug, 2);
    LOGGING_FUNCTION(info, 1);
    LOGGING_FUNCTION(warning, 1);
    LOGGING_FUNCTION(error, 0);

    /// Constructor
    BlockmodelPredictionApp() : m_args(), m_mcmc(),
        m_pGraph(0), m_pModel(new UndirectedBlockmodel), m_pPredictor(0) {}

    /// Returns whether we are running in quiet mode
    bool isQuiet() {
        return m_args.verbosity < 1;
    }

    /// Returns whether we are running in verbose mode
    bool isVerbose() {
        return m_args.verbosity > 1;
    }

    /// Lists the predictions using the predictor
    /**
     * \param  map  a mapping-like object that assigns names to vertex IDs
     */
    template <typename MapType>
    void listPredictions(const MapType& map) {
        size_t n = m_pModel->getVertexCount();

        if (m_args.sort) {
            typedef vector<pair<pair<int, int>, double> > Predictions;
            Predictions preds;
            preds.reserve(n * (n-1) / 2);

            info(">> sorting and listing predictions");

            for (size_t v1 = 0; v1 < n; v1++)
                for (size_t v2 = v1+1; v2 < n; v2++)
                    preds.push_back(make_pair(
                                make_pair(v1, v2),
                                m_pPredictor->predictProbability(v1, v2)
                    ));

            pair_comparator<2> cmp;
            make_heap(preds.begin(), preds.end(), cmp);

            while (!preds.empty()) {
                Predictions::const_iterator it = preds.begin();
                cout << map[it->first.first]  << '\t'
                     << map[it->first.second] << '\t'
                     << it->second << '\n';
                pop_heap(preds.begin(), preds.end(), cmp);
                preds.pop_back();
            }
        } else {
            info(">> listing predictions");

            for (size_t v1 = 0; v1 < n; v1++)
                for (size_t v2 = v1+1; v2 < n; v2++)
                    cout << map[v1] << '\t' << map[v2] << '\t'
                         << m_pPredictor->predictProbability(v1, v2) << '\n';
        }
    }

    /// Reads the model from the disk
    int readModel() {
        return readModel(*m_pModel);
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

    /// Reads the name mapping file from the disk
    int readNameMapping() {
        if (m_args.nameMappingFile.empty())
            return 0;

        info(">> loading name mapping: %s", m_args.nameMappingFile.c_str());

        m_nameMapping.clear();

        ifstream is(m_args.nameMappingFile.c_str());
        string line;

        while (!is.eof()) {
            if (is.fail()) {
                error("IO error while reading name mapping file, exiting...");
                return 2;
            }

            getline(is, line);
            m_nameMapping.push_back(line);
        }
        is.close();
        if (!m_nameMapping.empty() && m_nameMapping.back() == "")
            m_nameMapping.pop_back();
        info(">> loaded names for %d vertices", m_nameMapping.size());

        if (m_nameMapping.size() < m_pModel->getVertexCount()) {
            warning(">> numeric IDs will still be used for vertices without names");
            while (m_nameMapping.size() < m_pModel->getVertexCount()) {
                m_nameMapping.push_back(
                    StringUtil::format("%ld", (long)m_nameMapping.size())
                );
            }
        }

        return 0;
    }

    /// Runs the user interface
    int run(int argc, char** argv) {
        double logL, bestLogL;
        MersenneTwister* rng = m_mcmc.getRNG();

        m_args.parse(argc, argv);

        if (m_args.sampleCount <= 0)
            return 0;

        if (readModel())
            return 1;
        if (readNameMapping())
            return 2;

        debug(">> using random seed: %lu", m_args.randomSeed);
        rng->init_genrand(m_args.randomSeed);

        m_mcmc.setModel(m_pModel.get());
        
        // If we have requested only one sample, then always take the one
        // we have in the beginning
        if (m_args.sampleCount == 1)
            m_args.samplingFreq = 1.0;

        if (m_pModel->getGraph() == NULL && m_args.sampleCount > 1) {
            error("Loaded model has no associated graph; cannot start sampling.");
            return 2;
        }

        info(">> starting Markov chain");
        bestLogL = m_pModel->getLogLikelihood();
        m_pPredictor.reset(new AveragingPredictor(m_pModel.get()));

        /* Start taking samples */
        while (1) {
            if (rng->random() < m_args.samplingFreq)
                m_pPredictor->takeSample();

            if (m_pPredictor->getSampleCount() >= m_args.sampleCount)
                break;

            m_mcmc.step();

            logL = m_pModel->getLogLikelihood();
            if (bestLogL < logL)
                bestLogL = logL;

            if (m_mcmc.getStepCount() % m_args.logPeriod == 0 && !isQuiet()) {
                clog << '[' << setw(6) << m_mcmc.getStepCount() << "] "
                     << '(' << setw(6) << m_pPredictor->getSampleCount() << ") "
                     << setw(12) << logL << "\t(" << bestLogL << ")\t"
                     << (m_mcmc.wasLastProposalAccepted() ? '*' : ' ')
                     << setw(8) << m_mcmc.getAcceptanceRatio()
                     << '\n';
            }
        }

        info(">> sampling finished");

        /* Okay, list the predictions */
        if (!m_nameMapping.empty())
            listPredictions(m_nameMapping);
        else
            listPredictions(identity_map<>());

        return 0;
    }
};

int main(int argc, char** argv) {
    BlockmodelPredictionApp app;

    igraph::AttributeHandler::attach();
    return app.run(argc, argv);
}
