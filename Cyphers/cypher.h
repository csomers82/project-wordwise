#ifndef CYPHER_H
#define CYPHER_H

#define MAX_CHAR_KEY 1024
#define MAX_CHAR_LINE 255

typedef struct MsgQueue {
	char *	line;
	struct MsgQueue * next;
} MsgQueue;

///
//	Displays the functionality of the program
//
void cy_usage();

///
//	Opens an input fd to read char data from.
//
//		char * filename: the filename which is meant to opened
//
FILE * cy_open_file(char * filename);

///
//	Opens an input fd to read char data from.
//
//		char * filename: the filename which is meant to opened
//
FILE * cy_open_output_file(char * filename);

///
//	Takes in the key used for encryption.
//	params:
//		int * len:	a pointer to an integer that describes the length of the
//					key that is being used
//		char ** key:	pass in a NULL pointer
//		FILE * source:	the location the key is being read from
//	return:
//		char ** key, (via ptr):	a char array pointer to the encryption key. 
//		int success: a value representing 0 if the function succeeded and 1 if the
//					function accepted 'key' as a non-NULL pointer or read no characters 
int cy_accept_key(int * len, char ** key, FILE * source);


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
int * cy_refine_key(int * len, char * key_raw, int uncypher);

///
//	Takes the line and augments each char based on the corresponding key char.
//	params:
//		int keylen:	number of characters in the key
//		int * key:	NULL terminated char array being used
//		int * keypos:	starting char index in 'key'
//		char * line:	the NULL terminated char array being encrypted (overwritten)
//
void cy_cypher_line(int keylen, int * key, int * keypos, char * line);

///
//	Appends a new queue containing a char array to the end of the current queue, or creates the
//	queue if the parameter 'queue' is a NULL pointer. 
//	params:
//		MsgQueue * queue_end:	current message queue or a NULL pointer if empty
//		char * msg:	char array being stored on the queue node after 'queue_end'
//	returns:
//		MsgQueue * queue_end:	the new end of the MsgQueue list
MsgQueue * cy_append_write_create(MsgQueue * queue_end, char * msg );


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
MsgQueue * cy_unqueue_read_free(MsgQueue * queue_front, char ** read);


#endif
