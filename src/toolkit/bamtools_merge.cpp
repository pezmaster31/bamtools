// ***************************************************************************
// bamtools_merge.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 December 2012
// ---------------------------------------------------------------------------
// Merges multiple BAM files into one
// ***************************************************************************

#include "bamtools_merge.h"

#include <api/BamMultiReader.h>
#include <api/BamWriter.h>
#include <utils/bamtools_options.h>
#include <utils/bamtools_utilities.h>
using namespace BamTools;

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// ---------------------------------------------
// MergeSettings implementation

struct MergeTool::MergeSettings {

    // flags
    bool HasInput;
    bool HasInputFilelist;
    bool HasOutput;
    bool IsForceCompression;
    bool HasRegion;

    // filenames
    std::vector<std::string> InputFiles;
    std::string InputFilelist;

    // other parameters
    std::string OutputFilename;
    std::string Region;

    // constructor
    MergeSettings()
        : HasInput(false)
        , HasInputFilelist(false)
        , HasOutput(false)
        , IsForceCompression(false)
        , HasRegion(false)
        , OutputFilename(Options::StandardOut())
    { }
};

// ---------------------------------------------
// MergeToolPrivate implementation

struct MergeTool::MergeToolPrivate {

    // ctor & dtor
    public:
        MergeToolPrivate(MergeTool::MergeSettings* settings)
            : m_settings(settings)
        { }

        ~MergeToolPrivate() { }

    // interface
    public:
        bool Run();

    // data members
    private:
        MergeTool::MergeSettings* m_settings;
};

bool MergeTool::MergeToolPrivate::Run() {

    // set to default input if none provided
    if ( !m_settings->HasInput && !m_settings->HasInputFilelist )
        m_settings->InputFiles.push_back(Options::StandardIn());

    // add files in the filelist to the input file list
    if ( m_settings->HasInputFilelist ) {

        std::ifstream filelist(m_settings->InputFilelist.c_str(), std::ios::in);
        if ( !filelist.is_open() ) {
            std::cerr << "bamtools merge ERROR: could not open input BAM file list... Aborting." << std::endl;
            return false;
        }

        std::string line;
        while ( std::getline(filelist, line) )
            m_settings->InputFiles.push_back(line);
    }

    // opens the BAM files (by default without checking for indexes)
    BamMultiReader reader;
    if ( !reader.Open(m_settings->InputFiles) ) {
        std::cerr << "bamtools merge ERROR: could not open input BAM file(s)... Aborting." << std::endl;
        return false;
    }

    // retrieve header & reference dictionary info
    std::string mergedHeader = reader.GetHeaderText();
    RefVector references = reader.GetReferenceData();

    // determine compression mode for BamWriter
    bool writeUncompressed = ( m_settings->OutputFilename == Options::StandardOut() &&
                               !m_settings->IsForceCompression );
    BamWriter::CompressionMode compressionMode = BamWriter::Compressed;
    if ( writeUncompressed ) compressionMode = BamWriter::Uncompressed;

    // open BamWriter
    BamWriter writer;
    writer.SetCompressionMode(compressionMode);
    if ( !writer.Open(m_settings->OutputFilename, mergedHeader, references) ) {
        std::cerr << "bamtools merge ERROR: could not open "
             << m_settings->OutputFilename << " for writing." << std::endl;
        reader.Close();
        return false;
    }

    // if no region specified, store entire contents of file(s)
    if ( !m_settings->HasRegion ) {
        BamAlignment al;
        while ( reader.GetNextAlignmentCore(al) )
            writer.SaveAlignment(al);
    }

    // otherwise attempt to use region as constraint
    else {

        // if region string parses OK
        BamRegion region;
        if ( Utilities::ParseRegionString(m_settings->Region, reader, region) ) {

            // attempt to find index files
            reader.LocateIndexes();

            // if index data available for all BAM files, we can use SetRegion
            if ( reader.HasIndexes() ) {

                // attempt to use SetRegion(), if failed report error
                if ( !reader.SetRegion(region.LeftRefID,
                                       region.LeftPosition,
                                       region.RightRefID,
                                       region.RightPosition) )
                {
                    std::cerr << "bamtools merge ERROR: set region failed. Check that REGION describes a valid range"
                         << std::endl;
                    reader.Close();
                    return false;
                }

                // everything checks out, just iterate through specified region, storing alignments
                BamAlignment al;
                while ( reader.GetNextAlignmentCore(al) )
                    writer.SaveAlignment(al);
            }

            // no index data available, we have to iterate through until we
            // find overlapping alignments
            else {
                BamAlignment al;
                while ( reader.GetNextAlignmentCore(al) ) {
                    if ( (al.RefID >= region.LeftRefID)  && ( (al.Position + al.Length) >= region.LeftPosition ) &&
                         (al.RefID <= region.RightRefID) && ( al.Position <= region.RightPosition) )
                    {
                        writer.SaveAlignment(al);
                    }
                }
            }
        }

        // error parsing REGION string
        else {
            std::cerr << "bamtools merge ERROR: could not parse REGION - " << m_settings->Region << std::endl;
            std::cerr << "Check that REGION is in valid format (see documentation) and that the coordinates are valid"
                 << std::endl;
            reader.Close();
            writer.Close();
            return false;
        }
    }

    // clean & exit
    reader.Close();
    writer.Close();
    return true;
}

// ---------------------------------------------
// MergeTool implementation

MergeTool::MergeTool()
    : AbstractTool()
    , m_settings(new MergeSettings)
    , m_impl(0)
{
    // set program details
    Options::SetProgramInfo("bamtools merge", "merges multiple BAM files into one",
                            "[-in <filename> -in <filename> ... | -list <filelist>] [-out <filename> | [-forceCompression]] [-region <REGION>]");

    // set up options
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in",  "BAM filename", "the input BAM file(s)", "", m_settings->HasInput,  m_settings->InputFiles,     IO_Opts);
    Options::AddValueOption("-list",  "filename", "the input BAM file list, one line per file", "", m_settings->HasInputFilelist,  m_settings->InputFilelist, IO_Opts);
    Options::AddValueOption("-out", "BAM filename", "the output BAM file",   "", m_settings->HasOutput, m_settings->OutputFilename, IO_Opts);
    Options::AddOption("-forceCompression", "if results are sent to stdout (like when piping to another tool), default behavior is to leave output uncompressed. Use this flag to override and force compression", m_settings->IsForceCompression, IO_Opts);
    Options::AddValueOption("-region", "REGION", "genomic region. See README for more details", "", m_settings->HasRegion, m_settings->Region, IO_Opts);
}

MergeTool::~MergeTool() {

    delete m_settings;
    m_settings = 0;

    delete m_impl;
    m_impl = 0;
}

int MergeTool::Help() {
    Options::DisplayHelp();
    return 0;
}

int MergeTool::Run(int argc, char* argv[]) {

    // parse command line arguments
    Options::Parse(argc, argv, 1);

    // initialize MergeTool with settings
    m_impl = new MergeToolPrivate(m_settings);

    // run MergeTool, return success/fail
    if ( m_impl->Run() )
        return 0;
    else
        return 1;
}
