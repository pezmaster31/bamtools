// ***************************************************************************
// bamtools_cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 December 2012
// ---------------------------------------------------------------------------
// Prints general alignment statistics for BAM file(s).
// ***************************************************************************

#include "bamtools_stats.h"

#include <api/BamMultiReader.h>
#include <utils/bamtools_options.h>
using namespace BamTools;

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

// ---------------------------------------------
// StatsSettings implementation

struct StatsTool::StatsSettings
{

    // flags
    bool HasInput;
    bool HasInputFilelist;
    bool IsShowingInsertSizeSummary;

    // filenames
    std::vector<std::string> InputFiles;
    std::string InputFilelist;

    // constructor
    StatsSettings()
        : HasInput(false)
        , HasInputFilelist(false)
        , IsShowingInsertSizeSummary(false)
    {}
};

// ---------------------------------------------
// StatsToolPrivate implementation

struct StatsTool::StatsToolPrivate
{

    // ctor & dtor
public:
    StatsToolPrivate(StatsTool::StatsSettings* _settings);

    // 'public' interface
public:
    bool Run();

    // internal methods
private:
    bool CalculateMedian(std::vector<int>& data, double& median);
    void PrintStats();
    void ProcessAlignment(const BamAlignment& al);

    // data members
private:
    StatsTool::StatsSettings* m_settings;
    unsigned int m_numReads;
    unsigned int m_numPaired;
    unsigned int m_numProperPair;
    unsigned int m_numMapped;
    unsigned int m_numBothMatesMapped;
    unsigned int m_numForwardStrand;
    unsigned int m_numReverseStrand;
    unsigned int m_numFirstMate;
    unsigned int m_numSecondMate;
    unsigned int m_numSingletons;
    unsigned int m_numFailedQC;
    unsigned int m_numDuplicates;
    std::vector<int> m_insertSizes;
};

StatsTool::StatsToolPrivate::StatsToolPrivate(StatsTool::StatsSettings* settings)
    : m_settings(settings)
    , m_numReads(0)
    , m_numPaired(0)
    , m_numProperPair(0)
    , m_numMapped(0)
    , m_numBothMatesMapped(0)
    , m_numForwardStrand(0)
    , m_numReverseStrand(0)
    , m_numFirstMate(0)
    , m_numSecondMate(0)
    , m_numSingletons(0)
    , m_numFailedQC(0)
    , m_numDuplicates(0)
{
    m_insertSizes.reserve(100000);
}

// median is of type double because in the case of even number of data elements,
// we need to return the average of middle 2 elements
bool StatsTool::StatsToolPrivate::CalculateMedian(std::vector<int>& data, double& median)
{

    // skip if data empty
    if (data.empty()) return false;

    // find middle element
    std::size_t middleIndex = data.size() / 2;
    std::vector<int>::iterator target = data.begin() + middleIndex;
    nth_element(data.begin(), target, data.end());

    // odd number of elements
    if ((data.size() % 2) != 0) {
        median = (double)(*target);
        return true;
    }

    // even number of elements
    else {
        double rightTarget = (double)(*target);
        std::vector<int>::iterator leftTarget = target - 1;
        nth_element(data.begin(), leftTarget, data.end());
        median = (double)((rightTarget + *leftTarget) / 2.0);
        return true;
    }
}

// print BAM file alignment stats
void StatsTool::StatsToolPrivate::PrintStats()
{

    std::cout << std::endl;
    std::cout << "**********************************************" << std::endl;
    std::cout << "Stats for BAM file(s): " << std::endl;
    std::cout << "**********************************************" << std::endl;
    std::cout << std::endl;
    std::cout << "Total reads:       " << m_numReads << std::endl;
    std::cout << "Mapped reads:      " << m_numMapped << "\t("
              << ((float)m_numMapped / m_numReads) * 100 << "%)" << std::endl;
    std::cout << "Forward strand:    " << m_numForwardStrand << "\t("
              << ((float)m_numForwardStrand / m_numReads) * 100 << "%)" << std::endl;
    std::cout << "Reverse strand:    " << m_numReverseStrand << "\t("
              << ((float)m_numReverseStrand / m_numReads) * 100 << "%)" << std::endl;
    std::cout << "Failed QC:         " << m_numFailedQC << "\t("
              << ((float)m_numFailedQC / m_numReads) * 100 << "%)" << std::endl;
    std::cout << "Duplicates:        " << m_numDuplicates << "\t("
              << ((float)m_numDuplicates / m_numReads) * 100 << "%)" << std::endl;
    std::cout << "Paired-end reads:  " << m_numPaired << "\t("
              << ((float)m_numPaired / m_numReads) * 100 << "%)" << std::endl;

    if (m_numPaired != 0) {
        std::cout << "'Proper-pairs':    " << m_numProperPair << "\t("
                  << ((float)m_numProperPair / m_numPaired) * 100 << "%)" << std::endl;
        std::cout << "Both pairs mapped: " << m_numBothMatesMapped << "\t("
                  << ((float)m_numBothMatesMapped / m_numPaired) * 100 << "%)" << std::endl;
        std::cout << "Read 1:            " << m_numFirstMate << std::endl;
        std::cout << "Read 2:            " << m_numSecondMate << std::endl;
        std::cout << "Singletons:        " << m_numSingletons << "\t("
                  << ((float)m_numSingletons / m_numPaired) * 100 << "%)" << std::endl;
    }

    if (m_settings->IsShowingInsertSizeSummary) {

        double avgInsertSize = 0.0;
        if (!m_insertSizes.empty()) {
            avgInsertSize = (accumulate(m_insertSizes.begin(), m_insertSizes.end(), 0.0) /
                             (double)m_insertSizes.size());
            std::cout << "Average insert size (absolute value): " << avgInsertSize << std::endl;
        }

        double medianInsertSize = 0.0;
        if (CalculateMedian(m_insertSizes, medianInsertSize))
            std::cout << "Median insert size (absolute value): " << medianInsertSize << std::endl;
    }
    std::cout << std::endl;
}

// use current input alignment to update BAM file alignment stats
void StatsTool::StatsToolPrivate::ProcessAlignment(const BamAlignment& al)
{

    // increment total alignment counter
    ++m_numReads;

    // incrememt counters for pairing-independent flags
    if (al.IsDuplicate()) ++m_numDuplicates;
    if (al.IsFailedQC()) ++m_numFailedQC;
    if (al.IsMapped()) ++m_numMapped;

    // increment strand counters
    if (al.IsReverseStrand())
        ++m_numReverseStrand;
    else
        ++m_numForwardStrand;

    // if alignment is paired-end
    if (al.IsPaired()) {

        // increment PE counter
        ++m_numPaired;

        // increment first mate/second mate counters
        if (al.IsFirstMate()) ++m_numFirstMate;
        if (al.IsSecondMate()) ++m_numSecondMate;

        // if alignment is mapped, check mate status
        if (al.IsMapped()) {
            // if mate mapped
            if (al.IsMateMapped()) ++m_numBothMatesMapped;
            // else singleton
            else
                ++m_numSingletons;
        }

        // check for explicit proper pair flag
        if (al.IsProperPair()) ++m_numProperPair;

        // store insert size for first mate
        if (m_settings->IsShowingInsertSizeSummary && al.IsFirstMate() && (al.InsertSize != 0)) {
            int insertSize = std::abs(al.InsertSize);
            m_insertSizes.push_back(insertSize);
        }
    }
}

bool StatsTool::StatsToolPrivate::Run()
{

    // set to default input if none provided
    if (!m_settings->HasInput && !m_settings->HasInputFilelist)
        m_settings->InputFiles.push_back(Options::StandardIn());

    // add files in the filelist to the input file list
    if (m_settings->HasInputFilelist) {

        std::ifstream filelist(m_settings->InputFilelist.c_str(), std::ios::in);
        if (!filelist.is_open()) {
            std::cerr << "bamtools stats ERROR: could not open input BAM file list... Aborting."
                      << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(filelist, line))
            m_settings->InputFiles.push_back(line);
    }

    // open the BAM files
    BamMultiReader reader;
    if (!reader.Open(m_settings->InputFiles)) {
        std::cerr << "bamtools stats ERROR: could not open input BAM file(s)... Aborting."
                  << std::endl;
        reader.Close();
        return false;
    }

    // plow through alignments, keeping track of stats
    BamAlignment al;
    while (reader.GetNextAlignmentCore(al))
        ProcessAlignment(al);
    reader.Close();

    // print stats & exit
    PrintStats();
    return true;
}

// ---------------------------------------------
// StatsTool implementation

StatsTool::StatsTool()
    : AbstractTool()
    , m_settings(new StatsSettings)
    , m_impl(0)
{
    // set program details
    Options::SetProgramInfo(
        "bamtools stats", "prints general alignment statistics",
        "[-in <filename> -in <filename> ... | -list <filelist>] [statsOptions]");

    // set up options
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in", "BAM filename", "the input BAM file", "", m_settings->HasInput,
                            m_settings->InputFiles, IO_Opts, Options::StandardIn());
    Options::AddValueOption("-list", "filename", "the input BAM file list, one line per file", "",
                            m_settings->HasInputFilelist, m_settings->InputFilelist, IO_Opts);

    OptionGroup* AdditionalOpts = Options::CreateOptionGroup("Additional Stats");
    Options::AddOption("-insert", "summarize insert size data",
                       m_settings->IsShowingInsertSizeSummary, AdditionalOpts);
}

StatsTool::~StatsTool()
{

    delete m_settings;
    m_settings = 0;

    delete m_impl;
    m_impl = 0;
}

int StatsTool::Help()
{
    Options::DisplayHelp();
    return 0;
}

int StatsTool::Run(int argc, char* argv[])
{

    // parse command line arguments
    Options::Parse(argc, argv, 1);

    // initialize StatsTool with settings
    m_impl = new StatsToolPrivate(m_settings);

    // run StatsTool, return success/fail
    if (m_impl->Run())
        return 0;
    else
        return 1;
}
