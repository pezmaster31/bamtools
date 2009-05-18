// ***************************************************************************
// BamReader (c) 2009 Derek Barnett
// Marth Lab, Deptartment of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Provides the basic functionality for reading BAM files
// ***************************************************************************

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include <algorithm>
#include <string>
#include <utility>
#include <vector>
using namespace std;

#include "BamAlignment.h"

// our zlib constants
#define GZIP_ID1             31
#define GZIP_ID2            139
#define CM_DEFLATE            8
#define FLG_FEXTRA            4
#define OS_UNKNOWN          255
#define BGZF_XLEN             6
#define BGZF_ID1             66
#define BGZF_ID2             67
#define BGZF_LEN              2
#define GZIP_WINDOW_BITS    -15
#define Z_DEFAULT_MEM_LEVEL   8

// our BZGF constants
#define BLOCK_HEADER_LENGTH    18
#define BLOCK_FOOTER_LENGTH     8
#define MAX_BLOCK_SIZE      65536
#define DEFAULT_BLOCK_SIZE  65536

// our BAM constants
#define BAM_CORE_SIZE  32
#define BAM_CMATCH      0
#define BAM_CINS        1
#define BAM_CDEL        2
#define BAM_CREF_SKIP   3
#define BAM_CSOFT_CLIP  4
#define BAM_CHARD_CLIP  5
#define BAM_CPAD        6
#define BAM_CIGAR_SHIFT 4
#define BAM_CIGAR_MASK  ((1 << BAM_CIGAR_SHIFT) - 1)

// BAM indexing constants
#define MAX_BIN           37450 // =(8^6-1)/7+1
#define BAM_MIN_CHUNK_GAP 32768 
#define BAM_LIDX_SHIFT    14

// our variable sizes
#define SIZEOF_INT 4

// define our BZGF structure
#ifndef BGZF_DATA
#define BGZF_DATA
struct BgzfData {
	unsigned int UncompressedBlockSize;
	unsigned int CompressedBlockSize;
	unsigned int BlockLength;
	unsigned int BlockOffset;
	uint64_t BlockAddress;
	bool IsOpen;
	FILE* Stream;
	char* UncompressedBlock;
	char* CompressedBlock;

	// constructor
	BgzfData(void)
		: UncompressedBlockSize(DEFAULT_BLOCK_SIZE)
		, CompressedBlockSize(MAX_BLOCK_SIZE)
		, BlockLength(0)
		, BlockOffset(0)
		, BlockAddress(0)
		, IsOpen(false)
		, Stream(NULL)
		, UncompressedBlock(NULL)
		, CompressedBlock(NULL)
	{
		try {
			CompressedBlock   = new char[CompressedBlockSize];
			UncompressedBlock = new char[UncompressedBlockSize];
		} catch(bad_alloc&) {
			printf("ERROR: Unable to allocate memory for our BGZF object.\n");
			exit(1);
		}
	}

	// destructor
	~BgzfData(void) {
		if(CompressedBlock)   delete [] CompressedBlock;
		if(UncompressedBlock) delete [] UncompressedBlock;
	}
};
#endif // BGZF_DATA


// --------------------------- //
// BamIndex-related typedefs
// --------------------------- //

// offset for linear indexing
typedef vector<uint64_t> LinearOffsetVector;

// alignment 'chunk' boundaries
typedef pair<uint64_t, uint64_t> ChunkPair;
typedef vector<ChunkPair> ChunkVector;

// BAM bins.. each contains (binID, alignment 'chunks')
typedef pair<uint32_t, ChunkVector*> BamBin;
typedef vector<BamBin> BinVector;

// each reference sequence has a BinVector and LinearOffsetVector
typedef pair<BinVector*, LinearOffsetVector*> RefIndex;

// full BamIndex defined as: 
typedef vector<RefIndex*> BamIndex;

class BamReader {

	// constructor/destructor
	public:
		BamReader(void);
		~BamReader(void);

	// public interface
	public:
		// closes the BAM file
		void Close(void);
		// retrieves header text
		const string GetHeaderText(void) const;
		// saves the alignment to the alignment archive
		bool GetNextAlignment(BamAlignment& bAlignment);
		// return number of reference sequences in BAM file
		const int GetReferenceCount(void) const;
		// return vector of RefData entries
		const RefVector GetReferenceData(void) const;
		// get refID from reference name
		const int GetReferenceID(const string& refName) const;
		// jumps to 'left' position on refID
		bool Jump(int refID, unsigned int left = 0);
		// opens the BAM file
		void Open(const string& filename, const string& indexFilename = "");
		// move file pointer back to first alignment
		bool Rewind(void);

	// internal methods
	private:
		// checks BGZF block header
		bool BgzfCheckBlockHeader(char* header);
		// closes the BAM file
		void BgzfClose(void);
		// de-compresses the current block
		int BgzfInflateBlock(int blockLength);
		// opens the BAM file for reading
		void BgzfOpen(const string& filename);
		// reads BGZF data into a byte buffer
		unsigned int BgzfRead(char* data, const unsigned int dataLen);
		// reads BGZF block
		int BgzfReadBlock(void);
		// seek to position in BAM file
		bool BgzfSeek(int64_t position);
		// get file position in BAM file
		int64_t BgzfTell(void);
		// unpacks a buffer into an unsigned int
		static inline unsigned int BgzfUnpackUnsignedInt(char* buffer);
		// unpacks a buffer into an unsigned short
		static inline unsigned short BgzfUnpackUnsignedShort(char* buffer);
		// 	calculate bins that overlap region ( left to reference end for now )
		int BinsFromRegion(int, unsigned int, uint16_t[MAX_BIN]);
		// calculates alignment end position based on starting position and provided CIGAR operations
		unsigned int CalculateAlignmentEnd(const unsigned int& position, const vector<CigarOp>& cigarData);
		// clear out (delete pointers in) index data structure
		void ClearIndex(void);
		// calculate file offset for first alignment chunk overlapping 'left'
		int64_t GetOffset(int refID, unsigned int left);
		// checks to see if alignment overlaps current region
		bool IsOverlap(BamAlignment& bAlignment);
		// retrieves header text from BAM file
		void LoadHeaderData(void);
		// builds BamIndex data structure from BAM index file
		void LoadIndexData(FILE* indexStream);
		// retrieves BAM alignment under file pointer
		bool LoadNextAlignment(BamAlignment& bAlignment);
		// builds reference data structure from BAM file
		void LoadReferenceData(void);
		// open BAM index file (if successful, loads index)
		void OpenIndex(const string& indexFilename);
	
	// aligment file & index file data
	private:
		BgzfData  m_BGZF;
		string    m_headerText;
		BamIndex* m_index;
		RefVector m_references;
		bool      m_isIndexLoaded;
		int64_t   m_alignmentsBeginOffset;

	// user-specified region values
	private:
		bool         m_isRegionSpecified;
		int          m_currentRefID;
		unsigned int m_currentLeft;

	// BAM character constants
	private:
		static const char* DNA_LOOKUP;
		static const char* CIGAR_LOOKUP;
};

// unpacks a buffer into an unsigned int
inline unsigned int BamReader::BgzfUnpackUnsignedInt(char* buffer) {
	union { unsigned int value; unsigned char valueBuffer[sizeof(unsigned int)]; } un;
	un.valueBuffer[0] = buffer[0];
	un.valueBuffer[1] = buffer[1];
	un.valueBuffer[2] = buffer[2];
	un.valueBuffer[3] = buffer[3];
	return un.value;
}

// unpacks a buffer into an unsigned short
inline unsigned short BamReader::BgzfUnpackUnsignedShort(char* buffer) {
	union { unsigned short value; unsigned char valueBuffer[sizeof(unsigned short)];} un;
	un.valueBuffer[0] = buffer[0];
	un.valueBuffer[1] = buffer[1];
	return un.value;
}

// allows sorting/searching of a vector of pairs (instead of using maps)
template <typename Key, typename Value>
class LookupKeyCompare {

	typedef pair< Key, Value > LookupData;
	typedef typename LookupData::first_type Key_t;
	
	public:
		bool operator() (const LookupData& lhs, const LookupData& rhs) const { return keyLess(lhs.first, rhs.first); }
		bool operator() (const LookupData& lhs, const Key_t& k) const        { return keyLess(lhs.first, k); }
		bool operator() (const Key_t& k, const LookupData& rhs) const        { return keyLess(k, rhs.first); }
	private:
		bool keyLess(const Key_t& k1, const Key_t& k2) const { return k1 < k2; }
};

// return index of item if found, else return container.size()
template < typename InputIterator, typename EqualityComparable >
typename iterator_traits<InputIterator>::difference_type
Index(const InputIterator& begin, const InputIterator& end, const EqualityComparable& item) {
	return distance(begin, find(begin, end, item));
}

