#include <api/SamSequenceDictionary.h>
using namespace BamTools;

#include <iostream>
using namespace std;

// ctor
SamSequenceDictionary::SamSequenceDictionary(void) { }

// dtor
SamSequenceDictionary::~SamSequenceDictionary(void) {
    m_data.clear();
}

// adds sequence if not already in container
void SamSequenceDictionary::Add(const SamSequence& sequence) {
    if ( IsEmpty() || !Contains(sequence) )
        m_data.push_back(sequence);
}

// overload to support std::string
void SamSequenceDictionary::Add(const string& sequenceName) {
    Add( SamSequence(sequenceName) );
}

// add multiple sequences
void SamSequenceDictionary::Add(const vector<SamSequence>& sequences) {
    vector<SamSequence>::const_iterator rgIter = sequences.begin();
    vector<SamSequence>::const_iterator rgEnd  = sequences.end();
    for ( ; rgIter!= rgEnd; ++rgIter )
        Add(*rgIter);
}

// overload to support std::string
void SamSequenceDictionary::Add(const vector<string>& sequenceNames) {
    vector<string>::const_iterator rgIter = sequenceNames.begin();
    vector<string>::const_iterator rgEnd  = sequenceNames.end();
    for ( ; rgIter!= rgEnd; ++rgIter )
        Add(*rgIter);
}

// returns iterator to container begin
SamSequenceIterator SamSequenceDictionary::Begin(void) {
    return m_data.begin();
}

// returns const_iterator to container begin
SamSequenceConstIterator SamSequenceDictionary::Begin(void) const {
    return m_data.begin();
}

// clear sequence container
void SamSequenceDictionary::Clear(void) {
    m_data.clear();
}

// explicit request for const_iterator to container begin
SamSequenceConstIterator SamSequenceDictionary::ConstBegin(void) const {
    return m_data.begin();
}

// explicit request for const_iterator to container end
SamSequenceConstIterator SamSequenceDictionary::ConstEnd(void) const {
    return m_data.end();
}

// returns true if container contains a sequence with this ID tag
bool SamSequenceDictionary::Contains(const string& sequenceName) const {
    return ( IndexOf(sequenceName) != (int)m_data.size() );
}

bool SamSequenceDictionary::Contains(const SamSequence& seq) const {
    return ( IndexOf(seq) != (int)m_data.size() );
}

// returns iterator to container end
SamSequenceIterator SamSequenceDictionary::End(void) {
    return m_data.end();
}

// returns const_iterator to container begin
SamSequenceConstIterator SamSequenceDictionary::End(void) const {
    return m_data.end();
}

// returns vector index of sequence if found
// returns vector::size() (invalid index) if not found
int SamSequenceDictionary::IndexOf(const SamSequence& sequence) const {
    SamSequenceConstIterator begin = ConstBegin();
    SamSequenceConstIterator iter  = begin;
    SamSequenceConstIterator end   = ConstEnd();
    for ( ; iter != end; ++iter )
        if ( *iter == sequence ) break;
    return distance( begin, iter );
}

// overload to support std::string
int SamSequenceDictionary::IndexOf(const string& sequenceName) const {
    return IndexOf( SamSequence(sequenceName) );
}

// returns true if container is empty
bool SamSequenceDictionary::IsEmpty(void) const {
    return m_data.empty();
}

// removes sequence (if it exists)
void SamSequenceDictionary::Remove(const SamSequence& sequence) {
    if ( Contains(sequence) )
        m_data.erase( m_data.begin() + IndexOf(sequence) );
}

// overlaod to support std::string
void SamSequenceDictionary::Remove(const string& sequenceName) {
    Remove( SamSequence(sequenceName) );
}

// remove multiple sequences
void SamSequenceDictionary::Remove(const vector<SamSequence>& sequences) {
    vector<SamSequence>::const_iterator rgIter = sequences.begin();
    vector<SamSequence>::const_iterator rgEnd  = sequences.end();
    for ( ; rgIter!= rgEnd; ++rgIter )
        Remove(*rgIter);
}

// overload to support std::string
void SamSequenceDictionary::Remove(const vector<string>& sequenceNames) {
    vector<string>::const_iterator rgIter = sequenceNames.begin();
    vector<string>::const_iterator rgEnd  = sequenceNames.end();
    for ( ; rgIter!= rgEnd; ++rgIter )
        Remove(*rgIter);
}

// returns size of container (number of current sequences)
int SamSequenceDictionary::Size(void) const {
    return m_data.size();
}

// retrieves the SamSequence object associated with this name
// if sequenceName is unknown, a new SamSequence is created with this name (and invalid length 0)
// and a reference to this new sequence entry is returned (like std::map)
SamSequence& SamSequenceDictionary::operator[](const std::string& sequenceName) {

    // look up sequence ID
    int index = IndexOf(sequenceName);

    // if found, return sequence at index
    if ( index != (int)m_data.size() )
        return m_data[index];

    // otherwise, append new sequence and return reference
    else {
        SamSequence seq(sequenceName);
        seq.Length = "0";
        m_data.push_back(seq);
        return m_data.back();
    }
}

