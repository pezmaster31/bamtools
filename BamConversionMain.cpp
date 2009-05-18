#include <iostream>
#include "BamReader.h"
#include "BamWriter.h"

using namespace std;

int main(int argc, char* argv[]) {

	if(argc != 3) {
		cout << "USAGE: " << argv[0] << " <input BAM file> <output BAM file>" << endl;
		exit(1);
	}

	// localize our arguments
	const char* inputFilename  = argv[1];
	const char* outputFilename = argv[2];

	// open our BAM reader
	BamReader reader;
	reader.Open(inputFilename);

	// retrieve the SAM header text
	string samHeader = reader.GetHeaderText();

	// retrieve the reference sequence vector
	RefVector referenceSequences = reader.GetReferenceData();

	// open the BAM writer
	BamWriter writer;
	writer.Open(outputFilename, samHeader, referenceSequences);

	// copy all of the reads from the input file to the output file
	BamAlignment al;
	while(reader.GetNextAlignment(al)) writer.SaveAlignment(al);

	// close our files
	reader.Close();
	writer.Close();

	return 0;
}
