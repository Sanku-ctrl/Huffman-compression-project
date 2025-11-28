
# # Simple Makefile for the Huffman project
# # - builds a single executable at bin/huffman
# # - provides `make clean`, `make run`, and `make test`

# CC := gcc
# CFLAGS := -std=c11 -Wall -Wextra -O2

# SRCDIR := src
# BINDIR := bin
# TARGET := $(BINDIR)/huffman

# SOURCES := $(SRCDIR)/main.c $(SRCDIR)/huffman.c
# OBJECTS := $(patsubst $(SRCDIR)/%.c,$(BINDIR)/%.o,$(SOURCES))

# INCLUDES := -I$(SRCDIR)

# .PHONY: all clean run test

# all: $(TARGET)

# $(BINDIR):
# 	mkdir -p $(BINDIR)

# $(BINDIR)/%.o: $(SRCDIR)/%.c | $(BINDIR)
# 	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# $(TARGET): $(OBJECTS)
# 	$(CC) $(CFLAGS) $^ -o $@

# clean:
# 	rm -rf $(BINDIR)

# # Convenience: run compress on sample file (creates sample.huff)
# run: all
# 	./$(TARGET) -c test_files/sample_large.txt test_files/sample.huff

# # Test: compress then decompress and compare
# test: all
# 	@echo "Running sample compress/decompress test..."
# 	./$(TARGET) -c test_files/sample_large.txt test_files/sample.huff
# 	./$(TARGET) -d test_files/sample.huff test_files/sample.out
# 	@if cmp --silent test_files/sample_large.txt test_files/sample.out; then \
# 		echo "Test passed: output matches input"; \
# 		rm -f test_files/sample.huff test_files/sample.out; \
# 	else \
# 		echo "Test FAILED: output differs"; \
# 		exit 1; \
# 	fi

# modified version
# Compiler and flags
CC = gcc
# CFLAGS: -Wall (all warnings), -Wextra (extra warnings), -g (debug symbols), -O2 (optimization level 2)
CFLAGS = -Wall -Wextra -g -O2
# LDFLAGS: -lm (link math library, if needed for anything)
LDFLAGS = -lm

# --- Source Files ---
# Main CLI sources
CLI_SRCS = src/main.c src/huffman.c
# Library sources
LIB_SRCS = src/huffman.c
# Object files (auto-generates .o files in build/ for each .c)
CLI_OBJS = $(patsubst src/%.c, build/%.o, $(CLI_SRCS))
LIB_OBJS = $(patsubst src/%.c, build/%.o, $(LIB_SRCS))

# --- Target Executables ---
CLI_TARGET = bin/huffman
LIB_TARGET = bin/libhuffman.so

# --- Build Rules ---

# Default target: build all
# THIS IS THE CRITICAL FIX: It now builds BOTH targets
all: bin build $(CLI_TARGET) $(LIB_TARGET)

# Create bin and build directories
bin:
	mkdir -p bin

build:
	mkdir -p build

# Rule to build the CLI executable
$(CLI_TARGET): $(CLI_OBJS)
	$(CC) $(CFLAGS) -o $(CLI_TARGET) $(CLI_OBJS) $(LDFLAGS)
	@echo "Compiled CLI: $(CLI_TARGET)"

# Rule to build the shared library
# -fPIC: Position Independent Code (required for shared libs)
# -shared: Create a shared library
$(LIB_TARGET): $(LIB_OBJS)
	$(CC) $(CFLAGS) -fPIC -shared -o $(LIB_TARGET) $(LIB_OBJS) $(LDFLAGS)
	@echo "Compiled Shared Library: $(LIB_TARGET)"

# Rule to compile .c files into .o object files in the build/ directory
# -c: Compile only (don't link)
# $<: The first prerequisite (the .c file)
# $@: The target (the .o file)
build/%.o: src/%.c src/huffman.h
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

# --- Cleanup Rule ---

clean:
	rm -rf build bin
	@echo "Cleaned build artifacts."

# Phony targets don't represent actual files
.PHONY: all clean bin build