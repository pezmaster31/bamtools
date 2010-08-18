OBJ_DIR = ../../obj
BIN_DIR = ../../bin

# ----------------------------------
# define our source and object files
# ----------------------------------
SOURCES = BGZF.cpp \
          BamIndex.cpp \
          BamReader.cpp \
          BamMultiReader.cpp \
          BamWriter.cpp
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
