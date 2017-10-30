// ***************************************************************************
// bamtools_split.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 24 July 2013 (DB)
// ---------------------------------------------------------------------------
// Splits a BAM file on user-specified property, creating a new BAM output
// file for each value found
// ***************************************************************************

#include "bamtools_split.h"

#include <api/BamConstants.h>
#include <api/BamReader.h>
#include <api/BamWriter.h>
#include <utils/bamtools_options.h>
#include <utils/bamtools_variant.h>
using namespace BamTools;

#include <cstddef>
#include <ctime>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace BamTools {

// string constants
static const std::string SPLIT_MAPPED_TOKEN = ".MAPPED";
static const std::string SPLIT_UNMAPPED_TOKEN = ".UNMAPPED";
static const std::string SPLIT_PAIRED_TOKEN = ".PAIRED_END";
static const std::string SPLIT_SINGLE_TOKEN = ".SINGLE_END";
static const std::string SPLIT_REFERENCE_TOKEN = ".REF_";
static const std::string SPLIT_TAG_TOKEN = ".TAG_";

std::string GetTimestampString()
{

    // get human readable timestamp
    time_t currentTime;
    time(&currentTime);
    std::stringstream timeStream;
    timeStream << ctime(&currentTime);

    // convert whitespace to '_'
    std::string timeString = timeStream.str();
    std::size_t found = timeString.find(' ');
    while (found != std::string::npos) {
        timeString.replace(found, 1, 1, '_');
        found = timeString.find(' ', found + 1);
    }
    return timeString;
}

// remove copy of filename without extension
// (so /path/to/file.txt becomes /path/to/file )
std::string RemoveFilenameExtension(const std::string& filename)
{
    std::size_t found = filename.rfind('.');
    return filename.substr(0, found);
}

}  // namespace BamTools

// ---------------------------------------------
// SplitSettings implementation

struct SplitTool::SplitSettings
{

    // flags
    bool HasInputFilename;
    bool HasCustomOutputStub;
    bool HasCustomRefPrefix;
    bool HasCustomTagPrefix;
    bool HasListTagDelimiter;
    bool IsSplittingMapped;
    bool IsSplittingPaired;
    bool IsSplittingReference;
    bool IsSplittingTag;

    // string args
    std::string CustomOutputStub;
    std::string CustomRefPrefix;
    std::string CustomTagPrefix;
    std::string InputFilename;
    std::string TagToSplit;
    std::string ListTagDelimiter;

    // constructor
    SplitSettings()
        : HasInputFilename(false)
        , HasCustomOutputStub(false)
        , HasCustomRefPrefix(false)
        , HasCustomTagPrefix(false)
        , HasListTagDelimiter(false)
        , IsSplittingMapped(false)
        , IsSplittingPaired(false)
        , IsSplittingReference(false)
        , IsSplittingTag(false)
        , InputFilename(Options::StandardIn())
        , ListTagDelimiter("--")
    {}
};

// ---------------------------------------------
// SplitToolPrivate declaration

class SplitTool::SplitToolPrivate
{

    // ctor & dtor
public:
    SplitToolPrivate(SplitTool::SplitSettings* settings)
        : m_settings(settings)
    {}

    ~SplitToolPrivate()
    {
        m_reader.Close();
    }

    // 'public' interface
public:
    bool Run();

    // internal methods
private:
    // close & delete BamWriters in map
    template <typename T>
    void CloseWriters(std::map<T, BamWriter*>& writers);
    // calculate output stub based on IO args given
    void DetermineOutputFilenameStub();
    // open our BamReader
    bool OpenReader();
    // split alignments in BAM file based on isMapped property
    bool SplitMapped();
    // split alignments in BAM file based on isPaired property
    bool SplitPaired();
    // split alignments in BAM file based on refID property
    bool SplitReference();
    // finds first alignment and calls corresponding SplitTagImpl<>
    // depending on tag type
    bool SplitTag();

public:
    // handles list-type tags
    template <typename T>
    bool SplitListTagImpl(BamAlignment& al);

    // handles single-value tags
    template <typename T>
    bool SplitTagImpl(BamAlignment& al);

    // data members
private:
    SplitTool::SplitSettings* m_settings;
    std::string m_outputFilenameStub;
    BamReader m_reader;
    std::string m_header;
    RefVector m_references;
};

void SplitTool::SplitToolPrivate::DetermineOutputFilenameStub()
{

    // if user supplied output filename stub, use that
    if (m_settings->HasCustomOutputStub) m_outputFilenameStub = m_settings->CustomOutputStub;

    // else if user supplied input BAM filename, use that (minus ".bam" extension) as stub
    else if (m_settings->HasInputFilename)
        m_outputFilenameStub = RemoveFilenameExtension(m_settings->InputFilename);

    // otherwise, user did not specify -stub, and input is coming from STDIN
    // generate stub from timestamp
    else
        m_outputFilenameStub = GetTimestampString();
}

bool SplitTool::SplitToolPrivate::OpenReader()
{

    // attempt to open BAM file
    if (!m_reader.Open(m_settings->InputFilename)) {
        std::cerr << "bamtools split ERROR: could not open BAM file: " << m_settings->InputFilename
                  << std::endl;
        return false;
    }

    // save file 'metadata' & return success
    m_header = m_reader.GetHeaderText();
    m_references = m_reader.GetReferenceData();
    return true;
}

bool SplitTool::SplitToolPrivate::Run()
{

    // determine output stub
    DetermineOutputFilenameStub();

    // open up BamReader
    if (!OpenReader()) return false;

    // determine split type from settings
    if (m_settings->IsSplittingMapped) return SplitMapped();
    if (m_settings->IsSplittingPaired) return SplitPaired();
    if (m_settings->IsSplittingReference) return SplitReference();
    if (m_settings->IsSplittingTag) return SplitTag();

    // if we get here, no property was specified
    std::cerr
        << "bamtools split ERROR: no property given to split on... " << std::endl
        << "Please use -mapped, -paired, -reference, or -tag TAG to specify desired split behavior."
        << std::endl;
    return false;
}

bool SplitTool::SplitToolPrivate::SplitMapped()
{

    // set up splitting data structure
    std::map<bool, BamWriter*> outputFiles;
    std::map<bool, BamWriter*>::iterator writerIter;

    // iterate through alignments
    BamAlignment al;
    BamWriter* writer;
    bool isCurrentAlignmentMapped;
    while (m_reader.GetNextAlignment(al)) {

        // see if bool value exists
        isCurrentAlignmentMapped = al.IsMapped();
        writerIter = outputFiles.find(isCurrentAlignmentMapped);

        // if no writer associated with this value
        if (writerIter == outputFiles.end()) {

            // open new BamWriter
            const std::string outputFilename =
                m_outputFilenameStub +
                (isCurrentAlignmentMapped ? SPLIT_MAPPED_TOKEN : SPLIT_UNMAPPED_TOKEN) + ".bam";
            writer = new BamWriter;
            if (!writer->Open(outputFilename, m_header, m_references)) {
                std::cerr << "bamtools split ERROR: could not open " << outputFilename
                          << " for writing." << std::endl;
                return false;
            }

            // store in map
            outputFiles.insert(std::make_pair(isCurrentAlignmentMapped, writer));
        }

        // else grab corresponding writer
        else
            writer = (*writerIter).second;

        // store alignment in proper BAM output file
        if (writer) writer->SaveAlignment(al);
    }

    // clean up BamWriters
    CloseWriters(outputFiles);

    // return success
    return true;
}

bool SplitTool::SplitToolPrivate::SplitPaired()
{

    // set up splitting data structure
    std::map<bool, BamWriter*> outputFiles;
    std::map<bool, BamWriter*>::iterator writerIter;

    // iterate through alignments
    BamAlignment al;
    BamWriter* writer;
    bool isCurrentAlignmentPaired;
    while (m_reader.GetNextAlignment(al)) {

        // see if bool value exists
        isCurrentAlignmentPaired = al.IsPaired();
        writerIter = outputFiles.find(isCurrentAlignmentPaired);

        // if no writer associated with this value
        if (writerIter == outputFiles.end()) {

            // open new BamWriter
            const std::string outputFilename =
                m_outputFilenameStub +
                (isCurrentAlignmentPaired ? SPLIT_PAIRED_TOKEN : SPLIT_SINGLE_TOKEN) + ".bam";
            writer = new BamWriter;
            if (!writer->Open(outputFilename, m_header, m_references)) {
                std::cerr << "bamtool split ERROR: could not open " << outputFilename
                          << " for writing." << std::endl;
                return false;
            }

            // store in map
            outputFiles.insert(std::make_pair(isCurrentAlignmentPaired, writer));
        }

        // else grab corresponding writer
        else
            writer = (*writerIter).second;

        // store alignment in proper BAM output file
        if (writer) writer->SaveAlignment(al);
    }

    // clean up BamWriters
    CloseWriters(outputFiles);

    // return success
    return true;
}

bool SplitTool::SplitToolPrivate::SplitReference()
{

    // set up splitting data structure
    std::map<int32_t, BamWriter*> outputFiles;
    std::map<int32_t, BamWriter*>::iterator writerIter;

    // determine reference prefix
    std::string refPrefix = SPLIT_REFERENCE_TOKEN;
    if (m_settings->HasCustomRefPrefix) refPrefix = m_settings->CustomRefPrefix;

    // make sure prefix starts with '.'
    const std::size_t dotFound = refPrefix.find('.');
    if (dotFound != 0) refPrefix = std::string(1, '.') + refPrefix;

    // iterate through alignments
    BamAlignment al;
    BamWriter* writer;
    int32_t currentRefId;
    while (m_reader.GetNextAlignment(al)) {

        // see if bool value exists
        currentRefId = al.RefID;
        writerIter = outputFiles.find(currentRefId);

        // if no writer associated with this value
        if (writerIter == outputFiles.end()) {

            // fetch reference name for ID
            std::string refName;
            if (currentRefId == -1)
                refName = "unmapped";
            else
                refName = m_references.at(currentRefId).RefName;

            // construct new output filename
            const std::string outputFilename = m_outputFilenameStub + refPrefix + refName + ".bam";

            // open new BamWriter
            writer = new BamWriter;
            if (!writer->Open(outputFilename, m_header, m_references)) {
                std::cerr << "bamtools split ERROR: could not open " << outputFilename
                          << " for writing." << std::endl;
                return false;
            }

            // store in map
            outputFiles.insert(std::make_pair(currentRefId, writer));
        }

        // else grab corresponding writer
        else
            writer = (*writerIter).second;

        // store alignment in proper BAM output file
        if (writer) writer->SaveAlignment(al);
    }

    // clean up BamWriters
    CloseWriters(outputFiles);

    // return success
    return true;
}

// finds first alignment and calls corresponding SplitTagImpl<>() depending on tag type
bool SplitTool::SplitToolPrivate::SplitTag()
{

    // iterate through alignments, until we hit TAG
    BamAlignment al;
    while (m_reader.GetNextAlignment(al)) {

        // look for tag in this alignment and get tag type
        char tagType(0);
        if (!al.GetTagType(m_settings->TagToSplit, tagType)) continue;

        // request split method based on tag type
        // pass it the current alignment found
        switch (tagType) {

            case (Constants::BAM_TAG_TYPE_INT8):
                return SplitTagImpl<int8_t>(al);
            case (Constants::BAM_TAG_TYPE_INT16):
                return SplitTagImpl<int16_t>(al);
            case (Constants::BAM_TAG_TYPE_INT32):
                return SplitTagImpl<int32_t>(al);
            case (Constants::BAM_TAG_TYPE_UINT8):
                return SplitTagImpl<uint8_t>(al);
            case (Constants::BAM_TAG_TYPE_UINT16):
                return SplitTagImpl<uint16_t>(al);
            case (Constants::BAM_TAG_TYPE_UINT32):
                return SplitTagImpl<uint32_t>(al);
            case (Constants::BAM_TAG_TYPE_FLOAT):
                return SplitTagImpl<float>(al);

            case (Constants::BAM_TAG_TYPE_ASCII):
            case (Constants::BAM_TAG_TYPE_STRING):
            case (Constants::BAM_TAG_TYPE_HEX):
                return SplitTagImpl<std::string>(al);

            case (Constants::BAM_TAG_TYPE_ARRAY): {

                char arrayTagType(0);
                if (!al.GetArrayTagType(m_settings->TagToSplit, arrayTagType)) continue;
                switch (arrayTagType) {
                    case (Constants::BAM_TAG_TYPE_INT8):
                        return SplitListTagImpl<int8_t>(al);
                    case (Constants::BAM_TAG_TYPE_INT16):
                        return SplitListTagImpl<int16_t>(al);
                    case (Constants::BAM_TAG_TYPE_INT32):
                        return SplitListTagImpl<int32_t>(al);
                    case (Constants::BAM_TAG_TYPE_UINT8):
                        return SplitListTagImpl<uint8_t>(al);
                    case (Constants::BAM_TAG_TYPE_UINT16):
                        return SplitListTagImpl<uint16_t>(al);
                    case (Constants::BAM_TAG_TYPE_UINT32):
                        return SplitListTagImpl<uint32_t>(al);
                    case (Constants::BAM_TAG_TYPE_FLOAT):
                        return SplitListTagImpl<float>(al);
                    default:
                        std::cerr
                            << "bamtools split ERROR: array tag has unsupported element type: "
                            << arrayTagType << std::endl;
                        return false;
                }
            }

            default:
                std::cerr << "bamtools split ERROR: unknown tag type encountered: " << tagType
                          << std::endl;
                return false;
        }
    }

    // tag not found, but that's not an error - return success
    return true;
}

// --------------------------------------------------------------------------------
// template method implementation
// *Technical Note* - use of template methods declared & defined in ".cpp" file
//                    goes against normal practices, but works here because these
//                    are purely internal (no one can call from outside this file)

// close BamWriters & delete pointers
template <typename T>
void SplitTool::SplitToolPrivate::CloseWriters(std::map<T, BamWriter*>& writers)
{

    typedef std::map<T, BamWriter*> WriterMap;
    typedef typename WriterMap::iterator WriterMapIterator;

    // iterate over writers
    WriterMapIterator writerIter = writers.begin();
    WriterMapIterator writerEnd = writers.end();
    for (; writerIter != writerEnd; ++writerIter) {
        BamWriter* writer = (*writerIter).second;
        if (writer == 0) continue;

        // close BamWriter
        writer->Close();

        // destroy BamWriter
        delete writer;
        writer = 0;
    }

    // clear the container (destroying the items doesn't remove them)
    writers.clear();
}

// handle list-type tags
template <typename T>
bool SplitTool::SplitToolPrivate::SplitListTagImpl(BamAlignment& al)
{

    typedef std::vector<T> TagValueType;
    typedef std::map<std::string, BamWriter*> WriterMap;
    typedef typename WriterMap::iterator WriterMapIterator;

    // set up splitting data structure
    WriterMap outputFiles;
    WriterMapIterator writerIter;

    // determine tag prefix
    std::string tagPrefix = SPLIT_TAG_TOKEN;
    if (m_settings->HasCustomTagPrefix) tagPrefix = m_settings->CustomTagPrefix;

    // make sure prefix starts with '.'
    const std::size_t dotFound = tagPrefix.find('.');
    if (dotFound != 0) tagPrefix = std::string(1, '.') + tagPrefix;

    const std::string tag = m_settings->TagToSplit;
    BamWriter* writer;
    TagValueType currentValue;
    while (m_reader.GetNextAlignment(al)) {

        std::string listTagLabel;
        if (!al.GetTag(tag, currentValue))
            listTagLabel = "none";
        else {
            // make list label from tag data
            std::stringstream listTagLabelStream;
            typename TagValueType::const_iterator tagValueIter = currentValue.begin();
            typename TagValueType::const_iterator tagValueEnd = currentValue.end();
            for (; tagValueIter != tagValueEnd; ++tagValueIter)
                listTagLabelStream << (*tagValueIter) << m_settings->ListTagDelimiter;
            listTagLabel = listTagLabelStream.str();
            if (!listTagLabel.empty())
                listTagLabel = listTagLabel.substr(
                    0, listTagLabel.size() -
                           m_settings->ListTagDelimiter.size());  // pop last delimiter
        }

        // lookup writer for label
        writerIter = outputFiles.find(listTagLabel);

        // if not found, create one
        if (writerIter == outputFiles.end()) {

            // open new BamWriter, save first alignment
            std::stringstream outputFilenameStream;
            outputFilenameStream << m_outputFilenameStub << tagPrefix << tag << '_' << listTagLabel
                                 << ".bam";
            writer = new BamWriter;
            if (!writer->Open(outputFilenameStream.str(), m_header, m_references)) {
                std::cerr << "bamtools split ERROR: could not open " << outputFilenameStream.str()
                          << " for writing." << std::endl;
                return false;
            }

            // store in map
            outputFiles.insert(std::make_pair(listTagLabel, writer));
        }

        // else grab existing writer
        else
            writer = (*writerIter).second;

        // store alignment in proper BAM output file
        if (writer) writer->SaveAlignment(al);
    }

    // clean up & return success
    CloseWriters(outputFiles);
    return true;
}

// handle the single-value tags
template <typename T>
bool SplitTool::SplitToolPrivate::SplitTagImpl(BamAlignment& al)
{

    typedef T TagValueType;
    typedef std::map<TagValueType, BamWriter*> WriterMap;
    typedef typename WriterMap::iterator WriterMapIterator;

    // set up splitting data structure
    WriterMap outputFiles;
    WriterMapIterator writerIter;

    // determine tag prefix
    std::string tagPrefix = SPLIT_TAG_TOKEN;
    if (m_settings->HasCustomTagPrefix) tagPrefix = m_settings->CustomTagPrefix;

    // make sure prefix starts with '.'
    const std::size_t dotFound = tagPrefix.find('.');
    if (dotFound != 0) tagPrefix = std::string(1, '.') + tagPrefix;

    // local variables
    const std::string tag = m_settings->TagToSplit;
    BamWriter* writer;
    std::stringstream outputFilenameStream;
    TagValueType currentValue;

    // retrieve first alignment tag value
    if (al.GetTag(tag, currentValue)) {

        // open new BamWriter, save first alignment
        outputFilenameStream << m_outputFilenameStub << tagPrefix << tag << '_' << currentValue
                             << ".bam";
        writer = new BamWriter;
        if (!writer->Open(outputFilenameStream.str(), m_header, m_references)) {
            std::cerr << "bamtools split ERROR: could not open " << outputFilenameStream.str()
                      << " for writing." << std::endl;
            return false;
        }
        writer->SaveAlignment(al);

        // store in map
        outputFiles.insert(std::make_pair(currentValue, writer));

        // reset stream
        outputFilenameStream.str(std::string());
    }

    // iterate through remaining alignments
    while (m_reader.GetNextAlignment(al)) {

        // skip if this alignment doesn't have TAG
        if (!al.GetTag(tag, currentValue)) continue;

        // look up tag value in map
        writerIter = outputFiles.find(currentValue);

        // if no writer associated with this value
        if (writerIter == outputFiles.end()) {

            // open new BamWriter
            outputFilenameStream << m_outputFilenameStub << tagPrefix << tag << '_' << currentValue
                                 << ".bam";
            writer = new BamWriter;
            if (!writer->Open(outputFilenameStream.str(), m_header, m_references)) {
                std::cerr << "bamtool split ERROR: could not open " << outputFilenameStream.str()
                          << " for writing." << std::endl;
                return false;
            }

            // store in map
            outputFiles.insert(std::make_pair(currentValue, writer));

            // reset stream
            outputFilenameStream.str(std::string());
        }

        // else grab corresponding writer
        else
            writer = (*writerIter).second;

        // store alignment in proper BAM output file
        if (writer) writer->SaveAlignment(al);
    }

    // clean up BamWriters
    CloseWriters(outputFiles);

    // return success
    return true;
}

// ---------------------------------------------
// SplitTool implementation

SplitTool::SplitTool()
    : AbstractTool()
    , m_settings(new SplitSettings)
    , m_impl(0)
{
    // set program details
    const std::string name = "bamtools split";
    const std::string description =
        "splits a BAM file on user-specified property, creating a new BAM output file for each "
        "value found";
    const std::string args =
        "[-in <filename>] [-stub <filename stub>] < -mapped | -paired | -reference [-refPrefix "
        "<prefix>] | -tag <TAG> > ";
    Options::SetProgramInfo(name, description, args);

    // set up options
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in", "BAM filename", "the input BAM file", "",
                            m_settings->HasInputFilename, m_settings->InputFilename, IO_Opts,
                            Options::StandardIn());
    Options::AddValueOption(
        "-refPrefix", "string",
        "custom prefix for splitting by references. Currently files end with REF_<refName>.bam. "
        "This option allows you to replace \"REF_\" with a prefix of your choosing.",
        "", m_settings->HasCustomRefPrefix, m_settings->CustomRefPrefix, IO_Opts);
    Options::AddValueOption(
        "-tagPrefix", "string",
        "custom prefix for splitting by tags. Current files end with TAG_<tagname>_<tagvalue>.bam. "
        "This option allows you to replace \"TAG_\" with a prefix of your choosing.",
        "", m_settings->HasCustomTagPrefix, m_settings->CustomTagPrefix, IO_Opts);
    Options::AddValueOption("-stub", "filename stub",
                            "prefix stub for output BAM files (default behavior is to use input "
                            "filename, without .bam extension, as stub). If input is stdin and no "
                            "stub provided, a timestamp is generated as the stub.",
                            "", m_settings->HasCustomOutputStub, m_settings->CustomOutputStub,
                            IO_Opts);
    Options::AddValueOption("-tagListDelim", "string",
                            "delimiter used to separate values in the filenames generated from "
                            "splitting on list-type tags [--]",
                            "", m_settings->HasListTagDelimiter, m_settings->ListTagDelimiter,
                            IO_Opts);

    OptionGroup* SplitOpts = Options::CreateOptionGroup("Split Options");
    Options::AddOption("-mapped", "split mapped/unmapped alignments", m_settings->IsSplittingMapped,
                       SplitOpts);
    Options::AddOption("-paired", "split single-end/paired-end alignments",
                       m_settings->IsSplittingPaired, SplitOpts);
    Options::AddOption("-reference", "split alignments by reference",
                       m_settings->IsSplittingReference, SplitOpts);
    Options::AddValueOption("-tag", "tag name",
                            "splits alignments based on all values of TAG encountered (i.e. -tag "
                            "RG creates a BAM file for each read group in original BAM file)",
                            "", m_settings->IsSplittingTag, m_settings->TagToSplit, SplitOpts);
}

SplitTool::~SplitTool()
{

    delete m_settings;
    m_settings = 0;

    delete m_impl;
    m_impl = 0;
}

int SplitTool::Help()
{
    Options::DisplayHelp();
    return 0;
}

int SplitTool::Run(int argc, char* argv[])
{

    // parse command line arguments
    Options::Parse(argc, argv, 1);

    // initialize SplitTool with settings
    m_impl = new SplitToolPrivate(m_settings);

    // run SplitTool, return success/fail
    if (m_impl->Run())
        return 0;
    else
        return 1;
}
