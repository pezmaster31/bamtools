// ***************************************************************************
// SamHeaderValidator.h (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 23 December 2010 (DB)
// ---------------------------------------------------------------------------
// Provides functionality for validating SamHeader data
// ***************************************************************************

#ifndef SAM_HEADER_VALIDATOR_P_H
#define SAM_HEADER_VALIDATOR_P_H

//  -------------
//  W A R N I N G
//  -------------
//
// This file is not part of the BamTools API.  It exists purely as an
// implementation detail. This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.

#include <string>
#include <vector>

namespace BamTools {

class SamHeader;

namespace Internal {

class SamHeaderValidator {

    public:
        SamHeaderValidator(const BamTools::SamHeader& header);
        ~SamHeaderValidator(void);

    public:
        // validates SamHeader data
        // prints error & warning messages to stderr when (verbose == true)
        bool Validate(bool verbose = false);

    private:
        struct SamHeaderValidatorPrivate;
        SamHeaderValidatorPrivate* d;
};

} // namespace Internal
} // namespace BamTools

#endif // SAM_HEADER_VALIDATOR_P_H
