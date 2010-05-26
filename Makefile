CXX=		g++
CXXFLAGS=	-Wall -O3
PROG= 		bamtools bamtools_test
LIBS=		-lz
OBJS=		BGZF.o BamReader.o BamWriter.o BamMultiReader.o bamtools.o

all: $(PROG)

bamtools: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -fr gmon.out *.o *.a a.out *~
