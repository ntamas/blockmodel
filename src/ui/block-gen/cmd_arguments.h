/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef _CMD_ARGUMENTS_H
#define _CMD_ARGUMENTS_H

#include <block/io.hpp>
#include "../common/cmd_arguments_base.h"

/// Accepted output formats
typedef enum {
    FORMAT_EDGELIST, FORMAT_LEDA
} GraphFormat;

/// Command line parser for block-gen
class CommandLineArguments : public CommandLineArgumentsBase {
public:
    /********************/
    /* Basic parameters */
    /********************/

    /// Number of graphs to generate
    unsigned int count;

    /// Format of the input file
    Format inputFormat;

    /// Format of the output file
    GraphFormat outputFormat;

    /***********************/
    /* Advanced parameters */
    /***********************/

	/// Constructor
	CommandLineArguments();

    /// Handles the command line options not handled by the superclass
    virtual int handleOption(int id, const std::string& arg);

	/// Shows a help message on the given stream
	virtual void showHelp(std::ostream& os) const;
};

#endif     // _CMD_ARGUMENTS_H
