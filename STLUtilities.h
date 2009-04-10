// STLUtilities.h

// Derek Barnett
// Marth Lab, Boston College
// Last modified: 19 March 2009

#ifndef STL_UTILITIES_H
#define STL_UTILITIES_H

#include <algorithm>
using std::distance;
using std::find;
using std::transform;

#include <functional>
using std::unary_function;

#include <iterator>
using std::iterator_traits;

#include <string>
using std::string;

#include <utility>
using std::pair;

#include <vector>
using std::vector;

// -------------------------------------------------------------------------------- //
// This file contains a sample of STL tricks I've gathered along the way.
//
// Many thanks throughout to 'Effective' series by Scott Meyers.
// Some code is lifted (almost) verbatim from his texts where applicable.
// ------------------------------------------------------------------------------- //

// --------------------------------------------------------------------------------------------------------------------------------- //
// STL containers will delete the values they hold when the container is deleted or goes out of scope
// *** If the container contains pointers, however, they WILL NOT delete the actual pointed-to objects ***
// This struct allows for easy algorithm processing (instead of pesky loops and iterator-boundary issues) to delete pointed-to objects (for any C++ type) that have been allocated with 'new')
// Usage example:    for_each( container.begin(), container.end(), DeleteObject() ) 
//
// Unless of course, you like quirky, hard-to-spot memory leaks... then feel free to disregard this little STL lesson   =)
// --------------------------------------------------------------------------------------------------------------------------------- //
struct DeleteObject {
	template<typename T>
	void operator() (const T* p) const { 
		delete p; 
	}
};

template<typename T>
void ClearPointerVector(vector<T>& v) {
	if ( !v.empty() ) {
		for_each(v.begin(), v.end(), DeleteObject());
		v.clear();
	}
}

// --------------------------------------------------------------------------------------------------------------------------------- //
// Query a vector (other containers? havent tried) for an element
// Returns the index of that element
// Returns vector::size() if not found
// Works with reverse iterators as well... index is counted backward from last element though
// --------------------------------------------------------------------------------------------------------------------------------- //
template < typename InputIterator, typename EqualityComparable >
typename iterator_traits<InputIterator>::difference_type
Index(const InputIterator& begin, const InputIterator& end, const EqualityComparable& item) {
	return distance(begin, find(begin, end, item));
}

// ----------------------------------------------------------------------------------------------------------------------//
// This next section is a sort of work-around for the bulky associative maps
// The idea is to use a *SORTED* vector of pair objects as a lookup vector
// LookupVector = vector< pair<Key, Value> > 
// ----------------------------------------------------------------------------------------------------------------------//

// The following template classes allow a templatized comparison function for sorting & searching lookup vector

// allows sorting by Key ( less<Key> )
template <typename Key, typename Value>
class LookupKeyCompare {

	typedef pair< Key, Value > LookupData;
	typedef typename LookupData::first_type Key_t;
	
	public:
		bool operator() (const LookupData& lhs, const LookupData& rhs) const { return keyLess(lhs.first, rhs.first); }
		bool operator() (const LookupData& lhs, const Key_t& k) const { return keyLess(lhs.first, k); }
		bool operator() (const Key_t& k, const LookupData& rhs) const { return keyLess(k, rhs.first); }
	private:
		bool keyLess(const Key_t& k1, const Key_t& k2) const { return k1 < k2; }
};

// allows sorting by Value ( less<Value> )
template <typename Key, typename Value>
class LookupValueCompare {

	typedef pair< Key, Value > LookupData;
	typedef typename LookupData::second_type Value_t;
	
	public:
		bool operator() (const LookupData& lhs, const LookupData& rhs) const { return valueLess(lhs.second, rhs.second); }
		bool operator() (const LookupData& lhs, const Value_t& k) const { return valueLess(lhs.second, k); }
		bool operator() (const Value_t& k, const LookupData& rhs) const { return valueLess(k, rhs.second); }
	private:
		bool valueLess(const Value_t& k1, const Value_t& k2) const { return k1 < k2; }
};

// The following template functions/structs allow you to pull all keys or all values from this lookup structure

// pull Key from a data pair
template<typename Key, typename Value>
struct RipKey:
	public unary_function< pair<Key,Value>, Key> {
		Key operator() (pair<Key,Value> dataPair) { 
			return dataPair.first; 
		}
};

// pull Value from a data pair
template<typename Key, typename Value>
struct RipValue:
	public unary_function< pair<Key,Value>, Value> {
		Value operator() (pair<Key,Value> dataPair) { 
			return dataPair.second; 
		}
};

// pull all Keys from lookup, store in dest (overwrite contents of dest)
template <typename Key, typename Value>
void RipKeys( vector< pair<Key,Value> >& lookup, vector<Key>& dest) {
	dest.clear();
	dest.reserve(lookup.size());
	transform(lookup.begin(), lookup.end(), back_inserter(dest), RipKey<Key,Value>());
}

// pull all Values from lookup, store in dest (overwrite contents of dest)
template <typename Key, typename Value>
void RipValues( vector< pair<Key,Value> >& lookup, vector<Value>& dest) {
	dest.clear();
	dest.reserve(lookup.size());
	transform(lookup.begin(), lookup.end(), back_inserter(dest), RipValue<Key,Value>());
}

#endif /* STL_UTILITIES_H */
