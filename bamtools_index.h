// ***************************************************************************
// bamtools_index.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 26 May 2010
// ---------------------------------------------------------------------------
// Creates a BAM index (".bai") file for the provided BAM file.
// ***************************************************************************

#ifndef BAMTOOLS_INDEX_H
#define BAMTOOLS_INDEX_H

#include <iostream>
#include <string>

#include "BamReader.h"
// #include "GetOpt.h"
#include "bamtools_getopt.h"

namespace BamTools {

int BamIndexHelp(void) { 
    std::cerr << std::endl;
    std::cerr << "usage:\tbamtools index [--nclist] <BAM file>" << std::endl;
    std::cerr << "\t--nclist\tUse NCList indexing scheme (faster?)\t[default=off] ** JUST HERE AS POSSIBLE SWITCH EXAMPLE FOR NOW **" << std::endl;
    std::cerr << "\t<BAM file>\tInput BAM file to generate index from\t[req'd]" << std::endl;
    std::cerr << std::endl;
    return 0;
}

int RunBamIndex(int argc, char* argv[]) {
  
    // else parse command line for args  
    GetOpt options(argc, argv, 1);
    
    std::string inputFilename;
    options.addRequiredArgument("input", &inputFilename);
    
    bool useNCList;
    options.addSwitch("nclist", &useNCList);
    
    if ( !options.parse() ) return BamIndexHelp();
    
    // open our BAM reader
    BamReader reader;
    reader.Open(inputFilename);
    
    // create index for BAM file
    reader.CreateIndex();
    
    // clean & exit
    reader.Close();
    return 0;
}

} // namespace BamTools

#endif // BAMTOOLS_INDEX_H
