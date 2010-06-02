CXX=		g++
CXXFLAGS=	-Wall -O3
PROG= 		bamtools
API=		BGZF.o BamReader.o BamWriter.o BamMultiReader.o
UTILS=		bamtools_options.o
TOOLKIT=	bamtools_count.o bamtools_coverage.o bamtools_header.o bamtools_index.o bamtools_merge.o bamtools_sam.o bamtools_sort.o bamtools_stats.o
MAIN=		bamtools.o
LIBS=		-lz

all: $(PROG)

bamtools: $(API) $(UTILS) $(TOOLKIT) $(MAIN)
	$(CXX) $(CXXFLAGS) -o $@ $(API) $(UTILS) $(TOOLKIT) $(MAIN) $(LIBS)

clean:
	rm -fr gmon.out *.o *.a a.out *~
