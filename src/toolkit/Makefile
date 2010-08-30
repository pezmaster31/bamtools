# ==========================
# BamTools Makefile
# (c) 2010 Derek Barnett
#
# src/toolkit
# ==========================

API_DIR   = ../api
UTILS_DIR = ../utils
OBJ_DIR   = ../../obj
BIN_DIR   = ../../bin
THIRD_PARTY_DIR = ../third_party

INCLUDES = -I$(API_DIR)/ -I$(UTILS_DIR) -I$(THIRD_PARTY_DIR)

# ----------------------------------
# define our source and object files
# ----------------------------------
SOURCES = bamtools_convert.cpp \
          bamtools_count.cpp \
          bamtools_coverage.cpp \
          bamtools_filter.cpp \
          bamtools_header.cpp \
          bamtools_index.cpp \
          bamtools_merge.cpp \
          bamtools_random.cpp \
          bamtools_sort.cpp \
          bamtools_stats.cpp \
          bamtools.cpp 
OBJECTS= $(SOURCES:.cpp=.o)
BUILT_OBJECTS= $(patsubst %,$(OBJ_DIR)/%,$(OBJECTS))
EXT_OBJECTS = $(OBJ_DIR)/*.o
PROGRAM = bamtools

all: $(PROGRAM)

.PHONY: all

$(PROGRAM): $(BUILT_OBJECTS) $(EXT_OBJECTS)
	@echo "  * linking $(PROGRAM)"
	@$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $(BIN_DIR)/$@ $^ $(LIBS)

$(BUILT_OBJECTS): $(SOURCES)
	@echo "  * compiling" $(*F).cpp
	@$(CXX) -c -o $@ $(*F).cpp $(LDFLAGS) $(CXXFLAGS) $(INCLUDES)

$(EXT_OBJECTS):
	@$(MAKE) --no-print-directory -C $(API_DIR) $(UTILS_DIR)	
	
clean:
	@echo "Cleaning up."
	@rm -f $(OBJ_DIR)/* $(BIN_DIR)/*

.PHONY: clean
