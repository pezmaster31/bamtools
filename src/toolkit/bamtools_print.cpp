// ***************************************************************************
// bamtools_print.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 24 Feb 2019
// ---------------------------------------------------------------------------
// Text printout of alignments at a particular reference location
// ***************************************************************************

#include "bamtools_print.h"

#include <api/BamReader.h>
#include <utils/bamtools_options.h>
#include <utils/bamtools_utilities.h>
using namespace BamTools;

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <locale>

const std::string TRUE_STR = "true";
const std::string FALSE_STR = "false";
const std::string FORWARD_STRAND_STR = "+";
const std::string REVERSE_STRAND_STR = "-";
bool iequals(const std::string& a, const std::string& b);
bool stringToBool(const std::string& s, const std::string& optionName);

// ---------------------------------------------
// SeqCounter is a class for tracking frequency of aligned sequence segments

typedef uint64_t seq_counter_int;
typedef std::map< std::string, std::pair< seq_counter_int, seq_counter_int > > seq_counter;
typedef seq_counter::iterator seq_counter_itr;

class SeqCounter
{
    
    public:
        SeqCounter()
            : m_fwd(0)
            , m_rev(0)
        {}
      
        void addForward(std::string seq, seq_counter_int i);
        void addReverse(std::string seq, seq_counter_int i);
        void add(std::string seq, seq_counter_int i, bool reverse_strand);
        void add(std::string seq, bool reverse_strand)
        {
            this->add(seq,1,reverse_strand);
        }
        void add(std::string seq)
        {
            this->addForward(seq,1);
        }
        void print(std::string prefix, bool include_total, bool by_strand);
        void print(std::string prefix)
        {
            this->print(prefix, true, true);
        }
        void print()
        {
            this->print("");
        }
        seq_counter_int countForward(std::string seq);
        seq_counter_int countReverse(std::string seq);
        seq_counter_int count(std::string seq)
        {
            return(this->countForward(seq));
        }
        seq_counter_int countForward()
        {
            return(m_fwd);
        }
        seq_counter_int countReverse()
        {
            return(m_rev);
        }
      
        seq_counter_itr const begin()
        {
            return(m_seq_count.begin());
        }
        seq_counter_itr const end()
        {
            return(m_seq_count.end());
        }
        SeqCounter & operator+= (SeqCounter &s2);
    
    private:
          seq_counter_int m_fwd;
          seq_counter_int m_rev;
          seq_counter m_seq_count;
};


SeqCounter & SeqCounter::operator+= (SeqCounter &s2)
{
    this->m_fwd += s2.countForward();
    this->m_rev += s2.countReverse();
    for(seq_counter_itr it = s2.begin(); it != s2.end(); ++it) {
        seq_counter_itr destination = this->m_seq_count.find(it->first);
        if(destination != m_seq_count.end()) {
            destination->second.first  += it->second.first;
            destination->second.second += it->second.second;
        } else {
            this->m_seq_count[it->first] = it->second;
        }
    }
    return(*this);
}

void SeqCounter::print(std::string prefix, bool include_total, bool by_strand)
{
    if(by_strand) {
        if(include_total)
            std::cout << prefix << "Total\t" << m_fwd << "\t" << m_rev << std::endl;
        for(seq_counter_itr it = m_seq_count.begin(); it != m_seq_count.end(); ++it)
            std::cout << prefix << it->first << "\t" << it->second.first << "\t" << it->second.second << std::endl;
    } else {
        if(include_total)
            std::cout << prefix << "Total\t" << m_fwd + m_rev << std::endl;
        for(seq_counter_itr it = m_seq_count.begin(); it != m_seq_count.end(); ++it)
            std::cout << prefix << it->first << "\t" << it->second.first + it->second.second << std::endl;
    }
}

void SeqCounter::addReverse(std::string seq, seq_counter_int i)
{
    m_rev++;
    seq_counter_itr it = m_seq_count.find(seq);
    if(it != m_seq_count.end())
        it->second.second += i;
    else
        m_seq_count[seq] = std::make_pair(0,i);
}

void SeqCounter::addForward(std::string seq, seq_counter_int i)
{
    m_fwd++;
    seq_counter_itr it = m_seq_count.find(seq);
    if(it != m_seq_count.end())
        it->second.first += i;
    else
        m_seq_count[seq] = std::make_pair(i,0);
}

void SeqCounter::add(std::string seq, seq_counter_int i, bool reverse_strand)
{
    if(reverse_strand)
        this->addReverse(seq,i);
    else
        this->addForward(seq,i);
}

seq_counter_int SeqCounter::countForward(std::string seq)
{
    seq_counter_itr it = m_seq_count.find(seq);
    return( it != m_seq_count.end() ? it->second.first : 0 );
}

seq_counter_int SeqCounter::countReverse(std::string seq)
{
    seq_counter_itr it = m_seq_count.find(seq);
    return( it != m_seq_count.end() ? it->second.second : 0 );
}

// ---------------------------------------------
// PrintSettings implementation

struct PrintTool::PrintSettings
{

    // flags
    bool HasInput;
    bool HasInputFilelist;
    bool HasRegion;

    // flags which can be set as true or false
    bool ByBamFile;
    bool HasByBamFile;
    std::string ByBamFileStr;
    bool ByStrand;
    bool HasByStrand;
    std::string ByStrandStr;
    bool DotForMatch;
    bool HasDotForMatch;
    std::string DotForMatchStr;
    bool AllReads;
    bool HasAllReads;
    std::string AllReadsStr;
    bool IncludeTotal;
    bool HasIncludeTotal;
    std::string IncludeTotalStr;

    // filenames
    std::vector<std::string> InputFiles;
    std::string InputFilelist;
    std::string Region;

    // constructor
    PrintSettings()
        : HasInput(false)
        , HasInputFilelist(false)
        , HasRegion(false)

        , ByBamFile(false)
        , HasByBamFile(false)
        , ByStrand(false)
        , HasByStrand(false)
        , DotForMatch(true)
        , HasDotForMatch(false)
        , AllReads(false)
        , HasAllReads(false)
        , IncludeTotal(true)
        , HasIncludeTotal(false)
    {}
};

// ---------------------------------------------
// PrintToolPrivate implementation

struct PrintTool::PrintToolPrivate
{

    // ctor & dtor
public:
    PrintToolPrivate(PrintTool::PrintSettings* _settings);
    ~PrintToolPrivate() {}

    // 'public' interface
public:
    bool Run();

    // internal methods
private:
    void ProcessAlignment(const BamAlignment& al, const BamRegion& target_region, SeqCounter& seq_count, const std::string& infile);
    bool hasInvalidCigar(const BamAlignment & alignment);
    uint32_t getAlignmentEndPos(const BamAlignment & alignment);
    void parseMD(const std::string &MD_tag, std::vector<char> &MD_op, std::vector<int> &MD_len, std::vector<std::string> &MD_seq);
    bool build_target(std::string &target_seq, const BamRegion &target_region, int ref_pos, int advance, const std::string match);

    // data members
private:
    PrintTool::PrintSettings* m_settings;
};

PrintTool::PrintToolPrivate::PrintToolPrivate(PrintTool::PrintSettings* settings)
    : m_settings(settings)
{

}

// use current input alignment to update BAM file alignment 
void PrintTool::PrintToolPrivate::ProcessAlignment(const BamAlignment& alignment, const BamRegion& target_region, SeqCounter& seq_count, const std::string& infile)
{
    // Only proceed if the read is aligned to the targeted sequence and if we have an MD tag and Cigar is valid
    if(!alignment.IsMapped())
        return;
    if(!alignment.HasTag("MD"))
        return;
    if(hasInvalidCigar(alignment))
        return;

    // Only proceed if the alignment fully spans the targeted region
    int align_end_pos = getAlignmentEndPos(alignment);
    if((alignment.Position > target_region.LeftPosition) || (align_end_pos < target_region.RightPosition))
        return;

    // Get read bases, convert to uppercase, rev comp if necessary
    std::string read_bases = alignment.QueryBases;
    std::locale loc;
    for(unsigned int i=0; i < read_bases.length(); i++)
        read_bases[i] = toupper(read_bases[i],loc);
    bool reverse_strand = alignment.IsReverseStrand();
    std::string strand_str = (reverse_strand ? REVERSE_STRAND_STR : FORWARD_STRAND_STR);
    if(reverse_strand)
        Utilities::ReverseComplement(read_bases);

    // Parse MD tag to extract MD_op and MD_len
    std::string MD_tag;
    assert(alignment.HasTag("MD"));
    alignment.GetTag("MD",MD_tag);
    std::vector<char>        MD_op;  // Type of MD operation
    std::vector<int>         MD_len; // Number of bases for operation
    std::vector<std::string> MD_seq; // Bases for snp and deletion operations
    parseMD(MD_tag,MD_op,MD_len,MD_seq);

    // Create a local copy of CigarData because we will be modifying it and the alignment is const
    std::vector<CigarOp> CigarData = alignment.CigarData;

    // Synchronously scan through Cigar and MD to determine type & positions of errors
    // We always proceed from 5' to 3' in the original read, so for reverse strand alignments
    // we will be going backwards through cigar and MD
    int increment    = reverse_strand ? -1 : 1;
    int MD_idx       = reverse_strand ? MD_op.size()-1 : 0;
    int cigar_idx    = reverse_strand ? CigarData.size()-1 : 0;
    int read_idx     = 0;
    int ref_pos      = reverse_strand ? (align_end_pos-1) : alignment.Position;
    std::string target_seq = "";
    bool target_completed = false;
    while (cigar_idx < (int) CigarData.size() and cigar_idx >= 0) {

        // Advance cigar if required
        if (CigarData[cigar_idx].Length == 0 or CigarData[cigar_idx].Type == 'H') {
            cigar_idx += increment;
            continue;
        }

        // handle soft-clipping in cigar
        if (CigarData[cigar_idx].Type == 'S') {
            read_idx += CigarData[cigar_idx].Length;
            cigar_idx += increment;
            continue;
        }

        // Advance MD if required
        if (MD_idx < (int) MD_op.size() and MD_idx >= 0 and MD_len[MD_idx] == 0) {
            MD_idx += increment;
            continue;
        }

        if (CigarData[cigar_idx].Type == 'M' and MD_idx < (int) MD_op.size() and MD_idx >= 0 and MD_op[MD_idx] == 'M') {
            // Perfect match
            int advance = std::min((int)CigarData[cigar_idx].Length, MD_len[MD_idx]);
            std::string match = m_settings->DotForMatch ? "." : read_bases.substr(read_idx,advance);
            target_completed = build_target(target_seq,target_region,ref_pos,advance*increment,match);
            read_idx += advance;
            CigarData[cigar_idx].Length -= advance;
            MD_len[MD_idx] -= advance;
            ref_pos  += advance*increment;
        }
        else if (CigarData[cigar_idx].Type == 'I') {
            // Insertion (read has a base, reference doesn't)
            int advance = CigarData[cigar_idx].Length;
            if(    ( ( reverse_strand) && (ref_pos >= target_region.LeftPosition) && (ref_pos < (target_region.RightPosition-1)))
                || ( (!reverse_strand) && (ref_pos >  target_region.LeftPosition) && (ref_pos < (target_region.RightPosition  )))
            ) {
                std::locale loc;
                for(int i=0, j=read_idx; i < advance; i++, j++)
                    target_seq += tolower(read_bases[j],loc);
            }
            read_idx += advance;
            CigarData[cigar_idx].Length -= advance;
        }
        else if (CigarData[cigar_idx].Type == 'D' and MD_idx < (int) MD_op.size() and MD_idx >= 0 and MD_op[MD_idx] == 'D') {
            // Deletion (reference has a base, read doesn't)
            // If the two lengths below don't agree then something fishy is happening
            assert((int)CigarData[cigar_idx].Length == MD_len[MD_idx]);
            int advance = MD_len[MD_idx];
            CigarData[cigar_idx].Length = 0;
            MD_len[MD_idx] = 0;
            target_completed = build_target(target_seq,target_region,ref_pos,advance*increment,"-");
            ref_pos  += advance*increment;
        }
        else if (MD_idx < (int) MD_op.size() and MD_idx >= 0 and MD_op[MD_idx] == 'X') {
            // Substitution
            int advance = std::min((int)CigarData[cigar_idx].Length, MD_len[MD_idx]);
            for (int cnt = 0; cnt < advance; ++cnt) {
                target_completed = build_target(target_seq,target_region,ref_pos,increment,read_bases.substr(read_idx,1));
                read_idx++;
                ref_pos += increment;
            }
            CigarData[cigar_idx].Length -= advance;
            MD_len[MD_idx] -= advance;
        }
        else {
            std::cerr << "bamtools print WARNING: Unexpected CIGAR/MD combination for read " << alignment.Name << "(" << CigarData[cigar_idx].Type << ", ";
            if(MD_idx < (int) MD_op.size() and MD_idx >= 0)
                std::cerr << MD_op[MD_idx];
            else
                std::cerr << "NA";
            std::cerr << ")" << std::endl;
        }
        if(target_completed)
            break;
    }
    if(target_completed) {
        if(reverse_strand)
            Utilities::ReverseComplement(target_seq);
        if(m_settings->AllReads) {
            std::cout << infile << "\t" << alignment.Name << "\t" << strand_str << "\t" << target_seq << std::endl;
        }
        else {
            if(m_settings->ByStrand)
                seq_count.add(target_seq,reverse_strand);
            else
                seq_count.add(target_seq);
        }
    }
}

bool PrintTool::PrintToolPrivate::hasInvalidCigar(const BamAlignment & alignment)
{
    bool problem=false;
    if(alignment.IsMapped() && alignment.Length > 0) {
        unsigned int cigar_len=0;
        for(unsigned int i=0; i<alignment.CigarData.size(); ++i)
            if( (alignment.CigarData[i].Type != 'D') && (alignment.CigarData[i].Type != 'H') && (alignment.CigarData[i].Type != 'P') )
                cigar_len += alignment.CigarData[i].Length;
        if(cigar_len != (unsigned int) alignment.Length)
            problem=true;
    }
    return(problem);
}

uint32_t PrintTool::PrintToolPrivate::getAlignmentEndPos(const BamAlignment & alignment)
{
    int32_t end_pos = alignment.Position;
    for (uint32_t cigar_idx = 0; cigar_idx < alignment.CigarData.size(); cigar_idx++) {
        char cigar_op = alignment.CigarData[cigar_idx].Type;
        // advance along reference for appropriate cigar entries
        if (cigar_op == 'M' or cigar_op == 'D' or cigar_op == 'N' or cigar_op == '=' or cigar_op == 'X')
            end_pos += alignment.CigarData[cigar_idx].Length;
    }
    return(end_pos);
}

void PrintTool::PrintToolPrivate::parseMD(const std::string &MD_tag, std::vector<char> &MD_op, std::vector<int> &MD_len, std::vector<std::string> &MD_seq)
{
    MD_op.clear();
    MD_len.clear();
    MD_seq.clear();
    for (const char *MD_ptr = MD_tag.c_str(); *MD_ptr;) {
        int item_length = 0;
        std::string item_seq = "";
        if (*MD_ptr >= '0' and *MD_ptr <= '9') {    // Its a match
            MD_op.push_back('M');
            for (; *MD_ptr and *MD_ptr >= '0' and *MD_ptr <= '9'; ++MD_ptr)
                item_length = 10*item_length + *MD_ptr - '0';
        }
        else {
            if (*MD_ptr == '^') {
                // Its a deletion
                MD_ptr++;
                MD_op.push_back('D');
            }
            else {
                // Its a substitution 
                MD_op.push_back('X');
            }
            for (; *MD_ptr and *MD_ptr >= 'A' and *MD_ptr <= 'Z'; ++MD_ptr) {
                item_length++;
                item_seq += *MD_ptr;
            }
        }
        MD_len.push_back(item_length);
        MD_seq.push_back(item_seq);
    }
}

bool PrintTool::PrintToolPrivate::build_target(std::string &target_seq, const BamRegion &target_region, int ref_pos, int advance, const std::string match)
{
    if(    ( (advance > 0) && (ref_pos <  target_region.RightPosition) ) 
        || ( (advance < 0) && (ref_pos >= target_region.LeftPosition ) )
      )
    {
        // heading towards the other boundary, there may be something to add
        int extended_target_coverage = 0;
        int match_start_pos = 0;
        if(advance > 0) {
            extended_target_coverage = std::min(ref_pos + advance, target_region.RightPosition) - std::max(ref_pos, target_region.LeftPosition);
            match_start_pos = std::max(target_region.LeftPosition - ref_pos,0);
        } 
        else {
            extended_target_coverage = std::min(ref_pos+1, target_region.RightPosition) - std::max(ref_pos + 1 + advance, target_region.LeftPosition);
            match_start_pos = std::max(ref_pos-target_region.RightPosition+1,0);
        } 
        extended_target_coverage = std::max(extended_target_coverage,0);
        assert(extended_target_coverage <= abs(advance));
        if(extended_target_coverage > 0) {
            if(match.size()==1)
                target_seq += std::string(extended_target_coverage,match[0]);
            else if((unsigned int) extended_target_coverage <= match.size())
                target_seq += match.substr(match_start_pos,extended_target_coverage);
            else
                assert(true); // We should not end up here
        }
    }
    bool target_completed = (    ( (advance > 0) && (ref_pos + advance >= target_region.RightPosition) )
                              || ( (advance < 0) && (ref_pos + advance <  target_region.LeftPosition ) )
                            );
    return(target_completed);
}

bool PrintTool::PrintToolPrivate::Run()
{

    if(m_settings->HasByBamFile)
        m_settings->ByBamFile = stringToBool(m_settings->ByBamFileStr,"-byBamFile");
    if(m_settings->HasByStrand)
        m_settings->ByStrand = stringToBool(m_settings->ByStrandStr,"-byStrand");
    if(m_settings->HasDotForMatch)
        m_settings->DotForMatch = stringToBool(m_settings->DotForMatchStr,"-dotForMatch");
    if(m_settings->HasAllReads)
        m_settings->AllReads = stringToBool(m_settings->AllReadsStr,"-allReads");
    if(m_settings->HasIncludeTotal)
        m_settings->IncludeTotal = stringToBool(m_settings->IncludeTotalStr,"-includeTotal");

    // set to default input if none provided
    if (!m_settings->HasInput && !m_settings->HasInputFilelist)
        m_settings->InputFiles.push_back(Options::StandardIn());

    // add files in the filelist to the input file list
    if (m_settings->HasInputFilelist) {

        std::ifstream filelist(m_settings->InputFilelist.c_str(), std::ios::in);
        if (!filelist.is_open()) {
            std::cerr << "bamtools print ERROR: could not open input BAM file list... Aborting."
                      << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(filelist, line))
            m_settings->InputFiles.push_back(line);
    }

    // verify a valid region has been provided
    BamRegion region;
    if (!m_settings->HasRegion) {
        std::cerr << "bamtools print ERROR: no region specified... Aborting."
                  << std::endl;
        return false;
    } else {
        // Iterate over all bam files
        SeqCounter seq_count;
        for(std::vector<std::string>::iterator InputFile = m_settings->InputFiles.begin(); InputFile != m_settings->InputFiles.end(); InputFile++) {
            BamReader reader;
            if (!reader.Open(*InputFile)) {
                std::cerr << "bamtools print ERROR: could not open input BAM file " << *InputFile
                          << std::endl;
                reader.Close();
                return false;
            }

            unsigned long n_reads=0;
            // if region string parses OK
            if (Utilities::ParseRegionString(m_settings->Region, reader, region)) {

                // attempt to find index file
                reader.LocateIndex();

                // if index data available, we can use SetRegion
                if (reader.HasIndex()) {

                    // attempt to use SetRegion(), if failed report error
                    if (!reader.SetRegion(region.LeftRefID, region.LeftPosition, region.RightRefID,
                                          region.RightPosition)) {
                        std::cerr << "bamtools print ERROR: set region failed. Check that REGION "
                                     "describes a valid range"
                                  << std::endl;
                        reader.Close();
                        return false;
                    }

                    // everything checks out, iterate through all reads in specified region
                    SeqCounter this_seq_count;
                    BamAlignment al;
                    while (reader.GetNextAlignment(al)) {
                        ProcessAlignment(al,region,this_seq_count,*InputFile);
                        n_reads++;
                    }
                    seq_count += this_seq_count;
                    if(!m_settings->AllReads && m_settings->ByBamFile)
                        this_seq_count.print(*InputFile + "\t", m_settings->IncludeTotal, m_settings->ByStrand);
                }

                // no index data available, we have to iterate through until we
                // find overlapping alignments
                else {
                    SeqCounter this_seq_count;
                    BamAlignment al;
                    while (reader.GetNextAlignment(al)) {
                        if ((al.RefID >= region.LeftRefID) &&
                            ((al.Position + al.Length) >= region.LeftPosition) &&
                            (al.RefID <= region.RightRefID) && (al.Position <= region.RightPosition)) {
                            ProcessAlignment(al,region,this_seq_count,*InputFile);
                            n_reads++;
                        }
                    }
                    seq_count += this_seq_count;
                    if(!m_settings->AllReads && m_settings->ByBamFile)
                        this_seq_count.print(*InputFile + "\t", m_settings->IncludeTotal, m_settings->ByStrand);
                }
            }

            // error parsing REGION string
            else {
                std::cerr << "bamtools print ERROR: could not parse REGION: " << m_settings->Region
                          << std::endl;
                std::cerr << "Check that REGION is in valid format (see documentation) and that the "
                             "coordinates are valid"
                          << std::endl;
                reader.Close();
                return false;
            }
            reader.Close();
        }
        if(!m_settings->AllReads && !m_settings->ByBamFile)
            seq_count.print("", m_settings->IncludeTotal, m_settings->ByStrand);
    }

    return true;
}

// ---------------------------------------------
// PrintTool implementation

PrintTool::PrintTool()
    : AbstractTool()
    , m_settings(new PrintSettings)
    , m_impl(0)
{
    // set program details
    Options::SetProgramInfo(
        "bamtools print", "Text printing of alignments",
        "[-in <filename> -in <filename> ... | -list <filelist>] "
        "[-region <REGION>] [printingOptions]");

    // set up options
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in", "BAM filename", "the input BAM file", "", m_settings->HasInput,
                            m_settings->InputFiles, IO_Opts, Options::StandardIn());
    Options::AddValueOption("-list", "filename", "the input BAM file list, one line per file", "",
                            m_settings->HasInputFilelist, m_settings->InputFilelist, IO_Opts);
    Options::AddValueOption("-region", "REGION",
                            "genomic region. Index file is recommended for better performance, and "
                            "is used automatically if it exists. See \'bamtools help index\' for "
                            "more details on creating one.  Format is chr1:10..20 and coordinate "
                            "system is zero-based half-open",
                            "", m_settings->HasRegion, m_settings->Region, IO_Opts);

    const std::string boolArg = "true/false";
    OptionGroup* PrintingOpts = Options::CreateOptionGroup("Printing Options");
    Options::AddValueOption("-byBamFile", boolArg, "segregate results by input BAM",
                            "", m_settings->HasByBamFile, m_settings->ByBamFileStr,
                            PrintingOpts, FALSE_STR);
    Options::AddValueOption("-byStrand", boolArg, "segregate results by strand",
                            "", m_settings->HasByStrand, m_settings->ByStrandStr,
                            PrintingOpts, FALSE_STR);
    Options::AddValueOption("-dotForMatch", boolArg, "use \'.\' for matching bases",
                            "", m_settings->HasDotForMatch, m_settings->DotForMatchStr,
                            PrintingOpts, TRUE_STR);
    Options::AddValueOption("-allReads", boolArg, "print a line for eavery read - no aggregation",
                            "", m_settings->HasAllReads, m_settings->AllReadsStr,
                            PrintingOpts, FALSE_STR);
    Options::AddValueOption("-includeTotal", boolArg, "print a line indicating total number of reads",
                            "", m_settings->HasIncludeTotal, m_settings->IncludeTotalStr,
                            PrintingOpts, TRUE_STR);
}

PrintTool::~PrintTool()
{

    delete m_settings;
    m_settings = 0;

    delete m_impl;
    m_impl = 0;
}

int PrintTool::Help()
{
    Options::DisplayHelp();
    return 0;
}

int PrintTool::Run(int argc, char* argv[])
{

    // parse command line arguments
    Options::Parse(argc, argv, 1);

    // initialize PrintTool with settings
    m_impl = new PrintToolPrivate(m_settings);

    // run PrintTool, return success/fail
    if (m_impl->Run())
        return 0;
    else
        return 1;
}

bool stringToBool(const std::string& s, const std::string& optionName)
{
    if(iequals(s,"true"))
      return true;
    else if(iequals(s,"false"))
      return false;
    else
      std::cerr << "bamtools print ERROR: bad value for option " << optionName << " will set to FALSE for now" << std::endl;
    return false;
}


bool iequals(const std::string& a, const std::string& b)
{
    unsigned int sz = a.size();
    if (b.size() != sz)
        return false;
    for (unsigned int i = 0; i < sz; ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    return true;
}
