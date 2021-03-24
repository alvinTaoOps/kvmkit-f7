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

#define HID_MOUSE_POS_t int8_t
#define HID_WHEEL_t int8_t

typedef struct USB_MOUSE_MSG_Struct {
	uint8_t REPORT_ID;
	// Can a bit field work here or would it not serialize the same?
	uint8_t buttons; //TODO note buttons and blank bits depending on descriptor, until then --LSB FIRST--
	HID_MOUSE_POS_t pos_x;
	HID_MOUSE_POS_t pos_y;
	HID_WHEEL_t wheel;
	// uint8_t RESERVED; // Needed if sizeof(USB_MOUSE_MSG_t) < 5 for the STM32 HAL (if wheel is disabled)
}USB_MOUSE_MSG_t;


void MouseSetState(uint8_t buttons, HID_MOUSE_POS_t pos_x, HID_MOUSE_POS_t pos_y, HID_WHEEL_t wheel);

USB_MOUSE_MSG_t MouseGetState();
void MouseSendState();
void MouseSendFreshState();


#endif /* INC_HID_MOUSE_H_ */
