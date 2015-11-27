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

///
//A quick 'n dirty implementation of the sdbm public domain hash. 
//Only for string keys
//
//Parameters:
//	key: The key to hash
//
//Returns:
//	The hashvalue of the key
//unsigned long Hash_SDBM(void* key, unsigned int keyLength)
int hash_04_nicks_sdbm(const char * str, int max_tbl_size)
{
	
	char* charKey = (char*) str;
	unsigned int keyLength = (unsigned) strlen(str);
	unsigned long hash = 0;
	unsigned char byteVal;
	unsigned int i;
	// int rVal;

	for(i = 0; i < keyLength; ++i)
	{
		byteVal = charKey[i];
		hash = byteVal + (hash << 6) + (hash << 16) - hash;
	}
		
	//rVal = (int)(hash % max_tbl_size);
	return (int)(hash % max_tbl_size);
	//return (rVal > 0)? rVal: -1 * rVal;

}	

int rehash_00_add_one(const char * str, int mts, int prev_hash, int attempt_n)
{	return((prev_hash + 1) % mts);	
}

int rehash_01_add_attempt_sq(const char * str, int mts, int prev_hash, int attempt_n)
{	return((prev_hash + attempt_n * attempt_n) % mts);
}

int rehash_02_add_attempt(const char * str, int mts, int prev_hash, int attempt_n)
{	return((prev_hash + attempt_n) % mts);
}


