/*
 * mouse.c
 *
 *  Created on: Dec 31, 2020
 *      Author: Bowen Shaner
 */

#include <HID/mouse.h>
#include <HID/usb_hid.h>


USB_MOUSE_MSG_t mouseState;
uint8_t freshMouseState = 0;

HID_StatusTypeDef USB_Mouse_SendKeys(USB_MOUSE_MSG_t * mouse_out);

/*
 * sets mouse position
 * TODO how do we handle double click if the button fields might not be read on all three click-release-click?
 * 		maybe needs wait on freshness flag to clear?
 */
void MouseSetState(uint8_t buttons, HID_MOUSE_POS_t pos_x, HID_MOUSE_POS_t pos_y, HID_WHEEL_t wheel)
{
	mouseState.buttons = buttons & 0b00000111; // 3 button flags are 3 LSb
	mouseState.pos_x = pos_x;
	mouseState.pos_y = pos_y;
	mouseState.wheel = wheel;

	freshMouseState = 1;
}


USB_MOUSE_MSG_t GetMouseState()
{
	return mouseState;
}

void MouseSendState()
{
	USB_Mouse_SendKeys(&mouseState);
	freshMouseState = 0;
}

void MouseSendFreshState()
{
	if (freshMouseState > 0)
	{
		MouseSendState();
	}
}


/**
 * 	Immediately send a set of mouse inputs
 * 	We are fine using a pointer argument here (unlike keystroke) bc it doesn't matter if we drop one (for now?)
 */
HID_StatusTypeDef USB_Mouse_SendKeys(USB_MOUSE_MSG_t * mouse_out)
{
	mouse_out->REPORT_ID = HID_MOUSE_REPORT_ID;
	HID_StatusTypeDef ret = USB_HID_SendReport((uint8_t *) mouse_out, sizeof(USB_MOUSE_MSG_t));
	return ret;
}
