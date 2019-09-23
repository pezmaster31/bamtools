// ***************************************************************************
// bamtools_count.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 December 2012
// ---------------------------------------------------------------------------
// Prints alignment count for BAM file(s)
// ***************************************************************************

#include "bamtools_count.h"

#include <api/BamAlgorithms.h>
#include <api/BamMultiReader.h>
#include <utils/bamtools_options.h>
#include <utils/bamtools_utilities.h>
using namespace BamTools;

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// ---------------------------------------------
// CountSettings implementation

struct CountTool::CountSettings
{

    // flags
    bool HasInput;
    bool HasInputFilelist;
    bool HasRegion;

    // filenames
    std::vector<std::string> InputFiles;
    std::string InputFilelist;
    std::string Region;

    // constructor
    CountSettings()
        : HasInput(false)
        , HasInputFilelist(false)
        , HasRegion(false)
    {}
};

// ---------------------------------------------
// CountToolPrivate implementation

struct CountTool::CountToolPrivate
{

    // ctor & dtro
public:
    CountToolPrivate(CountTool::CountSettings* settings)
        : m_settings(settings)
    {}

    // interface
public:
    bool Run();

    // data members
private:
    CountTool::CountSettings* m_settings;
};

bool CountTool::CountToolPrivate::Run()
{

    // set to default input if none provided
    if (!m_settings->HasInput && !m_settings->HasInputFilelist)
        m_settings->InputFiles.push_back(Options::StandardIn());

    // add files in the filelist to the input file list
    if (m_settings->HasInputFilelist) {

        std::ifstream filelist(m_settings->InputFilelist.c_str(), std::ios::in);
        if (!filelist.is_open()) {
            std::cerr << "bamtools count ERROR: could not open input BAM file list... Aborting."
                      << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(filelist, line))
            m_settings->InputFiles.push_back(line);
    }

    // open reader without index
    BamMultiReader reader;
    if (!reader.Open(m_settings->InputFiles)) {
        std::cerr << "bamtools count ERROR: could not open input BAM file(s)... Aborting."
                  << std::endl;
        return false;
    }

    // alignment counter
    BamAlignment al;
    int alignmentCount(0);

    // if no region specified, count entire file
    if (!m_settings->HasRegion) {
        while (reader.GetNextAlignmentCore(al))
            ++alignmentCount;
    }

    // otherwise attempt to use region as constraint
    else {

        // if region string parses OK
        BamRegion region;
        if (Utilities::ParseRegionString(m_settings->Region, reader, region)) {

            // attempt to find index files
            reader.LocateIndexes();

            // if index data available for all BAM files, we can use SetRegion
            if (reader.HasIndexes()) {

                // attempt to set region on reader
                if (!reader.SetRegion(region.LeftRefID, region.LeftPosition, region.RightRefID,
                                      region.RightPosition)) {
                    std::cerr << "bamtools count ERROR: set region failed. Check that REGION "
                                 "describes a valid range"
                              << std::endl;
                    reader.Close();
                    return false;
                }

                // everything checks out, just iterate through specified region, counting alignments
                while (reader.GetNextAlignmentCore(al))
                    ++alignmentCount;
            }

            // no index data available, we have to iterate through until we
            // find overlapping alignments
            else {
                while (reader.GetNextAlignmentCore(al)) {
                    if ((al.RefID >= region.LeftRefID) &&
                        ((al.Position + al.Length) >= region.LeftPosition) &&
                        (al.RefID <= region.RightRefID) && (al.Position <= region.RightPosition)) {
                        ++alignmentCount;
                    }
                }
            }
        }

        // error parsing REGION string
        else {
            std::cerr << "bamtools count ERROR: could not parse REGION - " << m_settings->Region
                      << std::endl;
            std::cerr << "Check that REGION is in valid format (see documentation) and that the "
                         "coordinates are valid"
                      << std::endl;
            reader.Close();
            return false;
        }
    }

    // print results
    std::cout << alignmentCount << std::endl;

    // clean up & exit
    reader.Close();
    return true;
}

// ---------------------------------------------
// CountTool implementation

CountTool::CountTool()
    : AbstractTool()
    , m_settings(new CountSettings)
    , m_impl(0)
{
    // set program details
    Options::SetProgramInfo(
        "bamtools count", "prints number of alignments in BAM file(s)",
        "[-in <filename> -in <filename> ... | -list <filelist>] [-region <REGION>]");

    // set up options
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in", "BAM filename", "the input BAM file(s)", "",
                            m_settings->HasInput, m_settings->InputFiles, IO_Opts,
                            Options::StandardIn());
    Options::AddValueOption("-list", "filename", "the input BAM file list, one line per file", "",
                            m_settings->HasInputFilelist, m_settings->InputFilelist, IO_Opts);
    Options::AddValueOption("-region", "REGION",
                            "genomic region. Index file is recommended for better performance, and "
                            "is used automatically if it exists. See \'bamtools help index\' for "
                            "more details on creating one",
                            "", m_settings->HasRegion, m_settings->Region, IO_Opts);
}

CountTool::~CountTool()
{

    delete m_settings;
    m_settings = 0;

    delete m_impl;
    m_impl = 0;
}

int CountTool::Help()
{
    Options::DisplayHelp();
    return 0;
}

int CountTool::Run(int argc, char* argv[])
{

    // parse command line arguments
    Options::Parse(argc, argv, 1);

    // initialize CountTool with settings
    m_impl = new CountToolPrivate(m_settings);

    // run CountTool, return success/fail
    if (m_impl->Run())
        return 0;
    else
        return 1;
}
