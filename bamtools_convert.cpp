// ***************************************************************************
// bamtools_convert.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 22 July 2010
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
#include "bamtools_pileup.h"
#include "bamtools_utilities.h"
#include "BGZF.h"
#include "BamReader.h"
#include "BamMultiReader.h"

using namespace std;
using namespace BamTools;
  
namespace BamTools { 
 
    // format names
    static const string FORMAT_BED      = "bed";
    static const string FORMAT_BEDGRAPH = "bedgraph";
    static const string FORMAT_FASTA    = "fasta";
    static const string FORMAT_FASTQ    = "fastq";
    static const string FORMAT_JSON     = "json";
    static const string FORMAT_SAM      = "sam";
    static const string FORMAT_PILEUP   = "pileup";
    static const string FORMAT_WIGGLE   = "wig";

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
        void PrintBed(const BamAlignment& a);
        void PrintBedGraph(const BamAlignment& a);
        void PrintFasta(const BamAlignment& a);
        void PrintFastq(const BamAlignment& a);
        void PrintJson(const BamAlignment& a);
        void PrintSam(const BamAlignment& a);
        void PrintWiggle(const BamAlignment& a);
        
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
    bool HasInput;
    bool HasOutput;
    bool HasFormat;
    bool HasRegion;

    // pileup flags
    bool HasFastaFilename;
    bool IsOmittingSamHeader;
    bool IsPrintingPileupMapQualities;
    
    // options
    vector<string> InputFiles;
    string OutputFilename;
    string Format;
    string Region;
    
    // pileup options
    string FastaFilename;

    // constructor
    ConvertSettings(void)
        : HasInput(false)
        , HasOutput(false)
        , HasFormat(false)
        , HasRegion(false)
        , HasFastaFilename(false)
        , IsOmittingSamHeader(false)
        , IsPrintingPileupMapQualities(false)
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
    Options::SetProgramInfo("bamtools convert", "converts BAM to a number of other formats", "-format <FORMAT> [-in <filename> -in <filename> ...] [-out <filename>] [other options]");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in",     "BAM filename", "the input BAM file(s)", "", m_settings->HasInput,   m_settings->InputFiles,     IO_Opts, Options::StandardIn());
    Options::AddValueOption("-out",    "BAM filename", "the output BAM file",   "", m_settings->HasOutput,  m_settings->OutputFilename, IO_Opts, Options::StandardOut());
    Options::AddValueOption("-format", "FORMAT", "the output file format - see README for recognized formats", "", m_settings->HasFormat, m_settings->Format, IO_Opts);
   
    OptionGroup* FilterOpts = Options::CreateOptionGroup("Filters");
    Options::AddValueOption("-region", "REGION", "genomic region. Index file is recommended for better performance, and is read automatically if it exists as <filename>.bai. See \'bamtools help index\' for more  details on creating one", "", m_settings->HasRegion, m_settings->Region, FilterOpts);
    
    OptionGroup* PileupOpts = Options::CreateOptionGroup("Pileup Options");
    Options::AddValueOption("-fasta", "FASTA filename", "FASTA reference file", "", m_settings->HasFastaFilename, m_settings->FastaFilename, PileupOpts, "");
    Options::AddOption("-mapqual", "print the mapping qualities", m_settings->IsPrintingPileupMapQualities, PileupOpts);
    
    OptionGroup* SamOpts = Options::CreateOptionGroup("SAM Options");
    Options::AddOption("-noheader", "omit the SAM header from output", m_settings->IsOmittingSamHeader, SamOpts);
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
    if ( !m_settings->HasInput ) 
        m_settings->InputFiles.push_back(Options::StandardIn());
    
    // open input files
    BamMultiReader reader;
    reader.Open(m_settings->InputFiles, false);
    m_references = reader.GetReferenceData();

    // set region if specified
    BamRegion region;
    if ( m_settings->HasRegion ) {
        if ( Utilities::ParseRegionString(m_settings->Region, reader, region) ) {
            if ( !reader.SetRegion(region) )
              cerr << "Could not set BamReader region to REGION: " << m_settings->Region << endl;
        }
    }
        
    // if output file given
    ofstream outFile;
    if ( m_settings->HasOutput ) {
      
        // open output file stream
        outFile.open(m_settings->OutputFilename.c_str());
        if ( !outFile ) {
            cerr << "Could not open " << m_settings->OutputFilename << " for output" << endl; 
            return false; 
        }
        
        // set m_out to file's streambuf
        m_out.rdbuf(outFile.rdbuf()); 
    }
    
    // ------------------------
    // pileup is special case
    if ( m_settings->Format == FORMAT_PILEUP ) {
        
        // initialize pileup input/output
        Pileup pileup(&reader, &m_out);
        
        // ---------------------------
        // configure pileup settings
        
        if ( m_settings->HasRegion ) 
            pileup.SetRegion(region);
        
        if ( m_settings->HasFastaFilename ) 
            pileup.SetFastaFilename(m_settings->FastaFilename);
        
        pileup.SetIsPrintingMapQualities( m_settings->IsPrintingPileupMapQualities );
        
        // run pileup
        convertedOk = pileup.Run();
    }
    
    // -------------------------------------
    // else determine 'simpler' format type
    else {
    
        bool formatError = false;
        void (BamTools::ConvertTool::ConvertToolPrivate::*pFunction)(const BamAlignment&) = 0;
        if      ( m_settings->Format == FORMAT_BED )      pFunction = &BamTools::ConvertTool::ConvertToolPrivate::PrintBed;
        else if ( m_settings->Format == FORMAT_BEDGRAPH ) pFunction = &BamTools::ConvertTool::ConvertToolPrivate::PrintBedGraph;
        else if ( m_settings->Format == FORMAT_FASTA )    pFunction = &BamTools::ConvertTool::ConvertToolPrivate::PrintFasta;
        else if ( m_settings->Format == FORMAT_FASTQ )    pFunction = &BamTools::ConvertTool::ConvertToolPrivate::PrintFastq;
        else if ( m_settings->Format == FORMAT_JSON )     pFunction = &BamTools::ConvertTool::ConvertToolPrivate::PrintJson;
        else if ( m_settings->Format == FORMAT_SAM )      pFunction = &BamTools::ConvertTool::ConvertToolPrivate::PrintSam;
        else if ( m_settings->Format == FORMAT_WIGGLE )   pFunction = &BamTools::ConvertTool::ConvertToolPrivate::PrintWiggle;
        else { 
            cerr << "Unrecognized format: " << m_settings->Format << endl;
            cerr << "Please see help|README (?) for details on supported formats " << endl;
            formatError = true;
            convertedOk = false;
        }
        
        // if SAM format & not omitting header, print SAM header
        if ( (m_settings->Format == FORMAT_SAM) && !m_settings->IsOmittingSamHeader ) {
            string headerText = reader.GetHeaderText();
            m_out << headerText;
        }
        
        // ------------------------
        // do conversion
        if ( !formatError ) {
            BamAlignment a;
            while ( reader.GetNextAlignment(a) ) {
                (this->*pFunction)(a);
            }
        }
    }
    
    // ------------------------
    // clean up & exit
    reader.Close();
    if ( m_settings->HasOutput ) outFile.close();
    return convertedOk;   
}

// ----------------------------------------------------------
// Conversion/output methods
// ----------------------------------------------------------

void ConvertTool::ConvertToolPrivate::PrintBed(const BamAlignment& a)      { 
  
    // tab-delimited, 0-based half-open 
    // (e.g. a 50-base read aligned to pos 10 could have BED coordinates (10, 60) instead of BAM coordinates (10, 59) )
    // <chromName> <chromStart> <chromEnd> <readName> <score> <strand>

    m_out << m_references.at(a.RefID).RefName << "\t"
          << a.Position << "\t"
          << a.GetEndPosition() + 1 << "\t"
          << a.Name << "\t"
          << a.MapQuality << "\t"
          << (a.IsReverseStrand() ? "-" : "+") << endl;
}

void ConvertTool::ConvertToolPrivate::PrintBedGraph(const BamAlignment& a) { 
    ; 
}

// print BamAlignment in FASTA format
// N.B. - uses QueryBases NOT AlignedBases
void ConvertTool::ConvertToolPrivate::PrintFasta(const BamAlignment& a) { 
    
    // >BamAlignment.Name
    // BamAlignment.QueryBases (up to FASTA_LINE_MAX bases per line)
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
void ConvertTool::ConvertToolPrivate::PrintFastq(const BamAlignment& a) { 
  
    // @BamAlignment.Name
    // BamAlignment.QueryBases
    // +
    // BamAlignment.Qualities
  
    m_out << "@" << a.Name << endl
          << a.QueryBases  << endl
          << "+"           << endl
          << a.Qualities   << endl;
}

// print BamAlignment in JSON format
void ConvertTool::ConvertToolPrivate::PrintJson(const BamAlignment& a) {
  
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
void ConvertTool::ConvertToolPrivate::PrintSam(const BamAlignment& a) {
  
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
        string tagName = a.TagData.substr(index, 2);
        m_out << "\t" << tagName << ":";
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

void ConvertTool::ConvertToolPrivate::PrintWiggle(const BamAlignment& a) { 
    ; 
}