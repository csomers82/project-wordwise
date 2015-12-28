#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "dictionary.h"
#include "tree26.h"
#include "predictive.h"

#define MAX_CHAR_LINE 255 
#define MAX_FAIL 3
#define MAX_BYTES_PER_BUFFER 65536
#define DN_WORDBANK_FN "copy-american-english"
//#define DN_WORDBANK_FN "copy-american-english-abr"
//#define DN_WORDBANK_FN "ababc"

#define TRUE 1
#define FALSE 0


typedef struct MsgQueue {
	char *	line;
	struct MsgQueue * next;
} MsgQueue;


/* ********************************************************** *
 *	explode - breaks a string into unique heap allocated strings
 *		delimeted by '\0'. The string is split at param 'delims'
 *	in -	
 *		const char * str = string being considered 
 *		const char * delims = the characters that will split 'str'
 *		int * arrlen = number of sections in split string
 *	out -
 *		char ** strArr = an contiguous char array with ptrs to 
 *					each str beginning. Null char '\0' ends each
 */ 
char ** explode(const char * str, const char * delims, int * arrLen) 
{	
	// LOCAL DECLARATIONS
	int ntotal;//# of char strings in 'str'
	int len;//# of char in 'str'
	char * handle;// points to lense start
	char * lense;//points to copy of current part of string
	int i, c;//index of str
	int nstr;//number of str created
	int flag;//an indicator to expect a condition

	//BASIC VALIDATION
	if(!str)	fprintf(stderr, "Error: <explode>: 'str' == NULL\n"); 
	if(!delims)	fprintf(stderr, "Error: <explode>: 'delims' == NULL\n"); 
	if(!arrLen)	fprintf(stderr, "Error: <explode>: 'arrLen' == NULL\n"); 
	if(!str || !delims || !arrLen) return NULL;

	//EXECUTABLE STATEMENTS
	len = strlen(str);
	lense = malloc((len + 1) * sizeof(char));
	strcpy(lense, str);
	handle = lense;
	ntotal = 0;
	flag = 1;
	i = 0;

	//DETERMINE NUMBER OF VALID STRINGS
	//so long as the string is not on end, count changes btwn char & delim 
	while((lense[i] != '\0') && (strchr(delims, lense[i]) != NULL))  ++i;
	while(lense[i] != '\0') 
	{	
		//:Delimeter char is being hit
		if(strchr(delims, lense[i]) != NULL) 
		{	
			// case = from char to delim -> end substring 
			// else: case = delim to delim -> no change
			if(!flag) 
			{
				flag = 1;//TRUE, in delimeter section
			}
			
		} //:Delimeter char is not being hit
		else
		{	
			// case = from delim to char -> start substring
			// else: case = from char to char -> no change
			if(flag)
			{
				++ntotal;
				flag = 0;//FALSE, in char section
			}
		}
		++i;
	}
	//if(!flag) ++ntotal;
	//printf("nstrings = %d\n", ntotal);

	//ALLOCATE DATA FOR EACH POINTER IN RETURN CHAR**
	char ** strArr = malloc(ntotal * sizeof(char *));

	c = 0;
	// {increment through begining letters until first is not a delimiter}
	while((*lense != '\0') && (strchr(delims, *lense) != NULL))  {
		++lense;
		++c;
	}
	// {if strlen(str) - beggining_delimiters != 1}	
	if(*lense != '\0') 
	{	// {initialize variables for explosions}
		strArr[0] = lense;
		nstr = 0;
		flag = 0;
		i = 0; 
		while(c++ <= len  && *lense != '\0')
		{	
			//printf("lense = %s, *lense = '%c', lense[%d] = '%c'\n", lense, *lense, i, lense[i]);
			//:Delimeter char is being hit
			if(strchr(delims, lense[i]) != NULL) 
			{	
				// case = from char to delim -> end substring 
				// else: case = delim to delim -> no change
				if(!flag) 
				{
					// cap section w/ nullchar then copy
					lense[i] = '\0';
					strArr[nstr++] = strdup(lense);
					//printf("newstring = %s\n", strArr[nstr - 1]);
					// update ptr position and reset index
					lense += i;
					i = 0;
					flag = 1;//TRUE, in delimeter section
				}
				++lense;
					
			} //:Delimeter char is not being hit
			else
			{	
				// case = from delim to char -> start substring
				// else: case = from char to char OR first char -> no change
				if(flag)
				{
					flag = 0;//FALSE, in char section
				}
				++i;
			}
		}
	}
	free(handle);
	//printf("\n");
	*arrLen = ntotal;
	return(strArr);
}

///
//	Opens an input fd to read char data from.
//
//		char * filename: the filename which is meant to opened
//
FILE * open_file(char * filename)
{
	printf("filename = %s\n", filename);
	//LOCAL VARIABLES
	FILE * fp;
	
	//EXECUTABLE STATEMENTS
	fp = fopen(filename, "r");
	if(!fp) 
	{	
		fprintf(stderr, "Error: input file not recognized- source set to stdin\n");
		return(stdin);
	}
	return(fp);
}
///
//	Opens an input fd to read char data from.
//
//		char * filename: the filename which is meant to opened
//
FILE * open_output_file(char * filename)
{
	printf("filename = %s\n", filename);
	//LOCAL VARIABLES
	FILE * fp;
	
	//EXECUTABLE STATEMENTS
	fp = fopen(filename, "w");
	if(!fp) 
	{	
		fprintf(stderr, "Error: output file not recognized- source set to stdout\n");
		return(stdout);
	}
	return(fp);
}


///
//	Appends a new queue containing a char array to the end of the current queue, or creates the
//	queue if the parameter 'queue' is a NULL pointer. 
//	params:
//		MsgQueue * queue_end:	current message queue or a NULL pointer if empty
//		char * msg:	char array being stored on the queue node after 'queue_end'
//	returns:
//		MsgQueue * queue_end:	the new end of the MsgQueue list
MsgQueue * append_write_create(MsgQueue * queue_end, char * msg )
{
	// allocate a new message queueue node
	MsgQueue * new_end = malloc(sizeof(MsgQueue));
	new_end->next = NULL;
	new_end->line = msg;

	// append  the new node to the end
	if(queue_end) 
		queue_end->next = new_end;
	
	return(new_end);
}

///
//	Pops the front queue node off of 'queue_front' and points 'read' to the line char Array 
//	contained inside. The popped node is free'd
//	params:
//		MsgQueue * queue_front:	current front of the MsgQueue list
//		char ** read:	pass in as a NULL pointer	
//	return:
//		char ** read, (via ptr):	pointer to the popped node's char Array
//		MsgQueue * queue_front:	new front node in the MsgQueue list
//		
MsgQueue * unqueue_read_free(MsgQueue * queue_front, char ** read)
{
	// base validation
	assert(queue_front != NULL);

	// retrieve the message from the victim and reassign the front of the queue
	MsgQueue * new_front = queue_front->next;
	*read = queue_front->line;	
	free(queue_front);
	return(new_front);
}

/* *	
 *		
 *
 */
MsgQueue * buffered_file_input(FILE * fp)
{
	/// LOCAL MAIN VARIABLES
	long	file_end			= -1;// file size in bytes
	long	file_offset			= 0;// current file position
	long	load_size			= 0; // num of bytes per buffer-load
	long	n_bytes_read		= 0;// actual bytes read 
	MsgQueue *	q_front			= NULL;// message queue output
	MsgQueue *	q_back			= NULL;// message queue access point
	char *	file_character_buffer = NULL;// source for file data
	char	residual_character_buffer[MAX_CHAR_LINE];// 
	char *	extra_word_buffer	= NULL;// 
	char	delimiter			= '\n';// character that separates words in the file
	int		bool_large_word		= FALSE;// special case for word spanning buffer 
	int		bool_error			= FALSE;// local error code
	int		check_index			= 0;// a value for checking to see where the last 
	int		save_index			= 0;// 
	int		assembly_index		= 0;//
	int		n_residuals			= 0;// number of characters after the last full line in file
	int		n_loads				= 0;// number of loop iterations
	int		n_buffered_c		= 0;// number of char in buffer

	/// EXECUTABLE STATEMENTS 
	load_size = (MAX_BYTES_PER_BUFFER) * sizeof(char);
	extra_word_buffer = malloc(load_size + 1);
	
	// find total size of the file 
	fseek(fp, 0, SEEK_END);
	file_end = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	/* ---------------------------------
	 *	Buffered File Input Loop Logic
	 *	1) load a chunk
	 *	2) * handle top-of-buffer word endings
	 *	3) * save end-of-buffer word parts
	 *	4) save buffer data
	 */
	while(file_offset < file_end)
	{
		++n_loads;
		/// load a single chunk of data 
		//printf("\n\nLoad number %d]\n", ++n_loads);
		if(file_offset + load_size >= file_end)
		{
			//printf("Final load\n");
			load_size = (file_end - file_offset);
		}
		file_character_buffer = malloc(sizeof(char) * load_size + 1);
		n_bytes_read = fread(file_character_buffer, sizeof(char), load_size, fp);
		file_character_buffer[load_size] = '\0';
		file_offset += load_size;
		n_buffered_c = load_size;
		// check for reading failures
		if(n_bytes_read != load_size)
		{
			//fprintf(stderr, "Error: read %ld / %ld bytes\n", n_bytes_read, load_size);
			bool_error = 1;
		}
		//printf("Read: %s\n", file_character_buffer);
	


		/// assemble the first word with the characters in the residual_buffer
		if(n_residuals || bool_large_word)
		{
			// continue after the residual buffer data
			//printf("%4sAssembling pivot word\n", "");
			//printf("%4s(Buffer) = %s\n", "", residual_character_buffer);
			check_index = 0;
			while(n_buffered_c && file_character_buffer[check_index] != delimiter)
			{
				residual_character_buffer[save_index++] = file_character_buffer[check_index];
				file_character_buffer[check_index] = delimiter;
				++check_index;
				--n_buffered_c;
			}
			if(!n_buffered_c && file_offset < file_end) 
			{
				//printf("%8sspecial case\n", "");
				bool_large_word = TRUE;
				/// the "word" in question spans an entire buffer
				// keep reading buffers in an attempt to finish word
			}
			else 
			{	///* Save the residuals to the extra word buffer */ 
				bool_large_word = FALSE;
				// if the new word will not fit in the EWB, empty the buffer	
				if(save_index + assembly_index >= load_size)
				{	
					//printf("%8sOutputting extra words buffer\n", "");
					extra_word_buffer[--assembly_index] = '\0';
					q_back = append_write_create(q_back, strdup(extra_word_buffer));
					memset(extra_word_buffer, '\0', load_size);
					assembly_index = 0;
				}
				// copy the residual buffer to the extra word buffer
				int copy_index;
				for(copy_index = 0; copy_index < save_index; ++copy_index)
				{
					extra_word_buffer[assembly_index++] = residual_character_buffer[copy_index];
				}
				extra_word_buffer[assembly_index++] = delimiter;
			}
			--n_buffered_c;
			//printf("%4s(Buffer) = %s\n", "", residual_character_buffer);
			// save the residual data
		}
	


		/// handle an uneven cut caused by the amount of characters read
		if(file_offset < file_end)
		{
			// find the last word in the list
			//printf("%4sFinding residual characters\n", "");
			//printf("%s\n", file_character_buffer);
			n_residuals = 0;
			for(check_index = load_size - 1; check_index >= 0; --check_index)
			{
				if(file_character_buffer[check_index] == delimiter) 
				{
					break;
				}
				++n_residuals;
			}
			// save any residual characters
			if(!bool_large_word)
			{
				memset(residual_character_buffer, '\0', sizeof(residual_character_buffer));
				save_index = 0;
			}
			for(/* save index value */; save_index < n_residuals; ++save_index)
			{
				residual_character_buffer[save_index] = file_character_buffer[++check_index];
				file_character_buffer[check_index] = delimiter;
				--n_buffered_c;
			}
			//printf("%4s(Buffer) = %s\n", "", residual_character_buffer);
		}
	
	
		/// save the buffered input
		if(n_buffered_c > 0) 
		{
			if(!bool_large_word)
			{
				q_back = append_write_create(q_back, file_character_buffer);
				if(!q_front)
				{
					q_front = q_back;
				}
			}
		}
		else
		{
			free(file_character_buffer);
		}

	} /* END OF LOOP LOGIC */	
	if(assembly_index > 0)
	{
		printf("%8sOutputting extra words buffer\n", "");
		extra_word_buffer[assembly_index] = '\0';
		q_back = append_write_create(q_back, strdup(extra_word_buffer));
	}
	free(extra_word_buffer);
	printf("\e[34mLoaded %2d data sements!\e[0m\n", n_loads);

	
	return(q_front);

}

/*
int search_loop(Dictionary * dn)
{
	///LOCAL DECLARATIONS
	int search_bool = TRUE;// loop cntrl var
	char * buffer = NULL;// input buffer for user search data
	int item_position;// the hash value of the searched key 
	char ** entry;// a pointer to the returned dn data
	char * hc = "\e[34m";// hit color
	char * mc = "\e[35m";// miss color
	char * ec = "\e[0m";// end color
	char * message = "Please enter a string to search for: ";
	int search_n = 0;
	
	///EXECUTABLE STATEMENTS	
	entry = malloc(sizeof(char *));
	printf("entry = %p\n", entry);
	while(search_bool)
	{
		// clear buffer memory /
		free(buffer);

		// accept value from user /
		search_bool = prompt_user(message, &buffer);
		if( search_bool )
		{	
			// search for value in dictionary /
			//item_position = dictionary_probe_table(dn, buffer);	
			//printf("buffer = %s\n", buffer);
			*entry = dictionary_search(dn, buffer);
			
			if(!(*entry))
			{
				fprintf(stderr, "%d)\t%sItem \"%s\" not found.%s\n", search_n, mc, buffer, ec);
			}
			else
			{
				//printf("item position = %d\n", item_position);
				fprintf(stdout, "%d)\t%sFound: %s%s\n", search_n, hc, *entry, ec);
			}
			++search_n;
		}
		search_bool = prompt_continue();
	}
	printf("entry = %p\n", entry);
	free(buffer);
	free(entry);

	return 0;
}
*/

Tree26 * manage_buffered_file_tree(Tree26 * root)
{
	//LOCAL MAIN VARIABLES
	FILE * fp = NULL;//the input filename of program
	MsgQueue * q_front = NULL;// the output end of the message queue
	char * buffer = NULL;// current line being read from the file
	char * delims = ",\t\r\v\f\n";// delimeters in the data file
	char** frags;// the buffer split into its unique strings
	int add_i;// number of str's managed from the current line 
	int n_line_entries = 0;// number of unique str in the line read from the file
	clock_t start;
	clock_t finish;
	clock_t zero;
	double ellapsed;

	// accept the message data
	start = clock();
	fp = open_file(DN_WORDBANK_FN);
	if(!fp) 
	{
		fprintf(stderr, "Error: unable to open file for reading\n");
		return(root);
	}
	q_front = buffered_file_input(fp);
	fclose(fp);
	finish = clock();
	ellapsed = (double) (finish - start) / CLOCKS_PER_SEC;
	printf("\e[33mBuffered file loaded in [%lf] seconds\e[0m\n", ellapsed);

	
	zero = clock();
	int count = 0;
	// consider and use the data, so long as it is queue'd to manage
	while(q_front && !(count++ > 1000))
	{
		start = clock();
		// pop data, split data
		q_front = unqueue_read_free(q_front, &buffer);	
		frags = explode(buffer, delims, &n_line_entries);

		// add each split segment str
		for(add_i = 0; add_i < n_line_entries; ++add_i) 
		{	
			printf("Entry =\"%s\"\n", frags[add_i]);
			fflush(stdout);
			//printf("%sEntry: \"%s\"\tAddress: %p\n", "", frags[add_i], &frags[add_i]);
			root = tree26_insert(root, frags[add_i]);
			//printall(dn->hash_table, dn->max_size, stdout);
		}
		// buffer is free'd but frags is not (left in data as is, str's being used)
		free(buffer);
		//free(*frags);
		free(frags);
		finish = clock();
		ellapsed = (double) (finish - start) / CLOCKS_PER_SEC;
		printf("\e[33mData segment %2d loaded in [%lf] seconds\e[0m\n", count, ellapsed);
	}
	ellapsed = (double) (finish - zero) / CLOCKS_PER_SEC;
	printf("\e[33mData segment %2d loaded in [%lf] seconds\e[0m\n", count, ellapsed);

	return(root);
}

void tree_print(Tree26 * root, int depth)
{
	if( root )
	{
		int i, j;
		for(i = 0; i < depth; ++i)
			printf(". ");
		printf("%s\n", root->str);
		for(i = 0; i < N_BRANCHES; ++i)
		{
			//for(j = 0; j < depth; ++j)
			//	printf(". ");
			//printf("%c\n", i+65);
			if(root->branch[i])
			{
				tree_print(root->branch[i], depth + 1);
			}
		}
	}
}	

void tree_free(Tree26 * root)
{
	if( root )
	{
		free(root->str);
		int i;
		for(i = 0; i < N_BRANCHES; ++i)
		{
			if(root->branch[i])
			{
				tree_free(root->branch[i]);
			}
		}
		free(root->branch);
		free(root);
	}
	return;
}

void tree_test()
{

	Tree26 * root = tree26_create();
	root->str = strdup("");
	/*
	int i;
	char * buffer = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz'\\#$% &";
	char chr;
	for(i = 0; i < strlen(buffer); ++i)
	{
		printf("%c = %d = (%d)\t", buffer[i], ADJUST(buffer[i]), (int)buffer[i]);
		chr = buffer[i];
		if(((int)chr >= 65) && ((int)chr < 97))
			printf("A");
		else if(((int)chr >= 97) && ((int)chr <= 122))
			printf("a");
		else //((int)chr == 39))
			printf("*");
		printf("\n");
	}
	return;
	*/
	manage_buffered_file_tree(root);
	tree_print(root, 0);
	tree_free(root);
	return;
}

/************************************************************* 
 *	Takes the global ERROR value and uses to print the error
 *	diagnosis to stderr and the error logfile. 
 *	file:
 *		maux.c
 *	args:
 *		int ERRORCOPY : pass the value of ERROR
 *	returns:
 *		void
 */
void programErrorOut(int ERRORCOPY) 
{
	//are there errors?
	if( !ERRORCOPY )
	{
		return;
	}

	//open error log for printing
	FILE * log = fopen(ERRORLOG, "a");
	char * title = "WORD PREDICTION ERROR LOG";
	char * sect1 = "PRINT FUNCTION ERRORS";
	time_t log_time;
	int i;
	char * b = " ";
	char banner = '_';
	int banner_len = 40;

	if(!log) 
	{	fprintf(stderr, "Error: cannot open log file \"%s\"\n", ERRORLOG);
		return;
	}

	//// HEADER
	fprintf(log, "\n");
	for(i = 0; i < banner_len; ++i)// divider
		fprintf(log,"%c", banner);
	fprintf(log, "\n");
	fprintf(log, "%s%s\n", b, title);// title
	log_time = time(NULL);
	fprintf(log, "%8s%s", b, ctime(&log_time));/// time stamp
	for(i = 0; i < banner_len; ++i)// divider
		fprintf(log,"%c", banner);
	fprintf(log, "\n");

	////PRINT FUNCTION ERRORS
	if(ERRORCOPY  & (EC01 | EC02 | EC03)) 
	{	
		fprintf(log, "%s%s\n", b, sect1);
		if(ERRORCOPY & EC01)
		{	LOG(log, "EC01: Attempt to set cursor outside user defined margin values.\n");
		}
		if(ERRORCOPY & EC02)
		{	LOG(log, "EC02: Unrecognized foreground color value.\n");
		}
		if(ERRORCOPY & EC03)
		{	LOG(log, "EC03: Unrecognized attribute identifier value.\n");
		}
	}
	fclose(log);
	return;
}
