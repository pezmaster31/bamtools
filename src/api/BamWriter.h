// ***************************************************************************
// BamWriter.h (c) 2009 Michael Str�mberg, Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 October 2011 (DB)
// ---------------------------------------------------------------------------
// Provides the basic functionality for producing BAM files
// ***************************************************************************

#ifndef BAMWRITER_H
#define BAMWRITER_H

#include <string>
#include "api/BamAux.h"
#include "api/api_global.h"

namespace BamTools {

class BamAlignment;
struct SamHeader;

//! \cond
namespace Internal {
class BamWriterPrivate;
}  // namespace Internal
//! \endcond

class API_EXPORT BamWriter
{

    // enums
public:
    enum CompressionMode
    {
        Compressed = 0,
        Uncompressed
    };

    // ctor & dtor
public:
    BamWriter();
    ~BamWriter();

    // public interface
public:
    //  closes the current BAM file
    void Close();
    // returns a human-readable description of the last error that occurred
    std::string GetErrorString() const;
    // returns true if BAM file is open for writing
    bool IsOpen() const;
    // opens a BAM file for writing
    bool Open(const std::string& filename, const std::string& samHeaderText,
              const RefVector& referenceSequences);
    // opens a BAM file for writing
    bool Open(const std::string& filename, const SamHeader& samHeader,
              const RefVector& referenceSequences);
    // saves the alignment to the alignment archive
    bool SaveAlignment(const BamAlignment& alignment);
    // sets the output compression mode
    void SetCompressionMode(const BamWriter::CompressionMode& compressionMode);

    // private implementation
private:
    Internal::BamWriterPrivate* d;
};

}  // namespace BamTools

#endif  // BAMWRITER_H
