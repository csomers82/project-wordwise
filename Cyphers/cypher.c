#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "cypher.h"

///
//	Displays the functionality of the program
//
void cy_usage()
{
	fprintf(stderr, "Not enough arguments\n");
	fprintf(stderr, "cypher: -k key.txt -i inputmsg.txt -o output.txt -u\n");
	fprintf(stderr, "-k\tspecifies a file containing the bytes of the cypher \n");
	fprintf(stderr, "-i\tspecifies an input file that contains the message being cyphered \n");
	fprintf(stderr, "-o\tspecifies an output location for the data \n");
	fprintf(stderr, "-u\tspecifies to the program to uncypher the message with the key \n");
}

///
//	Opens an input fd to read char data from.
//
//		char * filename: the filename which is meant to opened
//
FILE * cy_open_file(char * filename)
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
FILE * cy_open_output_file(char * filename)
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
//	Takes in the key used for encryption.
//	params:
//		int * len:	ptr to an integer that describes the length of the
//					key that is being used
//		char ** key:	pass in a NULL pointer
//		FILE * source:	the location the key is being read from
//	return:
//		char ** key, (via ptr):	a char array pointer to the encryption key. 
//		int success: a value representing 0 if the function succeeded and 1 if the
//					function accepted 'key' as a non-NULL pointer or read no characters 
int cy_accept_key(int * len, char ** key, FILE * source)
{
	assert(*key == NULL);
	//LOCAL VARIABLES
	char buffer[MAX_CHAR_KEY] = {'\0'};// the read buffer cleared to NULL
	int inchar = 0;// input character
	int nchar = 0;// char counter 
	
	//EXECUTABLE STATEMENTS
	inchar = fgetc(source);
	while((inchar != EOF) && (nchar < MAX_CHAR_KEY - 1))
	{	
		//printf("%c", inchar);
		buffer[nchar] = (char)(inchar);
		++nchar;
		inchar = fgetc(source);
	}
	// catch errors
	if(!nchar) 
	{	fprintf(stderr, "Error: no characters read into key - key set to default\n");
		*len = 1;
		*key = strdup("M");// key is rot13
		return 1; 
	}
	// assign return values
	*len = nchar;
	*key = strdup(buffer);
	return 0;
}

///
//	Takes the key and returns an integer array that signifies the amount the key will
//	augment each character in the message text.
//	params:
//		int * len:	the number of characters in the keyu
//		char * key_raw:	the key as character data
//		int uncypher: bool to signify a cypher or reverse cypher
//	return:
//		int * key_ref: (pass in null) the key as integer data
//		
int * cy_refine_key(int * raw_len, char * key_raw, int uncypher) 
{
	//LOCAL VARIABLES
	int raw_i = 0;
	int ref_i = 0;
	int * key_refined;// the interpreted augment values
	int kchar = 0;// the value interpreted from the cypher
	int low = 1;// boolean for lowercase letter
	int ref_len = 0;

	//EXECUTABLE STATEMENTS
	// count actual needed int indices
	for(raw_i = 0; raw_i < *raw_len; ++raw_i)
		if(isalpha(key_raw[raw_i])) ++ref_len;
	
	// interate through raw key and pick out alpha characters
	key_refined = malloc(sizeof(int) * ref_len);
	for(raw_i = 0; raw_i < *raw_len; ++raw_i)
	{
		kchar = key_raw[raw_i];
		if(isalpha(kchar)) 
		{	low = islower(kchar);
			kchar -= low ? 97 : 65;// recenters letter position at 0;
			kchar *= uncypher ? -1 :  1;// A -> 1 shift,  
			kchar += uncypher ? 25 :  1;
			key_refined[ref_i] = kchar;
			++ref_i;
		}
	}

	*raw_len = ref_len;
	return(key_refined);
}

///
//	Takes the line and augments each char based on the corresponding key char.
//	params:
//		int keylen:	number of characters in the key
//		int * key:	NULL terminated char array being used
//		int * keypos:	starting char index in 'key'
//		char * line:	the NULL terminated char array being encrypted (overwritten)
//	
void cy_cypher_line(int keylen, int * key, int * keypos, char * line)
{
	//BASE VALIDATION
	assert(keylen > 0);
	assert(key != NULL);
	assert(*keypos >= 0 && *keypos < keylen);
	assert(line != NULL);

	//LOCAL VARIABLES
	int li = 0;// char index for 'line' 
	int low = 1;// boolean for lowercase letter
	int lchar = '\0';// the dereferenced char value at line[li]

	//EXECUTABLE STATEMENTS
	while(line[li] != '\0' && line[li] != EOF && li < MAX_CHAR_LINE) 
	{	
		{
			lchar = line[li];
			low = islower(lchar);
			lchar -= low ? 97 : 65;// recenters letter position at 0;
			lchar += key[*keypos];
			lchar %= 26;
			lchar += low ? 97 : 65;
			line[li] = lchar;
			*keypos = (*keypos + 1) % keylen;
			//*keypos += 1; 
			//*keypos %= keylen;
		}
		li += 1;
	}
	if(li >= MAX_CHAR_LINE)
		fprintf(stderr, "Error: buffer overread\n");
}


///
//	Appends a new queue containing a char array to the end of the current queue, or creates the
//	queue if the parameter 'queue' is a NULL pointer. 
//	params:
//		MsgQueue * queue_end:	current message queue or a NULL pointer if empty
//		char * msg:	char array being stored on the queue node after 'queue_end'
//	returns:
//		MsgQueue * queue_end:	the new end of the MsgQueue list
MsgQueue * cy_append_write_create(MsgQueue * queue_end, char * msg )
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
MsgQueue * cy_unqueue_read_free(MsgQueue * queue_front, char ** read)
{
	// base validation
	assert(queue_front != NULL);

	// retrieve the message from the victim and reassign the front of the queue
	MsgQueue * new_front = queue_front->next;
	*read = queue_front->line;	
	free(queue_front);
	return(new_front);
}


int main(int argc, char * argv[])
{
	//LOCAL MAIN VARIABLES
	FILE * message_fp = stdin;// target data stream of cypher
	FILE * key_fp = stdin;// the cypher data input stream
	FILE * output_fp = stdout;// the output file of the cypher'd data
	char * msg_filename = NULL;// 
	char * key_filename = NULL;// 
	char * out_filename = NULL;// 
	int uncypher = 0;// boolean, whether to run cypher or uncypher
	int key_length = 0;// the number of char in the key
	char * key_raw = NULL;// the key as readable to the user
	int * key_refined = NULL;// the key as integer augments
	int key_pos = 0;// the position of the cypher letter
	char * buffer = NULL;// the msg data read in one line
	MsgQueue * q_front = NULL;// the output end of the message queue
	MsgQueue * q_back = NULL;// the input end of the message queue

	int argi;

	//PARSE INPUTS
	if(argc <= 1) 
	{
		cy_usage();
		return 1;
	}
	for(argi = 1; argi < argc; ++argi)
	{
		if(!strcmp(argv[argi], "-i"))
		{	if((argi + 1) < argc) 
			msg_filename = strdup(argv[++argi]);
		}
		else if(!strcmp(argv[argi], "-k"))
		{	if((argi + 1) < argc) 
			key_filename = strdup(argv[++argi]);
		}
		else if(!strcmp(argv[argi], "-o"))
		{	if((argi + 1) < argc) 
			out_filename = strdup(argv[++argi]);
		}
		else if(!strcmp(argv[argi], "-u"))
		{	uncypher = 1;
		}

	}

	//EXECUTABLE STATEMENTS
	// accept the cyper being used 
	key_fp = cy_open_file(key_filename);
	cy_accept_key(&key_length, &key_raw, key_fp);
	key_refined = cy_refine_key(&key_length, key_raw, uncypher);
	fclose(key_fp);

	// accept the message data
	message_fp = cy_open_file(msg_filename);	
	buffer = malloc(sizeof(char) * MAX_CHAR_LINE);
	if(fgets(buffer, MAX_CHAR_LINE, message_fp))
	{
		q_front = cy_append_write_create(NULL, strdup(buffer));
		q_back = q_front;
		free(buffer);
		buffer = malloc(sizeof(char) * MAX_CHAR_LINE);
		while(fgets(buffer, MAX_CHAR_LINE, message_fp))
		{
			q_back = cy_append_write_create(q_back, strdup(buffer));
			free(buffer);
			buffer = malloc(sizeof(char) * MAX_CHAR_LINE);
		}
	}
	free(buffer);
	fclose(message_fp);

	// iterate through the message data, applying cypher, writing to output
	output_fp = cy_open_output_file(out_filename);
	while(q_front)
	{
		q_front = cy_unqueue_read_free(q_front, &buffer);
		cy_cypher_line(key_length, key_refined, &key_pos, buffer);
		fprintf(output_fp, "%s", buffer);
		free(buffer);
	}
	fclose(output_fp);

	// manage data
	free(key_raw);
	free(key_refined);
	free(key_filename);
	free(msg_filename);
	free(out_filename);

	return 0;
}
