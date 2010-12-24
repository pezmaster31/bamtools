// ***************************************************************************
// SamHeader.cpp (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 23 December 2010 (DB)
// ---------------------------------------------------------------------------
// Provides functionality for querying/manipulating SAM header data
// **************************************************************************

#include <api/SamHeader.h>
#include <api/internal/SamFormatParser_p.h>
#include <api/internal/SamFormatPrinter_p.h>
#include <api/internal/SamHeaderValidator_p.h>
using namespace BamTools;
using namespace BamTools::Internal;
using namespace std;

SamHeader::SamHeader(const string& headerText)
    : Version("")
    , SortOrder("")
    , GroupOrder("")
    , ProgramName("")
    , ProgramVersion("")
    , ProgramCommandLine("")
{
    SamFormatParser parser(*this);
    parser.Parse(headerText);
}

SamHeader::~SamHeader(void) {
    Clear();
}

void SamHeader::Clear(void) {
    Version.clear();
    SortOrder.clear();
    GroupOrder.clear();
    Sequences.Clear();
    ReadGroups.Clear();
    ProgramName.clear();
    ProgramVersion.clear();
    ProgramCommandLine.clear();
    Comments.clear();
}

// retrieve the SAM header, with any local modifications
string SamHeader::ToString(void) const {
    SamFormatPrinter printer(*this);
    return printer.ToString();
}

// query if header contains @HD ID:<Version>
bool SamHeader::HasVersion(void) const {
    return (!Version.empty());
}

// query if header contains @HD SO:<SortOrder>
bool SamHeader::HasSortOrder(void) const {
    return (!SortOrder.empty());
}

// query if header contains @HD GO:<GroupOrder>
bool SamHeader::HasGroupOrder(void) const {
    return (!GroupOrder.empty());
}

// query if header contains @SQ entries
bool SamHeader::HasSequences(void) const {
    return (!Sequences.IsEmpty());
}

// query if header contains @RG entries
bool SamHeader::HasReadGroups(void) const {
    return (!ReadGroups.IsEmpty());
}

// query if header contains @PG ID:<ProgramName>
bool SamHeader::HasProgramName(void) const {
    return (!ProgramName.empty());
}

// query if header contains @HD VN:<ProgramVersion>
bool SamHeader::HasProgramVersion(void) const {
    return (!ProgramVersion.empty());
}

// query if header contains @HD CL:<ProgramCommandLine>
bool SamHeader::HasProgramCommandLine(void) const {
    return (!ProgramCommandLine.empty());
}

// query if header contains @CO entries
bool SamHeader::HasComments(void) const {
    return (!Comments.empty());
}

// validation
bool SamHeader::IsValid(bool verbose) const {
    SamHeaderValidator validator(*this);
    return validator.Validate(verbose);
}
