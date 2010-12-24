// ***************************************************************************
// SamFormatPrinter.cpp (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 23 December 2010 (DB)
// ---------------------------------------------------------------------------
// Provides functionality for printing formatted SAM header to string
// ***************************************************************************

#include <api/SamConstants.h>
#include <api/SamHeader.h>
#include <api/internal/SamFormatPrinter_p.h>
using namespace BamTools;
using namespace BamTools::Internal;

#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

SamFormatPrinter::SamFormatPrinter(const SamHeader& header)
    : m_header(header)
{ }

SamFormatPrinter::~SamFormatPrinter(void) { }

const string SamFormatPrinter::FormatTag(const string &tag, const string &value) const {
    return string(Constants::SAM_TAB + tag + Constants::SAM_COLON + value);
}

const string SamFormatPrinter::ToString(void) const {

    // clear out stream
    stringstream out("");

    // generate formatted header text
    PrintHD(out);
    PrintSQ(out);
    PrintRG(out);
    PrintPG(out);
    PrintCO(out);

    // return result
    return out.str();
}

void SamFormatPrinter::PrintHD(std::stringstream& out) const {

    // if header has @HD data
    if ( m_header.HasVersion() ) {

        // @HD VN:<Version>
        out << Constants::SAM_HD_BEGIN_TOKEN
            << FormatTag(Constants::SAM_HD_VERSION_TAG, m_header.Version);

        // SO:<SortOrder>
        if ( m_header.HasSortOrder() )
            out << FormatTag(Constants::SAM_HD_SORTORDER_TAG, m_header.SortOrder);

        // GO:<GroupOrder>
        if ( m_header.HasGroupOrder() )
            out << FormatTag(Constants::SAM_HD_GROUPORDER_TAG, m_header.GroupOrder);

        // newline
        out << endl;
    }
}

void SamFormatPrinter::PrintSQ(std::stringstream& out) const {

    // iterate over sequence entries
    SamSequenceConstIterator seqIter = m_header.Sequences.ConstBegin();
    SamSequenceConstIterator seqEnd  = m_header.Sequences.ConstEnd();
    for ( ; seqIter != seqEnd; ++seqIter ) {
        const SamSequence& seq = (*seqIter);

        // @SQ SN:<Name> LN:<Length>
        out << Constants::SAM_SQ_BEGIN_TOKEN
            << FormatTag(Constants::SAM_SQ_NAME_TAG, seq.Name)
            << FormatTag(Constants::SAM_SQ_LENGTH_TAG, seq.Length);

        // AS:<AssemblyID>
        if ( seq.HasAssemblyID() )
            out << FormatTag(Constants::SAM_SQ_ASSEMBLYID_TAG, seq.AssemblyID);

        // M5:<Checksum>
        if ( seq.HasChecksum() )
            out << FormatTag(Constants::SAM_SQ_CHECKSUM_TAG, seq.Checksum);

        // UR:<URI>
        if ( seq.HasURI() )
            out << FormatTag(Constants::SAM_SQ_URI_TAG, seq.URI);

        // SP:<Species>
        if ( seq.HasSpecies() )
            out << FormatTag(Constants::SAM_SQ_SPECIES_TAG, seq.Species);

        // newline
        out << endl;
    }
}

void SamFormatPrinter::PrintRG(std::stringstream& out) const {

    // iterate over read group entries
    SamReadGroupConstIterator rgIter = m_header.ReadGroups.ConstBegin();
    SamReadGroupConstIterator rgEnd  = m_header.ReadGroups.ConstEnd();
    for ( ; rgIter != rgEnd; ++rgIter ) {
        const SamReadGroup& rg = (*rgIter);

        // @RG ID:<ID> SM:<Sample>
        out << Constants::SAM_RG_BEGIN_TOKEN
            << FormatTag(Constants::SAM_RG_ID_TAG, rg.ID)
            << FormatTag(Constants::SAM_RG_SAMPLE_TAG, rg.Sample);

        // LB:<Library>
        if ( rg.HasLibrary() )
            out << FormatTag(Constants::SAM_RG_LIBRARY_TAG, rg.Library);

        // DS:<Description>
        if ( rg.HasDescription() )
            out << FormatTag(Constants::SAM_RG_DESCRIPTION_TAG, rg.Description);

        // PU:<PlatformUnit>
        if ( rg.HasPlatformUnit() )
            out << FormatTag(Constants::SAM_RG_PLATFORMUNIT_TAG, rg.PlatformUnit);

        // PI:<PredictedInsertSize>
        if ( rg.HasPredictedInsertSize() )
            out << FormatTag(Constants::SAM_RG_PREDICTEDINSERTSIZE_TAG, rg.PredictedInsertSize);

        // CN:<SequencingCenter>
        if ( rg.HasSequencingCenter() )
            out << FormatTag(Constants::SAM_RG_SEQCENTER_TAG, rg.SequencingCenter);

        // DT:<ProductionDate>
        if ( rg.HasProductionDate() )
            out << FormatTag(Constants::SAM_RG_PRODUCTIONDATE_TAG, rg.ProductionDate);

        // PL:<SequencingTechnology>
        if ( rg.HasSequencingTechnology() )
            out << FormatTag(Constants::SAM_RG_SEQTECHNOLOGY_TAG, rg.SequencingTechnology);

        // newline
        out << endl;
    }
}

void SamFormatPrinter::PrintPG(std::stringstream& out) const {

    // if header has @PG data
    if ( m_header.HasProgramName() ) {

        // @PG ID:<ProgramName>
        out << Constants::SAM_PG_BEGIN_TOKEN
            << FormatTag(Constants::SAM_PG_NAME_TAG, m_header.ProgramName);

        // VN:<ProgramVersion>
        if ( m_header.HasProgramVersion() )
            out << FormatTag(Constants::SAM_PG_VERSION_TAG, m_header.ProgramVersion);

        // CL:<ProgramCommandLine>
        if ( m_header.HasProgramCommandLine() )
            out << FormatTag(Constants::SAM_PG_COMMANDLINE_TAG, m_header.ProgramCommandLine);

        // newline
        out << endl;
    }
}

void SamFormatPrinter::PrintCO(std::stringstream& out) const {

    // iterate over comments
    vector<string>::const_iterator commentIter = m_header.Comments.begin();
    vector<string>::const_iterator commentEnd  = m_header.Comments.end();
    for ( ; commentIter != commentEnd; ++commentIter ) {

        // @CO <Comment>
        out << Constants::SAM_CO_BEGIN_TOKEN
            << Constants::SAM_TAB
            << (*commentIter)
            << endl;
    }
}
