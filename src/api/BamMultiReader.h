// ***************************************************************************
// BamMultiReader.h (c) 2010 Erik Garrison, Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 17 January 2011 (DB)
// ---------------------------------------------------------------------------
// Functionality for simultaneously reading multiple BAM files
// ***************************************************************************

#ifndef BAMMULTIREADER_H
#define BAMMULTIREADER_H

#include <api/api_global.h>
#include <api/BamReader.h>
#include <map>
#include <sstream>
#include <string>
#include <utility>

namespace BamTools {

namespace Internal {
    class BamMultiReaderPrivate;
} // namespace Internal

class API_EXPORT BamMultiReader {

    // constructor / destructor
    public:
        BamMultiReader(void);
        ~BamMultiReader(void);

    // public interface
    public:

        // ----------------------
        // BAM file operations
        // ----------------------

        // close BAM files
        void Close(void);
        // opens BAM files (and optional BAM index files, if provided)
        // @openIndexes - triggers index opening, useful for suppressing
        // error messages during merging of files in which we may not have
        // indexes.
        // @coreMode - setup our first alignments using GetNextAlignmentCore();
        // also useful for merging
        // @preferStandardIndex - look for standard BAM index ".bai" first.  If false,
        // will look for BamTools index ".bti".
        bool Open(const std::vector<std::string>& filenames,
                  bool openIndexes = true,
                  bool coreMode = false,
                  bool preferStandardIndex = false);
        // returns whether underlying BAM readers ALL have an index loaded
        // this is useful to indicate whether Jump() or SetRegion() are possible
        bool IsIndexLoaded(void) const;
        // performs random-access jump to reference, position
        bool Jump(int refID, int position = 0);
        // list files associated with this multireader
        void PrintFilenames(void) const;
        // sets the target region
        bool SetRegion(const BamRegion& region);
        bool SetRegion(const int& leftRefID,
                       const int& leftBound,
                       const int& rightRefID,
                       const int& rightBound);
        // returns file pointers to beginning of alignments
        bool Rewind(void);

        // ----------------------
        // access alignment data
        // ----------------------

        // retrieves next available alignment (returns success/fail) from all files
        bool GetNextAlignment(BamAlignment& alignment);
        // retrieves next available alignment (returns success/fail) from all files
        // and populates the support data with information about the alignment
        // *** BUT DOES NOT PARSE CHARACTER DATA FROM THE ALIGNMENT
        bool GetNextAlignmentCore(BamAlignment& alignment);
        // ... should this be private?
        bool HasOpenReaders(void);
        // set sort order for merging BAM files (i.e. which alignment from the files is 'next'?)
        // default behavior is to sort by position, use this method to handle BAMs sorted by read name
        enum SortOrder { SortedByPosition = 0
                       , SortedByReadName
                       , Unsorted
                       };
        void SetSortOrder(const SortOrder& order);

        // ----------------------
        // access auxiliary data
        // ----------------------

        // returns unified SAM header text for all files
        const std::string GetHeaderText(void) const;
        // returns number of reference sequences
        const int GetReferenceCount(void) const;
        // returns vector of reference objects
        const BamTools::RefVector GetReferenceData(void) const;
        // returns reference id (used for BamMultiReader::Jump()) for the given reference name
        const int GetReferenceID(const std::string& refName) const;

        // ----------------------
        // BAM index operations
        // ----------------------

        // creates index for BAM files which lack them, saves to files (default = bamFilename + ".bai")
        bool CreateIndexes(bool useStandardIndex = true);
        // sets the index caching mode for the readers
        void SetIndexCacheMode(const BamIndex::BamIndexCacheMode mode);

    // private implementation
    private:
        Internal::BamMultiReaderPrivate* d;
};

} // namespace BamTools

#endif // BAMMULTIREADER_H
