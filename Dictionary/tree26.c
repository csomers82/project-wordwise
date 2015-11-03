
#include <stdio.h>
#include <stdlib.h>
#include "tree26.h"

/*
typedef struct Tree26 {
	char * str;// section of string present
	struct Tree26 * branch[N_BRANCHES];// a child branch for each next availible letter
	int	bool_complete;// TRUE or FALSE, is the string a valid string 
	int n_chars;// the current strlen()
} Tree26;
*/

//==============================================================================
/*@* TREE26 FUNCTIONS *@*/

/***
 *	Allocates and initializes the values of a 26 tree struct
 *	in:		void
 *	out:	Tree26 * new: a bare Tree26 struct
 */
Tree26 * tree26_create()
{
	/// ALLOCATE THE POINTER VARIABLES
	Tree26 * sprout = malloc(sizeof(Tree26));
	sprout->branch = malloc(sizeof(Tree26*) * N_BRANCHES);
	int branch;
	for(branch = 0; branch < N_BRANCHES; ++branch)
	{	// assign the children all to (NULL)
		sprout->branch[branch] = NULL;
	}
	sprout->n_chars = 0;
	sprout->bool_complete = FALSE;
	// assign root string to empty
	sprout->str = strdup("");
	return sprout;
}

/***
 *	Inserts a value into the tree at the appropriate location
 *	in:		Tree26 * root: tree source location
 *			const char * store:	new value coming into the tree
 *	out:	void
 */
void tree26_insert(Tree26 * root, const char * store)
{
	////LOCAL VARIABLES
	char current;// referenced char at *store
	Tree26 * tree;// the current head of the search effort


	////EXECUTABLE STATEMENTS
}

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
