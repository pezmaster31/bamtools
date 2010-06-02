// ***************************************************************************
// bamtools_sam.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 1 June 2010
// ---------------------------------------------------------------------------
// Prints a BAM file in the text-based SAM format.
// ***************************************************************************

#include <cstdlib>
#include <iostream>
#include <string>

#include "bamtools_sam.h"
#include "bamtools_options.h"
#include "BamReader.h"

using namespace std;
using namespace BamTools;

RefVector references;

// ---------------------------------------------
// print BamAlignment in SAM format

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
    string cigarString("CIGAR:NOT YET");
  
    // build TAG string
    string tagString("TAG:NOT YET");
  
    // print BamAlignment to stdout in SAM format
    cout << a.Name << '\t' 
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
         << tagString << endl;
}

// ---------------------------------------------
// SamSettings implementation

struct SamTool::SamSettings {

    // flags
    bool HasInputBamFilename;
    bool HasMaximumOutput;
    bool IsOmittingHeader;

    // filenames
    string InputBamFilename;
    
    // other parameters
    int MaximumOutput;
    
    // constructor
    SamSettings(void)
        : HasInputBamFilename(false)
        , HasMaximumOutput(false)
        , IsOmittingHeader(false)
        , InputBamFilename(Options::StandardIn())
    { }
};  

// ---------------------------------------------
// SamTool implementation

SamTool::SamTool(void)
    : AbstractTool()
    , m_settings(new SamSettings)
{
    // set program details
    Options::SetProgramInfo("bamtools sam", "prints BAM file in SAM text format", "-in <filename>");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in", "BAM filename", "the input BAM file", "", m_settings->HasInputBamFilename, m_settings->InputBamFilename, IO_Opts, Options::StandardIn());
    
    OptionGroup* FilterOpts = Options::CreateOptionGroup("Filters");
    Options::AddOption("-noheader", "omit the SAM header from output", m_settings->IsOmittingHeader, FilterOpts);
    Options::AddValueOption("-num", "N", "maximum number of alignments to output", "", m_settings->HasMaximumOutput, m_settings->MaximumOutput, FilterOpts);
}

SamTool::~SamTool(void) {
    delete m_settings;
    m_settings = 0;
}

int SamTool::Help(void) {
    Options::DisplayHelp();
    return 0;
}

int SamTool::Run(int argc, char* argv[]) {
  
    // parse command line arguments
    Options::Parse(argc, argv, 1);
    
    // open our BAM reader
    BamReader reader;
    reader.Open(m_settings->InputBamFilename);
    
    // if header desired, retrieve and print to stdout
    if ( !m_settings->IsOmittingHeader ) {
        string header = reader.GetHeaderText();
        cout << header << endl;
    }

    // store reference data
    references = reader.GetReferenceData();

    // print all alignments to stdout in SAM format
    if ( !m_settings->HasMaximumOutput ) {
        BamAlignment ba;
        while( reader.GetNextAlignment(ba) ) {
            PrintSAM(ba);
        }
    }  
    
    // print first N alignments to stdout in SAM format
    else {
        BamAlignment ba;
        int alignmentsPrinted = 0;
        while ( reader.GetNextAlignment(ba) && (alignmentsPrinted < m_settings->MaximumOutput) ) {
            PrintSAM(ba);
            ++alignmentsPrinted;
        }
    }
    
    // clean & exit
    reader.Close();
    return 0;
}
