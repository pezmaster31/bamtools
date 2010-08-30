// ***************************************************************************
// bamtools_filter_engine.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 30 August 2010
// ---------------------------------------------------------------------------
// 
// ***************************************************************************

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

// --------------------------------------------------------
// PropertyFilter implementation
PropertyFilter::PropertyFilter(void)
    : Type(PropertyFilter::EXACT)
    , LeftChild(0)
    , RightChild(0)
{ }

PropertyFilter::~PropertyFilter(void) {
    delete LeftChild;
    LeftChild = 0;
  
    delete RightChild;
    RightChild = 0;
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

// ================================================================
// DEBUGGING

void FilterEngine::print(void) {
    cout << endl;
    printAllProperties();
    printEnabledProperties();
    printFilters();
}

void FilterEngine::printAllProperties(void) {
    
    cout << "=======================================" << endl;
    cout << "All Properties: " << endl;
    cout << endl;
  
    const vector<string> propertyNames = allPropertyNames();
    vector<string>::const_iterator nameIter = propertyNames.begin();
    vector<string>::const_iterator nameEnd  = propertyNames.end();
    for ( ; nameIter != nameEnd; ++nameIter )
        cout << (*nameIter) << endl;
    cout << endl;
}

void FilterEngine::printEnabledProperties(void) {
    
    cout << "=======================================" << endl;
    cout << "Enabled Properties: " << endl;
    cout << endl;
  
    const vector<string> propertyNames = enabledPropertyNames();
    vector<string>::const_iterator nameIter = propertyNames.begin();
    vector<string>::const_iterator nameEnd  = propertyNames.end();
    for ( ; nameIter != nameEnd; ++nameIter )
        cout << (*nameIter) << endl;
    cout << endl;
}

void FilterEngine::printFilters(void) {
  
    cout << "=======================================" << endl;
    cout << "Current Filters: " << endl;
    cout << endl;
    
    // iterate over all filters in FilterEngine
    FilterMap::const_iterator filterIter = m_filters.begin();
    FilterMap::const_iterator filterEnd  = m_filters.end();
    for ( ; filterIter != filterEnd; ++filterIter ) {
        cout << "Filter Name: " << (*filterIter).first << endl;
      
        // see if filter set has this property enabled
        const PropertyFilter& filter = (*filterIter).second;
        PropertyMap::const_iterator propIter = filter.Properties.begin();
        PropertyMap::const_iterator propEnd  = filter.Properties.end();
        for ( ; propIter != propEnd; ++propIter ) {
          
            cout << " - " << (*propIter).first << " : ";
            const PropertyFilterValue& filterValue = (*propIter).second;
             
            if ( filterValue.Value.is_type<bool>() )              cout << "\t" << boolalpha << filterValue.Value.get<bool>();
            else if ( filterValue.Value.is_type<int>() )          cout << "\t" << filterValue.Value.get<int>();
            else if ( filterValue.Value.is_type<unsigned int>() ) cout << "\t" << filterValue.Value.get<unsigned int>();
            else if ( filterValue.Value.is_type<unsigned short>() ) cout << "\t" << filterValue.Value.get<unsigned short>();
            else if ( filterValue.Value.is_type<float>() )        cout << "\t" << filterValue.Value.get<float>();
            else if ( filterValue.Value.is_type<string>() )  cout << "\t" << filterValue.Value.get<string>();
            else cout << "** UNKNOWN VALUE TYPE!! **";
                
            switch( filterValue.Type ) {
                case (PropertyFilterValue::CONTAINS)           : cout << " (contains)"; break;
                case (PropertyFilterValue::ENDS_WITH)          : cout << " (ends_with)"; break;
                case (PropertyFilterValue::EXACT)              : cout << " (exact)"; break;
                case (PropertyFilterValue::GREATER_THAN)       : cout << " (greater_than)"; break;
                case (PropertyFilterValue::GREATER_THAN_EQUAL) : cout << " (greater_than_equal)"; break;
                case (PropertyFilterValue::LESS_THAN)          : cout << " (less_than)"; break;
                case (PropertyFilterValue::LESS_THAN_EQUAL)    : cout << " (less_than_equal)"; break;
                case (PropertyFilterValue::NOT)                : cout << " (not)"; break;
                case (PropertyFilterValue::STARTS_WITH)        : cout << " (starts_with)"; break;
                default : cout << " : ** UNKNOWN COMPARE TYPE!! **";
            }
            cout << endl;
        }
    }
}
