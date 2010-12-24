// ***************************************************************************
// BamMultiReader.cpp (c) 2010 Erik Garrison, Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 23 December 2010 (DB)
// ---------------------------------------------------------------------------
// Uses BGZF routines were adapted from the bgzf.c code developed at the Broad
// Institute.
// ---------------------------------------------------------------------------
// Functionality for simultaneously reading multiple BAM files.
//
// This functionality allows applications to work on very large sets of files
// without requiring intermediate merge, sort, and index steps for each file
// subset.  It also improves the performance of our merge system as it
// precludes the need to sort merged files.
// ***************************************************************************

#include <api/BamMultiReader.h>
#include <api/BGZF.h>
#include <api/internal/BamMultiReader_p.h>
using namespace BamTools;

#include <string>
#include <vector>
using namespace std;

// -----------------------------------------------------
// BamMultiReader implementation
// -----------------------------------------------------

BamMultiReader::BamMultiReader(void)
    : d(new Internal::BamMultiReaderPrivate)
{ }

BamMultiReader::~BamMultiReader(void) {
    delete d;
    d = 0;
}

void BamMultiReader::Close(void) {
    d->Close();
}

bool BamMultiReader::CreateIndexes(bool useStandardIndex) {
    return d->CreateIndexes(useStandardIndex);
}

void BamMultiReader::SetIndexCacheMode(const BamIndex::BamIndexCacheMode mode) {
    d->SetIndexCacheMode(mode);
}

const string BamMultiReader::GetHeaderText(void) const {
    return d->GetHeaderText();
}

bool BamMultiReader::GetNextAlignment(BamAlignment& nextAlignment) {
    return d->GetNextAlignment(nextAlignment);
}

bool BamMultiReader::GetNextAlignmentCore(BamAlignment& nextAlignment) {
    return d->GetNextAlignmentCore(nextAlignment);
}

const int BamMultiReader::GetReferenceCount(void) const {
    return d->GetReferenceCount();
}

const BamTools::RefVector BamMultiReader::GetReferenceData(void) const {
    return d->GetReferenceData();
}

const int BamMultiReader::GetReferenceID(const string& refName) const { 
    return d->GetReferenceID(refName);
}

bool BamMultiReader::HasOpenReaders() {
    return d->HasOpenReaders();
}

bool BamMultiReader::IsIndexLoaded(void) const {
    return d->IsIndexLoaded();
}

bool BamMultiReader::Jump(int refID, int position) {
    return d->Jump(refID, position);
}

bool BamMultiReader::Open(const vector<string>& filenames,
                          bool openIndexes,
                          bool coreMode,
                          bool preferStandardIndex)
{
    return d->Open(filenames, openIndexes, coreMode, preferStandardIndex);
}

void BamMultiReader::PrintFilenames(void) const {
    d->PrintFilenames();
}

bool BamMultiReader::Rewind(void) {
    return d->Rewind();
}

bool BamMultiReader::SetRegion(const int& leftRefID,
                               const int& leftPosition,
                               const int& rightRefID,
                               const int& rightPosition)
{
    BamRegion region(leftRefID, leftPosition, rightRefID, rightPosition);
    return d->SetRegion(region);
}

bool BamMultiReader::SetRegion(const BamRegion& region) {
    return d->SetRegion(region);
}

void BamMultiReader::SetSortOrder(const SortOrder& order) {
    d->SetSortOrder(order);
}
