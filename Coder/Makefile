# Makefile for Coder
# created on 03.03.2012

CC  = gcc
CoderOBJ = Coder.o

VERSION = 1.00

CFLAGS = -Wall -O3 -DVERSION=\"$(VERSION)\"
CC_LDFLAGS = -lm

####### Build rules

all: Coder

Coder : $(CoderOBJ)
	$(CC) $(CFLAGS) -o $@ $(CoderOBJ) $(CC_LDFLAGS)
	
Coder.o : Coder.c
	$(CC) $(CFLAGS) -c Coder.c

clean:
	rm -f *~ *.o Coder
