/* vim:set ts=4 sw=4 sts=4 et: */

#include <ctime>
#include <iostream>
#include <string>
#include <block/version.h>

#include "cmd_arguments.h"
#include "SimpleOpt.h"

using namespace std;
using namespace SimpleOpt;

enum {
    HELP, VERSION, VERBOSE, QUIET, USE_STDIN,
    NUM_GROUPS, NUM_SAMPLES, OUT_FILE, OUT_FORMAT,
    LOG_PERIOD, INIT_METHOD, BLOCK_SIZE, SEED
};

CommandLineArguments::CommandLineArguments() : verbosity(1),
    numGroups(-1), numSamples(100000), outputFile(), outputFormat(FORMAT_PLAIN),
    blockSize(65536), initMethod(GREEDY), logPeriod(8192),
    randomSeed(time(0))
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

        { OUT_FORMAT,  "-F",        SO_REQ_SEP },
        { OUT_FORMAT,  "--format",  SO_REQ_SEP },

        { NUM_GROUPS,  "-g",        SO_REQ_SEP },
        { NUM_GROUPS,  "--groups",  SO_REQ_SEP },

        { OUT_FILE,    "-o",        SO_REQ_SEP },
        { OUT_FILE,    "--output",  SO_REQ_SEP },

        { NUM_SAMPLES, "-s",        SO_REQ_SEP },
        { NUM_SAMPLES, "--samples", SO_REQ_SEP },

        /* advanced options */

        { BLOCK_SIZE,  "--block-size",  SO_REQ_SEP },
        { INIT_METHOD, "--init-method", SO_REQ_SEP },
        { LOG_PERIOD,  "--log-period",  SO_REQ_SEP },
        { SEED,        "--seed",        SO_REQ_SEP },

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

            case NUM_SAMPLES:
                numSamples = atol(args.OptionArg());
                break;

            case OUT_FILE:
                outputFile = args.OptionArg();
                break;

            case OUT_FORMAT:
                if (!strcmp(args.OptionArg(), "plain"))
                    outputFormat = FORMAT_PLAIN;
                else if (!strcmp(args.OptionArg(), "json"))
                    outputFormat = FORMAT_JSON;
                else if (!strcmp(args.OptionArg(), "null"))
                    outputFormat = FORMAT_NULL;
                else {
                    cerr << "Unknown output format: "
                         << args.OptionArg() << "\n";
                    exit(1);
                }
                break;

            /* Processing advanced parameters */

            case BLOCK_SIZE:
                blockSize = atoi(args.OptionArg());
                break;

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
                break;

            case LOG_PERIOD:
                logPeriod = atoi(args.OptionArg());
                break;

            case SEED:
                randomSeed = atol(args.OptionArg());
                break;
        }
    }

    /* Post-processing of algorithm values */

    /* If the output file is empty, use a single dash, meaning stdout */
    if (outputFile == "")
        outputFile = "-";

    /* If we already have stdin as the input file, return here */
    if (inputFile == "-")
        return;

    /* If no input file was given, show help and exit */
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
          "    -o FILE, --output FILE\n"
          "                        sets the name of the output file where the results\n"
          "                        will be written. The default is the standard\n"
          "                        output stream.\n"
          "    -s N, --samples N   sets the number of samples to be taken from the\n"
          "                        Markov chain after convergence.\n"
          "\n"
          "Advanced algorithm parameters:\n"
          "    --block-size N      sets the block size used when determining the\n"
          "                        convergence of the Markov chain to N\n"
          "    --init-method METH  use the given initialization method METH for\n"
          "                        the Markov chain. Available methods: greedy,\n"
          "                        random.\n"
          "    --log-period COUNT  shows a status message after every COUNT steps\n"
          "    --seed SEED         use the given number to seed the random number\n"
          "                        generator.\n"
    ;
}


