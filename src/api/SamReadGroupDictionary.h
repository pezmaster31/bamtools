// ***************************************************************************
// SamReadGroupDictionary.h (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 23 December 2010 (DB)
// ---------------------------------------------------------------------------
// Provides container operations for collection of read group entries
// *************************************************************************

#ifndef SAM_READGROUP_DICTIONARY_H
#define SAM_READGROUP_DICTIONARY_H

#include <api/api_global.h>
#include <api/SamReadGroup.h>
#include <string>
#include <vector>

namespace BamTools {

typedef std::vector<SamReadGroup>             SamReadGroupContainer;
typedef SamReadGroupContainer::iterator       SamReadGroupIterator;
typedef SamReadGroupContainer::const_iterator SamReadGroupConstIterator;

// stores read groups
// can access read groups using SamReadGroup object or (std::string) read group ID tag
class API_EXPORT SamReadGroupDictionary {

    // ctor & dtor
    public:
        SamReadGroupDictionary(void);
        explicit SamReadGroupDictionary(const SamReadGroupDictionary& other);
        ~SamReadGroupDictionary(void);

    // query/modify read group data
    public:
        // add a read group
        void Add(const SamReadGroup& readGroup);
        void Add(const std::string& readGroupIds);

        // add multiple read groups
        void Add(const std::vector<SamReadGroup>& readGroups);
        void Add(const std::vector<std::string>& readGroupIds);

        // clear all read groups records
        void Clear(void);

        // returns true if dictionary contains this read group
        bool Contains(const SamReadGroup& readGroup) const;
        bool Contains(const std::string& readGroupId) const;

        // returns true if dictionary is empty
        bool IsEmpty(void) const;

        // remove a single read group (does nothing if read group not found)
        void Remove(const SamReadGroup& readGroup);
        void Remove(const std::string& readGroupId);

        // remove multiple read groups
        void Remove(const std::vector<SamReadGroup>& readGroups);
        void Remove(const std::vector<std::string>& readGroupIds);

        // returns size of dictionary (number of current elements)
        int Size(void) const;

        // retrieves the SamReadGroup object associated with this ID
        // if readGroupId is unknown, a new SamReadGroup is created with this ID (and no other data)
        // and a reference to this new read group entry is returned (like std::map)
        //
        // * To avoid these partial entries being created, it is recommended to check
        //   for existence first using Contains()
        SamReadGroup& operator[](const std::string& readGroupId);

    // retrieve read group iterators
    // these are typedefs for STL iterators and thus are compatible with STL containers/algorithms
    public:
        SamReadGroupIterator      Begin(void);
        SamReadGroupConstIterator Begin(void) const;
        SamReadGroupConstIterator ConstBegin(void) const;
        SamReadGroupIterator      End(void);
        SamReadGroupConstIterator End(void) const;
        SamReadGroupConstIterator ConstEnd(void) const;

    // internal methods
    private:
        int IndexOf(const SamReadGroup& readGroup) const;
        int IndexOf(const std::string& readGroupId) const;

    // data members
    private:
        SamReadGroupContainer m_data;
};

} // namespace BamTools

#endif // SAM_READGROUP_DICTIONARY 
