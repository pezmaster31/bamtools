// ***************************************************************************
// SamReadGroup.h (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 October 2011 (DB)
// ---------------------------------------------------------------------------
// Provides direct read/write access to the SAM read group data fields.
// ***************************************************************************

#ifndef SAM_READGROUP_H
#define SAM_READGROUP_H

#include <string>
#include <vector>
#include "api/BamAux.h"
#include "api/api_global.h"

namespace BamTools {

struct API_EXPORT SamReadGroup
{

    // ctor & dtor
    SamReadGroup();
    SamReadGroup(const std::string& id);
    SamReadGroup(const SamReadGroup& other);
    ~SamReadGroup();

    // query/modify entire read group
    void Clear();  // clears all data fields

    // convenience query methods
    bool HasDescription() const;          // returns true if read group has a description
    bool HasFlowOrder() const;            // returns true if read group has a flow order entry
    bool HasID() const;                   // returns true if read group has a group ID
    bool HasKeySequence() const;          // returns true if read group has a key sequence
    bool HasLibrary() const;              // returns true if read group has a library name
    bool HasPlatformUnit() const;         // returns true if read group has a platform unit ID
    bool HasPredictedInsertSize() const;  // returns true if read group has a predicted insert size
    bool HasProductionDate() const;       // returns true if read group has a production date
    bool HasProgram() const;              // returns true if read group has a program entry
    bool HasSample() const;               // returns true if read group has a sample name
    bool HasSequencingCenter() const;     // returns true if read group has a sequencing center ID
    bool HasSequencingTechnology()
        const;  // returns true if read group has a sequencing technology ID

    // data fields
    std::string Description;           // DS:<Description>
    std::string FlowOrder;             // FO:<FlowOrder>
    std::string ID;                    // ID:<ID>              *Required for valid SAM header*
    std::string KeySequence;           // KS:<KeySequence>
    std::string Library;               // LB:<Library>
    std::string PlatformUnit;          // PU:<PlatformUnit>
    std::string PredictedInsertSize;   // PI:<PredictedInsertSize>
    std::string ProductionDate;        // DT:<ProductionDate>
    std::string Program;               // PG:<Program>
    std::string Sample;                // SM:<Sample>
    std::string SequencingCenter;      // CN:<SequencingCenter>
    std::string SequencingTechnology;  // PL:<SequencingTechnology>
    std::vector<CustomHeaderTag> CustomTags;  // optional custom tags
};

/*! \fn bool operator==(const SamReadGroup& lhs, const SamReadGroup& rhs)
    \brief tests equality by comparing read group IDs
*/
API_EXPORT inline bool operator==(const SamReadGroup& lhs, const SamReadGroup& rhs)
{
    return lhs.ID == rhs.ID;
}

}  // namespace BamTools

#endif  // SAM_READGROUP_H
