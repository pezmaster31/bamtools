# ==========================
# BamTools Makefile
# (c) 2010 Derek Barnett
#
# src/utils
# ==========================

API_DIR = ../api
OBJ_DIR = ../../obj
BIN_DIR = ../../bin

INCLUDES = -I$(API_DIR)/

# ----------------------------------
# define our source and object files
# ----------------------------------
SOURCES = bamtools_fasta.cpp \
          bamtools_options.cpp \
          bamtools_pileup.cpp \
          bamtools_utilities.cpp 
OBJECTS= $(SOURCES:.cpp=.o)
BUILT_OBJECTS= $(patsubst %,$(OBJ_DIR)/%,$(OBJECTS))

all: $(BUILT_OBJECTS)

$(BUILT_OBJECTS): $(SOURCES)
	@echo "  * compiling" $(*F).cpp
	@$(CXX) -c -o $@ $(*F).cpp $(LDFLAGS) $(CXXFLAGS) $(INCLUDES)

.PHONY: all

clean:
	@echo "Cleaning up."
	@rm -f $(OBJ_DIR)/* $(BIN_DIR)/*

.PHONY: clean
