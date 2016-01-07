#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <time.h>
#include "predictive.h"
#include </usr/include/curses.h>
//#include <assert.h>
//#include </usr/include/ncurses.h>
//#include <curses.h>
#include <wchar.h>
#include "dictionary.h"
#include "tree26.h"


///* GLOBAL VARIABLES *///

unsigned int	ERROR;
WINDOW *		WIN;
int				GLOBAL_X;
int				GLOBAL_Y;


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
Text * text_create(char * string, char fore, Text * tail)
{
	//CREATE NEW TEXT OBJECT
	Text * new = malloc(sizeof(Text));
	if(tail) tail->next = new;
	
	//INITIALIZE VALUES
	new->string = string;
	new->foreground = fore;
	new->attributes = NULL;
	new->posX = UNINIT;
	new->posY = UNINIT;
	new->bool_nl = TRUE;
	new->hMargin = STANDARD_H_MARGIN;
	new->vMargin = STANDARD_V_MARGIN;
	new->next = NULL;
	new->persistant = FALSE;
	return(new);
}



/************************************************************* 
 *	Initializes the curses library functionality and creates	
 *	a window that is WIDTH by HEIGHT in dimensions
 *	args:
 *	int * WIDTH - global variable representing screen width
 *	int * HEIGHT - global variable representing screen height
 *	returns:
 *	WINDOW * win - curses specific opaque window object
 */
WINDOW * window_0(int * WIDTH, int * HEIGHT) 
{
	//// LOCAL DECLARATIONS
	WINDOW * win = initscr();
	
	// quit game if no color
	if(!has_colors()) {
		beep();
		endwin();
		fprintf(stderr, "\nterminal does not support colors");
		return(NULL);
	}
	
	// insure at least min standard window size
	getmaxyx(win, *HEIGHT, *WIDTH);

	if(*WIDTH != COLS_PER_SCREEN || *HEIGHT < ROWS_PER_SCREEN ) 
	{			
		fprintf(stderr,  "Resize window to min: %3d x %3d\n\r"
						 "           currently: %3d x %3d\n", COLS_PER_SCREEN,
								ROWS_PER_SCREEN, *WIDTH, *HEIGHT);
		//printw("Resize window to min: %d x %d\n"
		//	   "           currently:%3d x%3d\n", COLS_PER_SCREEN,
		//						ROWS_PER_SCREEN, *WIDTH, *HEIGHT);
		endwin();
		return(NULL);
	}
	
	// start color definitions
	start_color();
	init_pair(0, COLOR_BLACK, COLOR_BLACK);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_BLUE, COLOR_BLACK);
	init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(6, COLOR_CYAN, COLOR_BLACK);
	init_pair(7, COLOR_WHITE, COLOR_BLACK);
	// other start window settings
	noecho();
	cbreak();
	idlok(win, true);
	keypad(win, true);
	clear();
	refresh();
	return(win);
}


/************************************************************* 
 *	Takes a text object and executes the screen printing
 *	file:
 *		pmain.c	
 *	args:
 *		Text *	output	- node of the text queue 
 */
void text_print(Text * t) 
{
//Starting with the parameter - 27, this initiates the
//coloring. The rest of the parameters are (attribute);(fore color);
//(background color).
//
//Text attributes			//Foreground colors
//0 All attributes off		//30 Black
//1 Bold on					//31 Red
//4 Underscore (on monochrome display adapter only)
//5 Blink on				//32 Green
//7 Reverse video on		//33 Yellow
//8 Concealed on			//34 Blue
//							//35 Magenta
//Background color			//36 Cyan
//40 Black					//37 White
//41 Red
//42 Green
//43 Yellow
//44 Blue
//45 Magenta
//46 Cyan
//47 White
	
	//[{ CHECK }]
	if( !t ) {
		FILE* fp = fopen("errep.txt", "a");
		if(fp) fprintf(fp, "Error: printcolor NULL char *\n");
		fclose(fp);
		return;
	}

	if(!(t->string) ) {
		FILE* fp = fopen("errep.txt", "a");
		if(fp) fprintf(fp, "Error: printcolor NULL char *\n");
		fclose(fp);
		return;
	}
	//[{ VARIABLES }]
	char txtc = tolower(t->foreground);
	char * attr = (t->attributes);
	int fore = COLOR_WHITE;
	int charIndex;
	int stringLen = strlen(t->string);
	int rightMargin =  COLS_PER_SCREEN - t->hMargin;

	//[{ FORMATING }]
	//  foreground/text color	
	if(txtc != ' ') 
	{
		if(txtc == 'w')	     fore = COLOR_WHITE;
		else if(txtc == 'c') fore = COLOR_CYAN;
		else if(txtc == 'p') fore = COLOR_MAGENTA;
		else if(txtc == 'm') fore = COLOR_MAGENTA;
		else if(txtc == 'u') fore = COLOR_BLUE;
		else if(txtc == 'b') fore = COLOR_BLUE;
		else if(txtc == 'y') fore = COLOR_YELLOW;
		else if(txtc == 'g') fore = COLOR_GREEN;
		else if(txtc == 'r') fore = COLOR_RED;
		else if(txtc == 'k') fore = COLOR_BLACK;
		else ERROR |= EC02;
		attron(COLOR_PAIR(fore));
	}
	//  text attributes
	if(attr != NULL) 
	{	for(charIndex = 0; charIndex < strlen(attr); ++charIndex) 
		{	if(attr[charIndex] == 'a')		attron(A_ALTCHARSET);
			else if(attr[charIndex] == 'b') attron(A_BOLD);
			else if(attr[charIndex] == 'd') attron(A_DIM);
			else if(attr[charIndex] == 'p') attron(A_PROTECT);
			else if(attr[charIndex] == 'r') attron(A_REVERSE);
			else if(attr[charIndex] == 's') attron(A_STANDOUT);
			else if(attr[charIndex] == 'i') attron(A_INVIS);
			else if(attr[charIndex] == 'u') attron(A_UNDERLINE);
			else ERROR |= EC03;
		}
	}
	//  position the cursor to print the text 
	if(	(t->posY >= t->vMargin) && 
		(t->posY < ROWS_PER_SCREEN - t->vMargin) &&
		(t->posX >= t->hMargin) && 
		(t->posX < rightMargin))
	{
		move(t->posY, t->posX);
	}
	else if( !(t->posX == UNINIT && t->posY == UNINIT) )
	{
		ERROR |= EC01;
		return;
	}
	
	////[{ PRINTING }]
	if(t->posX + stringLen < rightMargin)
	{
		printw("%s", t->string);	
		getyx(WIN, GLOBAL_Y, GLOBAL_X);
	}
	else 
	{
		char * keep = strdup(t->string);
		char * copy = keep;
		char tempChar;
		int dist = rightMargin - t->posX;
		int print_y;
		do 
		{	// save data for next print sections
			getyx(WIN, print_y, tempChar);
			tempChar = copy[dist];
			copy[dist] = '\0';	
			// print data
			printw("%s", copy);
			stringLen -= dist;
			// reset data and cursor 
			copy[dist] = tempChar;	
			copy = copy + dist;// pointer arithmatic
			dist = rightMargin - t->hMargin;
			move(print_y + 1, t->hMargin);
			// repeat while there are still chars
		} while(stringLen > 0); 

		free(keep);
		getyx(WIN, GLOBAL_Y, GLOBAL_X);

	}
	// OFF ATTRIBUTES	
	attroff(COLOR_PAIR(fore));
	if(txtc != ' ') attroff(fore);
	if(attr != NULL && !(ERROR & EC03)) {
		for(charIndex = 0; charIndex < strlen(attr); charIndex++) {
			if(attr[charIndex] == 'a')		attroff(A_ALTCHARSET);
			else if(attr[charIndex] == 'b') attroff(A_BOLD);
			else if(attr[charIndex] == 'd') attroff(A_DIM);
			else if(attr[charIndex] == 'p') attroff(A_PROTECT);
			else if(attr[charIndex] == 'r') attroff(A_REVERSE);
			else if(attr[charIndex] == 's') attroff(A_STANDOUT);
			else if(attr[charIndex] == 'i') attroff(A_INVIS);
			else if(attr[charIndex] == 'u') attroff(A_UNDERLINE);
		}
	}
	return;
}

/************************************************************* 
 *	Takes a Text object QUEUE and executes the screen printing
 *	The entire Text obj queue will be printed and if not desig
 *	-nated to be persistant, will be free'd. This is a higher
 *	level function than the actual print function, and calls
 *	text_print for each queue node.
 *	file:
 *		pmain.c	
 *	args:
 *		Text *	outputQueue	- node of the text queue 
 */
void text_manager(Text ** ptrAtHead)
{
	Text * queue = *ptrAtHead;
	Text * next = NULL;
	int errQuit = FALSE;
	while(queue && !errQuit)	
	{
		next = queue->next;
		text_print(queue);
		text_destroy(queue);
		queue = next;
		errQuit = programErrorOut(ERROR);
	}
	*ptrAtHead = NULL;
	return;
}

/************************************************************* 
 *	Takes the text queue and clears all of its memory for
 *	program exit. All vic nodes will be clear regardless of
 *	presistance state.
 *	file:
 *		pmain.c	
 *	args:
 *		Text *	text queue 
 */
void text_clear_all(Text * vic)
{
	Text * next = NULL;
	while(vic)	
	{
		next = vic->next;
		text_toggle(vic, FALSE);
		text_destroy(vic);
		vic = next;
	}
	programErrorOut(ERROR);
	vic = NULL;
	return;
}


/************************************************************* 
 *	Main: uses values on the program struct to execute the 
 *	program's functions. The main program loop executes the 
 *	following instrunctions:
 *		1) Print all queued text
 *		2) Accept input from user
 *		3) Handle input and execute reaction
 */
int main(int argc, char * argv[])
{
	////TEST THE PROGRAM FUNCTIONS
	//test_main();
	//return 0;

	////PROGRAM VARIABLES
	Program *	program	= program_create();
	int			cc		= CTRL_DONOTHING;	

	////EXECUTE CURSES SPECIFIC INITIALIZATIONS 
	program->wnd = window_0(&program->screen_width, &program->screen_height);
	if(!program->wnd) 
	{
		endwin();
		goto clear_program;
	}
	ERROR = wsetscrreg(program->wnd, 16, ROWS_PER_SCREEN-1);
	scrollok(program->wnd, true);
	if( ERROR == ERR )
	{	
		printw("Error with scroll reg.\n");
	}

	////PROGRAM INIT
	build_title(&program->queue_head, &program->queue_tail);
	WIN = program->wnd;
	GLOBAL_X = 0;
	GLOBAL_Y = 0;
	ERROR = 0u;
	program->ptrX = &GLOBAL_X;
	program->ptrY = &GLOBAL_Y;

	////PROGRAM LOOP
	while(program->loop_continue & !(ERROR))
	{
		text_manager(&program->queue_head);
		program->user_input = getch();
		program->control_code = input_eval(program->user_input);
		if(!ERROR && program->control_code) 
		{	
			cc = program->control_code;
			if(		cc == CTRL_ADDCHAR	){ CHECKCODE(cc); }
			else if(cc == CTRL_CLEAR	){ CHECKCODE(cc); }
			else if(cc == CTRL_CURSOR	){ CHECKCODE(cc); }
			else if(cc == CTRL_DELCHAR	){ CHECKCODE(cc); }
			else if(cc == CTRL_EXECUTE	){ CHECKCODE(cc); }
			else if(cc == CTRL_EXIT		){ program->loop_continue = FALSE;	}
			else if(cc == CTRL_SCROLL	){ CHECKCODE(cc); }
			else if(cc == CTRL_SELECT	){ CHECKCODE(cc); }
			else if(cc == CTRL_SWITCH	){ CHECKCODE(cc); }
			else if(cc == CTRL_UNDO		){ CHECKCODE(cc); }
		}
		//loop_continue = (user == 'c') ? TRUE : FALSE;
	}
	if( ERROR )
	{	programErrorOut(ERROR);
	}

	////PROGRAM CLOSE
	clear();
	endwin();
	clear_program:
	text_clear_all(program->queue_head);
	program_destroy(program);
	if(isendwin()) 
	{
		SCREEN * new = NULL;// bogus
		SCREEN * old;// for del
		old = set_term(new);
		delscreen(old);
	}
	return(0);
}






