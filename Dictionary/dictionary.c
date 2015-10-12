
#include <stdin.h>
#include <stdlib.h>
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
	new->curr_size = 0;
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
void dictionary_initialize(int (*hash_fx_ptr)(char*, int), Dictionary * dn)
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
	int (*hfunc)(char* search, int max);// a ptr to the hash func
	int max;// max size that will be used
	
	//EXECUTABLE STATEMENTS
	max = (int)(dn->max_size * dn->growth_factor);
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
			//map that ptr to the str from data
			new->hash_table[new_loc] = data[i];
		}
	}
	return(new);
}


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
