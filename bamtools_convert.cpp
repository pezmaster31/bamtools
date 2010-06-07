// ***************************************************************************
// bamtools_convert.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 7 June 2010
// ---------------------------------------------------------------------------
// Converts between BAM and a number of other formats
// ***************************************************************************

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "bamtools_convert.h"
#include "bamtools_format.h"
#include "bamtools_options.h"
#include "BGZF.h"
#include "BamReader.h"
#include "BamMultiReader.h"

using namespace std;
using namespace BamTools;
  
static RefVector references;  
  
namespace BamTools {
  
    static const string FORMAT_FASTA = "fasta";
    static const string FORMAT_FASTQ = "fastq";
    static const string FORMAT_JSON  = "json";
    static const string FORMAT_SAM   = "sam";
  
    void PrintFASTA(const BamAlignment& a);
    void PrintFASTQ(const BamAlignment& a);
    void PrintJSON(const BamAlignment& a);
    void PrintSAM(const BamAlignment& a);
    
} // namespace BamTools
  
// ---------------------------------------------
// ConvertSettings implementation

struct ConvertTool::ConvertSettings {

    // flags
    bool HasInputBamFilename;
    bool HasOutputBamFilename;
    bool HasFormat;

    // filenames
    string InputFilename;
    string OutputFilename;
    string Format;
    
    // constructor
    ConvertSettings(void)
        : HasInputBamFilename(false)
        , HasOutputBamFilename(false)
        , InputFilename(Options::StandardIn())
        , OutputFilename(Options::StandardOut())
    { } 
};  

// ---------------------------------------------
// ConvertTool implementation

ConvertTool::ConvertTool(void)
    : AbstractTool()
    , m_settings(new ConvertSettings)
{
    // set program details
    Options::SetProgramInfo("bamtools convert", "converts between BAM and a number of other formats", "-in <filename> -out <filename> -format <FORMAT>");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in",     "BAM filename", "the input BAM file(s)", "", m_settings->HasInputBamFilename,  m_settings->InputFilename,  IO_Opts, Options::StandardIn());
    Options::AddValueOption("-out",    "BAM filename", "the output BAM file",   "", m_settings->HasOutputBamFilename, m_settings->OutputFilename, IO_Opts, Options::StandardOut());
    Options::AddValueOption("-format", "FORMAT", "the output file format - see README for recognized formats", "", m_settings->HasFormat, m_settings->Format, IO_Opts);
}

ConvertTool::~ConvertTool(void) {
    delete m_settings;
    m_settings = 0;
}

int ConvertTool::Help(void) {
    Options::DisplayHelp();
    return 0;
}

int ConvertTool::Run(int argc, char* argv[]) {
  
    bool convertedOk = true;
  
    // parse command line arguments
    Options::Parse(argc, argv, 1);
    
    // open files
    BamReader reader;
    reader.Open(m_settings->InputFilename);
    references = reader.GetReferenceData();
        
    // ----------------------------------------
    // do conversion,depending on desired output format

    // FASTA
    if ( m_settings->Format == FORMAT_FASTA ) {
        cout << "Converting to FASTA" << endl;
    }
    
    // FASTQ
    else if ( m_settings->Format == FORMAT_FASTQ) {
        cout << "Converting to FASTQ" << endl;
    }
    
    // JSON
    else if ( m_settings->Format == FORMAT_JSON ) {
        cout << "Converting to JSON" << endl;
    }
    
    // SAM
    else if ( m_settings->Format == FORMAT_SAM ) {
        BamAlignment alignment;
        while ( reader.GetNextAlignment(alignment) ) {
            PrintSAM(alignment);
        }
    }
    
    // uncrecognized format
    else { 
        cerr << "Unrecognized format: " << m_settings->Format << endl;
        cerr << "Please see help|README (?) for details on supported formats " << endl;
        convertedOk = false;
    }
    
    // ------------------------
    // clean up & exit
    reader.Close();
    return (int)convertedOk;
}

// ----------------------------------------------------------
// Conversion/output methods
// ----------------------------------------------------------

// print BamAlignment in FASTA format
void BamTools::PrintFASTA(const BamAlignment& a) { 

}

// print BamAlignment in FASTQ format
void BamTools::PrintFASTQ(const BamAlignment& a) { 

}

// print BamAlignment in JSON format
void BamTools::PrintJSON(const BamAlignment& a) { 

}

// print BamAlignment in SAM format
void BamTools::PrintSAM(const BamAlignment& a) {
  
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
