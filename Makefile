VERSION=\"0.1\"
PROG_NAME=\"cgol\"
BIN_NAME=cgol

CC = gcc

# Platform
UNAME := $(shell $(CC) -dumpmachine 2>&1 | grep -E -o "linux|darwin")

ifeq ($(UNAME), linux)
OSFLAGS = -DLINUX
DEBUG = -ggdb
else ifeq ($(UNAME), darwin)
OSFLAGS = -DMACOSX
DEBUG = -g
else ifeq ($(UNAME), solaris)
OSFLAGS = -DSOLARIS
DEBUG = -g
endif

EXTRA_CFLAGS = -Wall
LIBS = -lcurses
CFLAGS = $(DEBUG) $(EXTRA_CFLAGS) $(OSFLAGS)
DEFS = -DPACKAGE=$(PACKAGE) -DVERSION=$(VERSION) -DTIOCGWINSZ

all: main.o tio.o utf8.o tcap.o display.o globals.o
	$(CC) $(CFLAGS) $(LIBS) \
	tio.o     \
	utf8.o    \
	tcap.o    \
	display.o \
	globals.o \
	main.o    \
	-o $(BIN_NAME)

main.o: main.c
	$(CC) $(CFLAGS) $(DEFS) -DPROG_NAME=$(PROG_NAME) -c main.c

tio.o: tio.c defs.h utf8.h
	$(CC) $(CFLAGS) $(DEFS) -c tio.c

utf8.o: utf8.c utf8.h
	$(CC) $(CFLAGS) $(DEFS) -c utf8.c

tcap.o: tcap.c tcap.h func_defs.h defs.h
	$(CC) $(CFLAGS) $(DEFS) -c tcap.c

display.o: display.c
	$(CC) $(CFLAGS) $(DEFS) -c display.c

globals.o: globals.c cgol.h
	$(CC) $(CFLAGS) $(DEFS) -c globals.c

clean:
	rm -f $(BIN_NAME) *.o

.PHONY: check-syntax

check-syntax:
	$(CC) $(CFLAGS) -Wextra -pedantic -fsyntax-only $(CHK_SOURCES)
