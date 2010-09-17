/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef _CMD_ARGUMENTS_H
#define _CMD_ARGUMENTS_H

#include <block/io.hpp>
#include <string>
#include "../common/cmd_arguments_base.h"

/// Possible initialization methods for the algorithm
typedef enum {
    GREEDY, RANDOM
} InitializationMethod;

/// Command line parser for block-fit
class CommandLineArguments : public CommandLineArgumentsBase {
public:
    /********************/
    /* Basic parameters */
    /********************/

	/// Desired number of groups
	int numGroups;

    /// Number of samples taken from the chain after convergence
    long numSamples;

    /// Format of the output file
    Format outputFormat;

    /***********************/
    /* Advanced parameters */
    /***********************/

    /// Block size used in MCMC sampling to assess convergence
    int blockSize;

    /// Initialization method to be used for the MCMC sampling
    InitializationMethod initMethod;

    /// Number of steps after which a status message is printed
    int logPeriod;

	/// Constructor
	CommandLineArguments();

    /// Handles the command line options not handled by the superclass
    virtual int handleOption(int id, const std::string& arg);

	/// Shows a help message on the given stream
	virtual void showHelp(std::ostream& os) const;
};

#endif     // _CMD_ARGUMENTS_H
