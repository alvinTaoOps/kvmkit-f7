/*
 * mouse.h
 *
 *  Created on: Dec 31, 2020
 *      Author: Bowen Shaner
 */

#ifndef INC_MOUSE_H_
#define INC_MOUSE_H_

#include "stdint.h"

//#include "ascii_usb_charmap.h"

typedef int16_t HID_MOUSE_POS_t;
typedef uint8_t HID_WHEEL_t;


typedef struct USB_MOUSE_MSG_Struct {
	// Can a bit field work here or would it not serialize the same?
	uint8_t buttons; //TODO note buttons and blank bits depending on descriptor, until then --LSB FIRST--
	HID_MOUSE_POS_t pos_x;
	HID_MOUSE_POS_t pos_y;
	HID_WHEEL_t wheel;
}USB_MOUSE_MSG_t;


/*
 * sets mouse position
 * TODO how do we handle double click if the button fields might not be read on all three click-release-click?
 * 		maybe needs to flag on "read from usb"?
 */
void SetMouseState(uint8_t buttons, HID_MOUSE_POS_t pos_x, HID_MOUSE_POS_t pos_y, HID_WHEEL_t wheel);


/*
 * gets mouse position
 *
 * TODO pos_x, y, and buttons should be sticky (not cleared once it sends) but I'm not sure about mouse wheel
 */
USB_MOUSE_MSG_t GetMouseState();


#endif /* INC_MOUSE_H_ */
