/* vim:set ts=4 sw=4 sts=4 et: */

#include <fstream>
#include <iostream>
#include <memory>
#include <block/blockmodel.h>
#include <block/io.hpp>
#include <block/util.hpp>
#include <igraph/cpp/graph.h>

#include "../common/logging.h"
#include "../common/string_util.h"
#include "cmd_arguments.h"

using namespace igraph;
using namespace std;

class BlockmodelGeneratorApp {
private:
    /// Parsed command line arguments
    CommandLineArguments m_args;

    /// The blockmodel being generated
    auto_ptr<Blockmodel> m_pModel;

public:
    LOGGING_FUNCTION(debug, 2);
    LOGGING_FUNCTION(info, 1);
    LOGGING_FUNCTION(error, 0);

    /// Constructor
    BlockmodelGeneratorApp() {}

    /// Generates the output filename for the given index
    string generateOutputFilename(int index) {
        return StringUtil::format(m_args.outputFile, index);
    }

    /// Returns whether we are running in quiet mode
    bool isQuiet() {
        return m_args.verbosity < 1;
    }

    /// Returns whether we are running in verbose mode
    bool isVerbose() {
        return m_args.verbosity > 1;
    }

    /// Reads the model from the disk
    int readModel() {
		switch (m_args.modelType) {
			case UNDIRECTED_BLOCKMODEL:
				m_pModel.reset(new UndirectedBlockmodel());
				break;

			case DEGREE_CORRECTED_UNDIRECTED_BLOCKMODEL:
				m_pModel.reset(new DegreeCorrectedUndirectedBlockmodel());
				break;

			default:
				throw std::runtime_error("invalid model type given");
		}

        info(">> loading model: %s", m_args.inputFile.c_str());

        PlainTextReader<Blockmodel> modelReader;

        try {
            if (m_args.inputFile == "-")
                modelReader.read(m_pModel.get(), cin);
            else {
                ifstream is(m_args.inputFile.c_str());
                modelReader.read(m_pModel.get(), is);
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
        MersenneTwister rng;
        FILE* out = NULL;

        m_args.parse(argc, argv);

        if (m_args.count <= 0)
            return 0;

        if (readModel())
            return 1;

        if (m_args.outputFile == "-")
            out = stdout;

        debug(">> using random seed: %lu", m_args.randomSeed);
        rng.init_genrand(m_args.randomSeed);

        for (size_t i = 0; i < m_args.count; i++) {
            Graph graph = m_pModel->generate(rng);

            if (out != stdout) {
                string outputFile = generateOutputFilename(i);
                out = fopen(outputFile.c_str(), "w");
                if (out) {
                    writeGraph(graph, out);
                    fclose(out);
                } else {
                    error("Cannot open output file: %s", outputFile.c_str());
                    return 4;
                }
            } else {
                writeGraph(graph, out);
            }
        }

        return 0;
    }

    /// Writes the given graph to the given stream
    void writeGraph(const Graph& graph, FILE* out) {
        switch (m_args.outputFormat) {
            case FORMAT_EDGELIST:
                graph.writeEdgelist(out);
                break;

            case FORMAT_LEDA:
                graph.writeLEDA(out);
                break;

            default:
                error("Invalid output format specifier. This should not happen.");
                break;
        }
    }
};

int main(int argc, char** argv) {
    BlockmodelGeneratorApp app;
    return app.run(argc, argv);
}
