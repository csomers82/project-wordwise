#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "dictionary.h"

/*
 * This is a file to create static and dynamic hash tables and basic
 * functions for handling the searching and storing of values from it
 *
 */


/* Dictionary Struct Function Tools */

///
//	Allocates a new unitialized Dictionary Struct 
//	in:		int max = the max table size
//			flaot threshold = percentage of currSize-to-maxSize that triggers reallocation
//			float growth_factor = ratio of the oldTable-to-newTable when reallocating
//	out:	Dictionary * new 
//
Dictionary * dictionary_create(int max, float threshold, float growth_factor)
{
	//LOCAL VARIABLES
	Dictionary * new = malloc(sizeof(Dictionary));
	char ** data;// ptr at hash_table
	int i;//index for iterating through hash table data
	
	//EXECUTABEL STATEMENTS
	new->max_size = max;
	new->hash_table = malloc(sizeof(char*) * max);
	data = (new->hash_table);
	for(i = 0; i < max; ++i)
	{
		data[i] = NULL;
	}
	new->cur_size = 0;
	new->threshold = threshold;
	new->growth_factor = growth_factor;
	new->hash_func = NULL;
	return(new);
}

///
//	Gives a newly allocated Dictionary a hash fx
//	in:		int (*hash_fx_ptr)(char*, int) = hash fx that accepts a str and a max tbl size
//			Dictionary * dn = dictionary target of initalization
//
void dictionary_initialize(int (*hash_fx_ptr)(const char*, int), Dictionary * dn)
{
	dn->hash_func = hash_fx_ptr;
}

///
//	Frees all of the associated memory in the Dictionary
//	in:		Dictionary * dn = vic of 'free'
//
void dictionary_free(Dictionary * dn)
{
	//LOCAL VARIABLES
	int i;// for iterating through hash table
	char ** data;// ptr at hash_table data

	//EXECUTABLE STATEMENTS
	data = (dn->hash_table);
	for(i = 0; i < dn->max_size; ++i)
		free(data[i]);
	free(data);
	free(dn);
}

///
//	Rehashes the data of the old dictionary to a new dictionary
//	in:		Dictionary * dn = curr_size/max_size > threshold
//	out:	Dictionary * new = curr/max(size) = threshold/growth_factor
//
Dictionary * dictionary_grow(Dictionary * dn)
{
	//LOCAL VARIABLES
	Dictionary * new;// grown table
	char ** data;// ptr at hash_table data
	int i;// for iterating through hash table
	int new_loc;// hash index for a str
	int (*hfunc)(const char* search, int max);// a ptr to the hash func
	int max;// max size that will be used
	int collisions;// a count of bad hashes for a str
	
	//EXECUTABLE STATEMENTS
	max = (int)(dn->max_size * dn->growth_factor);
	printf("\e[36mnew size = %d\e[0m\n", max);
	hfunc = dn->hash_func;
	new = dictionary_create(max, dn->threshold, dn->growth_factor);
	dictionary_initialize(hfunc, new);
	
	
	//reshash any previous data (new table already initialized)
	data = (dn->hash_table);
	for(i = 0; i < dn->max_size; ++i)
	{
		//if there is an entry at this hash index:
		if(data[i] != NULL)
		{
			//rehash location in new table
			new_loc = hfunc(data[i], max);
			collisions = 0;
			while(new->hash_table[new_loc] != NULL && ++collisions < dn->max_size) 
			{	new_loc = (collisions * collisions + new_loc) % max;
			}
			assert(collisions < dn->max_size);
			//map that ptr to the str from data
			new->hash_table[new_loc] = data[i];
		}
	}
	free(dn);
	return(new);
}

///
//	Hashes a single string, preserving the ptr, into the dicitonary's hash table
//	in:		Dictionary * dn = storage for string
//			const char * str = entry being stored 
//
void dictionary_add_entry(Dictionary * dn, const char * str)
{
	//LOCAL VARIABLES
	char ** data;// ptr at hash_table data
	int probe_i;// hash index for a str
	int (*hfunc)(const char* search, int max);// a ptr to the hash func
	int collisions;// a count of bad hashes for a str
	float volume;// the hash table's percentage full 
	
	//EXECUTABLE STATEMENTS
	hfunc = dn->hash_func;
	
	//dereference the hash table data
	data = (dn->hash_table);
	
	//rehash location in new table
	probe_i = hfunc(str, dn->max_size);
	collisions = 0;
	while(data[probe_i] != NULL && ++collisions < dn->max_size * 100) 
	{	probe_i = (collisions * collisions + probe_i) % dn->max_size;
	}
	if(collisions >= dn->max_size)
	{
		fprintf(stderr, "Excessive collisions.\nmax_size = %d, collisions = %d\n",dn->max_size, collisions);
	}
	//map that ptr to the str from data
	data[probe_i] = (char *)str;
	dn->cur_size += 1;

	volume = dn->cur_size * 100.0f / dn->max_size;
	//printf("percentage full = %f%%\n", volume);
	if(volume >= dn->threshold)
	{	printf("\e[32mTable expanded\e[0m\n");
		Dictionary * new = dictionary_grow(dn);
		*dn = *new;
	}
}

///
//	Gives a ptr to the dictionary entry that matches query, or NULL if np
//	in:		Dictionary * dn = the dictionary being searched
//			char * query = the string being searched
//	out:	char * found = the char ptr or NULL if not found
//
char * dictionary_search(Dictionary * dn, char * query)
{
	//LOCAL VARIABLES
	int (*hash_fx)(const char *, int);// dereferenced dictionary i/o fx
	int probe_i;// string index in the hash table
	int collisions;// count of collisions that have occured
	char ** table;// hash table data from the dictionary
	int max;//the max tabel size
	
	//EXECUTABLE STATEMENTS
	collisions = 0;
	max = dn->max_size;
	// hash the query
	hash_fx = dn->hash_func;
	probe_i = hash_fx(query, max);
	// rehash the query conditionally
	while(!strcmp(query, table[probe_i]) && ++collisions < max)
	{	probe_i = (collisions * collisions + probe_i) % max;
	}
	if(collisions >= max)
	{	fprintf(stderr, "Error: Query not found\n");
		return(NULL);
	}

	return(table[probe_i]);
}

///
//	Takes two dictionary structs and combines their tables into a single table w/ all of the 
//	properties of the table 'favored'
//	in:		Dictionary * favored = contributes table_data, hash_fx, growth_rate 
//			Dictionary * minor = contributes only its table data
//			float new_threshold = newly specified percent full alotment
//	out:	Dictionary * combined = the combined tables, favored properties, and new sizes
//									
Dictionary * dictionary_merge(Dictionary * favored, Dictionary * minor, float new_threshold)
{
	//LOCAL VARIABLES
	Dictionary * merged;// the combined dictionary
	char ** data;// ptr at hash_table data
	int i;// for iterating through hash table
	int new_loc;// hash index for a str
	int (*hfunc)(const char* search, int max);// a ptr to the hash func
	int max;// max size that will be used
	int collisions;// used for rehashing
	
	//EXECUTABLE STATEMENTS
	max = (int)(favored->max_size * favored->growth_factor);
	hfunc = favored->hash_func;
	merged = dictionary_create(max, new_threshold, favored->growth_factor);
	dictionary_initialize(hfunc, merged);
	
	//reshash any previous data from FAVORED (new table already initialized)
	data = (favored->hash_table);
	for(i = 0; i < favored->max_size; ++i)
	{
		//if there is an entry at this hash index:
		if(data[i] != NULL)
		{
			//rehash location in new table
			new_loc = hfunc(data[i], max);
			collisions = 0;
			while(merged->hash_table[new_loc] != NULL && ++collisions < favored->max_size) 
			{	new_loc = (collisions * collisions + new_loc) % merged->max_size;
			}
			assert(collisions < favored->max_size);
			
			//map that ptr to the str from data
			merged->hash_table[new_loc] = data[i];
		}
	}	
	//reshash any previous data from MINOR
	data = (minor->hash_table);
	for(i = 0; i < minor->max_size; ++i)
	{
		//if there is an entry at this hash index:
		if(data[i] != NULL)
		{
			//rehash location in new table
			new_loc = hfunc(data[i], max);
			collisions = 0;
			while(merged->hash_table[new_loc] != NULL && ++collisions < minor->max_size) 
			{	new_loc = (collisions * collisions + new_loc) % merged->max_size;
			}
			assert(collisions < minor->max_size);
			
			//map that ptr to the str from data
			merged->hash_table[new_loc] = data[i];
		}
	}
	free(favored);
	free(minor);
	return(merged);

}

///
//	Takes all data from a file and reads it in to inititalize a new Dictionary struct
//	in:		FILE* fp = the open stream where the dictionary is being read from
//	out:	Dictionary * new = the newly allocated, (yet unitialized?), dictionary
//
Dictionary * dictionary_read(FILE * fp)
{
	return(NULL);
}

///
//	Writes all of the data in a dictionary to a readable file - under development
//	in:		Dictionionary * dn = target of being written
//	out:	int success = 0 if true
//
int dictionary_write(Dictionary * dn)
{
	return(1);
}


/* Hashing and Collision Control */
///
//	Sum char values as integers, modulo table size
//	in:		const char * str = data being hashed
//			int max_tbl_size = max not current table size
//	out:	int index = integer hash value
//
int hash_0_char_sum(const char * str, int max_tbl_size)
{
	//LOCAL VARIABLES
	int len = strlen(str);
	int ind, sum;
	//EXECUTABLE STATEMENTS
	sum = 0;
	for(ind = 0; ind < len; ++ind)
		sum += str[ind];
	return(sum % max_tbl_size);
}



///
//	Multiply's strlen(str) by percentage of max_tbl_size, add char sum, modulo max_tbl_size
//	in:		const char * str = data being hashed
//			int max_tbl_size = max not current table size
//	out:	int index = integer hash value
//
int	hash_1_str_len(const char * str, int max_tbl_size)
{
	//LOCAL VARIABLES
	int len = strlen(str);
	int ind, hash;
	//EXECUTABLE STATEMENTS
	hash = 0;
	for(ind = 0; ind < len; ++ind)
		hash += str[ind];
	for(ind = 1; ind < len; ++ind)
		hash += 1000; 
	return(hash % max_tbl_size);
}


///
//	Sum char values as times char position, modulo table size
//	in:		const char * str = data being hashed
//			int max_tbl_size = max not current table size
//	out:	int index = integer hash value
//
int hash_2_sum_char_products(const char * str, int max_tbl_size)
{
	//LOCAL VARIABLES
	int len = strlen(str);
	int ind;
	long hash;
	//EXECUTABLE STATEMENTS
	hash = 0;
	for(ind = 0; ind < len; ++ind)
		hash += str[ind] * ind;
	return (int)(hash % max_tbl_size);
}


