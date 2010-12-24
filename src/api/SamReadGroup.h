// ***************************************************************************
// SamReadGroup.h (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 23 December 2010 (DB)
// ---------------------------------------------------------------------------
// Provides functionality for querying/manipulating read group data
// **************************************************************************

#ifndef SAM_READGROUP_H
#define SAM_READGROUP_H

#include "api/api_global.h"
#include <string>

namespace BamTools {

class API_EXPORT SamReadGroup {

    // ctor & dtor
    public:
        SamReadGroup(void);
        SamReadGroup(const std::string& id);
        ~SamReadGroup(void);

    // public methods
    public:

        // clear all contents
        void Clear(void);

        // convenience methods to check if SamReadGroup contains these values:
        bool HasID(void) const;
        bool HasSample(void) const;
        bool HasLibrary(void) const;
        bool HasDescription(void) const;
        bool HasPlatformUnit(void) const;
        bool HasPredictedInsertSize(void) const;
        bool HasSequencingCenter(void) const;
        bool HasProductionDate(void) const;
        bool HasSequencingTechnology(void) const;

    // data members
    public:
        std::string ID;                   // ID:<ID>
        std::string Sample;               // SM:<Sample>
        std::string Library;              // LB:<Library>
        std::string Description;          // DS:<Description>
        std::string PlatformUnit;         // PU:<PlatformUnit>
        std::string PredictedInsertSize;  // PI:<PredictedInsertSize>
        std::string SequencingCenter;     // CN:<SequencingCenter>
        std::string ProductionDate;       // DT:<ProductionDate>
        std::string SequencingTechnology; // PL:<SequencingTechnology>
};

// ---------------------------------------------------
// comparison operators

// for equality: compare IDs
inline bool operator==(const SamReadGroup& lhs, const SamReadGroup& rhs) {
    return lhs.ID == rhs.ID;
}

} // namespace BamTools

#endif // SAM_READGROUP_H
