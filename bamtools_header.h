// ***************************************************************************
// bamtools_header.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 26 May 2010
// ---------------------------------------------------------------------------
// Prints the SAM-style header from a single BAM file (or merged header from
// multiple BAM files) to stdout.
// ***************************************************************************

#ifndef BAMTOOLS_HEADER_H
#define BAMTOOLS_HEADER_H

#include <iostream>
#include <string>
#include <vector>

#include "BamReader.h"
#include "BamMultiReader.h"
// #include "GetOpt.h"
#include "bamtools_getopt.h"

namespace BamTools {

int BamHeaderHelp(void) { 
    std::cerr << std::endl;
    std::cerr << "usage:\tbamtools header [--in FILE [FILE] [FILE] ...]" << std::endl;
    std::cerr << std::endl;
    std::cerr << "\t--in FILE  Input file(s) to dump header contents from  [stdin]" << std::endl;
    std::cerr << std::endl;
    return 0;
}

int RunBamHeader(int argc, char* argv[]) {

    // else parse command line for args
    GetOpt options(argc, argv, 1);
    
    std::vector<std::string> inputFilenames;
    options.addVariableLengthOption("in", &inputFilenames);

    if ( !options.parse() ) return BamHeaderHelp();
    if ( inputFilenames.empty() ) { inputFilenames.push_back("stdin"); }
  
    // open files
    BamMultiReader reader;
    reader.Open(inputFilenames, false);
        
    // dump header contents to stdout
    std::cout << reader.GetHeaderText() << std::endl;
    
    // clean up & exit
    reader.Close();
    return 0;
}

} // namespace BamTools

#endif // BAMTOOLS_HEADER_H
