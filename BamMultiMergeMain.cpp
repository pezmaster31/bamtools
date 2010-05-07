#include "BamMultiReader.h"
#include "BamWriter.h"
#include <boost/algorithm/string.hpp>
#include <iostream>

using namespace BamTools;
using namespace std;

int main(int argc, char** argv) {

    if (argc == 1) {
        cerr << "USAGE: ./BamMultiMerge <output file> [input files]" << endl;
        exit(0);
    }

    string outputFilename = argv[1];

    BamMultiReader reader;
    vector<string> filenames;
    for (int i = 2; i<argc; ++i) {
        filenames.push_back(argv[i]);
    }

    reader.Open(filenames);

    //cerr << "merging to " << outputFilename << endl;
    string mergedHeader = reader.GetUnifiedHeaderText();
    //cerr << "mergedHeader = " << endl << mergedHeader << endl;

    // check that we are merging files which have the same sets of references
    RefVector references;
    int referencesSize = 0; bool first = true;
    for (int i = 2; i<argc; ++i) {
        BamReader areader;
        areader.Open( argv[i] );
        if (first) {
            references = areader.GetReferenceData();
            referencesSize = references.size();
            first = false;
        } else {
            RefVector newreferences = areader.GetReferenceData();
            int i = 0;
            for (RefVector::const_iterator it = references.begin(); it != references.end(); it++) {
                if (newreferences.at(i++).RefName != it->RefName) {
                    cerr << "BAM FILES ALIGNED AGAINST DIFFERING SETS OF REFERENCES, NOT MERGING" << endl;
                    exit(1);
                }
            }
        }
    }

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

    //cerr << "done" << endl;

    return 0;
}
