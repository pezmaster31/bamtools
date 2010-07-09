CXX=		g++
CXXFLAGS=	-Wall -O3 -D_FILE_OFFSET_BITS=64
PROG= 		bamtools
API=		BGZF.o \
		BamIndex.o \
		BamReader.o \
		BamWriter.o \
		BamMultiReader.o
UTILS=		bamtools_options.o \
		bamtools_utilities.o
TOOLKIT=	bamtools_convert.o \
		bamtools_count.o \
		bamtools_coverage.o \
		bamtools_filter.o \
		bamtools_header.o \
		bamtools_index.o \
		bamtools_merge.o \
		bamtools_sam.o \
		bamtools_sort.o \
		bamtools_stats.o
MAIN=		bamtools.o
OBJS=		$(API) $(UTILS) $(TOOLKIT) $(MAIN)
LIBS=		-lz

all: $(PROG)

bamtools: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -fr gmon.out *.o *.a a.out *~
