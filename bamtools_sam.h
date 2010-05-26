// ***************************************************************************
// bamtools_sam.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 26 May 2010
// ---------------------------------------------------------------------------
// Prints a BAM file in the text-based SAM format.
// ***************************************************************************

#ifndef BAMTOOLS_SAM_H
#define BAMTOOLS_SAM_H

#include <cstdlib>
#include <iostream>
#include <string>

#include "BamReader.h"
#include "bamtools_getopt.h"

namespace BamTools {

int BamSamHelp(void) { 
  
    // '--head' makes more sense than '--num' from a Unix perspective, but could be confusing with header info ?? 
    // but this is also only the default case (from the beginning of the file)
    // do we want to add a region specifier, eg 'chr2:1000..1500'? In this case, '--num' still makes sense (give me up to N alignments from this region)
  
    std::cerr << std::endl;
    std::cerr << "usage:\tbamtools sam [--in BAM file] [--num N] [--no_header]" << std::endl;
    std::cerr << "\t-i, --in\tInput BAM file to generate SAM-format\t\t\t[default=stdin]" << std::endl;
    std::cerr << "\t-n, --num N\tOnly print up to N alignments from beginning of file\t\t[default=50*]" << endl;  
    std::cerr << "\t--no_header\tOmits SAM header information from output (alignments only)\t[default=off]" << std::endl;
    std::cerr << std::endl;
    std::cerr << "* - By default bamtools sam will print all alignments in SAM format." << std::endl;
    std::cerr << "    However if '-n' or '--num' is included with no N, the default of 50 is used." << std::endl;
    std::cerr << std::endl;
    return 0;
}

static RefVector references;

void PrintSAM(const BamAlignment& a) {
  
    // tab-delimited
    // <QNAME> <FLAG> <RNAME> <POS> <MAPQ> <CIGAR> <MRNM> <MPOS> <ISIZE> <SEQ> <QUAL> [ <TAG>:<VTYPE>:<VALUE> [...] ]
  
    // ******************************* //
    // ** NOT FULLY IMPLEMENTED YET ** //
    //******************************** //
    //
    // Todo : build CIGAR string
    //        build TAG string
    //        there are some quirks, per the spec, regarding when to use '=' or not
    //
    // ******************************* //
    
    //
    // do validity check on RefID / MateRefID ??
    //
  
    // build CIGAR string
    std::string cigarString("CIGAR:NOT YET");
  
    // build TAG string
    std::string tagString("TAG:NOT YET");
  
    // print BamAlignment to stdout in SAM format
    std::cout << a.Name << '\t' 
              << a.AlignmentFlag << '\t'
              << references[a.RefID].RefName << '\t'
              << a.Position << '\t'
              << a.MapQuality << '\t'
              << cigarString << '\t'
              << ( a.IsPaired() ? references[a.MateRefID].RefName : "*" ) << '\t'
              << ( a.IsPaired() ? a.MatePosition : 0 ) << '\t'
              << ( a.IsPaired() ? a.InsertSize : 0 ) << '\t'
              << a.QueryBases << '\t'
              << a.Qualities << '\t'
              << tagString << std::endl;
}

int RunBamSam(int argc, char* argv[]) {
  
    // else parse command line for args  
    GetOpt options(argc, argv, 1);
    
    std::string inputFilename;
    options.addOption('i', "in", &inputFilename);
    
    std::string numberString;
    options.addOptionalOption('n', "num", &numberString, "50");
    
    bool isOmittingHeader;
    options.addSwitch("no_header", &isOmittingHeader);
    
    if ( !options.parse() ) return BamCoverageHelp();
    if ( inputFilename.empty() ) { inputFilename = "stdin"; }
    
    // maxNumberOfAlignments = all (if nothing specified)
    //                       = 50  (if '-n' or '--num' but no N)
    //                       = N   (if '-n N' or '--num N') 
    int maxNumberOfAlignments = -1;
    if ( !numberString.empty() ) { maxNumberOfAlignments = atoi(numberString.c_str()); }
     
    // open our BAM reader
    BamReader reader;
    reader.Open(inputFilename);
    
    // if header desired, retrieve and print to stdout
    if ( !isOmittingHeader ) {
        std::string header = reader.GetHeaderText();
        std::cout << header << std::endl;
    }

    // store reference data
    references = reader.GetReferenceData();

    // print all alignments to stdout in SAM format
    if ( maxNumberOfAlignments < 0 ) {
        BamAlignment ba;
        while( reader.GetNextAlignment(ba) ) {
            PrintSAM(ba);
        }
    }  
    
    // print first N alignments to stdout in SAM format
    else {
        BamAlignment ba;
        int alignmentsPrinted = 0;
        while ( reader.GetNextAlignment(ba) && (alignmentsPrinted < maxNumberOfAlignments) ) {
            PrintSAM(ba);
            ++alignmentsPrinted;
        }
    }
    
    // clean & exit
    reader.Close();
    return 0;
}

} // namespace BamTools

#endif // BAMTOOLS_SAM_H