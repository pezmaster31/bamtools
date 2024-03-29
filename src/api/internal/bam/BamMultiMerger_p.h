// ***************************************************************************
// BamMultiMerger_p.h (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 October 2011 (DB)
// ---------------------------------------------------------------------------
// Provides merging functionality for BamMultiReader.  At this point, supports
// sorting results by (refId, position) or by read name.
// ***************************************************************************

#ifndef BAMMULTIMERGER_P_H
#define BAMMULTIMERGER_P_H

#include "api/api_global.h"

//  -------------
//  W A R N I N G
//  -------------
//
// This file is not part of the BamTools API.  It exists purely as an
// implementation detail. This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.

#include <deque>
#include <functional>
#include <set>
#include <string>
#include "api/BamAlignment.h"
#include "api/BamReader.h"
#include "api/algorithms/Sort.h"

namespace BamTools {
namespace Internal {

struct API_NO_EXPORT MergeItem
{

    // data members
    BamReader* Reader;
    BamAlignment* Alignment;

    // ctors & dtor
    MergeItem(BamReader* reader = 0, BamAlignment* alignment = 0)
        : Reader(reader)
        , Alignment(alignment)
    {}
};

template <typename Compare>
struct API_NO_EXPORT MergeItemSorter
{

public:
    MergeItemSorter(const Compare& comp = Compare())
        : m_comp(comp)
    {}

    bool operator()(const MergeItem& lhs, const MergeItem& rhs) const
    {
        const BamAlignment& l = *lhs.Alignment;
        const BamAlignment& r = *rhs.Alignment;
        return m_comp(l, r);
    }

private:
    Compare m_comp;
};

// pure ABC so we can just work polymorphically with any specific merger implementation
class API_NO_EXPORT IMultiMerger
{

public:
    IMultiMerger() {}
    virtual ~IMultiMerger() {}

public:
    virtual void Add(MergeItem item) = 0;
    virtual void Clear() = 0;
    virtual const MergeItem& First() const = 0;
    virtual bool IsEmpty() const = 0;
    virtual void Remove(BamReader* reader) = 0;
    virtual int Size() const = 0;
    virtual MergeItem TakeFirst() = 0;
};

// general merger
template <typename Compare>
class API_NO_EXPORT MultiMerger : public IMultiMerger
{

public:
    typedef Compare CompareType;
    typedef MergeItemSorter<CompareType> MergeType;

public:
    explicit MultiMerger(const Compare& comp = Compare())
        : IMultiMerger()
        , m_data(MergeType(comp))
    {}

public:
    void Add(MergeItem item);
    void Clear();
    const MergeItem& First() const;
    bool IsEmpty() const;
    void Remove(BamReader* reader);
    int Size() const;
    MergeItem TakeFirst();

private:
    typedef MergeItem ValueType;
    typedef std::multiset<ValueType, MergeType> ContainerType;
    typedef typename ContainerType::iterator DataIterator;
    typedef typename ContainerType::const_iterator DataConstIterator;
    ContainerType m_data;
};

template <typename Compare>
void MultiMerger<Compare>::Add(MergeItem item)
{

    // N.B. - any future custom Compare types must define this method
    //        see algorithms/Sort.h

    if (CompareType::UsesCharData()) {
        item.Alignment->BuildCharData();
    }
    m_data.insert(item);
}

template <typename Compare>
void MultiMerger<Compare>::Clear()
{
    m_data.clear();
}

template <typename Compare>
const MergeItem& MultiMerger<Compare>::First() const
{
    const ValueType& entry = (*m_data.begin());
    return entry;
}

template <typename Compare>
bool MultiMerger<Compare>::IsEmpty() const
{
    return m_data.empty();
}
template <typename Compare>
void MultiMerger<Compare>::Remove(BamReader* reader)
{

    if (reader == 0) {
        return;
    }
    const std::string& filenameToRemove = reader->GetFilename();

    // iterate over readers in cache
    DataIterator dataIter = m_data.begin();
    DataIterator dataEnd = m_data.end();
    for (; dataIter != dataEnd; ++dataIter) {
        const MergeItem& item = (*dataIter);
        const BamReader* itemReader = item.Reader;
        if (itemReader == 0) {
            continue;
        }

        // remove iterator on match
        if (itemReader->GetFilename() == filenameToRemove) {
            m_data.erase(dataIter);
            return;
        }
    }
}
template <typename Compare>
int MultiMerger<Compare>::Size() const
{
    return m_data.size();
}

template <typename Compare>
MergeItem MultiMerger<Compare>::TakeFirst()
{
    DataIterator firstIter = m_data.begin();
    MergeItem firstItem = (*firstIter);
    m_data.erase(firstIter);
    return firstItem;
}

// unsorted "merger"
template <>
class API_NO_EXPORT MultiMerger<Algorithms::Sort::Unsorted> : public IMultiMerger
{

public:
    explicit MultiMerger(const Algorithms::Sort::Unsorted& = Algorithms::Sort::Unsorted())
        : IMultiMerger()
    {}

public:
    void Add(MergeItem item);
    void Clear();
    const MergeItem& First() const;
    bool IsEmpty() const;
    void Remove(BamReader* reader);
    int Size() const;
    MergeItem TakeFirst();

private:
    typedef MergeItem ValueType;
    typedef std::deque<ValueType> ContainerType;
    typedef ContainerType::iterator DataIterator;
    typedef ContainerType::const_iterator DataConstIterator;
    ContainerType m_data;
};

inline void MultiMerger<Algorithms::Sort::Unsorted>::Add(MergeItem item)
{
    m_data.push_back(item);
}

inline void MultiMerger<Algorithms::Sort::Unsorted>::Clear()
{
    m_data.clear();
}

inline const MergeItem& MultiMerger<Algorithms::Sort::Unsorted>::First() const
{
    return m_data.front();
}

inline bool MultiMerger<Algorithms::Sort::Unsorted>::IsEmpty() const
{
    return m_data.empty();
}

inline void MultiMerger<Algorithms::Sort::Unsorted>::Remove(BamReader* reader)
{

    if (reader == 0) {
        return;
    }
    const std::string filenameToRemove = reader->GetFilename();

    // iterate over readers in cache
    DataIterator dataIter = m_data.begin();
    DataIterator dataEnd = m_data.end();
    for (; dataIter != dataEnd; ++dataIter) {
        const MergeItem& item = (*dataIter);
        const BamReader* itemReader = item.Reader;
        if (itemReader == 0) {
            continue;
        }

        // remove iterator on match
        if (itemReader->GetFilename() == filenameToRemove) {
            m_data.erase(dataIter);
            return;
        }
    }
}

inline int MultiMerger<Algorithms::Sort::Unsorted>::Size() const
{
    return m_data.size();
}

inline MergeItem MultiMerger<Algorithms::Sort::Unsorted>::TakeFirst()
{
    MergeItem firstItem = m_data.front();
    m_data.pop_front();
    return firstItem;
}

}  // namespace Internal
}  // namespace BamTools

#endif  // BAMMULTIMERGER_P_H
