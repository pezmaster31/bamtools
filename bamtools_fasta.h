#ifndef BAMTOOLS_FASTA_H
#define BAMTOOLS_FASTA_H

#include <string>

namespace BamTools {

class Fasta {  
  
    // ctor & dtor
    public:
        Fasta(void);
        ~Fasta(void);
        
    // file-handling methods
    public:
        bool Close(void);
        bool Open(const std::string& filename, const std::string& indexFilename = "");
        
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
  
} // BAMTOOLS_FASTA_H
  
#endif // BAMTOOLS_FASTA_H