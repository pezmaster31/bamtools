// ***************************************************************************
// BamPipe_p.cpp (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 8 September 2011 (DB)
// ---------------------------------------------------------------------------
// Provides reading/writing of piped BAM files (stdin/stdout)
// ***************************************************************************

#include <api/internal/BamPipe_p.h>
using namespace BamTools;
using namespace BamTools::Internal;

#include <cstdio>
using namespace std;

BamPipe::BamPipe(void)
    : IBamIODevice()
    , m_stream(0)
{ }

BamPipe::~BamPipe(void) {
    Close();
}

void BamPipe::Close(void) {

    // skip if not open
    if ( !IsOpen() ) return;

    // flush & close FILE*
    fflush(m_stream);
    fclose(m_stream);

    // reset internals
    m_mode = IBamIODevice::NotOpen;
    m_stream = 0;
}

bool BamPipe::IsRandomAccess(void) const {
    return false;
}

bool BamPipe::Open(const IBamIODevice::OpenMode mode) {

    // make sure we're starting with a fresh pipe
    Close();

    // open stdin/stdout depending on requested openmode
    if ( mode == IBamIODevice::ReadOnly )
        m_stream = freopen(0, "rb", stdin);
    else if ( mode == IBamIODevice::WriteOnly )
        m_stream = freopen(0, "wb", stdout);
    else {
        SetErrorString("BamPipe ERROR - unsupported device mode");
        return false;
    }

    // check that we obtained a valid FILE*
    if ( m_stream == 0 ) {
        string error = "BamPipe ERROR - could not open handle on ";
        error += ( (mode == IBamIODevice::ReadOnly) ? "stdin" : "stdout" );
        SetErrorString(error);
        return false;
    }

    // store current IO mode & return success
    m_mode = mode;
    return true;
}

size_t BamPipe::Read(char* data, const unsigned int numBytes) {
    BT_ASSERT_X( m_stream, "BamPipe::Read() - null stream" );
    BT_ASSERT_X( (m_mode == IBamIODevice::ReadOnly), "BamPipe::Read() - device not in read-only mode");
    return fread(data, sizeof(char), numBytes, m_stream);
}

bool BamPipe::Seek(const int64_t& position) {
    (void)position; // suppress compiler warning about unused variable
    return false;   // seeking not allowed in pipe
}

int64_t BamPipe::Tell(void) const {
    BT_ASSERT_X( m_stream, "BamPipe::Tell() - null stream" );
    return ftell64(m_stream);
}

size_t BamPipe::Write(const char* data, const unsigned int numBytes) {
    BT_ASSERT_X( m_stream, "BamPipe::Write() - null stream" );
    BT_ASSERT_X( (m_mode == IBamIODevice::WriteOnly), "BamPipe::Write() - device not in write-only mode" );
    return fwrite(data, sizeof(char), numBytes, m_stream);
}
