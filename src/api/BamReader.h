// ***************************************************************************
// BamReader.h (c) 2009 Derek Barnett, Michael Str�mberg
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 18 November 2012 (DB)
// ---------------------------------------------------------------------------
// Provides read access to BAM files.
// ***************************************************************************

#ifndef BAMREADER_H
#define BAMREADER_H

#include <string>
#include "api/BamAlignment.h"
#include "api/BamIndex.h"
#include "api/SamHeader.h"
#include "api/api_global.h"

namespace BamTools {

namespace Internal {
class BamReaderPrivate;
}  // namespace Internal

class API_EXPORT BamReader
{

    // constructor / destructor
public:
    BamReader();
    ~BamReader();

    // public interface
public:
    // ----------------------
    // BAM file operations
    // ----------------------

    // closes the current BAM file
    bool Close();
    // returns filename of current BAM file
    const std::string GetFilename() const;
    // returns true if a BAM file is open for reading
    bool IsOpen() const;
    // performs random-access jump within BAM file
    bool Jump(int refID, int position = 0);
    // opens a BAM file
    bool Open(const std::string& filename);
    // returns internal file pointer to beginning of alignment data
    bool Rewind();
    // sets the target region of interest
    bool SetRegion(const BamRegion& region);
    // sets the target region of interest
    bool SetRegion(const int& leftRefID, const int& leftPosition, const int& rightRefID,
                   const int& rightPosition);

    // ----------------------
    // access alignment data
    // ----------------------

    // retrieves next available alignment
    bool GetNextAlignment(BamAlignment& alignment);
    // retrieves next available alignmnet (without populating the alignment's string data fields)
    bool GetNextAlignmentCore(BamAlignment& alignment);

    // ----------------------
    // access header data
    // ----------------------

    // returns a read-only reference to SAM header data
    const SamHeader& GetConstSamHeader() const;
    // returns an editable copy of SAM header data
    SamHeader GetHeader() const;
    // returns SAM header data, as SAM-formatted text
    std::string GetHeaderText() const;

    // ----------------------
    // access reference data
    // ----------------------

    // returns the number of reference sequences
    int GetReferenceCount() const;
    // returns all reference sequence entries
    const RefVector& GetReferenceData() const;
    // returns the ID of the reference with this name
    int GetReferenceID(const std::string& refName) const;

    // ----------------------
    // BAM index operations
    // ----------------------

    // creates an index file for current BAM file, using the requested index type
    bool CreateIndex(const BamIndex::IndexType& type = BamIndex::STANDARD);
    // returns true if index data is available
    bool HasIndex() const;
    // looks in BAM file's directory for a matching index file
    bool LocateIndex(const BamIndex::IndexType& preferredType = BamIndex::STANDARD);
    // opens a BAM index file
    bool OpenIndex(const std::string& indexFilename);
    // sets a custom BamIndex on this reader
    void SetIndex(BamIndex* index);

    // ----------------------
    // error handling
    // ----------------------

    // returns a human-readable description of the last error that occurred
    std::string GetErrorString() const;

    // private implementation
private:
    Internal::BamReaderPrivate* d;
};

}  // namespace BamTools

#endif  // BAMREADER_H
