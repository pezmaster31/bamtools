// ***************************************************************************
// SamProgramChain.cpp (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 October 2011 (DB)
// ---------------------------------------------------------------------------
// Provides methods for operating on a SamProgram record "chain"
// ***************************************************************************

#include "api/SamProgramChain.h"
using namespace BamTools;

#include <algorithm>
#include <cstdlib>
#include <iostream>

/*! \class BamTools::SamProgramChain
    \brief Sorted container "chain" of SamProgram records.

    Provides methods for operating on a collection of SamProgram records.

    \note Underlying container is *NOT* ordered by linkage, but by order of
    appearance in SamHeader and subsequent Add() calls. Using the current
    iterators will not allow you to step through the header's program history.
    Instead use First()/Last() to access oldest/newest records, respectively.
*/

/*! \fn SamProgramChain::SamProgramChain()
    \brief constructor
*/
SamProgramChain::SamProgramChain() {}

/*! \fn void SamProgramChain::Add(SamProgram& program)
    \brief Appends a program to program chain.

    Duplicate entries are silently discarded.

    \note Underlying container is *NOT* ordered by linkage, but by order of
    appearance in SamHeader and subsequent Add() calls. Using the current
    iterators will not allow you to step through the header's program history.
    Instead use First()/Last() to access oldest/newest records, respectively.

    \param[in] program entry to be appended
*/
void SamProgramChain::Add(SamProgram& program)
{

    // ignore duplicated records
    if (Contains(program)) return;

    // if other programs already in chain, try to find the "next" record
    // tries to match another record's PPID with @program's ID
    if (!IsEmpty()) program.NextProgramID = NextIdFor(program.ID);

    // store program record
    m_data.push_back(program);
}

/*! \fn void SamProgramChain::Add(std::vector<SamProgram>& programs)
    \brief Appends a batch of programs to the end of the chain.

    This is an overloaded function.

    \param[in] programs batch of program records to append
    \sa Add()
*/
void SamProgramChain::Add(std::vector<SamProgram>& programs)
{
    std::vector<SamProgram>::iterator pgIter = programs.begin();
    std::vector<SamProgram>::iterator pgEnd = programs.end();
    for (; pgIter != pgEnd; ++pgIter)
        Add(*pgIter);
}

/*! \fn SamProgramIterator SamProgramChain::Begin()
    \return an STL iterator pointing to the first (oldest) program record
    \sa ConstBegin(), End(), First()
*/
SamProgramIterator SamProgramChain::Begin()
{
    return m_data.begin();
}

/*! \fn SamProgramConstIterator SamProgramChain::Begin() const
    \return an STL const_iterator pointing to the first (oldest) program record

    This is an overloaded function.

    \sa ConstBegin(), End(), First()
*/
SamProgramConstIterator SamProgramChain::Begin() const
{
    return m_data.begin();
}

/*! \fn void SamProgramChain::Clear()
    \brief Clears all program records.
*/
void SamProgramChain::Clear()
{
    m_data.clear();
}

/*! \fn SamProgramConstIterator SamProgramChain::ConstBegin() const
    \return an STL const_iterator pointing to the first (oldest) program record
    \sa Begin(), ConstEnd(), First()
*/
SamProgramConstIterator SamProgramChain::ConstBegin() const
{
    return m_data.begin();
}

/*! \fn SamProgramConstIterator SamProgramChain::ConstEnd() const
    \return an STL const_iterator pointing to the imaginary entry after the last (newest) program record
    \sa ConstBegin(), End(), Last()
*/
SamProgramConstIterator SamProgramChain::ConstEnd() const
{
    return m_data.end();
}

/*! \fn bool SamProgramChain::Contains(const SamProgram& program) const
    \brief Returns true if chains has this program record (matching on ID).

    This is an overloaded function.

    \param[in] program SamProgram to search for
    \return \c true if chain contains program (matching on ID)
*/
bool SamProgramChain::Contains(const SamProgram& program) const
{
    return Contains(program.ID);
}

/*! \fn bool SamProgramChain::Contains(const std::string& programId) const
    \brief Returns true if chains has a program record with this ID

    \param[in] programId search for program matching this ID
    \return \c true if chain contains a program record with this ID
*/
bool SamProgramChain::Contains(const std::string& programId) const
{
    return (IndexOf(programId) != (int)m_data.size());
}

/*! \fn SamProgramIterator SamProgramChain::End()
    \return an STL iterator pointing to the imaginary entry after the last (newest) program record
    \sa Begin(), ConstEnd(), Last()
*/
SamProgramIterator SamProgramChain::End()
{
    return m_data.end();
}

/*! \fn SamProgramConstIterator SamProgramChain::End() const
    \return an STL const_iterator pointing to the imaginary entry after the last (newest) program record

    This is an overloaded function.

    \sa Begin(), ConstEnd(), Last()
*/
SamProgramConstIterator SamProgramChain::End() const
{
    return m_data.end();
}

/*! \fn SamProgram& SamProgramChain::First()
    \brief Fetches first (oldest) record in the chain.

    \warning This function will fail if the chain is empty. If this is possible,
    check the result of IsEmpty() before calling this function.

    \return a modifiable reference to the first (oldest) program entry
    \sa Begin(), Last()
*/
SamProgram& SamProgramChain::First()
{

    // find first record in container that has no PreviousProgramID entry
    SamProgramIterator iter = Begin();
    SamProgramIterator end = End();
    for (; iter != end; ++iter) {
        SamProgram& current = (*iter);
        if (!current.HasPreviousProgramID()) return current;
    }

    // otherwise error
    std::cerr << "SamProgramChain::First: could not find any record without a PP tag" << std::endl;
    std::exit(EXIT_FAILURE);
}

/*! \fn const SamProgram& SamProgramChain::First() const
    \brief Fetches first (oldest) record in the chain.

    This is an overloaded function.

    \warning This function will fail if the chain is empty. If this is possible,
    check the result of IsEmpty() before calling this function.

    \return a read-only reference to the first (oldest) program entry
    \sa Begin(), ConstBegin(), Last()
*/
const SamProgram& SamProgramChain::First() const
{

    // find first record in container that has no PreviousProgramID entry
    SamProgramConstIterator iter = ConstBegin();
    SamProgramConstIterator end = ConstEnd();
    for (; iter != end; ++iter) {
        const SamProgram& current = (*iter);
        if (!current.HasPreviousProgramID()) return current;
    }

    // otherwise error
    std::cerr << "SamProgramChain::First: could not find any record without a PP tag" << std::endl;
    std::exit(EXIT_FAILURE);
}

/*! \fn int SamProgramChain::IndexOf(const std::string& programId) const
    \internal
    \return index of program record if found.
    Otherwise, returns vector::size() (invalid index).
*/
int SamProgramChain::IndexOf(const std::string& programId) const
{
    SamProgramConstIterator begin = ConstBegin();
    SamProgramConstIterator iter = begin;
    SamProgramConstIterator end = ConstEnd();
    for (; iter != end; ++iter) {
        const SamProgram& current = (*iter);
        if (current.ID == programId) break;
    }
    return distance(begin, iter);
}

/*! \fn bool SamProgramChain::IsEmpty() const
    \brief Returns \c true if chain contains no records
    \sa Size()
*/
bool SamProgramChain::IsEmpty() const
{
    return m_data.empty();
}

/*! \fn SamProgram& SamProgramChain::Last()
    \brief Fetches last (newest) record in the chain.

    \warning This function will fail if the chain is empty. If this is possible,
    check the result of IsEmpty() before calling this function.

    \return a modifiable reference to the last (newest) program entry
    \sa End(), First()
*/
SamProgram& SamProgramChain::Last()
{
    // find first record in container that has no NextProgramID entry
    SamProgramIterator iter = Begin();
    SamProgramIterator end = End();
    for (; iter != end; ++iter) {
        SamProgram& current = (*iter);
        if (!current.HasNextProgramID()) return current;
    }

    // otherwise error
    std::cerr << "SamProgramChain::Last: could not determine last record" << std::endl;
    std::exit(EXIT_FAILURE);
}

/*! \fn const SamProgram& SamProgramChain::Last() const
    \brief Fetches last (newest) record in the chain.

    This is an overloaded function.

    \warning This function will fail if the chain is empty. If this is possible,
    check the result of IsEmpty() before calling this function.

    \return a read-only reference to the last (newest) program entry
    \sa End(), ConstEnd(), First()
*/
const SamProgram& SamProgramChain::Last() const
{
    // find first record in container that has no NextProgramID entry
    SamProgramConstIterator iter = ConstBegin();
    SamProgramConstIterator end = ConstEnd();
    for (; iter != end; ++iter) {
        const SamProgram& current = (*iter);
        if (!current.HasNextProgramID()) return current;
    }

    // otherwise error
    std::cerr << "SamProgramChain::Last: could not determine last record" << std::endl;
    std::exit(EXIT_FAILURE);
}

/*! \fn const std::string SamProgramChain::NextIdFor(const std::string& programId) const
    \internal

    \return ID of program record, whose PreviousProgramID matches \a programId.
    Otherwise, returns empty string if none found.
*/
const std::string SamProgramChain::NextIdFor(const std::string& programId) const
{

    // find first record in container whose PreviousProgramID matches @programId
    SamProgramConstIterator iter = ConstBegin();
    SamProgramConstIterator end = ConstEnd();
    for (; iter != end; ++iter) {
        const SamProgram& current = (*iter);
        if (!current.HasPreviousProgramID() && current.PreviousProgramID == programId) {
            return current.ID;
        }
    }

    // none found
    return std::string();
}

/*! \fn int SamProgramChain::Size() const
    \brief Returns number of program records in the chain.
    \sa IsEmpty()
*/
int SamProgramChain::Size() const
{
    return m_data.size();
}

/*! \fn SamProgram& SamProgramChain::operator[](const std::string& programId)
    \brief Retrieves the modifiable SamProgram record that matches \a programId.

    \warning If the chain contains no read group matching this ID, this function will
    print an error and terminate. Check the return value of Contains() if this may be
    possible.

    \param[in] programId ID of program record to retrieve
    \return a modifiable reference to the SamProgram associated with the ID
*/
SamProgram& SamProgramChain::operator[](const std::string& programId)
{

    // look up program record matching this ID
    int index = IndexOf(programId);

    // if record not found
    if (index == (int)m_data.size()) {
        std::cerr << "SamProgramChain::operator[] - unknown programId: " << programId << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // otherwise return program record at index
    return m_data.at(index);
}
