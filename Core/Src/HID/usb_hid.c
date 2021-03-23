/*
 * usb_hid.c
 *
 *  Created on: Mar 23, 2021
 *      Author: Bowen Shaner
 */
#include <HID/usb_hid.h>
#include "usbd_customhid.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

/**
 * Payloads must fit one of the report IDs defined in the HID configuration/Report
 * Blocks until the USB DMA has been configured, execution time can vary
 */
HID_StatusTypeDef USB_HID_SendReport(uint8_t * payload, uint8_t len)
{
	// TODO Hold multiple calls here w/ a flag, the method we call below isn't reentrant (BUSY_HID)

	// Is this a critical section? The interrupt might be going off here and getting the key stuck?
	// TODO try masking the button interrupt here!
	HAL_StatusTypeDef ret = USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, payload, len);
	// TODO handle HID_BUSY with some retries here

	HAL_Delay(15);

	return (HID_StatusTypeDef) ret;
}

uint8_t USB_HID_IsBusy()
{
	USBD_CUSTOM_HID_HandleTypeDef * hhid;
	hhid = hUsbDeviceFS.pClassData;
	return hhid->state;
}
