#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "dictionary.h"
#include "hash.h"

/*
 * This is a file to create static and dynamic hash tables and basic
 * functions for handling the searching and storing of values from it
 *
 */


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
uint32_t dictionary_config(int table_num, int tree_num, int hash_num, int rehash_num)
{
	//// LOCAL DECLARATIONS
	// Blank Configuration
	uint32_t config = 0x00000000;
	/*
	// Setting Individual Config Parts
	/ * 'BITMASK_' value is divided by amount of data the section
	 *	takes so that the result is a 1 in the starting bit of a
	 *	section of the config value. It is then multiplied by the
	 *	input number to enable the desired feature. * /
	uint32_t bitcast_tree = BITMASK_DATA_TREE / 0x0F * tree_num;
	uint32_t bitcast_table = BITMASK_DATA_TABLE / 0x0F * table_num;
	uint32_t bitcast_hashP = BITMASK_HASH_PRIMARY / 0xFF * hash_num;
	uint32_t bitcast_hashS = BITMASK_HASH_PRIMARY / 0xFF * rehash_num;
	//// UPDATE CONFIGURATION
	// In this section, due to the val of 'config' += is equivalent to |=
	config |= bitcast_tree; 
	config |= bitcast_table; 
	config |= bitcast_hashP;
	config |= bitcast_hashS; */
	
	//// UPDATE CONFIGURATION
	// put in the data values and slide the current ones over
	config |= rehash_num;
	config = config << 8;
	config |= hash_num;
	config = config << 4;
	config |= tree_num;
	config = config << 4;
	config |= table_num;
	return(config);
}

///
//	Allocates a new unitialized Dictionary Struct 
//	in:		int max = the max table size
//			uint32_t = storage of how the dictionary will be configured
//	out:	Dictionary * new 
//
Dictionary * dictionary_create(int max, uint32_t config)
{
	//LOCAL VARIABLES
	Dictionary * new = malloc(sizeof(Dictionary));
	int (*hashP)(const char *, int) = NULL;// a pointer to a function for dictionary use
	int (*hashS)(const char *, int, int, int) = NULL;// a pointer to a function for dictionary use
	char ** data;// ptr at hash_table 
	int i;//index for iterating through hash table data
	int table = (BITMASK_DATA_TABLE & config) ? 1 : 0;
	int tree =  (BITMASK_DATA_TREE & config) ? 1 : 0;
	
	///// EXECUTABLE STATEMENTS 
	//// overhead data values
	new->config = config;
	new->max_size = max;
	new->cur_size = 0;
	new->t_entry_collisions = 0;
	new->t_search_collisions = 0;
	new->t_hash_searches = 0;
	new->avg_entry_collisions = 0.0f;
	new->avg_search_collisions = 0.0f;
	/// default values if not initialized
	new->threshhold = 75.0f;//%
	new->growth_factor = 1.618033f;//phi
	new->hash_table = NULL;
	new->tree_head = NULL;

	//// config specific values
	/// assign a tree structure for Dictionary use, if apl
	if(table)
	{
		new->hash_table = malloc(sizeof(char*) * max);
		data = (new->hash_table);
		for(i = 0; i < max; ++i)
		{
			data[i] = NULL;
		}
		new->cur_size = 0;
	}
	// assign a hash table array for Dictionary use, if apl
	if(tree)
	{
		// create tree structure
	}
	// assign a hash function and rehash function, whether used or not
	switch((BITMASK_HASH_PRIMARY & config) >> 16)
	{
		case 1: hashP = hash_01_even_sect; break;
		case 2: hashP = hash_02_sum_char_products; break;
		case 3: hashP = hash_03_base_128; break;
		case 0: 
		default: hashP = hash_00_char_sum;  
	}
	switch((BITMASK_HASH_SECONDARY & config) >> 24)
	{
		case 1: hashS = rehash_01_add_attempt_sq; break;
		case 2: hashS = rehash_02_add_attempt; break;
		case 0: 
		default: hashS = rehash_00_add_one;
	}
	new->hash_func = hashP;
	new->hash_second = hashS;
	return(new);
}

///
//	Gives a newly allocated Dictionary a hash fx
//	in:		
//			Dictionary * dn = dictionary target of initalization
//			float threshold = percentage of currSize-to-maxSize that triggers reallocation
//			float growth_factor = ratio of the oldTable-to-newTable when reallocating
//
void dictionary_initialize(Dictionary * dn, float threshold, float growth_factor)
{
	dn->threshold = threshold;
	dn->growth_factor = growth_factor;
}

///
//	Appends the dicitonaries statistics to the logfile
//
void dictionary_log_statistics(Dictionary * dn)
{
	//local variables
	FILE * log = dn->logfile;
	char * title = "HASH TABLE LOG";
	char * sect1 = "TABLE FIELDS";
	char * sect2 = "ENTRY COLLISIONS";
	char * sect3 = "SEARCH COLLISIONS";
	const time_t log_time;
	int i;
	char b = ' ';
	char banner = '_';
	int banner_len = 20;
	int has_table;
	int has_tree;

	//// HEADER
	// divider
	for(i = 0; i < banner_len; ++i)
		fprintf(log, banner);
	fprintf("\n");
	// title
	fprintf("%s%s\n", b, title);
	/// time stamp
	log_time = time(NULL);
	fprintf(log, "%s%s\n", b, ctime(&log_time));
	// divider
	for(i = 0; i < banner_len; ++i)
		fprintf(log, banner);
	fprintf("\n");

	//// TABLE FIELDS
	fprintf("\n%s%s\n", b, sect1);
	fprintf("\n%2s%16s%d\n", b, "max table size = ", dn->max_size);
	fprintf("\n%2s%16s%d\n", b, "current table size = ", dn->cur_size);
	fprintf("\n%2s%16s%6.3f\n", b, "table growth factor = ", dn->growth_factor);
	fprintf("\n%2s%16s%6.3f\n", b, "percent volume threshhold = ", dn->threshold);

	//// ENTRY COLLISIONS
	fprintf("\n%s%s\n", b, sect2);

	
	// divider
	for(i = 0; i < banner_l; ++i)
		fprintf(log, banner);
	fprintf("\n");

	
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
//	Finds a valid table index given a string key
//	in:		Dictionary * dn = where the data an be found
//			const char * str = entry to be probed
//	out:	int position = final resing place of string
//
int dictionary_probe_table(Dictionary * dn, const char * str)
{
	////LOCAL VARIABLES
	char ** data;// where the probing is concerned
	int collisions;// the number of attempts
	unsigned int position;// the final resting point 
	int max = dn->max_size;// the number of slots in the hash table
	int (*rehash)(const char * str, int max, int k, int attempt_n); 

	////EXECUTABLE STATEMENTS
	rehash = (dn->hash_second);
	data = (dn->hash_table);

	position = dn->hash_func(str, max);
	collisions = 0;

	//so long as there is data at the desired index, rehash
	while(data[position] != NULL && collisions < INTEGER_MAX)
	{	position = rehash(str, max, position, ++collisions);
	}
	//if there were more collisions than max position, signify
	if(collisions >= max)
	{	fprintf(stderr, "Excessive collisions.\nmax_size = %d, collisions = %d\n",
						dn->max_size, collisions);
	}
	//data tracking
	dn->t_entry_collisions += collisions;
	dn->avg_entry_collision = dn->cur_size / dn->t_entry_collisions;

	return(position);
}
	

///
//	Rehashes the data of the old dictionary to a new dictionary
//	in:		Dictionary * dn = curr_size/max_size > threshold
//	out:	Dictionary * new = curr/max(size) = threshold/growth_factor
//
Dictionary * dictionary_grow(Dictionary * dn)
{
	////LOCAL VARIABLES
	Dictionary * new;// grown table
	char ** data;// ptr at hash_table data
	char ** new_table;// the new hash table data
	int i;// for iterating through hash table
	int new_loc;// hash index for a str
	int (*hfunc)(const char* search, int max);// a ptr to the hash func
	int (*rfunc)(const char* search, int max, int probe_i, int attempt_i);// a ptr to the hash func
	int max;// max size that will be used
	int collisions;// a count of bad hashes for a str
	long save_ec;//
	long save_sc;//
	int save_search_n;//
	float save_avg_ec;//
	float save_avg_sc;//
	
	////EXECUTABLE STATEMENTS
	max = (int)(dn->max_size * dn->growth_factor);
	printf("\e[36mnew size = %d\e[0m\n", max);
	hfunc = dn->hash_func;
	rfunc = dn->hash_second;
	//save past table data
	save_ec = dn->t_entry_collisions;
	save_sc = dn->t_search_collisions;
	save_search_n = dn->t_hash_searches;
	save_avg_sc = dn->avg_search_collisions;
	save_avg_ec = dn->avg_entry_collisions;
	dictionary_log_statistics(dn);

	//clear data for new stats	
	dn->t_entry_collisions = 0;
	dn->t_search_collisions = 0;
	dn->t_hash_searches = 0;
	dn->avg_search_collisions = 0.0f;
	dn->avg_entry_collisions = 0.0f;

	//create new hash_table
	new_table = malloc(sizeof(char*) * max);
	for(i = 0; i < max; ++i)
	{
		new_table[i] = NULL;
	}

	//reshash any previous data (new table already initialized)
	data = (dn->hash_table);
	for(i = 0; i < dn->max_size; ++i)
	{
		//if there is an entry at this hash index:
		if(data[i] != NULL)
		{
			//hash the value
			new_loc = dictionary_probe_table(dn, data[i]);
			new_table[new_loc] = data[i];
		}
	}
	//manage data and reassign
	free(data);
	dn->hash_table = new_table;
	dn->max_size = max;
	return(dn);
}

///
//	Hashes a single string, preserving the ptr, into the dicitonary's hash table
//	in:		Dictionary * dn = storage for string
//			const char * str = entry being stored 
//
void dictionary_add_entry(Dictionary * dn, const char * str)
{
	//LOCAL VARIABLES
	int hash_val;// hash index for a str
	int (*hfunc)(const char* search, int max);// a ptr to the hash func
	float volume;// the hash table's percentage full 
	
	//EXECUTABLE STATEMENTS
	hfunc = dn->hash_func;
	
	//hash the str vale
	hash_val = dictionary_probe_table(dn, str);
	dn->hash_table[hash_val] = (char *)str;
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
	dn->t_hash_searches += 1;
	dn->t_search_collisions += collisions;
	dn->avg_search_collisions = dn->t_hash_searches / dn->t_search_collisions;

	dictionary_log_search();

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
/*Dictionary * dictionary_merge(Dictionary * favored, Dictionary * minor, float new_threshold)
{
	////LOCAL VARIABLES
	Dictionary * merged;// the combined dictionary
	char ** data;// ptr at hash_table data
	int i;// for iterating through hash table
	int new_loc;// hash index for a str
	int (*hfunc)(const char* search, int max);// a ptr to the hash func
	int max;// max size that will be used
	int collisions;// used for rehashing
	
	////EXECUTABLE STATEMENTS
	max = (int)(favored->max_size * favored->growth_factor);
	hfunc = favored->hash_func;
	//merged = dictionary_create(max, new_threshold, favored->growth_factor);
	//dictionary_initialize(hfunc, merged);
	
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

}*/

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

