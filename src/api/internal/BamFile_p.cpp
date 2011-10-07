// ***************************************************************************
// BamFile_p.cpp (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 9 September 2011 (DB)
// ---------------------------------------------------------------------------
// Provides BAM file-specific IO behavior
// ***************************************************************************

#include <api/internal/BamFile_p.h>
using namespace BamTools;
using namespace BamTools::Internal;

#include <cstdio>
#include <iostream>
using namespace std;

BamFile::BamFile(const string& filename)
    : ILocalIODevice()
    , m_filename(filename)
{ }

BamFile::~BamFile(void) { }

void BamFile::Close(void) {
    if ( IsOpen() ) {
        m_filename.clear();
        ILocalIODevice::Close();
    }
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

bool BamFile::Seek(const int64_t& position) {
    BT_ASSERT_X( m_stream, "BamFile::Seek() - null stream" );
    cerr << "BamFile::Seek() - about to attempt seek" << endl;
    return ( fseek64(m_stream, position, SEEK_SET) == 0);
}
