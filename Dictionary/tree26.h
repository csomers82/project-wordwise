#ifndef TREE26
#define TREE26

#include <string.h>

//==============================================================================
/*@* GLOBAL CONSTANTS *@*/
#define MAX_EXPECTED_WORD_LENGTH 30
#define N_BRANCHES 27
#define TRUE 1
#define FALSE 0

//==============================================================================
/*@* STRUCTURES *@*/
/*** 
 *	A Structure that implements high performance predictive text software
 */ 
typedef struct Tree26 {
	char * str;// section of string present
	struct Tree26 ** branch;// a child branch for each next availible letter
	int	bool_complete_low;// TRUE or FALSE, is string a valid word 
	int	bool_complete_cap;// TRUE or FALSE, is string a proper noun 
	int n_chars;// the current strlen()
} Tree26;

//==============================================================================
/*@* TREE26 MACROS *@*/
#define ADJUST(chr) ((((int)chr >= 65) && ((int)chr <= 90)) ? ((int)chr - 65): \
					((((int)chr >= 97) && ((int)chr <= 122)) ? ((int)chr - 97): \
					((int)chr == 39)? 26: -1))
#define BRANCH(tr26, chr) (tr26->branch[ADJUST(chr)])


//==============================================================================
/*@* TREE26 FUNCTIONS *@*/

/***
 *	Allocates and initializes the values of a 26 tree struct
 *	in:		void
 *	out:	Tree26 * new: a bare Tree26 struct
 */
Tree26 * tree26_create();

/***
 *	Inserts a value into the tree at the appropriate location
 *	in:		Tree26 * root: tree source location
 *			const char * store:	new value coming into the tree
 *	out:	void
 */
Tree26 * tree26_insert(Tree26 * root, const char * store);


/***
 *	Takes a string with special/extended ascii table values and
 *	attempt to interpret them into a newly allocated string.
 *	Upon failure, NULL is returned. On success, a copy of the 
 *	psuedo-copy of the string is returned.
 *	in:		char * onsertion_entry:	assumed non-NULL, containing sp. char
 *	out:	char * interpreted: NULL or 0-128 ASCII values
 */
char * tree26_simplify_string(char * insertion_entry);


/***
 *	Returns a int equivalent boolean whether the root has children or not
 *	in:		Tree26 * root: tree source location
 *	out:	int isempty: is 1 if empty or 0 if not empty
 */
int tree26_isempty(Tree26 * root);

/***
 *	Frees the data associated with the given tree
 *	in:		Tree26 * vic: will be free'd
 *	out:	void
 */
void tree26_destroy(Tree26 * vic);

/***
 *	Removes a single node from the tree *obselete*
 *	in:		Tree26 * root: tree source data
 *			const char * rem_ref: string of node to be removed 
 *	out:	void
 */
void tree26_remove(Tree26 * root, const char * rem_ref);

/***
 *	Find the branch node that contains the reference string
 *	in:		Tree26 * root: tree source data
 *			const char * srk_ref: string of node being found
 *	out:	Tree26 * node: the root branch containing srk_ref
 */
Tree26 * tree26_search(Tree26 * root, const char * srk_ref);

#endif
