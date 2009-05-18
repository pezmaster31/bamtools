// BamReaderMain.cpp

// Derek Barnett
// Marth Lab, Boston College
// Last modified: 12 May 2009

#include "BamReader.h"

// C++ includes
#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

#include <vector>
using std::vector;

#include <string>
using std::string;

int main(int argc, char* argv[]) {

	// check command line parameters
	if (argc != 3) {
		cerr << endl;
		cerr << "Usage: BamReaderTest <bamFile> <bamIndexFile>" << endl;
		cerr << endl;
		exit(1);
	}

	// get filenames from command line
	const char* bamFilename = argv[1];
	const char* bamIndexFilename = argv[2];

	int alignmentsRead = 0;
	BamAlignment bAlignment;
	
	BamReader bReader;
	cerr << endl << "Opening BAM file (and index)" << endl << endl;
	bReader.Open(bamFilename, bamIndexFilename);

	string header = bReader.GetHeaderText();
	cerr << "Printing header text..." << endl;
	if ( header.empty() ) {
		cerr << "No header provided." << endl << endl;
	} else {
		cerr << "----------------------" << endl;
		cerr << "Header Text: " << endl;
		cerr << "----------------------" << endl;
		cerr << header << endl << endl;
	}

	RefVector references = bReader.GetReferenceData();
	cerr << "Printing references..." << endl;
	RefVector::const_iterator refIter = references.begin();
	RefVector::const_iterator refEnd  = references.end();
	for ( ; refIter != refEnd; ++refIter) {
		cerr << "Reference entry: " << endl;
		cerr << (*refIter).RefName << endl;
		cerr << (*refIter).RefLength << endl;
		cerr << "Has alignments? : " << ( ((*refIter).RefHasAlignments) ? "yes" : "no" ) << endl;
	}
	cerr << endl;

	alignmentsRead = 0;
	while ( bReader.GetNextAlignment(bAlignment) && (alignmentsRead <= 10) ) {
		cerr << "Alignment " << alignmentsRead << endl;
		cerr << bAlignment.Name << endl;
		cerr << "Aligned to ref " << bAlignment.RefID << " at position " << bAlignment.Position << endl;
		++alignmentsRead;
	}

	cerr << "Jumping in BAM file" << endl;
	if ( bReader.Jump(1, 5000) ) {
		cerr << "Jumping seems ok - getting first 10 alignments that start at or after chr2:5000" << endl;

		alignmentsRead = 0;
		while ( bReader.GetNextAlignment(bAlignment) && (alignmentsRead <= 10) ) {
			if ( bAlignment.Position >= 5000 ) { 
				cerr << "Alignment " << alignmentsRead << endl;
				cerr << bAlignment.Name << endl;
				cerr << "Aligned to ref " << bAlignment.RefID << " at position " << bAlignment.Position << endl;
				++alignmentsRead;
			}
		}
	}

	cerr << "Rewinding BAM file" << endl;
	if ( bReader.Rewind() ) {
		cerr << "Rewind seems to be ok - getting first 10 alignments" << endl;
		
		alignmentsRead = 0;
		while ( bReader.GetNextAlignment(bAlignment) && (alignmentsRead <= 10) ) {
			cerr << "Alignment " << alignmentsRead << endl;
			cerr << bAlignment.Name << endl;
			cerr << "Aligned to ref " << bAlignment.RefID << " at position " << bAlignment.Position << endl;
			++alignmentsRead;
		}
	}

	cerr << "Closing BAM file..." << endl << endl;
	bReader.Close();

	cerr << "Exiting..." << endl << endl;
	return 0;
}
