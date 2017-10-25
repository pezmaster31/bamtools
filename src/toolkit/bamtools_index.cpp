// ***************************************************************************
// bamtools_index.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 7 April 2011
// ---------------------------------------------------------------------------
// Creates a BAM index file
// ***************************************************************************

#include "bamtools_index.h"

#include <api/BamReader.h>
#include <utils/bamtools_options.h>
using namespace BamTools;

#include <iostream>
#include <string>

// ---------------------------------------------
// IndexSettings implementation

struct IndexTool::IndexSettings {

    // flags
    bool HasInputBamFilename;
    bool IsUsingBamtoolsIndex;

    // filenames
    std::string InputBamFilename;

    // constructor
    IndexSettings()
        : HasInputBamFilename(false)
        , IsUsingBamtoolsIndex(false)
        , InputBamFilename(Options::StandardIn())
    { }
};

// ---------------------------------------------
// IndexToolPrivate implementation

struct IndexTool::IndexToolPrivate {

    // ctor & dtor
    public:
        IndexToolPrivate(IndexTool::IndexSettings* settings)
            : m_settings(settings)
        { }

        ~IndexToolPrivate() { }

    // interface
    public:
        bool Run();

    // data members
    private:
        IndexTool::IndexSettings* m_settings;
};

bool IndexTool::IndexToolPrivate::Run() {

    // open our BAM reader
    BamReader reader;
    if ( !reader.Open(m_settings->InputBamFilename) ) {
        std::cerr << "bamtools index ERROR: could not open BAM file: "
             << m_settings->InputBamFilename << std::endl;
        return false;
    }

    // create index for BAM file
    const BamIndex::IndexType type = ( m_settings->IsUsingBamtoolsIndex ? BamIndex::BAMTOOLS
                                                                        : BamIndex::STANDARD );
    reader.CreateIndex(type);

    // clean & exit
    reader.Close();
    return true;
}

// ---------------------------------------------
// IndexTool implementation

IndexTool::IndexTool()
    : AbstractTool()
    , m_settings(new IndexSettings)
    , m_impl(0)
{
    // set program details
    Options::SetProgramInfo("bamtools index", "creates index for BAM file", "[-in <filename>] [-bti]");

    // set up options
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in", "BAM filename", "the input BAM file", "", m_settings->HasInputBamFilename, m_settings->InputBamFilename, IO_Opts, Options::StandardIn());
    Options::AddOption("-bti", "create (non-standard) BamTools index file (*.bti). Default behavior is to create standard BAM index (*.bai)", m_settings->IsUsingBamtoolsIndex, IO_Opts);
}

IndexTool::~IndexTool() {

    delete m_settings;
    m_settings = 0;

    delete m_impl;
    m_impl = 0;
}

int IndexTool::Help() {
    Options::DisplayHelp();
    return 0;
}

int IndexTool::Run(int argc, char* argv[]) {

    // parse command line arguments
    Options::Parse(argc, argv, 1);

    // initialize IndexTool with settings
    m_impl = new IndexToolPrivate(m_settings);

    // run IndexTool, return success/fail
    if ( m_impl->Run() )
        return 0;
    else
        return 1;
}
