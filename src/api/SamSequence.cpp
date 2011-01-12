// ***************************************************************************
// SamSequence.cpp (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 23 December 2010 (DB)
// ---------------------------------------------------------------------------
// Provides functionality for querying/manipulating sequence data
// *************************************************************************

#include <api/SamSequence.h>
using namespace BamTools;
using namespace std;

// ctor
SamSequence::SamSequence(const string& name)
    : Name(name)
    , Length("")
    , AssemblyID("")
    , Checksum("")
    , URI("")
    , Species("")
{ }

// copy ctor
SamSequence::SamSequence(const SamSequence& other)
    : Name(other.Name)
    , Length(other.Length)
    , AssemblyID(other.AssemblyID)
    , Checksum(other.Checksum)
    , URI(other.URI)
    , Species(other.Species)
{ }

// dtor
SamSequence::~SamSequence(void) {
    Clear();
}

// clear all contents
void SamSequence::Clear(void) {
    Name.clear();
    Length.clear();
    AssemblyID.clear();
    Checksum.clear();
    URI.clear();
    Species.clear();
}

// convenience methods to check if SamSequence contains these values:
bool SamSequence::HasName(void) const       { return (!Name.empty());       }
bool SamSequence::HasLength(void) const     { return (!Length.empty());     }
bool SamSequence::HasAssemblyID(void) const { return (!AssemblyID.empty()); }
bool SamSequence::HasChecksum(void) const   { return (!Checksum.empty());   }
bool SamSequence::HasURI(void) const        { return (!URI.empty());        }
bool SamSequence::HasSpecies(void) const    { return (!Species.empty());    }
