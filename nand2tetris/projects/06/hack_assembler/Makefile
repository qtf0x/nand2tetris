###
 # @file makefile
 # @author Vincent Marias <vmarias@mines.edu>
 # @date 03/02/2024
 ##

TARGET = Assembler
VPATH = src
INCLUDE_DIR = include
SRC_FILES = assembler.c sym_tbl.c parser.c translator.c

CC = cc
CCFLAGS =  -O2 -I$(INCLUDE_DIR)
CVERSION = -std=c17
# CCFLAGS_SANITIZER = -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=leak -fsanitize=undefined
# CCFLAGS_DEBUG = -g
# CCFLAGS_WARNINGS = -Wall -Wextra -Wconversion -Wdouble-promotion -Wunreachable-code -Wshadow -Wpedantic -pedantic-errors

export ASAN_OPTIONS=detect_invalid_pointer_pairs=2

OBJECTS = $(SRC_FILES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o  $@ $(CCFLAGS_SANITIZER) $^

.c.o:
	$(CC) $(CCFLAGS) $(CVERSION) $(CCFLAGS_SANITIZER) $(CCFLAGS_DEBUG) $(CCFLAGS_WARNINGS) -o $@ -c $<

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all clean depend
