// ***************************************************************************
// bamtools_convert.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 9 July 2010
// ---------------------------------------------------------------------------
// Converts between BAM and a number of other formats
// ***************************************************************************

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "bamtools_convert.h"
#include "bamtools_options.h"
#include "bamtools_utilities.h"
#include "BGZF.h"
#include "BamReader.h"
#include "BamMultiReader.h"

using namespace std;
using namespace BamTools;
  
namespace BamTools { 
  
    // format names
    static const string FORMAT_FASTA_LOWER = "fasta";
    static const string FORMAT_FASTA_UPPER = "FASTA";
    static const string FORMAT_FASTQ_LOWER = "fastq";
    static const string FORMAT_FASTQ_UPPER = "FASTQ";
    static const string FORMAT_JSON_LOWER  = "json";
    static const string FORMAT_JSON_UPPER  = "JSON";
    static const string FORMAT_SAM_LOWER   = "sam";
    static const string FORMAT_SAM_UPPER   = "SAM";

    // other constants
    static const unsigned int FASTA_LINE_MAX = 50;
    
} // namespace BamTools
  
struct ConvertTool::ConvertToolPrivate {
  
    // ctor & dtor
    public:
        ConvertToolPrivate(ConvertTool::ConvertSettings* settings);
        ~ConvertToolPrivate(void);
    
    // interface
    public:
        bool Run(void);
    
    // internal methods
    private:
        void PrintFASTA(const BamAlignment& a);
        void PrintFASTQ(const BamAlignment& a);
        void PrintJSON(const BamAlignment& a);
        void PrintSAM(const BamAlignment& a);
        
    // data members
    private: 
        ConvertTool::ConvertSettings* m_settings;
        RefVector m_references;
        ostream m_out;
};

// ---------------------------------------------
// ConvertSettings implementation

struct ConvertTool::ConvertSettings {

    // flags
    bool HasInputFilenames;
    bool HasOutputFilename;
    bool HasFormat;
    bool HasRegion;

    // options
    vector<string> InputFiles;
    string OutputFilename;
    string Format;
    string Region;

    // constructor
    ConvertSettings(void)
        : HasInputFilenames(false)
        , HasOutputFilename(false)
        , HasFormat(false)
        , HasRegion(false)
        , OutputFilename(Options::StandardOut())
    { } 
};  

// ---------------------------------------------
// ConvertTool implementation

ConvertTool::ConvertTool(void)
    : AbstractTool()
    , m_settings(new ConvertSettings)
    , m_impl(0)
{
    // set program details
    Options::SetProgramInfo("bamtools convert", "converts between BAM and a number of other formats", "-in <filename> -out <filename> -format <FORMAT>");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in",     "BAM filename", "the input BAM file(s)", "", m_settings->HasInputFilenames,  m_settings->InputFiles,     IO_Opts, Options::StandardIn());
    Options::AddValueOption("-out",    "BAM filename", "the output BAM file",   "", m_settings->HasOutputFilename,  m_settings->OutputFilename, IO_Opts, Options::StandardOut());
    Options::AddValueOption("-format", "FORMAT", "the output file format - see README for recognized formats", "", m_settings->HasFormat, m_settings->Format, IO_Opts);
   
    OptionGroup* FilterOpts = Options::CreateOptionGroup("Filters");
    Options::AddValueOption("-region", "REGION", "genomic region. Index file is recommended for better performance, and is read automatically if it exists as <filename>.bai. See \'bamtools help index\' for more  details on creating one", "", m_settings->HasRegion, m_settings->Region, FilterOpts);
}

ConvertTool::~ConvertTool(void) {
    delete m_settings;
    m_settings = 0;
    
    delete m_impl;
    m_impl = 0;
}

int ConvertTool::Help(void) {
    Options::DisplayHelp();
    return 0;
}

int ConvertTool::Run(int argc, char* argv[]) {
  
    // parse command line arguments
    Options::Parse(argc, argv, 1);
    
    // run internal ConvertTool implementation, return success/fail
    m_impl = new ConvertToolPrivate(m_settings);
    
    if ( m_impl->Run() ) 
        return 0;
    else 
        return 1;
}

// ---------------------------------------------
// ConvertToolPrivate implementation

ConvertTool::ConvertToolPrivate::ConvertToolPrivate(ConvertTool::ConvertSettings* settings)
    : m_settings(settings)
    , m_out(cout.rdbuf()) // default output to cout
{ }

ConvertTool::ConvertToolPrivate::~ConvertToolPrivate(void) { }

bool ConvertTool::ConvertToolPrivate::Run(void) {
 
    bool convertedOk = true;

    // ------------------------------------
    // initialize conversion input/output
        
    // set to default input if none provided
    if ( !m_settings->HasInputFilenames ) 
        m_settings->InputFiles.push_back(Options::StandardIn());
    
    // open files
    BamMultiReader reader;
    reader.Open(m_settings->InputFiles);
    m_references = reader.GetReferenceData();

    BamRegion region;
    if ( Utilities::ParseRegionString(m_settings->Region, reader, region) ) {
        if ( !reader.SetRegion(region) ) {
           cerr << "Could not set BamReader region to REGION: " << m_settings->Region << endl;
        }
    }
        
    // if an output filename given, open outfile 
    ofstream outFile;
    if ( m_settings->HasOutputFilename ) { 
        outFile.open(m_settings->OutputFilename.c_str());
        if (!outFile) { cerr << "Could not open " << m_settings->OutputFilename << " for output" << endl; return false; }
        m_out.rdbuf(outFile.rdbuf()); // set m_out to file's streambuf
    }
    
    // ------------------------
    // do conversion
    
    // FASTA
    if ( m_settings->Format == FORMAT_FASTA_LOWER || m_settings->Format == FORMAT_FASTA_UPPER ) {
        BamAlignment alignment;
        while ( reader.GetNextAlignment(alignment) ) {
            PrintFASTA(alignment);
        }
    }
    
    // FASTQ
    else if ( m_settings->Format == FORMAT_FASTQ_LOWER || m_settings->Format == FORMAT_FASTQ_UPPER ) {
        BamAlignment alignment;
        while ( reader.GetNextAlignment(alignment) ) {
            PrintFASTQ(alignment);
        }
    }
    
    // JSON 
    else if ( m_settings->Format == FORMAT_JSON_LOWER || m_settings->Format == FORMAT_JSON_UPPER ) {
        BamAlignment alignment;
        while ( reader.GetNextAlignment(alignment) ) {
            PrintJSON(alignment);
        }
    }
    
    // SAM
    else if ( m_settings->Format == FORMAT_SAM_LOWER || m_settings->Format == FORMAT_SAM_UPPER ) {
        BamAlignment alignment;
        while ( reader.GetNextAlignment(alignment) ) {
            PrintSAM(alignment);
        }
    }
    
    // error
    else {
        cerr << "Unrecognized format: " << m_settings->Format << endl;
        cerr << "Please see help|README (?) for details on supported formats " << endl;
        convertedOk = false;
    }
    
    // ------------------------
    // clean up & exit
    reader.Close();
    if ( m_settings->HasOutputFilename ) outFile.close();
    return convertedOk;  
}

// ----------------------------------------------------------
// Conversion/output methods
// ----------------------------------------------------------

// print BamAlignment in FASTA format
// N.B. - uses QueryBases NOT AlignedBases
void ConvertTool::ConvertToolPrivate::PrintFASTA(const BamAlignment& a) { 
    
    // >BamAlignment.Name
    // BamAlignment.QueryBases
    // ...
  
    // print header
    m_out << "> " << a.Name << endl;
    
    // if sequence fits on single line
    if ( a.QueryBases.length() <= FASTA_LINE_MAX )
        m_out << a.QueryBases << endl;
    
    // else split over multiple lines
    else {
      
        size_t position = 0;
        size_t seqLength = a.QueryBases.length();
        
        // write subsequences to each line
        while ( position < (seqLength - FASTA_LINE_MAX) ) {
            m_out << a.QueryBases.substr(position, FASTA_LINE_MAX) << endl;
            position += FASTA_LINE_MAX;
        }
        
        // write final subsequence
        m_out << a.QueryBases.substr(position) << endl;
    }
}

// print BamAlignment in FASTQ format
// N.B. - uses QueryBases NOT AlignedBases
void ConvertTool::ConvertToolPrivate::PrintFASTQ(const BamAlignment& a) { 
  
    // @BamAlignment.Name
    // BamAlignment.QueryBases
    // +
    // BamAlignment.Qualities
  
    m_out << "@" << a.Name << endl
          << a.QueryBases   << endl
          << "+"            << endl
          << a.Qualities    << endl;
}

// print BamAlignment in JSON format
void ConvertTool::ConvertToolPrivate::PrintJSON(const BamAlignment& a) {
  
    // write name & alignment flag
    m_out << "{\"name\":\"" << a.Name << "\",\"alignmentFlag\":\"" << a.AlignmentFlag << "\",";
    
    // write reference name
    if ( (a.RefID >= 0) && (a.RefID < (int)m_references.size()) ) 
        m_out << "\"reference\":\"" << m_references[a.RefID].RefName << "\",";
    
    // write position & map quality
    m_out << "\"position\":" << a.Position+1 << ",\"mapQuality\":" << a.MapQuality << ",";
    
    // write CIGAR
    const vector<CigarOp>& cigarData = a.CigarData;
    if ( !cigarData.empty() ) {
        m_out << "\"cigar\":[";
        vector<CigarOp>::const_iterator cigarBegin = cigarData.begin();
        vector<CigarOp>::const_iterator cigarIter = cigarBegin;
        vector<CigarOp>::const_iterator cigarEnd  = cigarData.end();
        for ( ; cigarIter != cigarEnd; ++cigarIter ) {
            const CigarOp& op = (*cigarIter);
            if (cigarIter != cigarBegin) m_out << ",";
            m_out << "\"" << op.Length << op.Type << "\"";
        }
        m_out << "],";
    }
    
    // write mate reference name, mate position, & insert size
    if ( a.IsPaired() && (a.MateRefID >= 0) && (a.MateRefID < (int)m_references.size()) ) {
        m_out << "\"mate\":{"
              << "\"reference\":\"" << m_references[a.MateRefID].RefName << "\","
              << "\"position\":" << a.MatePosition+1
              << ",\"insertSize\":" << a.InsertSize << "},";
    }
    
    // write sequence
    if ( !a.QueryBases.empty() ) 
        m_out << "\"queryBases\":\"" << a.QueryBases << "\",";
    
    // write qualities
    if ( !a.Qualities.empty() )
        m_out << "\"qualities\":\"" << a.Qualities << "\",";
    
    // write tag data
    const char* tagData = a.TagData.c_str();
    const size_t tagDataLength = a.TagData.length();
    size_t index = 0;
    if (index < tagDataLength) {

        m_out << "\"tags\":{";
        
        while ( index < tagDataLength ) {

            if (index > 0)
                m_out << ",";
            
            // write tag name
            m_out << "\"" << a.TagData.substr(index, 2) << "\":";
            index += 2;
            
            // get data type
            char type = a.TagData.at(index);
            ++index;
            
            switch (type) {
                case('A') : 
                    m_out << "\"" << tagData[index] << "\"";
                    ++index; 
                    break;
                
                case('C') : 
                    m_out << (int)tagData[index]; 
                    ++index; 
                    break;
                
                case('c') : 
                    m_out << (int)tagData[index];
                    ++index; 
                    break;
                
                case('S') : 
                    m_out << BgzfData::UnpackUnsignedShort(&tagData[index]); 
                    index += 2; 
                    break;
                    
                case('s') : 
                    m_out << BgzfData::UnpackSignedShort(&tagData[index]);
                    index += 2; 
                    break;
                
                case('I') : 
                    m_out << BgzfData::UnpackUnsignedInt(&tagData[index]);
                    index += 4; 
                    break;
                
                case('i') : 
                    m_out << BgzfData::UnpackSignedInt(&tagData[index]);
                    index += 4; 
                    break;
                
                case('f') : 
                    m_out << BgzfData::UnpackFloat(&tagData[index]);
                    index += 4; 
                    break;
                
                case('d') : 
                    m_out << BgzfData::UnpackDouble(&tagData[index]);
                    index += 8; 
                    break;
                
                case('Z') :
                case('H') : 
                    m_out << "\""; 
                    while (tagData[index]) {
                        m_out << tagData[index];
                        ++index;
                    }
                    m_out << "\""; 
                    ++index; 
                    break;      
            }
            
            if ( tagData[index] == '\0') 
                break;
        }

        m_out << "}";
    }

    m_out << "}" << endl;
    
}

// print BamAlignment in SAM format
void ConvertTool::ConvertToolPrivate::PrintSAM(const BamAlignment& a) {
  
    // tab-delimited
    // <QNAME> <FLAG> <RNAME> <POS> <MAPQ> <CIGAR> <MRNM> <MPOS> <ISIZE> <SEQ> <QUAL> [ <TAG>:<VTYPE>:<VALUE> [...] ]
  
    // write name & alignment flag
    m_out << a.Name << "\t" << a.AlignmentFlag << "\t";
    
    // write reference name
    if ( (a.RefID >= 0) && (a.RefID < (int)m_references.size()) ) 
        m_out << m_references[a.RefID].RefName << "\t";
    else 
        m_out << "*\t";
    
    // write position & map quality
    m_out << a.Position+1 << "\t" << a.MapQuality << "\t";
    
    // write CIGAR
    const vector<CigarOp>& cigarData = a.CigarData;
    if ( cigarData.empty() ) m_out << "*\t";
    else {
        vector<CigarOp>::const_iterator cigarIter = cigarData.begin();
        vector<CigarOp>::const_iterator cigarEnd  = cigarData.end();
        for ( ; cigarIter != cigarEnd; ++cigarIter ) {
            const CigarOp& op = (*cigarIter);
            m_out << op.Length << op.Type;
        }
        m_out << "\t";
    }
    
    // write mate reference name, mate position, & insert size
    if ( a.IsPaired() && (a.MateRefID >= 0) && (a.MateRefID < (int)m_references.size()) ) {
        if ( a.MateRefID == a.RefID ) m_out << "=\t";
        else m_out << m_references[a.MateRefID].RefName << "\t";
        m_out << a.MatePosition+1 << "\t" << a.InsertSize << "\t";
    } 
    else m_out << "*\t0\t0\t";
    
    // write sequence
    if ( a.QueryBases.empty() ) m_out << "*\t";
    else m_out << a.QueryBases << "\t";
    
    // write qualities
    if ( a.Qualities.empty() ) m_out << "*";
    else m_out << a.Qualities;
    
    // write tag data
    const char* tagData = a.TagData.c_str();
    const size_t tagDataLength = a.TagData.length();
    
    size_t index = 0;
    while ( index < tagDataLength ) {

        // write tag name        
        m_out << "\t" << a.TagData.substr(index, 2) << ":";
        index += 2;
        
        // get data type
        char type = a.TagData.at(index);
        ++index;
        
        switch (type) {
            case('A') : 
                m_out << "A:" << tagData[index]; 
                ++index; 
                break;
            
            case('C') : 
                m_out << "i:" << (int)tagData[index]; 
                ++index; 
                break;
            
            case('c') : 
                m_out << "i:" << (int)tagData[index];
                ++index; 
                break;
            
            case('S') : 
                m_out << "i:" << BgzfData::UnpackUnsignedShort(&tagData[index]); 
                index += 2; 
                break;
                
            case('s') : 
                m_out << "i:" << BgzfData::UnpackSignedShort(&tagData[index]);
                index += 2; 
                break;
            
            case('I') :
                m_out << "i:" << BgzfData::UnpackUnsignedInt(&tagData[index]);
                index += 4; 
                break;
            
            case('i') : 
                m_out << "i:" << BgzfData::UnpackSignedInt(&tagData[index]);
                index += 4; 
                break;
            
            case('f') : 
                m_out << "f:" << BgzfData::UnpackFloat(&tagData[index]);
                index += 4; 
                break;
            
            case('d') : 
                m_out << "d:" << BgzfData::UnpackDouble(&tagData[index]);
                index += 8; 
                break;
            
            case('Z') :
            case('H') : 
                m_out << type << ":"; 
                while (tagData[index]) {
                    m_out << tagData[index];
                    ++index;
                }
                ++index; 
                break;      
        }

        if ( tagData[index] == '\0') 
            break;
    }

    m_out << endl;
}
