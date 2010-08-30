// ***************************************************************************
// bamtools_filter.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 30 August 2010
// ---------------------------------------------------------------------------
// Filters a single BAM file (or filters multiple BAM files and merges) 
// according to some user-specified criteria.
// ***************************************************************************

// std includes
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// BamTools includes
#include "bamtools_filter.h"
#include "bamtools_filter_engine.h"
#include "bamtools_options.h"
#include "bamtools_utilities.h"
#include "BamReader.h"
#include "BamMultiReader.h"
#include "BamWriter.h"

//JsonCPP includes
#include "jsoncpp/json.h"

using namespace std;
using namespace BamTools; 
using namespace Json;
  
namespace BamTools {
  
// -------------------------------  
// string literal constants  

// property names
const string ALIGNMENTFLAG_PROPERTY       = "alignmentFlag";
const string INSERTSIZE_PROPERTY          = "insertSize";
const string ISDUPLICATE_PROPERTY         = "isDuplicate";
const string ISFAILEDQC_PROPERTY          = "isFailedQC";
const string ISFIRSTMATE_PROPERTY         = "isFirstMate";
const string ISMAPPED_PROPERTY            = "isMapped";
const string ISMATEMAPPED_PROPERTY        = "isMateMapped";
const string ISMATEREVERSESTRAND_PROPERTY = "isMateReverseStrand";
const string ISPAIRED_PROPERTY            = "isPaired";
const string ISPRIMARYALIGNMENT_PROPERTY  = "isPrimaryAlignment";
const string ISPROPERPAIR_PROPERTY        = "isProperPair";
const string ISREVERSESTRAND_PROPERTY     = "isReverseStrand";
const string ISSECONDMATE_PROPERTY        = "isSecondMate";
const string MAPQUALITY_PROPERTY          = "mapQuality";
const string MATEPOSITION_PROPERTY        = "matePosition";
const string MATEREFERENCE_PROPERTY       = "mateReference";
const string NAME_PROPERTY                = "name";
const string POSITION_PROPERTY            = "position";
const string QUERYBASES_PROPERTY          = "queryBases";
const string REFERENCE_PROPERTY           = "reference";

// boolalpha
const string TRUE_STR  = "true";
const string FALSE_STR = "false";
    
} // namespace BamTools
  
// ---------------------------------------------
// FilterToolPrivate declaration

class FilterTool::FilterToolPrivate {
      
    // ctor & dtor
    public:
        FilterToolPrivate(FilterTool::FilterSettings* settings);
        ~FilterToolPrivate(void);  
        
    // 'public' interface
    public:
        bool Run(void);
        
    // internal methods
    private:
        bool AddPropertyTokensToFilter(const string& filterName, const map<string, string>& propertyTokens);
        bool CheckAlignment(const BamAlignment& al);
        const string GetScriptContents(void);
        void InitProperties(void);
        bool ParseCommandLine(void);
        bool ParseFilterObject(const string& filterName, const Json::Value& filterObject);
        bool ParseScript(void);
        bool SetupFilters(void);
        
    // data members
    private:
        vector<string> m_propertyNames;
        FilterTool::FilterSettings* m_settings;
        RefVector m_references;
};
  
// ---------------------------------------------
// FilterSettings implementation

struct FilterTool::FilterSettings {

    // ----------------------------------
    // IO opts
  
    // flags
    bool HasInputBamFilename;
    bool HasOutputBamFilename;
    bool HasRegion;
    bool HasScriptFilename;
    
    // filenames
    vector<string> InputFiles;
    string OutputFilename;
    string Region;
    string ScriptFilename;
    
    // -----------------------------------
    // General filter opts
    
    // flags
    bool HasAlignmentFlagFilter;
    bool HasInsertSizeFilter;
    bool HasMapQualityFilter;
    bool HasNameFilter;
    bool HasQueryBasesFilter;
    
//     bool HasTagFilters;

    // filters
    string AlignmentFlagFilter;
    string InsertSizeFilter;
    string NameFilter;
    string MapQualityFilter;
    string QueryBasesFilter;
    
//     vector<string> TagFilters;

    // -----------------------------------
    // AlignmentFlag filter opts
    
    // flags
    bool HasIsDuplicateFilter;
    bool HasIsFailedQCFilter;
    bool HasIsFirstMateFilter;
    bool HasIsMappedFilter;
    bool HasIsMateMappedFilter;
    bool HasIsMateReverseStrandFilter;
    bool HasIsPairedFilter;
    bool HasIsPrimaryAlignmentFilter;
    bool HasIsProperPairFilter;
    bool HasIsReverseStrandFilter;
    bool HasIsSecondMateFilter;
    
    // filters
    string IsDuplicateFilter;
    string IsFailedQCFilter;
    string IsFirstMateFilter;
    string IsMappedFilter;
    string IsMateMappedFilter;
    string IsMateReverseStrandFilter;
    string IsPairedFilter;
    string IsPrimaryAlignmentFilter;
    string IsProperPairFilter;
    string IsReverseStrandFilter;
    string IsSecondMateFilter;
    
    // ---------------------------------
    // constructor
    
    FilterSettings(void)
        : HasInputBamFilename(false)
        , HasOutputBamFilename(false)
        , HasRegion(false)
        , HasScriptFilename(false)
        , OutputFilename(Options::StandardOut())
        , HasAlignmentFlagFilter(false)
        , HasInsertSizeFilter(false)
        , HasMapQualityFilter(false)
        , HasNameFilter(false)
        , HasQueryBasesFilter(false)
//         , HasTagFilters(false)
        , HasIsDuplicateFilter(false)
        , HasIsFailedQCFilter(false)
        , HasIsFirstMateFilter(false)
        , HasIsMappedFilter(false)
        , HasIsMateMappedFilter(false)
        , HasIsMateReverseStrandFilter(false)
        , HasIsPairedFilter(false)
        , HasIsPrimaryAlignmentFilter(false)
        , HasIsProperPairFilter(false)
        , HasIsReverseStrandFilter(false)
        , HasIsSecondMateFilter(false)
        , IsDuplicateFilter(TRUE_STR)
        , IsFailedQCFilter(TRUE_STR)
        , IsFirstMateFilter(TRUE_STR)
        , IsMappedFilter(TRUE_STR)
        , IsMateMappedFilter(TRUE_STR)
        , IsMateReverseStrandFilter(TRUE_STR)
        , IsPairedFilter(TRUE_STR)
        , IsPrimaryAlignmentFilter(TRUE_STR)
        , IsProperPairFilter(TRUE_STR)
        , IsReverseStrandFilter(TRUE_STR)
        , IsSecondMateFilter(TRUE_STR)
    { }
};  

// ---------------------------------------------
// FilterTool implementation

FilterTool::FilterTool(void)
    : AbstractTool()
    , m_settings(new FilterSettings)
    , m_impl(0)
{
    // set program details
    Options::SetProgramInfo("bamtools filter", "filters BAM file(s)", "-in <filename> [-in <filename> ... ] -out <filename> -region <REGION> [ [-script <filename] | [filterOptions] ]");
    
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in",     "BAM filename", "the input BAM file(s)", "", m_settings->HasInputBamFilename,  m_settings->InputFiles,     IO_Opts, Options::StandardIn());
    Options::AddValueOption("-out",    "BAM filename", "the output BAM file",   "", m_settings->HasOutputBamFilename, m_settings->OutputFilename, IO_Opts, Options::StandardOut());
    Options::AddValueOption("-region", "REGION",       "only read data from this genomic region (see README for more details)", "", m_settings->HasRegion, m_settings->Region, IO_Opts);
    Options::AddValueOption("-script", "filename",     "the filter script file (see README for more details)", "", m_settings->HasScriptFilename, m_settings->ScriptFilename, IO_Opts);
    
    OptionGroup* FilterOpts = Options::CreateOptionGroup("General Filters");
    Options::AddValueOption("-alignmentFlag", "int",     "keep reads with this *exact* alignment flag (for more detailed queries, see below)", "", m_settings->HasAlignmentFlagFilter, m_settings->AlignmentFlagFilter, FilterOpts);
    Options::AddValueOption("-insertSize",    "int",     "keep reads with insert size that mathces pattern",             "", m_settings->HasInsertSizeFilter, m_settings->InsertSizeFilter, FilterOpts);
    Options::AddValueOption("-mapQuality",    "[0-255]", "keep reads with map quality that matches pattern",             "", m_settings->HasMapQualityFilter, m_settings->MapQualityFilter, FilterOpts);
    Options::AddValueOption("-name",          "string",  "keep reads with name that matches pattern",                    "", m_settings->HasNameFilter,       m_settings->NameFilter,       FilterOpts);
    Options::AddValueOption("-queryBases",    "string",  "keep reads with motif that mathces pattern",                   "", m_settings->HasQueryBasesFilter, m_settings->QueryBasesFilter, FilterOpts);
    
//     Options::AddValueOption("-tag", "TAG:VALUE", "keep reads with this key=>value pair. If multiple tags are given, reads must match all", "", m_settings->HasTagFilters, m_settings->TagFilters, FilterOpts);
    
    OptionGroup* AlignmentFlagOpts = Options::CreateOptionGroup("Alignment Flag Filters");
    Options::AddValueOption("-isDuplicate",         "true/false", "keep only alignments that are marked as duplicate?", "", m_settings->HasIsDuplicateFilter,         m_settings->IsDuplicateFilter,         AlignmentFlagOpts, TRUE_STR);
    Options::AddValueOption("-isFailedQC",          "true/false", "keep only alignments that failed QC?",               "", m_settings->HasIsFailedQCFilter,          m_settings->IsFailedQCFilter,          AlignmentFlagOpts, TRUE_STR);
    Options::AddValueOption("-isFirstMate",         "true/false", "keep only alignments marked as first mate?",         "", m_settings->HasIsFirstMateFilter,         m_settings->IsFirstMateFilter,         AlignmentFlagOpts, TRUE_STR);
    Options::AddValueOption("-isMapped",            "true/false", "keep only alignments that were mapped?",             "", m_settings->HasIsMappedFilter,            m_settings->IsMappedFilter,            AlignmentFlagOpts, TRUE_STR);
    Options::AddValueOption("-isMateMapped",        "true/false", "keep only alignments with mates that mapped",        "", m_settings->HasIsMateMappedFilter,        m_settings->IsMateMappedFilter,        AlignmentFlagOpts, TRUE_STR);
    Options::AddValueOption("-isMateReverseStrand", "true/false", "keep only alignments with mate on reverese strand?", "", m_settings->HasIsMateReverseStrandFilter, m_settings->IsMateReverseStrandFilter, AlignmentFlagOpts, TRUE_STR);
    Options::AddValueOption("-isPaired",            "true/false", "keep only alignments that were sequenced as paired?","", m_settings->HasIsPairedFilter,            m_settings->IsPairedFilter,            AlignmentFlagOpts, TRUE_STR);
    Options::AddValueOption("-isPrimaryAlignment",  "true/false", "keep only alignments marked as primary?",            "", m_settings->HasIsPrimaryAlignmentFilter,  m_settings->IsPrimaryAlignmentFilter,  AlignmentFlagOpts, TRUE_STR);
    Options::AddValueOption("-isProperPair",        "true/false", "keep only alignments that passed PE resolution?",    "", m_settings->HasIsProperPairFilter,        m_settings->IsProperPairFilter,        AlignmentFlagOpts, TRUE_STR);
    Options::AddValueOption("-isReverseStrand",     "true/false", "keep only alignments on reverse strand?",            "", m_settings->HasIsReverseStrandFilter,     m_settings->IsReverseStrandFilter,     AlignmentFlagOpts, TRUE_STR);
    Options::AddValueOption("-isSecondMate",        "true/false", "keep only alignments marked as second mate?",        "", m_settings->HasIsSecondMateFilter,        m_settings->IsSecondMateFilter,        AlignmentFlagOpts, TRUE_STR);
}

FilterTool::~FilterTool(void) {
    delete m_settings;
    m_settings = 0;
    
    delete m_impl;
    m_impl = 0;
}

int FilterTool::Help(void) {
    Options::DisplayHelp();
    return 0;
}

int FilterTool::Run(int argc, char* argv[]) {
  
    // parse command line arguments
    Options::Parse(argc, argv, 1);
    
    // run internal FilterTool implementation, return success/fail
    m_impl = new FilterToolPrivate(m_settings);
    
    if ( m_impl->Run() ) return 0;
    else return 1;
}
 
// ---------------------------------------------
// FilterToolPrivate implementation
  
// constructor  
FilterTool::FilterToolPrivate::FilterToolPrivate(FilterTool::FilterSettings* settings)
    : m_settings(settings)
{ }  
  
// destructor
FilterTool::FilterToolPrivate::~FilterToolPrivate(void) { }

bool FilterTool::FilterToolPrivate::AddPropertyTokensToFilter(const string& filterName, const map<string, string>& propertyTokens) {

  
    // dummy temp values for token parsing
    bool boolValue;
    int32_t int32Value;
    uint16_t uint16Value;
    uint32_t uint32Value;
    string stringValue;
    PropertyFilterValue::ValueCompareType type;
  
    // iterate over property token map
    map<string, string>::const_iterator mapIter = propertyTokens.begin();
    map<string, string>::const_iterator mapEnd  = propertyTokens.end();
    for ( ; mapIter != mapEnd; ++mapIter ) {
      
        const string& propertyName = (*mapIter).first;
        const string& token        = (*mapIter).second;
        
        // ------------------------------
        // convert token to value & compare type 
        // then add to filter engine
        
        // bool conversion
        if ( propertyName == ISDUPLICATE_PROPERTY ||
             propertyName == ISFAILEDQC_PROPERTY ||
             propertyName == ISFIRSTMATE_PROPERTY ||
             propertyName == ISMAPPED_PROPERTY ||
             propertyName == ISMATEMAPPED_PROPERTY ||
             propertyName == ISMATEREVERSESTRAND_PROPERTY ||
             propertyName == ISPAIRED_PROPERTY ||
             propertyName == ISPRIMARYALIGNMENT_PROPERTY ||
             propertyName == ISPROPERPAIR_PROPERTY ||
             propertyName == ISREVERSESTRAND_PROPERTY ||
             propertyName == ISSECONDMATE_PROPERTY
           ) 
        {
            FilterEngine::parseToken(token, boolValue, type);
            FilterEngine::setProperty(filterName, propertyName, boolValue, type);
        }
        
        // int32_t conversion
        else if ( propertyName == INSERTSIZE_PROPERTY ||
                  propertyName == MATEPOSITION_PROPERTY ||
                  propertyName == POSITION_PROPERTY 
                ) 
        {
            FilterEngine::parseToken(token, int32Value, type);
            FilterEngine::setProperty(filterName, propertyName, int32Value, type);
        }
        
        // uint16_t conversion
        else if ( propertyName == MAPQUALITY_PROPERTY )
        {
            FilterEngine::parseToken(token, uint16Value, type);
            FilterEngine::setProperty(filterName, propertyName, uint16Value, type);
        }
        
        // uint32_t conversion
        else if ( propertyName == ALIGNMENTFLAG_PROPERTY )
        {
            FilterEngine::parseToken(token, uint32Value, type);
            FilterEngine::setProperty(filterName, propertyName, uint32Value, type);
        }
        
        // string conversion
        else if ( propertyName == MATEREFERENCE_PROPERTY ||
                  propertyName == NAME_PROPERTY ||
                  propertyName == QUERYBASES_PROPERTY ||
                  propertyName == REFERENCE_PROPERTY
                ) 
        {
            FilterEngine::parseToken(token, stringValue, type);
            FilterEngine::setProperty(filterName, propertyName, stringValue, type);
        }
      
        // else unknown property 
        else {
            cerr << "Unknown property: " << propertyName << "!" << endl;
            return false;
        }
    }
    return true;
}

bool FilterTool::FilterToolPrivate::CheckAlignment(const BamAlignment& al) {

    bool keepAlignment = true;
  
    // only consider properties that are actually enabled
    // iterate over these enabled properties
    const vector<string> enabledProperties = FilterEngine::enabledPropertyNames();
    vector<string>::const_iterator propIter = enabledProperties.begin();
    vector<string>::const_iterator propEnd  = enabledProperties.end();
    for ( ; propIter != propEnd; ++propIter ) {
     
        // check alignment data field depending on propertyName
        const string& propertyName = (*propIter);
        if      ( propertyName == ALIGNMENTFLAG_PROPERTY )        keepAlignment &= FilterEngine::check(ALIGNMENTFLAG_PROPERTY,       al.AlignmentFlag);
        else if ( propertyName == INSERTSIZE_PROPERTY )           keepAlignment &= FilterEngine::check(INSERTSIZE_PROPERTY,          al.InsertSize);
        else if ( propertyName == ISDUPLICATE_PROPERTY )          keepAlignment &= FilterEngine::check(ISDUPLICATE_PROPERTY,         al.IsDuplicate());
        else if ( propertyName == ISFAILEDQC_PROPERTY )           keepAlignment &= FilterEngine::check(ISFAILEDQC_PROPERTY,          al.IsFailedQC());
        else if ( propertyName == ISFIRSTMATE_PROPERTY )          keepAlignment &= FilterEngine::check(ISFIRSTMATE_PROPERTY,         al.IsFirstMate());
        else if ( propertyName == ISMAPPED_PROPERTY )             keepAlignment &= FilterEngine::check(ISMAPPED_PROPERTY,            al.IsMapped());
        else if ( propertyName == ISMATEMAPPED_PROPERTY )         keepAlignment &= FilterEngine::check(ISMATEMAPPED_PROPERTY,        al.IsMateMapped());
        else if ( propertyName == ISMATEREVERSESTRAND_PROPERTY )  keepAlignment &= FilterEngine::check(ISMATEREVERSESTRAND_PROPERTY, al.IsMateReverseStrand());
        else if ( propertyName == ISPAIRED_PROPERTY )             keepAlignment &= FilterEngine::check(ISPAIRED_PROPERTY,            al.IsPaired());
        else if ( propertyName == ISPRIMARYALIGNMENT_PROPERTY )   keepAlignment &= FilterEngine::check(ISPRIMARYALIGNMENT_PROPERTY,  al.IsPrimaryAlignment());
        else if ( propertyName == ISPROPERPAIR_PROPERTY )         keepAlignment &= FilterEngine::check(ISPROPERPAIR_PROPERTY,        al.IsProperPair());
        else if ( propertyName == ISREVERSESTRAND_PROPERTY )      keepAlignment &= FilterEngine::check(ISREVERSESTRAND_PROPERTY,     al.IsReverseStrand());
        else if ( propertyName == ISSECONDMATE_PROPERTY )         keepAlignment &= FilterEngine::check(ISSECONDMATE_PROPERTY,        al.IsSecondMate());
        else if ( propertyName == MAPQUALITY_PROPERTY )           keepAlignment &= FilterEngine::check(MAPQUALITY_PROPERTY,          al.MapQuality);
        else if ( propertyName == MATEPOSITION_PROPERTY )         keepAlignment &= ( al.IsPaired() && al.IsMateMapped() && FilterEngine::check(MATEPOSITION_PROPERTY, al.MateRefID) );
        else if ( propertyName == MATEREFERENCE_PROPERTY ) {
            if ( !al.IsPaired() || !al.IsMateMapped() ) return false;
            BAMTOOLS_ASSERT_MESSAGE( (al.MateRefID>=0 && (al.MateRefID<(int)m_references.size())), "Invalid MateRefID");
            const string& refName = m_references.at(al.MateRefID).RefName;
            keepAlignment &= FilterEngine::check(MATEREFERENCE_PROPERTY, refName);
        }
        else if ( propertyName == NAME_PROPERTY )                 keepAlignment &= FilterEngine::check(NAME_PROPERTY,                al.Name);
        else if ( propertyName == POSITION_PROPERTY )             keepAlignment &= FilterEngine::check(POSITION_PROPERTY,            al.Position);
        else if ( propertyName == QUERYBASES_PROPERTY )           keepAlignment &= FilterEngine::check(QUERYBASES_PROPERTY,          al.QueryBases);
        else if ( propertyName == REFERENCE_PROPERTY ) {
            BAMTOOLS_ASSERT_MESSAGE( (al.RefID>=0 && (al.RefID<(int)m_references.size())), "Invalid RefID");
            const string& refName = m_references.at(al.RefID).RefName;
            keepAlignment &= FilterEngine::check(REFERENCE_PROPERTY, refName);
        }
        else BAMTOOLS_ASSERT_MESSAGE( false, "Unknown property");
        
        // if alignment fails at ANY point, just quit and return false
        if ( !keepAlignment ) return false;
    }
  
    // return success (should still be true at this point)
    return keepAlignment;
}

const string FilterTool::FilterToolPrivate::GetScriptContents(void) {
  
    // open script for reading
    FILE* inFile = fopen(m_settings->ScriptFilename.c_str(), "rb");
    if ( !inFile ) {
        cerr << "FilterTool error: Could not open script: " << m_settings->ScriptFilename << " for reading" << endl;
        return false;
    }
    
    // read in entire script contents  
    char buffer[1024];
    ostringstream docStream("");
    while ( true ) {
        
        // peek ahead, make sure there is data available
        char ch = fgetc(inFile);
        ungetc(ch, inFile);
        if( feof(inFile) ) break;       
        
        // read next block of data
        if ( fgets(buffer, 1024, inFile) == 0 ) {
            cerr << "FilterTool error : could not read from script" << endl;
            return false;
        }
        
        docStream << buffer;
    }
    
    // close script file
    fclose(inFile);
    
    // import buffer contents to document, return
    string document = docStream.str();
    return document;
}

void FilterTool::FilterToolPrivate::InitProperties(void) {
  
    // store property names in vector 
    m_propertyNames.push_back(ALIGNMENTFLAG_PROPERTY);
    m_propertyNames.push_back(INSERTSIZE_PROPERTY);
    m_propertyNames.push_back(ISDUPLICATE_PROPERTY);
    m_propertyNames.push_back(ISFAILEDQC_PROPERTY);
    m_propertyNames.push_back(ISFIRSTMATE_PROPERTY);
    m_propertyNames.push_back(ISMAPPED_PROPERTY);
    m_propertyNames.push_back(ISMATEMAPPED_PROPERTY);
    m_propertyNames.push_back(ISMATEREVERSESTRAND_PROPERTY);
    m_propertyNames.push_back(ISPAIRED_PROPERTY);
    m_propertyNames.push_back(ISPRIMARYALIGNMENT_PROPERTY);
    m_propertyNames.push_back(ISPROPERPAIR_PROPERTY);
    m_propertyNames.push_back(ISREVERSESTRAND_PROPERTY);
    m_propertyNames.push_back(ISSECONDMATE_PROPERTY);
    m_propertyNames.push_back(MAPQUALITY_PROPERTY);
    m_propertyNames.push_back(MATEPOSITION_PROPERTY);
    m_propertyNames.push_back(MATEREFERENCE_PROPERTY);
    m_propertyNames.push_back(NAME_PROPERTY);
    m_propertyNames.push_back(POSITION_PROPERTY);
    m_propertyNames.push_back(QUERYBASES_PROPERTY);
    m_propertyNames.push_back(REFERENCE_PROPERTY);
    
    // add vector contents to FilterEngine
    vector<string>::const_iterator propertyNameIter = m_propertyNames.begin();
    vector<string>::const_iterator propertyNameEnd  = m_propertyNames.end();
    for ( ; propertyNameIter != propertyNameEnd; ++propertyNameIter )
        FilterEngine::addProperty((*propertyNameIter));
}

bool FilterTool::FilterToolPrivate::ParseCommandLine(void) {
  
    // add a rule set to filter engine
    const string CMD = "COMMAND_LINE";
    FilterEngine::addFilter(CMD);

    // map property names to command line args
    map<string, string> propertyTokens;
    if ( m_settings->HasAlignmentFlagFilter )       propertyTokens.insert( make_pair(ALIGNMENTFLAG_PROPERTY,       m_settings->AlignmentFlagFilter) );
    if ( m_settings->HasInsertSizeFilter )          propertyTokens.insert( make_pair(INSERTSIZE_PROPERTY,          m_settings->InsertSizeFilter) );
    if ( m_settings->HasIsDuplicateFilter )         propertyTokens.insert( make_pair(ISDUPLICATE_PROPERTY,         m_settings->IsDuplicateFilter) );
    if ( m_settings->HasIsFailedQCFilter )          propertyTokens.insert( make_pair(ISFAILEDQC_PROPERTY,          m_settings->IsFailedQCFilter) );
    if ( m_settings->HasIsFirstMateFilter )         propertyTokens.insert( make_pair(ISFIRSTMATE_PROPERTY,         m_settings->IsFirstMateFilter) );
    if ( m_settings->HasIsMappedFilter )            propertyTokens.insert( make_pair(ISMAPPED_PROPERTY,            m_settings->IsMappedFilter) );
    if ( m_settings->HasIsMateMappedFilter )        propertyTokens.insert( make_pair(ISMATEMAPPED_PROPERTY,        m_settings->IsMateMappedFilter) );
    if ( m_settings->HasIsMateReverseStrandFilter ) propertyTokens.insert( make_pair(ISMATEREVERSESTRAND_PROPERTY, m_settings->IsMateReverseStrandFilter) );
    if ( m_settings->HasIsPairedFilter )            propertyTokens.insert( make_pair(ISPAIRED_PROPERTY,            m_settings->IsPairedFilter) );
    if ( m_settings->HasIsPrimaryAlignmentFilter )  propertyTokens.insert( make_pair(ISPRIMARYALIGNMENT_PROPERTY,  m_settings->IsPrimaryAlignmentFilter) );
    if ( m_settings->HasIsProperPairFilter )        propertyTokens.insert( make_pair(ISPROPERPAIR_PROPERTY,        m_settings->IsProperPairFilter) );
    if ( m_settings->HasIsReverseStrandFilter )     propertyTokens.insert( make_pair(ISREVERSESTRAND_PROPERTY,     m_settings->IsReverseStrandFilter) );
    if ( m_settings->HasIsSecondMateFilter )        propertyTokens.insert( make_pair(ISSECONDMATE_PROPERTY,        m_settings->IsSecondMateFilter) );
    if ( m_settings->HasMapQualityFilter )          propertyTokens.insert( make_pair(MAPQUALITY_PROPERTY,          m_settings->MapQualityFilter) );
    if ( m_settings->HasNameFilter )                propertyTokens.insert( make_pair(NAME_PROPERTY,                m_settings->NameFilter) );
    if ( m_settings->HasQueryBasesFilter )          propertyTokens.insert( make_pair(QUERYBASES_PROPERTY,          m_settings->QueryBasesFilter) );

    // send add these properties to filter set "COMMAND_LINE"
    return AddPropertyTokensToFilter(CMD, propertyTokens);
}

bool FilterTool::FilterToolPrivate::ParseFilterObject(const string& filterName, const Json::Value& filterObject) {
  
    // filter object parsing variables
    Json::Value null(Json::nullValue);
    Json::Value propertyValue;
    
    // store results
    map<string, string> propertyTokens;
    
    // iterate over known properties
    vector<string>::const_iterator propertyNameIter = m_propertyNames.begin();
    vector<string>::const_iterator propertyNameEnd  = m_propertyNames.end();
    for ( ; propertyNameIter != propertyNameEnd; ++propertyNameIter ) {
        const string& propertyName = (*propertyNameIter);
        
        // if property defined in filter, add to token list
        propertyValue = filterObject.get(propertyName, null);
        if ( propertyValue != null ) 
            propertyTokens.insert( make_pair(propertyName, propertyValue.asString()) );
    }
  
    // add this filter to engin
    FilterEngine::addFilter(filterName);
  
    // add token list to this filter
    return AddPropertyTokensToFilter(filterName, propertyTokens);
}

bool FilterTool::FilterToolPrivate::ParseScript(void) {
  
    // read in script contents from file
    const string document = GetScriptContents();
    
    // set up JsonCPP reader and attempt to parse script
    Json::Value root;
    Json::Reader reader;
    if ( !reader.parse(document, root) ) {
        // use built-in error reporting mechanism to alert user what was wrong with the script
        cerr  << "Failed to parse configuration\n" << reader.getFormatedErrorMessages();
        return false;     
    }

    // see if root object contains multiple filters
    const Json::Value filters = root["filters"];
    if ( !filters.isNull() ) {
      
        bool success = true;
      
        // iterate over any filters found
        int filterIndex = 0;
        Json::Value::const_iterator filtersIter = filters.begin();
        Json::Value::const_iterator filtersEnd  = filters.end();
        for ( ; filtersIter != filtersEnd; ++filtersIter, ++filterIndex ) {
            Json::Value filter = (*filtersIter);
            
            // convert filter index to string
            stringstream convert;
            convert << filterIndex;
            const string filterName = convert.str();
            
            // create & parse filter 
            success &= ParseFilterObject(filterName, filter);
            
        }
        return success;
    } 
    
    // otherwise, root is the only filter (just contains properties)
    // create & parse filter named "ROOT"
    else return ParseFilterObject("ROOT", root);
}


bool FilterTool::FilterToolPrivate::Run(void) {
    
    // set to default input if none provided
    if ( !m_settings->HasInputBamFilename ) 
        m_settings->InputFiles.push_back(Options::StandardIn());
    
    // initialize defined properties & user-specified filters
    // quit if failed
    if ( !SetupFilters() ) return 1;

    // open reader without index
    BamMultiReader reader;
    reader.Open(m_settings->InputFiles, false, true);
    const string headerText = reader.GetHeaderText();
    m_references = reader.GetReferenceData();
    
    // open writer
    BamWriter writer;
    writer.Open(m_settings->OutputFilename, headerText, m_references);
    
    // set up error handling
    ostringstream errorStream("");
    bool foundError(false);
    
    // if no REGION specified, run filter on entire file contents
    if ( !m_settings->HasRegion ) {
        BamAlignment al;
        while ( reader.GetNextAlignment(al) ) {
            // perform main filter check
            if ( CheckAlignment(al) ) 
                writer.SaveAlignment(al);
        }
    }
    
    // REGION specified
    else {
      
        // attempt to parse string into BamRegion struct
        BamRegion region;
        if ( Utilities::ParseRegionString(m_settings->Region, reader, region) ) {

            // check if there are index files *.bai/*.bti corresponding to the input files
            bool hasDefaultIndex   = false;
            bool hasBamtoolsIndex  = false;
            bool hasNoIndex        = false;
            int defaultIndexCount   = 0;
            int bamtoolsIndexCount = 0;
            for (vector<string>::const_iterator f = m_settings->InputFiles.begin(); f != m_settings->InputFiles.end(); ++f) {
              
                if ( Utilities::FileExists(*f + ".bai") ) {
                    hasDefaultIndex = true;
                    ++defaultIndexCount;
                }       
                
                if ( Utilities::FileExists(*f + ".bti") ) {
                    hasBamtoolsIndex = true;
                    ++bamtoolsIndexCount;
                }
                  
                if ( !hasDefaultIndex && !hasBamtoolsIndex ) {
                    hasNoIndex = true;
                    cerr << "*WARNING - could not find index file for " << *f  
                         << ", parsing whole file(s) to get alignment counts for target region" 
                         << " (could be slow)" << endl;
                    break;
                }
            }
            
            // determine if index file types are heterogeneous
            bool hasDifferentIndexTypes = false;
            if ( defaultIndexCount > 0 && bamtoolsIndexCount > 0 ) {
                hasDifferentIndexTypes = true;
                cerr << "*WARNING - different index file formats found"  
                         << ", parsing whole file(s) to get alignment counts for target region" 
                         << " (could be slow)" << endl;
            }
            
            // if any input file has no index, or if input files use different index formats
            // can't use BamMultiReader to jump directly (**for now**)
            if ( hasNoIndex || hasDifferentIndexTypes ) {
                
                // read through sequentially, but onlt perform filter on reads overlapping REGION
                BamAlignment al;
                while( reader.GetNextAlignment(al) ) {
                    if ( (al.RefID >= region.LeftRefID)  && ( (al.Position + al.Length) >= region.LeftPosition ) &&
                         (al.RefID <= region.RightRefID) && ( al.Position <= region.RightPosition) ) 
                    {
                        // perform main filter check
                        if ( CheckAlignment(al) ) 
                            writer.SaveAlignment(al);
                    }
                }
            }
            
            // has index file for each input file (and same format)
            else {
              
                // this is kind of a hack...?
                BamMultiReader reader;
                reader.Open(m_settings->InputFiles, true, true, hasDefaultIndex );
              
                if ( !reader.SetRegion(region.LeftRefID, region.LeftPosition, region.RightRefID, region.RightPosition) ) {
                   foundError = true;
                   errorStream << "Could not set BamReader region to REGION: " << m_settings->Region << endl;
                } else {
                  
                    // filter only alignments from specified region
                    BamAlignment al;
                    while ( reader.GetNextAlignment(al) ) {
                        // perform main filter check
                        if ( CheckAlignment(al) ) 
                            writer.SaveAlignment(al);
                    }
                }
            }
            
        } else {
            foundError = true;
            errorStream << "Could not parse REGION: " << m_settings->Region << endl;
            errorStream << "Be sure REGION is in valid format (see README) and that coordinates are valid for selected references" << endl;
        }
    }

    // clean up & exit
    reader.Close();
    writer.Close();
    return 0;
}

bool FilterTool::FilterToolPrivate::SetupFilters(void) {
  
    // add known properties to FilterEngine
    InitProperties();
    
    // parse script for filter rules, if given
    if ( m_settings->HasScriptFilename ) return ParseScript();
    
    // otherwise check command line for filters
    else return ParseCommandLine();
}
