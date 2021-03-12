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
		.ascii_rep = {(char) 0x00},
		.usage_code_rep = {.modifiers = 0, .key1 = 0}
};

uint8_t Map_Escaped_Keys(char * target, USB_KEY_MSG_t * ret_hid);
void USB_KEY_MSG_Logical_Or(USB_KEY_MSG_t * hid_first_operand, USB_KEY_MSG_t * hid_second_operand);


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
 * 			e.g. '\ctrl+/a\' -> escape string ctrl (ctrl mod) plus ascii lookup 'a' key
 * Returns 1 if the mapping fails
 */
uint8_t MapToHid(char * in_ascii, uint8_t len, USB_KEY_MSG_t * ret_hid)
{
	uint8_t ret_val = 0;

	assert(len > 0);

	if ( len == 1 ){ 			// regular ascii character
		ret_val = map_ascii_to_hid(*in_ascii, ret_hid);

	} if ( len == 2 ){ 			// escaped ESC_CHAR
		assert(*(in_ascii)==ESC_CHAR && *(in_ascii+1)==ESC_CHAR);

		ret_val = map_ascii_to_hid(ESC_CHAR, ret_hid);

	} else { 					// full escape sequence
		assert(len > 2); // '\' + one or more chars + '\'
		assert(*(in_ascii)==ESC_CHAR && *(in_ascii+len-1)==ESC_CHAR);

		uint8_t target_start = 1;
		USB_KEY_MSG_t single_key;

		// Break up the escape string on the ESC_COMBINE operator ('+')
		for (uint8_t target_idx = 1; target_idx < len - 1; target_idx++)
		{
			if ((char) *(in_ascii + target_idx) == ESC_COMBINE)
			{
				in_ascii[target_idx] = ASCII_NULL; // label end of segment
				ret_val |= Map_Escaped_Keys(in_ascii + target_start, &single_key);
				USB_KEY_MSG_Logical_Or(ret_hid, &single_key);
				target_start = target_idx + 1;
			}
		}
		// Map the final escape segment
		in_ascii[len - 1] = ASCII_NULL;
		ret_val |= Map_Escaped_Keys(in_ascii + target_start, &single_key);
		USB_KEY_MSG_Logical_Or(ret_hid, &single_key);

	}
	return ret_val;
}

/**
 * Search the table of escape sequences or ascii mapping table to match the input
 * to HID key scan codes/modifiers.
 * If the first character is the escape ascii mapping character '/', the next character
 * is mapped from ascii and the rest of the input is ignored.
 * Returns 1 if mapping fails.
 */
uint8_t Map_Escaped_Keys(char * target, USB_KEY_MSG_t * ret_hid)
{
	if (target[0] == (char) ASCII_NULL) 		// empty input
	{
		return 0;
	} else if (target[0] == ESC_MAP_ASCII) 		// direct ascii mapping
	{
		return map_ascii_to_hid(target[1], ret_hid);
	}

	// otherwise, interpret the escape character
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

	return 0;
}


/**
 * Operator '|' implemented for two USB_KEY_MSG_t struct instances. Combines modifier and key presses.
 * Key overflow (6 are supported) gives the OVF error key scan codes. Both argument structs are
 * assigned the resultant value.
 */
void USB_KEY_MSG_Logical_Or(USB_KEY_MSG_t * hid_first_operand, USB_KEY_MSG_t * hid_second_operand)
{
	hid_first_operand->modifiers |= hid_second_operand->modifiers;
	hid_second_operand->modifiers = hid_first_operand->modifiers;

	uint8_t operand_key_count[2] = {0,0};
	USB_KEY_MSG_t * operand_arr[2] = {hid_first_operand, hid_second_operand};
	uint8_t * key_array[12] = {
			&operand_arr[0]->key1, &operand_arr[0]->key2, &operand_arr[0]->key3,
			&operand_arr[0]->key4, &operand_arr[0]->key5, &operand_arr[0]->key6,

			&operand_arr[1]->key1, &operand_arr[1]->key2, &operand_arr[1]->key3,
			&operand_arr[1]->key4, &operand_arr[1]->key5, &operand_arr[1]->key6
	};

	// count the number of pressed keys on each operand
	for (uint8_t i = 0; i < 12; i++)
	{
		if (*key_array[i] != 0) operand_key_count[(uint8_t) i/6]++;
	}

	// Too many keys pressed at once
	if (operand_key_count[0] + operand_key_count[1] > 6)
	{
		for (uint8_t i = 0; i < 12; i++)
		{
			*key_array[i] = KEY_ERR_OVF;
		}
	}
	else if (operand_key_count[0] + operand_key_count[1] == 0) // no keys pressed
	{
		// nop
	}
	else if (operand_key_count[0] == 0 && operand_key_count[1] > 0) // only keys on operand 1
	{
		operand_arr[0] = operand_arr[1];
	}
	else if (operand_key_count[1] == 0 && operand_key_count[0] > 0) // only keys on operand 0
	{
		operand_arr[1] = operand_arr[0];
	}
	else // combine keys on operands 0 and 1
	{
		uint8_t unique_keys[operand_key_count[0] + operand_key_count[1]];
		uint8_t unique_count = 0;

		for (uint8_t i = 0; i < 12; i++) // find unique keys
		{
			if (*key_array[i] != 0)
			{
				uint8_t u;
				for (u = 0; u < unique_count; u++)
				{
					if (*key_array[i] == unique_keys[u])
					{
						u = unique_count + 1;
					}
				}
				if (u == unique_count)
				{
					unique_keys[unique_count] = *key_array[i];
					unique_count++;
				}
			}
		}

		for (uint8_t i = 0; i < 6; i++) // write unique keys to our hid structs
		{
			*key_array[i] = unique_keys[unique_count];
			*key_array[i + 6] = unique_keys[unique_count];
			if ( --unique_count < 0 ) i = 6;
		}
	}
}
