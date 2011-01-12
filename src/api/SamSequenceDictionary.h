// ***************************************************************************
// SamSequenceDictionary.h (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 23 December 2010 (DB)
// ---------------------------------------------------------------------------
// Provides container operations for collection of sequence entries
// *************************************************************************

#ifndef SAM_SEQUENCE_DICTIONARY_H
#define SAM_SEQUENCE_DICTIONARY_H

#include <api/api_global.h>
#include <api/SamSequence.h>
#include <string>
#include <vector>

namespace BamTools {

typedef std::vector<SamSequence>             SamSequenceContainer;
typedef SamSequenceContainer::iterator       SamSequenceIterator;
typedef SamSequenceContainer::const_iterator SamSequenceConstIterator;

class API_EXPORT SamSequenceDictionary {

    // ctor & dtor
    public:
        SamSequenceDictionary(void);
        explicit SamSequenceDictionary(const SamSequenceDictionary& other);
        ~SamSequenceDictionary(void);

    // query/modify sequence data
    public:
        // add a sequence
        void Add(const SamSequence& sequence);
        void Add(const std::string& sequenceNames);

        // add multiple sequences
        void Add(const std::vector<SamSequence>& sequences);
        void Add(const std::vector<std::string>& sequenceNames);

        // clear all sequence records
        void Clear(void);

        // returns true if dictionary contains this sequence
        bool Contains(const SamSequence& sequence) const;
        bool Contains(const std::string& sequenceName) const;

        // returns true if dictionary is empty
        bool IsEmpty(void) const;

        // remove a single sequence (does nothing if sequence not found)
        void Remove(const SamSequence& sequence);
        void Remove(const std::string& sequenceName);

        // remove multiple sequences
        void Remove(const std::vector<SamSequence>& sequences);
        void Remove(const std::vector<std::string>& sequenceNames);

        // returns size of dictionary (number of current elements)
        int Size(void) const;

        // retrieves the SamSequence object associated with this name
        // if sequenceName is unknown, a new SamSequence is created with this name (and invalid length 0)
        // and a reference to this new sequence entry is returned (like std::map)
        //
        // * To avoid these partial entries being created, it is recommended to check
        //   for existence first using Contains()
        SamSequence& operator[](const std::string& sequenceName);

    // retrieve sequence iterators
    // these are typedefs for STL iterators and thus are compatible with STL containers/algorithms
    public:
        SamSequenceIterator      Begin(void);
        SamSequenceConstIterator Begin(void) const;
        SamSequenceConstIterator ConstBegin(void) const;
        SamSequenceIterator      End(void);
        SamSequenceConstIterator End(void) const;
        SamSequenceConstIterator ConstEnd(void) const;

    // internal methods
    private:
        int IndexOf(const SamSequence& sequence) const;
        int IndexOf(const std::string& sequenceName) const;

    // data members
    private:
        SamSequenceContainer m_data;
};

} // namespace BamTools

#endif // SAM_SEQUENCE_DICTIONARY 

