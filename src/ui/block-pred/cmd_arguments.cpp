/* vim:set ts=4 sw=4 sts=4 et: */

#include <ctime>
#include <iostream>
#include <string>
#include <block/version.h>

#include "cmd_arguments.h"

using namespace std;
using namespace SimpleOpt;

enum {
    COUNT, IN_FORMAT, SORT,
    LOG_PERIOD, NAME_MAPPING, NO_BURNIN, SAMPLING_FREQ
};

CommandLineArguments::CommandLineArguments() :
    CommandLineArgumentsBase("block-pred", BLOCKMODEL_VERSION_STRING),
    sampleCount(1), inputFormat(FORMAT_PLAIN), sort(false),
    burnin(true), logPeriod(1024), nameMappingFile(), samplingFreq(0.1)
{

    /* basic options */
    addOption(COUNT,     "-c", SO_REQ_SEP, "--count");
    addOption(IN_FORMAT, "-f", SO_REQ_SEP, "--input-format");
    addOption(SORT,      "-s", SO_NONE,    "--sort");

    /* advanced options */
    addOption(LOG_PERIOD,    "--log-period",    SO_REQ_SEP);
    addOption(NAME_MAPPING,  "--name-mapping",  SO_REQ_SEP);
    addOption(NO_BURNIN,     "--no-burnin",     SO_NONE);
    addOption(SAMPLING_FREQ, "--sampling-freq", SO_REQ_SEP);
}

int CommandLineArguments::handleOption(int id, const std::string& arg) {
    switch (id) {
        /* Processing basic algorithm parameters */

        case COUNT:
            sampleCount = atoi(arg.c_str());
            break;

        case IN_FORMAT:
            if (arg == "plain")
                inputFormat = FORMAT_PLAIN;
            else {
                cerr << "Unknown input format: " << arg << "\n";
                return 1;
            }
            break;

        case SORT:
            sort = true;
            break;

        /* Processing advanced parameters */

        case LOG_PERIOD:
            logPeriod = atoi(arg.c_str());
            break;

        case NAME_MAPPING:
            nameMappingFile = arg;
            break;

        case NO_BURNIN:
            burnin = false;
            break;

        case SAMPLING_FREQ:
            samplingFreq = atof(arg.c_str());
            break;
    }

    return 0;
}

void CommandLineArguments::showHelp(ostream& os) const {
    CommandLineArgumentsBase::showHelp(os);
    os << "Basic algorithm parameters:\n"
          "    -c COUNT, --count COUNT\n"
          "                        sets the number of samples to be taken from the\n"
          "                        Markov chain. Default = 1.\n"
          "    -f FORMAT, --input-format FORMAT\n"
          "                        sets the format of the input file. The default value\n"
          "                        is plain, which is a simple plain text format. Known\n"
          "                        formats are: plain.\n"
          "    -o FILE, --output FILE\n"
          "                        sets the name of the output file where the results\n"
          "                        will be written. The default is the standard\n"
          "                        output stream.\n"
          "    -s, --sort\n"
          "                        sorts the output in decreasing order of predicted\n"
          "                        probabilities.\n"
          "\n"
          "Advanced algorithm parameters:\n"
          "    --log-period COUNT  shows a status message after every COUNT steps\n"
          "    --model MODEL       selects the type of the model used for prediction.\n"
          "                        Available models: uncorrected (default), degree.\n"
          "    --name-mapping FILE\n"
          "                        reads vertex names from the given FILE and uses\n"
          "                        these names instead of indices in the output.\n"
          "    --sampling-freq P   take a sample from the Markov chain at every step\n"
          "                        with probability P. Smaller P values decorrelate the\n"
          "                        Markov chain at the expense of longer sampling time.\n"
          "                        Default = 0.1 (i.e. a sample is taken after every 10\n"
          "                        steps on average).\n"
          "    --seed SEED         use the given number to seed the random number\n"
          "                        generator.\n"
    ;
}


