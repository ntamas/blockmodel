/* vim:set ts=4 sw=4 sts=4 et: */

#include "cmd_arguments.h"
#include "SimpleOpt.h"

#include <iostream>
#include <string>

using namespace std;
using namespace SimpleOpt;

CommandLineArguments::CommandLineArguments(int argc, char** argv)
	: verbosity(1), numGroups(-1)
{
    CSimpleOpt::SOption optionSpec[] = {
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

        { NUM_GROUPS, "-g",       SO_REQ_SEP },
        { NUM_GROUPS, "--groups", SO_REQ_SEP },

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
                cerr << executableName << " 0.1" << '\n';
                exit(0);

            /* Log levels */
            case VERBOSE:
				verbosity = 2;
                break;
            case QUIET:
				verbosity = 0;
                break;

            /* Processing algorithm parameters */
			case NUM_GROUPS:
                numGroups = atoi(args.OptionArg());
                break;
        }
    }

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
          "Options:\n"
          "    -h, --help:         shows this help message\n"
          "    -V, --version:      shows the version number\n"
          "    -v, --verbose:      verbose mode (more output)\n"
          "    -q, --quiet:        quiet mode (less output, only errors)\n"
          "\n"
          "    -g K, --groups K:   sets the desired number of groups to\n"
          "                        K. Default = -1 (autodetection).\n"
    ;
}


