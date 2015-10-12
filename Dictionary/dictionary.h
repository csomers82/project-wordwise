
#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdin.h>
#include <string.h>
#include <ctype.h>

/*
 * This is a file to create static and dynamic hash tables and basic
 * functions for handling the searching and storing of values from it
 *
 */

typedef struct Dictionary {
	char ** hash_table;// the dictionary data in a char* array
	unsigned int max_size;// the number of dictionary entries max
	unsigned int cur_size;// the number of entries already stored
	int (*hash_func)(char***, char *);// ptr to the dictionary's method of i/o
	int growth_factor;// the ratio of memory when reallocating table
} Dictionary;

Dictionary * dictionary_create(unsigned int max, unsigned int growth_f);







#endif
