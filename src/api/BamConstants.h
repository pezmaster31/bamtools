#ifndef BAM_CONSTANTS_H
#define BAM_CONSTANTS_H

#include <string>

namespace BamTools {
namespace Constants {

const int BAM_SIZEOF_INT = 4;

// header magic number
const char* const  BAM_HEADER_MAGIC = "BAM\001";
const unsigned int BAM_HEADER_MAGIC_SIZE = 4;

// BAM alignment core size
const int BAM_CORE_SIZE = 32;

// BAM alignment flags
const int BAM_ALIGNMENT_PAIRED              = 1;
const int BAM_ALIGNMENT_PROPER_PAIR         = 2;
const int BAM_ALIGNMENT_UNMAPPED            = 4;
const int BAM_ALIGNMENT_MATE_UNMAPPED       = 8;
const int BAM_ALIGNMENT_REVERSE_STRAND      = 16;
const int BAM_ALIGNMENT_MATE_REVERSE_STRAND = 32;
const int BAM_ALIGNMENT_READ_1              = 64;
const int BAM_ALIGNMENT_READ_2              = 128;
const int BAM_ALIGNMENT_SECONDARY           = 256;
const int BAM_ALIGNMENT_QC_FAILED           = 512;
const int BAM_ALIGNMENT_DUPLICATE           = 1024;

// CIGAR constants
const char* const BAM_CIGAR_LOOKUP = "MIDNSHP";
const int BAM_CIGAR_MATCH    = 0;
const int BAM_CIGAR_INS      = 1;
const int BAM_CIGAR_DEL      = 2;
const int BAM_CIGAR_REFSKIP  = 3;
const int BAM_CIGAR_SOFTCLIP = 4;
const int BAM_CIGAR_HARDCLIP = 5;
const int BAM_CIGAR_PAD      = 6;

const int BAM_CIGAR_SHIFT    = 4;
const int BAM_CIGAR_MASK     = ((1 << BAM_CIGAR_SHIFT) - 1);

// DNA bases
const char* const BAM_DNA_LOOKUP = "=ACMGRSVTWYHKDBN";

} // namespace Constants
} // namespace BamTools

#endif // BAM_CONSTANTS_H
