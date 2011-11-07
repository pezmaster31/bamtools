// ***************************************************************************
// BamFtp_p.cpp (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 25 October 2011 (DB)
// ---------------------------------------------------------------------------
// Provides reading/writing of BAM files on FTP server
// ***************************************************************************

#include "api/internal/io/BamFtp_p.h"
using namespace BamTools;
using namespace BamTools::Internal;

using namespace std;

BamFtp::BamFtp(const string& url)
    : IBamIODevice()
{
    BT_ASSERT_X(false, "BamFtp not yet implemented");
}

BamFtp::~BamFtp(void) { }

void BamFtp::Close(void) {
    return ;
}

bool BamFtp::IsRandomAccess(void) const {
    return true;
}

bool BamFtp::Open(const IBamIODevice::OpenMode mode) {

    if ( mode != IBamIODevice::ReadOnly ) {
        SetErrorString("BamFtp::Open", "writing on this device is not supported");
        return false;
    }


    return true;
}

int64_t BamFtp::Read(char* data, const unsigned int numBytes) {
    (void)data;
    (void)numBytes;
    return 0;
}

bool BamFtp::Seek(const int64_t& position) {
    (void)position;
    return true;
}

int64_t BamFtp::Tell(void) const {
    return -1;
}

int64_t BamFtp::Write(const char* data, const unsigned int numBytes) {
    (void)data;
    (void)numBytes;
    BT_ASSERT_X(false, "BamFtp::Write : write-mode not supported on this device");
    return 0;
}
