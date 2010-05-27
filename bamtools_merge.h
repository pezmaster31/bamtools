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
    std::cerr << "usage:\tbamtools merge [--out FILE] [--region REGION] [--in FILE [FILE] [FILE] ...] " << std::endl;
    std::cerr << std::endl;
    std::cerr << "\t--in FILE        Input BAM file(s)                      [stdin]" << std::endl;
    std::cerr << "\t--region REGION  Only keep alignments from this region. [all alignments]" << std::endl;
    std::cerr << "\t                 REGION format - (eg chr2:1000..2000)" << std::endl;
    std::cerr << "\t--out FILE       Destination for merge results          [stdout]" << std::endl;
    std::cerr << std::endl;
    return 0;
}

int RunBamMerge(int argc, char* argv[]) {
  
    // only 'bamtool merge', show help
    if ( argc == 2 ) return BamMergeHelp();
  
    // else parse command line for args
    GetOpt options(argc, argv, 1);
    
    std::string outputFilename = "";
    options.addOption("out", &outputFilename);
    
    std::vector<std::string> inputFilenames;
    options.addVariableLengthOption("in", &inputFilenames);
    
    std::string regionString;
    options.addOption("region", &regionString);
    
    if ( !options.parse() || inputFilenames.empty() ) return BamMergeHelp();   
    if ( inputFilenames.empty() ) { inputFilenames.push_back("stdin"); }
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

    // if desired region provided
    if ( !regionString.empty() ) {
        // parse region string
        // only get alignments from this region
    } 
    
    // else get all alignments
    else {
        // store alignments to output file
        BamAlignment bAlignment;
        while (reader.GetNextAlignment(bAlignment)) {
            writer.SaveAlignment(bAlignment);
        }
    }
    // clean & exit
    reader.Close();
    writer.Close();
    return 0;  
}

} // namespace BamTools

#endif // BAMTOOLS_MERGE_H