CXX=		g++
CXXFLAGS=	-Wall -O3
PROG=		BamConversion BamDump BamTrim BamMultiMerge
LIBS=		-lz

all: $(PROG)

BamConversion:  BGZF.o BamReader.o BamWriter.o BamConversionMain.o
	$(CXX) $(CXXFLAGS) -o $@  BGZF.o BamReader.o BamWriter.o BamConversionMain.o $(LIBS)

BamDump:  BGZF.o BamReader.o BamDumpMain.o
	$(CXX) $(CXXFLAGS) -o $@  BGZF.o BamReader.o BamDumpMain.o $(LIBS)

BamTrim:  BGZF.o BamReader.o BamWriter.o BamTrimMain.o
	$(CXX) $(CXXFLAGS) -o $@  BGZF.o BamReader.o BamWriter.o BamTrimMain.o $(LIBS)

BamMultiMerge: BGZF.o BamMultiReader.o BamReader.o BamWriter.o BamMultiMergeMain.o
	$(CXX) $(CXXFLAGS) -o $@ BGZF.o BamMultiReader.o BamReader.o BamWriter.o BamMultiMergeMain.o $(LIBS)

clean:
	rm -fr gmon.out *.o *.a a.out *~
