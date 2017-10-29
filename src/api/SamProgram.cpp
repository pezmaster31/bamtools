// ***************************************************************************
// SamProgram.cpp (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 October 2011 (DB)
// ---------------------------------------------------------------------------
// Provides direct read/write access to the SAM header program records.
// ***************************************************************************

#include "api/SamProgram.h"
using namespace BamTools;

/*! \struct BamTools::SamProgram
    \brief Represents a SAM program record.

    Provides direct read/write access to the SAM header program records.

    \sa \samSpecURL
*/
/*! \var SamProgram::CommandLine
    \brief corresponds to \@PG CL:\<CommandLine\>
*/
/*! \var SamProgram::ID
    \brief corresponds to \@PG ID:\<ID\>

    Required for valid SAM header.
*/
/*! \var SamProgram::Name
    \brief corresponds to \@PG PN:\<Name\>
*/
/*! \var SamProgram::PreviousProgramID
    \brief corresponds to \@PG PP:\<PreviousProgramID\>
*/
/*! \var SamProgram::Version
    \brief corresponds to \@PG VN:\<Version\>
*/
/*! \var SamProgram::NextProgramID
    \internal
    Holds ID of the "next" program record in a SamProgramChain
*/

/*! \fn SamProgram::SamProgram()
    \brief default constructor
*/
SamProgram::SamProgram()
    : CommandLine("")
    , ID("")
    , Name("")
    , PreviousProgramID("")
    , Version("")
    , NextProgramID("")
{}

/*! \fn SamProgram::SamProgram(const std::string& id)
    \brief constructs program record with \a id

    \param id desired program record ID
*/
SamProgram::SamProgram(const std::string& id)
    : CommandLine("")
    , ID(id)
    , Name("")
    , PreviousProgramID("")
    , Version("")
    , NextProgramID("")
{}

/*! \fn SamProgram::SamProgram(const SamProgram& other)
    \brief copy constructor
*/
SamProgram::SamProgram(const SamProgram& other)
    : CommandLine(other.CommandLine)
    , ID(other.ID)
    , Name(other.Name)
    , PreviousProgramID(other.PreviousProgramID)
    , Version(other.Version)
    , CustomTags(other.CustomTags)
    , NextProgramID(other.NextProgramID)
{}

/*! \fn SamProgram::~SamProgram()
    \brief destructor
*/
SamProgram::~SamProgram() {}

/*! \fn void SamProgram::Clear()
    \brief Clears all data fields.
*/
void SamProgram::Clear()
{
    CommandLine.clear();
    ID.clear();
    Name.clear();
    PreviousProgramID.clear();
    Version.clear();
    NextProgramID.clear();
}

/*! \fn bool SamProgram::HasCommandLine() const
    \brief Returns \c true if program record contains \@PG: CL:\<CommandLine\>
*/
bool SamProgram::HasCommandLine() const
{
    return (!CommandLine.empty());
}

/*! \fn bool SamProgram::HasID() const
    \brief Returns \c true if program record contains \@PG: ID:\<ID\>
*/
bool SamProgram::HasID() const
{
    return (!ID.empty());
}

/*! \fn bool SamProgram::HasName() const
    \brief Returns \c true if program record contains \@PG: PN:\<Name\>
*/
bool SamProgram::HasName() const
{
    return (!Name.empty());
}

/*! \fn bool SamProgram::HasNextProgramID() const
    \internal
    \return true if program has a "next" record in a SamProgramChain
*/
bool SamProgram::HasNextProgramID() const
{
    return (!NextProgramID.empty());
}

/*! \fn bool SamProgram::HasPreviousProgramID() const
    \brief Returns \c true if program record contains \@PG: PP:\<PreviousProgramID\>
*/
bool SamProgram::HasPreviousProgramID() const
{
    return (!PreviousProgramID.empty());
}

/*! \fn bool SamProgram::HasVersion() const
    \brief Returns \c true if program record contains \@PG: VN:\<Version\>
*/
bool SamProgram::HasVersion() const
{
    return (!Version.empty());
}
