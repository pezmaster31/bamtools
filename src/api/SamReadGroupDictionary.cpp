// ***************************************************************************
// SamReadGroupDictionary.cpp (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 23 December 2010 (DB)
// ---------------------------------------------------------------------------
// Provides container operations for collection of read group entries
// *************************************************************************

#include <api/SamReadGroupDictionary.h>
using namespace BamTools;

#include <algorithm>
#include <iostream>
using namespace std;

// ctor
SamReadGroupDictionary::SamReadGroupDictionary(void) { }

// dtor
SamReadGroupDictionary::~SamReadGroupDictionary(void) {
    m_data.clear();
}

// adds read group if not already in container
void SamReadGroupDictionary::Add(const SamReadGroup& readGroup) {
    if ( IsEmpty() || !Contains(readGroup) )
        m_data.push_back(readGroup);
}

// overload to support std::string
void SamReadGroupDictionary::Add(const string& readGroupId) {
    Add( SamReadGroup(readGroupId) );
}

// add multiple read groups
void SamReadGroupDictionary::Add(const vector<SamReadGroup>& readGroups) {
    vector<SamReadGroup>::const_iterator rgIter = readGroups.begin();
    vector<SamReadGroup>::const_iterator rgEnd  = readGroups.end();
    for ( ; rgIter!= rgEnd; ++rgIter )
        Add(*rgIter);
}

// overload to support std::string
void SamReadGroupDictionary::Add(const vector<string>& readGroupIds) {
    vector<string>::const_iterator rgIter = readGroupIds.begin();
    vector<string>::const_iterator rgEnd  = readGroupIds.end();
    for ( ; rgIter!= rgEnd; ++rgIter )
        Add(*rgIter);
}

// returns iterator to container begin
SamReadGroupIterator SamReadGroupDictionary::Begin(void) {
    return m_data.begin();
}

// returns const_iterator to container begin
SamReadGroupConstIterator SamReadGroupDictionary::Begin(void) const {
    return m_data.begin();
}

// clear read group container
void SamReadGroupDictionary::Clear(void) {
    m_data.clear();
}

// explicit request for const_iterator to container begin
SamReadGroupConstIterator SamReadGroupDictionary::ConstBegin(void) const {
    return m_data.begin();
}

// explicit request for const_iterator to container end
SamReadGroupConstIterator SamReadGroupDictionary::ConstEnd(void) const {
    return m_data.end();
}

// returns true if container contains a read group with this ID tag
bool SamReadGroupDictionary::Contains(const string& readGroupId) const {
    return ( IndexOf(readGroupId) != (int)m_data.size() );
}

bool SamReadGroupDictionary::Contains(const SamReadGroup& readGroup) const {
    return ( IndexOf(readGroup) != (int)m_data.size() );
}

// returns iterator to container end
SamReadGroupIterator SamReadGroupDictionary::End(void) {
    return m_data.end();
}

// returns const_iterator to container begin
SamReadGroupConstIterator SamReadGroupDictionary::End(void) const {
    return m_data.end();
}

// returns vector index of read group if found
// returns vector::size() (invalid index) if not found
int SamReadGroupDictionary::IndexOf(const SamReadGroup& readGroup) const {
    SamReadGroupConstIterator begin = ConstBegin();
    SamReadGroupConstIterator iter  = begin;
    SamReadGroupConstIterator end   = ConstEnd();
    for ( ; iter != end; ++iter )
        if ( *iter == readGroup ) break;
    return distance( begin, iter );
}

// overload to support std::string
int SamReadGroupDictionary::IndexOf(const string& readGroupId) const {
    return IndexOf( SamReadGroup(readGroupId) );
}

// returns true if container is empty
bool SamReadGroupDictionary::IsEmpty(void) const {
    return m_data.empty();
}

// removes read group (if it exists)
void SamReadGroupDictionary::Remove(const SamReadGroup& readGroup) {
    if ( Contains(readGroup) )
        m_data.erase( m_data.begin() + IndexOf(readGroup) );
}

// overlaod to support std::string
void SamReadGroupDictionary::Remove(const string& readGroupId) {
    Remove( SamReadGroup(readGroupId) );
}

// remove multiple read groups
void SamReadGroupDictionary::Remove(const vector<SamReadGroup>& readGroups) {
    vector<SamReadGroup>::const_iterator rgIter = readGroups.begin();
    vector<SamReadGroup>::const_iterator rgEnd  = readGroups.end();
    for ( ; rgIter!= rgEnd; ++rgIter )
        Remove(*rgIter);
}

// overload to support std::string
void SamReadGroupDictionary::Remove(const vector<string>& readGroupIds) {
    vector<string>::const_iterator rgIter = readGroupIds.begin();
    vector<string>::const_iterator rgEnd  = readGroupIds.end();
    for ( ; rgIter!= rgEnd; ++rgIter )
        Remove(*rgIter);
}

// returns size of container (number of current read groups)
int SamReadGroupDictionary::Size(void) const {
    return m_data.size();
}

// retrieves the SamReadGroup object associated with this ID
// if readGroupId is unknown, a new SamReadGroup is created with this ID
// and a reference to this new read group entry is returned (like std::map)
SamReadGroup& SamReadGroupDictionary::operator[](const std::string& readGroupId) {

    // look up read group ID
    int index = IndexOf(readGroupId);

    // if found, return read group at index
    if ( index != (int)m_data.size() )
        return m_data[index];

    // otherwise, append new read group and return reference
    else {
        SamReadGroup rg(readGroupId);
        m_data.push_back(rg);
        return m_data.back();
    }
}
