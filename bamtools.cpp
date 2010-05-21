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
#include <fstream>
#include <string>
using namespace std;

// BamTools includes
#include "BamReader.h"
#include "BamWriter.h"
#include "BamMultiReader.h"
using namespace BamTools;

void usageSummary(string cmdname) {
    cerr << "usage: " << cmdname << " <command> [options]" << endl
         << "actions:" << endl
         << "    index <BAM file>   # generates BAM index <BAM file>.bai" << endl
         << "    merge <merged BAM file> [<BAM file> <BAM file> ...]   # merges BAM files into a single file" << endl
         << "    dump [<BAM file> <BAM file> ...]   # dumps alignment summaries to stdout" << endl
         << "    header [<BAM file> <BAM file> ...]   # prints header, or unified header for BAM file or files" << endl;
         //<< "    trim <input BAM file> <input BAM index file> <output BAM file> <reference name> <leftBound> <rightBound>" << endl;
}


void BamMerge(string outputFilename, vector<string> filenames) {

    BamMultiReader reader;

    reader.Open(filenames, false); // opens the files without checking for indexes

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

void BamDumpHeader(vector<string> files) {

	BamMultiReader reader;
	reader.Open(files, false);
	
    cout << reader.GetHeaderText() << endl;

	reader.Close();

}


int main(int argc, char* argv[]) {

	// validate argument count
	if( argc < 3 ) {
        usageSummary(argv[0]);
		exit(1);
	}

    string command = argv[1];
    
    if (command == "index") {

        BamCreateIndex(argv[2]);

    } else if (command == "merge") {

        vector<string> files;
        string outputFile = argv[2];

        // check if our output exists, and exit if so
        ifstream output(outputFile.c_str());
        if (output.good()) {
            cerr << "ERROR: output file " << outputFile << " exists, exiting." << endl;
            exit(1);
        } else {
            output.close();
        }

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

    } else if (command == "header") {

        vector<string> files;
        for (int i = 2; i<argc; ++i) {
            files.push_back(argv[i]);
        }
        BamDumpHeader(files);

    }


	return 0;
}
