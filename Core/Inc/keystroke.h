/*
 * keystroke.h
 *
 *  Created on: Dec 31, 2020
 *      Author: Bowen Shaner
 */

#ifndef INC_KEYSTROKE_H_
#define INC_KEYSTROKE_H_

#include "stdint.h"

#define KEY_BUF_SIZE 100

typedef struct USB_KEY_MSG_Struct {
	uint8_t modifiers;
	uint8_t rsvd;
	uint8_t key1;
	uint8_t key2;
	//add more simultaneous keys
}USB_KEY_MSG_t;

/*
 * Write a character into the key buffer
 * (puts character in and increments *kb_ins, pushes *kb_pop forward if it is next index)
 */
void InsertCharacter(char input);

//TODO When popping, escape \+ denotes that the prior and next keys are pressed simultaneously


/*
 * Get the next set of presses off of the key buffer, converted into a USB compatible value
 * This includes simultaneous and modifier keys (and-ed). Simultaneous keys are preceded by \+ e.g. \+(alt)\+1\+23 is alt+1+2+3
 * (gets character from kb_pop, increments kb_pop unless *kb_in is the next index)
 */
USB_KEY_MSG_t PopKey();


#endif /* INC_KEYSTROKE_H_ */
