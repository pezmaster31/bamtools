// ***************************************************************************
// BamAlgorithms.h (c) 2009 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 29 September 2011 (DB)
// ---------------------------------------------------------------------------
// Provides generic algorithms that are intended to work with BamTools data
// structures. Where possible, these are intended to be STL-compatible.
// ***************************************************************************

#ifndef BAMALGORITHMS_H
#define BAMALGORITHMS_H

#include <api/api_global.h>
#include <api/BamAlignment.h>
#include <api/BamReader.h>
#include <api/BamMultiReader.h>
#include <algorithm>
#include <functional>
#include <string>
#include <vector>

namespace BamTools {
namespace Algorithms {

// -------------------------------------------------------
// Built-in function objects for comparing BamAlignments

typedef std::binary_function<BamAlignment, BamAlignment, bool> BamAlignmentComparer;

// Algorithms::SortByName<Compare>
// compare alignments by name (default comparison is std::less<std::string> )
template<template <typename> class Compare = std::less>
struct API_EXPORT SortByName : public BamAlignmentComparer {
    bool operator()(const BamTools::BamAlignment& lhs,
                    const BamTools::BamAlignment& rhs)
    {
        Compare<std::string> comp;
        return comp(lhs.Name, rhs.Name);
    }
};

// Algorithms::SortByPosition<Compare>
// compare alignments by position (default comparison is std::less<int>)
template<template <typename> class Compare = std::less>
struct API_EXPORT SortByPosition : public BamAlignmentComparer {
    bool operator()(const BamTools::BamAlignment& lhs,
                    const BamTools::BamAlignment& rhs)
    {
        // force unmapped aligmnents to end
        if ( lhs.RefID == -1 ) return false;
        if ( rhs.RefID == -1 ) return true;

        // otherwise compare first on RefID, then position
        Compare<int32_t> comp;
        if ( lhs.RefID == rhs.RefID )
            return comp(lhs.Position, rhs.Position);
        return comp(lhs.RefID, rhs.RefID);
    }
};

// Algorithms::SortByTag<Compare>("XY")
// compare alignments by tag value (default comparison is std::less<T>)
// where T is the expected tag type (e.g. RG -> string, NM -> int, etc.)
template<typename T, template <typename> class Compare = std::less>
struct API_EXPORT SortByTag : public BamAlignmentComparer {

    // ctor - needed to provide the tag name ("RG", "NM", "Aq", etc)
    explicit SortByTag(const std::string& tag) : m_tag(tag) { }

    bool operator()(const BamTools::BamAlignment& lhs,
                    const BamTools::BamAlignment& rhs)
    {
        // force alignments without tag to end
        T lhsTagValue;
        T rhsTagValue;
        if ( !lhs.GetTag(m_tag, lhsTagValue) ) return false;
        if ( !rhs.GetTag(m_tag, rhsTagValue) ) return true;

        // otherwise compare tag values
        Compare<T> comp;
        return comp(lhsTagValue, rhsTagValue);
    }

    private:
        std::string m_tag;
};

// Algorithms::Unsorted
// placeholder comparison object, ignores the alignments' data
// N.B. - returning false typically retains initial insertion order
struct API_EXPORT Unsorted : public BamAlignmentComparer {
    bool operator()(const BamTools::BamAlignment& /*lhs*/,
                    const BamTools::BamAlignment& /*rhs*/)
    {
        return false;
    }
};

API_EXPORT template<typename Compare>
std::vector<BamAlignment> SortReaderRegion(BamReader& reader,
                                           const BamRegion& region,
                                           const Compare& comp = Compare())
{
    // return empty container if unable to find region
    if ( !reader.IsOpen() )          return std::vector<BamAlignment>();
    if ( !reader.SetRegion(region) ) return std::vector<BamAlignment>();

    // iterate through region, grabbing alignments
    BamAlignment al;
    std::vector<BamAlignment> results;
    while ( reader.GetNextAlignment(al) )
        results.push_back(al);

    // sort & return alignments
    std::sort(results.begin(), results.end(), comp);
    return results;
}

API_EXPORT template<typename Compare>
std::vector<BamAlignment> SortReaderRegion(BamMultiReader& reader,
                                           const BamRegion& region,
                                           const Compare& comp = Compare())
{
    // return empty container if unable to find region
    if ( !reader.HasOpenReaders() )  return std::vector<BamAlignment>();
    if ( !reader.SetRegion(region) ) return std::vector<BamAlignment>();

    // iterate through region, grabbing alignments
    BamAlignment al;
    std::vector<BamAlignment> results;
    while ( reader.GetNextAlignment(al) )
        results.push_back(al);

    // sort & return alignments
    std::sort(results.begin(), results.end(), comp);
    return results;
}

} // namespace Algorithms
} // namespace BamTools

#endif // BAM_ALGORITHMS_H
