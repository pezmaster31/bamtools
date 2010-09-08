// ***************************************************************************
// bamtools_merge.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 7 September 2010
// ---------------------------------------------------------------------------
// Merges multiple BAM files into one.
// ***************************************************************************

#include <iostream>
#include <string>
#include <vector>

#include "bamtools_merge.h"
#include "bamtools_options.h"
#include "bamtools_utilities.h"
#include "BamMultiReader.h"
#include "BamWriter.h"

using namespace std;
using namespace BamTools;

// ---------------------------------------------
// MergeSettings implementation

struct MergeTool::MergeSettings {

    // flags
    bool HasInputBamFilename;
    bool HasOutputBamFilename;
    bool IsForceCompression;
    bool HasRegion;
    
    // filenames
    vector<string> InputFiles;
    
    // other parameters
    string OutputFilename;
    string Region;
    
    // constructor
    MergeSettings(void)
        : HasInputBamFilename(false)
        , HasOutputBamFilename(false)
        , IsForceCompression(false)
        , HasRegion(false)
        , OutputFilename(Options::StandardOut())
    { }
};  

// ---------------------------------------------
// MergeTool implementation

MergeTool::MergeTool(void)
    : AbstractTool()
    , m_settings(new MergeSettings)
{
    // set program details
    Options::SetProgramInfo("bamtools merge", "merges multiple BAM files into one", "[-in <filename> -in <filename> ...] [-out <filename>]");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in",  "BAM filename", "the input BAM file(s)", "", m_settings->HasInputBamFilename,  m_settings->InputFiles,     IO_Opts);
    Options::AddValueOption("-out", "BAM filename", "the output BAM file",   "", m_settings->HasOutputBamFilename, m_settings->OutputFilename, IO_Opts);
    Options::AddOption("-forceCompression", "if results are sent to stdout (like when piping to another tool), default behavior is to leave output uncompressed. Use this flag to override and force compression", m_settings->IsForceCompression, IO_Opts);
    
    OptionGroup* FilterOpts = Options::CreateOptionGroup("Filters");
    Options::AddValueOption("-region", "REGION", "genomic region. See README for more details", "", m_settings->HasRegion, m_settings->Region, FilterOpts);
}

MergeTool::~MergeTool(void) {
    delete m_settings;
    m_settings = 0;
}

int MergeTool::Help(void) {
    Options::DisplayHelp();
    return 0;
}

int MergeTool::Run(int argc, char* argv[]) {
  
    // parse command line arguments
    Options::Parse(argc, argv, 1);
    
     // set to default input if none provided
    if ( !m_settings->HasInputBamFilename ) 
        m_settings->InputFiles.push_back(Options::StandardIn());
    
    // opens the BAM files (by default without checking for indexes)
    BamMultiReader reader;
    if ( !reader.Open(m_settings->InputFiles, false, true) ) { 
        cerr << "ERROR: Could not open input BAM file(s)... Aborting." << endl;
        return 1;
    }
    
    // retrieve header & reference dictionary info
    std::string mergedHeader = reader.GetHeaderText();
    RefVector references = reader.GetReferenceData();

    // open writer
    BamWriter writer;
    bool writeUncompressed = ( m_settings->OutputFilename == Options::StandardOut() && !m_settings->IsForceCompression );
    if ( !writer.Open(m_settings->OutputFilename, mergedHeader, references, writeUncompressed) ) {
        cerr << "ERROR: Could not open BAM file " << m_settings->OutputFilename << " for writing... Aborting." << endl;
        reader.Close();
        return 1;
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

            // attempt to re-open reader with index files
            reader.Close();
            bool openedOK = reader.Open(m_settings->InputFiles, true, true );
            
            // if error
            if ( !openedOK ) {
                cerr << "ERROR: Could not open input BAM file(s)... Aborting." << endl;
                return 1;
            }
            
            // if index data available, we can use SetRegion
            if ( reader.IsIndexLoaded() ) {
              
                // attempt to use SetRegion(), if failed report error
                if ( !reader.SetRegion(region.LeftRefID, region.LeftPosition, region.RightRefID, region.RightPosition) ) {
                    cerr << "ERROR: Region requested, but could not set BamReader region to REGION: " << m_settings->Region << " Aborting." << endl;
                    reader.Close();
                    return 1;
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
            cerr << "ERROR: Could not parse REGION - " << m_settings->Region << endl;
            cerr << "Be sure REGION is in valid format (see README) and that coordinates are valid for selected references" << endl;
            reader.Close();
            writer.Close();
            return 1;
        }
    }
    
    // clean & exit
    reader.Close();
    writer.Close();
    return 0;  
}
