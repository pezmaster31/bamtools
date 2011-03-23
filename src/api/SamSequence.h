// ***************************************************************************
// SamSequence.h (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 4 March 2011 (DB)
// ---------------------------------------------------------------------------
// Provides direct read/write access to the SAM sequence data fields.
// ***************************************************************************

#ifndef SAM_SEQUENCE_H
#define SAM_SEQUENCE_H

#include <api/api_global.h>
#include <string>

namespace BamTools {

struct API_EXPORT SamSequence {

    // ctor & dtor
    SamSequence(void);
    SamSequence(const std::string& name, const int& length);
    SamSequence(const SamSequence& other);
    ~SamSequence(void);

    // query/modify entire sequence
    void Clear(void);                           // clears all contents

    // convenience query methods
    bool HasName(void) const;                   // returns true if sequence has a name
    bool HasLength(void) const;                 // returns true if sequence has a length
    bool HasAssemblyID(void) const;             // returns true if sequence has an assembly ID
    bool HasChecksum(void) const;               // returns true if sequence has an MD5 checksum
    bool HasURI(void) const;                    // returns true if sequence has a URI
    bool HasSpecies(void) const;                // returns true if sequence has a species ID

    // data members
    std::string Name;       // SN:<Name>
    std::string Length;     // LN:<Length>
    std::string AssemblyID; // AS:<AssemblyID>
    std::string Checksum;   // M5:<Checksum>
    std::string URI;        // UR:<URI>
    std::string Species;    // SP:<Species>
};

/*! \fn bool operator==(const SamSequence& lhs, const SamSequence& rhs)
    \brief tests equality by comparing sequence names, lengths, & checksums (if available)
*/
API_EXPORT inline bool operator==(const SamSequence& lhs, const SamSequence& rhs) {
    if ( lhs.Name   != rhs.Name   ) return false;
    if ( lhs.Length != rhs.Length ) return false;
    if ( lhs.HasChecksum() && rhs.HasChecksum() )
        return (lhs.Checksum == rhs.Checksum);
    else return true;
}

} // namespace BamTools

#endif // SAM_SEQUENCE_H
