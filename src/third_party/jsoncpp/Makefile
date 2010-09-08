# ==========================
# BamTools Makefile
# (c) 2010 Derek Barnett
#
# src/third_party/jsoncpp
# ==========================

OBJ_DIR = ../../../obj
BIN_DIR = ../../../bin

# ----------------------------------
# define our source and object files
# ----------------------------------
SOURCES = json_reader.cpp \
          json_value.cpp \
          json_writer.cpp
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
