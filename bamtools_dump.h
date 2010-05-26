// ***************************************************************************
// bamtools_dump.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 26 May 2010
// ---------------------------------------------------------------------------
// Dumps alignment summaries out to stdout.
//
// ** This should probably go the way of the dodo soon? bamtools sam makes this
//   obsolete and probably worthless.
// 
// ***************************************************************************

#ifndef BAMTOOLS_DUMP_H
#define BAMTOOLS_DUMP_H

#include <iostream>
#include <string>
#include <vector>

#include "BamMultiReader.h"
// #include "GetOpt.h"
#include "bamtools_getopt.h"

namespace BamTools { 

int BamDumpHelp(void) { 
    std::cerr << std::endl;
    std::cerr << "usage:\tbamtools dump [BAM file1] [BAM file2] [BAM file3]..." << std::endl;
    std::cerr << "\t[BAM file]\tInput file(s) to dump alignment summaries from [default=stdin]" << std::endl;
    std::cerr << std::endl;
    return 0;
}

// Spit out basic BamAlignment data 
void PrintAlignment(const BamTools::BamAlignment& alignment) {
    std::cout << "---------------------------------" << std::endl;
    std::cout << "Name: "       << alignment.Name << std::endl;
    std::cout << "Aligned to: " << alignment.RefID;
    std::cout << ":"            << alignment.Position << std::endl;
    std::cout << std::endl;
}

int RunBamDump(int argc, char* argv[]) {
  
    // else parse command line for args
    GetOpt options(argc, argv, 1);
    
    std::vector<std::string> inputFilenames;
    options.addVariableLengthOption("in", &inputFilenames);

    if ( !options.parse() ) return BamDumpHelp();
    if ( inputFilenames.empty() ) { inputFilenames.push_back("stdin"); }
    
    // open files
    BamMultiReader reader;
    reader.Open(inputFilenames, false);

    // dump alignment summaries to stdout
    BamAlignment bAlignment;
    while (reader.GetNextAlignment(bAlignment)) {
        PrintAlignment(bAlignment);
    }

    // clean up & exit
    reader.Close(); 
    return 0;
}

} // namespace BamTools

#endif // BAMTOOLS_DUMP_H
