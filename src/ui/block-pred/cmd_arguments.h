/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef _CMD_ARGUMENTS_H
#define _CMD_ARGUMENTS_H

#include <string>
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

    /// Whether we want to sort the predictions
    bool sort;

    /***********************/
    /* Advanced parameters */
    /***********************/

    /// Whether we want to run a burn-in period first
    bool burnin;

    /// Number of steps after which a status message is printed
    int logPeriod;

    /// Name of a file to be used for mapping vertex IDs to names
    std::string nameMappingFile;

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
