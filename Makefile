#
# MAKEFILE
# Francesco Piras - Dario Varano
# June 2015
#

CC = gcc
CFLAGS = -c -Wall -Werror -O2
LDFLAGS =
PROG = lz78
SRCS = main.c dictionary.c bitio.c compressor.c decompressor.c
OBJS = $(SRCS:.c=.o)
BIN = ./bin/

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(BIN)$(PROG)

main.o: definitions.h compressor.h decompressor.h
	$(CC) $(CFLAGS) main.c -o main.o

dictionary.o: dictionary.h definitions.h
	$(CC) $(CFLAGS) dictionary.c -o dictionary.o

bitio.o: definitions.h bitio.h
	$(CC) $(CFLAGS) bitio.c -o bitio.o

compressor.o: compressor.h dictionary.h bitio.h
	$(CC) $(CFLAGS) compressor.c -o compressor.o

decompressor.o: decompressor.h dictionary.h bitio.h
	$(CC) $(CFLAGS) decompressor.c -o decompressor.o

.PHONY: clean	
clean:
	-rm *.o $(BIN)$(PROG)
