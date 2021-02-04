/*
 * ascii_usb_charmap.h
 *
 *  Created on: Jan 21, 2021
 *      Author: Bowen Shaner
 */

#ifndef INC_HID_ASCII_USB_CHARMAP_H_
#define INC_HID_ASCII_USB_CHARMAP_H_

// USB keyboard codes
#define USB_HID_MODIFIER_LEFT_CTRL   	0x01
#define USB_HID_MODIFIER_LEFT_SHIFT  	0x02
#define USB_HID_MODIFIER_LEFT_ALT    	0x04
#define USB_HID_MODIFIER_LEFT_GUI    	0x08 // (Win/Apple/Meta)
#define USB_HID_MODIFIER_RIGHT_CTRL  	0x10
#define USB_HID_MODIFIER_RIGHT_SHIFT 	0x20
#define USB_HID_MODIFIER_RIGHT_ALT   	0x40
#define USB_HID_MODIFIER_RIGHT_GUI   	0x80

#define USB_HID_SPACE					0x2C
#define USB_HID_DOT						0x37
#define USB_HID_NEWLINE					0x28
#define USB_HID_FSLASH					0x38
#define USB_HID_BSLASH					0x31
#define USB_HID_COMMA					0x36

// I want a hash table here, but I'm not sure if that'll work all that well on smaller micros, memory-contraint-wise
// Maybe just use the ascii codes as array indexes to an array of usb_key_msg structs?
typedef struct ASCII_USB_RELATION {
	char ascii_rep;
	USB_KEY_MSG_t usage_code_rep;
} ASCII_USB_RELATION_t;

ASCII_USB_RELATION_t NO_EVENT_INDICATED = {
		.ascii_rep = 0x00,
		.usage_code_rep = {.modifiers = 0, .key1 = 0}
};


#endif /* INC_HID_ASCII_USB_CHARMAP_H_ */
