// ***************************************************************************
// SamReadGroup.cpp (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 4 March 2011 (DB)
// ---------------------------------------------------------------------------
// Provides direct read/write access to the SAM read group data fields.
// ***************************************************************************

#include <api/SamReadGroup.h>
using namespace BamTools;
using namespace std;

/*! \struct BamTools::SamReadGroup
    \brief Represents a SAM read group entry.

    Provides direct read/write access to the SAM read group data fields.

    \sa http://samtools.sourceforge.net/SAM-1.3.pdf
*/
/*! \var SamReadGroup::ID
    \brief corresponds to \@RG ID:\<ID\>
*/
/*! \var SamReadGroup::Sample
    \brief corresponds to \@RG SM:\<Sample\>
*/
/*! \var SamReadGroup::Library
    \brief corresponds to \@RG LB:\<Library\>
*/
/*! \var SamReadGroup::Description
    \brief corresponds to \@RG DS:\<Description\>
*/
/*! \var SamReadGroup::PlatformUnit
    \brief corresponds to \@RG PU:\<PlatformUnit\>
*/
/*! \var SamReadGroup::PredictedInsertSize
    \brief corresponds to \@RG PI:\<PredictedInsertSize\>
*/
/*! \var SamReadGroup::SequencingCenter
    \brief corresponds to \@RG CN:\<SequencingCenter\>
*/
/*! \var SamReadGroup::ProductionDate
    \brief corresponds to \@RG DT:\<ProductionDate\>
*/
/*! \var SamReadGroup::SequencingTechnology
    \brief corresponds to \@RG PL:\<SequencingTechnology\>
*/

/*! \fn SamReadGroup::SamReadGroup(void)
    \brief default constructor
*/
SamReadGroup::SamReadGroup(void)
    : ID("")
    , Sample("")
    , Library("")
    , Description("")
    , PlatformUnit("")
    , PredictedInsertSize("")
    , SequencingCenter("")
    , ProductionDate("")
    , SequencingTechnology("")
{ }

/*! \fn SamReadGroup::SamReadGroup(const std::string& id)
    \brief constructs read group with \a id

    \param id desired read group ID
*/
SamReadGroup::SamReadGroup(const std::string& id)
    : ID(id)
    , Sample("")
    , Library("")
    , Description("")
    , PlatformUnit("")
    , PredictedInsertSize("")
    , SequencingCenter("")
    , ProductionDate("")
    , SequencingTechnology("")
{ }

/*! \fn SamReadGroup::SamReadGroup(const SamReadGroup& other)
    \brief copy constructor
*/
SamReadGroup::SamReadGroup(const SamReadGroup& other)
    : ID(other.ID)
    , Sample(other.Sample)
    , Library(other.Library)
    , Description(other.Description)
    , PlatformUnit(other.PlatformUnit)
    , PredictedInsertSize(other.PredictedInsertSize)
    , SequencingCenter(other.SequencingCenter)
    , ProductionDate(other.ProductionDate)
    , SequencingTechnology(other.SequencingTechnology)
{ }

/*! \fn SamReadGroup::~SamReadGroup(void)
    \brief destructor
*/
SamReadGroup::~SamReadGroup(void) { }

/*! \fn void SamReadGroup::Clear(void)
    \brief Clears all data fields.
*/
void SamReadGroup::Clear(void) {
    ID.clear();
    Sample.clear();
    Library.clear();
    Description.clear();
    PlatformUnit.clear();
    PredictedInsertSize.clear();
    SequencingCenter.clear();
    ProductionDate.clear();
    SequencingTechnology.clear();
}

/*! \fn bool SamReadGroup::HasID(void) const
    \brief Returns \c true if read group contains \@RG: ID:\<ID\>
*/
bool SamReadGroup::HasID(void) const {
    return (!ID.empty());
}

/*! \fn bool SamReadGroup::HasSample(void) const
    \brief Returns \c true if read group contains \@RG SM:\<Sample\>
*/
bool SamReadGroup::HasSample(void) const {
    return (!Sample.empty());
}

/*! \fn bool SamReadGroup::HasLibrary(void) const
    \brief Returns \c true if read group contains \@RG LB:\<Library\>
*/
bool SamReadGroup::HasLibrary(void) const {
    return (!Library.empty());
}

/*! \fn bool SamReadGroup::HasDescription(void) const
    \brief Returns \c true if read group contains \@RG DS:\<Description\>
*/
bool SamReadGroup::HasDescription(void) const {
    return (!Description.empty());
}

/*! \fn bool SamReadGroup::HasPlatformUnit(void) const
    \brief Returns \c true if read group contains \@RG PU:\<PlatformUnit\>
*/
bool SamReadGroup::HasPlatformUnit(void) const {
    return (!PlatformUnit.empty());
}

/*! \fn bool SamReadGroup::HasPredictedInsertSize(void) const
    \brief Returns \c true if read group contains \@RG PI:\<PredictedInsertSize\>
*/
bool SamReadGroup::HasPredictedInsertSize(void) const {
    return (!PredictedInsertSize.empty());
}

/*! \fn bool SamReadGroup::HasSequencingCenter(void) const
    \brief Returns \c true if read group contains \@RG CN:\<SequencingCenter\>
*/
bool SamReadGroup::HasSequencingCenter(void) const {
    return (!SequencingCenter.empty());
}

/*! \fn bool SamReadGroup::HasProductionDate(void) const
    \brief Returns \c true if read group contains \@RG DT:\<ProductionDate\>
*/
bool SamReadGroup::HasProductionDate(void) const {
    return (!ProductionDate.empty());
}

/*! \fn bool SamReadGroup::HasSequencingTechnology(void) const
    \brief Returns \c true if read group contains \@RG PL:\<SequencingTechnology\>
*/
bool SamReadGroup::HasSequencingTechnology(void) const {
    return (!SequencingTechnology.empty());
}
