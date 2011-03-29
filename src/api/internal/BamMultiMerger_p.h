// ***************************************************************************
// BamMultiMerger_p.h (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 18 March 2011 (DB)
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
#include <set>
#include <queue>
#include <string>
#include <utility>

namespace BamTools {
namespace Internal {

typedef std::pair<BamReader*, BamAlignment*> ReaderAlignment;

// generic MultiMerger interface
class IBamMultiMerger {

    public:
        IBamMultiMerger(){ }
        virtual ~IBamMultiMerger(void) { }

    public:
        virtual void Add(const ReaderAlignment& value) =0;
        virtual void Clear(void) =0;
        virtual const ReaderAlignment& First(void) const =0;
        virtual bool IsEmpty(void) const =0;
        virtual void Remove(BamReader* reader) =0;
        virtual int Size(void) const =0;
        virtual ReaderAlignment TakeFirst(void) =0;

};


template<typename BF>
class CommonMultiMerger : public IBamMultiMerger{
  public:
        CommonMultiMerger()  { }
        ~CommonMultiMerger(void) { }

    public:
        void Add(const ReaderAlignment& value);
        void Clear(void);
        const ReaderAlignment& First(void) const;
        bool IsEmpty(void) const;
        void Remove(BamReader* reader);
        int Size(void) const;
        ReaderAlignment TakeFirst(void);
    private:
        typedef ReaderAlignment               ValueType;
        typedef std::multiset<ValueType, BF> ContainerType;
        typedef typename ContainerType::iterator           DataIterator;
        typedef typename ContainerType::const_iterator     DataConstIterator;
        ContainerType m_data;
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
        bool IsEmpty(void) const;
        void Remove(BamReader* reader);
        int Size(void) const;
        ReaderAlignment TakeFirst(void);

    private:
        typedef ReaderAlignment ElementType;
        typedef std::vector<ReaderAlignment>  ContainerType;
        typedef ContainerType::iterator       DataIterator;
        typedef ContainerType::const_iterator DataConstIterator;

        ContainerType m_data;
};


//---------------------------------------------------------------------------
// CommonMultiMerger implementation
template <typename BF>
inline void CommonMultiMerger<BF>::Add(const ReaderAlignment& value) {
    m_data.insert( value);
}

template <typename BF>
inline void CommonMultiMerger<BF>::Clear(void) {
    m_data.clear();
}

template <typename BF>
inline const ReaderAlignment& CommonMultiMerger<BF>::First(void) const {
    const ValueType& entry = (*m_data.begin());
    return entry;
}

template <typename BF>
inline bool CommonMultiMerger<BF>::IsEmpty(void) const {
    return m_data.empty();
}
template <typename BF>
inline void CommonMultiMerger<BF>::Remove(BamReader* reader) {
    if ( reader == 0 ) return;
    const std::string filenameToRemove = reader->GetFilename();
    // iterate over readers in cache
    DataIterator dataIter = m_data.begin();
    DataIterator dataEnd  = m_data.end();
    for ( ; dataIter != dataEnd; ++dataIter ) {
        const ValueType& entry = (*dataIter);
        const BamReader* entryReader = entry.first;
        if ( entryReader == 0 ) continue;
        // remove iterator on match
        if ( entryReader->GetFilename() == filenameToRemove ) {
            m_data.erase(dataIter);
            return;
        }
    }
}
template <typename BF>
inline int CommonMultiMerger<BF>::Size(void) const {
    return m_data.size();
}
template <typename BF>
inline ReaderAlignment CommonMultiMerger<BF>::TakeFirst(void) {
    DataIterator first = m_data.begin();
    ReaderAlignment next = (*first);
    m_data.erase(first);
    return next;
}

//----------------------------------------------------------------
// MultiMerger for Unsorted Files

inline void UnsortedMultiMerger::Add(const ReaderAlignment& value) {
    m_data.push_back(value);
}

inline void UnsortedMultiMerger::Clear(void) {
    for (size_t i = 0; i < m_data.size(); ++i )
        m_data.pop_back();
}

inline const ReaderAlignment& UnsortedMultiMerger::First(void) const {
    return m_data.front();
}

inline bool UnsortedMultiMerger::IsEmpty(void) const {
    return m_data.empty();
}

inline void UnsortedMultiMerger::Remove(BamReader* reader) {

    if ( reader == 0 ) return;
    const std::string filenameToRemove = reader->GetFilename();

    // iterate over readers in cache
    DataIterator dataIter = m_data.begin();
    DataIterator dataEnd  = m_data.end();
    for ( ; dataIter != dataEnd; ++dataIter ) {
        const BamReader* entryReader = (*dataIter).first;
        if ( entryReader == 0 ) continue;

        // remove iterator on match
        if ( entryReader->GetFilename() == filenameToRemove ) {
            m_data.erase(dataIter);
            return;
        }
    }
}

inline int UnsortedMultiMerger::Size(void) const {
    return m_data.size();
}

inline ReaderAlignment UnsortedMultiMerger::TakeFirst(void) {
    ReaderAlignment first = m_data.front();
    m_data.erase( m_data.begin() );
    return first;
}

} // namespace Internal
} // namespace BamTools

#endif // BAMMULTIMERGER_P_H
