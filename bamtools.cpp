// ***************************************************************************
// bamtools.cpp (c) 2010 Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Integrates a number of BamTools functionalities into a single executable.
// ***************************************************************************

// Std C/C++ includes
#include <cstdlib>
#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>
using namespace std;

// BamTools includes
#include "BamReader.h"
#include "BamWriter.h"
#include "BamMultiReader.h"
using namespace BamTools;

void usageSummary() {
    cerr << "usage: bamtools <command> [options]" << endl
         << "actions:" << endl
         << "    index <bam file>" << endl
         << "    merge <merged BAM file> [<BAM file> <BAM file> ...]" << endl
         << "    dump [<BAM file> <BAM file> ...]" << endl;
         //<< "    trim <input BAM file> <input BAM index file> <output BAM file> <reference name> <leftBound> <rightBound>" << endl;
}


void BamMerge(string outputFilename, vector<string> filenames) {

    BamMultiReader reader;

    reader.Open(filenames);

    string mergedHeader = reader.GetHeaderText();

    RefVector references = reader.GetReferenceData();

    // open BamWriter
    BamWriter writer;
    writer.Open( outputFilename.c_str(), mergedHeader, references);

    BamAlignment bAlignment;
    while (reader.GetNextAlignment(bAlignment)) {
        // write to output file
        writer.SaveAlignment(bAlignment);
    }

    // close output file
    writer.Close();
    // close input files
    reader.Close();

}

void BamCreateIndex(const char* inputFilename) {

	// open our BAM reader
	BamReader reader;
	reader.Open(inputFilename);

    // create index file
    reader.CreateIndex();

	// close our file
	reader.Close();

}

// Spit out basic BamAlignment data 
void PrintAlignment(const BamAlignment& alignment) {
	cout << "---------------------------------" << endl;
	cout << "Name: "       << alignment.Name << endl;
	cout << "Aligned to: " << alignment.RefID;
	cout << ":"            << alignment.Position << endl;
        cout << endl;
}

void BamDump(vector<string> files) {

	BamMultiReader reader;
	reader.Open(files);
	
	BamAlignment bAlignment;
	while (reader.GetNextAlignment(bAlignment)) {
            PrintAlignment(bAlignment);
	}

	reader.Close();

}


int main(int argc, char* argv[]) {

	// validate argument count
	if( argc < 2 ) {
        usageSummary();
		exit(1);
	}

    string command = argv[1];
    
    if (command == "index") {
        BamCreateIndex(argv[2]);
    } else if (command == "merge") {
        vector<string> files;
        string outputFile = argv[2];
        for (int i = 3; i<argc; ++i) {
            files.push_back(argv[i]);
        }
        BamMerge(outputFile, files);
    } else if (command == "dump") {
        vector<string> files;
        for (int i = 2; i<argc; ++i) {
            files.push_back(argv[i]);
        }
        BamDump(files);
    }


	return 0;
}
