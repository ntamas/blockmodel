/* vim:set ts=4 sw=4 sts=4 et: */

#include <ctime>
#include <iostream>
#include <string>
#include <block/version.h>

#include "cmd_arguments.h"

using namespace std;
using namespace SimpleOpt;

enum {
    COUNT, IN_FORMAT, OUT_FORMAT
};

CommandLineArguments::CommandLineArguments() :
    CommandLineArgumentsBase("block-gen", BLOCKMODEL_VERSION_STRING),
    count(1), inputFormat(FORMAT_PLAIN), outputFormat(FORMAT_EDGELIST) {

    /* basic options */
    addOption(COUNT,       "-c", SO_REQ_SEP, "--count");
    addOption(IN_FORMAT,   "-f", SO_REQ_SEP, "--input-format");
    addOption(OUT_FORMAT,  "-F", SO_REQ_SEP, "--output-format");
}

int CommandLineArguments::handleOption(int id, const std::string& arg) {
    switch (id) {
        /* Processing basic algorithm parameters */

        case COUNT:
            count = atoi(arg.c_str());
            break;

        case IN_FORMAT:
            if (arg == "plain")
                inputFormat = FORMAT_PLAIN;
            // else if (arg == "json")
            //     inputFormat = FORMAT_JSON;
            else {
                cerr << "Unknown input format: " << arg << "\n";
                return 1;
            }
            break;

        case OUT_FORMAT:
            if (arg == "edgelist")
                outputFormat = FORMAT_EDGELIST;
            else if (arg == "graphml")
                outputFormat = FORMAT_GRAPHML;
            else if (arg == "gml")
                outputFormat = FORMAT_GML;
            else if (arg == "leda")
                outputFormat = FORMAT_LEDA;
            else {
                cerr << "Unknown output format: " << arg << "\n";
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
          "                        sets the number of graphs to be generated. If this\n"
          "                        is greater than 1, use %%d in the output file name\n"
          "                        to add a counter. Default = 1.\n"
          "    -f FORMAT, --input-format FORMAT\n"
          "                        sets the format of the input file. The default value\n"
          "                        is plain, which is a simple plain text format. Known\n"
          "                        formats are: plain.\n"
          "    -F format, --output-format FORMAT\n"
          "                        sets the format of the output file. The default value\n"
          "                        is edgelist, which dumps the edges of the graph (where\n"
          "                        each edge is identified by a non-negative number).\n"
          "                        Known formats are: edgelist, graphml, gml, leda.\n"
          "    -o FILE, --output FILE\n"
          "                        sets the name of the output file where the results\n"
          "                        will be written. The default is the standard\n"
          "                        output stream.\n"
          "\n"
          "Advanced algorithm parameters:\n"
          "    --model MODEL       selects the type of the model used for generation.\n"
          "                        Available models: uncorrected (default), degree.\n"
          "    --seed SEED         use the given number to seed the random number\n"
          "                        generator.\n"
    ;
}


