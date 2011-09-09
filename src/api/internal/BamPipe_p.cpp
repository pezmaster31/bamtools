// ***************************************************************************
// BamPipe_p.cpp (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 9 September 2011 (DB)
// ---------------------------------------------------------------------------
// Provides BAM pipe-specific IO behavior
// ***************************************************************************

#include <api/internal/BamPipe_p.h>
using namespace BamTools;
using namespace BamTools::Internal;

#include <cstdio>
#include <iostream>
using namespace std;

BamPipe::BamPipe(void) : ILocalIODevice() { }

BamPipe::~BamPipe(void) { }

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

bool BamPipe::Seek(const int64_t& position) {
//    (void)position; // suppress compiler warning about unused variable
//    return false;   // seeking not allowed in pipe

    BT_ASSERT_X( m_stream, "BamFile::Seek() - null stream" );
    cerr << "BamPipe::Seek() - about to attempt seek" << endl;
    bool result = ( fseek64(m_stream, position, SEEK_SET) == 0);
    if ( !result ) {
        cerr << "BamPipe can't be seeked in" << endl;
    }
    return result;

//    return ( fseek64(m_stream, position, SEEK_SET) == 0);

}
