/* vim:set ts=4 sw=4 sts=4 et: */

#include <fstream>
#include <iostream>
#include <memory>
#include <block/blockmodel.h>
#include <block/io.hpp>
#include <block/optimization.h>
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

public:
    LOGGING_FUNCTION(debug, 2);
    LOGGING_FUNCTION(info, 1);
    LOGGING_FUNCTION(error, 0);

    /// Constructor
    BlockmodelPredictionApp() {}

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
        auto_ptr<Reader<UndirectedBlockmodel> > pModelReader;

        info(">> loading model: %s", m_args.inputFile.c_str());

        switch (m_args.inputFormat) {
            default:
                pModelReader.reset(new PlainTextReader<UndirectedBlockmodel>);
                break;
        };

        try {
            if (m_args.inputFile == "-")
                pModelReader->read(model, cin);
            else {
                ifstream is(m_args.inputFile.c_str());
                pModelReader->read(model, is);
            }
        } catch (const runtime_error& ex) {
            error("Cannot read input file: %s", m_args.inputFile.c_str());
            error(ex.what());
            return 1;
        }

        return 0;
    }

    /// Runs the user interface
    int run(int argc, char** argv) {
        UndirectedBlockmodel model;

        m_args.parse(argc, argv);

        if (m_args.sampleCount <= 0)
            return 0;

        if (readModel(model))
            return 1;

        debug(">> using random seed: %lu", m_args.randomSeed);
        m_mcmc.getRNG()->init_genrand(m_args.randomSeed);

        return 0;
    }
};

int main(int argc, char** argv) {
    BlockmodelPredictionApp app;
    return app.run(argc, argv);
}
