/* vim:set ts=4 sw=4 sts=4 et: */

#include <ctime>
#include <iostream>
#include <string>
#include <block/version.h>

#include "cmd_arguments.h"

using namespace std;
using namespace SimpleOpt;

enum {
    COUNT, IN_FORMAT
};

CommandLineArguments::CommandLineArguments() :
    CommandLineArgumentsBase("block-pred", BLOCKMODEL_VERSION_STRING),
    sampleCount(1), inputFormat(FORMAT_PLAIN) {

    /* basic options */
    addOption(COUNT,     "-c", SO_REQ_SEP, "--count");
    addOption(IN_FORMAT, "-f", SO_REQ_SEP, "--input-format");
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
            else if (arg == "json")
                inputFormat = FORMAT_JSON;
            else {
                cerr << "Unknown input format: " << arg << "\n";
                return 1;
            }
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
          "                        formats are: json, plain.\n"
          "    -o FILE, --output FILE\n"
          "                        sets the name of the output file where the results\n"
          "                        will be written. The default is the standard\n"
          "                        output stream.\n"
          "\n"
          "Advanced algorithm parameters:\n"
          "    --seed SEED         use the given number to seed the random number\n"
          "                        generator.\n"
    ;
}


