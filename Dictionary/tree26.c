
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <wchar.h>
#include <assert.h>
#include "tree26.h"
#include "predictive.h"//ONLY NECESSARY TO PROJECT WORDFIGURE
					   //FOR DEBUG STATEMENT AVAILABILITY

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
	sprout->bool_complete_low = FALSE;
	sprout->bool_complete_cap = FALSE;
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
			/*
			fprintf(stderr, "index  = %d\n", length);
			fprintf(stderr, "\n");
			//fprintf(stderr, "Error: unexpected symbol\n");
			//fprintf(stderr, "%s\n", savee);
			*/
			char * attempt = NULL;// attempt to simplify the entry
			//printf("%s\n", (char*)savee);
			//attempt = tree26_simplify_string((char *) savee);
			free((char *) savee);
			if(attempt) break;
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
				int res = strcmp(savee, tree->str);
				if( !res )
				{
					tree->bool_complete_low = TRUE;
					//free(tree->str);
					//tree->str = (char *) savee;
					printf("Existing word validated.\n");
					return(root);
				}
				else 
				{
					//fprintf(stderr, "Error: target-position / string-definition mismatch\n");
					//fprintf(stderr, "savee    = .%s.\n", savee);
					//fprintf(stderr, "tree-str = .%s.\n", tree->str);
					if( res > 0 )
					{
						//fprintf(stderr, "l\n");
						tree->bool_complete_low = TRUE;

						free(tree->str);
						tree->str = (char *) savee;
					}
					else 
					{
						tree->bool_complete_cap = TRUE;
						//fprintf(stderr, "U\n");
						//free( (char *) savee);
					}
					//fprintf(stderr, "\n");
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
				if(savee[0] >= 96 && savee[0] <= 122) 
				{
					sprout->bool_complete_low = TRUE;
				}
				else {
					sprout->bool_complete_cap = TRUE;
				}

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
 *	Takes a string with special/extended ascii table values and
 *	attempt to interpret them into a newly allocated string.
 *	Upon failure, NULL is returned. On success, a copy of the 
 *	psuedo-copy of the string is returned.
 *	in:		char * onsertion_entry:	assumed non-NULL, containing sp. char
 *	out:	char * interpreted: NULL or 0-128 ASCII values
 */
char * tree26_simplify_string(char * insertion_entry)
{
	////LOCAL VARIABLES
	int cindex, sindex, windex;
	int ascii_i;
	int n_sp_characters = 8;
	int length = strlen(insertion_entry);
	wchar_t * special_char_array[8] = {
		L"áäâ",
		L"éèê",
		L"óöô",
		L"üû"
		L"ç",
		L"í",
		L"ñ",
		L"Å"
	}; 
	int wcalens[8] = {3, 3, 3, 2, 1, 1, 1, 1};
		/* { "\303\ ä,   â },
		{ é,   è,   ê},
		{ ó ,  ö ,  ô},
		{ü , û},
		{ç },
		{í },
		{ñ },
		{Å }*/
	//wchar_t * input = malloc(sizeof(wchar_t) * (length + 1));
	char * interpreted_array = "aeoucinA";
	char * interpreted = malloc(sizeof(char) * (length + 1));
	int bool_okay = 1;

	////EXECUTABLE STATMENTS
	//mbsrtowcs(input, &insertion_entry, length, NULL); 
	for(cindex = 0; cindex < length; ++cindex)
	{
		//set intpr. value for now, extract char as int
		interpreted[cindex] = insertion_entry[cindex];
		ascii_i = (int) insertion_entry[cindex];

		//check for character outside of bounds !(A-Z,a-z, or ')
		if(	!(	(ascii_i >= 65 && ascii_i <= 90) ||
				(ascii_i >= 96 && ascii_i <= 122)   ) )
		{
			//allocate tools for conversion
			char bytes[2];
			bytes[0] = insertion_entry[cindex];
			bytes[1] = insertion_entry[cindex + 1];
			wchar_t converted = (wchar_t) 0;
			mbrtowc(&converted, (bytes), sizeof(char) * 2, NULL); 

			printf("wc = %C\n", converted);
			//for all of the possible sp.ch.'s, check against each
			bool_okay = 0;
			for(sindex = 0; sindex < n_sp_characters; ++sindex)
			{
				//sp.ch. match was found, break and loop
				printf("sca[si] = %S\n", special_char_array[sindex]);
				for(windex = 0; windex < wcalens[sindex]; ++windex)
				{
					printf("wc = %C \n", special_char_array[sindex][windex]);
					if(special_char_array[sindex][windex] == converted)
					{
						interpreted[cindex] = interpreted_array[sindex];
						bool_okay = 1;
						break;
					}
				}
			}
			//out of bound character cannot be simplified; fail
			if( !bool_okay )
			{
				free(interpreted);
				return(NULL);
			}
		}
	}

	return(interpreted);
	
	//return(NULL);
}

/***
 *	Returns a int equivalent boolean whether the root has children or not
 *	in:		Tree26 * root: tree source location
 *	out:	int isempty: is 1 if empty or 0 if not empty
 */
int tree26_isempty(Tree26 * root)
{
	if(!root)
	{	return(FALSE * 2);
	}
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
	Tree26 * stack[MAX_EXPECTED_WORD_LENGTH];// the storage for the previous node "CLRS.pdf"
	int bri_stack[MAX_EXPECTED_WORD_LENGTH] = {0};// stores the previous node branch indices in parallel
	int reset;// control flag
	unsigned int bogus = 0u;
	Tree26 ** child_array = NULL;
	
	////EXECUTABLE STATEMENTS
	curr = vic;
	depth = 0;
	br_i = 0;
	stack[depth] = curr;
		//STAFF;
		//SHOWp(stack[depth]);
		//SHOWp(curr);
		//SHOWs(curr->str);
		//SHOWi(depth);
		//SHOWi(bri_stack[depth]);
	//FIX SO THAT FIRST NODE IS STACK
	//while(!tree26_isempty(stack[depth]) && bogus < 200000u)
	while(depth >= 0 && bogus < 200000u)
	{
		
		bogus += 1;
		reset = FALSE;
		//check each branch of the current node
		//SSEP;
		for(br_i = bri_stack[depth]; br_i < N_BRANCHES; ++br_i)
		{
			//SHOWi(br_i);
			//SHOWp(curr->branch[br_i]);
			///case_0: branch has children - push node
			if(curr->branch[br_i] )//&& (!tree26_isempty(curr->branch[br_i])))
			{
				//SHOWs("Branch Found, depth increasing");
				//save current stack level details
				bri_stack[depth] = br_i;

				//push on next node
				++depth;
				curr = curr->branch[br_i];
				stack[depth] = curr;
				reset = TRUE;
				break;
			}
			//free(curr->branch[br_i]);
		}
		///case_1: branch is leaf - pop node
		if(!reset && (br_i == N_BRANCHES))//&& tree26_isempty(curr))
		{
			//SSEP;
			//SHOWs("Leaf Node Case: no reset, all branches NULL");
			//free node data
			free(curr->str);
			child_array = (*curr).branch;
			free(child_array);
			free(curr);

			stack[depth] = NULL;
			bri_stack[depth] = 0;

			//pop stack data
			--depth;
			
			if(depth >= 0) 
			{
				//update references
				curr = stack[depth];
				br_i = bri_stack[depth];
				//disconnect node (absolutely necessary)
				curr->branch[br_i] = NULL;
			}
			if(depth > 0)
			{
				last = stack[depth - 1];
				lbi = bri_stack[depth - 1];
				last->branch[lbi] = NULL;
			}
		}
		//SPACE;
		//STAFF;
		//SHOWp(stack[depth]);
		//SHOWp(curr);
		//SHOWs(curr->str);
		//SHOWi(depth);
		//SHOWi(bri_stack[depth]);
	}
	//SPACE;	
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

