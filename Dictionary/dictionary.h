
#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <time.h>

/*
 * This is a file to create static and dynamic hash tables and basic
 * functions for handling the searching and storing of values from it
 *
 */

// FERMOT's FINAL PROOF
// proof with most attempts and failures thatn any other
// tree - ways possible to win

/* Bit Masks */
#define BITMASK_DATA_TABLE		0x0000000F 
#define BITMASK_DATA_TREE		0x000000F0
#define BITMASK_DATA_STORAGE	0x000000FF
#define BITMASK_HASH_PRIMARY	0x0000FF00
#define BITMASK_HASH_SECONDARY	0x00FF0000


/* Structures */
typedef struct Dictionary {
	uint32_t config;// a bitpacked boolean value representing the Dictionary's configuration
	char ** hash_table;// the dictionary data in a char* array
	void * tree_head;// the dictionary data as a node* tree 
	int max_size;// the number of dictionary entries max
	int cur_size;// the number of entries already stored
	int (*hash_func)(const char*, int max);// ptr to the dictionary's method of i/o
	int (*hash_second)(const char*, int max, int probe, int attempt);// ptr to the dictionary's method of rehashinrehashingg
	long t_entry_collisions;// sum of all entry collions
	long t_search_collisions;// sum of all collisions when searching
	int t_hash_searches;// number of searches admistered via hash table
	float threshold;// the percentage of the table that needs to be full in order to realloc
	float growth_factor;// the ratio of oldSize-to-newSize memory when reallocating table
	float avg_entry_collisions;// t_entry_c / cur_size
	float avg_search_collisions;// t_search_C / cur_size
} Dictionary;


/* Dictionary Struct Function Tools */


///
//	packs various values into an int so that less parameters
//	require being passed into creating the dictionary
//	in:		int tree_num	= 0 or index of dicitonary tree struct
//			int table_num	= 0 or index of dicitonary table struct
//			int hash_num	= 0 or index of the hash function
//			int rehash_num	= 0 or index of the rehash function
//	out:	uint32_t config = packed configuration value
//
uint32_t dictionary_config(int table_num, int tree_num, int hash_num, int rehash_num);



///
//	Allocates a new unitialized Dictionary Struct 
//	in:		int max = the max table size
//			uint32_t = storage of how the dictionary will be configured
//	out:	Dictionary * new 
//
Dictionary * dictionary_create(int max, uint32_t config );

///
//	Gives a newly allocated Dictionary a hash fx
//	in:		
//			Dictionary * dn = dictionary target of initalization
//			float threshold = percentage of currSize-to-maxSize that triggers reallocation
//			float growth_factor = ratio of the oldTable-to-newTable when reallocating
//
void dictionary_initialize(Dictionary * dn, float threshold, float growth_factor);


///
//	Appends the dicitonaries statistics to the logfile
//
void dictionary_log_statistics(Dictionary * dn);



///
//	Frees all of the associated memory in the Dictionary
//	in:		Dictionary * dn = vic of 'free'
//
void dictionary_free(Dictionary * dn);

///
//	Finds a valid table index given a string key
//	in:		Dictionary * dn = where the data an be found
//			const char * str = entry to be probed
//	out:	int position = final resing place of string
//
int dictionary_probe_table(Dictionary * dn, const char * str);


///
//	Rehashes the data of the old dictionary to a new dictionary
//	in:		Dictionary * dn = curr_size/max_size > threshold
//	out:	Dictionary * new = curr/max(size) = threshold/growth_factor
//
Dictionary * dictionary_grow(Dictionary * dn);


///
//	Hashes a single string, preserving the ptr, into the dicitonary's hash table
//	in:		Dictionary * dn = storage for string
//			const char * str = entry being stored 
//
void dictionary_add_entry(Dictionary * dn, const char * str);

///
//	Gives a ptr to the dictionary entry that matches the query, or NULL if np
//	in:		Dictionary * dn = the dictionary being searched
//			char * query = the string being searched
//	out:	char * found = the char ptr or NULL if not found
//
char * dictionary_search(Dictionary * dn, char * query);

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
//	in:		const char * str = data being hashed
//			int max_tbl_size = max not current table size
//	out:	int index = integer hash value
//
int	hash_0_char_sum(const char * str, int max_tbl_size);

///
//	Multiply's strlen(str) by percentage of max_tbl_size, add char sum, modulo max_tbl_size
//	in:		const char * str = data being hashed
//			int max_tbl_size = max not current table size
//	out:	int index = integer hash value
//
int hash_1_str_len(const char * str, int max_tbl_size);

///
//	Sum char values as times char position, modulo table size
//	in:		const char * str = data being hashed
//			int max_tbl_size = max not current table size
//	out:	int index = integer hash value
//
int hash_2_sum_char_products(const char * str, int max_tbl_size);


#endif
