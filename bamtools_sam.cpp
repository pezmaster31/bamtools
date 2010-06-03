// ***************************************************************************
// bamtools_sam.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 2 June 2010
// ---------------------------------------------------------------------------
// Prints a BAM file in the text-based SAM format.
// ***************************************************************************

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "bamtools_sam.h"
#include "bamtools_options.h"
#include "BamReader.h"
#include "BGZF.h"

using namespace std;
using namespace BamTools;

static RefVector references;

// ---------------------------------------------
// print BamAlignment in SAM format

void PrintSAM(const BamAlignment& a) {
  
    // tab-delimited
    // <QNAME> <FLAG> <RNAME> <POS> <MAPQ> <CIGAR> <MRNM> <MPOS> <ISIZE> <SEQ> <QUAL> [ <TAG>:<VTYPE>:<VALUE> [...] ]
  
    ostringstream sb("");
    
    // write name & alignment flag
    cout << a.Name << "\t" << a.AlignmentFlag << "\t";
    
    // write reference name
    if ( (a.RefID >= 0) && (a.RefID < (int)references.size()) ) cout << references[a.RefID].RefName << "\t";
    else cout << "*\t";
    
    // write position & map quality
    cout << a.Position+1 << "\t" << a.MapQuality << "\t";
    
    // write CIGAR
    const vector<CigarOp>& cigarData = a.CigarData;
    if ( cigarData.empty() ) cout << "*\t";
    else {
        vector<CigarOp>::const_iterator cigarIter = cigarData.begin();
        vector<CigarOp>::const_iterator cigarEnd  = cigarData.end();
        for ( ; cigarIter != cigarEnd; ++cigarIter ) {
            const CigarOp& op = (*cigarIter);
            cout << op.Length << op.Type;
        }
        cout << "\t";
    }
    
    // write mate reference name, mate position, & insert size
    if ( a.IsPaired() && (a.MateRefID >= 0) && (a.MateRefID < (int)references.size()) ) {
        if ( a.MateRefID == a.RefID ) cout << "=\t";
        else cout << references[a.MateRefID].RefName << "\t";
        cout << a.MatePosition+1 << "\t" << a.InsertSize << "\t";
    } 
    else cout << "*\t0\t0\t";
    
    // write sequence
    if ( a.QueryBases.empty() ) cout << "*\t";
    else cout << a.QueryBases << "\t";
    
    // write qualities
    if ( a.Qualities.empty() ) cout << "*";
    else cout << a.Qualities;
    
    // write tag data
    const char* tagData = a.TagData.c_str();
    const size_t tagDataLength = a.TagData.length();
    size_t index = 0;
    while ( index < tagDataLength ) {
        
        // write tag name
        cout << "\t" << a.TagData.substr(index, 2) << ":";
        index += 2;
        
        // get data type
        char type = a.TagData.at(index);
        ++index;
        
        switch (type) {
            case('A') : 
                cout << "A:" << tagData[index]; 
                ++index; 
                break;
            
            case('C') : 
                cout << "i:" << atoi(&tagData[index]); 
                ++index; 
                break;
            
            case('c') : 
                cout << "i:" << atoi(&tagData[index]);
                ++index; 
                break;
            
            case('S') : 
                cout << "i:" << BgzfData::UnpackUnsignedShort(&tagData[index]); 
                index += 2; 
                break;
                
            case('s') : 
                cout << "i:" << BgzfData::UnpackSignedShort(&tagData[index]);
                index += 2; 
                break;
            
            case('I') : 
                cout << "i:" << BgzfData::UnpackUnsignedInt(&tagData[index]);
                index += 4; 
                break;
            
            case('i') : 
                cout << "i:" << BgzfData::UnpackSignedInt(&tagData[index]);
                index += 4; 
                break;
            
            case('f') : 
                cout << "f:" << BgzfData::UnpackFloat(&tagData[index]);
                index += 4; 
                break;
            
            case('d') : 
                cout << "d:" << BgzfData::UnpackDouble(&tagData[index]);
                index += 8; 
                break;
            
            case('Z') :
            case('H') : 
                cout << type << ":"; 
                while (tagData[index]) {
                    cout << tagData[index];
                    ++index;
                }
                ++index; 
                break;      
        }
    }
    
    // write stream to stdout
    cout << sb.str() << endl;
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
