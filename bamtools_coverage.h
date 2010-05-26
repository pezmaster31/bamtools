// ***************************************************************************
// bamtools_coverage.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 26 May 2010
// ---------------------------------------------------------------------------
// Prints coverage statistics for a single BAM file 
//
// ** Expand to multiple?? 
//
// ***************************************************************************

#ifndef BAMTOOLS_COVERAGE_H
#define BAMTOOLS_COVERAGE_H

#include <iostream>
#include <string>

#include "BamReader.h"
#include "bamtools_getopt.h"

namespace BamTools {

int BamCoverageHelp(void) { 
    std::cerr << std::endl;
    std::cerr << "usage:\tbamtools coverage [--in BAM file]" << std::endl;
    std::cerr << "\t-i, --in\tInput BAM file to generate coverage stats\t[default=stdin]" << std::endl;
    std::cerr << std::endl;
    return 0;
}

int RunBamCoverage(int argc, char* argv[]) {
  
    // else parse command line for args  
    GetOpt options(argc, argv, 1);
    
    std::string inputFilename;
    options.addOption('i', "in", &inputFilename);
    
    if ( !options.parse() ) return BamCoverageHelp();
    if ( inputFilename.empty() ) { inputFilename = "stdin"; }
    
//     // open our BAM reader
//     BamReader reader;
//     reader.Open(inputFilename);
    
    // generate coverage stats
    std::cerr << "Generating coverage stats for " << inputFilename << std::endl;
    std::cerr << "FEATURE NOT YET IMPLEMENTED!" << std::endl;
    
    // clean & exit
//     reader.Close();
    return 0;
}

} // namespace BamTools

#endif // BAMTOOLS_COVERAGE_H
