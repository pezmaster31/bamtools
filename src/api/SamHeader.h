// ***************************************************************************
// SamHeader.h (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 October 2011 (DB)
// ---------------------------------------------------------------------------
// Provides direct read/write access to the SAM header data fields.
// ***************************************************************************

#ifndef SAM_HEADER_H
#define SAM_HEADER_H

#include <string>
#include <vector>
#include "api/BamAux.h"
#include "api/SamProgramChain.h"
#include "api/SamReadGroupDictionary.h"
#include "api/SamSequenceDictionary.h"
#include "api/api_global.h"

namespace BamTools {

struct API_EXPORT SamHeader
{

    // ctor & dtor
    SamHeader(const std::string& headerText = std::string());

    // query/modify entire SamHeader
    void Clear();  // clears all header contents
    std::string GetErrorString() const;
    bool HasError() const;
    bool IsValid(bool verbose = false) const;  // returns true if SAM header is well-formed
    void SetHeaderText(
        const std::string& headerText);  // replaces data fields with contents of SAM-formatted text
    std::string ToString() const;        // returns the printable, SAM-formatted header text

    // convenience query methods
    bool HasVersion() const;     // returns true if header contains format version entry
    bool HasSortOrder() const;   // returns true if header contains sort order entry
    bool HasGroupOrder() const;  // returns true if header contains group order entry
    bool HasSequences() const;   // returns true if header contains any sequence entries
    bool HasReadGroups() const;  // returns true if header contains any read group entries
    bool HasPrograms() const;    // returns true if header contains any program record entries
    bool HasComments() const;    // returns true if header contains comments

    // --------------
    // data members
    // --------------

    // header metadata (@HD line)
    std::string Version;                      // VN:<Version>  *Required, if @HD record is present*
    std::string SortOrder;                    // SO:<SortOrder>
    std::string GroupOrder;                   // GO:<GroupOrder>
    std::vector<CustomHeaderTag> CustomTags;  // optional custom tags on @HD line

    // header sequences (@SQ entries)
    SamSequenceDictionary Sequences;

    // header read groups (@RG entries)
    SamReadGroupDictionary ReadGroups;

    // header program data (@PG entries)
    SamProgramChain Programs;

    // header comments (@CO entries)
    std::vector<std::string> Comments;

    // internal data
private:
    mutable std::string m_errorString;
};

}  // namespace BamTools

#endif  // SAM_HEADER_H
