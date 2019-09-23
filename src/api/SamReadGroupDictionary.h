// ***************************************************************************
// SamReadGroupDictionary.h (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 16 October 2011 (DB)
// ---------------------------------------------------------------------------
// Provides methods for operating on a collection of SamReadGroup entries.
// ***************************************************************************

#ifndef SAM_READGROUP_DICTIONARY_H
#define SAM_READGROUP_DICTIONARY_H

#include <cstddef>
#include <map>
#include <string>
#include <vector>
#include "api/SamReadGroup.h"
#include "api/api_global.h"

namespace BamTools {

typedef std::vector<SamReadGroup> SamReadGroupContainer;
typedef SamReadGroupContainer::iterator SamReadGroupIterator;
typedef SamReadGroupContainer::const_iterator SamReadGroupConstIterator;

class API_EXPORT SamReadGroupDictionary
{

    // ctor & dtor
public:
    SamReadGroupDictionary();

    // query/modify read group data
public:
    // adds a read group
    void Add(const SamReadGroup& readGroup);
    void Add(const std::string& readGroupId);

    // adds multiple read groups
    void Add(const SamReadGroupDictionary& readGroups);
    void Add(const std::vector<SamReadGroup>& readGroups);
    void Add(const std::vector<std::string>& readGroupIds);

    // clears all read group entries
    void Clear();

    // returns true if dictionary contains this read group
    bool Contains(const SamReadGroup& readGroup) const;
    bool Contains(const std::string& readGroupId) const;

    // returns true if dictionary is empty
    bool IsEmpty() const;

    // removes read group, if found
    void Remove(const SamReadGroup& readGroup);
    void Remove(const std::string& readGroupId);

    // removes multiple read groups
    void Remove(const std::vector<SamReadGroup>& readGroups);
    void Remove(const std::vector<std::string>& readGroupIds);

    // returns number of read groups in dictionary
    int Size() const;

    // retrieves a modifiable reference to the SamReadGroup object associated with this ID
    SamReadGroup& operator[](const std::string& readGroupId);

    // retrieve STL-compatible iterators
public:
    SamReadGroupIterator Begin();                  // returns iterator to begin()
    SamReadGroupConstIterator Begin() const;       // returns const_iterator to begin()
    SamReadGroupConstIterator ConstBegin() const;  // returns const_iterator to begin()
    SamReadGroupIterator End();                    // returns iterator to end()
    SamReadGroupConstIterator End() const;         // returns const_iterator to end()
    SamReadGroupConstIterator ConstEnd() const;    // returns const_iterator to end()

    // data members
private:
    SamReadGroupContainer m_data;
    std::map<std::string, std::size_t> m_lookupData;
};

}  // namespace BamTools

#endif  // SAM_READGROUP_DICTIONARY_H
