#include "BamSortCriteria.h"
#include <algorithm>
#include <api/internal/BamMultiReader_p.h>
#include <api/SamConstants.h>

using namespace BamTools;
const string BamSortCriteria::allowedTags[3]={"QNAME","POS","AS"};
const string BamSortCriteria::coreTags[1]={"POS"};


bool BamSortCriteria::isTagCoreAttribute() {
    int length = sizeof(coreTags)/sizeof(coreTags[0]);
    for(int i=0;i<length;i++){
        if(sortCriteria == coreTags[i]){
          return true;
      }
    }
    return false;
}

string BamSortCriteria::getAllowedTags(){
  string s;
  int length = sizeof(allowedTags)/sizeof(allowedTags[0]);
  s = allowedTags[0];
  for(int i=1;i<length;i++){
      s+=", "+ allowedTags[i];
  }
  return s;
}

bool BamSortCriteria::isTagValid(const string& tag) {
  int length = sizeof(allowedTags)/sizeof(allowedTags[0]);
    for(int i=0;i<length;i++){
        if(tag.compare(allowedTags[i])==0){
          return true;
      }
    }
    return false;
}

bool BamSortCriteria::isTagValid() {
    return isTagValid(sortCriteria);
}



IBamMultiMerger* BamSortCriteria::getMerger(void ) {
    
    if(descending){
      if (sortCriteria=="QNAME") {
          return new CommonMultiMerger<SortReaderAlignment<SortGreaterThanName> >();
      } else if (sortCriteria=="POS") {
          return new CommonMultiMerger<SortReaderAlignment<SortGreaterThanPosition> >();
      } else if (sortCriteria=="AS") {
          return new CommonMultiMerger<SortReaderAlignment<SortGreaterThanAlignmentScore> >();
      } else if (sortCriteria == "") {
          return new UnsortedMultiMerger;
      }
    }else{
      if (sortCriteria=="QNAME") {
          return new CommonMultiMerger<SortReaderAlignment<SortLessThanName> >();
      } else if (sortCriteria=="POS") {
          return new CommonMultiMerger<SortReaderAlignment<SortLessThanPosition> >();
      } else if (sortCriteria=="AS") {
          return new CommonMultiMerger<SortReaderAlignment<SortLessThanAlignmentScore> >();
      } else if (sortCriteria == "") {
          return new UnsortedMultiMerger;
      }
    }
    cerr << "BamMultiReader ERROR: requested sort order is unknown" << endl;
    return 0;
}


void BamSortCriteria::sortBuffer(BamAlignmentIterator begin, BamAlignmentIterator end) {
    /**
     * It seems like this step could not be simplified for the
     * ascending and descending case
     */
   if (!descending ) {
        if (sortCriteria=="QNAME") {
            sort(begin,end,SortLessThanName());
        } else if (sortCriteria=="POS") {
            sort(begin,end,SortLessThanPosition());
        } else if (sortCriteria=="AS") {
            sort(begin,end,SortLessThanAlignmentScore());
        } else {
            cerr << "BamMultiReader ERROR: requested sort order ("<<sortCriteria<<")is unknown" << endl;
        }
    
    } else {
        if (sortCriteria=="QNAME") {//SortSortLessThanName()
            sort(begin,end,SortGreaterThanName());
        } else if (sortCriteria=="POS") {
            sort(begin,end,SortGreaterThanPosition());
        } else if (sortCriteria=="AS") {
            sort(begin,end,SortGreaterThanAlignmentScore());
        } else {
            cerr << "BamMultiReader ERROR: requested sort order ("<<sortCriteria<<")is unknown" << endl;
        }
    }
}



string BamSortCriteria::getSamHeaderSort(){
  if(sortCriteria=="QNAME"){
      return Constants::SAM_HD_SORTORDER_QUERYNAME;
    }else if(sortCriteria =="POS"){
      return Constants::SAM_HD_SORTORDER_COORDINATE;
    }else if(sortCriteria =="AS"){
      return  Constants::SAM_HD_SORTORDER_ALIGNMENTSCORE;
    }
    cerr<<"bamtools sort ERROR: Sort criteria " << sortCriteria <<" could not be found"<<endl;
    return "";
}

