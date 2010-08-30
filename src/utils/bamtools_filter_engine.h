// ***************************************************************************
// bamtools_filter_engine.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 30 August 2010
// ---------------------------------------------------------------------------
// 
// ***************************************************************************

#ifndef BAMTOOLS_FILTER_ENGINE_H
#define BAMTOOLS_FILTER_ENGINE_H

#include <algorithm>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "bamtools_utilities.h"
#include "bamtools_variant.h"

namespace BamTools {

struct PropertyFilterValue {
  
    // define valid ValueCompareTypes
    enum ValueCompareType { CONTAINS = 0
                          , ENDS_WITH
                          , EXACT
                          , GREATER_THAN
                          , GREATER_THAN_EQUAL
                          , LESS_THAN
                          , LESS_THAN_EQUAL
                          , NOT
                          , STARTS_WITH
                          };
                   
    // ctor
    PropertyFilterValue(const Variant& value = Variant(),
                        const ValueCompareType& type = PropertyFilterValue::EXACT)
        : Value(value)
        , Type(type)
    { }
          
    // filter check methods      
    template<typename T>
    bool check(const T& query) const;
    bool check(const std::string& query) const;
             
    // data members
    Variant Value;
    ValueCompareType Type;
};

inline
const std::string toString(const PropertyFilterValue::ValueCompareType& type) {
  
    switch ( type ) {
        case ( PropertyFilterValue::CONTAINS )           : return std::string( "CONTAINS");
        case ( PropertyFilterValue::ENDS_WITH )          : return std::string( "ENDS_WITH");
        case ( PropertyFilterValue::EXACT )              : return std::string( "EXACT");
        case ( PropertyFilterValue::GREATER_THAN )       : return std::string( "GREATER_THAN");
        case ( PropertyFilterValue::GREATER_THAN_EQUAL ) : return std::string( "GREATER_THAN_EQUAL");
        case ( PropertyFilterValue::LESS_THAN )          : return std::string( "LESS_THAN");
        case ( PropertyFilterValue::LESS_THAN_EQUAL )    : return std::string( "LESS_THAN_EQUAL");
        case ( PropertyFilterValue::NOT )                : return std::string( "NOT");
        case ( PropertyFilterValue::STARTS_WITH )        : return std::string( "STARTS_WITH");
        default : BAMTOOLS_ASSERT_UNREACHABLE;
    }
    return std::string();
}

// property name => property filter value 
// ('name' => ('SSR', STARTS_WITH), 'mapQuality' => (50, GREATER_THAN_EQUAL), etc...)
typedef std::map<std::string, PropertyFilterValue> PropertyMap;

struct PropertyFilter {  
  
    // will be used more later
    // if we implement a compound 'rules' system  - i.e. "(filter1 AND filter2) OR filter 3"
    enum FilterCompareType { AND = 0
                           , EXACT
                           , NOT
                           , OR
                           };
  
    // data members
    PropertyMap Properties;
    FilterCompareType Type; 

    // ctor
    PropertyFilter(void) : Type( PropertyFilter::EXACT ) { }
    
    // filter check methods      
    template<typename T>
    bool check(const std::string& propertyName, const T& query) const;
};

// filter name => properties  
// ('filter1' => properties1, 'filter2' => properties2, etc...)
typedef std::map<std::string, PropertyFilter> FilterMap;
  
// used to store properties known to engine & keep track of enabled state
struct Property {
    std::string Name;
    bool IsEnabled;
    Property(const std::string& name, bool isEnabled = false) 
        : Name(name)
        , IsEnabled(isEnabled) 
    { }
};

inline bool operator<  (const Property& lhs, const Property& rhs) { return lhs.Name <  rhs.Name; }
inline bool operator== (const Property& lhs, const Property& rhs) { return lhs.Name == rhs.Name; }
  
class FilterEngine {
  
    // 'filter set' methods
    public:
        // creates a new filter set, returns true if created, false if error or already exists
        static bool addFilter(const std::string& filterName);       
        
        // return list of current filter names
        static const std::vector<std::string> filterNames(void);    
  
    // 'property' methods
    public:
      
        // add a new known property (& type) to engine
        static bool addProperty(const std::string& propertyName);
  
        // sets property filter (value, type) for propertyName, on a particular filter set 
        // setProperty("filter1", "mapQuality", 50, GREATER_THAN_EQUAL)
        template<typename T>
        static bool setProperty(const std::string& filterName, 
                                const std::string& propertyName, 
                                const T& value,
                                const PropertyFilterValue::ValueCompareType& type = PropertyFilterValue::EXACT);
        
        // returns list of all properties known by FilterEngine  ( any created using addProperty() )
        static const std::vector<std::string> allPropertyNames(void);
        
        // returns list of property names that are 'enabled' ( only those touched by setProperty() )
        static const std::vector<std::string> enabledPropertyNames(void);  
  
    // token parsing (for property filter generation)
    public:
        template<typename T>
        static bool parseToken(const std::string& token, T& value, PropertyFilterValue::ValueCompareType& type);
        
    // query evaluation
    public:
        // returns true if query passes all filters on 'propertyName'
        template<typename T>
        static bool check(const std::string& propertyName, const T& query);

    // data members
    private:
        // all 'filter sets'
        static FilterMap m_filters;
        
        // all known properties
        static std::vector<Property> m_properties;  
        
        // token-parsing constants
        static const int NOT_CHAR          = (int)'!';
        static const int EQUAL_CHAR        = (int)'=';
        static const int GREATER_THAN_CHAR = (int)'>';
        static const int LESS_THAN_CHAR    = (int)'<';
        static const int WILDCARD_CHAR     = (int)'*';
};

// -------------------------------------------------------------------
// template methods

// checks a query against a filter (value, compare type)
template<typename T>
bool PropertyFilterValue::check(const T& query) const {
  
    // ensure filter value & query are same type
    if ( !Value.is_type<T>() ) { 
        std::cerr << "Cannot compare different types!" << std::endl;
        return false;
    }
    
    // string matching
    if ( Value.is_type<std::string>() ) {
        std::cerr << "Cannot compare different types - query is a string!" << std::endl;
        return false;
    } 
    
    // numeric matching based on our filter type
    switch ( Type ) {
        case ( PropertyFilterValue::EXACT)              : return ( query == Value.get<T>() );
        case ( PropertyFilterValue::GREATER_THAN)       : return ( query >  Value.get<T>() ); 
        case ( PropertyFilterValue::GREATER_THAN_EQUAL) : return ( query >= Value.get<T>() ); 
        case ( PropertyFilterValue::LESS_THAN)          : return ( query <  Value.get<T>() );
        case ( PropertyFilterValue::LESS_THAN_EQUAL)    : return ( query <= Value.get<T>() );
        case ( PropertyFilterValue::NOT)                : return ( query != Value.get<T>() );
        default : BAMTOOLS_ASSERT_UNREACHABLE;
    }
    return false;
}

template<typename T>
bool PropertyFilter::check(const std::string& propertyName, const T& query) const {
  
    // if propertyName found for this filter, 
    PropertyMap::const_iterator propIter = Properties.find(propertyName);
    if ( propIter != Properties.end() ) {
      const PropertyFilterValue& filterValue = (*propIter).second;
      
      // check 
      switch ( Type ) {
          case ( PropertyFilter::EXACT ) : return filterValue.check(query);
          case ( PropertyFilter::NOT )   : return !filterValue.check(query);
          case ( PropertyFilter::AND )   :
          case ( PropertyFilter::OR )    : BAMTOOLS_ASSERT_MESSAGE(false, "Cannot use a binary compare operator on 1 value");
          default : BAMTOOLS_ASSERT_UNREACHABLE;
      }
      return false; // unreachable
    }

    // property unknown to this filter
    else return true;
}

template<typename T>
bool FilterEngine::parseToken(const std::string& token, T& value, PropertyFilterValue::ValueCompareType& type) {
    
    // skip if token is empty
    if ( token.empty() ) return false;
    
    // will store token after special chars are removed
    std::string strippedToken;
    
    // if only single character
    if ( token.length() == 1 ) {
        strippedToken = token;
        type = PropertyFilterValue::EXACT;
    } 
    
    // more than one character, check for special chars
    else {
        const int firstChar = (int)token.at(0);
        
        switch ( (int)firstChar ) {
          
            case ( (int)FilterEngine::NOT_CHAR ) :
              
                strippedToken = token.substr(1);       
                type = PropertyFilterValue::NOT;
                
                break;
                
            case ( (int)FilterEngine::GREATER_THAN_CHAR ) :
                
                // check for '>=' case
                if ( token.at(1) == FilterEngine::EQUAL_CHAR ) {
                    if ( token.length() == 2 ) return false;
                    strippedToken = token.substr(2);
                    type = PropertyFilterValue::GREATER_THAN_EQUAL;
                } 
                
                // otherwise only '>'
                else {
                    strippedToken = token.substr(1);
                    type = PropertyFilterValue::GREATER_THAN;
                }
                
                break;
                
            case ( (int)FilterEngine::LESS_THAN_CHAR ) : 
         
                // check for '<=' case
                if ( token.at(1) == FilterEngine::EQUAL_CHAR ) {
                    if ( token.length() == 2 ) return false;
                    strippedToken = token.substr(2);
                    type = PropertyFilterValue::LESS_THAN_EQUAL;
                } 
                
                // otherwise only '<'
                else {
                    strippedToken = token.substr(1);
                    type = PropertyFilterValue::LESS_THAN;
                }
                
                break;
                
            case ( (int)FilterEngine::WILDCARD_CHAR ) : 
              
                // check for *str* case (CONTAINS)
                if ( token.at( token.length() - 1 ) == FilterEngine::WILDCARD_CHAR ) {
                    if ( token.length() == 2 ) return false;
                    strippedToken = token.substr(1, token.length() - 2);
                    type = PropertyFilterValue::CONTAINS;
                }
                
                // otherwise *str case (ENDS_WITH)
                else {
                    strippedToken = token.substr(1);
                    type = PropertyFilterValue::ENDS_WITH;
                }
                
                break;
               
                
            default :
              
                // check for str* case (STARTS_WITH)
                if ( token.at( token.length() - 1 ) == FilterEngine::WILDCARD_CHAR ) {
                    if ( token.length() == 2 ) return false;
                    strippedToken = token.substr(0, token.length() - 1);
                    type = PropertyFilterValue::STARTS_WITH;
                }
                
                // otherwise EXACT
                else {
                    strippedToken = token;
                    type = PropertyFilterValue::EXACT;
                }
                
                break;
        }
    }
    
    // convert stripped token to value
    std::stringstream stream(strippedToken);
    if ( strippedToken == "true" || strippedToken == "false" )
        stream >> std::boolalpha >> value;
    else 
        stream >> value;
    
    // check for valid CompareType on type T
    Variant variantCheck = value;
    
    // if T is not string AND CompareType is for string values, return false
    if ( !variantCheck.is_type<std::string>() ) {
        if ( type == PropertyFilterValue::CONTAINS || 
             type == PropertyFilterValue::ENDS_WITH || 
             type == PropertyFilterValue::STARTS_WITH )          
            
          return false;
    }
    
    // return success
    return true;
}

// sets property filter (value, type) for propertyName, on a particular filter set 
// setProperty("filter1", "mapQuality", 50, GREATER_THAN_EQUAL)
template<typename T>
bool FilterEngine::setProperty(const std::string& filterName, 
                               const std::string& propertyName, 
                               const T& value,
                               const PropertyFilterValue::ValueCompareType& type)
{
    // lookup filter by name, return false if not found
    FilterMap::iterator filterIter = m_filters.find(filterName);
    if ( filterIter == m_filters.end() ) return false;
      
    // lookup property for filter, add new PropertyFilterValue if not found, modify if already exists
    PropertyFilter& filter = (*filterIter).second;
    PropertyMap::iterator propertyIter = filter.Properties.find(propertyName);
    
    bool success;
    
    // property not found for this filter, create new entry
    if ( propertyIter == filter.Properties.end() )
        success = (filter.Properties.insert(std::make_pair(propertyName, PropertyFilterValue(value, type)))).second;
    
    // property already exists, modify
    else {
        PropertyFilterValue& filterValue = (*propertyIter).second;
        filterValue.Value = value;
        filterValue.Type  = type;
        success = true;
    }
    
    // if error so far, return false
    if ( !success ) return false;
    
    // --------------------------------------------
    // otherwise, set Property.IsEnabled to true
    
    // lookup property
    std::vector<Property>::iterator knownPropertyIter = std::find( m_properties.begin(), m_properties.end(), propertyName);
    
    // if not found, create a new (enabled) entry (& re-sort list)
    if ( knownPropertyIter == m_properties.end() ) {
        m_properties.push_back( Property(propertyName, true) );
        std::sort( m_properties.begin(), m_properties.end() );
    } 
    
    // property already known, set as enabled
    else 
        (*knownPropertyIter).IsEnabled = true;

    // return success
    return true;
}

// returns false if query does not pass any filters on 'propertyName' 
// returns true if property unknown (i.e. nothing has been set for this property... so query is considered to pass filter)
template<typename T>
bool FilterEngine::check(const std::string& propertyName, const T& query) {
  
    // check enabled properties list
    // return true if no properties enabled at all OR if property is unknown to FilterEngine
    const std::vector<std::string> enabledProperties = enabledPropertyNames();
    if ( enabledProperties.empty() ) return true;
    const bool found = std::binary_search( enabledProperties.begin(), enabledProperties.end(), propertyName );
    if ( !found ) return true;
    
    // iterate over all filters in FilterEngine
    FilterMap::const_iterator filterIter = m_filters.begin();
    FilterMap::const_iterator filterEnd  = m_filters.end();
    for ( ; filterIter != filterEnd; ++filterIter ) {
      
        // check query against this filter
        const PropertyFilter& filter = (*filterIter).second;
        if ( filter.check(propertyName, query) ) return true;
    }
 
    // query passes none of the filters with current property enabled
    return false;
}

} // namespace BamTools

#endif // BAMTOOLS_FILTER_ENGINE_H