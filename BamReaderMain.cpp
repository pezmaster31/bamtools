// BamReaderMain.cpp

// Derek Barnett
// Marth Lab, Boston College
// Last modified: 6 April 2009

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

	string refName;
	int refID;

	int alignmentCount;

	vector<string> refNames;

	BamAlignment bAlignment;
	BamAlignmentVector alignments;

	RefVector references;

	// ------------------------------------------------------------------------------------------------------ //
	// Declare BamReader & open BAM file - automatically loads up header and index file (.bai) information
	// ------------------------------------------------------------------------------------------------------ //
	
	BamReader bReader(bamFilename, bamIndexFilename);

	cerr << endl;
	cerr << "Opening BamReader for BAM file: " << bamFilename << " ..... ";
	
	if ( bReader.Open() ) { 
		cerr << "ok" << endl; 
	}
	else {
		cerr << "error" << endl;
		exit(1);
	}
	
	// ------------------------------------------------------------ //
	// Find out how many reference sequences are in BAM file
	// ------------------------------------------------------------ //
	
	references = bReader.GetReferenceData();

	cerr << endl;
	cerr << "Total number of ref seqs: " << references.size() << endl;
	
	// ---------------------------------------------------------------------------- //
	// Get the names/lengths of all the reference sequences that have alignments
	// ---------------------------------------------------------------------------- //
	
	cerr << endl;
	cerr << "All ref sequences with alignments:" << endl;
	cerr << endl;


	if ( !references.empty() ) {
		
		RefVector::iterator refIter = references.begin();
		RefVector::iterator refEnd  = references.end();
		
		refID = 0;
		// iterate over reference names, print to STDERR
		for( ; refIter != refEnd; ++refIter) {

			if ( (*refIter).RefHasAlignments ) {
				cerr << "ID:     " << refID << endl;
				cerr << "Name:   " << (*refIter).RefName << endl;
				cerr << "Length: " << (*refIter).RefLength << endl;
				cerr << endl;
			}

			++refID;
		}
	}

	// --------------------------------------------------------------------------------------------- //
	// Get the SAM-style header text, if available (not available in the example file shown here)
	// --------------------------------------------------------------------------------------------- //	
	
	cerr << "------------------------------------" << endl;
	cerr << "SAM header text: " << endl;
	
	// get (SAM) header text
	string header = bReader.GetHeaderText();
	
	cerr << ( (header.empty()) ? "no header data" : header ) << endl;
	cerr << "------------------------------------" << endl;

	// --------------------------------------------------------------------------------------------- //
	// Here we start accessing alignments
	// The first method shows how to iterate over all reads in a BamFile
	// This method will work on any BAM file (sorted/non-sorted, with/without an index)
	// --------------------------------------------------------------------------------------------- //

	// Call Rewind() to make sure you're at the 1st alignment in the file
	// Please note, however, it's not necessary in this case, since file pointer initially set to 1st alignment
	// but this is probably a good habit to develop to ensure correctness and make your intent obvious in the code

	cerr << "Getting (up to) first 1000 alignments" << endl;

	// start at 1st alignment
	if ( bReader.Rewind() ) {
		
		alignmentCount = 0;
		while ( bReader.GetNextAlignment(bAlignment) && (alignmentCount < 1000) ) {
		
			// disregard unmapped alignments
			if ( bAlignment.IsMapped() ) {

				++alignmentCount;
				
				cout << "----------------------------" << endl;
				cout << "Alignment " << alignmentCount << endl;
				cout << bAlignment.Name << endl;
				cout << bAlignment.AlignedBases << endl;
				cout << "Aligned to " << references.at(bAlignment.RefID).RefName << ":" << bAlignment.Position << endl;
				
				cout << "Cigar Data: " << endl;

				vector<CigarOp>::const_iterator cigarIter = bAlignment.CigarData.begin();
				vector<CigarOp>::const_iterator cigarEnd  = bAlignment.CigarData.end();
				for ( ; cigarIter != cigarEnd; ++cigarIter ) {
					cout << "Type: " << (*cigarIter).Type << "\tLength: " << (*cigarIter).Length << endl;
				}

				if(!bAlignment.TagData.empty()) {
				  cout << "Tag data is present." << endl;
				  string readGroup;
				  if(bAlignment.GetReadGroup(readGroup)) {
				    cout << "- read group: " << readGroup << endl;
				  }
				}
			}
		}

		cerr << "Found " << alignmentCount << " alignments." << endl;
	} else { cerr << "Could not rewind" << endl; }

	// ---------------------------------------------------------------------------------------------------------- //
	// You can iterate over each individual alignment that overlaps a specified region
	// Set the refID & left boundary parameters using Jump(refID, left)
	// Jump() actually positions the file pointer actually before the left boundary
	// This ensures that reads beginning before, but overlapping 'left' are included as well 
	// Client is responsible for setting/checking right boundary - see below
	// ---------------------------------------------------------------------------------------------------------- //
/*
	cerr << "Jumping to region" << endl;

	// get refID using a known refName
	refName = "seq1";
	refID = bReader.GetRefID(refName);
	if (refID == (int)references.size()) { 
		cerr << "Reference " << refName << " not found." << endl;
		exit(1);
	}

	// set left boundary
	unsigned int leftBound = 500;

	// set right boundary - either user-specified number to set a discrete region
	//                      OR you can query the BamReader for the end of the reference
	unsigned int rightBound = references.at(refID).RefLength;

	cerr << endl;
	cerr << "Iterating over alignments on reference: " << refName << " from " << leftBound << " to ref end (" << rightBound << ")" << endl;

	// set region - specific region on reference
	if ( bReader.Jump(refID, leftBound) ) { 

		alignmentCount = 0;
		while ( bReader.GetNextAlignment(bAlignment) && (bAlignment.Position <= rightBound) ) {
		
			if ( bAlignment.IsMapped() ) {

				++alignmentCount;
			
				if ( (alignmentCount % 100000) == 0) { cerr << "Retrieved " << alignmentCount << " so far..." << endl; }
			
				cout << "----------------------------" << endl;
				cout << "Alignment " << alignmentCount << endl;
				cout << bAlignment.Name << endl;
				cout << bAlignment.AlignedBases << endl;
				cout << "Aligned to " << references.at(bAlignment.RefID).RefName << ":" << bAlignment.Position << endl;
			}
		}

		cerr << "Found " << alignmentCount << " alignments." << endl; 
	} else { cerr << "Could not jump to region specified" << endl; }

	// ----------------------------------------------------------------------------------------------------- //
	// You can 'rewind' back to beginning of BAM file at any time 
	// ----------------------------------------------------------------------------------------------------- //

	cerr << endl;
	cerr << "Rewinding BAM file... then getting first 1000 alignments" << endl;

	alignmentCount = 0;
	if (bReader.Rewind() ) {
		while ( bReader.GetNextAlignment(bAlignment) && (alignmentCount < 1000) ) {
		
			// disregard unmapped alignments
			if ( bAlignment.IsMapped() ) {

				++alignmentCount;
			
				cout << "----------------------------" << endl;
				cout << "Alignment " << alignmentCount << endl;
				cout << bAlignment.Name << endl;
				cout << bAlignment.AlignedBases << endl;
				cout << "Aligned to " << references.at(bAlignment.RefID).RefName << ":" << bAlignment.Position << endl;
			}
		}

		cerr << "Found " << alignmentCount << " alignments." << endl;
	} else { cerr << "Could not rewind" << endl; }
*/
	// ---------------------------------------------------------------------- //
	// Close BamReader object (releases internal header/index data) and exit
	// ---------------------------------------------------------------------- //
	
	cerr << endl;
	cerr << "Closing BAM file: " << bamFilename << endl;
	
	bReader.Close();

	cerr << "Exiting..." << endl << endl;
	return 0;
}
