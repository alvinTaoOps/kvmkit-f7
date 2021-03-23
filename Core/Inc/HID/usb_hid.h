/*
 * usb_hid.h
 *
 *  Created on: Mar 23, 2021
 *      Author: Bowen Shaner
 */

#ifndef INC_HID_USB_HID_H_
#define INC_HID_USB_HID_H_

#include "stdint.h"

typedef enum {
	HID_OK       	= 0x0U,
	HID_ERROR    	= 0x1U,
	HID_BUSY     	= 0x2U,
	HID_TIMEOUT		= 0x3U
} HID_StatusTypeDef; //Shadows HAL_StatusTypeDef for [0x0,0x3], higher values may be custom

HID_StatusTypeDef USB_HID_SendReport(uint8_t * payload, uint8_t len);

uint8_t USB_HID_IsBusy();

#endif /* INC_HID_USB_HID_H_ */
