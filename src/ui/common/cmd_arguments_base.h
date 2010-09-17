/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef _CMD_ARGUMENTS_BASE_H
#define _CMD_ARGUMENTS_BASE_H

#include <block/io.hpp>
#include <string>
#include "SimpleOpt.h"

/// Base class for command line argument parsers
/**
 * This base class provides support for a few command line arguments that are
 * common to all the utilities we provide in this package.
 */
class CommandLineArgumentsBase {
private:
	/// String storing the name of the executable used to start the program
	std::string m_executableName;

    /// String storing the version number of the application
    std::string m_versionNumber;

    /// A vector of command line option specifications
    std::vector<SimpleOpt::CSimpleOpt::SOption> m_options;

public:
    /********************/
    /* Basic parameters */
    /********************/

	/// Name of the input file
	std::string inputFile;

    /// Verbosity level
    int verbosity;

    /// Name of the output file
    std::string outputFile;

    /// The random seed to be used
    unsigned long randomSeed;

public:
	/// Constructor
	CommandLineArgumentsBase(const std::string programName = "",
            const std::string version = "");

    /// Virtual destructor that does nothing
    virtual ~CommandLineArgumentsBase() {}

    /// Adds an option to the list of command line options
    void addOption(int id, const char* option, SimpleOpt::ESOArgType type,
                   const char* longOption = 0);
    
    /// Handles the option with the given ID and argument
    /**
     * \return  zero if everything is OK, an exit code otherwise
     */
    virtual int handleOption(int id, const std::string& arg) = 0;

    /// Parses the command line arguments
    void parse(int argc, char** argv);

	/// Shows a help message on the given stream
	virtual void showHelp(std::ostream& os) const;

protected:
    /// Shows the "General options" section from the help message
    virtual void showGeneralOptionsHelp(std::ostream& os) const;
};

#endif     // _CMD_ARGUMENTS_BASE_H
