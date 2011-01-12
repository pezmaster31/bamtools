// ***************************************************************************
// BamHeader_p.cpp (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 25 December 2010 (DB)
// ---------------------------------------------------------------------------
// Provides the basic functionality for handling BAM headers.
// ***************************************************************************

#include <api/BamAux.h>
#include <api/BamConstants.h>
#include <api/BGZF.h>
#include <api/internal/BamHeader_p.h>
using namespace BamTools;
using namespace BamTools::Internal;

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
using namespace std;

// ---------------------------------
// BamHeaderPrivate implementation

struct BamHeader::BamHeaderPrivate {

    // data members
    SamHeader* m_samHeader;

    // ctor
    BamHeaderPrivate(void)
        : m_samHeader(new SamHeader(""))
    { }

    // dtor
    ~BamHeaderPrivate(void) {
        delete m_samHeader;
        m_samHeader = 0;
    }

    // 'public' interface
    bool Load(BgzfData* stream);

    // internal methods
    bool CheckMagicNumber(BgzfData* stream);
    bool ReadHeaderLength(BgzfData* stream, uint32_t& length);
    bool ReadHeaderText(BgzfData* stream, const uint32_t& length);
};

bool BamHeader::BamHeaderPrivate::Load(BgzfData* stream) {

    // cannot load if invalid stream
    if ( stream == 0 )
        return false;

    // cannot load if magic number is invalid
    if ( !CheckMagicNumber(stream) )
        return false;

    // cannot load header if cannot read header length
    uint32_t length(0);
    if ( !ReadHeaderLength(stream, length) )
        return false;

    // cannot load header if cannot read header text
    if ( !ReadHeaderText(stream, length) )
        return false;

    // otherwise, everything OK
    return true;
}

bool BamHeader::BamHeaderPrivate::CheckMagicNumber(BgzfData* stream) {

    // try to read magic number
    char buffer[Constants::BAM_HEADER_MAGIC_SIZE];
    if ( stream->Read(buffer, Constants::BAM_HEADER_MAGIC_SIZE) != (int)Constants::BAM_HEADER_MAGIC_SIZE ) {
        fprintf(stderr, "BAM header error - could not read magic number\n");
        return false;
    }

    // validate magic number
    if ( strncmp(buffer, Constants::BAM_HEADER_MAGIC, Constants::BAM_HEADER_MAGIC_SIZE) != 0 ) {
        fprintf(stderr, "BAM header error - invalid magic number\n");
        return false;
    }

    // all checks out
    return true;
}

bool BamHeader::BamHeaderPrivate::ReadHeaderLength(BgzfData* stream, uint32_t& length) {

    // attempt to read BAM header text length
    char buffer[sizeof(uint32_t)];
    if ( stream->Read(buffer, sizeof(uint32_t)) != sizeof(uint32_t) ) {
        fprintf(stderr, "BAM header error - could not read header length\n");
        return false;
    }

    // convert char buffer to length, return success
    length = BgzfData::UnpackUnsignedInt(buffer);
    if ( BamTools::SystemIsBigEndian() )
        SwapEndian_32(length);
    return true;
}

bool BamHeader::BamHeaderPrivate::ReadHeaderText(BgzfData* stream, const uint32_t& length) {

    // set up destination buffer
    char* headerText = (char*)calloc(length + 1, 1);

    // attempt to read header text
    const unsigned bytesRead = stream->Read(headerText, length);
    const bool readOk = ( bytesRead == length );
    if ( readOk )
        m_samHeader->SetHeaderText( (string)((const char*)headerText) );
    else
        fprintf(stderr, "BAM header error - could not read header text\n");

    // clean up calloc-ed temp variable (on success or fail)
    free(headerText);

    // return read success
    return readOk;
}

// --------------------------
// BamHeader implementation

BamHeader::BamHeader(void)
    : d(new BamHeaderPrivate)
{ }

BamHeader::~BamHeader(void) {
    delete d;
    d = 0;
}

void BamHeader::Clear(void) {
    d->m_samHeader->Clear();
}

bool BamHeader::IsValid(void) const {
    return d->m_samHeader->IsValid();
}

bool BamHeader::Load(BgzfData* stream) {
    return d->Load(stream);
}

SamHeader BamHeader::ToSamHeader(void) const {
    return *(d->m_samHeader);
}

string BamHeader::ToString(void) const {
    return d->m_samHeader->ToString();
}
