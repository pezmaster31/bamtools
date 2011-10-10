// ***************************************************************************
// BamHttp_p.cpp (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 October 2011 (DB)
// ---------------------------------------------------------------------------
// Provides reading/writing of BAM files on HTTP server
// ***************************************************************************

#include "api/internal/BamHttp_p.h"
using namespace BamTools;
using namespace BamTools::Internal;

using namespace std;

BamHttp::BamHttp(const string& url)
    : IBamIODevice()
{
    BT_ASSERT_X(false, "BamHttp not yet implemented");
}

BamHttp::~BamHttp(void) { }

void BamHttp::Close(void) {
    return ;
}

bool BamHttp::IsRandomAccess(void) const {
    return true;
}

bool BamHttp::Open(const IBamIODevice::OpenMode mode) {
    (void) mode;
    return true;
}

size_t BamHttp::Read(char* data, const unsigned int numBytes) {
    (void)data;
    (void)numBytes;
    return 0;
}

bool BamHttp::Seek(const int64_t& position) {
    (void)position;
    return true;
}

int64_t BamHttp::Tell(void) const {
    return -1;
}

size_t BamHttp::Write(const char* data, const unsigned int numBytes) {
    (void)data;
    (void)numBytes;
    return 0;
}
