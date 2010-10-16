// ***************************************************************************
// bamtools_cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 12 October 2010
// ---------------------------------------------------------------------------
// Prints general alignment statistics for BAM file(s).
// ***************************************************************************

#include <cmath>
#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include "bamtools_stats.h"
#include "bamtools_options.h"
#include "BamMultiReader.h"
using namespace std;
using namespace BamTools;

// ---------------------------------------------
// StatsSettings implementation

struct StatsTool::StatsSettings {

    // flags
    bool HasInput;
    bool IsShowingInsertSizeSummary;

    // filenames
    vector<string> InputFiles;
    
    // constructor
    StatsSettings(void)
        : HasInput(false)
        , IsShowingInsertSizeSummary(false)
    { }
};  

// ---------------------------------------------
// StatsToolPrivate implementation

struct StatsTool::StatsToolPrivate {
  
    // ctor & dtor
    public:
        StatsToolPrivate(StatsTool::StatsSettings* _settings);
        ~StatsToolPrivate(void);
  
    // 'public' interface
    public:
        bool Run(void);
        
    // internal methods
    private:
        bool CalculateMedian(vector<int>& data, double& median); 
        void PrintStats(void);
        void ProcessAlignment(const BamAlignment& al);
        
    // data members
    private:
        StatsTool::StatsSettings* settings;
        unsigned int numReads;
        unsigned int numPaired;
        unsigned int numProperPair;
        unsigned int numMapped;
        unsigned int numBothMatesMapped;
        unsigned int numForwardStrand;
        unsigned int numReverseStrand;
        unsigned int numFirstMate;
        unsigned int numSecondMate;
        unsigned int numSingletons;
        unsigned int numFailedQC;
        unsigned int numDuplicates;
        vector<int> insertSizes;
};

StatsTool::StatsToolPrivate::StatsToolPrivate(StatsTool::StatsSettings* _settings)
    : settings(_settings)
    , numReads(0)
    , numPaired(0)
    , numProperPair(0)
    , numMapped(0)
    , numBothMatesMapped(0)
    , numForwardStrand(0)
    , numReverseStrand(0)
    , numFirstMate(0)
    , numSecondMate(0)
    , numSingletons(0)
    , numFailedQC(0)
    , numDuplicates(0)
{ 
    insertSizes.reserve(100000);
}

StatsTool::StatsToolPrivate::~StatsToolPrivate(void) { }

// median is of type double because in the case of even number of data elements, we need to return the average of middle 2 elements
bool StatsTool::StatsToolPrivate::CalculateMedian(vector<int>& data, double& median) { 
  
    // check that data exists
    if ( data.empty() ) return false;

    // find middle element
    size_t middleIndex = data.size() / 2;
    vector<int>::iterator target = data.begin() + middleIndex;
    nth_element(data.begin(), target, data.end());
    
    // odd number of elements
    if ( (data.size() % 2) != 0) {
        median = (double)(*target);
        return true;
    }
    
    // even number of elements
    else {
        double rightTarget = (double)(*target);
        vector<int>::iterator leftTarget = target - 1;
        nth_element(data.begin(), leftTarget, data.end());
        median = (double)((rightTarget+*leftTarget)/2.0);
        return true;
    }
}

// print BAM file alignment stats
void StatsTool::StatsToolPrivate::PrintStats(void) {
  
    cout << endl;
    cout << "**********************************************" << endl;
    cout << "Stats for BAM file(s): " << endl;
    cout << "**********************************************" << endl;
    cout << endl;
    cout << "Total reads:       " << numReads << endl;
    cout << "Mapped reads:      " << numMapped << "\t(" << ((float)numMapped/numReads)*100 << "%)" << endl;
    cout << "Forward strand:    " << numForwardStrand << "\t(" << ((float)numForwardStrand/numReads)*100 << "%)" << endl;
    cout << "Reverse strand:    " << numReverseStrand << "\t(" << ((float)numReverseStrand/numReads)*100 << "%)" << endl;
    cout << "Failed QC:         " << numFailedQC << "\t(" << ((float)numFailedQC/numReads)*100 << "%)" << endl;
    cout << "Duplicates:        " << numDuplicates << "\t(" << ((float)numDuplicates/numReads)*100 << "%)" << endl;
    cout << "Paired-end reads:  " << numPaired << "\t(" << ((float)numPaired/numReads)*100 << "%)" << endl;
    
    if ( numPaired != 0 ) {
        cout << "'Proper-pairs':    " << numProperPair << "\t(" << ((float)numProperPair/numPaired)*100 << "%)" << endl;
        cout << "Both pairs mapped: " << numBothMatesMapped << "\t(" << ((float)numBothMatesMapped/numPaired)*100 << "%)" << endl;
        cout << "Read 1:            " << numFirstMate << endl;
        cout << "Read 2:            " << numSecondMate << endl;
        cout << "Singletons:        " << numSingletons << "\t(" << ((float)numSingletons/numPaired)*100 << "%)" << endl;
    }
    
    if ( settings->IsShowingInsertSizeSummary ) {
      
        double avgInsertSize = 0.0;
        if ( !insertSizes.empty() ) {
            avgInsertSize = ( accumulate(insertSizes.begin(), insertSizes.end(), 0.0) / (double)insertSizes.size() );
            cout << "Average insert size (absolute value): " << avgInsertSize << endl;
        }
        
        double medianInsertSize = 0.0;
        if ( CalculateMedian(insertSizes, medianInsertSize) )
            cout << "Median insert size (absolute value): " << medianInsertSize << endl;
    }
    cout << endl;
}

// use current input alignment to update BAM file alignment stats
void StatsTool::StatsToolPrivate::ProcessAlignment(const BamAlignment& al) {
  
    // increment total alignment counter
    ++numReads;
    
    // check the paired-independent flags
    if ( al.IsDuplicate() ) ++numDuplicates;
    if ( al.IsFailedQC()  ) ++numFailedQC;
    if ( al.IsMapped()    ) ++numMapped;
    
    // check forward/reverse strand
    if ( al.IsReverseStrand() ) 
        ++numReverseStrand; 
    else 
        ++numForwardStrand;
    
    // if alignment is paired-end
    if ( al.IsPaired() ) {
      
        // increment PE counter
        ++numPaired;
      
        // increment first mate/second mate counters
        if ( al.IsFirstMate()  ) ++numFirstMate;
        if ( al.IsSecondMate() ) ++numSecondMate;
        
        // if alignment is mapped, check mate status
        if ( al.IsMapped() ) {
            // if mate mapped
            if ( al.IsMateMapped() ) 
                ++numBothMatesMapped;
            // else singleton
            else 
                ++numSingletons;
        }
        
        // check for explicit proper pair flag
        if ( al.IsProperPair() ) ++numProperPair;
        
        // store insert size for first mate 
        if ( settings->IsShowingInsertSizeSummary && al.IsFirstMate() && (al.InsertSize != 0) ) {
            int insertSize = abs(al.InsertSize);
            insertSizes.push_back( insertSize );
        }
    }
}

bool StatsTool::StatsToolPrivate::Run() {
  
    // opens the BAM files without checking for indexes 
    BamMultiReader reader;
    if ( !reader.Open(settings->InputFiles, false, true) ) {
        cerr << "Could not open input BAM file(s)... quitting." << endl;
        reader.Close();
        return false;
    }
    
    // plow through file, keeping track of stats
    BamAlignment al;
    while ( reader.GetNextAlignmentCore(al) )
        ProcessAlignment(al);
    
    // print stats
    PrintStats();
    
    // clean and exit
    reader.Close();
    return true; 
}

// ---------------------------------------------
// StatsTool implementation

StatsTool::StatsTool(void)
    : AbstractTool()
    , m_settings(new StatsSettings)
    , m_impl(0)
{
    // set program details
    Options::SetProgramInfo("bamtools stats", "prints general alignment statistics", "[-in <filename> -in <filename> ...] [statsOptions]");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in", "BAM filename", "the input BAM file", "", m_settings->HasInput,  m_settings->InputFiles,  IO_Opts, Options::StandardIn());
    
    OptionGroup* AdditionalOpts = Options::CreateOptionGroup("Additional Stats");
    Options::AddOption("-insert", "summarize insert size data", m_settings->IsShowingInsertSizeSummary, AdditionalOpts);
}

StatsTool::~StatsTool(void) {
    delete m_settings;
    m_settings = 0;
    
    delete m_impl;
    m_impl = 0;
}

int StatsTool::Help(void) {
    Options::DisplayHelp();
    return 0;
}

int StatsTool::Run(int argc, char* argv[]) {
  
    // parse command line arguments
    Options::Parse(argc, argv, 1);
    
    // set to default input if none provided
    if ( !m_settings->HasInput ) 
        m_settings->InputFiles.push_back(Options::StandardIn());
    
    // run internal StatsTool implementation, return success/fail
    m_impl = new StatsToolPrivate(m_settings);
    
    if ( m_impl->Run() ) return 0;
    else return 1;
}
