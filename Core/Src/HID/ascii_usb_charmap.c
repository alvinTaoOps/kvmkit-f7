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
#define ESC_CHAR '\\'

const ASCII_USB_RELATION_t NO_EVENT_INDICATED = {
		.ascii_rep = {0x00},
		.usage_code_rep = {.modifiers = 0, .key1 = 0}
};


// I want a hash table here, but I'm not sure if that'll work all that well on smaller micros, memory-contraint-wise
// Maybe just use the ascii codes as array indexes to an array of usb_key_msg structs?
// If you want a key combo that isn't available here, better enter it directly w/ cpk
const ASCII_USB_RELATION_t escape_string_hid_map[10] = {
		{ .ascii_rep = "cad",
		  .usage_code_rep = {.modifiers = USB_HID_MODIFIER_LEFT_CTRL|USB_HID_MODIFIER_LEFT_ALT,
							 .key1 = USB_HID_DELETE }},
		{ .ascii_rep = "c",
		  .usage_code_rep = {.modifiers = USB_HID_MODIFIER_LEFT_CTRL,
						     .key1 = USB_HID_C }},
		{ .ascii_rep = "v",
		  .usage_code_rep = {.modifiers = USB_HID_MODIFIER_LEFT_CTRL,
							 .key1 = USB_HID_V }}
};


/**
 * Map an input to a USB HID keyboard message, input options are:
 * 		- one ascii char [0,128): pointer + len=1
 * 		- an escaped char array, from the header table: pointer(first char is '\') + len>1
 * 	@param
 *
 */
uint8_t map_to_hid(char * in_ascii, uint8_t len, USB_KEY_MSG_t * ret_hid)
{
	assert(len > 0);
	if ( len == 1 ){
		return map_ascii_to_hid(*in_ascii, ret_hid);
	} if ( len == 2 ){
		assert(*(in_ascii)==ESC_CHAR && *(in_ascii+1)==ESC_CHAR);
		return map_ascii_to_hid(ESC_CHAR, ret_hid);
	} else {
		assert(len > 2); // '\' + one or more chars + '\'
		int j = NO_MATCH;
		for (int i = 0; i < len && j == NO_MATCH; i++)
		{

		}
		if ( j == NO_MATCH ){
			return 1;
		}
	}
	return 0;
}
