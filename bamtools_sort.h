// ***************************************************************************
// bamtools_sortt.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 26 May 2010
// ---------------------------------------------------------------------------
// Sorts an input BAM file (default by position) and stores in a new BAM file.
// ***************************************************************************

#ifndef BAMTOOLS_SORT_H
#define BAMTOOLS_SORT_H

#include <iostream>
#include <string>

#include "BamReader.h"
#include "bamtools_getopt.h"

namespace BamTools {

int BamSortHelp(void) { 
    std::cerr << std::endl;
    std::cerr << "usage:\tbamtools sort [--in FILE] [--out FILE]" << std::endl;
    std::cerr << std::endl;
    std::cerr << "\t--in FILE   Input BAM file to sort          [stdin]" << std::endl;
    std::cerr << "\t--out FILE  Destination of sorted BAM file  [stdout]" << std::endl;
    std::cerr << std::endl;
    return 0;
}

int RunBamSort(int argc, char* argv[]) {
  
    // else parse command line for args  
    GetOpt options(argc, argv, 1);
    
    std::string inputFilename;
    options.addOption("in", &inputFilename);
    
    std::string outputFilename;
    options.addOption("out", &outputFilename);
    
    if ( !options.parse() ) return BamCoverageHelp();
    if ( inputFilename.empty() )  { inputFilename  = "stdin"; }
    if ( outputFilename.empty() ) { outputFilename = "stdout"; }
    
    // open our BAM reader
//     BamReader reader;
//     reader.Open(inputFilename);
//     
//     // retrieve header & reference dictionary info
//     std::string header = reader.GetHeaderText();
//     RefVector references = reader.GetReferenceData(); 
//     
//     BamWriter writer;
//     writer.Open(outputFilename, header, references);
//     
    // sort BAM file
    std::cerr << "Sorting " << inputFilename << std::endl;
    std::cerr << "Saving sorted BAM in " << outputFilename << endl;
    std::cerr << "FEATURE NOT YET IMPLEMENTED!" << std::endl;
    
    // clean & exit
//     reader.Close();
//     writer.Close();
    return 0;
}

} // namespace BamTools

#endif // BAMTOOLS_SORT_H