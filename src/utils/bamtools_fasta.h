// ***************************************************************************
// bamtools_fasta.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 October 2011
// ---------------------------------------------------------------------------
// Provides FASTA reading/indexing functionality.
// ***************************************************************************

#ifndef BAMTOOLS_FASTA_H
#define BAMTOOLS_FASTA_H

#include <string>
#include "utils/utils_global.h"

namespace BamTools {

class UTILS_EXPORT Fasta
{

    // ctor & dtor
public:
    Fasta();
    ~Fasta();

    // file-handling methods
public:
    bool Close();
    bool Open(const std::string& filename, const std::string& indexFilename = std::string());

    // sequence access methods
public:
    bool GetBase(const int& refID, const int& position, char& base);
    bool GetSequence(const int& refId, const int& start, const int& stop, std::string& sequence);

    // index-handling methods
public:
    bool CreateIndex(const std::string& indexFilename);

    // internal implementation
private:
    struct FastaPrivate;
    FastaPrivate* d;
};

}  // namespace BamTools

#endif  // BAMTOOLS_FASTA_H
