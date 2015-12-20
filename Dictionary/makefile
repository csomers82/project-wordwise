# Dictionary makefile
CC=gcc
EXEC=lookup
CFLAGS=-g 
LIBS=
SOURCES=dictionary.c tree26.c hash.c main.c
DEPENDANTS=dictionary.h hash.h tree26.h

lookup : $(SOURCES) $(DEPENDANTS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXEC)

change : 
	$(CC) change.c 

clean :
	rm lookup -f; rm a.out -f 
