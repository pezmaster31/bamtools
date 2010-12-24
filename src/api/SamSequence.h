// ***************************************************************************
// SamSequence.h (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 23 December 2010 (DB)
// ---------------------------------------------------------------------------
// Provides functionality for querying/manipulating sequence data
// **************************************************************************

#ifndef SAM_SEQUENCE_H
#define SAM_SEQUENCE_H

#include <api/api_global.h>
#include <string>

namespace BamTools {

class API_EXPORT SamSequence {

    // ctor & dtor
    public:
        SamSequence(const std::string& name = "");
        ~SamSequence(void);

    // public methods
    public:

        // clear all contents
        void Clear(void);

        // convenience methods to check if SamSequence contains these values:
        bool HasName(void) const;
        bool HasLength(void) const;
        bool HasAssemblyID(void) const;
        bool HasChecksum(void) const;
        bool HasURI(void) const;
        bool HasSpecies(void) const;

    // data members
    public:
        std::string Name;       // SN:<Name>
        std::string Length;     // LN:<Length>
        std::string AssemblyID; // AS:<AssemblyID>
        std::string Checksum;   // M5:<Checksum>
        std::string URI;        // UR:<URI>
        std::string Species;    // SP:<Species>
};

// ---------------------------------------------------
// comparison operators

// for equality: compare Name, Length, & Checksum (if it exists for both)
inline bool operator==(const SamSequence& lhs, const SamSequence& rhs) {
    if ( lhs.Name   != rhs.Name   ) return false;
    if ( lhs.Length != rhs.Length ) return false;
    if ( lhs.HasChecksum() && rhs.HasChecksum() )
        return (lhs.Checksum == rhs.Checksum);
    else return true;
}

} // namespace BamTools

#endif // SAM_SEQUENCE_H
