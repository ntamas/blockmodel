/* vim:set ts=4 sw=4 sts=4 et: */

#include <fstream>
#include <iostream>
#include <memory>
#include <block/blockmodel.h>
#include <block/io.hpp>
#include <block/util.hpp>
#include <igraph/cpp/graph.h>

#include "../common/logging.h"
#include "cmd_arguments.h"

using namespace igraph;
using namespace std;

class BlockmodelGeneratorApp {
private:
    /// Parsed command line arguments
    CommandLineArguments m_args;

public:
    LOGGING_FUNCTION(debug, 2);
    LOGGING_FUNCTION(info, 1);
    LOGGING_FUNCTION(error, 0);

    /// Constructor
    BlockmodelGeneratorApp() {}

    /// Returns whether we are running in quiet mode
    bool isQuiet() {
        return m_args.verbosity < 1;
    }

    /// Returns whether we are running in verbose mode
    bool isVerbose() {
        return m_args.verbosity > 1;
    }

    /// Runs the user interface
    int run(int argc, char** argv) {
        std::auto_ptr<Reader<UndirectedBlockmodel> > pModelReader;
        UndirectedBlockmodel model;

        m_args.parse(argc, argv);
        
        switch (m_args.inputFormat) {
            default:
                pModelReader.reset(new PlainTextReader<UndirectedBlockmodel>);
                break;
        };

        try {
            if (m_args.inputFile == "-")
                pModelReader->read(model, std::cin);
            else {
                std::ifstream is(m_args.inputFile.c_str());
                pModelReader->read(model, is);
            }
        } catch (const std::runtime_error& ex) {
            error("cannot read input file: %s", m_args.inputFile.c_str());
            error(ex.what());
            return 1;
        }

        for (long int i = 0; i < m_args.count; i++) {
            Graph graph = model.generate();
            graph.writeEdgelist(stdout);
        }

        return 0;
    }
};

int main(int argc, char** argv) {
    BlockmodelGeneratorApp app;
    return app.run(argc, argv);
}
