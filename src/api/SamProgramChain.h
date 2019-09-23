// ***************************************************************************
// SamProgramChain.h (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 October 2011 (DB)
// ---------------------------------------------------------------------------
// Provides methods for operating on a SamProgram record "chain"
// ***************************************************************************

#ifndef SAM_PROGRAMCHAIN_H
#define SAM_PROGRAMCHAIN_H

#include <string>
#include <vector>
#include "api/SamProgram.h"
#include "api/api_global.h"

namespace BamTools {

// chain is *NOT* sorted in any order
// use First()/Last() to retrieve oldest/newest programs, respectively
typedef std::vector<SamProgram> SamProgramContainer;
typedef SamProgramContainer::iterator SamProgramIterator;
typedef SamProgramContainer::const_iterator SamProgramConstIterator;

class API_EXPORT SamProgramChain
{

    // ctor & dtor
public:
    SamProgramChain();

    // query/modify program data
public:
    // appends a program record to the chain
    void Add(SamProgram& program);
    void Add(std::vector<SamProgram>& programs);

    // clears all read group entries
    void Clear();

    // returns true if chain contains this program record (matches on ID)
    bool Contains(const SamProgram& program) const;
    bool Contains(const std::string& programId) const;

    // returns the first (oldest) program in the chain
    SamProgram& First();
    const SamProgram& First() const;

    // returns true if chain is empty
    bool IsEmpty() const;

    // returns last (most recent) program in the chain
    SamProgram& Last();
    const SamProgram& Last() const;

    // returns number of program records in the chain
    int Size() const;

    // retrieves a modifiable reference to the SamProgram object associated with this ID
    SamProgram& operator[](const std::string& programId);

    // retrieve STL-compatible iterators
public:
    SamProgramIterator Begin();                  // returns iterator to begin()
    SamProgramConstIterator Begin() const;       // returns const_iterator to begin()
    SamProgramConstIterator ConstBegin() const;  // returns const_iterator to begin()
    SamProgramIterator End();                    // returns iterator to end()
    SamProgramConstIterator End() const;         // returns const_iterator to end()
    SamProgramConstIterator ConstEnd() const;    // returns const_iterator to end()

    // internal methods
private:
    int IndexOf(const std::string& programId) const;
    const std::string NextIdFor(const std::string& programId) const;

    // data members
private:
    SamProgramContainer m_data;
};

}  // namespace BamTools

#endif  // SAM_PROGRAMCHAIN_H
