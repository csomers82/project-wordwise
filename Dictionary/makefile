# Dictionary makefile
CC=gcc
EXEC=lookup
CFLAGS=-g 
LIBS=
SOURCES=dictionary.c hash.c main.c
DEPENDANTS=dictionary.h hash.h

lookup : $(SOURCES) $(DEPENDANTS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXEC)

change : 
	$(CC) change.c 

clean :
	rm lookup -f; rm a.out -f 
