// ***************************************************************************
// BamFtp_p.cpp (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 October 2011 (DB)
// ---------------------------------------------------------------------------
// Provides reading/writing of BAM files on FTP server
// ***************************************************************************

#include "api/internal/BamFtp_p.h"
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
    (void) mode;
    return true;
}

size_t BamFtp::Read(char* data, const unsigned int numBytes) {
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

size_t BamFtp::Write(const char* data, const unsigned int numBytes) {
    (void)data;
    (void)numBytes;
    return 0;
}
