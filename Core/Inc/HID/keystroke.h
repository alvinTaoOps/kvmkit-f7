/*
 * keystroke.h
 *
 *  Created on: Dec 31, 2020
 *      Author: Bowen Shaner
 */

#ifndef INC_HID_KEYSTROKE_H_
#define INC_HID_KEYSTROKE_H_

#include "stdint.h"
#include <assert.h>

#define KEY_BUF_SIZE 100

#define HID_KEYBOARD_REPORT_ID 0x01 		// This must match the report descriptor

/*
 * This struct is used as a uint8[], so it can't be __packed__
 * modifiers could be changed to bit fields if the command console arguments change
 */
typedef struct USB_KEY_MSG_Struct {
	uint8_t REPORT_ID;
	uint8_t modifiers;
	uint8_t RESERVED;
	uint8_t key1;
	uint8_t key2;
	uint8_t key3;
	uint8_t key4;
	uint8_t key5;
	uint8_t key6;
}USB_KEY_MSG_t;
static_assert(sizeof(USB_KEY_MSG_t) <= 9, "Too large for HID profile");

void InsertCharacters(char * input, uint8_t n_char);
void InsertHidKey(uint8_t modifiers, uint8_t * keys, uint8_t n_keys);

void USB_Keyboard_SendString(char * s);


/*
 * Get the next set of presses off of the key buffer, converted into a USB compatible value
 * This includes simultaneous and modifier keys (and-ed). Simultaneous keys are preceded by \+ e.g. \+(alt)\+1\+23 is alt+1+2+3
 * (gets character from kb_pop, increments kb_pop unless *kb_in is the next index)
 */
uint8_t SendKey();
void FlushKeyQueue();


#endif /* INC_HID_KEYSTROKE_H_ */
