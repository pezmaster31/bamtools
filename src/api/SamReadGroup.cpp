// ***************************************************************************
// SamReadGroup.cpp (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 October 2011 (DB)
// ---------------------------------------------------------------------------
// Provides direct read/write access to the SAM read group data fields.
// ***************************************************************************

#include "api/SamReadGroup.h"
using namespace BamTools;

/*! \struct BamTools::SamReadGroup
    \brief Represents a SAM read group entry.

    Provides direct read/write access to the SAM read group data fields.

    \sa \samSpecURL
*/
/*! \var SamReadGroup::Description
    \brief corresponds to \@RG DS:\<Description\>
*/
/*! \var SamReadGroup::FlowOrder
    \brief corresponds to \@RG FO:\<FlowOrder\>
*/
/*! \var SamReadGroup::ID
    \brief corresponds to \@RG ID:\<ID\>

    Required for valid SAM header.
*/
/*! \var SamReadGroup::KeySequence
    \brief corresponds to \@RG KS:\<KeySequence\>
*/
/*! \var SamReadGroup::Library
    \brief corresponds to \@RG LB:\<Library\>
*/
/*! \var SamReadGroup::PlatformUnit
    \brief corresponds to \@RG PU:\<PlatformUnit\>
*/
/*! \var SamReadGroup::PredictedInsertSize
    \brief corresponds to \@RG PI:\<PredictedInsertSize\>
*/
/*! \var SamReadGroup::ProductionDate
    \brief corresponds to \@RG DT:\<ProductionDate\>
*/
/*! \var SamReadGroup::Program
    \brief corresponds to \@RG PG:\<Program\>
*/
/*! \var SamReadGroup::Sample
    \brief corresponds to \@RG SM:\<Sample\>
*/
/*! \var SamReadGroup::SequencingCenter
    \brief corresponds to \@RG CN:\<SequencingCenter\>
*/
/*! \var SamReadGroup::SequencingTechnology
    \brief corresponds to \@RG PL:\<SequencingTechnology\>
*/

/*! \fn SamReadGroup::SamReadGroup()
    \brief default constructor
*/
SamReadGroup::SamReadGroup() {}

/*! \fn SamReadGroup::SamReadGroup(const std::string& id)
    \brief constructs read group with \a id

    \param id desired read group ID
*/
SamReadGroup::SamReadGroup(const std::string& id)
    : ID(id)
{}

/*! \fn SamReadGroup::SamReadGroup(const SamReadGroup& other)
    \brief copy constructor
*/
SamReadGroup::SamReadGroup(const SamReadGroup& other)
    : Description(other.Description)
    , FlowOrder(other.FlowOrder)
    , ID(other.ID)
    , KeySequence(other.KeySequence)
    , Library(other.Library)
    , PlatformUnit(other.PlatformUnit)
    , PredictedInsertSize(other.PredictedInsertSize)
    , ProductionDate(other.ProductionDate)
    , Program(other.Program)
    , Sample(other.Sample)
    , SequencingCenter(other.SequencingCenter)
    , SequencingTechnology(other.SequencingTechnology)
    , CustomTags(other.CustomTags)
{}

/*! \fn SamReadGroup::~SamReadGroup()
    \brief destructor
*/
SamReadGroup::~SamReadGroup() {}

/*! \fn void SamReadGroup::Clear()
    \brief Clears all data fields.
*/
void SamReadGroup::Clear()
{
    Description.clear();
    FlowOrder.clear();
    ID.clear();
    KeySequence.clear();
    Library.clear();
    PlatformUnit.clear();
    PredictedInsertSize.clear();
    ProductionDate.clear();
    Program.clear();
    Sample.clear();
    SequencingCenter.clear();
    SequencingTechnology.clear();
    CustomTags.clear();
}

/*! \fn bool SamReadGroup::HasDescription() const
    \brief Returns \c true if read group contains \@RG DS:\<Description\>
*/
bool SamReadGroup::HasDescription() const
{
    return (!Description.empty());
}

/*! \fn bool SamReadGroup::HasFlowOrder() const
    \brief Returns \c true if read group contains \@RG FO:\<FlowOrder\>
*/
bool SamReadGroup::HasFlowOrder() const
{
    return (!FlowOrder.empty());
}

/*! \fn bool SamReadGroup::HasID() const
    \brief Returns \c true if read group contains \@RG: ID:\<ID\>
*/
bool SamReadGroup::HasID() const
{
    return (!ID.empty());
}

/*! \fn bool SamReadGroup::HasKeySequence() const
    \brief Returns \c true if read group contains \@RG KS:\<KeySequence\>
*/
bool SamReadGroup::HasKeySequence() const
{
    return (!KeySequence.empty());
}

/*! \fn bool SamReadGroup::HasLibrary() const
    \brief Returns \c true if read group contains \@RG LB:\<Library\>
*/
bool SamReadGroup::HasLibrary() const
{
    return (!Library.empty());
}

/*! \fn bool SamReadGroup::HasPlatformUnit() const
    \brief Returns \c true if read group contains \@RG PU:\<PlatformUnit\>
*/
bool SamReadGroup::HasPlatformUnit() const
{
    return (!PlatformUnit.empty());
}

/*! \fn bool SamReadGroup::HasPredictedInsertSize() const
    \brief Returns \c true if read group contains \@RG PI:\<PredictedInsertSize\>
*/
bool SamReadGroup::HasPredictedInsertSize() const
{
    return (!PredictedInsertSize.empty());
}

/*! \fn bool SamReadGroup::HasProductionDate() const
    \brief Returns \c true if read group contains \@RG DT:\<ProductionDate\>
*/
bool SamReadGroup::HasProductionDate() const
{
    return (!ProductionDate.empty());
}

/*! \fn bool SamReadGroup::HasProgram() const
    \brief Returns \c true if read group contains \@RG PG:\<Program\>
*/
bool SamReadGroup::HasProgram() const
{
    return (!Program.empty());
}

/*! \fn bool SamReadGroup::HasSample() const
    \brief Returns \c true if read group contains \@RG SM:\<Sample\>
*/
bool SamReadGroup::HasSample() const
{
    return (!Sample.empty());
}

/*! \fn bool SamReadGroup::HasSequencingCenter() const
    \brief Returns \c true if read group contains \@RG CN:\<SequencingCenter\>
*/
bool SamReadGroup::HasSequencingCenter() const
{
    return (!SequencingCenter.empty());
}

/*! \fn bool SamReadGroup::HasSequencingTechnology() const
    \brief Returns \c true if read group contains \@RG PL:\<SequencingTechnology\>
*/
bool SamReadGroup::HasSequencingTechnology() const
{
    return (!SequencingTechnology.empty());
}
