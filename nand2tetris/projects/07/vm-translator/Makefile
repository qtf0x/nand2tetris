###
 # @file Makefile
 # @author Vincent Marias <vmarias@mines.edu>
 # @date 03/19/2024
 #
 # @desc This file is part of the VMTranslator program. Provides build targets
 # for GNU Make.
 #
 # @copyright Vincent Marias 2024
 ##

TARGET = VMTranslator
VPATH = src
INCLUDE_DIR = include
SRC_FILES = translator.c parser.c writer.c

CC = cc
CCFLAGS =  -Og -I$(INCLUDE_DIR)
CVERSION = -std=c17
CCFLAGS_SANITIZER = -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=leak -fsanitize=undefined
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
