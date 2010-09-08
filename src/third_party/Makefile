# ==========================
# BamTools Makefile
# (c) 2010 Derek Barnett
#
# src/third_party
# ==========================

OBJ_DIR = ../../obj
BIN_DIR = ../../bin
SRC_DIR = 

# define source subdirectories
SUBDIRS = jsoncpp

all:
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
