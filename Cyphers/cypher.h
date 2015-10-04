#ifndef CYPHER_H
#define CYPHER_H

#include <string.h>
#include <ctypes.h>
#include "cypher.h"


typedef struct MsgQue {
	char *	line;
	MsgQue*	next;
} MsgQue;


///
//	Opens an input fd to read char data from.
//
//		char * filename: the filename which is meant to opened
//
FILE * cy_open_message_file(char * filename);

///
//	Takes in the key used for encryption.
//	params:
//		int * len:	a pointer to an integer that describes the length of the
//					key that is being used
//		char**key:	A NULL pointer that will be assigned to the char array pointer
//					representing the encryption key. 
//	return:
//		int success: a value representing 0 if the function succeeded and 1 if the
//					function accepted 'key' as a non-NULL pointer or read no characters 
int cy_accept_key(int * len, char ** key);

///
//	Takes the line and augments each char based on the corresponding key char.
//	params:
//		int keylen:	number of characters in the key
//		char * key:	NULL terminated char array being used
//		int * keypos:	starting char index in 'key'
//		char * line:	the NULL terminated char array being encrypted (overwritten)
//	return:
//		int success:	indicated 0 if the function succeeded and 1 if the input param
//						-eters were faulty.
int cy_cyper_line(int keylen, char * key, int * keypos, char * line);

///
//	Takes the line and decrements each char based on the corresponding key char.
//	params:
//		int keylen:	number of characters in the key
//		char * key:	NULL terminated char array being used
//		int * keypos:	starting char index in 'key'
//		char * line:	the NULL terminated char array being encrypted (overwritten)
//	return:
//		int success:	indicated 0 if the function succeeded and 1 if the input param
//						-eters were faulty.
int cy_uncyper_line(int keylen, char * key, int * keypos, char * line);

///
//	Appends a new que containing a char array to the end of the current que, or creates the
//	que if the parameter 'que' is a NULL pointer. 
//	params:
//		MsgQue * que_end:	current message que or a NULL pointer if empty
//		char * msg:	char array being stored on the que node after 'que_end'
//	returns:
//		MsgQue * que_end:	the new end of the MsgQue list
MsgQue * cy_append_write_create(MsgQue * que_end, char * msg );


///
//	Pops the front que node off of 'que_front' and points 'read' to the line char Array 
//	contained inside. The popped node is free'd
//	params:
//		MsgQue * que_front:	current front of the MsgQue list
//		char ** read:	pass in as a NULL pointer	
//	return:
//		char ** read, (via ptr):	pointer to the popped node's char Array
//		MsgQue * que_front:	new front node in the MsgQue list
//		
MsgQue * cy_unque_read_free(MsgQue * que_front, char ** read);


#endif
