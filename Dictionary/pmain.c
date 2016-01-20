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
		if(fp) fprintf(fp, "Error: printcolor NULL (Text *) arg \n");
		fclose(fp);
		return;
	}

	if(!(t->string) ) {
		FILE* fp = fopen("errep.txt", "a");
		if(fp) fprintf(fp, "Error: printcolor NULL (char *)\n");
		fclose(fp);
		return;
	}
	//[{ VARIABLES }]
	char txtc = tolower(t->foreground);
	char * attr = (t->attributes);
	int fore = COLOR_WHITE;
	int charIndex;
	int stringLen = strlen(t->string);
	//(!t->bool_wide) ? strlen((char*) t->string) : wcslen((wchar_t*) t->string);
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
		//if(!(t->bool_wide))	printw("%s", (char *) t->string);	
		//else				printw("%S", (wchar_t *) t->string);	
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
			printw("%s", t->string);	
			//if(!(t->bool_wide))	printw("%s", (char *) t->string);	
			//else				printw("%S", (wchar_t *) t->string);	
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
 *	Positions the cursor at the position denoted by the 
 *	active edit box. Only if the program is in edit mode will 
 *	the program execute the repositioning (cursor)
 *	file:
 *		pmain.c
 *	args:
 *		Program * program: source of all cursor and ebox vars
 */
void cursor_reposition(Program * p)
{
	if(p->phase == EDIT)
	{
		int x, y, i; 
		i = p->ebox_active;
		x = p->ebox_array[i].text_x_orig + 
			p->ebox_array[i].index;
		y = p->ebox_array[i].text_y_orig;
		wmove(WIN, y, x);
		getyx(WIN, GLOBAL_Y, GLOBAL_X);
	}
}

/*************************************************************
 *	Accepts a character and evaluates the correct program
 *	course of action.
 *	file:
 *		pmain.c
 *	args:
 *		Program * p: contains the values associated with i/o
 */
void handle_char(Program * p)
{
	if(p->phase == EDIT)
	{
		int i = p->ebox_active;
		////inserting a character
		if(	(p->control_code == CTRL_ADDCHAR) &&
			(p->ebox_array[i].index < p->ebox_array[i].max)	)
		{
			//insert the user character
			delch();
			insch(p->user_input);
			move(GLOBAL_Y, ++GLOBAL_X);

			//handle the edit box and tree ptrs 
			p->ebox_array[i].index += 1;
			if(p->ebox_active == WORKSPACE)
			{
				p->pos_index += 1;
				if(p->node) 
				{	
					//either valid next branch or NULL
					p->node = BRANCH(p->node, p->user_input);
				}
				p->pos_stack[p->pos_index] = p->node;
				CHECKSTACK(p->pos_stack, p->pos_index);
			}
			else if(p->ebox_active == PARAMETERS)
			{
				if(	(p->pos_index != 0 ) )
				{
					ebox_clear(p, WORKSPACE);
				}
			}
		}
		////backspacing a character
		else if(	(p->control_code == CTRL_DELCHAR) &&
					(p->ebox_array[i].index > 0)			)
		{
			//handle the edit box and tree ptrs
			p->ebox_array[i].index -= 1;
			if(p->ebox_active == WORKSPACE)
			{
				p->pos_stack[p->pos_index] = NULL;
				p->pos_index -= 1;
				p->node = p->pos_stack[p->pos_index];
				CHECKSTACK(p->pos_stack, p->pos_index);
			}

			//remove the top character
			move(GLOBAL_Y, --GLOBAL_X);
			delch();
			insch(BLANK_CHAR);
		}
		////clearing all of the characters
		else if( p->control_code == CTRL_CLEAR )
		{
			ebox_clear(p, p->ebox_active);	
		}
	}
}



/************************************************************* 
 *	Create the text objects necessary to print the queued 
 *	results from the created framework
 *	file:
 *		pmain.c
 *	args:
 *		Program * p:	Text *	results_array
 *						int		results_index
 *	returns:
 *		void
 */
void results_grab(Program * p)
{
	////LOCAL VARIABLES
	Text * resultTextQueue = NULL;
	int scroll_reg_lines = SCROLL_END - SCROLL_BEG;
	int min = p->results_limit;
	int	index;

	////EXECUTABLE STATEMENTS
	//point start at index
	resultTextQueue = p->results_array[p->results_index];
	//ensure all of the necessary text objects are connected
	min = (min < scroll_reg_lines) ? min : scroll_reg_lines;
	for(index = p_results_index; index < (p->results_index + min - 1); ++index)
	{
		p->results_array[index]->next = p->results_array[index + 1]; 
		text_toggle(p->results_array[index], TRUE);
	}
	resultTextQueue[index]->next = NULL; 

	//sever end to fit scroll window
	p->results_array[index]->next;
	resultTextQueue->next = NULL;
	

	//copy text and attach object
	p->queue_tail->next = resultTextQueue;
	resultTextQueue;
	return;
}

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
TreeQueue * tree26_bfs(Program * p)
{
	///LOCAL DECLARATIONS
	TreeQueue *	search_q_head	= NULL;
	TreeQueue *	search_q_tail	= NULL;
	TreeQueue * result_q_head	= NULL;
	TreeQueue * result_q_tail	= NULL;
	TreeQueue *	tempQ			= NULL;
	Tree26 *	currNode		= NULL;
	int			depth			= 0;			
	int			index			= 0;
	int			depth_max		= 1;

	///EXECUTABLE STATEMENTS
	// asserted that current tree node is valid
	if(!p->node)
	{	
		ERROR |= EC0C;
		return NULL;
	}
	// initialize 
	treeQueue_create(tempQ, p->node);
	search_q_tail = tempQ;
	search_q_head = tempQ;

	//// Conduct breadth first search!
	while (depth < depth_max) 
	{ do {	
		STAFF;
		
		//==========================================
		// case 1: node is a word 
		//		:: pop search : push result 
		currNode = search_q_head->node;
		if(	currNode->bool_complete_low ||
			currNode->bool_complete_cap )
		{
			SHOWs("push result");
			treeQueue_create(tempQ, currNode);
			if( result_q_tail)
				result_q_tail->next = tempQ;
			result_q_tail = tempQ;
			if(!result_q_head)
				result_q_head = result_q_tail;
			
			SHOWs("free head");
			SHOWp(search_q_head);
			free(search_q_head);
		}
		// case 2: node is incomplete 
		//		:: pop search : push children
		else {
			SHOWs(currNode->str);
			for(index = 0; index < N_BRANCHES; ++index)
			{
				if(currNode->branch[index])
				{
					SHOWc(97 + index);
					treeQueue_create(tempQ, currNode->branch[index]);
					search_q_tail->next = tempQ;
					SHOWp(search_q_head->next);
					SHOWp(search_q_tail->next);
					search_q_tail = tempQ;
					if(!search_q_head)
						search_q_head = search_q_tail;	
				}
			}

		}
		search_q_head = search_q_head->next;
		SPACE;
		
	  } while(search_q_head);
		depth += 1;
	}

	return(result_q_head);
}

/************************************************************* 
 *	Takes the a TreeQueue list and puts the first one hundred 
 *	search results into the lines of a text object
 *	file:
 *		pmain.c
 *	args:
 *		TreeQueue * resultList:	A queue of Tree26 node ptrs
 *		int	* limit: total number of results returned
 *	returns:
 *		Text * newSet: list appended to program text queue
 */
Text * results_frame(TreeQueue * resultList, int * limit)
{
	////LOCAL VARIABLES
	TreeQueue *	treeCurr		= resultList;// top of result queue
	TreeQueue *	treeNext		= NULL;// next queue'd result node
	Text **		results_array	= NULL;// previous text queue obj
	Text *		tail			= NULL;// previous object to be created
	char 		buffer[60]		= {'\0'};// character information
	int			count			= 0;// limiting count of results returned
	char *		colors			= "krgybmcw";

	////EXECUTABLES STATEMENTS
	// create results array
	results_array = malloc(sizeof(Text*) * MAX_RESULTS);

	// loop for all results and no more than MAX_RESULTS times 
	while((treeCurr) && (count < MAX_RESULTS))
	{
		// create context for loop iteration
		treeNext = treeCurr->next;

		// allocate character data
		memset(buffer, '\0', 60);
		if(count < 10)	
		{	sprintf(buffer, "| 0%1d |  %30s", count, treeCurr->node->str);
		}
		else 
		{	sprintf(buffer, "| %2d |  %30s", count, treeCurr->node->str);
		}

		// create Text object
		results_array[count] = text_create(strdup(buffer), colors[(count / 2 + 1) % 8], tail);

		// update loop conditions for next pass
		treeCurr = treeNext;
		++count;
	}

	// clear result past MAX_RESULTS
	while(treeCurr)
	{
		treeNext = treeCurr->next;
		treeCurr->next = NULL;
		free(treeCurr);
		treeCurr = treeNext;
	}
	return(results_array);
}


/************************************************************* 
 *	Main: uses values on the program struct to execute the 
 *	program's functions. The program's loop executes the 
 *	following instrunctions:
 *		1) Print all queued text
 *		2) Accept input from user
 *		3) Adjust Tree26 ptr stack
 *		4) BFS from Tree26 ptr
 *		5) Queue all complete word results
 *		E) Evaluate continue
 */
int main(int argc, char * argv[])
{
	////TEST THE PROGRAM FUNCTIONS
	test_main();
	return 0;

	////PROGRAM VARIABLES
	Program *	program		= program_create();
	TreeQueue *	candidates	= NULL;
	int			cc			= CTRL_DONOTHING;	

	////EXECUTE CURSES SPECIFIC INITIALIZATIONS 
	program->wnd = window_0(&program->screen_width, &program->screen_height);
	if(!program->wnd) 
	{
		endwin();
		goto clear_program;
	}
	ERROR = wsetscrreg(program->wnd, 16, ROWS_PER_SCREEN-8);
	scrollok(program->wnd, true);
	if( ERROR == ERR )
	{	
		printw("Error with scroll reg.\n");
		ERROR = EC0D;
	}

	////PROGRAM INIT
	program->tree = tree26_create();
	program->tree = manage_buffered_file_tree(program->tree);
	if(!program->tree)
		ERROR |= EC0B;
	program->node = program->tree; 
	program->pos_stack[0] = program->node; 
	program->pos_index += 1;
	program->queue_tail = build_title(&program->queue_head);
	program->queue_tail = build_box(EDIT_1_X, 
									EDIT_1_Y, 
									EDIT_1_WIDTH, 
									EDIT_BOX_HEIGHT, 
									EDIT_1_COLOR, 
									program->queue_tail);

	program->queue_tail = build_box(EDIT_2_X, 
									EDIT_2_Y, 
									EDIT_2_WIDTH, 
									EDIT_BOX_HEIGHT, 
									EDIT_2_COLOR, 
									program->queue_tail);

	program->queue_tail = text_create(strdup(EDIT_1_LABEL),	EDIT_1_COLOR, program->queue_tail);
	text_position(program->queue_tail, (EDIT_1_Y + (EDIT_BOX_HEIGHT / 2)), (EDIT_1_X + 2));
	program->queue_tail = text_create(strdup(EDIT_2_LABEL),	EDIT_2_COLOR, program->queue_tail);
	text_position(program->queue_tail, (EDIT_2_Y + (EDIT_BOX_HEIGHT / 2)), (EDIT_2_X + 2));
	
	WIN = program->wnd;
	GLOBAL_X = 0;
	GLOBAL_Y = 0;
	ERROR = 0u;
	program->ptrX = &GLOBAL_X;
	program->ptrY = &GLOBAL_Y;
	program->ebox_array = program_create_eboxes();

	////PROGRAM LOOP
	while(program->loop_continue & !(ERROR))
	{
		text_manager(&program->queue_head);
		cursor_reposition(program);
		program->user_input = getch();
		program->control_code = input_eval(program->user_input);
		if(!ERROR && program->control_code) 
		{	
			cc = program->control_code;
			if(	(cc == CTRL_ADDCHAR	) ||
				(cc == CTRL_CLEAR	) ||
				(cc == CTRL_DELCHAR	) )
			{  
				handle_char(program);
			}
			else if(cc == CTRL_CURSOR	){ CHECKCODE(cc); }
			else if(cc == CTRL_EXECUTE	){ CHECKCODE(cc); }
			else if(cc == CTRL_EXIT		)
			{ 
				program->loop_continue = FALSE;	
			}
			else if(cc == CTRL_SCROLL	){ CHECKCODE(cc); }
			else if(cc == CTRL_SELECT	){ CHECKCODE(cc); }
			else if(cc == CTRL_SWITCH	)
			{  
				program->ebox_active += 1;	
				program->ebox_active %= N_EDIT_BOXES;	
			}
			else if(cc == CTRL_UNDO		){ CHECKCODE(cc); }
		}
		if(program->phase == EDIT)
		{
			results_clear(program->results_array);	
			candidates = tree26_bfs(program);
			program->results_array = results_frame(candidates, 
									&program->results_limit);	
			program->results_index = 0;
			results_grab(program);
		}
	}
	if( ERROR )
	{	programErrorOut(ERROR);
	}

	////PROGRAM CLOSE
	clear();
	endwin();

	clear_program:
	text_clear_all(program->queue_head);
	tree26_destroy(program->tree);
	program_destroy_eboxes(program->ebox_array);
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






