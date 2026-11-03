# bsf - GNU make + gcc
#
#   make          -> bsf.o
#   make test     -> builds and runs the STANDALONE tests

CC     = gcc
CFLAGS = -std=c89 -pedantic -Wall -Wextra -Wconversion -O2

all: bsf.o

bsf.o: bsf.c bsf.h
	$(CC) $(CFLAGS) -c bsf.c -o bsf.o

bsftest: bsf.c bsf.h
	$(CC) $(CFLAGS) -DSTANDALONE bsf.c -o bsftest

test: bsftest
	./bsftest

clean:
	rm -f bsf.o bsftest

.PHONY: all test clean
