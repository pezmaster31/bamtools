// ***************************************************************************
// BamWriter.h (c) 2009 Michael Str�mberg, Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 19 November 2010 (DB)
// ---------------------------------------------------------------------------
// Uses BGZF routines were adapted from the bgzf.c code developed at the Broad
// Institute.
// ---------------------------------------------------------------------------
// Provides the basic functionality for producing BAM files
// ***************************************************************************

#ifndef BAMWRITER_H
#define BAMWRITER_H

#include <api/api_global.h>
#include <api/BamAlignment.h>
#include <string>

namespace BamTools {

class API_EXPORT BamWriter {

    // constructor/destructor
    public:
        BamWriter(void);
        ~BamWriter(void);

    // public interface
    public:
        // closes the alignment archive
        void Close(void);
        // opens the alignment archive
        bool Open(const std::string& filename, 
                  const std::string& samHeader, 
                  const BamTools::RefVector& referenceSequences, 
                  bool writeUncompressed = false);
        // saves the alignment to the alignment archive
        void SaveAlignment(const BamTools::BamAlignment& al);

    // private implementation
    private:
        struct BamWriterPrivate;
        BamWriterPrivate* d;
};

} // namespace BamTools

#endif // BAMWRITER_H
