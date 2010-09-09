/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef _CMD_ARGUMENTS_H
#define _CMD_ARGUMENTS_H

#include <string>

/// Possible initialization methods for the algorithm
typedef enum {
    GREEDY, RANDOM
} InitializationMethod;

/// This class parsers and stores parameters retrieved from the command line
class CommandLineArguments {
private:
	/// String storing the name of the executable used to start the program
	std::string executableName;

public:
    /********************/
    /* Basic parameters */
    /********************/

	/// Name of the input file
	std::string inputFile;

    /// Verbosity level
    int verbosity;

	/// Desired number of groups
	int numGroups;

    /// Number of samples taken from the chain after convergence
    long numSamples;

    /***********************/
    /* Advanced parameters */
    /***********************/

    /// Block size used in MCMC sampling to assess convergence
    int blockSize;

    /// Initialization method to be used for the MCMC sampling
    InitializationMethod initMethod;

    /// Number of steps after which a status message is printed
    int logPeriod;

    /// The random seed to be used
    unsigned long randomSeed;

	/// Constructor
	CommandLineArguments();
    
    /// Parses the command line arguments
    void parse(int argc, char** argv);

	/// Shows a help message on the given stream
	void showHelp(std::ostream& os) const;
};

#endif     // _CMD_ARGUMENTS_H
