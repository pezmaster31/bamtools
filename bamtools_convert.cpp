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
//#include "bamtools_format.h"
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
  
    void PrintFASTA(ostream& out, const BamAlignment& a);
    void PrintFASTQ(ostream& out, const BamAlignment& a);
    void PrintJSON(ostream& out, const BamAlignment& a);
    void PrintSAM(ostream& out, const BamAlignment& a);
    
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
        //cout << "Converting to FASTA" << endl;
    }
    
    // FASTQ
    else if ( m_settings->Format == FORMAT_FASTQ) {
        //cout << "Converting to FASTQ" << endl;
    }
    
    // JSON
    else if ( m_settings->Format == FORMAT_JSON ) {
        //cout << "Converting to JSON" << endl;
        BamAlignment alignment;
        while ( reader.GetNextAlignment(alignment) ) {
            PrintJSON(cout, alignment);
        }

    }
    
    // SAM
    else if ( m_settings->Format == FORMAT_SAM ) {
        BamAlignment alignment;
        while ( reader.GetNextAlignment(alignment) ) {
            PrintSAM(cout, alignment);
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
void BamTools::PrintFASTA(ostream& out, const BamAlignment& a) { 

}

// print BamAlignment in FASTQ format
void BamTools::PrintFASTQ(ostream& out, const BamAlignment& a) { 

}

// print BamAlignment in JSON format
void BamTools::PrintJSON(ostream& out, const BamAlignment& a) {
  
    // tab-delimited
    // <QNAME> <FLAG> <RNAME> <POS> <MAPQ> <CIGAR> <MRNM> <MPOS> <ISIZE> <SEQ> <QUAL> [ <TAG>:<VTYPE>:<VALUE> [...] ]
  
    // write name & alignment flag
    out << "{\"name\":\"" << a.Name << "\",\"alignmentFlag\":\"" 
        << a.AlignmentFlag << "\",";
    
    // write reference name
    if ( (a.RefID >= 0) && (a.RefID < (int)references.size()) ) out << "\"reference\":\"" << references[a.RefID].RefName << "\",";
    //else out << "*\t";
    
    // write position & map quality
    out << "\"position\":" << a.Position+1 << ",\"mapQuality\":" << a.MapQuality << ",";
    
    // write CIGAR
    const vector<CigarOp>& cigarData = a.CigarData;
    if ( !cigarData.empty() ) {
        out << "\"cigar\":[";
        vector<CigarOp>::const_iterator cigarBegin = cigarData.begin();
        vector<CigarOp>::const_iterator cigarIter = cigarBegin;
        vector<CigarOp>::const_iterator cigarEnd  = cigarData.end();
        for ( ; cigarIter != cigarEnd; ++cigarIter ) {
            const CigarOp& op = (*cigarIter);
            if (cigarIter != cigarBegin) out << ",";
            out << "\"" << op.Length << op.Type << "\"";
        }
        out << "],";
    }
    
    // write mate reference name, mate position, & insert size
    if ( a.IsPaired() && (a.MateRefID >= 0) && (a.MateRefID < (int)references.size()) ) {
        out << "\"mate\":{"
            << "\"reference\":\"" << references[a.MateRefID].RefName << "\","
            << "\"position\":" << a.MatePosition+1
            << ",\"insertSize\":" << a.InsertSize << "},";
    }
    
    // write sequence
    if ( !a.QueryBases.empty() ) {
        out << "\"queryBases\":\"" << a.QueryBases << "\",";
    }
    
    // write qualities
    if ( !a.Qualities.empty() ) {
        out << "\"qualities\":\"" << a.Qualities << "\",";
    }
    
    // write tag data
    const char* tagData = a.TagData.c_str();
    const size_t tagDataLength = a.TagData.length();
    size_t index = 0;
    if (index < tagDataLength) {

        out << "\"tags\":{";
        
        while ( index < tagDataLength ) {

            if (index > 0)
                out << ",";
            
            // write tag name
            out << "\"" << a.TagData.substr(index, 2) << "\":";
            index += 2;
            
            // get data type
            char type = a.TagData.at(index);
            ++index;
            
            switch (type) {
                case('A') : 
                    out << "\"" << tagData[index] << "\"";
                    ++index; 
                    break;
                
                case('C') : 
                    out << (int)tagData[index]; 
                    ++index; 
                    break;
                
                case('c') : 
                    out << (int)tagData[index];
                    ++index; 
                    break;
                
                case('S') : 
                    out << BgzfData::UnpackUnsignedShort(&tagData[index]); 
                    index += 2; 
                    break;
                    
                case('s') : 
                    out << BgzfData::UnpackSignedShort(&tagData[index]);
                    index += 2; 
                    break;
                
                case('I') : 
                    out << BgzfData::UnpackUnsignedInt(&tagData[index]);
                    index += 4; 
                    break;
                
                case('i') : 
                    out << BgzfData::UnpackSignedInt(&tagData[index]);
                    index += 4; 
                    break;
                
                case('f') : 
                    out << BgzfData::UnpackFloat(&tagData[index]);
                    index += 4; 
                    break;
                
                case('d') : 
                    out << BgzfData::UnpackDouble(&tagData[index]);
                    index += 8; 
                    break;
                
                case('Z') :
                case('H') : 
                    out << "\""; 
                    while (tagData[index]) {
                        out << tagData[index];
                        ++index;
                    }
                    out << "\""; 
                    ++index; 
                    break;      
            }
            
            if ( tagData[index] == '\0') break;
        }

        out << "}";
    }

    out << "}" << endl;
    
}

// print BamAlignment in SAM format
void BamTools::PrintSAM(ostream& out, const BamAlignment& a) {
  
    // tab-delimited
    // <QNAME> <FLAG> <RNAME> <POS> <MAPQ> <CIGAR> <MRNM> <MPOS> <ISIZE> <SEQ> <QUAL> [ <TAG>:<VTYPE>:<VALUE> [...] ]
  
    // write name & alignment flag
    out << a.Name << "\t" << a.AlignmentFlag << "\t";
    
    // write reference name
    if ( (a.RefID >= 0) && (a.RefID < (int)references.size()) ) out << references[a.RefID].RefName << "\t";
    else out << "*\t";
    
    // write position & map quality
    out << a.Position+1 << "\t" << a.MapQuality << "\t";
    
    // write CIGAR
    const vector<CigarOp>& cigarData = a.CigarData;
    if ( cigarData.empty() ) out << "*\t";
    else {
        vector<CigarOp>::const_iterator cigarIter = cigarData.begin();
        vector<CigarOp>::const_iterator cigarEnd  = cigarData.end();
        for ( ; cigarIter != cigarEnd; ++cigarIter ) {
            const CigarOp& op = (*cigarIter);
            out << op.Length << op.Type;
        }
        out << "\t";
    }
    
    // write mate reference name, mate position, & insert size
    if ( a.IsPaired() && (a.MateRefID >= 0) && (a.MateRefID < (int)references.size()) ) {
        if ( a.MateRefID == a.RefID ) out << "=\t";
        else out << references[a.MateRefID].RefName << "\t";
        out << a.MatePosition+1 << "\t" << a.InsertSize << "\t";
    } 
    else out << "*\t0\t0\t";
    
    // write sequence
    if ( a.QueryBases.empty() ) out << "*\t";
    else out << a.QueryBases << "\t";
    
    // write qualities
    if ( a.Qualities.empty() ) out << "*";
    else out << a.Qualities;
    
    // write tag data
    const char* tagData = a.TagData.c_str();
    const size_t tagDataLength = a.TagData.length();
    
    size_t index = 0;
    while ( index < tagDataLength ) {

        // write tag name        
        out << "\t" << a.TagData.substr(index, 2) << ":";
        index += 2;
        
        // get data type
        char type = a.TagData.at(index);
        ++index;
        
        switch (type) {
            case('A') : 
                out << "A:" << tagData[index]; 
                ++index; 
                break;
            
            case('C') : 
                out << "i:" << (int)tagData[index]; 
                ++index; 
                break;
            
            case('c') : 
                out << "i:" << (int)tagData[index];
                ++index; 
                break;
            
            case('S') : 
                out << "i:" << BgzfData::UnpackUnsignedShort(&tagData[index]); 
                index += 2; 
                break;
                
            case('s') : 
                out << "i:" << BgzfData::UnpackSignedShort(&tagData[index]);
                index += 2; 
                break;
            
            case('I') :
                out << "i:" << BgzfData::UnpackUnsignedInt(&tagData[index]);
                index += 4; 
                break;
            
            case('i') : 
                out << "i:" << BgzfData::UnpackSignedInt(&tagData[index]);
                index += 4; 
                break;
            
            case('f') : 
                out << "f:" << BgzfData::UnpackFloat(&tagData[index]);
                index += 4; 
                break;
            
            case('d') : 
                out << "d:" << BgzfData::UnpackDouble(&tagData[index]);
                index += 8; 
                break;
            
            case('Z') :
            case('H') : 
                out << type << ":"; 
                while (tagData[index]) {
                    out << tagData[index];
                    ++index;
                }
                ++index; 
                break;      
        }

        if ( tagData[index] == '\0') break;
    }

    out << endl;
}
