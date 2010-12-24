// ***************************************************************************
// BamMultiReader_p.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 23 December 2010 (DB)
// ---------------------------------------------------------------------------
// Functionality for simultaneously reading multiple BAM files
// *************************************************************************

#include <api/BamAlignment.h>
#include <api/BamMultiReader.h>
#include <api/internal/BamMultiMerger_p.h>
#include <api/internal/BamMultiReader_p.h>
using namespace BamTools;
using namespace BamTools::Internal;

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
using namespace std;

// ctor
BamMultiReaderPrivate::BamMultiReaderPrivate(void)
    : m_alignments(0)
    , m_isCoreMode(false)
    , m_isSortedByPosition(true)
{ }

// dtor
BamMultiReaderPrivate::~BamMultiReaderPrivate(void) {

    // close all open BAM readers
    Close();

    // clean up alignment cache
    delete m_alignments;
    m_alignments = 0;
}

// close the BAM files
void BamMultiReaderPrivate::Close(void) {

    // clear out alignment cache
    m_alignments->Clear();

    // iterate over readers
    vector<ReaderAlignment>::iterator readerIter = m_readers.begin();
    vector<ReaderAlignment>::iterator readerEnd  = m_readers.end();
    for ( ; readerIter != readerEnd; ++readerIter ) {

        // close reader
        BamReader*    reader    = (*readerIter).first;
        BamAlignment* alignment = (*readerIter).second;
        if ( reader ) reader->Close();

        // delete pointers
        delete reader;
        reader = 0;
        delete alignment;
        alignment = 0;
    }

    // clear out readers
    m_readers.clear();

    // reset flags
    m_isCoreMode = false;
    m_isSortedByPosition = true;
}

// saves index data to BAM index files (".bai"/".bti") where necessary, returns success/fail
bool BamMultiReaderPrivate::CreateIndexes(bool useStandardIndex) {

    bool result = true;
    vector<ReaderAlignment>::iterator readerIter = m_readers.begin();
    vector<ReaderAlignment>::iterator readerEnd  = m_readers.end();
    for ( ; readerIter != readerEnd; ++readerIter ) {
        BamReader* reader = (*readerIter).first;
        result &= reader->CreateIndex(useStandardIndex);
    }
    return result;
}

const string BamMultiReaderPrivate::ExtractReadGroup(const string& headerLine) const {

    string readGroup("");
    stringstream headerLineSs(headerLine);
    string part;

    // parse @RG header line, looking for the ID: tag
    while( getline(headerLineSs, part, '\t') ) {
        stringstream partSs(part);
        string subtag;
        getline(partSs, subtag, ':');
        if ( subtag == "ID" ) {
            getline(partSs, readGroup, ':');
            break;
        }
    }
    return readGroup;
}

// makes a virtual, unified header for all the bam files in the multireader
const string BamMultiReaderPrivate::GetHeaderText(void) const {

    // just spit single header out if only have one reader open
    if ( m_readers.size() == 1 ) {


        vector<ReaderAlignment>::const_iterator readerBegin = m_readers.begin();
        const ReaderAlignment& entry = (*readerBegin);
        const BamReader* reader = entry.first;
        if ( reader == 0 ) return "";
        return reader->GetHeaderText();
    }

    string mergedHeader = "";
    map<string, bool> readGroups;

    // foreach extraction entry (each BAM file)
    vector<ReaderAlignment>::const_iterator readerBegin = m_readers.begin();
    vector<ReaderAlignment>::const_iterator readerIter  = readerBegin;
    vector<ReaderAlignment>::const_iterator readerEnd   = m_readers.end();
    for ( ; readerIter != readerEnd; ++readerIter ) {

        // get header from reader
        const BamReader* reader = (*readerIter).first;
        if ( reader == 0 ) continue;
        string headerText = reader->GetHeaderText();
        if ( headerText.empty() ) continue;

        // store header text in lines
        map<string, bool> currentFileReadGroups;
        const vector<string> lines = SplitHeaderText(headerText);

        // iterate over header lines
        vector<string>::const_iterator linesIter = lines.begin();
        vector<string>::const_iterator linesEnd  = lines.end();
        for ( ; linesIter != linesEnd; ++linesIter ) {

            // get next line from header, skip if empty
            const string headerLine = (*linesIter);
            if ( headerLine.empty() ) { continue; }

            // if first file, save HD & SQ entries
            if ( readerIter == readerBegin ) {
                if ( headerLine.find("@HD") == 0 || headerLine.find("@SQ") == 0) {
                    mergedHeader.append(headerLine.c_str());
                    mergedHeader.append(1, '\n');
                }
            }

            // (for all files) append RG entries if they are unique
            if ( headerLine.find("@RG") == 0 ) {

                // extract read group name from line
                const string readGroup = ExtractReadGroup(headerLine);

                // make sure not to duplicate @RG entries
                if ( readGroups.find(readGroup) == readGroups.end() ) {
                    mergedHeader.append(headerLine.c_str() );
                    mergedHeader.append(1, '\n');
                    readGroups[readGroup] = true;
                    currentFileReadGroups[readGroup] = true;
                } else {
                    // warn iff we are reading one file and discover duplicated @RG tags in the header
                    // otherwise, we emit no warning, as we might be merging multiple BAM files with identical @RG tags
                    if ( currentFileReadGroups.find(readGroup) != currentFileReadGroups.end() ) {
                        cerr << "WARNING: duplicate @RG tag " << readGroup
                            << " entry in header of " << reader->GetFilename() << endl;
                    }
                }
            }
        }
    }

    // return merged header text
    return mergedHeader;
}

// get next alignment among all files
bool BamMultiReaderPrivate::GetNextAlignment(BamAlignment& al) {
    return LoadNextAlignment(al, false);
}

// get next alignment among all files without parsing character data from alignments
bool BamMultiReaderPrivate::GetNextAlignmentCore(BamAlignment& al) {
    return LoadNextAlignment(al, true);
}

// ---------------------------------------------------------------------------------------
//
// NB: The following GetReferenceX() functions assume that we have identical
// references for all BAM files.  We enforce this by invoking the above
// validation function (ValidateReaders) to verify that our reference data
// is the same across all files on Open, so we will not encounter a situation
// in which there is a mismatch and we are still live.
//
// ---------------------------------------------------------------------------------------

// returns the number of reference sequences
const int BamMultiReaderPrivate::GetReferenceCount(void) const {
    const ReaderAlignment& firstReader = m_readers.front();
    const BamReader* reader = firstReader.first;
    if ( reader ) return reader->GetReferenceCount();
    else return 0;
}

// returns vector of reference objects
const RefVector BamMultiReaderPrivate::GetReferenceData(void) const {
    const ReaderAlignment& firstReader = m_readers.front();
    const BamReader* reader = firstReader.first;
    if ( reader ) return reader->GetReferenceData();
    else return RefVector();
}

// returns refID from reference name
const int BamMultiReaderPrivate::GetReferenceID(const string& refName) const {
    const ReaderAlignment& firstReader = m_readers.front();
    const BamReader* reader = firstReader.first;
    if ( reader ) return reader->GetReferenceID(refName);
    else return -1; // ERROR case - how to report
}

// ---------------------------------------------------------------------------------------

// checks if any readers still have alignments
bool BamMultiReaderPrivate::HasOpenReaders(void) {
    return ( m_alignments->Size() > 0 );
}

// returns whether underlying BAM readers ALL have an index loaded
// this is useful to indicate whether Jump() or SetRegion() are possible
bool BamMultiReaderPrivate::IsIndexLoaded(void) const {
    bool ok = true;
    vector<ReaderAlignment>::const_iterator readerIter = m_readers.begin();
    vector<ReaderAlignment>::const_iterator readerEnd  = m_readers.end();
    for ( ; readerIter != readerEnd; ++readerIter ) {
        const BamReader* reader = (*readerIter).first;
        if ( reader ) ok &= reader->IsIndexLoaded();
    }
    return ok;
}

// jumps to specified region(refID, leftBound) in BAM files, returns success/fail
bool BamMultiReaderPrivate::Jump(int refID, int position) {

    bool ok = true;
    vector<ReaderAlignment>::iterator readerIter = m_readers.begin();
    vector<ReaderAlignment>::iterator readerEnd  = m_readers.end();
    for ( ; readerIter != readerEnd; ++readerIter ) {
        BamReader* reader = (*readerIter).first;
        if ( reader == 0 ) continue;

        ok &= reader->Jump(refID, position);
        if ( !ok ) {
            cerr << "ERROR: could not jump " << reader->GetFilename()
                 << " to " << refID << ":" << position << endl;
            exit(1);
        }
    }

    if (ok) UpdateAlignments();
    return ok;
}

bool BamMultiReaderPrivate::LoadNextAlignment(BamAlignment& al, bool coreMode) {

    // bail out if no more data to process
    if ( !HasOpenReaders() ) return false;

    // "pop" next alignment and reader
    ReaderAlignment nextReaderAlignment = m_alignments->TakeFirst();
    BamReader*    reader    = nextReaderAlignment.first;
    BamAlignment* alignment = nextReaderAlignment.second;

    // save it by copy to our argument
    al = BamAlignment(*alignment);

    // peek to next alignment & store in cache
    m_isCoreMode = coreMode;
    SaveNextAlignment(reader,alignment);

    // return success
    return true;
}

// opens BAM files
bool BamMultiReaderPrivate::Open(const vector<string>& filenames,
                                 bool openIndexes,
                                 bool coreMode,
                                 bool preferStandardIndex)
{
    // store core mode flag
    m_isCoreMode = coreMode;

    // first clear out any prior alignment cache prior data
    if ( m_alignments ) {
        m_alignments->Clear();
        delete m_alignments;
        m_alignments = 0;
    }

    // create alignment cache based on sorting mode
    if ( m_isSortedByPosition )
        m_alignments = new PositionMultiMerger;
    else
        m_alignments = new ReadNameMultiMerger;

    // iterate over filenames
    vector<string>::const_iterator filenameIter = filenames.begin();
    vector<string>::const_iterator filenameEnd  = filenames.end();
    for ( ; filenameIter != filenameEnd; ++filenameIter ) {
        const string filename = (*filenameIter);

        bool openedOk = true;
        BamReader* reader = new BamReader;
        openedOk = reader->Open(filename, "", openIndexes, preferStandardIndex);

        // if file opened ok
        if ( openedOk ) {

            // try to read first alignment
            bool fileOk = true;
            BamAlignment* alignment = new BamAlignment;
            fileOk &= ( coreMode ? reader->GetNextAlignmentCore(*alignment)
                                 : reader->GetNextAlignment(*alignment) );

            if ( fileOk ) {

                m_readers.push_back( make_pair(reader, alignment) );
                m_alignments->Add( make_pair(reader, alignment) );

            } else {
                cerr << "WARNING: could not read first alignment in "
                     << filename << ", ignoring file" << endl;

                // if only file available & could not be read, return failure
                if ( filenames.size() == 1 )
                    return false;
            }
        }

        // TODO; any further error handling when openedOK is false ??
        else return false;
    }

    // files opened ok, at least one alignment could be read,
    // now need to check that all files use same reference data
    ValidateReaders();
    return true;
}

// print associated filenames to stdout
void BamMultiReaderPrivate::PrintFilenames(void) const {

    vector<ReaderAlignment>::const_iterator readerIter = m_readers.begin();
    vector<ReaderAlignment>::const_iterator readerEnd  = m_readers.end();
    for ( ; readerIter != readerEnd; ++readerIter ) {
        const BamReader* reader = (*readerIter).first;
        if ( reader == 0 ) continue;
        cout << reader->GetFilename() << endl;
    }
}

// returns BAM file pointers to beginning of alignment data
bool BamMultiReaderPrivate::Rewind(void) {

    bool result = true;
    vector<ReaderAlignment>::iterator readerIter = m_readers.begin();
    vector<ReaderAlignment>::iterator readerEnd  = m_readers.end();
    for ( ; readerIter != readerEnd; ++readerIter ) {
        BamReader* reader = (*readerIter).first;
        if ( reader == 0 ) continue;
        result &= reader->Rewind();
    }
    return result;
}

void BamMultiReaderPrivate::SaveNextAlignment(BamReader* reader, BamAlignment* alignment) {

    // must be in core mode && sorting by position to call GNACore()
    if ( m_isCoreMode && m_isSortedByPosition ) {
        if ( reader->GetNextAlignmentCore(*alignment) )
            m_alignments->Add( make_pair(reader, alignment) );
    }

    // not in core mode and/or sorting by readname, must call GNA()
    else {
        if ( reader->GetNextAlignment(*alignment) )
            m_alignments->Add( make_pair(reader, alignment) );
    }
}

// sets the index caching mode on the readers
void BamMultiReaderPrivate::SetIndexCacheMode(const BamIndex::BamIndexCacheMode mode) {

    vector<ReaderAlignment>::iterator readerIter = m_readers.begin();
    vector<ReaderAlignment>::iterator readerEnd  = m_readers.end();
    for ( ; readerIter != readerEnd; ++readerIter ) {
        BamReader* reader = (*readerIter).first;
        if ( reader == 0 ) continue;
        reader->SetIndexCacheMode(mode);
    }
}

bool BamMultiReaderPrivate::SetRegion(const BamRegion& region) {

    // NB: While it may make sense to track readers in which we can
    // successfully SetRegion, In practice a failure of SetRegion means "no
    // alignments here."  It makes sense to simply accept the failure,
    // UpdateAlignments(), and continue.

    vector<ReaderAlignment>::iterator readerIter = m_readers.begin();
    vector<ReaderAlignment>::iterator readerEnd  = m_readers.end();
    for ( ; readerIter != readerEnd; ++readerIter ) {
        BamReader* reader = (*readerIter).first;
        if ( reader == 0 ) continue;
        if ( !reader->SetRegion(region) ) {
            cerr << "ERROR: could not jump " << reader->GetFilename() << " to "
                 << region.LeftRefID  << ":" << region.LeftPosition   << ".."
                 << region.RightRefID << ":" << region.RightPosition  << endl;
        }
    }

    UpdateAlignments();
    return true;
}

void BamMultiReaderPrivate::SetSortOrder(const BamMultiReader::SortOrder& order) {

    const BamMultiReader::SortOrder oldSortOrder = ( m_isSortedByPosition ? BamMultiReader::SortedByPosition
                                                                          : BamMultiReader::SortedByReadName ) ;
    // skip if no change needed
    if ( oldSortOrder == order ) return;

    // create new alignment cache
    IBamMultiMerger* newAlignmentCache(0);
    if ( order == BamMultiReader::SortedByPosition ) {
        newAlignmentCache = new PositionMultiMerger;
        m_isSortedByPosition = true;
    }
    else {
        newAlignmentCache = new ReadNameMultiMerger;
        m_isSortedByPosition = false;
    }

    // copy old cache contents to new cache
    while ( m_alignments->Size() > 0 ) {
        ReaderAlignment value = m_alignments->TakeFirst();
        newAlignmentCache->Add(value);
    }

    // remove old cache structure & point to new cache
    delete m_alignments;
    m_alignments = newAlignmentCache;
}

// updates our alignment cache
void BamMultiReaderPrivate::UpdateAlignments(void) {

    // clear the cache
    m_alignments->Clear();

    // iterate over readers
    vector<ReaderAlignment>::iterator readerIter = m_readers.begin();
    vector<ReaderAlignment>::iterator readerEnd  = m_readers.end();
    for ( ; readerIter != readerEnd; ++readerIter ) {
        BamReader* reader = (*readerIter).first;
        BamAlignment* alignment = (*readerIter).second;
        if ( reader == 0 ) continue;
        SaveNextAlignment(reader, alignment);
    }
}

// splits the entire header into a list of strings
const vector<string> BamMultiReaderPrivate::SplitHeaderText(const string& headerText) const {
    stringstream header(headerText);
    vector<string> lines;
    string item;
    while ( getline(header, item) )
        lines.push_back(item);
    return lines;
}

// ValidateReaders checks that all the readers point to BAM files representing
// alignments against the same set of reference sequences, and that the
// sequences are identically ordered.  If these checks fail the operation of
// the multireader is undefined, so we force program exit.
void BamMultiReaderPrivate::ValidateReaders(void) const {

    // retrieve first reader data
    const BamReader* firstReader = m_readers.front().first;
    if ( firstReader == 0 ) return; // signal error?
    const RefVector firstReaderRefData = firstReader->GetReferenceData();
    const int firstReaderRefCount = firstReader->GetReferenceCount();
    const int firstReaderRefSize = firstReaderRefData.size();

    // iterate over all readers
    vector<ReaderAlignment>::const_iterator readerIter = m_readers.begin();
    vector<ReaderAlignment>::const_iterator readerEnd  = m_readers.end();
    for ( ; readerIter != readerEnd; ++readerIter ) {

        // get current reader data
        BamReader* reader = (*readerIter).first;
        if ( reader == 0 ) continue; // error?
        const RefVector currentReaderRefData = reader->GetReferenceData();
        const int currentReaderRefCount = reader->GetReferenceCount();
        const int currentReaderRefSize  = currentReaderRefData.size();

        // init container iterators
        RefVector::const_iterator firstRefIter   = firstReaderRefData.begin();
        RefVector::const_iterator firstRefEnd    = firstReaderRefData.end();
        RefVector::const_iterator currentRefIter = currentReaderRefData.begin();

        // compare reference counts from BamReader ( & container size, in case of BR error)
        if ( (currentReaderRefCount != firstReaderRefCount) ||
             (firstReaderRefSize    != currentReaderRefSize) )
        {
            cerr << "ERROR: mismatched number of references in " << reader->GetFilename()
                 << " expected " << firstReaderRefCount
                 << " reference sequences but only found " << currentReaderRefCount << endl;
            exit(1);
        }

        // this will be ok; we just checked above that we have identically-sized sets of references
        // here we simply check if they are all, in fact, equal in content
        while ( firstRefIter != firstRefEnd ) {

            const RefData& firstRef   = (*firstRefIter);
            const RefData& currentRef = (*currentRefIter);

            // compare reference name & length
            if ( (firstRef.RefName   != currentRef.RefName) ||
                 (firstRef.RefLength != currentRef.RefLength) )
            {
                cerr << "ERROR: mismatched references found in " << reader->GetFilename()
                     << " expected: " << endl;
                RefVector::const_iterator refIter = firstReaderRefData.begin();
                RefVector::const_iterator refEnd  = firstReaderRefData.end();
                for ( ; refIter != refEnd; ++refIter ) {
                    const RefData& entry = (*refIter);
                    cerr << entry.RefName << " " << entry.RefLength << endl;
                }

                cerr << "but found: " << endl;
                refIter = currentReaderRefData.begin();
                refEnd  = currentReaderRefData.end();
                for ( ; refIter != refEnd; ++refIter ) {
                    const RefData& entry = (*refIter);
                    cerr << entry.RefName << " " << entry.RefLength << endl;
                }

                exit(1);
            }

            // update iterators
            ++firstRefIter;
            ++currentRefIter;
        }
    }
}
