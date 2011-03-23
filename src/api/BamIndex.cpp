// ***************************************************************************
// BamIndex.cpp (c) 2009 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 23 March 2011 (DB)
// ---------------------------------------------------------------------------
// Provides index functionality - both for the default (standardized) BAM 
// index format (.bai) as well as a BamTools-specific (nonstandard) index 
// format (.bti).
// ***************************************************************************

#include <api/BamIndex.h>
#include <api/BamReader.h>
#include <api/internal/BamStandardIndex_p.h>
#include <api/internal/BamToolsIndex_p.h>
using namespace BamTools;
using namespace BamTools::Internal;

#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <map>
using namespace std;

/*! \class BamTools::BamIndex
    \brief Provides methods for generating & loading BAM index files.

    This class straddles the line between public API and internal
    implementation detail. Most client code should never have to use this
    class directly.

    It is exposed to the public API to allow advanced users to implement
    their own custom indexing schemes.

    N.B. - Please note that if you wish to derive your own subclass, you are
           entering waters that are not well-documented at the moment and are
           likely to be changing soon anyway. Implementing a custom index is
           technically do-able at the moment, but the learning curve is (at the
           moment) overly steep. Changes will be coming soon to alleviate some
           of this headache.
*/

// ctor
BamIndex::BamIndex(void)
    : m_indexStream(0)
    , m_indexFilename("")
    , m_cacheMode(BamIndex::LimitedIndexCaching)
{ }

// dtor
BamIndex::~BamIndex(void) {
    if ( IsOpen() ) fclose(m_indexStream);
    m_indexFilename = "";
}

// return true if FILE* is open
bool BamIndex::IsOpen(void) const {
    return ( m_indexStream != 0 );
}

// loads existing data from file into memory
bool BamIndex::Load(const string& filename)  {

    // open index file, abort on error
    if ( !OpenIndexFile(filename, "rb") ) {
        fprintf(stderr, "BamIndex ERROR: unable to open the BAM index file %s for reading.\n", filename.c_str());
        return false;
    }

    // check magic number
    if ( !LoadHeader() ) {
        fclose(m_indexStream);
        return false;
    }

    // load reference data (but only keep in memory if full caching requested)
    bool saveInitialLoad = ( m_cacheMode == BamIndex::FullIndexCaching );
    if ( !LoadAllReferences(saveInitialLoad) ) {
        fclose(m_indexStream);
        return false;
    }

    // update index cache based on selected mode
    UpdateCache();

    // return success
    return true;
}

// opens index file for reading/writing, return true if opened OK
bool BamIndex::OpenIndexFile(const string& filename, const string& mode) {

    // attempt to open file, return false if error
    m_indexStream = fopen(filename.c_str(), mode.c_str());
    if ( m_indexStream == 0 )  return false;

    // otherwise save filename & return sucess
    m_indexFilename = filename;
    return true;
}

// rewind index file to beginning of index data, return true if rewound OK
bool BamIndex::Rewind(void) {
    return ( fseek64(m_indexStream, DataBeginOffset(), SEEK_SET) == 0 );
}

// change the index caching behavior
void BamIndex::SetCacheMode(const BamIndex::IndexCacheMode& mode) {
    if ( mode != m_cacheMode ) {
        m_cacheMode = mode;
        UpdateCache();
    }
}

// updates in-memory cache of index data, depending on current cache mode
void BamIndex::UpdateCache(void) {

    // skip if file not open
    if ( !IsOpen() ) return;

    // reflect requested cache mode behavior
    switch ( m_cacheMode ) {

        case (BamIndex::FullIndexCaching) :
            Rewind();
            LoadAllReferences(true);
            break;

        case (BamIndex::LimitedIndexCaching) :
            if ( HasFullDataCache() )
                KeepOnlyFirstReferenceOffsets();
            else {
                ClearAllData();
                SkipToFirstReference();
                LoadFirstReference(true);
            }
            break;

        case(BamIndex::NoIndexCaching) :
            ClearAllData();
            break;

        default :
            // unreachable
            ;
    }
}

// writes in-memory index data out to file
bool BamIndex::Write(const string& bamFilename) {

    // open index file for writing
    string indexFilename = bamFilename + Extension();
    if ( !OpenIndexFile(indexFilename, "wb") ) {
        fprintf(stderr, "BamIndex ERROR: could not open file to save index data.\n");
        return false;
    }

    // write index header data
    if ( !WriteHeader() ) {
        fprintf(stderr, "BamIndex ERROR: there was a problem writing index metadata to the new index file.\n");
        fflush(m_indexStream);
        fclose(m_indexStream);
        exit(1);
    }

    // write main index data
    if ( !WriteAllReferences() ) {
        fprintf(stderr, "BamIndex ERROR: there was a problem writing index data to the new index file.\n");
        fflush(m_indexStream);
        fclose(m_indexStream);
        exit(1);
    }

    // flush any remaining output
    fflush(m_indexStream);
    fclose(m_indexStream);

    // re-open index file for later reading
    if ( !OpenIndexFile(indexFilename, "rb") ) {
        fprintf(stderr, "BamIndex ERROR: could not open newly created index file for reading.\n");
        return false;
    }

    // save index filename & return success
    m_indexFilename = indexFilename;
    return true;
}
