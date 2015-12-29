
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
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
	sprout->branch = (struct Tree26 **) malloc(sizeof(Tree26*) * N_BRANCHES);
	int branch;
	for(branch = 0; branch < N_BRANCHES; ++branch)
	{	// assign the children all to (NULL)
		sprout->branch[branch] = NULL;
	}
	sprout->n_chars = 0;
	sprout->bool_complete = FALSE;
	// assign root string to empty
	sprout->str = NULL;//strdup("");
	return sprout;
}

/***
 *	Inserts a value into the tree at the appropriate location
 *	Implements the insertion with an iterative algoritm.
 *
 *	in:		Tree26 * root: tree source location
 *			const char * savee:	new value coming into the tree
 *	out:	void
 */
Tree26 * tree26_insert(	Tree26 * root, 
						const char * savee)
{
	////LOCAL VARIABLES
	int str_i;// index of string being stored
	int bi;//index of branch in question
	int length;// length of string savee
	char buffer[32];// the savee built letter by letter
	Tree26 * tree;// the current head of the search effort
	Tree26 * past;// the next branch for travel
	Tree26 * sprout;// a new child node

	////EXECUTABLE STATEMENTS

	//check for incompatible characters in the string (tree protection)
	for( length = 0; length < strlen(savee); ++length)
	{
		if(ADJUST(savee[length]) < 0)
		{
			fprintf(stderr, "Error: unexpected symbol\n");
			fprintf(stderr, "savee  = %s\n", savee);
			fprintf(stderr, "index  = %d\n", length);
			fprintf(stderr, "\n");
			free((char *) savee);
			return(root);
		}
	}
	//primary initialization
	memset(buffer, '\0', sizeof(buffer));
	str_i = 0;
	past = root;
	tree = BRANCH(root, savee[str_i]); 
	length = strlen(savee);
	//printf("savee = %s\n", savee);
	//printf("root = %p\n", root);
	
	//if/ tree value is not the final tree value 
	while((str_i < length))// && (str_i < 32)) 
	{
		// add the next letter to the node definition: buffer
		buffer[str_i] = savee[str_i];
		//printf("index = %2d,    buffer = %s\n", str_i, buffer);
		//printf("\t\t\t\tstr_i = %d\n", str_i);
			
		//if/ node is not null: continue traversal
		if( tree )
		{
			past = tree;
			// final destination node, validate string
			if(str_i + 1 == length)
			{
				tree->bool_complete = TRUE;
				int res = strcmp(savee, tree->str);
				if( !res )
				{
					free(tree->str);
					tree->str = (char *) savee;
					printf("Existing word validated.\n");
					return(root);
				}
				else 
				{
					if( res > 0 )
					{
						free(tree->str);
						tree->str = (char *) savee;
					}
					else 
					{
						free( (char *) savee);
					}
					fprintf(stderr, "Error: target-position / string-definition mismatch\n");
					fprintf(stderr, "savee    = .%s.\n", savee);
					fprintf(stderr, "tree-str = .%s.\n", tree->str);
					fprintf(stderr, "\n");
					return(root);
				}
			}
			++str_i;
			tree = BRANCH(tree, savee[str_i]);
		}
		else// is null: create node, place intermediate value
		{
			sprout = tree26_create();
			sprout->n_chars = str_i + 1;

			bi = ADJUST(savee[str_i]);
			past->branch[bi] = sprout;
			
			past = sprout;
			++str_i;

			// final pass
			if(str_i == length)
			{
				sprout->str = (char *) savee;
				sprout->bool_complete = TRUE;
			}
			else
			{
				sprout->str = strdup(buffer);
				tree = BRANCH(sprout, savee[str_i]);
			}
		}
		//printf("\t\t\t\tstr_i = %d\n", str_i);
	}
	//printf("\t\t\t\tstr_i = %d\n", str_i);
	
	
	if(str_i >= 32)
	{
		printf("Error: fault in loop logic\n");
	}
	return(root);

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
	Tree26 * stack[20];// the storage for the previous node "CLRS.pdf"
	int bri_stack[20] = {0};// stores the previous node branch indices in parallel
	int reset;// control flag
	unsigned int bogus = 0u;
	
	////EXECUTABLE STATEMENTS
	curr = vic;
	depth = 0;
	br_i = 0;
	stack[depth] = curr;
	//FIX SO THAT FIRST NODE IS STACK
	while(!tree26_isempty(stack[depth]) && bogus < 200000u)
	{
		bogus += 1;
		reset = FALSE;
		//check each branch of the current node
		for(br_i = bri_stack[depth] + 1; br_i < N_BRANCHES; ++br_i)
		{
			///case_0: branch has children - push node
			if(curr->branch[br_i] )//&& (!tree26_isempty(curr->branch[br_i])))
			{
				//save current stack level details
				bri_stack[depth] = br_i;
				stack[depth] = curr;

				//push on next node
				++depth;
				curr = curr->branch[br_i];
				reset = TRUE;
				break;
			}
		}
		///case_1: branch is leaf - pop node
		if(!reset && (br_i == N_BRANCHES))//&& tree26_isempty(curr))
		{
			//free node data
			free(curr->str);
			free(curr->branch);
			free(curr);
			//pop stack data
			--depth;
			curr = stack[depth];
			br_i = bri_stack[depth];
			curr->branch[br_i] = NULL;
			
			// deattach last link (not necessary?) 
			if(depth > 0)
			{
				last = stack[depth - 1];
				lbi = bri_stack[depth - 1];
				last->branch[lbi] = NULL;
			}
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

