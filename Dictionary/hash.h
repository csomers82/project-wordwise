
#ifndef HASH_FUNCTIONS
#define HASH_FUNCTIONS

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>




/* Hashing and Collision Control 
 *	
 *	Common Parameters:
 *		const char * str = data being hashed
 *		int max_tbl_size = max not current table size
 *
 *	Common Return Value:
 *		int index = integer hash value
 *
 **/


//	in:		const char * str = data being hashed
//			int max_tbl_size = max not current table size
//	out:	int index = integer hash value



/*
 *	1]	Sum char values as integers
 *	2]	Modulo table size
 */
int hash_00_char_sum(const char * str, int max_tbl_size);

/*
 *	1]	Sum char values as integers
 *	2]	Add Special Value
 *			Divide max_tbl_size by number of desired equal-sized sections -> A
 *			Multiple (A) with section width
 *	3]	Modulo max_tbl_size
 */
int	hash_01_even_sect(const char * str, int max_tbl_size);

/*
 *	*]	For each character
 *	1]	Multiple char position in str by char value -> A
 *	2]*	Add (A) to hash sum.  * end loop
 *	3]	Modulo max_tbl_size
 */
int hash_02_sum_char_products(const char * str, int max_tbl_size);

/*
 *	*]	For each character
 *	1]	Multiple char pos-in-str by char val -> A
 *	2]	Multiple (A) by 128 ^ pos-in-str power -> A
 *	3]*	Add (A) to hash sum. * end loop
 *	4]	Modulo max_tbl_size
 */
int hash_03_base_128(const char * str, int max_tbl_size);



int rehash_00_add_one(const char * str, int mts, int prev_hash, int attempt_n); 
int rehash_01_add_attempt_sq(const char * str, int mts, int prev_hash, int attempt_n); 
int rehash_02_add_attempt(const char * str, int mts, int prev_hash, int attempt_n); 


#endif
