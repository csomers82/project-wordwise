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

int ERROR;


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
/*	#ifdef COLOUR
		#define AA 31
		#define BB 32
		#define CC 33
		#define DD 34
		#define EE 35
		#define SHADE  30 // {30-K, 31-R, 32-G, 33-Y, 34-B, 35-M, 36-C, 37-W}
		#define SHADE2 30 // {30-K, 31-R, 32-G, 33-Y, 34-B, 35-M, 36-C, 37-W}
		#ifdef SHOWFILE
			#define FILINF fprintf(stderr, "\n\e[%dm%s", SHADE, __FILE__)
			#define WHERE FILINF fprintf(stderr, "\e[%dm%d\e[%dm {%s}==>\e[0m", SHADE2, __LINE__, SHADE, __func__)
		#else
			#define FILINF fprintf(stderr, "\n")
			#define WHERE FILINF fprintf(stderr, "\e[%dm%d\e[%dm {%s}==>\e[0m", SHADE2, __LINE__, SHADE, __func__)
		#endif	
		#define SHOWi(x) WHERE fprintf(stderr,"\e[%dm[%4s] = %d\e[0m", AA, #x, x)
		#define SHOWl(x) WHERE fprintf(stderr,"\e[%dm[%4s] = %Ld\e[0m", FF, #x, x)
		#define SHOWf(x) WHERE fprintf(stderr,"\e[%dm[%4s] = %f\e[0m", BB, #x, x)
		#define SHOWc(x) WHERE fprintf(stderr,"\e[%dm[%4s] = %c\e[0m", CC, #x, x)
		#define SHOWp(x) WHERE fprintf(stderr,"\e[%dm[%4s] = %p\e[0m", DD, #x, x)
		#define SHOWs(x) WHERE fprintf(stderr,"\e[%dm[%4s] = %s\e[0m", EE, #x, x)
	#else	
		#ifdef SHOWFILE
			#define FILINF fprintf(stderr, "\n%s:", __FILE__)
			#define WHERE FILINF fprintf(stderr, "::%s:%d %s==> ", __LINE__, __func__)
		#else
			#define FILINF 
			#define WHERE fprintf(stderr, "\n::%s:%d %s==> ", __LINE__, __func__)
		#endif	
		#define SHOWi(x) WHERE fprintf(stderr,"[%4s] = %d", #x, x)
		#define SHOWf(x) WHERE fprintf(stderr,"[%4s] = %f", #x, x)
		#define SHOWl(x) WHERE fprintf(stderr,"[%4s] = %Ld",#x, x)
		#define SHOWc(x) WHERE fprintf(stderr,"[%4s] = %c", #x, x)
		#define SHOWp(x) WHERE fprintf(stderr,"[%4s] = %p", #x, x)
		#define SHOWs(x) WHERE fprintf(stderr,"[%4s] = %s", #x, x)
	#endif
*/
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
	#define AA 
	#define BB 
	#define CC 
	#define DD 
	#define EE
#endif

void test1()
{
	ERROR = FALSE;
	char * str = strdup("testing123...");
	char col = 'b';
	Text * new = NULL;

	SHOWp(new);	
	new = text_create(str, col, NULL);
	SHOWp(new);	

	text_toggle(new );
	text_destroy(new);
	if(ERROR)
	{ 
		printf("sizeof(Text) = %ld\n", sizeof(Text));
		printf("sizeof(new) = %ld\n", sizeof(new));
		fprintf(stderr, "Error: sizeof error.\n");
	}
	text_toggle(new );
	text_destroy(new);
	return;
}

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
Text * text_create(char * string, char fore, Text * tail)
{
	//CREATE NEW TEXT OBJECT
	Text * new = malloc(sizeof(Text));
	if(tail) tail->next = tail;
	
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
 *		predictive.c	
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
	int fore = COLOR_WHITE;
	int back = COLOR_BLACK;

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
	}
	//  text attributes
	if(attr != NULL) {
		int l;
		for(l = 0; l < strlen(attr); l++) {
			if(attr[l] == 'a')		attron(A_ALTCHARSET);
			else if(attr[l] == 'b') attron(A_BOLD);
			else if(attr[l] == 'd') attron(A_DIM);
			else if(attr[l] == 'p') attron(A_PROTECT);
			else if(attr[l] == 'r') attron(A_REVERSE);
			else if(attr[l] == 's') attron(A_STANDOUT);
			else if(attr[l] == 'i') attron(A_INVIS);
			else if(attr[l] == 'u') attron(A_UNDERLINE);
			else ERROR |= EC03;
		}
	}
	//  position the cursor to print the text 
	if(	t->yPos >= t->vMargin && 
		t->yPos < ROWS_PER_SCREEN - t->vMargin &&
		t->xPos >= t->hMargin && 
		t->xPos hMargin < ROWS_PER_SCREEN - t->vMargin)
	{
		move(t->yPos, t->xPos);
	}
	else if( !(t->xPos == UNINIT && t->yPos == UNINIT) )
	{
		ERROR |= EC01;
	}
	attron(COLOR_PAIR(fore));
	//[{ PRINTING }]
	printw("%s", str);	

	// OFF ATTRIBUTES	
	attroff(COLOR_PAIR(fore));
	if(txtc != ' ') attroff(fore);
	if(bckc != ' ') attroff(back);
	if(attr != NULL) {
		int l;
		for(l = 0; l < strlen(attr); l++) {
			if(attr[l] == 'a')		attroff(A_ALTCHARSET);
			else if(attr[l] == 'b') attroff(A_BOLD);
			else if(attr[l] == 'd') attroff(A_DIM);
			else if(attr[l] == 'p') attroff(A_PROTECT);
			else if(attr[l] == 'r') attroff(A_REVERSE);
			else if(attr[l] == 's') attroff(A_STANDOUT);
			else if(attr[l] == 'i') attroff(A_INVIS);
			else if(attr[l] == 'u') attroff(A_UNDERLINE);
			else ERROR |= EC04;
		}
	}
}



}


/************************************************************* 
 *
 */
int main(int argc, char * argv[])
{
	////PROGRAM VARIABLES
	int loop_continue	= TRUE;				// true/false continue program
	int screen_height	= ROWS_PER_SCREEN;	// window cursor heigths
	int screen_width	= COLS_PER_SCREEN;	// window cursor widths
	WINDOW * wnd		= NULL;				// curses opaque window object
	
	////PREWINDOW PRINT TESTING
	test1();
	fprintf(stderr, "sizeof chtype = %li\n", sizeof(chtype));
	fprintf(stderr, "sizeof COLOR_BLUE = %li\n", sizeof(COLOR_BLUE));
/*	fprintf(stderr, "val of COLOR_BLUE = %i\n", COLOR_BLUE);

	////EXECUTE CURSES SPECIFIC INITIALIZATIONS 
	wnd = window_0(&screen_width, &screen_height);
	if(!wnd) 
	{
		endwin();
		goto clear_program;
	}

	////PROGRAM INIT

	////PROGRAM LOOP
	while( loop_continue )
	{
		loop_continue = FALSE;
		printw("Ah yeah!\n");
		getch();
	}

	////PROGRAM CLOSE
	clear();
	endwin();
	clear_program:
	if(isendwin()) {
		SCREEN * new = NULL;// bogus
		SCREEN * old;// for del
		old = set_term(new);
		delscreen(old);
	}
*/
	return(0);
}






