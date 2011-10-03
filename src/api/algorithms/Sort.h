// ***************************************************************************
// Sort.h (c) 2009 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 29 September 2011 (DB)
// ---------------------------------------------------------------------------
// Provides sorting functionality.
// ***************************************************************************

#ifndef ALGORITHMS_SORT_H
#define ALGORITHMS_SORT_H

#include <api/api_global.h>
#include <api/BamAlignment.h>
#include <api/BamReader.h>
#include <api/BamMultiReader.h>
#include <algorithm>
#include <cassert>
#include <functional>
#include <string>
#include <vector>

namespace BamTools {
namespace Algorithms {

struct API_EXPORT Sort {

    enum Order { AscendingOrder = 0
               , DescendingOrder
               };

    template<typename ElemType>
    static inline bool sort_helper(const Sort::Order& order,
                                   const ElemType& lhs,
                                   const ElemType& rhs)
    {
        switch ( order ) {
            case ( Sort::AscendingOrder  ) : { std::less<ElemType> comp;    return comp(lhs, rhs); }
            case ( Sort::DescendingOrder ) : { std::greater<ElemType> comp; return comp(lhs, rhs); }
            default : assert(false);
        }
        return false; // <-- unreachable
    }

    typedef std::binary_function<BamAlignment, BamAlignment, bool> AlignmentSortBase;

    // compare alignments by name
    struct ByName : public AlignmentSortBase {

        ByName(const Sort::Order& order = Sort::AscendingOrder)
            : m_order(order)
        { }

        bool operator()(const BamTools::BamAlignment& lhs,
                        const BamTools::BamAlignment& rhs)
        {
            return sort_helper(m_order, lhs.Name, rhs.Name);
        }

        static inline bool UsesCharData(void) { return true; }

        private:
            const Sort::Order& m_order;
    };

    // compare alignments by position
    struct ByPosition : public AlignmentSortBase {

        ByPosition(const Sort::Order& order = Sort::AscendingOrder)
            : m_order(order)
        { }

        bool operator()(const BamTools::BamAlignment& lhs,
                        const BamTools::BamAlignment& rhs)
        {
            // force unmapped aligmnents to end
            if ( lhs.RefID == -1 ) return false;
            if ( rhs.RefID == -1 ) return true;

            // if on same reference, sort on position
            if ( lhs.RefID == rhs.RefID )
                return sort_helper(m_order, lhs.Position, rhs.Position);

            // otherwise sort on reference ID
            return sort_helper(m_order, lhs.RefID, rhs.RefID);
        }

        static inline bool UsesCharData(void) { return false; }

        private:
            Sort::Order m_order;
    };

    // compare alignments by tag value
    template<typename T>
    struct ByTag : public AlignmentSortBase {

        ByTag(const std::string& tag,
              const Sort::Order& order = Sort::AscendingOrder)
            : m_tag(tag)
            , m_order(order)
        { }

        bool operator()(const BamTools::BamAlignment& lhs,
                        const BamTools::BamAlignment& rhs)
        {
            // force alignments without tag to end
            T lhsTagValue;
            T rhsTagValue;
            if ( !lhs.GetTag(m_tag, lhsTagValue) ) return false;
            if ( !rhs.GetTag(m_tag, rhsTagValue) ) return true;

            // otherwise compare on tag values
            return sort_helper(m_order, lhsTagValue, rhsTagValue);
        }

        static inline bool UsesCharData(void) { return true; }

        private:
            std::string m_tag;
            Sort::Order m_order;
    };

    // essentially a placeholder comparison object, ignores the alignments' data
    // N.B. - returning false tends to retain insertion order
    struct Unsorted : public AlignmentSortBase {
        inline bool operator()(const BamTools::BamAlignment& /*lhs*/,
                               const BamTools::BamAlignment& /*rhs*/)
        {
            return false;
        }

        static inline bool UsesCharData(void) { return false; }
    };
};

// in-place sorting of alignment vector
template<typename Compare>
inline
void SortAlignments(std::vector<BamAlignment>& data,
                    const Compare& comp = Compare())
{
    std::sort(data.begin(), data.end(), comp);
}

// returns sorted copy of input alignment vector
template<typename Compare>
inline
std::vector<BamAlignment> SortAlignmentsCopy(const std::vector<BamAlignment>& input,
                                             const Compare& comp = Compare())
{
    std::vector<BamAlignment> output(input);
    SortAlignments(output, comp);
    return output;
}

// pulls a region from a position-sorted BAM file
// returns the alignments sorted by user-defined Compare type
template<typename Compare>
std::vector<BamAlignment> GetSortedRegion(BamReader& reader,
                                          const BamRegion& region,
                                          const Compare& comp = Compare())
{
    // return empty container if unable to find region
    if ( !reader.IsOpen() )          return std::vector<BamAlignment>();
    if ( !reader.SetRegion(region) ) return std::vector<BamAlignment>();

    // iterate through region, grabbing alignments
    BamAlignment al;
    std::vector<BamAlignment> results;
    while ( reader.GetNextAlignmentCore(al) )
        results.push_back(al);

    // sort & return alignments
    SortAlignments(results, comp);
    return results;
}

template<typename Compare>
std::vector<BamAlignment> GetSortedRegion(BamMultiReader& reader,
                                          const BamRegion& region,
                                          const Compare& comp = Compare())
{
    // return empty container if unable to find region
    if ( !reader.HasOpenReaders() )  return std::vector<BamAlignment>();
    if ( !reader.SetRegion(region) ) return std::vector<BamAlignment>();

    // iterate through region, grabbing alignments
    BamAlignment al;
    std::vector<BamAlignment> results;
    while ( reader.GetNextAlignmentCore(al) )
        results.push_back(al);

    // sort & return alignments
    SortAlignments(results, comp);
    return results;
}

} // namespace Algorithms
} // namespace BamTools

#endif // ALGORITHMS_SORT_H
