// ***************************************************************************
// bamtools.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 22 July 2010
// ---------------------------------------------------------------------------
// Integrates a number of BamTools functionalities into a single executable.
// ***************************************************************************

// Std C/C++ includes
#include <iostream>

// BamTools includes
#include "bamtools_convert.h"
#include "bamtools_count.h"
#include "bamtools_coverage.h"
#include "bamtools_filter.h"
#include "bamtools_header.h"
#include "bamtools_index.h"
#include "bamtools_merge.h"
#include "bamtools_random.h"
#include "bamtools_sort.h"
#include "bamtools_stats.h"

using namespace std;
using namespace BamTools;

// ------------------------------------------
// bamtools subtool names
static const string CONVERT  = "convert";
static const string COUNT    = "count";
static const string COVERAGE = "coverage";
static const string FILTER   = "filter";
static const string HEADER   = "header";
static const string INDEX    = "index";
static const string MERGE    = "merge";
static const string RANDOM   = "random";
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

// ------------------------------------------
// Print help info
int Help(int argc, char* argv[]) {
  
    // 'bamtools help COMMAND'
    if (argc > 2) {
        
        AbstractTool* tool(0);
        if ( argv[2] == CONVERT )  tool = new ConvertTool;
        if ( argv[2] == COUNT )    tool = new CountTool;
        if ( argv[2] == COVERAGE ) tool = new CoverageTool;
        if ( argv[2] == FILTER )   tool = new FilterTool;
        if ( argv[2] == HEADER )   tool = new HeaderTool;
        if ( argv[2] == INDEX )    tool = new IndexTool;
        if ( argv[2] == MERGE )    tool = new MergeTool;
        if ( argv[2] == RANDOM )   tool = new RandomTool;
        if ( argv[2] == SORT )     tool = new SortTool;
        if ( argv[2] == STATS )    tool = new StatsTool;
        
        // if tool known, print its help screen
        if ( tool ) return tool->Help();
    }

    // either 'bamtools help' or unrecognized argument after 'help'
    cerr << endl;
    cerr << "usage: bamtools [--help] COMMAND [ARGS]" << endl;
    cerr << endl;
    cerr << "Available bamtools commands:" << endl;
    cerr << "\tconvert   Converts between BAM and a number of other formats" << endl;
    cerr << "\tcount     Prints number of alignments in BAM file" << endl;
    cerr << "\tcoverage  Prints coverage statistics from the input BAM file" << endl;
    cerr << "\tfilter    Filters BAM file(s) by user-specified criteria" << endl;
    cerr << "\theader    Prints BAM header information" << endl;
    cerr << "\tindex     Generates index for BAM file" << endl;
    cerr << "\tmerge     Merge multiple BAM files into single file" << endl;
    cerr << "\trandom    Grab a random subset of alignments" << endl;
    cerr << "\tsort      Sorts the BAM file according to some criteria" << endl;
    cerr << "\tstats     Prints some basic statistics from the input BAM file" << endl;
    cerr << endl;
    cerr << "See 'bamtools help COMMAND' for more information on a specific command." << endl;
    cerr << endl;
    return 0;
}

// ------------------------------------------
// Print version info
int Version(void) {
    cout << endl;
    cout << "bamtools v0.8.xx" << endl;
    cout << "Part of BamTools API and toolkit" << endl;
    cout << "Primary authors: Derek Barnett, Erik Garrison, Michael Stromberg" << endl;
    cout << "(c) 2009-2010 Marth Lab, Biology Dept., Boston College" << endl;
    cout << endl;
    return 0;
}

// ------------------------------------------
// toolkit entry point
int main(int argc, char* argv[]) {

    // just 'bamtools'
    if ( (argc == 1) ) return Help(argc, argv);
    
    // 'bamtools help', 'bamtools --help', or 'bamtools -h'
    if ( (argv[1] == HELP) || (argv[1] == LONG_HELP) || (argv[1] == SHORT_HELP) ) return Help(argc, argv); 
    
    // 'bamtools version', 'bamtools --version', or 'bamtools -v'
    if ( (argv[1] == VERSION) || (argv[1] == LONG_VERSION) || (argv[1] == SHORT_VERSION) ) return Version(); 
        
    // determine desired sub-tool
    AbstractTool* tool(0);
    if ( argv[1] == CONVERT )  tool = new ConvertTool;
    if ( argv[1] == COUNT )    tool = new CountTool;
    if ( argv[1] == COVERAGE ) tool = new CoverageTool;
    if ( argv[1] == FILTER )   tool = new FilterTool;
    if ( argv[1] == HEADER )   tool = new HeaderTool;
    if ( argv[1] == INDEX )    tool = new IndexTool;
    if ( argv[1] == MERGE )    tool = new MergeTool;
    if ( argv[1] == RANDOM )   tool = new RandomTool;
    if ( argv[1] == SORT )     tool = new SortTool;
    if ( argv[1] == STATS )    tool = new StatsTool;
    
    // if found, run tool
    if ( tool ) return tool->Run(argc, argv);
    // no match found, show help
    else return Help(argc, argv); 
}
