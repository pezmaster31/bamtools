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
template<typename COMP>
class SortPosition : public binary_function<BamAlignment, BamAlignment, bool >{
  public:
        bool operator() (const BamAlignment& lhs, const BamAlignment& rhs) {
            COMP c;
            if ( lhs.RefID != rhs.RefID ){
                return c(lhs.RefID, rhs.RefID);
            }else {
                int32_t lh,rh;
                lh = lhs.Position;
                rh = rhs.Position;
               // printf("%d %d %d %d\n",lh,rh,lhs.Position,rhs.Position);
               // return c(lhs.Position, rhs.Position);
                 return c(lh,rh);
            }
        }
    };

    
 // QNAME
  template<typename COMP>
  class SortName : public binary_function<BamAlignment, BamAlignment, bool> {
  public:
      bool operator() (const BamAlignment& lhs, const BamAlignment& rhs) {
          COMP c;
          return c(lhs.Name, rhs.Name);
        }
    };
      
    // AS Alignment Score from BFAST
       
    template<typename COMP>
    class SortAlignmentScore : public BamAlignmentBFunction{
        public:
        bool operator() ( const  BamAlignment& lhs, const BamAlignment&  rhs) const {
          int32_t lh, rh;
          lhs.GetTag("AS",lh);
          rhs.GetTag("AS",rh);
          
          COMP c;
          return c(lh,rh);
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
