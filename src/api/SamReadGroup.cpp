// ***************************************************************************
// SamReadGroup.cpp (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 23 December 2010 (DB)
// ---------------------------------------------------------------------------
// Provides functionality for querying/manipulating read group data
// **************************************************************************

#include <api/SamReadGroup.h>
using namespace BamTools;
using namespace std;

// default ctor
SamReadGroup::SamReadGroup(void)
    : ID("")
    , Sample("")
    , Library("")
    , Description("")
    , PlatformUnit("")
    , PredictedInsertSize("")
    , SequencingCenter("")
    , ProductionDate("")
    , SequencingTechnology("")
{ }

// ctor with provided ID
SamReadGroup::SamReadGroup(const string& id)
    : ID(id)
    , Sample("")
    , Library("")
    , Description("")
    , PlatformUnit("")
    , PredictedInsertSize("")
    , SequencingCenter("")
    , ProductionDate("")
    , SequencingTechnology("")
{ }

// copy ctor
SamReadGroup::SamReadGroup(const SamReadGroup& other)
    : ID(other.ID)
    , Sample(other.Sample)
    , Library(other.Library)
    , Description(other.Description)
    , PlatformUnit(other.PlatformUnit)
    , PredictedInsertSize(other.PredictedInsertSize)
    , SequencingCenter(other.SequencingCenter)
    , ProductionDate(other.ProductionDate)
    , SequencingTechnology(other.SequencingTechnology)
{ }

// dtor
SamReadGroup::~SamReadGroup(void) {
    Clear();
}

// clear all contents
void SamReadGroup::Clear(void) {
    ID.clear();
    Sample.clear();
    Library.clear();
    Description.clear();
    PlatformUnit.clear();
    PredictedInsertSize.clear();
    SequencingCenter.clear();
    ProductionDate.clear();
    SequencingTechnology.clear();
}

// convenience methods to check if SamReadGroup contains these values:
bool SamReadGroup::HasID(void) const                   { return (!ID.empty());                   }
bool SamReadGroup::HasSample(void) const               { return (!Sample.empty());               }
bool SamReadGroup::HasLibrary(void) const              { return (!Library.empty());              }
bool SamReadGroup::HasDescription(void) const          { return (!Description.empty());          }
bool SamReadGroup::HasPlatformUnit(void) const         { return (!PlatformUnit.empty());         }
bool SamReadGroup::HasPredictedInsertSize(void) const  { return (!PredictedInsertSize.empty());  }
bool SamReadGroup::HasSequencingCenter(void) const     { return (!SequencingCenter.empty());     }
bool SamReadGroup::HasProductionDate(void) const       { return (!ProductionDate.empty());       }
bool SamReadGroup::HasSequencingTechnology(void) const { return (!SequencingTechnology.empty()); }
