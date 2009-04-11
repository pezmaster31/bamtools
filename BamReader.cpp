// BamReader.cpp

// Derek Barnett
// Marth Lab, Boston College
// Last modified: 6 April 2009

#include "BamReader.h"
#include <iostream>
using std::cerr;
using std::endl;

// static character constants
const char* BamReader::DNA_LOOKUP   = "=ACMGRSVTWYHKDBN";
const char* BamReader::CIGAR_LOOKUP = "MIDNSHP";

BamReader::BamReader(const char* filename, const char* indexFilename) 
	: m_filename( (char*)filename )
	, m_indexFilename( (char*)indexFilename )
	, m_file(0)
	, m_index(NULL)
	, m_headerText("")
	, m_isOpen(false)
	, m_isIndexLoaded(false)
	, m_isRegionSpecified(false)
	, m_isCalculateAlignedBases(true)
	, m_currentRefID(0)
	, m_currentLeft(0)
	, m_alignmentsBeginOffset(0)
{
	Open();
}

BamReader::~BamReader(void) {

	// close file
	if ( m_isOpen ) { Close(); }
}

// open BAM file
bool BamReader::Open(void) {

	if (!m_isOpen && m_filename != NULL ) {

		// open file
		m_file = bam_open(m_filename, "r"); 
		
		// get header info && index info
		if ( (m_file != NULL) && LoadHeader() ) {

			// save file offset where alignments start
			m_alignmentsBeginOffset = bam_tell(m_file);
			
			// set open flag
			m_isOpen = true;
		}

		// try to open (and load) index data, if index file given
		if ( m_indexFilename != NULL ) {
			OpenIndex();
		}
	}

	return m_isOpen;
}

bool BamReader::OpenIndex(void) {

	if ( m_indexFilename && !m_isIndexLoaded ) {
		m_isIndexLoaded = LoadIndex();
	}
	return m_isIndexLoaded;
}

// close BAM file
bool BamReader::Close(void) {
	
	if (m_isOpen) {

		// close file
		int ret = bam_close(m_file);
		
		// delete index info
		if ( m_index != NULL) { delete m_index; }

		// clear open flag
		m_isOpen = false;

		// clear index flag
		m_isIndexLoaded = false;

		// clear region flag
		m_isRegionSpecified = false;

		// return success/fail of bam_close
		return (ret == 0);
	} 

	return true;
}

// get BAM filename
const char* BamReader::Filename(void) const { 
	return (const char*)m_filename; 
}

// set BAM filename
void BamReader::SetFilename(const char* filename) {
	m_filename = (char*)filename;
}

// get BAM Index filename
const char* BamReader::IndexFilename(void) const { 
	return (const char*)m_indexFilename; 
}

// set BAM Index filename
void BamReader::SetIndexFilename(const char* indexFilename) {
	m_indexFilename = (char*)indexFilename;
}

// return full header text
const string BamReader::GetHeaderText(void) const { 
	return m_headerText; 
}

// return number of reference sequences in BAM file
const int BamReader::GetReferenceCount(void) const { 
	return m_references.size();
}

// get RefID from reference name
const int BamReader::GetRefID(string refName) const { 
	
	vector<string> refNames;
	RefVector::const_iterator refIter = m_references.begin();
    RefVector::const_iterator refEnd  = m_references.end();
    for ( ; refIter != refEnd; ++refIter) {
		refNames.push_back( (*refIter).RefName );
    }

	// return 'index-of' refName (if not found, returns refNames.size())
	return Index( refNames.begin(), refNames.end(), refName );
}

const RefVector BamReader::GetReferenceData(void) const {
	return m_references;
}

bool BamReader::Jump(int refID, unsigned int left) {

	// if index available, and region is valid
	if ( m_isIndexLoaded && m_references.at(refID).RefHasAlignments && (left <= m_references.at(refID).RefLength) ) { 
		m_currentRefID = refID;
		m_currentLeft  = left;
		m_isRegionSpecified = true;
		return ( bam_seek(m_file, GetOffset(m_currentRefID, m_currentLeft), SEEK_SET) == 0 );
	}
	return false;
}

bool BamReader::Rewind(void) {

	int refID = 0;
	int refCount = m_references.size();
	for ( ; refID < refCount; ++refID ) {
		if ( m_references.at(refID).RefHasAlignments ) { break; } 
	}

	m_currentRefID = refID;
	m_currentLeft = 0;
	m_isRegionSpecified = false;

	return ( bam_seek(m_file, m_alignmentsBeginOffset, SEEK_SET) == 0 );
}	

// get next alignment from specified region
bool BamReader::GetNextAlignment(BamAlignment& bAlignment) {

	// try to load 'next' read
	if ( LoadNextAlignment(bAlignment) ) {

		// if specified region, check for region overlap
		if ( m_isRegionSpecified ) {

			// if overlap, return true
			if ( IsOverlap(bAlignment) ) { return true; }
			// if not, try the next alignment
			else { return GetNextAlignment(bAlignment); }
		} 

		// not using region, valid read detected, return success
		else { return true; }
	}

	// no valid alignment to load
	return false;
}

void BamReader::SetCalculateAlignedBases(bool flag) {
	m_isCalculateAlignedBases = flag;
}

int BamReader::BinsFromRegion(int refID, unsigned int left, uint16_t list[MAX_BIN]) {

	// get region boundaries
	uint32_t begin = left;
	uint32_t end   = m_references.at(refID).RefLength - 1;

	// initialize list, bin '0' always a valid bin
	int i = 0;
	list[i++] = 0;

	// get rest of bins that contain this region
	unsigned int k;
	for (k =    1 + (begin>>26); k <=    1 + (end>>26); ++k) { list[i++] = k; }
	for (k =    9 + (begin>>23); k <=    9 + (end>>23); ++k) { list[i++] = k; }
	for (k =   73 + (begin>>20); k <=   73 + (end>>20); ++k) { list[i++] = k; }
	for (k =  585 + (begin>>17); k <=  585 + (end>>17); ++k) { list[i++] = k; }
	for (k = 4681 + (begin>>14); k <= 4681 + (end>>14); ++k) { list[i++] = k; }
	
	// return number of bins stored
	return i;
}

uint32_t BamReader::CalculateAlignmentEnd(const unsigned int& position, const vector<CigarOp>& cigarData) {

	// initialize alignment end to starting position
	uint32_t alignEnd = position;

	// iterate over cigar operations
	vector<CigarOp>::const_iterator cigarIter = cigarData.begin();
	vector<CigarOp>::const_iterator cigarEnd  = cigarData.end();
	for ( ; cigarIter != cigarEnd; ++cigarIter) {
		if ( (*cigarIter).Type == 'M' || (*cigarIter).Type == 'D' || (*cigarIter).Type == 'N') {
			alignEnd += (*cigarIter).Length;
		}
	}
	return alignEnd;
}

uint64_t BamReader::GetOffset(int refID, unsigned int left) {

	//  make space for bins
	uint16_t* bins = (uint16_t*)calloc(MAX_BIN, 2); 		
	
	// returns number of bins overlapping (left, right)
	// stores indices of those bins in 'bins'
	int numBins = BinsFromRegion(refID, left, bins);				

	// access index data for refID
	RefIndex* refIndex = m_index->at(refID);

	// get list of BAM bins for this reference sequence
	BinVector* refBins = refIndex->first;

	sort( refBins->begin(), refBins->end(), LookupKeyCompare<uint32_t, ChunkVector*>() );

	// declare ChunkVector
	ChunkVector regionChunks;

	// declaure LinearOffsetVector
	LinearOffsetVector* linearOffsets = refIndex->second;

	// some sort of linear offset vs bin index voodoo... not completely sure what's going here
	uint64_t minOffset = ((left>>BAM_LIDX_SHIFT) >= linearOffsets->size()) ? 0 : linearOffsets->at(left>>BAM_LIDX_SHIFT);

	BinVector::iterator binBegin = refBins->begin();
	BinVector::iterator binEnd   = refBins->end();

	// iterate over bins overlapping region, count chunks
	for (int i = 0; i < numBins; ++i) {
		
		// look for bin with ID=bin[i]
		BinVector::iterator binIter = binBegin;

		for ( ; binIter != binEnd; ++binIter ) {
		
			// if found, increment n_off by number of chunks for each bin
			if ( (*binIter).first == (uint32_t)bins[i] ) { 
				
				// iterate over chunks in that bin
				ChunkVector* binChunks = (*binIter).second;
				
				ChunkVector::iterator chunkIter = binChunks->begin();
				ChunkVector::iterator chunkEnd  = binChunks->end();
				for ( ; chunkIter != chunkEnd; ++chunkIter) {
				
					// if right bound of pair is greater than min_off (linear offset value), store pair
					if ( (*chunkIter).second > minOffset) { 
						regionChunks.push_back( (*chunkIter) ); 
					}
				}
			}
		}
	}

	// clean up temp array
	free(bins);

	// there should be at least 1
	assert(regionChunks.size() > 0);

	// sort chunks by start position
	sort ( regionChunks.begin(), regionChunks.end(), LookupKeyCompare<uint64_t, uint64_t>() );

	// resolve overlaps between adjacent blocks; this may happen due to the merge in indexing
	int numOffsets = regionChunks.size();	
	for (int i = 1; i < numOffsets; ++i) {
		if ( regionChunks.at(i-1).second >= regionChunks.at(i).first ) {
			regionChunks.at(i-1).second = regionChunks.at(i).first;
		}
	}
	
	// merge adjacent chunks
	int l = 0;
	for (int i = 1; i < numOffsets; ++i) {
		// if adjacent, expand boundaries of (merged) chunk
		if ( (regionChunks.at(l).second>>16) == (regionChunks.at(i).first>>16) ) {
			regionChunks.at(l).second = regionChunks.at(i).second;
		}
		// else, move on to next (merged) chunk index
		else { regionChunks.at(++l) = regionChunks.at(i); }
	}

	// return beginning file offset of first chunk for region
	return regionChunks.at(0).first;
}

bool BamReader::IsOverlap(BamAlignment& bAlignment) {

	// if on different reference sequence, quit
	if ( bAlignment.RefID != (unsigned int)m_currentRefID ) { return false; }

	// read starts after left boundary
	if ( bAlignment.Position >= m_currentLeft) { return true; }

	// get alignment end
	uint32_t alignEnd = CalculateAlignmentEnd(bAlignment.Position, bAlignment.CigarData);

	// return whether alignment end overlaps left boundary
	return ( alignEnd >= m_currentLeft );
}

bool BamReader::LoadHeader(void) {

	// check to see if proper BAM header
	char buf[4];
	if (bam_read(m_file, buf, 4) != 4) { return false; }
	if (strncmp(buf, "BAM\001", 4)) {
		fprintf(stderr, "wrong header type!\n");
		return false;
	}
	
	// get BAM header text length
	int32_t headerTextLength;
	bam_read(m_file, &headerTextLength, 4);

	// get BAM header text
	char* headerText = (char*)calloc(headerTextLength + 1, 1);
	bam_read(m_file, headerText, headerTextLength);
	m_headerText = (string)((const char*)headerText);
	
	// clean up calloc-ed temp variable
	free(headerText);

	// get number of reference sequences
	int32_t numberRefSeqs;
	bam_read(m_file, &numberRefSeqs, 4);
	if (numberRefSeqs == 0) { return false; }

	m_references.reserve((int)numberRefSeqs);
	
	// reference variables
	int32_t  refNameLength;
	char*    refName;
	uint32_t refLength;

	// iterate over all references in header
	for (int i = 0; i != numberRefSeqs; ++i) {

		// get length of reference name
		bam_read(m_file, &refNameLength, 4);
		refName = (char*)calloc(refNameLength, 1);

		// get reference name and reference sequence length
		bam_read(m_file, refName, refNameLength);
		bam_read(m_file, &refLength, 4);

		// store data for reference
		RefData aReference;
		aReference.RefName   = (string)((const char*)refName);
		aReference.RefLength = refLength;
		m_references.push_back(aReference);

		// clean up calloc-ed temp variable
		free(refName);
	}
	
	return true;
}

bool BamReader::LoadIndex(void) {

	// check to see if index file exists
	FILE* indexFile;
	if ( ( indexFile = fopen(m_indexFilename, "r") ) == 0 ) {
		fprintf(stderr, "The alignment is not indexed. Please run SAMtools \'index\' command first.\n");
		return false;
	}

	// see if index is valid BAM index
	char magic[4];
	fread(magic, 1, 4, indexFile);
	if (strncmp(magic, "BAI\1", 4)) {
		fprintf(stderr, "Problem with index - wrong \'magic\' number.\n");
		fclose(indexFile);
		return false;
	}

	// get number of reference sequences
	uint32_t numRefSeqs;
	fread(&numRefSeqs, 4, 1, indexFile);
	
	// intialize BamIndex data structure
	m_index = new BamIndex;
	m_index->reserve(numRefSeqs);

	// iterate over reference sequences
	for (unsigned int i = 0; i < numRefSeqs; ++i) {
		
		// get number of bins for this reference sequence
		int32_t numBins;
		fread(&numBins, 4, 1, indexFile);
		
		if (numBins > 0) { m_references.at(i).RefHasAlignments = true; }

		// intialize BinVector
		BinVector* bins = new BinVector;
		bins->reserve(numBins);
		
		// iterate over bins for that reference sequence
		for (int j = 0; j < numBins; ++j) {
			
			// get binID 
			uint32_t binID;
			fread(&binID, 4, 1, indexFile);
			
			// get number of regionChunks in this bin
			uint32_t numChunks;
			fread(&numChunks, 4, 1, indexFile);
			
			// intialize ChunkVector
			ChunkVector* regionChunks = new ChunkVector;
			regionChunks->reserve(numChunks);
			
			// iterate over regionChunks in this bin
			for (unsigned int k = 0; k < numChunks; ++k) {
				
				// get chunk boundaries (left, right) 
				uint64_t left;
				uint64_t right;
				fread(&left, 8, 1, indexFile);
				fread(&right, 8, 1, indexFile);
				
				// save ChunkPair
				regionChunks->push_back( ChunkPair(left, right) );
			}
			
			// save binID, chunkVector for this bin
			bins->push_back( BamBin(binID, regionChunks) );
		}
		
		// load linear index for this reference sequence
		
		// get number of linear offsets
		int32_t numLinearOffsets;
		fread(&numLinearOffsets, 4, 1, indexFile);
		
		// intialize LinearOffsetVector
		LinearOffsetVector* linearOffsets = new LinearOffsetVector;
		linearOffsets->reserve(numLinearOffsets);
		
		// iterate over linear offsets for this reference sequeence
		for (int j = 0; j < numLinearOffsets; ++j) {
			// get a linear offset
			uint64_t linearOffset;
			fread(&linearOffset, 8, 1, indexFile);
			// store linear offset
			linearOffsets->push_back(linearOffset);
		}
		
		// store index data for that reference sequence
		m_index->push_back( new RefIndex(bins, linearOffsets) );
	}
	
	// close index file (.bai) and return
	fclose(indexFile);
	return true;
}

bool BamReader::LoadNextAlignment(BamAlignment& bAlignment) {

	// check valid alignment block header data
	int32_t block_len;
	int32_t ret;
	uint32_t x[8];

	int32_t bytesRead = 0;

	// read in the 'block length' value, make sure it's not zero
	if ( (ret = bam_read(m_file, &block_len, 4)) == 0 )        { return false; }
	bytesRead += 4;

	// read in core alignment data, make the right size of data was read 
	if ( bam_read(m_file, x, BAM_CORE_SIZE) != BAM_CORE_SIZE ) { return false; }
	bytesRead += BAM_CORE_SIZE;

	// set BamAlignment 'core' data
	bAlignment.RefID         = x[0]; 
	bAlignment.Position      = x[1];
	bAlignment.Bin           = x[2]>>16; 
	bAlignment.MapQuality    = x[2]>>8&0xff; 
	bAlignment.AlignmentFlag = x[3]>>16; 
	bAlignment.MateRefID     = x[5]; 
	bAlignment.MatePosition  = x[6]; 
	bAlignment.InsertSize    = x[7];

	// fetch & store often-used lengths for character data parsing
	unsigned int queryNameLength     = x[2]&0xff;
	unsigned int numCigarOperations  = x[3]&0xffff;
	unsigned int querySequenceLength = x[4];
	
	// get length of character data
	int dataLength = block_len - BAM_CORE_SIZE;

	// set up destination buffers for character data
	uint8_t*  allCharData = (uint8_t*)calloc(sizeof(uint8_t), dataLength);
	uint32_t* cigarData   = (uint32_t*)(allCharData+queryNameLength);

	const unsigned int tagDataOffset = (numCigarOperations * 4) + queryNameLength + (querySequenceLength + 1) / 2 + querySequenceLength;
	const unsigned int tagDataLen = dataLength - tagDataOffset;
	char* tagData = ((char*)allCharData) + tagDataOffset;
	
	// get character data - make sure proper data size was read
	if (bam_read(m_file, allCharData, dataLength) != dataLength) { return false; }
	else {

		bytesRead += dataLength;

		// clear out bases, qualities, aligned bases, CIGAR, and tag data
		bAlignment.QueryBases.clear();
		bAlignment.Qualities.clear();
		bAlignment.AlignedBases.clear();
		bAlignment.CigarData.clear();
		bAlignment.TagData.clear();

		// save name
		bAlignment.Name = (string)((const char*)(allCharData));
		
		// save bases
		char singleBase[2];
		uint8_t* s = ( allCharData + (numCigarOperations*4) + queryNameLength);
		for (unsigned int i = 0; i < querySequenceLength; ++i) { 
			// numeric to char conversion
			sprintf( singleBase, "%c", DNA_LOOKUP[ ( ( s[(i/2)] >> (4*(1-(i%2)))) & 0xf ) ] );
			// append character data to Bases
			bAlignment.QueryBases.append( (const char*)singleBase );
		}
		
		// save sequence length
		bAlignment.Length = bAlignment.QueryBases.length();
		
		// save qualities
		char singleQuality[4];
		uint8_t* t = ( allCharData + (numCigarOperations*4) + queryNameLength + (querySequenceLength + 1)/2);
		for (unsigned int i = 0; i < querySequenceLength; ++i) { 
			// numeric to char conversion
			sprintf( singleQuality, "%c", ( t[i]+33 ) ); 
			// append character data to Qualities
			bAlignment.Qualities.append( (const char*)singleQuality );
		}
		
		// save CIGAR-related data;
		int k = 0;
		for (unsigned int i = 0; i < numCigarOperations; ++i) {

			// build CigarOp struct
			CigarOp op;
			op.Length = (cigarData[i] >> BAM_CIGAR_SHIFT);
			op.Type   = CIGAR_LOOKUP[ (cigarData[i] & BAM_CIGAR_MASK) ];

			// save CigarOp
			bAlignment.CigarData.push_back(op);

			// can skip this step if user wants to ignore
			if (m_isCalculateAlignedBases) {

				// build AlignedBases string
				switch (op.Type) {
					
					case ('M') : 
					case ('I') : bAlignment.AlignedBases.append( bAlignment.QueryBases.substr(k, op.Length) ); 	// for 'M', 'I' - write bases
					case ('S') : k += op.Length;                                                     			// for 'S' - skip over query bases
								 break;
					
					case ('D') : 
					case ('P') : bAlignment.AlignedBases.append( op.Length, '*' );	// for 'D', 'P' - write padding;
								 break;
					
					case ('N') : bAlignment.AlignedBases.append( op.Length, 'N' );  // for 'N' - write N's, skip bases in query sequence
								 k += op.Length;
								 break;

					case ('H') : break; 											// for 'H' - do nothing, move to next op
					
					default    : assert(false);	// shouldn't get here
								 break;
				}
			}
		}

		// read in the tag data
		bAlignment.TagData.resize(tagDataLen);
		memcpy((char*)bAlignment.TagData.data(), tagData, tagDataLen);
	}
	free(allCharData);

/*
	// (optional) read tag parsing data
	string tag;
	char data;
	int i = 0;

	// still data to read (auxiliary tags)
	while ( bytesRead < block_len ) {

		if ( bam_read(m_file, &data, 1) == 1 ) { 
			
			++bytesRead;

			if (bytesRead == block_len && data != '\0') {
				fprintf(stderr, "ERROR: Invalid termination of tag info at end of alignment block.\n");
				exit(1);
			}

			tag.append(1, data);
			if ( data == '\0' ) {
				bAlignment.Tags.push_back(tag);
				tag = "";
				i = 0;
			} else {
				if ( (i == 1) && (i == 2) ) { tag.append(1, ':'); }
				++i;
			}
		} else {
			fprintf(stderr, "ERROR: Could not read tag info.\n");
			exit(1);
		}
	}
*/
	return true;
}
