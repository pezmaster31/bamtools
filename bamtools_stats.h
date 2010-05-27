// ***************************************************************************
// bamtools_stats.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 26 May 2010
// ---------------------------------------------------------------------------
// Prints general statistics for a single BAM file
//
// ** Expand to multiple??
//
// ***************************************************************************

#ifndef BAMTOOLS_STATS_H
#define BAMTOOLS_STATS_H

#include <iostream>
#include <string>

#include "BamReader.h"
#include "bamtools_getopt.h"

namespace BamTools {

int BamStatsHelp(void) { 
    std::cerr << std::endl;
    std::cerr << "usage:\tbamtools stats [--in FILE]" << std::endl;
    std::cerr << std::endl;
    std::cerr << "\t--in FILE  Input BAM file to calculate general stats  [stdin]" << std::endl;
    std::cerr << std::endl;
    return 0;
}

int RunBamStats(int argc, char* argv[]) {
  
    // else parse command line for args  
    GetOpt options(argc, argv, 1);
    
    std::string inputFilename;
    options.addOption("in", &inputFilename);
    
    if ( !options.parse() ) return BamStatsHelp();
    if ( inputFilename.empty() ) { inputFilename = "stdin"; }
    
    // open our BAM reader
//     BamReader reader;
//     reader.Open(inputFilename);
    
    // calculate general stats
    std::cerr << "Calculating general stats for " << inputFilename << std::endl;
    std::cerr << "FEATURE NOT YET IMPLEMENTED!" << std::endl;
    
    // clean & exit
//     reader.Close();
    return 0;
}

} // namespace BamTools

#endif // BAMTOOLS_STATS_H