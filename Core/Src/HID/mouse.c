/*
 * mouse.c
 *
 *  Created on: Dec 31, 2020
 *      Author: Bowen Shaner
 */

#include <HID/mouse.h>
#include <HID/usb_hid.h>


USB_MOUSE_MSG_t mouseState;


HID_StatusTypeDef USB_Mouse_SendKeys(USB_MOUSE_MSG_t * mouse_out);

/*
 * sets mouse position
 * TODO how do we handle double click if the button fields might not be read on all three click-release-click?
 * 		maybe needs to flag on "read from usb"?
 */
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

void SendMouseState()
{
	USB_Mouse_SendKeys(&mouseState);
}


/**
 * 	Immediately send a set of mouse inputs
 * 	We are fine using a pointer argument here (unlike keystroke) bc it doesn't matter if we drop one (for now?)
 */
HID_StatusTypeDef USB_Mouse_SendKeys(USB_MOUSE_MSG_t * mouse_out)
{
	mouse_out->REPORT_ID = HID_MOUSE_REPORT_ID;
	HID_StatusTypeDef ret = USB_HID_SendReport((uint8_t *) &mouse_out, sizeof(USB_MOUSE_MSG_t));
	return ret;
}
