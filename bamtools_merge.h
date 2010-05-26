// ***************************************************************************
// bamtools_merge.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 26 May 2010
// ---------------------------------------------------------------------------
// Merges multiple BAM files into one.
//
// ** Provide selectable region? eg chr2:10000..20000
//
// ***************************************************************************

#ifndef BAMTOOLS_MERGE_H
#define BAMTOOLS_MERGE_H

#include <iostream>
#include <string>
#include <vector>

#include "BamMultiReader.h"
#include "BamWriter.h"
// #include "GetOpt.h"
#include "bamtools_getopt.h"

namespace BamTools {

int BamMergeHelp(void) { 
    std::cerr << std::endl;
    std::cerr << "usage:\tbamtools merge [--out FILE] --in <BAM file1> [BAM file2] [BAM file3]..." << std::endl;
    std::cerr << "\t--in\tInput BAM file(s)\t\t[at least 1 req'd]" << std::endl;
    std::cerr << "\t--out\tDestination for merge results\t[default=stdout]" << std::endl;
    std::cerr << std::endl;
    return 0;
}

int RunBamMerge(int argc, char* argv[]) {
  
    // only 'bamtool merge', show help
    if ( argc == 2 ) return BamMergeHelp();
  
    // else parse command line for args
    GetOpt options(argc, argv, 1);
    
    std::string outputFilename = "";
    options.addOption('o', "out", &outputFilename);
    
    std::vector<std::string> inputFilenames;
    options.addVariableLengthOption("in", &inputFilenames);
    
    if ( !options.parse() || inputFilenames.empty() ) return BamMergeHelp();   
    if ( outputFilename.empty() ) { outputFilename = "stdout"; }
    
    // opens the BAM files without checking for indexes
    BamMultiReader reader;
    reader.Open(inputFilenames, false); 

    // retrieve header & reference dictionary info
    std::string mergedHeader = reader.GetHeaderText();
    RefVector references = reader.GetReferenceData();

    // open BamWriter
    BamWriter writer;
    writer.Open(outputFilename, mergedHeader, references);

    // store alignments to output file
    BamAlignment bAlignment;
    while (reader.GetNextAlignment(bAlignment)) {
        writer.SaveAlignment(bAlignment);
    }

    // clean & exit
    reader.Close();
    writer.Close();
    return 0;  
}

} // namespace BamTools

#endif // BAMTOOLS_MERGE_H