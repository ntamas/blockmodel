/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef _CMD_ARGUMENTS_H
#define _CMD_ARGUMENTS_H

#include <string>

/// This class parsers and stores parameters retrieved from the command line
class CommandLineArguments {
private:
	typedef enum {
		HELP, VERSION, NUM_GROUPS, VERBOSE, QUIET, USE_STDIN
	} OptionIO;

	/// String storing the name of the executable used to start the program
	std::string executableName;

public:
	/// Name of the input file
	std::string inputFile;

    /// Verbosity level
    int verbosity;

	/// Desired number of groups
	int numGroups;

	/// Constructor
	CommandLineArguments(int argc, char** argv);

	/// Shows a help message on the given stream
	void showHelp(std::ostream& os) const;
};

#endif     // _CMD_ARGUMENTS_H
