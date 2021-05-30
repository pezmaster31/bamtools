// ***************************************************************************
// bamtools.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 12 October 2012 (DB)
// ---------------------------------------------------------------------------
// Integrates a number of BamTools functionalities into a single executable.
// ***************************************************************************

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include "bamtools_convert.h"
#include "bamtools_count.h"
#include "bamtools_coverage.h"
#include "bamtools_filter.h"
#include "bamtools_header.h"
#include "bamtools_index.h"
#include "bamtools_merge.h"
#include "bamtools_random.h"
#include "bamtools_resolve.h"
#include "bamtools_revert.h"
#include "bamtools_sort.h"
#include "bamtools_split.h"
#include "bamtools_stats.h"
#include "bamtools_version.h"
using namespace BamTools;

// bamtools subtool names
static const std::string CONVERT = "convert";
static const std::string COUNT = "count";
static const std::string COVERAGE = "coverage";
static const std::string FILTER = "filter";
static const std::string HEADER = "header";
static const std::string INDEX = "index";
static const std::string MERGE = "merge";
static const std::string RANDOM = "random";
static const std::string RESOLVE = "resolve";
static const std::string REVERT = "revert";
static const std::string SORT = "sort";
static const std::string SPLIT = "split";
static const std::string STATS = "stats";

// bamtools help/version constants
static const std::string HELP = "help";
static const std::string LONG_HELP = "--help";
static const std::string SHORT_HELP = "-h";
static const std::string VERSION = "version";
static const std::string LONG_VERSION = "--version";
static const std::string SHORT_VERSION = "-v";

// determine if string is a help constant
static bool IsHelp(char* str)
{
    return (str == HELP || str == LONG_HELP || str == SHORT_HELP);
}

// determine if string is a version constant
static bool IsVersion(char* str)
{
    return (str == VERSION || str == LONG_VERSION || str == SHORT_VERSION);
}

// subtool factory method
AbstractTool* CreateTool(const std::string& arg)
{

    // determine tool type based on arg
    if (arg == CONVERT) return new ConvertTool;
    if (arg == COUNT) return new CountTool;
    if (arg == COVERAGE) return new CoverageTool;
    if (arg == FILTER) return new FilterTool;
    if (arg == HEADER) return new HeaderTool;
    if (arg == INDEX) return new IndexTool;
    if (arg == MERGE) return new MergeTool;
    if (arg == RANDOM) return new RandomTool;
    if (arg == RESOLVE) return new ResolveTool;
    if (arg == REVERT) return new RevertTool;
    if (arg == SORT) return new SortTool;
    if (arg == SPLIT) return new SplitTool;
    if (arg == STATS) return new StatsTool;

    // unknown arg
    return 0;
}

// print help info
int Help(int argc, char* argv[])
{

    // check for 'bamtools help COMMAND' to print tool-specific help message
    if (argc > 2) {

        // determine desired sub-tool
        AbstractTool* tool = CreateTool(argv[2]);

        // if tool known, print its help screen
        if (tool) return tool->Help();
    }

    // print general BamTools help message
    std::cerr << std::endl;
    std::cerr << "usage: bamtools [--help] COMMAND [ARGS]" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Available bamtools commands:" << std::endl;
    std::cerr << "\tconvert         Converts between BAM and a number of other formats"
              << std::endl;
    std::cerr << "\tcount           Prints number of alignments in BAM file(s)" << std::endl;
    std::cerr << "\tcoverage        Prints coverage statistics from the input BAM file"
              << std::endl;
    std::cerr << "\tfilter          Filters BAM file(s) by user-specified criteria" << std::endl;
    std::cerr << "\theader          Prints BAM header information" << std::endl;
    std::cerr << "\tindex           Generates index for BAM file" << std::endl;
    std::cerr << "\tmerge           Merge multiple BAM files into single file" << std::endl;
    std::cerr << "\trandom          Select random alignments from existing BAM file(s), intended "
                 "more as a testing tool."
              << std::endl;
    std::cerr
        << "\tresolve         Resolves paired-end reads (marking the IsProperPair flag as needed)"
        << std::endl;
    std::cerr << "\trevert          Removes duplicate marks and restores original base qualities"
              << std::endl;
    std::cerr << "\tsort            Sorts the BAM file according to some criteria" << std::endl;
    std::cerr << "\tsplit           Splits a BAM file on user-specified property, creating a new "
                 "BAM output file for each value found"
              << std::endl;
    std::cerr << "\tstats           Prints some basic statistics from input BAM file(s)"
              << std::endl;
    std::cerr << std::endl;
    std::cerr << "See 'bamtools help COMMAND' for more information on a specific command."
              << std::endl;
    std::cerr << std::endl;
    return EXIT_SUCCESS;
}

// print version info
int Version()
{

    std::stringstream versionStream;
    versionStream << BAMTOOLS_VERSION_MAJOR << '.' << BAMTOOLS_VERSION_MINOR << '.'
                  << BAMTOOLS_VERSION_PATCH;

    std::cout << std::endl;
    std::cout << "bamtools " << versionStream.str() << std::endl;
    std::cout << "Part of BamTools API and toolkit" << std::endl;
    std::cout << "Primary authors: Derek Barnett, Erik Garrison, Michael Stromberg" << std::endl;
    std::cout << "(c) 2009-2012 Marth Lab, Biology Dept., Boston College" << std::endl;
    std::cout << std::endl;
    return EXIT_SUCCESS;
}

// toolkit entry point
int main(int argc, char* argv[])
{

    // just 'bamtools'
    if (argc == 1) return Help(argc, argv);

    // 'bamtools help', 'bamtools --help', or 'bamtools -h'
    if (IsHelp(argv[1])) return Help(argc, argv);

    // 'bamtools version', 'bamtools --version', or 'bamtools -v'
    if (IsVersion(argv[1])) return Version();

    // determine desired sub-tool, run if found
    AbstractTool* tool = CreateTool(argv[1]);
    if (tool) return tool->Run(argc, argv);
    delete tool;

    // no tool matched, show help
    return Help(argc, argv);
}
