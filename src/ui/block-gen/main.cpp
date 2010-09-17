/* vim:set ts=4 sw=4 sts=4 et: */

#include <iostream>
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
        m_args.parse(argc, argv);
        
        // TODO

        /*
        switch (m_args.outputFormat) {
            case FORMAT_JSON:
                m_pModelWriter.reset(new JSONWriter<UndirectedBlockmodel>);
                break;

			case FORMAT_NULL:
                m_pModelWriter.reset(new NullWriter<UndirectedBlockmodel>);
                break;

            default:
                m_pModelWriter.reset(new PlainTextWriter<UndirectedBlockmodel>);
        };
        */

        return 0;
    }
};

int main(int argc, char** argv) {
    BlockmodelGeneratorApp app;
    return app.run(argc, argv);
}
