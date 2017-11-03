// ***************************************************************************
// bamtools_coverage.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 24 July 2013
// ---------------------------------------------------------------------------
// Prints coverage data for a single BAM file
// ***************************************************************************

#include "bamtools_coverage.h"

#include <api/BamReader.h>
#include <utils/bamtools_options.h>
#include <utils/bamtools_pileup_engine.h>
using namespace BamTools;

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace BamTools {

// ---------------------------------------------
// CoverageVisitor implementation

class CoverageVisitor : public PileupVisitor
{

public:
    CoverageVisitor(const RefVector& references, std::ostream* out)
        : PileupVisitor()
        , m_references(references)
        , m_out(out)
    {}
    ~CoverageVisitor() {}

    // PileupVisitor interface implementation
public:
    // prints coverage results ( tab-delimited )
    void Visit(const PileupPosition& pileupData)
    {
        *m_out << m_references[pileupData.RefId].RefName << '\t' << pileupData.Position << '\t'
               << pileupData.PileupAlignments.size() << std::endl;
    }

private:
    RefVector m_references;
    std::ostream* m_out;
};

}  // namespace BamTools

// ---------------------------------------------
// CoverageSettings implementation

struct CoverageTool::CoverageSettings
{

    // flags
    bool HasInputFile;
    bool HasOutputFile;

    // filenames
    std::string InputBamFilename;
    std::string OutputFilename;

    // constructor
    CoverageSettings()
        : HasInputFile(false)
        , HasOutputFile(false)
        , InputBamFilename(Options::StandardIn())
        , OutputFilename(Options::StandardOut())
    {}
};

// ---------------------------------------------
// CoverageToolPrivate implementation

struct CoverageTool::CoverageToolPrivate
{

    // ctor & dtor
public:
    CoverageToolPrivate(CoverageTool::CoverageSettings* settings)
        : m_settings(settings)
        , m_out(std::cout.rdbuf())
    {}

    ~CoverageToolPrivate() {}

    // interface
public:
    bool Run();

    // data members
private:
    CoverageTool::CoverageSettings* m_settings;
    std::ostream m_out;
    RefVector m_references;
};

bool CoverageTool::CoverageToolPrivate::Run()
{

    // if output filename given
    std::ofstream outFile;
    if (m_settings->HasOutputFile) {

        // open output file stream
        outFile.open(m_settings->OutputFilename.c_str());
        if (!outFile) {
            std::cerr << "bamtools coverage ERROR: could not open " << m_settings->OutputFilename
                      << " for output" << std::endl;
            return false;
        }

        // set m_out to file's streambuf
        m_out.rdbuf(outFile.rdbuf());
    }

    //open our BAM reader
    BamReader reader;
    if (!reader.Open(m_settings->InputBamFilename)) {
        std::cerr << "bamtools coverage ERROR: could not open input BAM file: "
                  << m_settings->InputBamFilename << std::endl;
        return false;
    }

    // retrieve references
    m_references = reader.GetReferenceData();

    // set up our output 'visitor'
    CoverageVisitor* cv = new CoverageVisitor(m_references, &m_out);

    // set up pileup engine with 'visitor'
    PileupEngine pileup;
    pileup.AddVisitor(cv);

    // process input data
    BamAlignment al;
    while (reader.GetNextAlignment(al))
        pileup.AddAlignment(al);
    pileup.Flush();

    // clean up
    reader.Close();
    if (m_settings->HasOutputFile) outFile.close();
    delete cv;
    cv = 0;

    // return success
    return true;
}

// ---------------------------------------------
// CoverageTool implementation

CoverageTool::CoverageTool()
    : AbstractTool()
    , m_settings(new CoverageSettings)
    , m_impl(0)
{
    // set program details
    Options::SetProgramInfo("bamtools coverage", "prints coverage data for a single BAM file",
                            "[-in <filename>] [-out <filename>]");

    // set up options
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in", "BAM filename", "the input BAM file", "",
                            m_settings->HasInputFile, m_settings->InputBamFilename, IO_Opts,
                            Options::StandardIn());
    Options::AddValueOption("-out", "filename", "the output file", "", m_settings->HasOutputFile,
                            m_settings->OutputFilename, IO_Opts, Options::StandardOut());
}

CoverageTool::~CoverageTool()
{

    delete m_settings;
    m_settings = 0;

    delete m_impl;
    m_impl = 0;
}

int CoverageTool::Help()
{
    Options::DisplayHelp();
    return 0;
}

int CoverageTool::Run(int argc, char* argv[])
{

    // parse command line arguments
    Options::Parse(argc, argv, 1);

    // initialize CoverageTool with settings
    m_impl = new CoverageToolPrivate(m_settings);

    // run CoverageTool, return success/fail
    if (m_impl->Run())
        return 0;
    else
        return 1;
}
