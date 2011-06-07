// ***************************************************************************
// bamtools_resolve.cpp (c) 2011
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 7 June 2011
// ---------------------------------------------------------------------------
// Resolves paired-end reads (marking the IsProperPair flag as needed) in a
// BAM file.
// ***************************************************************************

#include "bamtools_resolve.h"

#include <api/BamReader.h>
#include <api/BamWriter.h>
#include <api/SamConstants.h>
#include <api/SamHeader.h>
#include <utils/bamtools_options.h>
#include <utils/bamtools_utilities.h>
using namespace BamTools;

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
using namespace std;

enum { debug = 1 };

// constants
static const int NUM_MODELS = 8;
static const string READ_GROUP_TAG = "RG";
static const double DEFAULT_CONFIDENCE_INTERVAL = 0.9973;
static const double DEFAULT_MODEL_COUNT_THRESHOLD = 0.1;

// -----------------------------------------------
// ModelType implementation

struct ModelType {

    // data members
    unsigned char ID;
    vector<uint32_t> FragmentLengths;

    // ctor
    ModelType(const unsigned char id)
        : ID(id)
    {
        // preallocate space for 10K fragments per model type
        FragmentLengths.reserve(10000);
    }

    // convenience access to internal fragment lengths vector
    vector<uint32_t>::iterator begin(void) { return FragmentLengths.begin(); }
    vector<uint32_t>::const_iterator begin(void) const { return FragmentLengths.begin(); }
    void clear(void) { FragmentLengths.clear(); }
    vector<uint32_t>::iterator end(void) { return FragmentLengths.end(); }
    vector<uint32_t>::const_iterator end(void) const { return FragmentLengths.end(); }
    void push_back(const uint32_t& x) { FragmentLengths.push_back(x); }
    size_t size(void) const { return FragmentLengths.size(); }

    // constants
    static const unsigned char DUMMY_ID;
};

const unsigned char ModelType::DUMMY_ID = 100;

bool operator>(const ModelType& lhs, const ModelType& rhs) {
    return lhs.size() > rhs.size();
}

unsigned char CalculateModelType(const BamAlignment& al) {

    // localize alignment's mate positions & orientations for convenience
    const int32_t m1_begin = ( al.IsFirstMate() ? al.Position : al.MatePosition );
    const int32_t m2_begin = ( al.IsFirstMate() ? al.MatePosition : al.Position );
    const bool m1_isReverseStrand = ( al.IsFirstMate() ? al.IsReverseStrand() : al.IsMateReverseStrand() );
    const bool m2_isReverseStrand = ( al.IsFirstMate() ? al.IsMateReverseStrand() : al.IsReverseStrand() );

    // determine 'model type'
    if ( m1_begin < m2_begin ) {
        if ( !m1_isReverseStrand && !m2_isReverseStrand ) return 0;
        if ( !m1_isReverseStrand &&  m2_isReverseStrand ) return 1;
        if (  m1_isReverseStrand && !m2_isReverseStrand ) return 2;
        if (  m1_isReverseStrand &&  m2_isReverseStrand ) return 3;
    } else {
        if ( !m2_isReverseStrand && !m1_isReverseStrand ) return 4;
        if ( !m2_isReverseStrand &&  m1_isReverseStrand ) return 5;
        if (  m2_isReverseStrand && !m1_isReverseStrand ) return 6;
        if (  m2_isReverseStrand &&  m1_isReverseStrand ) return 7;
    }

    // unknown model
    return ModelType::DUMMY_ID;
}

// ---------------------------------------------
// ReadGroupResolver implementation

struct ReadGroupResolver {

    // data members
    int32_t MinFragmentLength;
    int32_t MedianFragmentLength;
    int32_t MaxFragmentLength;
    vector<ModelType> Models;

    static double ConfidenceInterval;

    // ctor
    ReadGroupResolver(void);

    // resolving methods
    bool IsValidInsertSize(const BamAlignment& al) const;
    bool IsValidOrientation(const BamAlignment& al) const;

    // select 2 best models based on observed data
    void DetermineTopModels(void);

    static void SetConfidenceInterval(const double& ci);
};

double ReadGroupResolver::ConfidenceInterval = DEFAULT_CONFIDENCE_INTERVAL;

ReadGroupResolver::ReadGroupResolver(void)
    : MinFragmentLength(0)
    , MedianFragmentLength(0)
    , MaxFragmentLength(0)
{
    // pre-allocate space for 8 models
    Models.reserve(NUM_MODELS);
    for ( int i = 0; i < NUM_MODELS; ++i )
        Models.push_back( ModelType((unsigned char)(i+1)) );
}

bool ReadGroupResolver::IsValidInsertSize(const BamAlignment& al) const {
    return ( al.InsertSize >= MinFragmentLength &&
             al.InsertSize <= MaxFragmentLength );
}

bool ReadGroupResolver::IsValidOrientation(const BamAlignment& al) const {
    const unsigned char currentModel = CalculateModelType(al);
    return ( currentModel == Models[0].ID || currentModel == Models[1].ID );
}

void ReadGroupResolver::DetermineTopModels(void) {

    // sort models (most common -> least common)
    sort( Models.begin(), Models.end(), std::greater<ModelType>() );

    // make sure that the 2 most common models are some threshold more common
    // than the remaining models
    const unsigned int activeModelCountSum = Models[0].size() + Models[1].size();
    const unsigned int unusedModelCountSum = Models[2].size() + Models[3].size() +
                                             Models[4].size() + Models[5].size() +
                                             Models[6].size() + Models[7].size();
    if ( activeModelCountSum == 0 ) return;
    const double unusedPercentage = (double)unusedModelCountSum / (double)activeModelCountSum;
    if ( unusedPercentage > DEFAULT_MODEL_COUNT_THRESHOLD ) {
        cerr << "ERROR: When determining whether to apply mate-pair or paired-end constraints, "
             << "an irregularity in the alignment model counts was discovered." << endl
             << endl;
        cerr << "       Normal mate-pair data sets have the highest counts for alignment models:  4 & 5." << endl;
        cerr << "       Normal paired-end data sets have the highest counts for alignment models: 2 & 6." << endl;
        cerr << "       Normal solid-end data sets have the highest counts for alignment models:  1 & 8." << endl
             << endl;
        cerr << "       We expect that the ratio of the 6 lowest counts to the 2 highest counts to be no larger than "
             << DEFAULT_MODEL_COUNT_THRESHOLD << ", but in this data set the ratio was " << unusedPercentage << endl
             << endl;
        for ( unsigned char i = 0; i < NUM_MODELS; ++i )
            cerr << "- alignment model " << Models[i].ID << " : " << Models[i].size() << " hits" << endl;
        exit(1);
    }

    // emit a warning if the best alignment models are non-standard
    const bool isModel1Top = (Models[0].ID == 1) || (Models[1].ID == 1);
    const bool isModel2Top = (Models[0].ID == 2) || (Models[1].ID == 2);
    const bool isModel4Top = (Models[0].ID == 4) || (Models[1].ID == 4);
    const bool isModel5Top = (Models[0].ID == 5) || (Models[1].ID == 5);
    const bool isModel6Top = (Models[0].ID == 6) || (Models[1].ID == 6);
    const bool isModel8Top = (Models[0].ID == 8) || (Models[1].ID == 8);

    bool isMatePair  = ( isModel4Top && isModel5Top ? true : false );
    bool isPairedEnd = ( isModel2Top && isModel6Top ? true : false );
    bool isSolidPair = ( isModel1Top && isModel8Top ? true : false );

    if ( debug ) {
        if      ( isMatePair  ) cerr << "- resolving mate-pair alignments" << endl;
        else if ( isPairedEnd ) cerr << "- resolving paired-end alignments" << endl;
        else if ( isSolidPair ) cerr << "- resolving solid-pair alignments" << endl;
        else {
            cerr << "- WARNING: Found a non-standard alignment model configuration. "
                 << "Using alignment models " << Models[0].ID << " & " << Models[1].ID << endl;
        }
    }

    // store only the fragments from the best alignment models, then sort
    vector<uint32_t> fragments;
    fragments.reserve( Models[0].size() + Models[1].size() );
    fragments.insert( fragments.end(), Models[0].begin(), Models[0].end() );
    fragments.insert( fragments.end(), Models[1].begin(), Models[1].end() );
    sort ( fragments.begin(), fragments.end() );

    // clear out Model fragment data, not needed anymore
    // keep sorted though, with IDs, we'll be coming back to that
    for ( int i = 0; i < NUM_MODELS; ++i )
        Models[i].clear();

    // determine min,median, & max fragment lengths for each read group
    const double halfNonConfidenceInterval = (1.0 - ReadGroupResolver::ConfidenceInterval)/2.0;
    const unsigned int numFragmentLengths = fragments.size();
    if ( numFragmentLengths == 0 ) return;

    const unsigned int minIndex = (unsigned int)(numFragmentLengths * halfNonConfidenceInterval);
    MinFragmentLength = fragments[minIndex];

    const unsigned int medianIndex = (unsigned int)(numFragmentLengths * 0.5);
    MedianFragmentLength = fragments[medianIndex];

    const unsigned int maxIndex = (unsigned int)(numFragmentLengths * (1.0-halfNonConfidenceInterval));
    MaxFragmentLength = fragments[maxIndex];
}

void ReadGroupResolver::SetConfidenceInterval(const double& ci) {
    ConfidenceInterval = ci;
}

// ---------------------------------------------
// ResolveSettings implementation

struct ResolveTool::ResolveSettings {

    // flags
    bool HasInputFile;
    bool HasOutputFile;
    bool HasStatsFile;
    bool HasConfidenceInterval;
    bool IsForceCompression;

    // filenames
    string InputFilename;
    string OutputFilename;
    string StatsFilename;

    // 'resolve options'
    double ConfidenceInterval;

    // constructor
    ResolveSettings(void)
        : HasInputFile(false)
        , HasOutputFile(false)
        , HasStatsFile(false)
        , IsForceCompression(false)
        , InputFilename(Options::StandardIn())
        , OutputFilename(Options::StandardOut())
        , StatsFilename("")
        , ConfidenceInterval(DEFAULT_CONFIDENCE_INTERVAL)
    { }
};

// ---------------------------------------------
// ResolveToolPrivate implementation

struct ResolveTool::ResolveToolPrivate {

    // ctor & dtor
    public:
        ResolveToolPrivate(ResolveTool::ResolveSettings* settings)
            : m_settings(settings)
        { }
        ~ResolveToolPrivate(void) { }

    // 'public' interface
    public:
        bool Run(void);

    // internal methods
    private:
        bool CalculateStats(BamReader& reader);
        void ParseHeader(const SamHeader& header);
        bool ParseStatsFile(void);
        void ResolveAlignment(BamAlignment& al);

    // data members
    private:
        ResolveTool::ResolveSettings* m_settings;
        map<string, ReadGroupResolver> m_readGroups;
};

bool ResolveTool::ResolveToolPrivate::CalculateStats(BamReader& reader) {

    // ensure that we get a fresh BamReader
    reader.Rewind();

    // read through BAM file
    BamAlignment al;
    string readGroup("");
    map<string, ReadGroupResolver>::iterator rgIter;
    while ( reader.GetNextAlignmentCore(al) ) {

        // skip if alignment is not paired, mapped, nor mate is mapped
        if ( !al.IsPaired() || !al.IsMapped() || !al.IsMateMapped() )
            continue;

        // determine model type, skip if model unknown
        const unsigned char currentModelType = CalculateModelType(al);
        assert( currentModelType != ModelType::DUMMY_ID );

        // flesh out the char data, so we can retrieve its read group ID
        al.BuildCharData();

        // get read group from alignment
        readGroup.clear();
        al.GetTag(READ_GROUP_TAG, readGroup);

        // look up resolver for read group
        rgIter = m_readGroups.find(readGroup);
        if ( rgIter == m_readGroups.end() )  {
            cerr << "bamtools resolve ERROR - unable to calculate stats, unknown read group encountered: "
                 << readGroup << endl;
            return false;
        }
        ReadGroupResolver& resolver = (*rgIter).second;

        // update stats for current read group, current model type
        resolver.Models[currentModelType].push_back(al.InsertSize);
    }

    // iterate back through read groups
    map<string, ReadGroupResolver>::iterator rgEnd  = m_readGroups.end();
    for ( rgIter = m_readGroups.begin(); rgIter != rgEnd; ++rgIter ) {
        ReadGroupResolver& resolver = (*rgIter).second;

        // calculate acceptable orientation & insert sizes for this read group
        resolver.DetermineTopModels();

        if ( debug ) {
            cerr << "----------------------------------------" << endl
                 << "ReadGroup: " << (*rgIter).first << endl
                 << "----------------------------------------" << endl
                 << "Min FL: " << resolver.MinFragmentLength << endl
                 << "Med FL: " << resolver.MedianFragmentLength << endl
                 << "Max FL: " << resolver.MaxFragmentLength << endl
                 << endl;
        }
    }

    // return reader to beginning & return success
    reader.Rewind();
    return true;
}

void ResolveTool::ResolveToolPrivate::ParseHeader(const SamHeader& header) {

    // iterate over header read groups, creating a 'resolver' for each
    SamReadGroupConstIterator rgIter = header.ReadGroups.ConstBegin();
    SamReadGroupConstIterator rgEnd  = header.ReadGroups.ConstEnd();
    for ( ; rgIter != rgEnd; ++rgIter ) {
        const SamReadGroup& rg = (*rgIter);
        m_readGroups.insert( make_pair<string, ReadGroupResolver>(rg.ID, ReadGroupResolver()) );
    }
}

bool ResolveTool::ResolveToolPrivate::ParseStatsFile(void) {
    cerr << "ResolveTool::ParseStatsFile() ERROR - not yet implemented!" << endl;
    return false;
}

void ResolveTool::ResolveToolPrivate::ResolveAlignment(BamAlignment& al) {

    // clear proper-pair flag
    al.SetIsProperPair(false);

    // quit check if alignment is not from paired-end read
    if ( !al.IsPaired() ) return;

    // quit check if either alignment or mate are unmapped
    if ( !al.IsMapped() || !al.IsMateMapped() ) return;

    // quit check if map quality is 0
    if ( al.MapQuality == 0 ) return;

    // get read group from alignment
    // empty string if not found, this is OK - we handle a default empty read group case
    string readGroup("");
    al.GetTag(READ_GROUP_TAG, readGroup);

    // look up read group's 'resolver'
    map<string, ReadGroupResolver>::iterator rgIter = m_readGroups.find(readGroup);
    if ( rgIter == m_readGroups.end() ) {
        cerr << "bamtools resolve ERROR - read group found that was not in header: "
             << readGroup << endl;
        exit(1);
    }
    const ReadGroupResolver& resolver = (*rgIter).second;

    // quit check if pairs are not in proper orientation (tech-dependent, can differ for each RG)
    if ( !resolver.IsValidOrientation(al) ) return;

    // quit check if pairs are not within "reasonable" distance (differs for each RG)
    if ( !resolver.IsValidInsertSize(al) ) return;

    // if we get here, alignment is OK - set 'proper pair' flag
    al.SetIsProperPair(true);
}

bool ResolveTool::ResolveToolPrivate::Run(void) {

    ReadGroupResolver::SetConfidenceInterval(m_settings->ConfidenceInterval);

    // initialize read group map with default (empty name) read group
    m_readGroups.insert( make_pair<string, ReadGroupResolver>("", ReadGroupResolver()) );

    // open our BAM reader
    BamReader reader;
    if ( !reader.Open(m_settings->InputFilename) ) {
        cerr << "bamtools resolve ERROR: could not open input BAM file: "
             << m_settings->InputFilename << endl;
        return false;
    }

    // retrieve header & reference dictionary info
    const SamHeader& header = reader.GetHeader();
    const RefVector& references = reader.GetReferenceData();

    // parse BAM header for read groups
    ParseHeader(header);

    // if existing stats file provided, parse for fragment lengths
    if ( m_settings->HasStatsFile ) {
        if ( !ParseStatsFile() ) {
            cerr << "bamtools resolve ERROR - could not parse stats file" << endl;
            reader.Close();
            return false;
        }
    }
    // otherwise calculate stats from BAM alignments
    else {
        if ( !CalculateStats(reader) ) {
            cerr << "bamtools resolve ERROR - could not calculate stats from BAM file" << endl;
            reader.Close();
            return false;
        }
    }

    // determine compression mode for BamWriter
    bool writeUncompressed = ( m_settings->OutputFilename == Options::StandardOut() &&
                               !m_settings->IsForceCompression );
    BamWriter::CompressionMode compressionMode = BamWriter::Compressed;
    if ( writeUncompressed ) compressionMode = BamWriter::Uncompressed;

    // open BamWriter
    BamWriter writer;
    writer.SetCompressionMode(compressionMode);
    if ( !writer.Open(m_settings->OutputFilename, header, references) ) {
        cerr << "bamtools resolve ERROR: could not open "
             << m_settings->OutputFilename << " for writing." << endl;
        reader.Close();
        return false;
    }

    // plow through alignments, setting/clearing 'proper pair' flag
    // and writing to new output BAM file
    BamAlignment al;
    while ( reader.GetNextAlignment(al) ) {
        ResolveAlignment(al);
        writer.SaveAlignment(al);
    }

    // clean up & return success
    reader.Close();
    writer.Close();
    return true;
}

// ---------------------------------------------
// ResolveTool implementation

ResolveTool::ResolveTool(void)
    : AbstractTool()
    , m_settings(new ResolveSettings)
    , m_impl(0)
{
    // set program details
    Options::SetProgramInfo("bamtools resolve", "resolves paired-end reads (marking the IsProperPair flag as needed)", "[-in <filename>] [-out <filename> | [-forceCompression] ] ");

    // set up options
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in",  "BAM filename", "the input BAM file",  "", m_settings->HasInputFile,  m_settings->InputFilename,  IO_Opts, Options::StandardIn());
    Options::AddValueOption("-out", "BAM filename", "the output BAM file", "", m_settings->HasOutputFile, m_settings->OutputFilename, IO_Opts, Options::StandardOut());
//    Options::AddValueOption("-stats", "STATS filename", "alignment stats file", "", m_settings->HasStatsFile, m_settings->StatsFilename, IO_Opts, "");
    Options::AddOption("-forceCompression", "if results are sent to stdout (like when piping to another tool), default behavior is to leave output uncompressed. Use this flag to override and force compression",
                       m_settings->IsForceCompression, IO_Opts);

    OptionGroup* ResolveOpts = Options::CreateOptionGroup("Resolve Settings");
    Options::AddValueOption("-ci", "double", "confidence interval",
                            "", m_settings->HasConfidenceInterval, m_settings->ConfidenceInterval, ResolveOpts);
}

ResolveTool::~ResolveTool(void) {

    delete m_settings;
    m_settings = 0;

    delete m_impl;
    m_impl = 0;
}

int ResolveTool::Help(void) {
    Options::DisplayHelp();
    return 0;
}

int ResolveTool::Run(int argc, char* argv[]) {

    // parse command line arguments
    Options::Parse(argc, argv, 1);

    // initialize ResolveTool
    m_impl = new ResolveToolPrivate(m_settings);

    // run ResolveTool, return success/failure
    if ( m_impl->Run() )
        return 0;
    else
        return 1;
}
