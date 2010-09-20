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

    /// Runs the user interface
    int run(int argc, char** argv) {
        auto_ptr<Reader<UndirectedBlockmodel> > pModelReader;
        UndirectedBlockmodel model;
        FILE* out = NULL;

        m_args.parse(argc, argv);

        if (m_args.count == 0)
            return 0;

        if (m_args.count > 1) {
            if (m_args.outputFile == "-") {
                error("Output file must not be the standard output if "
                      "-c is given");
                return 2;
            }

            if (m_args.outputFile.find('%') == string::npos) {
                error("The name of the output file must contain a %%d "
                      "placeholder if -c is given");
                return 3;
            }
        }

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

        if (m_args.outputFile == "-")
            out = stdout;

        for (long int i = 0; i < m_args.count; i++) {
            Graph graph = model.generate();

            if (out != stdout) {
                string outputFile = generateOutputFilename(i);
                out = fopen(outputFile.c_str(), "w");
                if (out) {
                    graph.writeEdgelist(out);
                    fclose(out);
                } else {
                    error("Cannot open output file: %s", outputFile.c_str());
                    return 4;
                }
            } else {
                graph.writeEdgelist(out);
            }
        }

        return 0;
    }
};

int main(int argc, char** argv) {
    BlockmodelGeneratorApp app;
    return app.run(argc, argv);
}
