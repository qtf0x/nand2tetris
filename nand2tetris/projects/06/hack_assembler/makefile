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
CCFLAGS = -Ofast -I$(INCLUDE_DIR)
CVERSION = -std=c17
# CCFLAGS_DEBUG = -g
# CCFLAGS_ERRORS = -Wall -Wextra -Wconversion -Wdouble-promotion -Wunreachable-code -Wshadow -Wpedantic -pedantic-errors

OBJECTS = $(SRC_FILES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^

.c.o:
	$(CC) $(CCFLAGS) $(CVERSION) $(CCFLAGS_DEBUG) $(CCFLAGS_ERRORS) -o $@ -c $<

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all clean depend
