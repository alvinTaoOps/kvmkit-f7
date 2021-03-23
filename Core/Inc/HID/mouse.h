/*
 * mouse.h
 *
 *  Created on: Dec 31, 2020
 *      Author: Bowen Shaner
 */

#ifndef INC_HID_MOUSE_H_
#define INC_HID_MOUSE_H_

#include "stdint.h"

#define HID_MOUSE_REPORT_ID 0x02 		// This must match the report descriptor

typedef int16_t HID_MOUSE_POS_t;
typedef uint8_t HID_WHEEL_t;

typedef struct USB_MOUSE_MSG_Struct {
	uint8_t REPORT_ID;
	// Can a bit field work here or would it not serialize the same?
	uint8_t buttons; //TODO note buttons and blank bits depending on descriptor, until then --LSB FIRST--
	HID_MOUSE_POS_t pos_x;
	HID_MOUSE_POS_t pos_y;
	HID_WHEEL_t wheel;
}USB_MOUSE_MSG_t;


void SetMouseState(uint8_t buttons, HID_MOUSE_POS_t pos_x, HID_MOUSE_POS_t pos_y, HID_WHEEL_t wheel);


USB_MOUSE_MSG_t GetMouseState();


#endif /* INC_HID_MOUSE_H_ */
