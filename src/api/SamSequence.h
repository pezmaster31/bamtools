// ***************************************************************************
// SamSequence.h (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 October 2011 (DB)
// ---------------------------------------------------------------------------
// Provides direct read/write access to the SAM sequence data fields.
// ***************************************************************************

#ifndef SAM_SEQUENCE_H
#define SAM_SEQUENCE_H

#include <string>
#include <vector>
#include "api/BamAux.h"
#include "api/api_global.h"

namespace BamTools {

struct API_EXPORT SamSequence
{

    // ctor & dtor
    SamSequence();
    SamSequence(const std::string& name, const int& length);
    SamSequence(const std::string& name, const std::string& length);

    // query/modify entire sequence
    void Clear();  // clears all contents

    // convenience query methods
    bool HasAssemblyID() const;  // returns true if sequence has an assembly ID
    bool HasChecksum() const;    // returns true if sequence has an MD5 checksum
    bool HasLength() const;      // returns true if sequence has a length
    bool HasName() const;        // returns true if sequence has a name
    bool HasSpecies() const;     // returns true if sequence has a species ID
    bool HasURI() const;         // returns true if sequence has a URI

    // data members
    std::string AssemblyID;                   // AS:<AssemblyID>
    std::string Checksum;                     // M5:<Checksum>
    std::string Length;                       // LN:<Length>      *Required for valid SAM header*
    std::string Name;                         // SN:<Name>        *Required for valid SAM header*
    std::string Species;                      // SP:<Species>
    std::string URI;                          // UR:<URI>
    std::vector<CustomHeaderTag> CustomTags;  // optional custom tags
};

/*! \fn bool operator==(const SamSequence& lhs, const SamSequence& rhs)
    \brief tests equality by comparing sequence names, lengths, & checksums (if available)
*/
inline bool operator==(const SamSequence& lhs, const SamSequence& rhs)
{
    if (lhs.Name != rhs.Name) {
        return false;
    }
    if (lhs.Length != rhs.Length) {
        return false;
    }
    if (lhs.HasChecksum() && rhs.HasChecksum()) {
        return (lhs.Checksum == rhs.Checksum);
    } else {
        return true;
    }
}

}  // namespace BamTools

#endif  // SAM_SEQUENCE_H
