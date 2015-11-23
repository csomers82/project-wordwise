#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "dictionary.h"

#define MAX_CHAR_LINE 255 
#define MAX_FAIL 3
//#define MAX_BYTES_PER_BUFFER 65536
#define MAX_BYTES_PER_BUFFER 20
//#define DN_WORDBANK_FN "wordbank_abr.txt"
#define DN_WORDBANK_FN "abcd"
//#define DN_WORDBANK_FN "wordbank_10.txt"
//#define DN_WORDBANK_FN "/usr/share/dict/american-english"

#define True 1
#define False 0


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



void printall(char ** words, int max, FILE * fp)
{
	int lcv;// loop ctrl variable 
	fprintf(fp, "Dictionary\n");
	for(lcv = 0; lcv < max; ++lcv)
	{
		fprintf(fp, " dictionary[%2d] = ", lcv);
		if(words[lcv] != NULL)
		fprintf(fp, "\"%-4s\"", words[lcv]);
		else
		fprintf(fp, "%-4s", (fp == stdout)? "\e[31mNULL\e[0m": "NULL");
		//if(lcv % 4 == 3) printf("\n");
		//else printf("\t");
		fprintf(fp, "\n");
	}
	fprintf(fp, "\n");
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
	long file_end =			-1;// file size in bytes
	long file_offset =		 0;// current file position
	long load_size =		 0; // num of bytes per buffer-load
	long n_bytes_read =		 0;// actual bytes read 
	MsgQueue * q_front =	 NULL;// message queue output
	MsgQueue * q_back =		 NULL;// message queue access point
	char * file_character_buffer = NULL;// source for file data
	char residual_character_buffer[MAX_CHAR_LINE];// 
	char front_end_buffer[MAX_CHAR_LINE];// 
	char current_char =		 '\0';// character that represents the last char read
	char delimeter =		 '.';// character that separates words in the file
	int bool_large_word =	 False;// special case for word spanning buffer 
	int bool_error =		 False;// local error code
	int check_index =		 0;// a value for checking to see where the last 
	int save_index =		 0;// 
	//int assembly_index =	 0;//
	int	n_residuals =		 0;// number of characters after the last full line in file
	int n_loads	=			 0;// number of loop iterations
	int n_buffered_c =		 0;// number of char in buffer

	/// EXECUTABLE STATEMENTS 
	load_size = (MAX_BYTES_PER_BUFFER) * sizeof(char);
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
		/// load a single chunk of data 
		printf("\n\nLoad number %d]\n", ++n_loads);
		if(file_offset + load_size >= file_end)
		{
			printf("Final load\n");
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
			fprintf(stderr, "Error: read %ld / %ld bytes\n", n_bytes_read, load_size);
			bool_error = 1;
		}
		printf("Read: %s\n", file_character_buffer);
	


		/// assemble the first word with the characters in the residual_buffer
		if(n_residuals || bool_large_word)
		{
			// continue after the residual buffer data
			printf("%4sAssembling pivot word\n", "");
			printf("%4s(Buffer) = %s\n", "", residual_character_buffer);
			check_index = 0;
			while(n_buffered_c && file_character_buffer[check_index] != delimeter)
			{
				residual_character_buffer[save_index++] = file_character_buffer[check_index];
				file_character_buffer[check_index] = delimeter;
				++check_index;
				--n_buffered_c;
			}
			if(!n_buffered_c && file_offset < file_end) 
			{
				printf("%8sspecial case\n", "");
				bool_large_word = True;
				/// the "word" in question spans an entire buffer
				// keep reading buffers in an attempt to finish word
			}
			else 
			{
				bool_large_word = False;
				residual_character_buffer[++save_index] = '\0';
				q_back = append_write_create(q_back, strdup(residual_character_buffer));
			}
			--n_buffered_c;
			printf("%4s(Buffer) = %s\n", "", residual_character_buffer);
			// save the residual data
		}
	


		/// handle an uneven cut caused by the amount of characters read
		if(file_offset < file_end)
		{
			// find the last word in the list
			printf("%4sFinding residual characters\n", "");
			printf("%s\n", file_character_buffer);
			n_residuals = 0;
			for(check_index = load_size - 1; check_index >= 0; --check_index)
			{
				if(file_character_buffer[check_index] == delimeter) 
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
				file_character_buffer[check_index] = delimeter;
				--n_buffered_c;
			}
			printf("%4s(Buffer) = %s\n", "", residual_character_buffer);
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

	return(q_front);

}

Dictionary * manage_file(Dictionary * dn)
{
	//LOCAL MAIN VARIABLES
	FILE * fp = NULL;//the input filename of program
	MsgQueue * q_front = NULL;// the output end of the message queue
	MsgQueue * q_back = NULL;// the input end of the message queue
	char * buffer = NULL;// current line being read from the file
	char * delims = ",\t\r\v\f\n";// delimeters in the data file
	char** frags;// the buffer split into its unique strings
	int add_i;// number of str's managed from the current line 
	int n_line_entries = 0;// number of unique str in the line read from the file

	// accept the message data
	fp = open_file(DN_WORDBANK_FN);
	if(!fp) 
	{
		fprintf(stderr, "Error: unable to open file for reading\n");
		return(dn);
	}
	buffer = malloc(sizeof(char) * MAX_CHAR_LINE);
	if(fgets(buffer, MAX_CHAR_LINE, fp))
	{
		// accept data into the queue	
		q_front = append_write_create(NULL, strdup(buffer));
		q_back = q_front;
		free(buffer);
		buffer = malloc(sizeof(char) * MAX_CHAR_LINE);
		// and continue so long as the file has data
		while(fgets(buffer, MAX_CHAR_LINE, fp))
		{
			q_back = append_write_create(q_back, strdup(buffer));
			free(buffer);
			buffer = malloc(sizeof(char) * MAX_CHAR_LINE);
		}
	}
	// free the data
	free(buffer);
	fclose(fp);

	// consider and use the data, so long as it is queue'd to manage
	while(q_front)
	{
		// pop data, split data
		q_front = unqueue_read_free(q_front, &buffer);	
		frags = explode(buffer, delims, &n_line_entries);

		// add each split segment str
		for(add_i = 0; add_i < n_line_entries; ++add_i) 
		{	
			dictionary_add_entry(dn, frags[add_i]);
			//printall(dn->hash_table, dn->max_size, stdout);
		}
		// buffer is free'd but frags is not (left in data as is, str's being used)
		free(buffer);
		//free(*frags);
		free(frags);
	}

	return(dn);
}



void add(Dictionary * dn)
{
	char buffer[256];
	printf("Enter string into the dictionary: ");
	scanf("%s", buffer);
	dictionary_add_entry(dn, strdup(buffer));
}


void test_explode(char * delim)
{
	char buffer[256];
	printf("Input a string to be split: ");
	scanf("%s", buffer);
	char * str = strdup(buffer);


	int i, len = 0;
	char ** nothing = explode(str, delim, &len); 
	for(i = 0; i < len; ++i)
		free(nothing[i]);
	free(nothing);
	free(str);

}

void printb(uint32_t b)
{
	int bit_n;
	for(bit_n = 0; bit_n < 32; )
	{
		printf("%s", (b & 0x00000001) ? "1": "\e[34m0\e[0m");
		if(++bit_n % 8 == 0) printf("%3s", " ");
		else if(bit_n % 4 == 0) printf("%1s"," ");
		b = b >> 1;
	}
	printf("\n");
}

int prompt_user(char * message, char ** end_buffer)
{
	/// LOCAL DECLARATIONS
	int error_num = 0;
	int n_unacceptable = MAX_FAIL;
	char * buffer = malloc(sizeof(char) * MAX_CHAR_LINE + 1);
	memset(buffer, '\0', sizeof(buffer));
	*end_buffer = buffer;
	do {
		// ACCEPT USER VALUE
		printf("%s", message);
		scanf("%s", buffer);
		if( buffer != NULL && *buffer != '\0' )
			break;
		fprintf(stderr, "Error: No input string.\n");
	} while(++error_num < n_unacceptable);
	if(error_num >= n_unacceptable)
	{
		fprintf(stderr, "Error: Too many errors.\n");
		fprintf(stderr, "Exiting with Failure Status\n");
		return False;
	}
	return True;
}
char * strlower(char * str)
{
	int i;
	char * copy = strdup(str);
	for(i = 0; i < strlen(str); ++i)
	{
		copy[i] = tolower(str[i]);
	}
	return(copy);
}


int prompt_continue(void)
{
	/// LOCAL DECLARATIONS
	char * message = "Enter another search? (yes/no):    ";
	int n_unacceptable = 2;
	int error_num = 0;
	char * buffer = NULL;//malloc(sizeof(char) * MAX_CHAR_LINE + 1);
	char * case_copy;// ptr to buffer for changing to lowercase
	int valid_bool = False;
	int r_value = -1;
	do {
		// clear buffer
		free(buffer);

		// ACCEPT USER VALUE
		valid_bool = prompt_user(message, &buffer);
		if( valid_bool )
		{
			if(buffer)
			{
				case_copy = strlower(buffer);
				if( !strcmp(case_copy, "yes"))
				{	
					r_value = True;
				}
				else if( !strcmp(buffer, "no"))
				{	
					r_value = False;
				}
				free(case_copy);
				if( r_value + 1 )
				{
					free(buffer);
					return r_value;
				}
				fprintf(stderr, "Error: option must match \"yes\" or \"no\"\n");
			}
		}
	} while(++error_num < n_unacceptable);
	free(buffer);
	fprintf(stderr, "Error: Too many errors.\n");
	fprintf(stderr, "Exiting with Failure Status\n");
	return False;
}

void test1()
{
	// test:
	//	prompt_user
	//
	char * buffer = NULL;
	char * message = "Input a string:    ";
	int pass_fail = False;
	pass_fail = prompt_user(message, &buffer);
	char * show = (buffer != NULL)? buffer: "(null)";
	printf("string input == \"%s\"\n", show);
	printf("function success == %d \n", pass_fail);
	free(buffer);

}

void test2()
{
	// test:
	//	buffered_file_input()
	//
	MsgQueue * input = NULL;
	MsgQueue * part = NULL;
	char * message = NULL;
	FILE * fp = fopen(DN_WORDBANK_FN, "r");
	int count = 0;

	input = buffered_file_input(fp);
	while( input )
	{
		printf("%3d: (%p):\t", ++count, input);			
		input = unqueue_read_free(input, &message);
		printf("%s\n", message);
		free(message);
	}

	fclose(fp);
}
/*
void test3()
{
	// test:
	//	explode()
	//
	int sz_max = 10;
	char * t = "  a bc d";//"taco\tburrito nacho\rsalsa";
	char * d = " \t\f\v\n\raeiou";
	char ** frags;
	char * str = "aushtlknawrt\nenskdnf\vkwek!";
	char ** grenade = explode(str, " ", &sz_max);

	frags = explode(t, d, &check);
	for(sz_max = 0; sz_max < check; ++sz_max)
	{
		test_explode(d);
	}
		printf("frags[%d] = \"%s\"\n", sz_max, frags[sz_max]);
		printf("frags[%d] = \"%p\"\n", sz_max, frags[sz_max]);
		free(frags[sz_max]);
	}
	free(frags);
	
}
*/
int search_loop(Dictionary * dn)
{
	///LOCAL DECLARATIONS
	int search_bool = True;// loop cntrl var
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
		/* clear buffer memory */
		free(buffer);

		/* accept value from user*/
		search_bool = prompt_user(message, &buffer);
		if( search_bool )
		{	
			/* search for value in dictionary */
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


int main(int argc, char * argv[])
{
	char * str = ("Lettuce commence testing\n");
	printf("%s", str);
	//test variables
	Dictionary * DN;
	int sz_max = 30000;
	float thresh = 75.0f;// % of whole
	float sz_ratio = 4.0f;// * orig
	int check = 1;


	test2();
	return 0;

	int tree_n, tbl_n, hash_n, rehash_n;
	tree_n = 0;// no tree sruct
	tbl_n = 1;// standard, one slot table
	hash_n = 3;// base 128
	rehash_n = 1;// linear probe
	

	uint32_t config = dictionary_config(tbl_n, tree_n, hash_n, rehash_n);  
	printb(config);


	// dictionary instance
	DN = dictionary_create(sz_max, config);
	
	dictionary_initialize(DN, thresh, sz_ratio);
	DN = manage_file(DN);
	//while(check--)
	//{
	//		add(DN);// add another entry into the table
	//	if(check % 2) printall(DN->hash_table, DN->max_size);
	//		printall(DN->hash_table, DN->max_size);
	//	}
	
	search_loop(DN);
	FILE * dlog = fopen("dw.log", "w");
	printall(DN->hash_table, DN->max_size, dlog );
	fclose(dlog);
	dictionary_free(DN);
	
	return 0;
}
