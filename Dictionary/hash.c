#include "hash.h"

/* Hashing and Collision Control */
///
//	Sum char values as integers, modulo table size
//	in:		const char * str = data being hashed
//			int max_tbl_size = max not current table size
//	out:	int index = integer hash value
//
int hash_00_char_sum(const char * str, int max_tbl_size)
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
int	hash_01_even_sect(const char * str, int max_tbl_size)
{
	//LOCAL VARIABLES
	int len = strlen(str);
	uint32_t ind, hash;
	int section_width = (max_tbl_size / 12);// 12 is arbitrary divinding number

	//EXECUTABLE STATEMENTS
	hash = 0;
	// sum the chars
	for(ind = 0; ind < len; ++ind)
		hash += str[ind];
	// augment the section
	for(ind = 1; ind < len; ++ind)
		hash += section_width; 
	return(hash % max_tbl_size);
}


///
//	Sum char values as times char position, modulo table size
//	in:		const char * str = data being hashed
//			int max_tbl_size = max not current table size
//	out:	int index = integer hash value
//
int hash_02_sum_char_products(const char * str, int max_tbl_size)
{
	//LOCAL VARIABLES
	int len = strlen(str);
	int ind;
	long hash;
	//EXECUTABLE STATEMENTS
	hash = 0;
	for(ind = 0; ind < len; ++ind)
		hash += str[ind] * (ind + 1);
	return (int)(hash % max_tbl_size);
}

///
//	Multiplies  by a power of 128
//	in:		const char * str = data being hashed
//			int max_tbl_size = max not current table size
//	out:	int index = integer hash value
//
int hash_03_base_128(const char * str, int max_tbl_size)
{
	//LOCAL VARIABLES
	int len = strlen(str);
	int ind, val, pow;
	uint64_t hash;
	//EXECUTABLE STATEMENTS
	hash = 0;
	for(ind = 0; ind < len; ++ind)
	{
		val = 0;
		val = (int)str[ind] * (ind + 1);
		for(pow = 0; pow < ind; ++pow)
			val *= 128;
		hash += val;
	}
	return (int)(hash % max_tbl_size);
}


int rehash_00_add_one(const char * str, int mts, int prev_hash, int attempt_n)
{	return(prev_hash + 1);	
}

int rehash_01_add_attempt_sq(const char * str, int mts, int prev_hash, int attempt_n)
{	return(prev_hash + attempt_n * attempt_n);
}

int rehash_02_add_attempt(const char * str, int mts, int prev_hash, int attempt_n)
{	return(prev_hash + attempt_n);
}
