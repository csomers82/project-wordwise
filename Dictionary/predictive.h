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
#define COLS_PER_SCREEN	160
#define ROWS_PER_SCREEN 45

#define STANDARD_H_MARGIN 2
#define STANDARD_V_MARGIN 4
#define TRUE 1
#define FALSE 0
#define UNINIT -1

#define EC01 1
#define EC02 2
#define EC03 4
#define EC04 8



extern int ERROR;

/*============================================================*/
/* MACROS	 */
#define text_position(textPtr, X, Y) ({\
			if( sizeof(X) == sizeof(int) )\
			{\
				if( sizeof(Y) == sizeof(int) )\
				{\
					if( sizeof(textPtr) == sizeof(void*) )\
					{\
						textPtr->posX = X;\
						text->posY = Y;\
					}\
					else {\
						ERROR = TRUE;\
					}\
				}\
				else {\
					ERROR = TRUE;\
				}\
			}\
			else {\
				ERROR = TRUE;\
			}\
		})

#define text_toggle(textPtr) ({\
			if(( sizeof(textPtr) == sizeof(void*)) && \
			   ( textPtr->persistant == TRUE || \
			     textPtr->persistant == FALSE )) \
			{\
				textPtr->persistant = (textPtr->persistant) \
					? FALSE : TRUE;\
			}\
			else {\
				ERROR = TRUE;\
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
				ERROR = TRUE;\
			}\
		})

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
	struct Text *	next;		// next set of text
} Text;



/*============================================================*/
/* FUNCTIONS */

/************************************************************* 
 *	Initializes the curses library functionality and creates	
 *	a window that is WIDTH by HEIGHT in dimensions
 *	file:
 *		predictive.c
 *	args:
 *		int * WIDTH - global variable representing screen width
 *		int * HEIGHT - global variable representing screen height
 *	returns:
 *		WINDOW * win - curses specific opaque window object
 */
WINDOW * window_0(int * WIDTH, int * HEIGHT);


/************************************************************* 
 *	Creates text object to be handled in printing
 *	file:
 *		predictive.c	
 *	args:
 *		char *	string	- text that the object represents
 *		char	fore	- foreground or standard if space character
 *		Text *	tail	- end of the text queue or NULL
 *	returns:
 *		Text *	new		- ptr to new heap allocated Text obj
 */
Text * text_create(char * string, char fore, Text * tail);


/************************************************************* 
 *	Takes a text object and executes the screen printing
 *	file:
 *		predictive.c	
 *	args:
 *		Text *	output	- node of the text queue 
 */
void text_print(Text * t);

/* void text_destroy(text): MACRO: frees single text object */
/* void text_position(text, x, y): MACRO: assigns text coords */


#endif
