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
LEX_OPTS = --8bit --noyywrap --bison-bridge --fast --nodefault --warn
LEXER = scanner.cpp

PARSE_GEN = bison
PARSE_FILES = jack.y
PARSE_OPTS = -d -t
PARSER = parser.cpp
PARSER_HEADER = parser.h

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

$(LEXER): $(LEX_FILES)
	$(LEX_GEN) -o $(LEXER) $(LEX_OPTS) $(LEX_FILES)

$(PARSER): $(PARSE_FILES)
	$(PARSE_GEN) -o $(PARSER) --header=$(PARSER_HEADER) $(PARSE_OPTS) $(PARSE_FILES)

.cpp.o:
	$(CC) $(CCFLAGS) $(CVERSION) $(CCFLAGS_SANITIZER) $(CCFLAGS_DEBUG) $(CCFLAGS_WARNINGS) -o $@ -c $<

clean:
	rm -f $(TARGET) $(OBJECTS) $(LEXER) $(PARSER)

.PHONY: all clean depend
