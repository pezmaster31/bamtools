// ***************************************************************************
// BamDump.cpp (c) 2009 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 15 July 2009 (DB)
// ---------------------------------------------------------------------------
// Spits out all alignments in BAM file.
//
// N.B. - Could result in HUGE text file. This is mostly a debugging tool
// for small files.  You have been warned.
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

void PrintAlignment(const BamAlignment&);

int main(int argc, char* argv[]) {

	// validate argument count
	if( argc != 2 ) {
		cerr << "USAGE: " << argv[0] << " <input BAM file> " << endl;
		exit(1);
	}

	string filename = argv[1];
	cout << "Printing alignments from file: " << filename << endl;
	
	BamReader reader;
	reader.Open(filename);
	
	BamAlignment bAlignment;
	while (reader.GetNextAlignment(bAlignment)) {
            PrintAlignment(bAlignment);
	}

	reader.Close();
	return 0;
}
	
// Spit out basic BamAlignment data 
void PrintAlignment(const BamAlignment& alignment) {
	cout << "---------------------------------" << endl;
	cout << "Name: "       << alignment.Name << endl;
	cout << "Aligned to: " << alignment.RefID;
	cout << ":"            << alignment.Position << endl;
        cout << "TagData:    " << alignment.TagData << endl;
        
        string readGroup;
        cout << "ReadGroup:  " << ( alignment.GetReadGroup(readGroup) ? readGroup : "Unknown" ) << endl;
        cout << endl;
}
