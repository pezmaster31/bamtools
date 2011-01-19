// ***************************************************************************
// BamMultiMerger_p.h (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 17 January 2011 (DB)
// ---------------------------------------------------------------------------
// Provides merging functionality for BamMultiReader.  At this point, supports
// sorting results by (refId, position) or by read name.
// ***************************************************************************

#ifndef BAMMULTIMERGER_P_H
#define BAMMULTIMERGER_P_H

//  -------------
//  W A R N I N G
//  -------------
//
// This file is not part of the BamTools API.  It exists purely as an
// implementation detail. This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.

#include <api/BamAlignment.h>
#include <api/BamReader.h>
#include <map>
#include <queue>
#include <string>
#include <utility>

namespace BamTools {
namespace Internal {

typedef std::pair<BamReader*, BamAlignment*> ReaderAlignment;

// generic MultiMerger interface
class IBamMultiMerger {

    public:
        IBamMultiMerger(void) { }
        virtual ~IBamMultiMerger(void) { }

    public:
        virtual void Add(const ReaderAlignment& value) =0;
        virtual void Clear(void) =0;
        virtual const ReaderAlignment& First(void) const =0;
        virtual const int Size(void) const =0;
        virtual ReaderAlignment TakeFirst(void) =0;
};

// IBamMultiMerger implementation - sorted on BamAlignment: (RefId, Position)
class PositionMultiMerger : public IBamMultiMerger {

    public:
        PositionMultiMerger(void) : IBamMultiMerger() { }
        ~PositionMultiMerger(void) { }

    public:
        void Add(const ReaderAlignment& value);
        void Clear(void);
        const ReaderAlignment& First(void) const;
        const int Size(void) const;
        ReaderAlignment TakeFirst(void);

    private:
        typedef std::pair<int, int>                     KeyType;
        typedef std::multimap<KeyType, ReaderAlignment> IndexType;
        typedef std::pair<KeyType, ReaderAlignment>     KeyValueType;
        typedef IndexType::iterator                     IndexIterator;
        typedef IndexType::const_iterator               IndexConstIterator;

        IndexType m_data;
};

// IBamMultiMerger implementation - sorted on BamAlignment: Name
class ReadNameMultiMerger : public IBamMultiMerger {

    public:
        ReadNameMultiMerger(void) : IBamMultiMerger() { }
        ~ReadNameMultiMerger(void) { }

    public:
        void Add(const ReaderAlignment& value);
        void Clear(void);
        const ReaderAlignment& First(void) const;
        const int Size(void) const;
        ReaderAlignment TakeFirst(void);

    private:
        typedef std::string                             KeyType;
        typedef std::multimap<KeyType, ReaderAlignment> IndexType;
        typedef std::pair<KeyType, ReaderAlignment>     KeyValueType;
        typedef IndexType::iterator                     IndexIterator;
        typedef IndexType::const_iterator               IndexConstIterator;

        IndexType m_data;
};

// IBamMultiMerger implementation - unsorted BAM file(s)
class UnsortedMultiMerger : public IBamMultiMerger {

    public:
        UnsortedMultiMerger(void) : IBamMultiMerger() { }
        ~UnsortedMultiMerger(void) { }

    public:
        void Add(const ReaderAlignment& value);
        void Clear(void);
        const ReaderAlignment& First(void) const;
        const int Size(void) const;
        ReaderAlignment TakeFirst(void);

    private:
        typedef std::queue<ReaderAlignment> IndexType;
        IndexType m_data;
};

// ------------------------------------------
// PositionMultiMerger implementation

inline void PositionMultiMerger::Add(const ReaderAlignment& value) {
    const KeyType key = std::make_pair<int, int>( value.second->RefID, value.second->Position );
    m_data.insert( KeyValueType(key, value) );
}

inline void PositionMultiMerger::Clear(void) {
    m_data.clear();
}

inline const ReaderAlignment& PositionMultiMerger::First(void) const {
    const KeyValueType& entry = (*m_data.begin());
    return entry.second;
}

inline const int PositionMultiMerger::Size(void) const {
    return m_data.size();
}

inline ReaderAlignment PositionMultiMerger::TakeFirst(void) {
    IndexIterator first = m_data.begin();
    ReaderAlignment next = (*first).second;
    m_data.erase(first);
    return next;
}

// ------------------------------------------
// ReadNameMultiMerger implementation

inline void ReadNameMultiMerger::Add(const ReaderAlignment& value) {
    const KeyType key = value.second->Name;
    m_data.insert( KeyValueType(key, value) );
}

inline void ReadNameMultiMerger::Clear(void) {
    m_data.clear();
}

inline const ReaderAlignment& ReadNameMultiMerger::First(void) const {
    const KeyValueType& entry = (*m_data.begin());
    return entry.second;
}

inline const int ReadNameMultiMerger::Size(void) const {
    return m_data.size();
}

inline ReaderAlignment ReadNameMultiMerger::TakeFirst(void) {
    IndexIterator first = m_data.begin();
    ReaderAlignment next = (*first).second;
    m_data.erase(first);
    return next;
}

// ------------------------------------------
// UnsortedMultiMerger implementation

inline void UnsortedMultiMerger::Add(const ReaderAlignment& value) {
    m_data.push(value);
}

inline void UnsortedMultiMerger::Clear(void) {
    for (size_t i = 0; i < m_data.size(); ++i )
        m_data.pop();
}

inline const ReaderAlignment& UnsortedMultiMerger::First(void) const {
    return m_data.front();
}

inline const int UnsortedMultiMerger::Size(void) const {
    return m_data.size();
}

inline ReaderAlignment UnsortedMultiMerger::TakeFirst(void) {
    ReaderAlignment first = m_data.front();
    m_data.pop();
    return first;
}

} // namespace Internal
} // namespace BamTools

#endif // BAMMULTIMERGER_P_H
