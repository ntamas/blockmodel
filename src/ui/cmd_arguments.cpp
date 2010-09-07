/* vim:set ts=4 sw=4 sts=4 et: */

#include <iostream>
#include <string>
#include <block/version.h>

#include "cmd_arguments.h"
#include "SimpleOpt.h"

using namespace std;
using namespace SimpleOpt;

enum {
    HELP, VERSION, NUM_GROUPS, VERBOSE, QUIET, USE_STDIN,
    LOG_PERIOD, INIT_METHOD
};

CommandLineArguments::CommandLineArguments() : verbosity(1),
      numGroups(-1),
      logPeriod(-1), initMethod(GREEDY)
{}

void CommandLineArguments::parse(int argc, char** argv) {
    CSimpleOpt::SOption optionSpec[] = {
        /* standard options */

        { USE_STDIN, "-", SO_NONE },

        { HELP, "-?",     SO_NONE     },
        { HELP, "-h",     SO_NONE     },
        { HELP, "--help", SO_NONE     },

        { VERSION, "-V",        SO_NONE },
        { VERSION, "--version", SO_NONE },

        { VERBOSE, "-v",        SO_NONE },
        { VERBOSE, "--verbose", SO_NONE },

        { QUIET,   "-q",        SO_NONE },
        { QUIET,   "--quiet",   SO_NONE },

        /* basic options */

        { NUM_GROUPS, "-g",       SO_REQ_SEP },
        { NUM_GROUPS, "--groups", SO_REQ_SEP },

        /* advanced options */

        { LOG_PERIOD,  "--log-period",  SO_REQ_SEP },
        { INIT_METHOD, "--init-method", SO_REQ_SEP },

        SO_END_OF_OPTIONS
    };

    if (argc > 0)
        executableName = argv[0];

    CSimpleOpt args(argc, argv, optionSpec);

    while (args.Next()) {
        if (args.LastError() != SO_SUCCESS) {
            cerr << "Invalid argument: " << args.OptionText() << "\n";
            exit(1);
        }

        const char* arg_array = args.OptionArg();
        string arg;

        if (arg_array)
            arg = arg_array;

        switch (args.OptionId()) {
            /* Processing - */
            case USE_STDIN:
                inputFile = "-";
                break;

            /* Processing --help and --version */
            case HELP:
                showHelp(cerr);
                exit(0);
            case VERSION:
                cerr << "blockmodel " << BLOCKMODEL_VERSION_STRING << '\n';
                exit(0);

            /* Log levels */
            case VERBOSE:
				verbosity = 2;
                break;
            case QUIET:
				verbosity = 0;
                break;

            /* Processing basic algorithm parameters */
			case NUM_GROUPS:
                numGroups = atoi(args.OptionArg());
                break;

            /* Processing advanced parameters */
            case LOG_PERIOD:
                logPeriod = atoi(args.OptionArg());

            case INIT_METHOD:
                if (!strcmp(args.OptionArg(), "greedy"))
                    initMethod = GREEDY;
                else if (!strcmp(args.OptionArg(), "random"))
                    initMethod = RANDOM;
                else {
                    cerr << "Unknown initialization method: "
                         << args.OptionArg() << "\n";
                    exit(1);
                }
        }
    }

    /* Post-processing of algorithm values */
    if (logPeriod <= 0)
        logPeriod = 8192;

    /* If we already have stdin as the input file, return here */
    if (inputFile == "-")
        return;

    /* If no input file given, show help and exit */
    if (args.FileCount() == 0) {
        showHelp(cerr);
        exit(1);
    }

    inputFile = args.Files()[0];
}

void CommandLineArguments::showHelp(ostream& os) const {
    os << "Usage:\n    " << executableName << " [options] input-file\n"
          "\n"
          "General options:\n"
          "    -h, --help          shows this help message\n"
          "    -V, --version       shows the version number\n"
          "    -v, --verbose       verbose mode (more output)\n"
          "    -q, --quiet         quiet mode (less output, only errors)\n"
          "\n"
          "Basic algorithm parameters:\n"
          "    -g K, --groups K    sets the desired number of groups to\n"
          "                        K. Default = -1 (autodetection).\n"
          "\n"
          "Advanced algorithm parameters:\n"
          "    --log-period COUNT  shows a status message after every COUNT steps\n"
    ;
}


