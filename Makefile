CC=		gcc
CXX=		g++
CFLAGS=		-Wall -pg -O3 -m64
CXXFLAGS=	$(CFLAGS)
DFLAGS=		-D_IOLIB=2 #-D_NDEBUG
OBJS=		BamReader.o bgzf.o
PROG=		BamReaderTest
INCLUDES=       
ARFLAGS=	-crs
LIBS=		-lz
SUBDIRS=	.

.SUFFIXES:.c .cpp .o

.c.o:
		$(CC) -c $(CFLAGS) $(DFLAGS) $(INCLUDES) $< -o $@

.cpp.o:
		$(CXX) -c $(CXXFLAGS) $(DFLAGS) $(INCLUDES) $< -o $@

all: $(PROG) BamConversion

lib:libbambc.a

libbambc.a:$(OBJS)
		$(AR) $(ARFLAGS) $@ $(OBJS)

BamReaderTest:lib BamReaderMain.o
		$(CXX) $(CXXFLAGS) -o $@ BamReaderMain.o $(LIBS) -L. -lbambc

BamConversion: lib BamWriter.o BamConversionMain.o
	$(CXX) $(CXXFLAGS) -o $@ BamWriter.o BamConversionMain.o $(LIBS) -L. -lbambc

clean:
		rm -fr gmon.out *.o *.a a.out $(PROG) *~
