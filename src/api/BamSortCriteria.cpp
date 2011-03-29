#include "BamSortCriteria.h"
#include <algorithm>
#include <api/internal/BamMultiReader_p.h>
#include <api/SamConstants.h>

const string BamSortCriteria::allowedTags[4]={"QNAME","POS","AS"};
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


bool BamSortCriteria::isTagValid() {
    int length = sizeof(allowedTags)/sizeof(allowedTags[0]);
    for(int i=0;i<length;i++){
        if(sortCriteria == allowedTags[i]){
          return true;
      }
    }
    return false;
}


template <typename T>
IBamMultiMerger* BamSortCriteria::getMergerDesc() {
    if (descending) {
        return new CommonMultiMerger<SortGreaterThanReaderAlignment<T> >;
    }
    return new CommonMultiMerger<T>;
}

IBamMultiMerger* BamSortCriteria::getMerger(void ) {
    if (sortCriteria=="QNAME") {
        return getMergerDesc<SortLessReaderAlignment<SortLessThanName> >();
    } else if (sortCriteria=="POS") {
        return getMergerDesc<SortLessReaderAlignment<SortLessThanPosition> >();
    } else if (sortCriteria=="AS") {
        return getMergerDesc<SortLessReaderAlignment<SortLessThanAlignmentScore> >();
    } else if (sortCriteria == "") {
        return new UnsortedMultiMerger;
    }
    cerr << "BamMultiReader ERROR: requested sort order is unknown" << endl;
    return 0;
}


void BamSortCriteria::sortBuffer(BamAlignmentIterator begin, BamAlignmentIterator end) {
    /**
     * It seems like this step could not be simplified for the
     * ascending and descending case
     */
    if (!descending) {
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
            sort(begin,end,SortGreaterThanBamAlignment<SortLessThanName>(SortLessThanName()));
        } else if (sortCriteria=="POS") {
            sort(begin,end,SortGreaterThanBamAlignment<SortLessThanPosition>(SortLessThanPosition()));
        } else if (sortCriteria=="AS") {
            sort(begin,end,SortGreaterThanBamAlignment<SortLessThanAlignmentScore>(SortLessThanAlignmentScore()));
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

