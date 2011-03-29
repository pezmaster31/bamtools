// ***************************************************************************
// SamConstants.h (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 23 December 2010 (DB)
// ---------------------------------------------------------------------------
// Provides constants for SAM header
// ***************************************************************************

#ifndef SAM_CONSTANTS_H
#define SAM_CONSTANTS_H

#include <api/api_global.h>
#include <string>

namespace BamTools {
namespace Constants {

const char SAM_COLON  = ':';
const char SAM_EQUAL  = '=';
const char SAM_PERIOD = '.';
const char SAM_STAR   = '*';
const char SAM_TAB    = '\t';
const std::string SAM_DIGITS = "0123456789";

// HD entries
const std::string SAM_HD_BEGIN_TOKEN    = "@HD";
const std::string SAM_HD_VERSION_TAG    = "VN";
const std::string SAM_HD_SORTORDER_TAG  = "SO";
const std::string SAM_HD_GROUPORDER_TAG = "GO";

// SQ entries
const std::string SAM_SQ_BEGIN_TOKEN    = "@SQ";
const std::string SAM_SQ_NAME_TAG       = "SN";
const std::string SAM_SQ_LENGTH_TAG     = "LN";
const std::string SAM_SQ_ASSEMBLYID_TAG = "AS";
const std::string SAM_SQ_URI_TAG        = "UR";
const std::string SAM_SQ_CHECKSUM_TAG   = "M5";
const std::string SAM_SQ_SPECIES_TAG    = "SP";

// RG entries
const std::string SAM_RG_BEGIN_TOKEN             = "@RG";
const std::string SAM_RG_ID_TAG                  = "ID";
const std::string SAM_RG_SAMPLE_TAG              = "SM";
const std::string SAM_RG_LIBRARY_TAG             = "LB";
const std::string SAM_RG_DESCRIPTION_TAG         = "DS";
const std::string SAM_RG_PLATFORMUNIT_TAG        = "PU";
const std::string SAM_RG_PREDICTEDINSERTSIZE_TAG = "PI";
const std::string SAM_RG_SEQCENTER_TAG           = "CN";
const std::string SAM_RG_PRODUCTIONDATE_TAG      = "DT";
const std::string SAM_RG_SEQTECHNOLOGY_TAG       = "PL";

// PG entries
const std::string SAM_PG_BEGIN_TOKEN     = "@PG";
const std::string SAM_PG_NAME_TAG        = "ID";
const std::string SAM_PG_VERSION_TAG     = "VN";
const std::string SAM_PG_COMMANDLINE_TAG = "CL";

// CO entries
const std::string SAM_CO_BEGIN_TOKEN = "@CO";

// HD:SO values
const std::string SAM_HD_SORTORDER_COORDINATE = "coordinate";
const std::string SAM_HD_SORTORDER_QUERYNAME  = "queryname";
const std::string SAM_HD_SORTORDER_ALIGNMENTPOSTION = "alignmentposition";
const std::string SAM_HD_SORTORDER_UNSORTED   = "unsorted";

// HD:GO values
const std::string SAM_HD_GROUPORDER_NONE      = "none";
const std::string SAM_HD_GROUPORDER_QUERY     = "query";
const std::string SAM_HD_GROUPORDER_REFERENCE = "reference";

// SQ:LN values
const unsigned int SAM_SQ_LENGTH_MIN = 1;
const unsigned int SAM_SQ_LENGTH_MAX = 536870911; // 2^29 - 1

// --------------
// RG:PL values

// 454
const std::string SAM_RG_SEQTECHNOLOGY_454         = "454";
const std::string SAM_RG_SEQTECHNOLOGY_LS454_LOWER = "ls454";
const std::string SAM_RG_SEQTECHNOLOGY_LS454_UPPER = "LS454";

// Helicos
const std::string SAM_RG_SEQTECHNOLOGY_HELICOS_LOWER = "helicos";
const std::string SAM_RG_SEQTECHNOLOGY_HELICOS_UPPER = "HELICOS";

// Illumina
const std::string SAM_RG_SEQTECHNOLOGY_ILLUMINA_LOWER = "illumina";
const std::string SAM_RG_SEQTECHNOLOGY_ILLUMINA_UPPER = "ILLUMINA";

// PacBio
const std::string SAM_RG_SEQTECHNOLOGY_PACBIO_LOWER = "pacbio";
const std::string SAM_RG_SEQTECHNOLOGY_PACBIO_UPPER = "PACBIO";

// SOLiD
const std::string SAM_RG_SEQTECHNOLOGY_SOLID_LOWER = "solid";
const std::string SAM_RG_SEQTECHNOLOGY_SOLID_UPPER = "SOLID";

} // namespace Constants
} // namespace BamTools

#endif // SAM_CONSTANTS_H
