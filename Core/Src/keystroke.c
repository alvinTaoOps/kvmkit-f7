/*
 * keystroke.c
 *
 *  Created on: Dec 31, 2020
 *      Author: Bowen Shaner
 */
#include "keystroke.h"
//#include "ascii_usb_charmap.h"

//#define DEFAULT_ON_NO_COMMAND HOLD
#define DEFAULT_ON_NO_COMMAND RELEASE

// Move to ascii_usb_charmap.h once implemented
// I want a hash table here, but I'm not sure if that'll work all that well on smaller micros, memory-contraint-wise
// Maybe just use the ascii codes as array indexes to an array of usb_key_msg structs?
typedef struct ASCII_USB_RELATION {
	char ascii_rep;
	USB_KEY_MSG_t usage_code_rep;
} ASCII_USB_RELATION_t;

static ASCII_USB_RELATION_t NO_EVENT_INDICATED = { .ascii_rep = 0x00 };


uint8_t overwrite_counter = 0;

USB_KEY_MSG_t key_buf[KEY_BUF_SIZE] = {0}; // ensure first element is zero for predictable hold behavior
uint8_t kb_ins = 1;
uint8_t kb_pop = 1;

// local methods
void iter_w_wrap(uint8_t * kb_select){ *kb_select = (*kb_select + 1) % KEY_BUF_SIZE; } // TODO: can this be a define method?
USB_KEY_MSG_t charToHidMessage(char input);


void InsertCharacter(char input)
{
	key_buf[kb_ins] = charToHidMessage(input);

	if ( (kb_ins + 1) % KEY_BUF_SIZE == kb_pop ) // If we are going to start overwriting unsent data
	{
		overwrite_counter++;
		iter_w_wrap(&kb_pop);
	}

	iter_w_wrap(&kb_ins);
}

#if DEFAULT_ON_NO_COMMAND==HOLD
uint8_t decr_w_wrap(uint8_t index)
{
	return (index - 1 + KEY_BUF_SIZE) % KEY_BUF_SIZE;
}
#endif


USB_KEY_MSG_t PopKey()
{
	USB_KEY_MSG_t ret;

	if ( kb_ins == kb_pop ) // if we are out of new data to send
	{
#if DEFAULT_ON_NO_COMMAND == HOLD

		ret = key_buf[decr_w_wrap(kb_pop)];
#else
		ret = NO_EVENT_INDICATED->usage_code_rep;
#endif
	}
	else
	{
		ret = key_buf[kb_pop];
		iter_w_wrap(&kb_pop);
	}

	return ret;
}

//TODO When popping, escape \+ denotes that the following keys are pressed simultaneously
//		(including the next one not preceded by \+)
USB_KEY_MSG_t charToHidMessage(char input)
{
	// TODO do the ascii lookup into the usb key combo table and return the real value
	return (NO_EVENT_INDICATED.usage_code_rep);
}
