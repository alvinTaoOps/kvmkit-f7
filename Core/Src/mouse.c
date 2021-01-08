/*
 * mouse.c
 *
 *  Created on: Dec 31, 2020
 *      Author: Bowen Shaner
 */

#include "mouse.h"

USB_MOUSE_MSG_t mouseState;


void SetMouseState(uint8_t buttons, HID_MOUSE_POS_t pos_x, HID_MOUSE_POS_t pos_y, HID_WHEEL_t wheel)
{
	buttons << 3; buttons >> 3; // Clear the 3 MSB (reserved bits), leaving the five button state flags
	mouseState.buttons = buttons;
	mouseState.pos_x = pos_x;
	mouseState.pos_y = pos_y;
	mouseState.wheel = wheel;
}


/*
 * TODO pos_x, y, and buttons should be sticky (not cleared once sent) but I'm not sure about mouse wheel
 * TODO If this result doesn't slot right into the USB report, change the struct typedef so it makes the
 * 		right series of bytes. If that fails then change this into a little byte array serializer?
 * TODO Does this need to return a copy not a ref? If the uart interrupt updates after GET, that might
 * 		overrun a prior input, but the new command should have a better x/y anyway?
 */
USB_MOUSE_MSG_t GetMouseState()
{
	return mouseState;
}
