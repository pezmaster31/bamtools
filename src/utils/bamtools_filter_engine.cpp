// ***************************************************************************
// bamtools_filter_engine.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 30 August 2010
// ---------------------------------------------------------------------------
// 
// ***************************************************************************

#include <iostream>
#include "bamtools_filter_engine.h"
#include "BamAux.h"
using namespace std;
using namespace BamTools;

// ---------------------------------------------------------
// FilterValue implementation

// checks a string query against filter (value, compare type)
bool PropertyFilterValue::check(const string& query) const {
  
    // ensure filter value & query are same type
    if ( !Value.is_type<string>() ) {
        cerr << "Cannot compare different types!" << endl;
        return false;
    }
  
    // localize string version of our filter value
    const string& valueString = Value.get<string>();
    
    // string matching based on our filter type
    switch ( Type ) {
        case ( PropertyFilterValue::CONTAINS)           : return ( query.find(valueString) != string::npos );
        case ( PropertyFilterValue::ENDS_WITH)          : return ( query.find(valueString) == (query.length() - valueString.length()) ); 
        case ( PropertyFilterValue::EXACT)              : return ( query == valueString );
        case ( PropertyFilterValue::GREATER_THAN)       : return ( query >  valueString ); 
        case ( PropertyFilterValue::GREATER_THAN_EQUAL) : return ( query >= valueString ); 
        case ( PropertyFilterValue::LESS_THAN)          : return ( query <  valueString );
        case ( PropertyFilterValue::LESS_THAN_EQUAL)    : return ( query <= valueString );
        case ( PropertyFilterValue::NOT)                : return ( query != valueString );
        case ( PropertyFilterValue::STARTS_WITH)        : return ( query.find(valueString) == 0 );
        default : BAMTOOLS_ASSERT_UNREACHABLE;
    }
    return false;
}

// ---------------------------------------------------------
// FilterEngine implementation

// static FilterEngine data members
FilterMap FilterEngine::m_filters;
vector<Property> FilterEngine::m_properties;

// creates a new filter set, returns true if created, false if error or already exists
bool FilterEngine::addFilter(const string& filterName) {
    return (m_filters.insert(make_pair(filterName, PropertyFilter()))).second;
}

// return list of current filter names
const vector<string> FilterEngine::filterNames(void) {
    vector<string> names;
    names.reserve(m_filters.size());
    FilterMap::const_iterator mapIter = m_filters.begin();
    FilterMap::const_iterator mapEnd  = m_filters.end();
    for ( ; mapIter != mapEnd; ++mapIter )
        names.push_back( (*mapIter).first ); 
    return names;
}

// add a new known property (& type) to engine
bool FilterEngine::addProperty(const string& propertyName) {
    const vector<string> propertyNames = allPropertyNames();
    bool found = binary_search( propertyNames.begin(), propertyNames.end(), propertyName );
    if ( found ) return false;
    m_properties.push_back( Property(propertyName) );
    sort( m_properties.begin(), m_properties.end() );
    return true;
}


// returns list of all properties known by FilterEngine  ( any created using addProperty() )
const vector<string> FilterEngine::allPropertyNames(void) {
    vector<string> names;
    names.reserve(m_properties.size());
    vector<Property>::const_iterator propIter = m_properties.begin();
    vector<Property>::const_iterator propEnd  = m_properties.end();
    for ( ; propIter != propEnd; ++propIter )
        names.push_back( (*propIter).Name );    
    return names;
}

// returns list of property names that are 'enabled' ( only those touched by setProperty() )
const vector<string> FilterEngine::enabledPropertyNames(void) {
    vector<string> names;
    names.reserve(m_properties.size());
    vector<Property>::const_iterator propIter = m_properties.begin();
    vector<Property>::const_iterator propEnd  = m_properties.end();
    for ( ; propIter != propEnd; ++propIter )
        if ( (*propIter).IsEnabled ) names.push_back( (*propIter).Name );    
    return names;
}
