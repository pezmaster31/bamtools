// ***************************************************************************
// SamHeader.h (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 12 January 2011 (DB)
// ---------------------------------------------------------------------------
// Provides functionality for querying/manipulating SAM header data
// **************************************************************************

#ifndef SAM_HEADER_H
#define SAM_HEADER_H

#include <api/api_global.h>
#include <api/SamReadGroupDictionary.h>
#include <api/SamSequenceDictionary.h>
#include <string>
#include <vector>

namespace BamTools {

struct API_EXPORT SamHeader {

    // ctor & dtor
    public:
        SamHeader(const std::string& headerText = "");
        SamHeader(const SamHeader& other);
        ~SamHeader(void);

    // query/modify entire SamHeader at once
    public:

        // clear all header contents
        void Clear(void);

        // checks if SAM header is well-formed
        // @verbose - if true, validation errors & warnings will be printed to stderr
        // otherwise, output is suppressed and only validation check occurs
        bool IsValid(bool verbose = false) const;

        // replaces SamHeader contents with headerText
        void SetHeaderText(const std::string& headerText);

        // retrieves the printable, SAM-formatted header
        // (with any local modifications since construction)
        std::string ToString(void) const;

    // query if header contains data elements
    public:
        bool HasVersion(void) const;
        bool HasSortOrder(void) const;
        bool HasGroupOrder(void) const;
        bool HasSequences(void) const;
        bool HasReadGroups(void) const;
        bool HasProgramName(void) const;
        bool HasProgramVersion(void) const;
        bool HasProgramCommandLine(void) const;
        bool HasComments(void) const;

    // data members
    public:

        // header metadata (@HD line)
        std::string Version;                // VN:<Version>
        std::string SortOrder;              // SO:<SortOrder>
        std::string GroupOrder;             // GO:<GroupOrder>

        // header sequences (@SQ entries)
        SamSequenceDictionary Sequences;

        // header read groups (@RG entries)
        SamReadGroupDictionary ReadGroups;

        // header program data (@PG entries)
        std::string ProgramName;            // ID:<ProgramName>
        std::string ProgramVersion;         // VN:<ProgramVersion>
        std::string ProgramCommandLine;     // CL:<ProgramCommandLine>

        // header comments (@CO entries)
        std::vector<std::string> Comments;
};

} // namespace BamTools

#endif // SAM_HEADER_H
