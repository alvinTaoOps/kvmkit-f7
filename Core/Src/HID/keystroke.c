/*
 * keystroke.c
 *
 *  Created on: Dec 31, 2020
 *      Author: Bowen Shaner
 */
#include <HID/keystroke.h>
#include <HID/ascii_usb_charmap.h>
#include <HID/usb_hid.h>
#include <error_and_debug.h>

#include "string.h"
#include "stdio.h"

#define DEFAULT_HOLD 0
#define DEFAULT_RELEASE 1
//#define DEFAULT_ON_NO_COMMAND DEFAULT_HOLD
#define DEFAULT_ON_NO_COMMAND DEFAULT_RELEASE

const static USB_KEY_MSG_t NO_KEY_PRESS =
		{.modifiers=0, .key1=0, .key2=0, .key3=0, .key4=0, .key5=0, .key6=0};

USB_KEY_MSG_t key_buf[KEY_BUF_SIZE]; // ensure first element is zero for predictable hold behavior
uint8_t kb_ins = 1;
uint8_t kb_pop = 1;
uint8_t overwrite_counter = 0;

// local methods
static void iter_w_wrap(uint8_t * kb_select) { *kb_select = (*kb_select + 1) % KEY_BUF_SIZE; } // TODO: make this be a define method?
static HID_StatusTypeDef USB_Keyboard_SendKeys(USB_KEY_MSG_t keys);

/**
 * Push an hid keyboard message onto the buffer, handling insertion/removal pointers and
 * 	counting items that are overwritten before being read out of the buffer
 */
void InsertOnBuffer(USB_KEY_MSG_t hid_key)
{
	memcpy(&key_buf[kb_ins], &hid_key, sizeof(key_buf[kb_ins]));

	if ( (kb_ins + 1) % KEY_BUF_SIZE == kb_pop ) // If we are going to start overwriting unsent data
	{
		overwrite_counter++;
		iter_w_wrap(&kb_pop);
	}

	iter_w_wrap(&kb_ins);
}


/*
 * Write a character into the key buffer as an hid message
 */
void InsertCharacters(char * input, uint8_t n_char)
{
	int i = 0;
	char i_char;

	char esc_builder[MAX_ASCII_REP_SIZE + 2]; // adding in space for escape chars
	int esc_index = -1;
	int fail_to_find_char = 0;

	USB_KEY_MSG_t key_tmp;

	while ( i < n_char && !fail_to_find_char)
	{
		i_char = input[i];
		i++;

		if ( i_char == ESC_CHAR )
		{
			if ( esc_index == -1 )		// found opening escape character
			{
				esc_builder[0] = i_char;
				esc_index = 1;			// enable the esc_builder
			}
			else						// found closing escape character
			{
				esc_builder[esc_index] = i_char;
				if (MapToHid(esc_builder, ++esc_index, &key_tmp)){
					fail_to_find_char = 1;
				} else {
					InsertOnBuffer(key_tmp);
				}
				esc_index = -1;			// disable the esc_builder
			}
		}
		else
		{
			if ( esc_index < 0)			// non-escaped character
			{
				if (MapToHid(&i_char, 1, &key_tmp)){
					fail_to_find_char = 1;
				} else {
					InsertOnBuffer(key_tmp);
				}
			}
			else
			{
				esc_builder[esc_index] = i_char;  // add to the escape string
				++esc_index;
			}
		}

		if ( esc_index == MAX_ASCII_REP_SIZE )
		{
			assert(esc_index < MAX_ASCII_REP_SIZE);
			fail_to_find_char = 1;
		}
	}

#ifndef NDEBUG
	if (fail_to_find_char)
	{
		char err_msg[50];
		printf(err_msg, "Could not translate %c\n", i_char);
	}
#endif
}


/**
 * 	Push an HID message onto the key buffer
 */
void InsertHidKey(uint8_t modifiers, uint8_t keys[], uint8_t n_keys)
{
	USB_KEY_MSG_t hid_key;
	memset(&hid_key, 0, sizeof(hid_key));
	uint8_t * struct_members[] = {
			&hid_key.key1, &hid_key.key2, &hid_key.key3,
			&hid_key.key4, &hid_key.key5, &hid_key.key6
	}; // it would be easier to memcpy into the struct, but padding becomes an issue

	hid_key.modifiers = modifiers;

	for ( int i=0; i < n_keys; i++ ) {
		*struct_members[i] = keys[i];
	}

	InsertOnBuffer(hid_key);
}


#if DEFAULT_ON_NO_COMMAND == DEFAULT_HOLD
uint8_t decr_w_wrap(uint8_t index)
{
	return (index - 1 + KEY_BUF_SIZE) % KEY_BUF_SIZE;
}
#endif

/**
 * Send the next hid message in the buffer
 */
uint8_t SendKey()
{
	USB_KEY_MSG_t ret;

	uint8_t empty_buf = ( kb_ins == kb_pop );
	if ( empty_buf ) // if we are out of new data to send
	{
#if DEFAULT_ON_NO_COMMAND == DEFAULT_HOLD

		ret = key_buf[decr_w_wrap(kb_pop)];
#else
		return !empty_buf;
#endif
	}
	else
	{
		ret = key_buf[kb_pop];
		iter_w_wrap(&kb_pop);
	}

	if(USB_Keyboard_SendKeys(ret) != HID_OK)
	{
		Error_Handler_Context(__FILE__, __LINE__);
	}

#if DEFAULT_ON_NO_COMMAND == DEFAULT_RELEASE
	if(USB_Keyboard_SendKeys(NO_KEY_PRESS) != HID_OK)
	{
		Error_Handler_Context(__FILE__, __LINE__);
	}
#endif

	return !empty_buf;
}

void FlushKeyQueue()
{
	uint8_t is_next = 1;
	while ( is_next )
	{
		is_next = SendKey();
	}
}

/**
 * 	Immediately send a combination of keys and modifiers
 */
static HID_StatusTypeDef USB_Keyboard_SendKeys(USB_KEY_MSG_t keys)
{
	keys.REPORT_ID = HID_KEYBOARD_REPORT_ID;
	HID_StatusTypeDef ret = USB_HID_SendReport((uint8_t *) &keys, sizeof(USB_KEY_MSG_t));

	// We can't return until we know the pointer to our message struct is done being used
	while (USB_HID_IsBusy()) {
		asm("NOP");
	}

	return ret;
}

/**
 * 	Immediately send a string of characters as usb hid keypresses.
 * 	Releases all keys in between each character.
 */
void USB_Keyboard_SendString(char * s)
{
	USB_KEY_MSG_t keypress;
	uint8_t i = 0;
	while (*(s+i)) //until ascii null char
	{
		MapToHid(s+i, 1, &keypress);
		if(USB_Keyboard_SendKeys(keypress) != HID_OK)
		{
			Error_Handler_Context(__FILE__, __LINE__);
		}

		if(USB_Keyboard_SendKeys(NO_KEY_PRESS) != HID_OK)
		{
			Error_Handler_Context(__FILE__, __LINE__);
		}
		i++;
	}
}
