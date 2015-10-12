
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

/* Structures */
typedef struct Dictionary {
	char ** hash_table;// the dictionary data in a char* array
	int max_size;// the number of dictionary entries max
	int cur_size;// the number of entries already stored
	int (*hash_func)(char***, char *);// ptr to the dictionary's method of i/o
	float threshold;// the percentage of the table that needs to be full in order to realloc
	float growth_factor;// the ratio of oldSize-to-newSize memory when reallocating table
} Dictionary;


/* Dictionary Struct Function Tools */

///
//	Allocates a new unitialized Dictionary Struct 
//	in:		int max = the max table size
//			flaot threshold = percentage of currSize-to-maxSize that triggers reallocation
//			float growth_factor = ratio of the oldTable-to-newTable when reallocating
//	out:	Dictionary * new 
//
Dictionary * dictionary_create(int max, float threshold, float growth_factor);

///
//	Gives a newly allocated Dictionary a hash fx
//	in:		int (*hash_fx_ptr)(char*, int) = hash fx that accepts a str and a max tbl size
//			Dictionary * dn = dictionary target of initalization
//
void		 dictionary_initialize(int (*hash_fx_ptr)(char*, int), Dictionary * dn);

///
//	Frees all of the associated memory in the Dictionary
//	in:		Dictionary * dn = vic of 'free'
//
void		 dictionary_free(Dictionary * dn);

///
//	Rehashes the data of the old dictionary to a new dictionary
//	in:		Dictionary * dn = curr_size/max_size > threshold
//	out:	Dictionary * new = curr/max(size) = threshold/growth_factor
//
Dictionary * dictionary_grow(Dictionary * dn);

///
//	Gives a ptr to the dictionary entry that matches the str, or NULL if np
//	in:		Dictionary * dn = the dictionary being searched
//			char * str = the string being searched
//	out:	char * found = the char ptr or NULL if not found
//
char *		 dictionary_search(Dictionary * dn, char * str);

///
//	Takes two dictionary structs and combines their tables into a single table w/ all of the 
//	properties of the table 'favored'
//	in:		Dictionary * favored = contributes table_data, hash_fx, growth_rate 
//			Dictionary * minor = contributes only its table data
//			float new_threshold = newly specified percent full alotment
//	out:	Dictionary * combined = the combined tables, favored properties, and new sizes
//									
Dictionary * dictionary_merge(Dictionary * favored, Dictionary * minor, float new_threshold);


///
//	Takes all data from a file and reads it in to inititalize a new Dictionary struct
//	in:		FILE* fp = the open stream where the dictionary is being read from
//	out:	Dictionary * new = the newly allocated, (yet unitialized?), dictionary
//
Dictionary * dictionary_read(FILE * fp);

///
//	Writes all of the data in a dictionary to a readable file - under development
//	in:		Dictionionary * dn = target of being written
//	out:	int success = 0 if true
//
int			 dictionary_write(Dictionary * dn);

/* Hashing and Collision Control */
///
//	Sum char values as integers, modulo table size
//	in:		char * str = data being hashed
//			int max_tbl_size = max not current table size
//	out:	int index = integer hash value
//
int			 hash_0_char_sum(char * str, int max_tbl_size);

///
//	Multiply's strlen(str) by percentage of max_tbl_size, add char sum, modulo max_tbl_size
//	in:		char * str = data being hashed
//			int max_tbl_size = max not current table size
//	out:	int index = integer hash value
//
int			 hash_1_str_len(char * str, int max_tbl_size);





#endif
