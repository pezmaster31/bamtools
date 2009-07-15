// ***************************************************************************
// BamTrimMain.cpp (c) 2009 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 15 July 2009 (DB)
// ---------------------------------------------------------------------------
// Basic example of reading/writing BAM files. Pulls alignments overlapping 
// the range specified by user from one BAM file and writes to a new BAM file.
// ***************************************************************************

// Std C/C++ includes
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;

// BamTools includes
#include "BamReader.h"
#include "BamWriter.h"
using namespace BamTools;

int main(int argc, char* argv[]) {

	// validate argument count
	if( argc != 7 ) {
		cerr << "USAGE: " << argv[0] << " <input BAM file> <input BAM index file> <output BAM file> <reference name> <leftBound> <rightBound> " << endl;
		exit(1);
	}

	// store arguments
	string inBamFilename  = argv[1];
	string indexFilename  = argv[2];
	string outBamFilename = argv[3];
	string referenceName  = argv[4];
	string leftBound_str  = argv[5];
	string rightBound_str = argv[6];

	// open our BAM reader
	BamReader reader;
	reader.Open(inBamFilename, indexFilename);

	// get header & reference information
	string header = reader.GetHeaderText();
	RefVector references = reader.GetReferenceData();
	
	// open our BAM writer
	BamWriter writer;
	writer.Open(outBamFilename, header, references);

	// get reference ID from name
	int refID = 0;
	RefVector::const_iterator refIter = references.begin();
	RefVector::const_iterator refEnd  = references.end();
	for ( ; refIter != refEnd; ++refIter ) {
		if ( (*refIter).RefName == referenceName ) { break; }
		++refID;
	}
	
	// validate ID
	if ( refIter == refEnd ) {
		cerr << "Reference: " << referenceName << " not found." << endl;
		exit(1);
	}

	// convert boundary arguments to numeric values
	unsigned int leftBound  = (unsigned int) atoi( leftBound_str.c_str()  );
	unsigned int rightBound = (unsigned int) atoi( rightBound_str.c_str() );
	
	// attempt jump to range of interest
	if ( reader.Jump(refID, leftBound) ) {
		
		// while data exists and alignment begin before right bound
		BamAlignment bAlignment;
		while ( reader.GetNextAlignment(bAlignment) && (bAlignment.Position <= rightBound) ) {
			// save alignment to archive
			writer.SaveAlignment(bAlignment);
		}
	} 
	
	// if jump failed
	else {
		cerr << "Could not jump to ref:pos " << referenceName << ":" << leftBound << endl;
		exit(1);
	}

	// clean up and exit
	reader.Close();
	writer.Close();	
	return 0;
}