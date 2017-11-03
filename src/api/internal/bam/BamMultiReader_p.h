// ***************************************************************************
// BamMultiReader_p.h (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 14 January 2013 (DB)
// ---------------------------------------------------------------------------
// Functionality for simultaneously reading multiple BAM files
// *************************************************************************

#ifndef BAMMULTIREADER_P_H
#define BAMMULTIREADER_P_H

//  -------------
//  W A R N I N G
//  -------------
//
// This file is not part of the BamTools API.  It exists purely as an
// implementation detail. This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.

#include <string>
#include <vector>
#include "api/BamMultiReader.h"
#include "api/SamHeader.h"
#include "api/internal/bam/BamMultiMerger_p.h"

namespace BamTools {
namespace Internal {

class BamMultiReaderPrivate
{

    // typedefs
public:
    typedef std::pair<BamReader*, BamAlignment*> ReaderAlignment;

    // constructor / destructor
public:
    BamMultiReaderPrivate();
    ~BamMultiReaderPrivate();

    // public interface
public:
    // file operations
    bool Close();
    bool CloseFile(const std::string& filename);
    const std::vector<std::string> Filenames() const;
    bool Jump(int refID, int position = 0);
    bool Open(const std::vector<std::string>& filenames);
    bool OpenFile(const std::string& filename);
    bool Rewind();
    bool SetRegion(const BamRegion& region);

    // access alignment data
    BamMultiReader::MergeOrder GetMergeOrder() const;
    bool GetNextAlignment(BamAlignment& al);
    bool GetNextAlignmentCore(BamAlignment& al);
    bool HasOpenReaders();
    bool SetExplicitMergeOrder(BamMultiReader::MergeOrder order);

    // access auxiliary data
    SamHeader GetHeader() const;
    std::string GetHeaderText() const;
    int GetReferenceCount() const;
    const BamTools::RefVector GetReferenceData() const;
    int GetReferenceID(const std::string& refName) const;

    // BAM index operations
    bool CreateIndexes(const BamIndex::IndexType& type = BamIndex::STANDARD);
    bool HasIndexes() const;
    bool LocateIndexes(const BamIndex::IndexType& preferredType = BamIndex::STANDARD);
    bool OpenIndexes(const std::vector<std::string>& indexFilenames);

    // error handling
    std::string GetErrorString() const;

    // 'internal' methods
public:
    bool CloseFiles(const std::vector<std::string>& filenames);
    IMultiMerger* CreateAlignmentCache();
    bool PopNextCachedAlignment(BamAlignment& al, const bool needCharData);
    bool RewindReaders();
    void SaveNextAlignment(BamReader* reader, BamAlignment* alignment);
    void SetErrorString(const std::string& where, const std::string& what) const;  //
    bool UpdateAlignmentCache();
    bool ValidateReaders() const;

    // data members
public:
    std::vector<MergeItem> m_readers;
    IMultiMerger* m_alignmentCache;

    bool m_hasUserMergeOrder;
    BamMultiReader::MergeOrder m_mergeOrder;

    mutable std::string m_errorString;
};

}  // namespace Internal
}  // namespace BamTools

#endif  // BAMMULTIREADER_P_H
