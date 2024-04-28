###
 # @file Makefile
 # @author Vincent Marias <vmarias@mines.edu>
 # @date 04/27/2024
 #
 # @desc This file is part of the JackAnalyzer program. Provides build targets
 # for GNU Make.
 #
 # @copyright Vincent Marias 2024
 ##

LEX_GEN = flex
LEX_FILES = jack.l
LEX_OPTS =
LEXER = lex.yy.cpp

PARSE_GEN = bison
PARSE_FILE = jack.y
PARSE_OPTS =
PARSER =

TARGET = JackAnalyzer
VPATH = src
INCLUDE_DIR = include
SRC_FILES = $(LEXER) $(PARSER)

CC = clang++
CCFLAGS =  -Og -I$(INCLUDE_DIR)
CCFLAGS_DEBUG = -g
CCFLAGS_WARNINGS = -Wall -Wextra -Wconversion -Wdouble-promotion -Wunreachable-code -Wshadow -Wpedantic -pedantic-errors
# CCFLAGS_SANITIZER = -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=leak -fsanitize=undefined
CVERSION = -std=c++17
LINKER_OPTS = -lfl

# export ASAN_OPTIONS=detect_invalid_pointer_pairs=2

OBJECTS = $(SRC_FILES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o  $@ $(CCFLAGS_SANITIZER) $(LINKER_OPTS) $^

.cpp.o:
	$(CC) $(CCFLAGS) $(CVERSION) $(CCFLAGS_SANITIZER) $(CCFLAGS_DEBUG) $(CCFLAGS_WARNINGS) -o $@ -c $<

$(LEXER): $(LEX_FILES)
	$(LEX_GEN) -o $(LEXER) $(LEX_OPTS) $(LEX_FILES)

clean:
	rm -f $(TARGET) $(OBJECTS) $(LEXER)

.PHONY: all clean depend
