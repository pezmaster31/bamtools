CXX=		g++
CXXFLAGS=	-Wall -O3
PROG=		BamReaderTest BamConversion
LIBS=		-lz

all: $(PROG)

BamReaderTest: BamReader.o BamReaderMain.o
	$(CXX) $(CXXFLAGS) -o $@ BamReader.o BamReaderMain.o $(LIBS)

BamConversion: BamReader.o BamWriter.o BamConversionMain.o
	$(CXX) $(CXXFLAGS) -o $@ BamReader.o BamWriter.o BamConversionMain.o $(LIBS)

BamMerge: BamReader.o BamWriter.o BamMerge.o
	$(CXX) $(CXXFLAGS) -o $@ BamReader.o BamWriter.o BamMerge.o $(LIBS)

clean:
		rm -fr gmon.out *.o *.a a.out *~
