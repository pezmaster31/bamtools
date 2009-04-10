// BamReader.h

/* The MIT License

   Copyright (c) 2008 Genome Research Ltd (GRL).

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

/*
	Implementation of BAM-parsing was translated to C++ directly from Heng Li's SAMtools package 
        (thus the carryover of above MIT license)
	Contact: Derek Barnett <barnetde@bc.edu>
*/

// Derek Barnett
// Marth Lab, Boston College
// Last modified: 6 April 2009

#ifndef BAMREADER_H
#define BAMREADER_H

// custom includes
#include "BamAlignment.h"
#include "STLUtilities.h"

// C library includes
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
typedef char                 int8_t;
typedef unsigned char       uint8_t;
typedef short               int16_t;
typedef unsigned short     uint16_t;
typedef int                 int32_t;
typedef unsigned int       uint32_t;
typedef long long           int64_t;
typedef unsigned long long uint64_t;
#else
#include <stdint.h>
#endif

// BGZF library includes/defines
#include "bgzf.h"
typedef BGZF* BamFile;
#define bam_open(f_name, mode)      bgzf_open(f_name, mode)
#define bam_close(f_ptr)            bgzf_close(f_ptr)
#define bam_read(f_ptr, buf, size)  bgzf_read(f_ptr, buf, size)
#define bam_write(f_ptr, buf, size) bgzf_write(f_ptr, buf, size)
#define bam_tell(f_ptr)             bgzf_tell(f_ptr)
#define bam_seek(f_ptr, pos, dir)   bgzf_seek(f_ptr, pos, dir)

// size of alignment data block in BAM file (bytes)
#define BAM_CORE_SIZE 32

// BAM indexing constants
#define MAX_BIN           37450 // =(8^6-1)/7+1
#define BAM_MIN_CHUNK_GAP 32768 
#define BAM_LIDX_SHIFT    14

// CIGAR-retrieval mask/shift constants
#define BAM_CIGAR_SHIFT 4
#define BAM_CIGAR_MASK  ((1 << BAM_CIGAR_SHIFT) - 1)

// CIGAR-operation types
#define BAM_CMATCH      0
#define BAM_CINS        1
#define BAM_CDEL        2
#define BAM_CREF_SKIP   3
#define BAM_CSOFT_CLIP  4
#define BAM_CHARD_CLIP  5
#define BAM_CPAD        6

// --------------------------- //
// Bam header info
// --------------------------- //

// --------------------------- //
// BamIndex-related typedefs
// --------------------------- //

// offset for linear indexing
typedef vector<uint64_t> LinearOffsetVector;

// chunk boundaries
typedef pair<uint64_t, uint64_t> ChunkPair;
// list of chunks in a BAM bin
typedef vector<ChunkPair> ChunkVector;

// BAM bins for a reference sequence
// replaces khash - uint32_t is key, ChunkVector is value
typedef pair<uint32_t, ChunkVector*> BamBin;
typedef vector<BamBin> BinVector;

// each reference sequence has a BinVector and LinearOffsetVector
typedef pair<BinVector*, LinearOffsetVector*> RefIndex;

// full BamIndex defined as: 
typedef vector<RefIndex*> BamIndex;

// ---------------------------------------------------------------------------//

class BamReader {
	
	public:
		// constructors
		BamReader(const char* fileName = NULL, const char* indexFilename = NULL);

	public:
		// destructor
		~BamReader(void);
	
	// BAM interface methods
	public:

		// ----------------------- //
		// File manipulation
		// ----------------------- //
		
		// open BAM file (automatically opens index if provided)
		bool Open(void);
		
		// open BAM index (allows index to be opened separately - i.e. sometime after the BAM file is opened)
		bool OpenIndex(void);
		
		// close BAM file
		bool Close(void);
		
		// get BAM filename
		const char* Filename(void) const;
	
		// set BAM filename
		void SetFilename(const char*);

		// get BAM Index filename
		const char* IndexFilename(void) const;
		
		// set BAM Index filename
		void SetIndexFilename(const char*);

		// ----------------------- //
		// Access BAM header
		// ----------------------- //
		
		// return full header text
		const string GetHeaderText(void) const;
		
		// --------------------------------- //
		// Access reference sequence info
		// --------------------------------- //
		
		// return number of reference sequences in BAM file
		const int GetReferenceCount(void) const;

		// return vector of RefData entries
		const RefVector GetReferenceData(void) const;

		// get refID from reference name
		const int GetRefID(string refName) const;		
		
		// ----------------------------------------- //
		// File position moving
		// ----------------------------------------- //

		// jumps to 'left' position on refID
		// actually jumps before position, so reads that overlap 'left' are included as well
		// 'left' defaults to reference begin if not specified
		bool Jump(int refID, unsigned int left = 0);

		// Jump to beginning of BAM file, clears any region previously set by Jump()
		bool Rewind(void);
		
		// ------------------------------ //
		// Access alignments
		// ------------------------------ //
		
		// get next alignment
		bool GetNextAlignment(BamAlignment& read);

		// allow user to specifiy whether 'AlignedBases' string is calculated when alignment is loaded
		void SetCalculateAlignedBases(bool);

	// internal utility methods
	private:
		int      BinsFromRegion(int, unsigned int, uint16_t[MAX_BIN]);
		uint32_t CalculateAlignmentEnd(const unsigned int&, const vector<CigarOp>&);
		uint64_t GetOffset(int, unsigned int);
		bool     IsOverlap(BamAlignment&);
		bool     LoadHeader(void);
		bool     LoadIndex(void);
		bool     LoadNextAlignment(BamAlignment&);

	private:	
		// main BAM reader components
		char*      m_filename;
		char*      m_indexFilename;
		BamFile    m_file;
		BamIndex*  m_index;
		RefVector  m_references;
		string     m_headerText;

		// state flags
		bool m_isOpen;			         // BAM file is open for processing
		bool m_isIndexLoaded;	         // BAM Index data is loaded and available for processing
		bool m_isRegionSpecified;        // a region has been specified - specifically, a user has called Jump()
		bool m_isCalculateAlignedBases;  // build 'AlignedBases' string when getting an alignment, otherwise skip (default = true)

		// region values
		int m_currentRefID;
		unsigned int m_currentLeft;

		// file offset of 1st read in BAM file
		int64_t m_alignmentsBeginOffset;

	private:
		// BAM character constants
		static const char* DNA_LOOKUP;
		static const char* CIGAR_LOOKUP;
};

#endif /* BAMREADER_H */
