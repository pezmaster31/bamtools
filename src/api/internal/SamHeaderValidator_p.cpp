// ***************************************************************************
// SamHeaderValidator.cpp (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 23 December 2010 (DB)
// ---------------------------------------------------------------------------
// Provides functionality for validating SamHeader data
// ***************************************************************************

#include <api/SamConstants.h>
#include <api/SamHeader.h>
#include <api/internal/SamHeaderValidator_p.h>
#include <api/internal/SamHeaderVersion_p.h>
using namespace BamTools;
using namespace BamTools::Internal;

#include <iostream>
#include <set>
#include <sstream>
#include <vector>
using namespace std;

// -------------------------------------------------------------------
// Allow validation rules to vary between SAM header versions
//
// use SAM_VERSION_X_Y to tag important changes
//
// Together, they will allow for comparisons like:
// if ( m_version < SAM_VERSION_2_0 ) {
//     // use some older rule
// else
//     // use rule introduced with version 2.0

static const SamHeaderVersion SAM_VERSION_1_0 = SamHeaderVersion(1,0);
static const SamHeaderVersion SAM_VERSION_1_3 = SamHeaderVersion(1,3);

// -----------------------------------------
// SamHeaderValidatorPrivate implementation

class SamHeaderValidator::SamHeaderValidatorPrivate {

    // ctor & dtor
    public:
        SamHeaderValidatorPrivate(const SamHeader& header);
        ~SamHeaderValidatorPrivate(void) { }

    // 'public' methods
    public:
        bool Validate(bool verbose);

    // internal validation methods
    private:

        // validate header metadata
        bool ValidateMetadata(void);
        bool ValidateVersion(void);
        bool ContainsOnlyDigits(const string& s);
        bool ValidateSortOrder(void);
        bool ValidateGroupOrder(void);

        // validate sequence dictionary
        bool ValidateSequenceDictionary(void);
        bool ContainsUniqueSequenceNames(void);
        bool CheckNameFormat(const string& name);
        bool ValidateSequence(const SamSequence& seq);
        bool CheckLengthInRange(const string& length);

        // validate read group dictionary
        bool ValidateReadGroupDictionary(void);
        bool ValidateReadGroup(const SamReadGroup& rg);
        bool ContainsUniqueIDsAndPlatformUnits(void);
        bool CheckReadGroupID(const string& id);
        bool CheckSequencingTechnology(const string& technology);
        bool Is454(const string& technology);
        bool IsHelicos(const string& technology);
        bool IsIllumina(const string& technology);
        bool IsPacBio(const string& technology);
        bool IsSolid(const string& technology);

        // validate program data
        bool ValidateProgramData(void);
        bool ContainsUniqueProgramIds(void);
        bool ValidatePreviousProgramIds(void);

    // error reporting
    private:
        void AddError(const string& message);
        void AddWarning(const string& message);
        void PrintErrorMessages(void);
        void PrintWarningMessages(void);

    // data members
    private:
        const SamHeader&       m_header;
        const SamHeaderVersion m_version;

        bool m_isVerboseOutput;
        const string ERROR_PREFIX;
        const string WARN_PREFIX;
        const string NEWLINE;
        vector<string> m_errorMessages;
        vector<string> m_warningMessages;
};

SamHeaderValidator::SamHeaderValidatorPrivate::SamHeaderValidatorPrivate(const SamHeader& header)
    : m_header(header)
    , m_version( header.Version )
    , m_isVerboseOutput(false)
    , ERROR_PREFIX("ERROR: ")
    , WARN_PREFIX("WARNING: ")
    , NEWLINE("\n")
{ }

bool SamHeaderValidator::SamHeaderValidatorPrivate::Validate(bool verbose) {

    // set error reporting mode
    m_isVerboseOutput = verbose;

    // validate header components
    bool isValid = true;
    isValid &= ValidateMetadata();
    isValid &= ValidateSequenceDictionary();
    isValid &= ValidateReadGroupDictionary();
    isValid &= ValidateProgramData();

    // report errors if desired
    if ( m_isVerboseOutput ) {
        PrintErrorMessages();
        PrintWarningMessages();
    }

    // return validation status
    return isValid;
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::ValidateMetadata(void) {
    bool isValid = true;
    isValid &= ValidateVersion();
    isValid &= ValidateSortOrder();
    isValid &= ValidateGroupOrder();
    return isValid;
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::ValidateVersion(void) {

    const string& version = m_header.Version;

    // warn if version not present
    if ( version.empty() ) {
        AddWarning("Version (VN) missing. Not required, but strongly recommended");
        return true;
    }

    // invalid if version does not contain a period
    const size_t periodFound = version.find(Constants::SAM_PERIOD);
    if ( periodFound == string::npos ) {
        AddError("Invalid version (VN) format: " + version);
        return false;
    }

    // invalid if major version is empty or contains non-digits
    const string majorVersion = version.substr(0, periodFound);
    if ( majorVersion.empty() || !ContainsOnlyDigits(majorVersion) ) {
        AddError("Invalid version (VN) format: " + version);
        return false;
    }

    // invalid if major version is empty or contains non-digits
    const string minorVersion = version.substr(periodFound + 1);
    if ( minorVersion.empty() || !ContainsOnlyDigits(minorVersion) ) {
        AddError("Invalid version (VN) format: " + version);
        return false;
    }

    // TODO: check if version is not just syntactically OK,
    // but is also a valid SAM version ( 1.0 .. CURRENT )

    // all checked out this far, then version is OK
    return true;
}

// assumes non-empty input string
bool SamHeaderValidator::SamHeaderValidatorPrivate::ContainsOnlyDigits(const string& s) {
    const size_t nonDigitPosition = s.find_first_not_of(Constants::SAM_DIGITS);
    return ( nonDigitPosition == string::npos ) ;
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::ValidateSortOrder(void) {

    const string& sortOrder = m_header.SortOrder;

    // warn if sort order not present
    if ( sortOrder.empty() ) {
        AddWarning("Sort order (SO) missing. Not required, but strongly recommended");
        return true;
    }

    // if sort order is valid keyword
    if ( sortOrder == Constants::SAM_HD_SORTORDER_COORDINATE ||
         sortOrder == Constants::SAM_HD_SORTORDER_QUERYNAME  ||
         sortOrder == Constants::SAM_HD_SORTORDER_UNSORTED
       )
    { return true; }

    // otherwise
    AddError("Invalid sort order (SO): " + sortOrder);
    return false;
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::ValidateGroupOrder(void) {

    const string& groupOrder = m_header.GroupOrder;

    // if no group order, no problem, just return OK
    if ( groupOrder.empty() ) return true;

    // if group order is valid keyword
    if ( groupOrder == Constants::SAM_HD_GROUPORDER_NONE  ||
         groupOrder == Constants::SAM_HD_GROUPORDER_QUERY ||
         groupOrder == Constants::SAM_HD_GROUPORDER_REFERENCE
       )
    { return true; }

    // otherwise
    AddError("Invalid group order (GO): " + groupOrder);
    return false;
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::ValidateSequenceDictionary(void) {

    // TODO: warn/error if no sequences ?

    bool isValid = true;

    // check for unique sequence names
    isValid &= ContainsUniqueSequenceNames();

    // iterate over sequences
    const SamSequenceDictionary& sequences = m_header.Sequences;
    SamSequenceConstIterator seqIter = sequences.ConstBegin();
    SamSequenceConstIterator seqEnd  = sequences.ConstEnd();
    for ( ; seqIter != seqEnd; ++seqIter ) {
        const SamSequence& seq = (*seqIter);
        isValid &= ValidateSequence(seq);
    }

    // return validation state
    return isValid;
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::ContainsUniqueSequenceNames(void) {

    bool isValid = true;
    set<string> sequenceNames;
    set<string>::iterator nameIter;

    // iterate over sequences
    const SamSequenceDictionary& sequences = m_header.Sequences;
    SamSequenceConstIterator seqIter = sequences.ConstBegin();
    SamSequenceConstIterator seqEnd  = sequences.ConstEnd();
    for ( ; seqIter != seqEnd; ++seqIter ) {
        const SamSequence& seq = (*seqIter);
        const string& name = seq.Name;

        // lookup sequence name
        nameIter = sequenceNames.find(name);

        // error if found (duplicate entry)
        if ( nameIter != sequenceNames.end() ) {
            AddError("Sequence name (SN): " + name + " is not unique");
            isValid = false;
        }

        // otherwise ok, store name
        sequenceNames.insert(name);
    }

    // return validation state
    return isValid;
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::ValidateSequence(const SamSequence& seq) {
    bool isValid = true;
    isValid &= CheckNameFormat(seq.Name);
    isValid &= CheckLengthInRange(seq.Length);
    return isValid;
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::CheckNameFormat(const string& name) {

    // invalid if name is empty
    if ( name.empty() ) {
        AddError("Sequence entry (@SQ) is missing SN tag");
        return false;
    }

    // invalid if first character is a reserved char
    const char firstChar = name.at(0);
    if ( firstChar == Constants::SAM_EQUAL || firstChar == Constants::SAM_STAR ) {
        AddError("Invalid sequence name (SN): " + name);
        return false;
    }
    // otherwise OK
    return true;
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::CheckLengthInRange(const string& length) {

    // invalid if empty
    if ( length.empty() ) {
        AddError("Sequence entry (@SQ) is missing LN tag");
        return false;
    }

    // convert string length to numeric
    stringstream lengthStream(length);
    unsigned int sequenceLength;
    lengthStream >> sequenceLength;

    // invalid if length outside accepted range
    if ( sequenceLength < Constants::SAM_SQ_LENGTH_MIN || sequenceLength > Constants::SAM_SQ_LENGTH_MAX ) {
        AddError("Sequence length (LN): " + length + " out of range");
        return false;
    }

    // otherwise OK
    return true;
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::ValidateReadGroupDictionary(void) {

    // TODO: warn/error if no read groups ?

    bool isValid = true;

    // check for unique read group IDs & platform units
    isValid &= ContainsUniqueIDsAndPlatformUnits();

    // iterate over read groups
    const SamReadGroupDictionary& readGroups = m_header.ReadGroups;
    SamReadGroupConstIterator rgIter = readGroups.ConstBegin();
    SamReadGroupConstIterator rgEnd  = readGroups.ConstEnd();
    for ( ; rgIter != rgEnd; ++rgIter ) {
        const SamReadGroup& rg = (*rgIter);
        isValid &= ValidateReadGroup(rg);
    }

    // return validation state
    return isValid;
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::ContainsUniqueIDsAndPlatformUnits(void) {

    bool isValid = true;
    set<string> readGroupIds;
    set<string> platformUnits;
    set<string>::iterator idIter;
    set<string>::iterator puIter;

    // iterate over sequences
    const SamReadGroupDictionary& readGroups = m_header.ReadGroups;
    SamReadGroupConstIterator rgIter = readGroups.ConstBegin();
    SamReadGroupConstIterator rgEnd  = readGroups.ConstEnd();
    for ( ; rgIter != rgEnd; ++rgIter ) {
        const SamReadGroup& rg = (*rgIter);

        // --------------------------------
        // check for unique ID

        // lookup read group ID
        const string& id = rg.ID;
        idIter = readGroupIds.find(id);

        // error if found (duplicate entry)
        if ( idIter != readGroupIds.end() ) {
            AddError("Read group ID (ID): " + id + " is not unique");
            isValid = false;
        }

        // otherwise ok, store id
        readGroupIds.insert(id);

        // --------------------------------
        // check for unique platform unit

        // lookup platform unit
        const string& pu = rg.PlatformUnit;
        puIter = platformUnits.find(pu);

        // error if found (duplicate entry)
        if ( puIter != platformUnits.end() ) {
            AddError("Platform unit (PU): " + pu + " is not unique");
            isValid = false;
        }

        // otherwise ok, store platform unit
        platformUnits.insert(pu);
    }

    // return validation state
    return isValid;
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::ValidateReadGroup(const SamReadGroup& rg) {
    bool isValid = true;
    isValid &= CheckReadGroupID(rg.ID);
    isValid &= CheckSequencingTechnology(rg.SequencingTechnology);
    return isValid;
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::CheckReadGroupID(const string& id) {

    // invalid if empty
    if ( id.empty() ) {
        AddError("Read group entry (@RG) is missing ID tag");
        return false;
    }

    // otherwise OK
    return true;
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::CheckSequencingTechnology(const string& technology) {

    // if no technology provided, no problem, just return OK
    if ( technology.empty() ) return true;

    // if technology is valid keyword
    if ( Is454(technology)      ||
         IsHelicos(technology)  ||
         IsIllumina(technology) ||
         IsPacBio(technology)   ||
         IsSolid(technology)
       )
    { return true; }

    // otherwise
    AddError("Invalid read group sequencing platform (PL): " + technology);
    return false;
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::Is454(const string& technology) {
    return ( technology == Constants::SAM_RG_SEQTECHNOLOGY_454 ||
             technology == Constants::SAM_RG_SEQTECHNOLOGY_LS454_LOWER ||
             technology == Constants::SAM_RG_SEQTECHNOLOGY_LS454_UPPER
           );
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::IsHelicos(const string& technology) {
    return ( technology == Constants::SAM_RG_SEQTECHNOLOGY_HELICOS_LOWER ||
             technology == Constants::SAM_RG_SEQTECHNOLOGY_HELICOS_UPPER
           );
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::IsIllumina(const string& technology) {
    return ( technology == Constants::SAM_RG_SEQTECHNOLOGY_ILLUMINA_LOWER ||
             technology == Constants::SAM_RG_SEQTECHNOLOGY_ILLUMINA_UPPER
           );
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::IsPacBio(const string& technology) {
    return ( technology == Constants::SAM_RG_SEQTECHNOLOGY_PACBIO_LOWER ||
             technology == Constants::SAM_RG_SEQTECHNOLOGY_PACBIO_UPPER
           );
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::IsSolid(const string& technology) {
    return ( technology == Constants::SAM_RG_SEQTECHNOLOGY_SOLID_LOWER ||
             technology == Constants::SAM_RG_SEQTECHNOLOGY_SOLID_UPPER
           );
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::ValidateProgramData(void) {
    bool isValid = true;
    isValid &= ContainsUniqueProgramIds();
    isValid &= ValidatePreviousProgramIds();
    return isValid;
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::ContainsUniqueProgramIds(void) {
    bool isValid = true;
    // TODO: once we have ability to handle multiple @PG entries,
    // check here for duplicate ID's
    // but for now, just return true
    return isValid;
}

bool SamHeaderValidator::SamHeaderValidatorPrivate::ValidatePreviousProgramIds(void) {
    bool isValid = true;
    // TODO: check that PP entries are valid later, after we get multiple @PG-entry handling
    // just return true for now
    return isValid;
}
void SamHeaderValidator::SamHeaderValidatorPrivate::AddError(const string& message) {
    m_errorMessages.push_back(ERROR_PREFIX + message + NEWLINE);
}

void SamHeaderValidator::SamHeaderValidatorPrivate::AddWarning(const string& message) {
    m_warningMessages.push_back(WARN_PREFIX + message + NEWLINE);
}

void SamHeaderValidator::SamHeaderValidatorPrivate::PrintErrorMessages(void) {

    // skip if no error messages
    if ( m_errorMessages.empty() ) return;

    // print error header line
    cerr << "* SAM header has " << m_errorMessages.size() << " errors:" << endl;

    // print each error message
    vector<string>::const_iterator errorIter = m_errorMessages.begin();
    vector<string>::const_iterator errorEnd  = m_errorMessages.end();
    for ( ; errorIter != errorEnd; ++errorIter )
        cerr << (*errorIter);
}

void SamHeaderValidator::SamHeaderValidatorPrivate::PrintWarningMessages(void) {

    // skip if no warning messages
    if ( m_warningMessages.empty() ) return;

    // print warning header line
    cerr << "* SAM header has " << m_warningMessages.size() << " warnings:" << endl;

    // print each warning message
    vector<string>::const_iterator warnIter = m_warningMessages.begin();
    vector<string>::const_iterator warnEnd  = m_warningMessages.end();
    for ( ; warnIter != warnEnd; ++warnIter )
        cerr << (*warnIter);
}

// -----------------------------------
// SamHeaderValidator implementation

SamHeaderValidator::SamHeaderValidator(const BamTools::SamHeader& header)
    : d( new SamHeaderValidatorPrivate(header) )
{ }

SamHeaderValidator::~SamHeaderValidator(void) {
    delete d;
    d = 0;
}

bool SamHeaderValidator::Validate(bool verbose) { return d->Validate(verbose); }
