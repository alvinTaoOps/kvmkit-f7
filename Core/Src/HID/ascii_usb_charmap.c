/*
 * ascii_usb_charmap.c
 *
 *  Created on: Feb 4, 2021
 *      Author: Bowen Shaner
 */

#include <assert.h>

#include "HID/keyboard.h"
#include "HID/ascii_usb_charmap.h"

#define NO_MATCH -1
#define MATCH 0
#define ASCII_NULL 0x00

const ASCII_USB_RELATION_t NO_EVENT_INDICATED = {
		.ascii_rep = (char) 0x00,
		.usage_code_rep = {.modifiers = 0, .key1 = 0}
};


// tiny db if this gets big? TODO: measure if lookup time is a problem when we walk this
// If you want a key combo that isn't available here, enter directly w/ cpk or use '+' compose
// (Maintained with csv_to_escape_table.py)
const ASCII_USB_RELATION_t escape_string_hid_map[ESC_MAP_SIZE] = {
		{ .ascii_rep = "cad",
		  .usage_code_rep = {.modifiers = USB_HID_MOD_LEFT_CTRL|USB_HID_MOD_LEFT_ALT,
							 .key1 = USB_HID_DELETE }},
		{ .ascii_rep = "c",
		  .usage_code_rep = {.modifiers = USB_HID_MOD_LEFT_CTRL,
							 .key1 = USB_HID_C }},
		{ .ascii_rep = "v",
		  .usage_code_rep = {.modifiers = USB_HID_MOD_LEFT_CTRL,
							 .key1 = USB_HID_V }},
		{ .ascii_rep = "n",
		  .usage_code_rep = {.modifiers = USB_HID_MOD_NONE,
							 .key1 = USB_HID_ENTER }},
		{ .ascii_rep = "bs",
		  .usage_code_rep = {.modifiers = USB_HID_MOD_NONE,
							 .key1 = USB_HID_BACKSPACE }},
		{ .ascii_rep = "del",
		  .usage_code_rep = {.modifiers = USB_HID_MOD_NONE,
							 .key1 = USB_HID_DELETE }},
		{ .ascii_rep = "esc",
		  .usage_code_rep = {.modifiers = USB_HID_MOD_NONE,
							 .key1 = USB_HID_ESC }},
		{ .ascii_rep = "os",
		  .usage_code_rep = {.modifiers = USB_HID_MOD_NONE,
							 .key1 = USB_HID_ENTER }},
		{ .ascii_rep = "tab",
		  .usage_code_rep = {.modifiers = USB_HID_MOD_NONE,
							 .key1 = USB_HID_TAB }},
};


/**
 * Map an input to a USB HID keyboard message, input options are:
 * 		- one ascii char [0,128): pointer + len=1
 * 		- an escaped char array, from the header table: pointer(first char is '\') + len>1
 */
uint8_t MapToHid(char * in_ascii, uint8_t len, USB_KEY_MSG_t * ret_hid)
{
	// TODO: increase this size to fit '+' composed escape strings. 6 * MAX_ASCII_REP_SIZE + 7?
	char target[MAX_ASCII_REP_SIZE + 2];

	assert(len > 0);
	if ( len == 1 ){
		return map_ascii_to_hid(*in_ascii, ret_hid);

	} if ( len == 2 ){
		assert(*(in_ascii)==ESC_CHAR && *(in_ascii+1)==ESC_CHAR);

		return map_ascii_to_hid(ESC_CHAR, ret_hid);

	} else {
		assert(len > 2); // '\' + one or more chars + '\'
		assert(*(in_ascii)==ESC_CHAR && *(in_ascii+len-1)==ESC_CHAR);

		memcpy(target, in_ascii + 1, (long unsigned int) len - 1);
		target[len - 2] = ASCII_NULL;

		// TODO: Decompose the escape string into '+' delimited segments, combine all buttons and mods for result
		//			e.g. '\ctrl+\a\' -> escape string ctrl (ctrl mod) plus ascii lookup 'a' key

		int j = NO_MATCH;
		for (int i = 0; i < ESC_MAP_SIZE && j == NO_MATCH; i++)
		{
			if (strcmp(escape_string_hid_map[i].ascii_rep, target) == 0)
			{
				*ret_hid = escape_string_hid_map[i].usage_code_rep;
				j = MATCH;
			}
		}
		if ( j == NO_MATCH ){
			return 1;
		}
	}
	return 0;
}
