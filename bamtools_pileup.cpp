#include <vector>
#include "BamMultiReader.h"
#include "bamtools_pileup.h"
using namespace std;
using namespace BamTools;

struct Pileup::PileupPrivate {
  
    // ---------------------
    // data members
  
    // IO & settings
    BamMultiReader* Reader;
    ostream* OutStream;
    string FastaFilename;
    bool IsPrintingMapQualities;
    BamRegion Region;
    
    // parsing data
    int CurrentId;
    int CurrentPosition;
    vector<BamAlignment> CurrentData;
    RefVector References;
    
    // ----------------------
    // ctor
    
    PileupPrivate(BamMultiReader* reader, ostream* outStream)
        : Reader(reader)
        , OutStream(outStream)
        , FastaFilename("")
        , IsPrintingMapQualities(false)
    { }
    
    // ----------------------
    // internal methods
    
    void PrintCurrentData(void);
    bool Run(void);
};

void Pileup::PileupPrivate::PrintCurrentData(void) {
  
    // remove any data that ends before CurrentPosition
    size_t i = 0;
    while ( i < CurrentData.size() ) {
        if ( CurrentData[i].GetEndPosition() < CurrentPosition )
            CurrentData.erase(CurrentData.begin() + i);
        else
            ++i;
    }
  
    // if not data remains, return
    if ( CurrentData.empty() ) return;
    
    // initialize empty strings
    string bases     = "";
    string baseQuals = "";
    string mapQuals  = "";
    
    // iterate over alignments
    vector<BamAlignment>::const_iterator dataIter = CurrentData.begin();
    vector<BamAlignment>::const_iterator dataEnd  = CurrentData.end();
    for ( ; dataIter != dataEnd; ++dataIter ) {
        
        // retrieve alignment
        const BamAlignment& al = (*dataIter);
        
        // determine current base character & store
        const char base = al.AlignedBases[CurrentPosition -al.Position];
        if ( al.IsReverseStrand() ) 
            bases.push_back( tolower(base) );
        else 
            bases.push_back( toupper(base) );
        
        // determine current base quality & store
        baseQuals.push_back( al.Qualities[CurrentPosition - al.Position] );
        
        // if using mapQuals, determine current mapQual & store
        if ( IsPrintingMapQualities ) {
            int mapQuality = (int)(al.MapQuality + 33);
            if ( mapQuality > 126 ) mapQuality = 126; 
            mapQuals.push_back((char)mapQuality);
        }
    }
    
    // print results to OutStream
    const string& refName = References[CurrentId].RefName;
    const char refBase = 'N';
    
    *OutStream << refName << "\t" << CurrentPosition << "\t" << refBase << "\t" << CurrentData.size() << "\t" << bases << "\t" << baseQuals;
    if ( IsPrintingMapQualities ) *OutStream << "\t" << mapQuals;
    *OutStream << endl;
}

bool Pileup::PileupPrivate::Run(void) {
  
    // -----------------------------
    // validate input & output 
    
    if ( !Reader ) {
        cerr << "Pileup::Run() : Invalid multireader" << endl;
        return false;
    }
    
    if ( !OutStream) { 
        cerr << "Pileup::Run() : Invalid output stream" << endl;
        return false;
    }
    
    References = Reader->GetReferenceData();
    
    // -----------------------------
    // process input data
    
    // get first entry    
    BamAlignment al;
    if ( !Reader->GetNextAlignment(al) ) {
        cerr << "Pileup::Run() : Could not read from multireader" << endl;
        return false;
    }
    
    // set initial markers & store first entry
    CurrentId = al.RefID;
    CurrentPosition = al.Position;
    CurrentData.clear();
    CurrentData.push_back(al);
    
    // iterate over remaining data
    while ( Reader->GetNextAlignment(al) ) {
        
        // if same reference
        if ( al.RefID == CurrentId ) {
          
            // if same position, store and move on
            if ( al.Position == CurrentPosition )
                CurrentData.push_back(al);
            
            // if less than CurrentPosition - sorting error => ABORT
            else if ( al.Position < CurrentPosition ) {
                cerr << "Pileup::Run() : Data not sorted correctly!" << endl;
                return false;
            }
            
            // else print pileup data until 'catching up' to CurrentPosition
            else {
                while ( al.Position > CurrentPosition ) {
                    PrintCurrentData();
                    ++CurrentPosition;
                }
                CurrentData.push_back(al);
            }
        } 
        
        // if reference ID less than CurrentID - sorting error => ABORT
        else if ( al.RefID < CurrentId ) {
            cerr << "Pileup::Run() : Data not sorted correctly!" << endl;
            return false;
        }
        
        // else moved forward onto next reference
        else {
            
            // print any remaining pileup data from previous reference
            while ( !CurrentData.empty() ) {
                PrintCurrentData();
                ++CurrentPosition;
            }
            
            // store first entry on this new reference, update markers
            CurrentData.clear();
            CurrentData.push_back(al);
            CurrentId = al.RefID;
            CurrentPosition = al.Position;
        }
    }
    
    // ------------------------------------
    // handle  any remaining data entries
    
    while ( !CurrentData.empty() ) {
        PrintCurrentData();
        ++CurrentPosition;
    }
  
    // -------------------------
    // return success
    
    return true;
}

// ----------------------------------------------------------
// Pileup implementation

Pileup::Pileup(BamMultiReader* reader, ostream* outStream) {
    d = new PileupPrivate(reader, outStream);
}

Pileup::~Pileup(void) {
    delete d;
    d = 0;
}

bool Pileup::Run(void) {
    return d->Run();
}

void Pileup::SetFastaFilename(const string& filename) {
    d->FastaFilename = filename;
}

void Pileup::SetIsPrintingMapQualities(bool ok) {
    d->IsPrintingMapQualities = ok;
}

void Pileup::SetRegion(const BamRegion& region) {
    d->Region = region;
}
