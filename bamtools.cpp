// ***************************************************************************
// bamtools.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 26 May 2010
// ---------------------------------------------------------------------------
// Integrates a number of BamTools functionalities into a single executable.
// ***************************************************************************

// Std C/C++ includes
#include <iostream>

// BamTools includes
#include "bamtools_coverage.h"
#include "bamtools_dump.h"
#include "bamtools_header.h"
#include "bamtools_index.h"
#include "bamtools_merge.h"
#include "bamtools_sam.h"
#include "bamtools_sort.h"
#include "bamtools_stats.h"

using namespace std;
using namespace BamTools;

// ------------------------------------------
// bamtools subtool names
static const string COVERAGE = "coverage";
static const string DUMP     = "dump"; // <-- do we even want to keep this? I think 'bamtools sam' will be more useful anyway 
                                       // nobody's going to want what was essentially an early, bloated, debugging output
static const string HEADER   = "header";
static const string INDEX    = "index";
static const string MERGE    = "merge";
static const string SAM      = "sam";
static const string SORT     = "sort";
static const string STATS    = "stats";

// ------------------------------------------
// bamtools help/version names
static const string HELP       = "help";
static const string LONG_HELP  = "--help";
static const string SHORT_HELP = "-h";

static const string VERSION       = "version";
static const string LONG_VERSION  = "--version";
static const string SHORT_VERSION = "-v";

int Help(int argc, char* argv[]) {
  
    // 'bamtools help COMMAND'
    if (argc > 2) {
        if ( argv[2] == COVERAGE) return BamCoverageHelp();
        if ( argv[2] == DUMP )    return BamDumpHelp();                 // keep?
        if ( argv[2] == HEADER )  return BamHeaderHelp();
        if ( argv[2] == INDEX )   return BamIndexHelp();
        if ( argv[2] == MERGE )   return BamMergeHelp();
        if ( argv[2] == SAM )     return BamSamHelp();
        if ( argv[2] == SORT )    return BamSortHelp();
        if ( argv[2] == STATS )   return BamStatsHelp();
    }
     
    // either 'bamtools help' or unrecognized argument after 'help'
    cerr << endl;
    cerr << "usage: bamtools [--help] COMMAND [ARGS]" << endl;
    cerr << endl;
    cerr << "Available bamtools commands:" << endl;
    cerr << "\tcoverage  Prints coverage statistics from the input BAM file" << endl;
    cerr << "\tdump      Dump BAM file contents to text output" << endl;                          // keep?
    cerr << "\theader    Prints BAM header information" << endl;
    cerr << "\tindex     Generates index for BAM file" << endl;
    cerr << "\tmerge     Merge multiple BAM files into single file" << endl;
    cerr << "\tsam       Prints the BAM file in SAM (text) format" << endl;
    cerr << "\tsort      Sorts the BAM file according to some criteria" << endl;
    cerr << "\tstats     Prints some basic statistics from the input BAM file" << endl;
    cerr << endl;
    cerr << "See 'bamtools help COMMAND' for more information on a specific command." << endl;
    cerr << endl;
    
    return 0;
}

int Version(void) {
    cout << endl;
    cout << "bamtools v0.x.xx" << endl;
    cout << "Part of BamTools API and toolkit" << endl;
    cout << "Primary authors: Derek Barnett, Erik Garrison, Michael Stromberg" << endl;
    cout << "(c) 2009-2010 Marth Lab, Biology Dept., Boston College" << endl;
    cout << endl;
    return 0;
}

int main(int argc, char* argv[]) {

    // just 'bamtools'
    if ( (argc == 1) ) return Help(argc, argv);
    
    // 'bamtools help', 'bamtools --help', or 'bamtools -h'
    if ( (argv[1] == HELP) || (argv[1] == LONG_HELP) || (argv[1] == SHORT_HELP) ) return Help(argc, argv); 
    
    // 'bamtools version', 'bamtools --version', or 'bamtools -v'
    if ( (argv[1] == VERSION) || (argv[1] == LONG_VERSION) || (argv[1] == SHORT_VERSION) ) return Version(); 
        
    // run desired sub-tool
    if ( argv[1] == COVERAGE ) return RunBamCoverage(argc, argv);
    if ( argv[1] == DUMP )     return RunBamDump(argc, argv);           // keep?
    if ( argv[1] == HEADER )   return RunBamHeader(argc, argv);
    if ( argv[1] == INDEX )    return RunBamIndex(argc, argv);
    if ( argv[1] == MERGE )    return RunBamMerge(argc, argv); 
    if ( argv[1] == SAM )      return RunBamSam(argc, argv);
    if ( argv[1] == SORT )     return RunBamSort(argc, argv);
    if ( argv[1] == STATS )    return RunBamStats(argc, argv);
    
    // unrecognized 2nd argument, print help
    return Help(argc, argv);    
}
