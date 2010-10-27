# ==========================
# BamTools Makefile
# (c) 2010 Derek Barnett
#
# top-level
# ==========================

# define current BamTools version
BT_MAJOR_VER = 0
BT_MINOR_VER = 8
BT_BUILD_VER = 1025
export BT_VERSION = $(BT_MAJOR_VER).$(BT_MINOR_VER).$(BT_BUILD_VER)

# define main directories
export OBJ_DIR  = obj
export BIN_DIR  = bin
export SRC_DIR  = src

# define compile/link flags
export CXX      = g++
export CXXFLAGS = -Wall -O3 -D_FILE_OFFSET_BITS=64 -DBT_VERSION=$(BT_VERSION)
export LIBS     = -lz

# define source subdirectories
SUBDIRS = $(SRC_DIR)/api \
          $(SRC_DIR)/utils \
          $(SRC_DIR)/third_party \
          $(SRC_DIR)/toolkit 

all:
	@echo "Building BamTools:"
	@echo "Version: $$BT_VERSION"
	@echo "========================================================="
	@echo ""
	@echo "- Creating target directories"
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)
	@echo ""
	@for dir in $(SUBDIRS); do \
		echo "- Building in $$dir"; \
		$(MAKE) --no-print-directory -C $$dir; \
		echo ""; \
	done

.PHONY: all

clean:
	@echo "Cleaning up."
	@rm -f $(OBJ_DIR)/* $(BIN_DIR)/*

.PHONY: clean
