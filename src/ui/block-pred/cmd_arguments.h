/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef _CMD_ARGUMENTS_H
#define _CMD_ARGUMENTS_H

#include <block/io.hpp>
#include "../common/cmd_arguments_base.h"

/// Command line parser for block-pred
class CommandLineArguments : public CommandLineArgumentsBase {
public:
    /********************/
    /* Basic parameters */
    /********************/

    /// Number of samples to take
    unsigned int sampleCount;

    /// Format of the input file
    Format inputFormat;

    /***********************/
    /* Advanced parameters */
    /***********************/

    /// Number of steps after which a status message is printed
    int logPeriod;

    /// Sampling frequency
    float samplingFreq;

	/// Constructor
	CommandLineArguments();

    /// Handles the command line options not handled by the superclass
    virtual int handleOption(int id, const std::string& arg);

	/// Shows a help message on the given stream
	virtual void showHelp(std::ostream& os) const;
};

#endif     // _CMD_ARGUMENTS_H
