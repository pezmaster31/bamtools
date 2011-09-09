// ***************************************************************************
// BamFile_p.cpp (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 8 September 2011 (DB)
// ---------------------------------------------------------------------------
// Provides reading of local BAM files
// ***************************************************************************

#include <api/internal/BamFile_p.h>
using namespace BamTools;
using namespace BamTools::Internal;

#include <cstdio>
using namespace std;

BamFile::BamFile(const string& filename)
    : IBamIODevice()
    , m_stream(0)
    , m_filename(filename)
{ }

BamFile::~BamFile(void) {
    Close();
}

void BamFile::Close(void) {

    // skip if not open
    if ( !IsOpen() )
        return;

    // flush & close FILE*
    fflush(m_stream);
    fclose(m_stream);

    // reset internals
    m_mode = IBamIODevice::NotOpen;
    m_stream = 0;
    m_filename.clear();
}

bool BamFile::IsRandomAccess(void) const {
    return true;
}

bool BamFile::Open(const IBamIODevice::OpenMode mode) {

    // make sure we're starting with a fresh file stream
    Close();

    // attempt to open FILE* depending on requested openmode
    if ( mode == IBamIODevice::ReadOnly )
        m_stream = fopen(m_filename.c_str(), "rb");
    else if ( mode == IBamIODevice::WriteOnly )
        m_stream = fopen(m_filename.c_str(), "wb");
    else {
        SetErrorString("BamFile ERROR - unknown device open mode");
        return false;
    }

    // check that we obtained a valid FILE*
    if ( m_stream == 0 ) {
        string error = "BamFile ERROR - could not open handle on ";
        error += ( (m_filename.empty()) ? "empty filename" : m_filename );
        SetErrorString(error);
        return false;
    }

    // store current IO mode & return success
    m_mode = mode;
    return true;
}

size_t BamFile::Read(char* data, const unsigned int numBytes) {
    BT_ASSERT_X( m_stream, "BamFile::Read() - null stream" );
    BT_ASSERT_X( (m_mode == IBamIODevice::ReadOnly), "BamFile::Read() - device not in read-only mode");
    return fread(data, sizeof(char), numBytes, m_stream);
}

bool BamFile::Seek(const int64_t& position) {
    BT_ASSERT_X( m_stream, "BamFile::Seek() - null stream" );
    return ( fseek64(m_stream, position, SEEK_SET) == 0);
}

int64_t BamFile::Tell(void) const {
    BT_ASSERT_X( m_stream, "BamFile::Tell() - null stream" );
    return ftell64(m_stream);
}

size_t BamFile::Write(const char* data, const unsigned int numBytes) {
    BT_ASSERT_X( m_stream, "BamFile::Write() - null stream" );
    BT_ASSERT_X( (m_mode == IBamIODevice::WriteOnly), "BamFile::Write() - device not in write-only mode" );
    return fwrite(data, sizeof(char), numBytes, m_stream);
}
