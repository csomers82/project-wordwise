#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <pthread.h>
#include <time.h>
//#include <ncurses.h>

#ifndef PREDICTIVE_H
#define PREDICTIVE_H

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
#define LOGFALSE 1U

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



extern unsigned int ERROR;
extern unsigned int ERRQUIT;

extern WINDOW * WIN;
extern int GLOBAL_X;
extern int GLOBAL_Y;

/*============================================================*/
/* MACROS	 */
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


#define text_destroy(textPtr) ({\
			if( sizeof(textPtr) == sizeof(void*) ) \
			{\
				if( textPtr->persistant == FALSE ) \
				{\
					free(textPtr->string);\
					free(textPtr->attributes);\
					free(textPtr);\
				}\
			}\
			else {\
				ERROR |= EC07;\
			}\
		})

#define LOG(log, errmsg)	({\
			fprintf(stderr	, errmsg);\
			fprintf(log		, errmsg);\
		})

#define DEBUG
#ifdef DEBUG
	#define MISUSE fprintf(stderr, "\nError: improper use of '%s'", __func__)
	#define STAFF fprintf(stderr, "\n================================")
	#define SSEP fprintf(stderr, "\n--------------------------------")
	#define SHOWi(x) fprintf(stderr,"[%4s] = %d\n", #x, x)
	#define SHOWf(x) fprintf(stderr,"[%4s] = %f\n", #x, x)
	#define SHOWl(x) fprintf(stderr,"[%4s] = %Ld\n",#x, x)
	#define SHOWc(x) fprintf(stderr,"[%4s] = %c\n", #x, x)
	#define SHOWp(x) fprintf(stderr,"[%4s] = %p\n", #x, x)
	#define SHOWs(x) fprintf(stderr,"[%4s] = %s\n", #x, x)
#else
	#define MISUSE
	#define STAFF
	#define SSEP
	#define SPACE(x)
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
 


/*============================================================*/
/* FUNCTIONS */


/************************************************************* 
 * test function 
 */
void test1(void);
void test2(void);
void tree_test(void);



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
 *		void
 *	returns:
 *		Text * titleQueue: contains the title lines
 */
Text * build_title(void);

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
 *	Creates text object to be handled in printing
 *	file:
 *		pmain.c	
 *	args:
 *		char *	string	- text that the object represents
 *		char	fore	- foreground or standard if space character
 *		Text *	tail	- end of the text queue or NULL
 *	returns:
 *		Text *	new		- ptr to new heap allocated Text obj
 */
Text * text_create(char * string, char fore, Text * tail);


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

/* void text_destroy(text): MACRO: frees single text object */
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
 *		Text *	outputQueue	- node of the text queue 
 */
void text_manager(Text * t);




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
