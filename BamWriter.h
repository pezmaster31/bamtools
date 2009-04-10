// ***************************************************************************
// BamWriter (c) 2009 Michael Strömberg
// Marth Lab, Deptartment of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Provides the basic functionality for producing BAM files
// ***************************************************************************

#pragma once

#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include "BamAlignment.h"

using namespace std;

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

// our variable sizes
#define SIZEOF_INT 4

// define our BZGF structure
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

class BamWriter {
public:
	// constructor
	BamWriter(void);
	// destructor
	~BamWriter(void);
	// closes the alignment archive
	void Close(void);
	// opens the alignment archive
	void Open(const string& filename, const string& samHeader, const RefVector& referenceSequences);
	// saves the alignment to the alignment archive
	void SaveAlignment(const BamAlignment& al);
private:
	// closes the BAM file
	void BgzfClose(void);
	// compresses the current block
	int BgzfDeflateBlock(void);
	// flushes the data in the BGZF block
	void BgzfFlushBlock(void);
	// opens the BAM file for writing
	void BgzfOpen(const string& filename);
	// packs an unsigned integer into the specified buffer
	static inline void BgzfPackUnsignedInt(char* buffer, unsigned int value);
	// packs an unsigned short into the specified buffer
	static inline void BgzfPackUnsignedShort(char* buffer, unsigned short value);
	// writes the supplied data into the BGZF buffer
	unsigned int BgzfWrite(const char* data, const unsigned int dataLen);
	// calculates the minimum bin that contains a region [begin, end)
	static inline unsigned int CalculateMinimumBin(unsigned int begin, unsigned int end);
	// creates a packed cigar string from the supplied alignment
	static void CreatePackedCigar(const vector<CigarOp>& cigarOperations, string& packedCigar);
	// encodes the supplied query sequence into 4-bit notation
	static void EncodeQuerySequence(const string& query, string& encodedQuery);
	// our BGZF output object
	BgzfData mBGZF;
};

// packs an unsigned integer into the specified buffer
inline void BamWriter::BgzfPackUnsignedInt(char* buffer, unsigned int value) {
	buffer[0] = (char)value;
	buffer[1] = (char)(value >> 8);
	buffer[2] = (char)(value >> 16);
	buffer[3] = (char)(value >> 24);
}

// packs an unsigned short into the specified buffer
inline void BamWriter::BgzfPackUnsignedShort(char* buffer, unsigned short value) {
	buffer[0] = (char)value;
	buffer[1] = (char)(value >> 8);
}
