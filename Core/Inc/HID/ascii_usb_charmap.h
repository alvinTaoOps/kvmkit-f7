/*
 * ascii_usb_charmap.h
 *
 *  Created on: Jan 21, 2021
 *      Author: Bowen Shaner
 */



#ifndef INC_HID_ASCII_USB_CHARMAP_H_
#define INC_HID_ASCII_USB_CHARMAP_H_

#define MAX_ASCII_REP_SIZE 5		// Allow up to four characters to give lots of options for escape strings
#define ESC_MAP_SIZE 4
#define ESC_CHAR '\\'

// USB keyboard codes
#define USB_HID_MODIFIER_NONE			0x00
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

#define USB_HID_DELETE					0x4c
#define USB_HID_C						0x06
#define USB_HID_V						0x19
#define USB_HID_LF						0x28

typedef struct ASCII_USB_RELATION {
	char const ascii_rep[MAX_ASCII_REP_SIZE];
	USB_KEY_MSG_t usage_code_rep;
} ASCII_USB_RELATION_t;

extern const ASCII_USB_RELATION_t NO_EVENT_INDICATED;
extern const ASCII_USB_RELATION_t escape_string_hid_map[ESC_MAP_SIZE];


// TODO add map for char -> usb_hid for ascii 128+, where possible

uint8_t MapToHid(char * in_ascii, uint8_t len, USB_KEY_MSG_t * ret_hid);


#endif /* INC_HID_ASCII_USB_CHARMAP_H_ */
