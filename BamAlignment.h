// BamAlignment.h

// Derek Barnett
// Marth Lab, Boston College
// Last modified: 20 March 2009

#ifndef BAMALIGNMENT_H
#define BAMALIGNMENT_H

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

// C++ includes
#include <string>
using std::string;

#include <vector>
using std::vector;

struct CigarOp {
	uint32_t Length;
	char     Type;
};

struct RefData {
  string       RefName;
  unsigned int RefLength;
  bool         RefHasAlignments;

  // constructor
  RefData(void)
  : RefLength(0)
  , RefHasAlignments(false)
  { }
};

typedef vector<RefData> RefVector;

struct BamAlignment {

    // queries against alignment flag - see below for further detail
    public:
        bool IsPaired(void) const            { return ( (AlignmentFlag & PAIRED)        != 0 ); }
        bool IsProperPair(void) const        { return ( (AlignmentFlag & PROPER_PAIR)   != 0 ); }
        bool IsMapped(void) const            { return ( (AlignmentFlag & UNMAPPED)      == 0 ); }
        bool IsMateMapped(void) const        { return ( (AlignmentFlag & MATE_UNMAPPED) == 0 ); }
        bool IsReverseStrand(void) const     { return ( (AlignmentFlag & REVERSE)       != 0 ); }
        bool IsMateReverseStrand(void) const { return ( (AlignmentFlag & MATE_REVERSE)  != 0 ); }
        bool IsFirstMate(void) const         { return ( (AlignmentFlag & READ_1)        != 0 ); }
        bool IsSecondMate(void) const        { return ( (AlignmentFlag & READ_2)        != 0 ); }
        bool IsPrimaryAlignment(void) const  { return ( (AlignmentFlag & SECONDARY)     == 0 ); }
        bool IsFailedQC(void) const          { return ( (AlignmentFlag & QC_FAILED)     != 0 ); }
        bool IsDuplicate(void) const         { return ( (AlignmentFlag & DUPLICATE)     != 0 ); }

    // data members
    public:
        string       Name;           // read name
        unsigned int Length;         // query length
        string       QueryBases;     // original sequence ( produced from machine )
        string       AlignedBases;   // aligned sequence ( with indels ) 
        string       Qualities;      // FASTQ qualities ( still in ASCII characters )
		vector<string> Tags;
        unsigned int RefID;          // ID for reference sequence
        unsigned int Position;       // position on reference sequence where alignment starts
        unsigned int Bin;            // bin in BAM file where this alignment resides
        unsigned int MapQuality;     // mapping quality 
        unsigned int AlignmentFlag;  // see above for available queries
		vector<CigarOp> CigarData;   // vector of CIGAR operations (length & type) )
        unsigned int MateRefID;      // ID for reference sequence that mate was aligned to
        unsigned int MatePosition;   // position that mate was aligned to
        unsigned int InsertSize;     // mate pair insert size
		

    // alignment flag query constants
    private:
        enum { PAIRED        = 1,		// Alignment comes from paired-end data
               PROPER_PAIR   = 2,		// Alignment passed paired-end resolution
               UNMAPPED      = 4,		// Read is unmapped
               MATE_UNMAPPED = 8,		// Mate is unmapped
               REVERSE       = 16,		// Read is on reverse strand
               MATE_REVERSE  = 32,		// Mate is on reverse strand
               READ_1        = 64, 		// This alignment is mate 1 of pair
               READ_2        = 128,		// This alignment is mate 2 of pair
               SECONDARY     = 256,		// This alignment is not the primary (best) alignment for read
               QC_FAILED     = 512,		// Read did not pass prior quality control steps
               DUPLICATE     = 1024		// Read is PCR duplicate
        };
};

// commonly used vector in this library
typedef vector< BamAlignment > BamAlignmentVector;

#endif /* BAMALIGNMENT_H */
