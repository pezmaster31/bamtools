// ***************************************************************************
// SamHeader.cpp (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 4 March 2011 (DB)
// ---------------------------------------------------------------------------
// Provides direct read/write access to the SAM header data fields.
// ***************************************************************************

#include <api/SamHeader.h>
#include <api/internal/SamFormatParser_p.h>
#include <api/internal/SamFormatPrinter_p.h>
#include <api/internal/SamHeaderValidator_p.h>
using namespace BamTools;
using namespace BamTools::Internal;
using namespace std;

/*! \struct BamTools::SamHeader
    \brief Represents the SAM-formatted text header that is part of the BAM file header.

    Provides direct read/write access to the SAM header data fields.

    \sa http://samtools.sourceforge.net/SAM-1.3.pdf
*/
/*! \var SamHeader::Version
    \brief corresponds to \@HD VN:\<Version\>
*/
/*! \var SamHeader::SortOrder
    \brief corresponds to \@HD SO:\<SortOrder\>
*/
/*! \var SamHeader::GroupOrder
    \brief corresponds to \@HD GO:\<GroupOrder\>
*/
/*! \var SamHeader::Sequences
    \brief corresponds to \@SQ entries
    \sa SamSequence, SamSequenceDictionary
*/
/*! \var SamHeader::ReadGroups
    \brief corresponds to \@RG entries
    \sa SamReadGroup, SamReadGroupDictionary
*/
/*! \var SamHeader::ProgramName
    \brief corresponds to \@PG ID:\<ProgramName\>
*/
/*! \var SamHeader::ProgramVersion
    \brief corresponds to \@PG VN:\<ProgramVersion\>
*/
/*! \var SamHeader::ProgramCommandLine
    \brief corresponds to \@PG CL:\<ProgramCommandLine\>
*/
/*! \var SamHeader::Comments
    \brief corresponds to \@CO entries
*/

/*! \fn SamHeader::SamHeader(const std::string& headerText = "")
    \brief constructor
*/
SamHeader::SamHeader(const std::string& headerText)
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

/*! \fn SamHeader::SamHeader(const SamHeader& other)
    \brief copy constructor
*/
SamHeader::SamHeader(const SamHeader& other)
    : Version(other.Version)
    , SortOrder(other.SortOrder)
    , GroupOrder(other.GroupOrder)
    , Sequences(other.Sequences)
    , ReadGroups(other.ReadGroups)
    , ProgramName(other.ProgramName)
    , ProgramVersion(other.ProgramVersion)
    , ProgramCommandLine(other.ProgramCommandLine)
{ }

/*! \fn SamHeader::~SamHeader(void)
    \brief destructor
*/
SamHeader::~SamHeader(void) { }

/*! \fn void SamHeader::Clear(void)
    \brief Clears all header contents.
*/
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

/*! \fn bool SamHeader::HasVersion(void) const
    \brief Returns \c true if header contains \@HD ID:\<Version\>
*/
bool SamHeader::HasVersion(void) const {
    return (!Version.empty());
}

/*! \fn bool SamHeader::HasSortOrder(void) const
    \brief Returns \c true if header contains \@HD SO:\<SortOrder\>
*/
bool SamHeader::HasSortOrder(void) const {
    return (!SortOrder.empty());
}

/*! \fn bool SamHeader::HasGroupOrder(void) const
    \brief Returns \c true if header contains \@HD GO:\<GroupOrder\>
*/
bool SamHeader::HasGroupOrder(void) const {
    return (!GroupOrder.empty());
}

/*! \fn bool SamHeader::HasSequences(void) const
    \brief Returns \c true if header contains any \@SQ entries
*/
bool SamHeader::HasSequences(void) const {
    return (!Sequences.IsEmpty());
}

/*! \fn bool SamHeader::HasReadGroups(void) const
    \brief Returns \c true if header contains any \@RG entries
*/
bool SamHeader::HasReadGroups(void) const {
    return (!ReadGroups.IsEmpty());
}

/*! \fn bool SamHeader::HasProgramName(void) const
    \brief Returns \c true if header contains \@PG ID:\<ProgramName\>
*/
bool SamHeader::HasProgramName(void) const {
    return (!ProgramName.empty());
}

/*! \fn bool SamHeader::HasProgramVersion(void) const
    \brief Returns \c true if header contains \@PG VN:\<ProgramVersion\>
*/
bool SamHeader::HasProgramVersion(void) const {
    return (!ProgramVersion.empty());
}

/*! \fn bool SamHeader::HasProgramCommandLine(void) const
    \brief Returns \c true if header contains \@PG CL:\<ProgramCommandLine\>
*/
bool SamHeader::HasProgramCommandLine(void) const {
    return (!ProgramCommandLine.empty());
}

/*! \fn bool SamHeader::HasComments(void) const
    \brief Returns \c true if header contains any \@CO entries
*/
bool SamHeader::HasComments(void) const {
    return (!Comments.empty());
}

/*! \fn bool SamHeader::IsValid(bool verbose = false) const
    \brief Checks header contents for required data and proper formatting.
    \param verbose If set to true, validation errors & warnings will be printed to stderr.
                   Otherwise, output is suppressed and only validation check occurs.
    \return \c true if SAM header is well-formed
*/
bool SamHeader::IsValid(bool verbose) const {
    SamHeaderValidator validator(*this);
    return validator.Validate(verbose);
}

/*! \fn void SamHeader::SetHeaderText(const std::string& headerText)
    \brief Replaces header contents with \a headerText.
    \param headerText SAM formatted-text that will be parsed into data fields
*/
void SamHeader::SetHeaderText(const std::string& headerText) {

    // clear prior data
    Clear();

    // parse header text into data
    SamFormatParser parser(*this);
    parser.Parse(headerText);
}

/*! \fn std::string SamHeader::ToString(void) const
    \brief Converts data fields to SAM-formatted text.

    Applies any local modifications made since creating this object or calling SetHeaderText().

    \return SAM-formatted header text
*/
string SamHeader::ToString(void) const {
    SamFormatPrinter printer(*this);
    return printer.ToString();
}
