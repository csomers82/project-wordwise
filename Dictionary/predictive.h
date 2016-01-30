#ifndef PREDICTIVE_H
#define PREDICTIVE_H


#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include <curses.h>
#include <pthread.h>
#include <time.h>
//#include <ncurses.h>

#include "tree26.h"

/*============================================================*/
/* CONSTANTS */

/* SCREEN PROPERTIES */
#define COLS_PER_SCREEN	160
#define ROWS_PER_SCREEN 45
#define STANDARD_H_MARGIN 4
#define STANDARD_V_MARGIN 2

/* PROGRAM VALUES */
#define TRUE 1
#define FALSE 0
#define UNINIT -1
#define LOGTRUE 1U
#define LOGFALSE 0U
#define BLANK_CHAR	' '
#define N_EDIT_BOXES	2
#define EDIT_BOX_MAX	30
#define EDIT_BOX_HEIGHT	4
#define EDIT_1_LABEL	("Begin typing word here:")
#define	EDIT_1_X		10
#define	EDIT_1_Y		10
#define EDIT_1_COLOR	'b'
#define EDIT_1_WIDTH	(	strlen(EDIT_1_LABEL) + \
							EDIT_BOX_MAX + \
							1 + 4 + 4					)
#define EDIT_2_LABEL	("Restrict letters to:")
#define	EDIT_2_X		76
#define	EDIT_2_Y		10
#define EDIT_2_COLOR	'g'
#define EDIT_2_WIDTH	(	strlen(EDIT_2_LABEL) + \
							EDIT_BOX_MAX + \
							1 + 4 + 4					)
#define BOX_HORCH	L'_'
#define BOX_VERCH	L'|'
#define BOX_TLCH	L' '
#define BOX_TRCH	L' '
#define BOX_BLCH	L'|'
#define BOX_BRCH	L'|'
#define BFS_DEPTH	1
#define MAX_RESULTS	100
#define SCROLL_BEG	16
#define SCROLL_END	(ROWS_PER_SCREEN - 8)
#define SCROLL_SIZE_T (SCROLL_END - SCROLL_BEG)


/* CONTROL CODES */
#define CTRL_ADDCHAR	( 1)
#define CTRL_CLEAR		(-2)
#define CTRL_CURSOR		( 2)
#define CTRL_DONOTHING	( 0)
#define CTRL_DELCHAR	(-3)
#define CTRL_EXECUTE	( 4)
#define CTRL_EXIT		(-1)
#define CTRL_SCROLL		( 3)
#define CTRL_SELECT		( 5)
#define CTRL_SWITCH		( 6)
#define CTRL_UNDO		(-4)

/* DIAGNOSTICS */
//#define CHECKS
#ifdef CHECKS
	#define CHECKTXT(x) ({\
		move(20,20);\
		wscrl(WIN, -1);\
		printw("t. %s = \"%s\"", #x, x);\
	})
	#define CHECKPTR(x) ({\
		move(20,20);\
		wscrl(WIN, -1);\
		printw("&. %s = %p", #x, x);\
	})
	#define CHECKCODE(x) ({\
		move(20,20);\
		wscrl(WIN, -1);\
		printw(".. %s = %d", #x, x);\
	})
	#define CHECKSTACK(stack, i) ({\
		move(20,20);\
		wscrl(WIN, -1);\
		if(stack[i])\
		printw(":. %s[%2d] = %p:(%s)", #stack, i, stack[i], stack[i]->str );\
		else\
		printw(":. %s[%2d] = %p", #stack, i, stack[i]);\
	})
#else
	#define CHECKCODE(x) 
	#define CHECKSTACK(stack, i) 
#endif

/* ERROR HANDLING */
#define ERRORLOG "errors.log"
#define EC01 1
#define EC02 2
#define EC03 4
#define EC04 8
#define EC05 16
#define EC06 32
#define EC07 64
#define EC08 128
#define EC09 256
#define EC0A 512
#define EC0B 1024
#define EC0C 2048
#define EC0D 4096
#define EC0E 8192
#define EC0F 16384
#define EC10 32768
#define EC11 65536
#define EC12 131072
#define EC13 262144
#define EC14 524288
#define EC15 1048576
#define EC16 2097152
#define EC17 4194304
#define EC18 8388608

enum EboxNames {WORKSPACE, PARAMETERS};
enum Phase {EDIT, CONTROL, SELECT, HELP};

extern unsigned int ERROR;
extern unsigned int ERRQUIT;

extern WINDOW * WIN;
extern int GLOBAL_X;
extern int GLOBAL_Y;

/*============================================================*/
/* MACROS	 */

#define treeQueue_create(tq_ptr, t26_ptr) ({\
			TreeQueue * _NEWNODE	= malloc(sizeof(TreeQueue));\
			_NEWNODE->node			= t26_ptr;\
			_NEWNODE->next			= NULL;\
			tq_ptr = _NEWNODE;\
		})

#define text_position(textPtr, Y, X) ({\
			if( sizeof(X) == sizeof(int) )\
			{\
				if( sizeof(Y) == sizeof(int) )\
				{\
					if( sizeof(textPtr) == sizeof(void*) )\
					{\
						textPtr->posX = X;\
						textPtr->posY = Y;\
					}\
					else {\
						ERROR |= EC04;\
					}\
				}\
				else {\
					ERROR |= EC05;\
				}\
			}\
			else {\
				ERROR |= EC06;\
			}\
		})

#define text_toggle(textPtr, oneOrZero) ({\
			if( sizeof(textPtr) == sizeof(void*)) \
			{\
				if((oneOrZero == TRUE || oneOrZero == FALSE )) \
				{\
					textPtr->persistant = oneOrZero;\
				}\
				else {\
					ERROR |= EC09;\
				}\
			}\
			else {\
				ERROR |= EC08;\
			}\
		})

#define LOG(log, errmsg)	({\
			fprintf(stderr	, errmsg);\
			fprintf(log		, errmsg);\
		})

//#define DEBUG
#define SHOWFILE
#define COLOUR
#ifdef DEBUG
	#define STAFF fprintf(stderr, "\n================================")
	#define SSEP fprintf(stderr, "\n--------------------------------")
	#define SPACE fprintf(stderr, "\n\n")
	#ifndef COLOUR
		#define MISUSE fprintf(stderr, "\nError: improper use of '%s'", __func__)
		#define SHOWi(x) fprintf(stderr,"[%4s] = %d\n", #x, x)
		#define SHOWf(x) fprintf(stderr,"[%4s] = %f\n", #x, x)
		#define SHOWl(x) fprintf(stderr,"[%4s] = %Ld\n",#x, x)
		#define SHOWc(x) fprintf(stderr,"[%4s] = %c\n", #x, x)
		#define SHOWp(x) fprintf(stderr,"[%4s] = %p\n", #x, x)
		#define SHOWs(x) fprintf(stderr,"[%4s] = %s\n", #x, x)
	#else
		// {30-K, 31-R, 32-G, 33-Y, 34-B, 35-M, 36-C, 37-W}
		#define AA 32
		#define BB 37
		#define CC 33
		#define DD 35
		#define EE 31
		#define SHADE  34 
		#define SHADE2 34 
		#ifdef SHOWFILE
			#define FILINF fprintf(stderr, "\n\e[%dm%s", SHADE, __FILE__);
			#define WHERE FILINF fprintf(stderr, "\e[%dm%d\e[%dm {%s}==>\e[0m", SHADE2, __LINE__, SHADE, __func__);
		#else
			#define FILINF fprintf(stderr, "\n");
			#define WHERE FILINF fprintf(stderr, "\e[%dm%d\e[%dm {%s}==>\e[0m", SHADE2, __LINE__, SHADE, __func__);
		#endif	
		#define SHOWi(x) WHERE fprintf(stderr,"\e[%dm[%4s] = %d\e[0m", AA, #x, x);
		#define SHOWl(x) WHERE fprintf(stderr,"\e[%dm[%4s] = %Ld\e[0m", BB, #x, x);
		#define SHOWc(x) WHERE fprintf(stderr,"\e[%dm[%4s] = %c\e[0m", CC, #x, x);
		#define SHOWp(x) WHERE fprintf(stderr,"\e[%dm[%4s] = %p\e[0m", DD, #x, x);
		#ifndef FANCY
			#define SHOWs(x) WHERE fprintf(stderr,"\e[%dm[%4s] = %s\e[0m", EE, #x, x);
		#else
			#define SHOWs(x) ({\
				WHERE\
				int length = strlen(x);\
				char * checkthis = malloc((length + 3) * sizeof(char));\
				memset(checkthis, '\0', (length + 3) * sizeof(char));\
				sprintf(checkthis, "\"%s\"", x);\
				if(strcmp(#x, checkthis)) {\
					fprintf(stderr,"\e[%dm[%4s] = %s\e[0m", EE, #x, x);\
				}\
				else {\
					fprintf(stderr,"\e[%dm[%4s] = %s\e[0m", EE, "debug", x);\
				}\
				free(checkthis);\
			})
		#endif
	#endif
#else
	#define MISUSE
	#define STAFF
	#define SSEP
	#define SPACE
	#define SHOWi(x) 
	#define SHOWl(x)
	#define SHOWf(x)
	#define SHOWc(x)
	#define SHOWp(x)
	#define SHOWs(x) 
#endif

/*============================================================*/
/* STRUCTS */


typedef struct _win_st WINDOW;

/****
 *	MsgQueue Struct: is a basic LINKEDLIST (SLL) with a char*.
 *	It is used in the extraction of text from the dictionary
 *	input file.
 */ 
typedef struct MsgQueue {
	char *				line;
	struct MsgQueue *	next;
} MsgQueue;


/****
 *	TreeQueue Struct: is a basic LINKEDLIST (SLL) with a Tree26*.
 *	It is used in the program's breadth first search BFS
 */
typedef struct TreeQueue {
	Tree26 *			node;
	struct TreeQueue *	next;
} TreeQueue;



/****
 *	Text Struct: frames the position, color, and attributes of 
 *	curses text written to the screen 
 */
typedef struct Text {
	char *			string;		// characters to be printed	
	char 			foreground;	// color of the text 
	char * 			attributes;	// atrribute qualities of text
	int				posX;		// lefthand offset on screen
	int				posY;		// tophand offset on screen
	int				bool_nl;	// true/false add newline char
	int				hMargin;	// preserve horizontal space 
	int				vMargin;	// preserve vertical space 
	int				persistant;	// maintain
	struct Text *	next;		// nextset of text
} Text;
 

/****
 *	Editbox Struct: frames an instance of editable text entered
 *	by the user. Program as is contains 2.
 */
typedef struct Editbox {
	char *  label;				// static test label
	char	text[EDIT_BOX_MAX];	// text buffer by letter 
	int		max;				// max size of buffer
	int		index;				// which character the cursor is at
	int		text_x_orig;		// what coord the .text[0] resides
	int		text_y_orig;		// what coord the .text[0] resides
} Ebox;


/****
 *	Program Struct: Holds most important program values for
 *	availability accross functions.
 */
typedef struct Program {
	WINDOW *	wnd;			// at global WIN
	int *		ptrX;			// at global x
	int *		ptrY;			// at global y
	Tree26 *	tree;			// handle of tree26 root  node	
	Tree26 *	node;			// ptr to current tree26 node
	Tree26 **	pos_stack;		// ARRAY of node ptrs (ebox chars) 
	int			pos_index;		// index of offset in pos_stack
	Text *		queue_head;		// front of text output queue	
	Text *		queue_tail;		// attachement end of text dequeue
	Ebox *		ebox_array;		// ARRAY of edit boxes
	int			ebox_active;	// index of selected edit box
	Text **	    results_array;	// bfs results top scroll stack
	int		    results_index;	// index of offset in result array
	int		    results_limit;	// index of offset in result array
	int			loop_continue;	// loop control variable
	int			control_code;	// user input interpreted
	wchar_t		user_input;		// user input raw
	int			screen_height;	// window cursor heigths
	int			screen_width;	// window cursor widths
	enum Phase	phase;			// program "mode of conduct"
} Program;

/*============================================================*/
/*============================================================*/

/* FUNCTIONS */


/************************************************************* 
 * test function
 */
void test_main(void);
void test1(void);
void test2(void);
void test3(void);
void tree_test(void);
void test4(void);

/************************************************************* 
 *	A function that frees all of the memory that was assoc-
 *	-iated with the previous BFS. It DOES NOT free the pointer
 *	to the 'results_array'.
 *	file:
 *		paux.c
 *	args:
 *		Text **	results_ptr_array	- ptr array of vics
 *		int		limit	- how many results there are to clear
 *	returns:
 *		void
 */
void results_clear(Text ** results_ptr_array, int limit); 


/************************************************************* 
 *	Handles the text that is related to results. It is handled
 *	differently for effeciency and ease of separation.
 *	file:
 *		pmain.c
 *	args:
 *		Program * program: contains:
 *			Text **	results_array	- to be printed
 *			int		results_index	- when to start print
 *			int		results_limit	- when to end print
 *	returns:
 *		void
 */
void results_manager(Program * program);

/************************************************************* 
 *	Creates a Text pointer array for program use.
 *	file:
 *		paux.c
 *	args:
 *
 *	returns:
 *		Text ** results_array: ARRAY of first 100 returned res'
 */
Text ** results_init(void);



/************************************************************* 
 *	Takes the a TreeQueue list and puts the first one hundred 
 *	search results into the lines of a text object
 *	file:
 *		pmain.c
 *	args:
 *		Program * p: Will have the necessary value scope
 *		TreeQueue * resultList:	A queue of Tree26 node ptrs
 *	returns:
 *		Text * results_array: ARRAY of first 100 returned res'
 */
Text ** results_setup(Program * p, TreeQueue * resultList);



/************************************************************* 
 *	Creates a SLL of TreeQueue nodes that represents the first
 *	layer of words that can be found with a breadth first 
 *	search. 
 *	file:
 *		pmain.c
 *	args:
 *		Program * p: contains tree pointer (Program->node)
 *	returns:
 *		TreeQueue * tq: singally linked list of  results
 */
TreeQueue * tree26_bfs(Program * p);



/************************************************************* 
 *	Using the blank root node, grows a tree26 struct using 
 *	constant defined dictionary file. The return value should
 *	be the unedited address of the root node that now points
 *	to child nodes.
 *	file:
 *		paux.c
 *	args:
 *		Tree26 * root: blank string and unititialized node.
 *	returns:
 *		Tree26 * root: now has children in child ptr array
 */
Tree26 * manage_buffered_file_tree(Tree26 * root);


/*************************************************************
 *	Accepts a character and evaluates the correct program
 *	course of action.
 *	file:
 *		pmain.c
 *	args:
 *		Program * p: contains the values associated with i/o
 */
void handle_char(Program * p);

/************************************************************* 
 *	positions the cursor at the position denoted by the 
 *	active edit box.
 *	file:
 *		pmain.c
 *	args:
 *		Program * program: source of all cursor and ebox vars
 */
void cursor_reposition(Program * p);



/************************************************************* 
 *	clears all of the characters in the WORKSPACE, Ebox 
 *	struct, and in the Tree26 position stack.
 *	file:
 *		paux.c
 *	args:
 *		Program * p: holds stack, ebox, and positions
 *		enum EboxNames which: contains index of space to clear
 *	returns:
 *		void
 */
void ebox_clear(Program * p, enum EboxNames which);



/************************************************************* 
 *	allocates the program's edit boxes 
 *	file: 
 *		paux.c
 *	returns:
 *		Ebox ebox_array[N_EDIT_BOXES]:	an array of edit boxes
 */
Ebox * program_create_eboxes(void);



/************************************************************* 
 *	deallocates the program's edit boxes 
 *	file: 
 *		paux.c
 *	returns:
 *		Ebox ebox_array[N_EDIT_BOXES]:	an array of edit boxes
 */
void program_destroy_eboxes(Ebox * vicArray);




/************************************************************* 
 *	allocates texts objects for a box.
 *	file:
 *		paux.c
 *	args:
 *		int			y:			y origin
 *		int			x:			x origin
 *		int			width:		number of columns
 *		int			height:		number of rows
 *		int			color:		color
 *		text *		tail:		tail of text queue
 *	returns:
 *		text *		newtail:	tail location after appending
 */
Text * build_box(int x,
				 int y,
				 int width,	
				 int height,		
				 char color,
				 Text * tail);
 

/************************************************************* 
 *	Allocates a program object to store important program 
 *	values
 *	file:
 *		paux.c
 *	returns:
 *		Program * new: contains major program values
 */
Program * program_create();

/************************************************************* 
 *	Allocates a program object to store important program 
 *	values
 *	file:
 *		paux.c
 *	returns:
 *		Program * new: contains major program values
 */
void program_destroy(Program * vic);


/************************************************************* 
 *	Takes the global ERROR value and uses to print the error
 *	diagnosis to stderr and the error logfile. 
 *	file:
 *		paux.c
 *	args:
 *		int ERRORCOPY: pass the value of ERROR
 *	returns:
 *		int ENDPROGRAM: bool to end program
 */
int programErrorOut(int ERRORCOPY);


/************************************************************* 
 *	Takes the global ERROR value and uses to print the error
 *	diagnosis to stderr and the error logfile. 
 *	file:
 *		paux.c
 *	args:
 *		Text ** head: ptr to headptr node
 *	returns:
 *		Text * tail: ptr to tailptr node
 */
Text * build_title(Text ** h);

/************************************************************* 
 *	Initializes the curses library functionality and creates	
 *	a window that is WIDTH by HEIGHT in dimensions
 *	file:
 *		pmain.c
 *	args:
 *		int * WIDTH - global variable representing screen width
 *		int * HEIGHT - global variable representing screen height
 *	returns:
 *		WINDOW * win - curses sp:wqecific opaque window object
 */
WINDOW * window_0(int * WIDTH, int * HEIGHT);

/*************************************************************
 *	Evaluates a keyboard input and returns the appropriate 
 *	control code for program redirection
 *	file:
 *		paux.c
 *	args:
 *		wchar_t	userKey - the input
 *	returns:
 *		int		ctrlCode - redirects the program loop
 */
int input_eval(wchar_t userKey);

/************************************************************* 
 *	Creates text object to be handled in printing
 *	file:
 *		paux.c	
 *	args:
 *		void *	string	- text that the object represents
 *		char	fore	- foreground or standard if space character
 *		Text *	tail	- end of the text queue or NULL
 *	returns:
 *		Text *	new		- ptr to new heap allocated Text obj
 */
Text * text_create(void * string, char fore, Text * tail);

 
/************************************************************* 
 *	Takes a text object and executes the screen printing. This
 *	is a single instance of a print function and calls curses
 *	library attribute and print functions
 *	file:
 *		pmain.c	
 *	args:
 *		Text *	output	- node of the text queue 
 */
void text_print(Text * t);


/************************************************************* 
 *	Takes a ptr at a text object ptr and frees the data. 
 *	file:
 *		paux.c	
 *	args:
 *		Text **	vic	- node of the text queue 
 */
void text_destroy(Text ** vic);


/* void text_position(text, x, y): MACRO: assigns text coords */


/************************************************************* 
 *	Takes a text object QUEUE and executes the screen printing
 *	The entire Text obj queue will be printed and if not desig
 *	-nated to be persistant, will be free'd. This is a higher
 *	level function than the actual print function, and calls
 *	text_print for each queue node.
 *	file:
 *		pmain.c	
 *	args:
 *		Text ** tHead	- ptr to the head of the queue
 *		Text **	tTail	- ptr to the tail of the queue
 */
void text_manager(Text ** tHead, Text ** tTail);

/************************************************************* 
 *	Takes the text queue and clears all of its memort for
 *	program exit. All queue nodes will be clear regradless of
 *	presistance state.
 *	file:
 *		pmain.c	
 *	args:
 *		Text *	text queue 
 */
void text_clear_all(Text * vic);


/************************************************************* 
 *	Opens an input fd to read char data from.
 *	file:
 *		paux.c
 *	args:
 *		char * filename: the filename which is meant to opened
 *	returns:
 *		FILE* file
 */
FILE * open_file(char * filename);

/************************************************************* 
 *	Opens an input fd to read char data from.
 *	file:
 *		paux.c
 *	args:
 *		char * filename: the filename which is meant to opened
 *	returns:
 *		FILE * file
 */
FILE * open_output_file(char * filename);


#endif
