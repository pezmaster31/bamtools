// ***************************************************************************
// SamProgram.h (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 October 2011 (DB)
// ---------------------------------------------------------------------------
// Provides direct read/write access to the SAM header program records.
// ***************************************************************************

#ifndef SAM_PROGRAM_H
#define SAM_PROGRAM_H

#include <string>
#include "api/BamAux.h"
#include "api/api_global.h"

namespace BamTools {

class SamProgramChain;

struct API_EXPORT SamProgram
{

    // ctor & dtor
    SamProgram();
    SamProgram(const std::string& id);

    // query/modify entire program record
    void Clear();  // clears all data fields

    // convenience query methods
    bool HasCommandLine() const;  // returns true if program record has a command line entry
    bool HasID() const;           // returns true if program record has an ID
    bool HasName() const;         // returns true if program record has a name
    bool HasPreviousProgramID()
        const;                // returns true if program record has a 'previous program ID'
    bool HasVersion() const;  // returns true if program record has a version

    // data members
    std::string CommandLine;                  // CL:<CommandLine>
    std::string ID;                           // ID:<ID>          *Required for valid SAM header*
    std::string Name;                         // PN:<Name>
    std::string PreviousProgramID;            // PP:<PreviousProgramID>
    std::string Version;                      // VN:<Version>
    std::vector<CustomHeaderTag> CustomTags;  // optional custom tags

    // internal (non-standard) methods & fields
private:
    bool HasNextProgramID() const;
    std::string NextProgramID;
    friend class BamTools::SamProgramChain;
};

/*! \fn bool operator==(const SamProgram& lhs, const SamProgram& rhs)
    \brief tests equality by comparing program IDs
*/
inline bool operator==(const SamProgram& lhs, const SamProgram& rhs)
{
    return lhs.ID == rhs.ID;
}

}  // namespace BamTools

#endif  // SAM_PROGRAM_H
