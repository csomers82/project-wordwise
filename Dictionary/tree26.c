
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
int tree26_isempty(Tree26 * root)
{
	int i;// for iterating through the branches
	for(i = 0; i < N_BRANCHES; ++i)
	{	if(root->branch[i]) 
		{
			return FALSE;
		}
	}
	return TRUE;
}

/***
 *	Frees the data associated with the given tree
 *	in:		Tree26 * vic: will be free'd
 *	out:	void
 */
void tree26_destroy(Tree26 * vic)
{
	////LOCAL VARIABLES
	int depth;// depth index for the tree
	int br_i;// branch number of N_BRANCHES
	int lbi;// for an easier reference during popping procedure
	Tree26 * curr;// current node in question
	Tree26 * last;// for easier reference
	Tree26 stack[20];// the storage for the previous node "CLRS.pdf"
	int bri_stack[20];// stores the previous node branch indices in parallel
	int reset;// control flag
	
	////EXECUTABLE STATEMENTS
	curr = vic;
	depth = 0;
	br_i = 0;
	stack[depth] = curr;
	//FIX SO THAT FIRST NODE IS STACK
	while(!tree26_isempty(stack[depth]))
	{
		reset = FALSE;
		//check each branch of the current node
		while(!reset && br_i < N_BRANCHES)
		{
			///case_0: branch has children - push node
			if(curr->branch[br_i] &&
			  (!tree26_isempty(curr->branch[br_i])))
			{
				//adjust cur branch index in current stack, push on next
				bri_stack[depth] = br_i;
				++depth;
				stack[depth] = curr;
				//set cur to new root just found
				curr = curr->branch[br_i];
				//reset br_i, will become 0
				br_i = -1;
				reset = TRUE;
			}
			++br_i;
		}
		///case_1: branch is leaf - pop node
		if(!reset && tree26_isempty(curr))
		{
			//free node data
			free(curr->str);
			free(curr->branch);
			free(curr);
			// deattach last link (not necessary?) 
			if(depth > 0)
			{
				last = stack[depth - 1];
				lbi = bri_stack[depth - 1];
				last->branch[lbi] = NULL;
			}
			--depth;
			curr = stack[depth];
			br_i = bri_stack[depth];
		}
	}
	
}


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
