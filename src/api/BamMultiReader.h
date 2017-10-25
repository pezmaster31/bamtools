// ***************************************************************************
// BamMultiReader.h (c) 2010 Erik Garrison, Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 14 January 2013 (DB)
// ---------------------------------------------------------------------------
// Convenience class for reading multiple BAM files.
// ***************************************************************************

#ifndef BAMMULTIREADER_H
#define BAMMULTIREADER_H

#include "api/api_global.h"
#include "api/BamReader.h"
#include <map>
#include <sstream>
#include <string>
#include <utility>

namespace BamTools {

namespace Internal {
    class BamMultiReaderPrivate;
} // namespace Internal

class API_EXPORT BamMultiReader {

    // enums
    public:
        // possible merge order strategies
        enum MergeOrder { RoundRobinMerge = 0
                        , MergeByCoordinate
                        , MergeByName
                        };

    // constructor / destructor
    public:
        BamMultiReader();
        ~BamMultiReader();

    // public interface
    public:

        // ----------------------
        // BAM file operations
        // ----------------------

        // closes all open BAM files
        bool Close();
        // close only the requested BAM file
        bool CloseFile(const std::string& filename);
        // returns list of filenames for all open BAM files
        const std::vector<std::string> Filenames() const;
        // returns curent merge order strategy
        BamMultiReader::MergeOrder GetMergeOrder() const;
        // returns true if multireader has any open BAM files
        bool HasOpenReaders() const;
        // performs random-access jump within current BAM files
        bool Jump(int refID, int position = 0);
        // opens BAM files
        bool Open(const std::vector<std::string>& filenames);
        // opens a single BAM file, adding to any other current BAM files
        bool OpenFile(const std::string& filename);
        // returns file pointers to beginning of alignments
        bool Rewind();
        // sets an explicit merge order, regardless of the BAM files' SO header tag
        bool SetExplicitMergeOrder(BamMultiReader::MergeOrder order);
        // sets the target region of interest
        bool SetRegion(const BamRegion& region);
        // sets the target region of interest
        bool SetRegion(const int& leftRefID,
                       const int& leftPosition,
                       const int& rightRefID,
                       const int& rightPosition);

        // ----------------------
        // access alignment data
        // ----------------------

        // retrieves next available alignment
        bool GetNextAlignment(BamAlignment& alignment);
        // retrieves next available alignment (without populating the alignment's string data fields)
        bool GetNextAlignmentCore(BamAlignment& alignment);

        // ----------------------
        // access auxiliary data
        // ----------------------

        // returns unified SAM header for all files
        SamHeader GetHeader() const;
        // returns unified SAM header text for all files
        std::string GetHeaderText() const;
        // returns number of reference sequences
        int GetReferenceCount() const;
        // returns all reference sequence entries.
        const BamTools::RefVector GetReferenceData() const;
        // returns the ID of the reference with this name.
        int GetReferenceID(const std::string& refName) const;

        // ----------------------
        // BAM index operations
        // ----------------------

        // creates index files for current BAM files
        bool CreateIndexes(const BamIndex::IndexType& type = BamIndex::STANDARD);
        // returns true if all BAM files have index data available
        bool HasIndexes() const;
        // looks for index files that match current BAM files
        bool LocateIndexes(const BamIndex::IndexType& preferredType = BamIndex::STANDARD);
        // opens index files for current BAM files.
        bool OpenIndexes(const std::vector<std::string>& indexFilenames);

        // ----------------------
        // error handling
        // ----------------------

        // returns a human-readable description of the last error that occurred
        std::string GetErrorString() const;

    // private implementation
    private:
        Internal::BamMultiReaderPrivate* d;
};

} // namespace BamTools

#endif // BAMMULTIREADER_H
