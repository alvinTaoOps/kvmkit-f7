/*
 * mouse.c
 *
 *  Created on: Dec 31, 2020
 *      Author: Bowen Shaner
 */

#include <HID/mouse.h>

USB_MOUSE_MSG_t mouseState;


void SetMouseState(uint8_t buttons, HID_MOUSE_POS_t pos_x, HID_MOUSE_POS_t pos_y, HID_WHEEL_t wheel)
{
	mouseState.buttons = buttons & 0b00011111; // 5 button flags are 5 LSb
	mouseState.pos_x = pos_x;
	mouseState.pos_y = pos_y;
	mouseState.wheel = wheel;
}


/*
 * TODO absolute x, y, and buttons should be sticky (not cleared once sent), not sure about clearing mouse wheel, relative x,y
 */
USB_MOUSE_MSG_t GetMouseState()
{
	return mouseState;
}
