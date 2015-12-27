# Dictionary makefile
CC=gcc
EXEC=predict
CFLAGS=-g -Wall -Wshadow 
LIBS=-lcurses -lncurses -pthread -lm 
SOURCES=predictive.c dictionary.c tree26.c hash.c
DEPENDANTS=predictive.h dictionary.h hash.h tree26.h

#lookup : dictionary.h hash.h tree26.h
#	$(CC) $(CFLAGS) main.c dictionary.c tree26.c hash.c -o lookup

predict : $(SOURCES) $(DEPENDANTS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXEC) $(LIBS)


change : 
	$(CC) change.c 

clean :
	rm lookup -f; rm a.out -f 
