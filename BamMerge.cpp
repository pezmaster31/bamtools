#include <iostream>
#include <map>
#include "BamReader.h"
#include "BamWriter.h"

using namespace std;

int main(int argc, char* argv[]) {

	for (int a=0; a<argc; a++) {
		cout <<  argv[a] << " ";
  }
  cout << endl;
  
  if(argc != 4) {
		cout << "USAGE: " << argv[0] << " <input BAM file 1> <input BAM file 1> <output BAM file>" << endl;
		exit(1);
	}

	// localize our arguments
	const char* inputFilename1  = argv[1];
	const char* inputFilename2  = argv[2];
	const char* outputFilename = argv[3];

	// open our BAM reader1 for input file 1
	BamReader reader1;
	reader1.SetFilename(inputFilename1);

	// open our BAM reader2 for input file 2
	BamReader reader2;
	reader2.SetFilename(inputFilename2);

	// check reader1
	if(!reader1.Open()) {
		cout << "ERROR: Unable to open the BAM file 1 (" << inputFilename1 << ")." << endl;
		exit(1);
	}

	// check reader2
	if(!reader2.Open()) {
		cout << "ERROR: Unable to open the BAM file 1 (" << inputFilename2 << ")." << endl;
		exit(1);
	}
	
	// retrieve the header text from both files
	string samHeader1 = reader1.GetHeaderText();
	string samHeader2 = reader2.GetHeaderText();

	// retrieve the reference sequence vectors
	RefVector referenceSequences1 = reader1.GetReferenceData();
	RefVector referenceSequences2 = reader2.GetReferenceData();

	// process reference sequences from file 1
	map<string, unsigned int, less<string> > refLengthMap;
	map<string, unsigned int, less<string> > refIdMap;
	map<string, RefData, less<string> > refDataMap;
 
	cerr << "Reference list from file 1" << endl;
	unsigned int refCounter = 0;
	for (RefVector::const_iterator refIter = referenceSequences1.begin();
	     refIter != referenceSequences1.end(); refIter++) {

	  // retrieve
	  RefData rd = *refIter;

	  // get member data
	  string refName = rd.RefName;
	  unsigned int refLength = rd.RefLength;
	  int refId = reader1.GetRefID(refName);

	  // report
	  cerr << "  refName=" << refName << " refId=" << refId << endl;

	  // store in maps
	  refLengthMap[refName] = refLength;
	  refIdMap[refName] = refCounter;
	  refDataMap[refName] = rd;
	
	  // increment ref count
	  refCounter++;
	}

	// process reference sequences from file 1
	map<unsigned int, unsigned int, less<unsigned int> > recodedRefId;

	cerr << "Reference list from file 2" << endl;
	for (RefVector::const_iterator refIter = referenceSequences2.begin();
	     refIter != referenceSequences2.end(); refIter++) {

	  // retrieve
	  RefData rd = *refIter;

	  // get member data
	  string refName = rd.RefName;
	  unsigned int refLength = rd.RefLength;
	  int refId = reader2.GetRefID(refName);

	  // report
	  cerr << "  refName=" << refName << " refId=" << refId << endl;

	  // if refName already in map, check ref length
	  if (refLengthMap.count(refName) > 0) {
	    
	    // check if length is the same
	    if (refLengthMap[refName] != refLength) {
	      cerr << "Refernce name in two files with inconsistent lengths: " << refName << " ... exiting." << endl;
	      exit(1);
	    }

	    // make ref id recoding entry
	    unsigned int refIdNew = refIdMap[refName];
	    recodedRefId[refId] = refIdNew;
	  }
	  // otherwise make new refId and RefData
	  else {
	    // store in maps
	    refLengthMap[refName] = refLength;
	    refIdMap[refName] = refCounter;
	    refDataMap[refName] = rd;
	
	    // make ref id recoding entry
	    recodedRefId[refId] = refCounter;

	    // increment ref count
	    refCounter++;
	  }
	}

	// make new referenceSequences vector
	RefVector referenceSequences3;
	unsigned rc = 0;
	for (map<string, RefData, less<string> >::const_iterator rdIter = refDataMap.begin();
	     rdIter != refDataMap.end(); rdIter++) {
	  RefData rd = rdIter->second;
	  referenceSequences3.push_back(rd);
	  cerr << "Adding RefData for refName=" << rd.RefName << " refId=" << rc << endl;
	  rc++;
	}
	cerr << "Length of referenceSequences vector=" << referenceSequences3.size() << endl;
	

	// open output bam file and write:
	//   empty header
	//   new referenceSequences3 vector
	string samHeader3 = "";

	BamWriter writer;
	writer.Open(outputFilename, samHeader3, referenceSequences3);

	// iterate through alignments from file 1 and write them unchanged
	BamAlignment ba;
	
	unsigned int ac1 = 0;
	while(reader1.GetNextAlignment(ba)) {
	  writer.SaveAlignment(ba);
	  ac1++;
	}
	
	cerr << "Write " << ac1 << " " << inputFilename1 << " records to " << outputFilename << endl;

	// close input file 1
	reader1.Close();

	// iterate through alignments from file 2 and:
	//   assign re-coded refId
	//   write alignment
	int ac2 = 0;
	while(reader2.GetNextAlignment(ba) ) {
	  ac2++;

	  // retrieve original refId
	  unsigned int refId = ba.RefID;
	  
	  // check if original refId is in recoding table... bomb if not
	  if (recodedRefId.count(refId) <= 0) {
	    cerr << "Alignment in file 2 has inconsistent reference sequence ID... exiting." << endl;
	    exit(1);
	  }

	  // assign new refId
	  ba.RefID = recodedRefId[refId];

	  //	  cerr << "  recoding refId=" << refId << " to refId=" << recodedRefId[refId] << endl;

	  writer.SaveAlignment(ba);
	}

	cerr << "Write " << ac2 << " " << inputFilename2 << " records to " << outputFilename << endl;
	
	// close input file 2
	reader2.Close();

	// close output file
	writer.Close();

	cerr << "Total " << ac1+ac2 <<  " records to " << outputFilename << endl;

	// return
	return 0;
}
