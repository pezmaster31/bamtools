#ifndef BAMSORTCRITERIA_H
#define BAMSORTCRITERIA_H

#include <api/BamAlignment.h>
#include <vector>
#include <api/internal/BamMultiMerger_p.h>

using namespace std;


using namespace BamTools::Internal;
namespace BamTools {

typedef binary_function<BamAlignment,BamAlignment,bool> BamAlignmentBFunction;
typedef binary_function<ReaderAlignment,ReaderAlignment,bool> ReadAlignmentBFunction;
typedef vector< BamAlignment >::iterator BamAlignmentIterator;


class BamSortCriteria{
private:
  string sortCriteria;
  bool descending;
  static const string allowedTags[];
  static const string coreTags[];
  
public:
  static string getAllowedTags();
  BamSortCriteria():sortCriteria("QNAME"),descending(false){}
  
  
  BamSortCriteria(string sortCriteria, bool descending):sortCriteria(sortCriteria),descending(descending){
    if(!isTagValid()){
     cerr << "BamSortCriteria ERROR: Requested sort order ("<<sortCriteria << ") is unknown. Valid tags are: "<<getAllowedTags()<<"\n";
    }
  }
  
  string getSortCriteria(){return sortCriteria;};
  bool isDescending(){return descending;};
  void sortBuffer(BamAlignmentIterator begin,BamAlignmentIterator end);
  IBamMultiMerger* getMerger(void);
  string getSamHeaderSort();
  bool isTagCoreAttribute();
  bool isTagValid();
  static bool isTagValid(const string& crit);
  
};




/**
 * Classes that extend the binary_function and have a operator() function to compare two
 * BamAlignment for sorting. This class has to be created if bammtools should sort for other 
 * criterias
 **/


// POS
class SortLessThanPosition : public binary_function<BamAlignment, BamAlignment, bool >{
  public:
        bool operator() (const BamAlignment& lhs, const BamAlignment& rhs) {
          
            if ( lhs.RefID != rhs.RefID )
                return lhs.RefID < rhs.RefID;
            else 
                return lhs.Position < rhs.Position;
        }
    };

  class SortGreaterThanPosition : public binary_function<BamAlignment, BamAlignment, bool >{
  public:
        bool operator() (const BamAlignment& lhs, const BamAlignment& rhs) {
          
            if ( lhs.RefID != rhs.RefID )
                return lhs.RefID > rhs.RefID;
            else 
                return lhs.Position > rhs.Position;
        }
    };
    
 // QNAME
 class SortLessThanName : public binary_function<BamAlignment, BamAlignment, bool> {
  public:
      bool operator() (const BamAlignment& lhs, const BamAlignment& rhs) {
            return lhs.Name < rhs.Name;
        }
    };
    
  class SortGreaterThanName : public binary_function<BamAlignment, BamAlignment, bool> {
  public:
      bool operator() (const BamAlignment& lhs, const BamAlignment& rhs) {
            return lhs.Name > rhs.Name;
        }
    };
     
    // AS Alignment Score from BFAST
    class SortLessThanAlignmentScore : public BamAlignmentBFunction{//binary_function<BamAlignment, BamAlignment, bool>{
        public:
        bool operator() ( const  BamAlignment& lhs, const BamAlignment&  rhs) const {
          uint32_t lh, rh;
          lhs.GetTag("AS",lh);
          rhs.GetTag("AS",rh);
          return lh < rh;
        }
    };
    
    class SortGreaterThanAlignmentScore : public BamAlignmentBFunction{//binary_function<BamAlignment, BamAlignment, bool>{
        public:
        bool operator() ( const  BamAlignment& lhs, const BamAlignment&  rhs) const {
          uint32_t lh, rh;
          lhs.GetTag("AS",lh);
          rhs.GetTag("AS",rh);
          return lh > rh;
        }
    };
    
    
    //--------------------------------------------------------
    /**
     * The two classes are used for a descending search as they flip the conditions of the defined
     * SortLessThan* classes. One is used for the Reader and th other for the BamAlignment
     **/
    template<typename T>
    class SortReaderAlignment: public binary_function<ReaderAlignment, ReaderAlignment, bool>{
    public:
      bool operator() (const ReaderAlignment& lhs, const ReaderAlignment& rhs){
        T t;
        const BamAlignment l= *lhs.second;
        const BamAlignment r= *rhs.second;
        return t(l,r);
      }
    };
    
}

#endif // BAMSORTCRITERIA_H
