###
 # @file makefile
 # @author Vincent Marias <vmarias@mines.edu>
 # @date 03/02/2024
 ##

TARGET = HackAssembler
SRC_FILES = assembler.c sym_tbl.c parser.c translator.c

CC = clang
CCFLAGS = -Ofast
CCFLAGS_DEBUG =
CCFLAGS_ERRORS = -Wall -Wextra -Wconversion -Wdouble-promotion -Wunreachable-code -Wshadow -Wpedantic -pedantic-errors
CVERSION = -std=c17

OBJECTS = $(SRC_FILES:.c=.o)

ifeq ($(shell echo "Windows"), "Windows")
	TARGET := $(TARGET).exe
	DEL = del
	Q =
else
	DEL = rm -f
	Q = "
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^

.c.o:
	$(CC) $(CCFLAGS) $(CVERSION) $(CCFLAGS_DEBUG) $(CCFLAGS_ERRORS) -o $@ -c $<

clean:
	$(DEL) $(TARGET) $(OBJECTS) makefile.bak

depend:
	@sed -i.bak '/^# DEPENDENCIES/,$$d' makefile
	@$(DEL) sed*
	@echo $(Q)# DEPENDENCIES$(Q) >> makefile
	@$(CC) -MM $(SRC_FILES) >> makefile

.PHONY: all clean depend

# DEPENDENCIES
assembler.o: assembler.c sym_tbl.h
sym_tbl.o: sym_tbl.c sym_tbl.h
