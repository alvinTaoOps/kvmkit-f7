/*
 * ascii_usb_charmap.h
 *
 *  Created on: Jan 21, 2021
 *      Author: Bowen Shaner
 *
 */



#ifndef INC_HID_ASCII_USB_CHARMAP_H_
#define INC_HID_ASCII_USB_CHARMAP_H_

#define MAX_ASCII_REP_SIZE 5		// Allot characters to give options for escape strings
#define ESC_MAP_SIZE 9
#define ESC_CHAR '\\'
#define ESC_COMBINE '+'
#define ESC_MAP_ASCII '/'

// USB keyboard codes
#define USB_HID_MOD_NONE				0x00
#define USB_HID_MOD_LEFT_CTRL   		0x01
#define USB_HID_MOD_LEFT_SHIFT  		0x02
#define USB_HID_MOD_LEFT_ALT    		0x04
#define USB_HID_MOD_LEFT_GUI    		0x08
#define USB_HID_MOD_RIGHT_CTRL  		0x10
#define USB_HID_MOD_RIGHT_SHIFT 		0x20
#define USB_HID_MOD_RIGHT_ALT   		0x40
#define USB_HID_MOD_RIGHT_GUI   		0x80

#define KEY_NONE 						0x00
#define KEY_ERR_OVF 					0x01
#define USB_HID_ENTER					0x28
#define USB_HID_ESC						0x29
#define USB_HID_BACKSPACE				0x2A
#define USB_HID_TAB						0x2B
#define USB_HID_SPACE					0x2C

#define USB_HID_COMMA					0x36
#define USB_HID_DOT						0x37
#define USB_HID_FSLASH					0x38
#define USB_HID_CAPSLCK					0x39
#define USB_HID_F1						0x3A
#define USB_HID_F2						0x3B
#define USB_HID_F3						0x3C
#define USB_HID_F4						0x3D
#define USB_HID_F5						0x3E
#define USB_HID_F6						0x3F
#define USB_HID_F7						0x40
#define USB_HID_F8						0x41
#define USB_HID_F9						0x42
#define USB_HID_F10						0x43
#define USB_HID_F11						0x44
#define USB_HID_F12						0x45

#define USB_HID_SYSRQ 					0x46
#define USB_HID_PRINTSCREEN 			USB_HID_SYSRQ
#define USB_HID_SCROLLLOCK 				0x47
#define USB_HID_PAUSE 					0x48
#define USB_HID_INSERT 					0x49
#define USB_HID_HOME 					0x4A
#define USB_HID_PAGEUP 					0x4B
#define USB_HID_DELETE					0x4C
#define USB_HID_END						0x4D
#define USB_HID_PAGEDOWN				0x4E

#define USB_HID_RIGHT					0x4F
#define USB_HID_LEFT					0x50
#define USB_HID_DOWN					0x51
#define USB_HID_UP						0x52

#define USB_HID_MUTE 					0x7F
#define USB_HID_VOLUMEUP 				0x80
#define USB_HID_VOLUMEDOWN 				0x81

#define USB_HID_LEFTCTRL 				0xE0
#define USB_HID_LEFTSHIFT 				0xE1
#define USB_HID_LEFTALT 				0xE2
#define USB_HID_LEFTOS	 				0xE3
#define USB_HID_RIGHTCTRL 				0xE4
#define USB_HID_RIGHTSHIFT 				0xE5
#define USB_HID_RIGHTALT 				0xE6
#define USB_HID_RIGHTOS 				0xE7

#define USB_HID_MEDIA_PLAYPAUSE 		0xE8
#define USB_HID_MEDIA_STOPCD 			0xE9
#define USB_HID_MEDIA_PREVIOUSSONG 		0xEA
#define USB_HID_MEDIA_NEXTSONG 			0xEB
#define USB_HID_MEDIA_EJECTCD 			0xEC
// No volume re-trigger
#define USB_HID_MEDIA_WWW 				0xF0
#define USB_HID_MEDIA_BACK 				0xF1
#define USB_HID_MEDIA_FORWARD 			0xF2
#define USB_HID_MEDIA_STOP 				0xF3
#define USB_HID_MEDIA_FIND 				0xF4
#define USB_HID_MEDIA_SCROLLUP 			0xF5
#define USB_HID_MEDIA_SCROLLDOWN 		0xF6
#define USB_HID_MEDIA_EDIT 				0xF7
#define USB_HID_MEDIA_SLEEP 			0xF8
#define USB_HID_MEDIA_COFFEE 			0xF9
#define USB_HID_MEDIA_REFRESH 			0xFA
#define USB_HID_MEDIA_CALC 				0xFB

// redundant ASCII for escape shortcuts
#define USB_HID_C						0x06
#define USB_HID_V						0x19

#define USB_HID_BSLASH					0x31

typedef struct ASCII_USB_RELATION {
	char const ascii_rep[MAX_ASCII_REP_SIZE];
	USB_KEY_MSG_t usage_code_rep;
} ASCII_USB_RELATION_t;


uint8_t MapToHid(char * in_ascii, uint8_t len, USB_KEY_MSG_t * ret_hid);


#endif /* INC_HID_ASCII_USB_CHARMAP_H_ */
