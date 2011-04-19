// ***************************************************************************
// bamtools_resolve.cpp (c) 2011
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 7 April 2011
// ---------------------------------------------------------------------------
// Resolves paired-end reads (marking the IsProperPair flag as needed)
// ***************************************************************************

#include "bamtools_resolve.h"

#include <api/BamReader.h>
#include <api/BamWriter.h>
#include <utils/bamtools_options.h>
#include <utils/bamtools_utilities.h>
using namespace BamTools;

#include <iostream>
#include <string>
using namespace std;

// ---------------------------------------------
// ResolveSettings implementation

struct ResolveTool::ResolveSettings {

    // flags
    bool HasInput;
    bool HasOutput;
    bool IsForceCompression;

    // filenames
    string InputFilename;
    string OutputFilename;

    // constructor
    ResolveSettings(void)
        : HasInput(false)
        , HasOutput(false)
        , IsForceCompression(false)
        , InputFilename(Options::StandardIn())
        , OutputFilename(Options::StandardOut())
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

    // data members
    private:
        ResolveTool::ResolveSettings* m_settings;
};

bool ResolveTool::ResolveToolPrivate::Run(void) {
    cerr << "Resoling BAM file..." << endl;
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
    Options::SetProgramInfo("bamtools resolve", "resolves paired-end reads (marking the IsProperPair flag as needed)", "[-in <filename> -in <filename> ...] [-out <filename> | [-forceCompression]] [resolveOptions]");

    // set up options
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in",  "BAM filename", "the input BAM file",  "", m_settings->HasInput,  m_settings->InputFilename,  IO_Opts, Options::StandardIn());
    Options::AddValueOption("-out", "BAM filename", "the output BAM file", "", m_settings->HasOutput, m_settings->OutputFilename, IO_Opts, Options::StandardOut());
    Options::AddOption("-forceCompression", "if results are sent to stdout (like when piping to another tool), default behavior is to leave output uncompressed. Use this flag to override and force compression", m_settings->IsForceCompression, IO_Opts);
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
    if ( m_impl->Run() ) return 0;
    else return 1;
}
