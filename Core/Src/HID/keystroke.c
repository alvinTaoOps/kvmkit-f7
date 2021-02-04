/*
 * keystroke.c
 *
 *  Created on: Dec 31, 2020
 *      Author: Bowen Shaner
 */
#include <HID/keystroke.h>
#include <HID/ascii_usb_charmap.h>
#include "usbd_customhid.h"

#define DEFAULT_HOLD 0
#define DEFAULT_RELEASE 1
//#define DEFAULT_ON_NO_COMMAND DEFAULT_HOLD
#define DEFAULT_ON_NO_COMMAND DEFAULT_RELEASE

extern USBD_HandleTypeDef hUsbDeviceFS;

const static USB_KEY_MSG_t NO_KEY_PRESS =
		{.modifiers=0, .key1=0, .key2=0, .key3=0, .key4=0, .key5=0, .key6=0};

USB_KEY_MSG_t key_buf[KEY_BUF_SIZE]; // ensure first element is zero for predictable hold behavior
uint8_t kb_ins = 1;
uint8_t kb_pop = 1;
uint8_t overwrite_counter = 0;

// local methods
void iter_w_wrap(uint8_t * kb_select){ *kb_select = (*kb_select + 1) % KEY_BUF_SIZE; } // TODO: make this be a define method?
USB_KEY_MSG_t charToHidMessage(char * input, uint8_t n_char);
HAL_StatusTypeDef USB_Keyboard_SendKeys(USB_KEY_MSG_t keys);

/**
 * Push an hid keyboard message onto the buffer, handling insertion/removal pointers and
 * 	counting items that are overwritten before being read out of the buffer
 */
void InsertOnBuffer(USB_KEY_MSG_t hid_key)
{
	memcpy(&key_buf[kb_ins], &hid_key, sizeof(key_buf[kb_ins]));

	if ( (kb_ins + 1) % KEY_BUF_SIZE == kb_pop ) // If we are going to start overwriting unsent data
	{
		overwrite_counter++;
		iter_w_wrap(&kb_pop);
	}

	iter_w_wrap(&kb_ins);
}

/*
 * Write a character into the key buffer as an hid message
 */
void InsertCharacter(char * input, uint8_t n_char)
{
	//TODO loop through the input array
		// look ahead for escapes/keycombos (so library only needs the atomic combinations)
		// send each atomic press to charToHid
		// handle any illegal combinations
		// insert the result onto the buffer
	InsertOnBuffer(charToHidMessage(input, n_char));
}

/**
 * 	Push an HID message onto the key buffer
 */
void InsertHidKey(uint8_t modifiers, uint8_t keys[], uint8_t n_keys)
{
	USB_KEY_MSG_t hid_key;
	memset(&hid_key, 0, sizeof(hid_key));
	uint8_t * struct_members[] = {
			&hid_key.key1, &hid_key.key2, &hid_key.key3,
			&hid_key.key4, &hid_key.key5, &hid_key.key6
	}; // it would be easier to memcpy into the struct, but padding becomes an issue

	hid_key.modifiers = modifiers;

	for ( int i=0; i < n_keys; i++ ) {
		*struct_members[i] = keys[i];
	}

	InsertOnBuffer(hid_key);
}

#if DEFAULT_ON_NO_COMMAND == DEFAULT_HOLD
uint8_t decr_w_wrap(uint8_t index)
{
	return (index - 1 + KEY_BUF_SIZE) % KEY_BUF_SIZE;
}
#endif

/**
 * Send the next hid message in the buffer
 */
uint8_t SendKey()
{
	USB_KEY_MSG_t ret;

	uint8_t empty_buf = ( kb_ins == kb_pop );
	if ( empty_buf ) // if we are out of new data to send
	{
#if DEFAULT_ON_NO_COMMAND == DEFAULT_HOLD

		ret = key_buf[decr_w_wrap(kb_pop)];
#else
		ret = NO_EVENT_INDICATED.usage_code_rep;
#endif
	}
	else
	{
		ret = key_buf[kb_pop];
		iter_w_wrap(&kb_pop);
	}

	if(USB_Keyboard_SendKeys(ret) != HAL_OK)
	{
		Error_Handler_Context(__FILE__, __LINE__);
	}
	HAL_Delay(15);

	if(USB_Keyboard_SendKeys(NO_KEY_PRESS) != HAL_OK)
	{
		Error_Handler_Context(__FILE__, __LINE__);
	}
	HAL_Delay(15);

	return !empty_buf;
}

void FlushKeyQueue()
{
	uint8_t is_next = 1;
	while ( is_next )
	{
		is_next = SendKey();
	}
}

/*
 * Use the ascii-hid keycode lookup tables to turn a string of chars and modifiers
 * 		into a keyboard hid message
 */
USB_KEY_MSG_t charToHidMessage(char * input, uint8_t n_char)
{
	// TODO do the ascii lookup into the usb key combo table and return the real message
	//return (NO_EVENT_INDICATED.usage_code_rep);

	uint8_t HID_buffer[8];
	memset(HID_buffer, 0, 8);
	USB_KEY_MSG_t hid_msg;
	memset(&hid_msg, 0, sizeof(hid_msg));

	//assert(n_char > 0); //TODO implement assertions

	char ch = input[0];

	if(ch >= 'a' && ch <= 'z')
		{
			HID_buffer[0] = 0;
			// convert ch to HID letter, starting at a = 4
			HID_buffer[2] = (uint8_t)(4 + (ch - 'a'));
		}
		else if(ch >= 'A' && ch <= 'Z')
		{
			// Add left shift
			HID_buffer[0] = USB_HID_MODIFIER_LEFT_SHIFT;
			// convert ch to lower case
			ch = ch - ('A'-'a');
			// convert ch to HID letter, starting at a = 4
			HID_buffer[2] = (uint8_t)(4 + (ch - 'a'));
		}
		else if(ch >= '0' && ch <= '9') // Check if number
		{
			HID_buffer[0] = 0;
			// convert ch to HID number, starting at 1 = 30, 0 = 39
			if(ch == '0')
			{
				HID_buffer[2] = 39;
			}
			else
			{
				HID_buffer[2] = (uint8_t)(30 + (ch - '1'));
			}
		}
		else // not a letter nor a number
		{
			switch(ch)
			{
				case ' ':
					HID_buffer[0] = 0;
					HID_buffer[2] = 44;
					break;
				case '.':
					HID_buffer[0] = 0;
					HID_buffer[2] = 55;
					break;
				case '\n':
					HID_buffer[0] = 0;
					HID_buffer[2] = 40;
					break;
				case '!':
					//combination of shift modifier and key
					HID_buffer[0] = USB_HID_MODIFIER_LEFT_SHIFT;	// shift
					HID_buffer[2] = 30; // number 1
					break;
				case '?':
					//combination of shift modifier and key
					HID_buffer[0] = USB_HID_MODIFIER_LEFT_SHIFT;	// shift
					HID_buffer[2] = USB_HID_FSLASH; // key '/'
					break;
				case '@':
					//combination of shift modifier and key
					HID_buffer[0] = USB_HID_MODIFIER_LEFT_SHIFT;	// shift
					HID_buffer[2] = 31; // number 2
					break;
				default:
					HID_buffer[0] = 0;
					HID_buffer[2] = 0; // not implemented
			}
		}

	hid_msg.modifiers = HID_buffer[0];
	hid_msg.key1 = HID_buffer[2];

	return hid_msg;
}

/**
 * 	Immediately send a combination of keys and modifiers
 * 	TODO: register a timer to call this function and send the next keypress in the queue (if any) every 15ms, else use hold behavior
 */
HAL_StatusTypeDef USB_Keyboard_SendKeys(USB_KEY_MSG_t keys)
{
	// Is this a critical section? The interrupt might be going here and getting the key stuck?
	// TODO try masking the button interrupt here!
	HAL_StatusTypeDef ret = USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, &keys.modifiers, 8);

	HAL_Delay(15);

	return ret;
}

/**
 * 	Immediately send a string of characters as usb hid keypresses.
 * 	Releases all keys in between each character.
 */
void USB_Keyboard_SendString(char * s)
{
	USB_KEY_MSG_t keypress;
	uint8_t i = 0;
	while (*(s+i)) //until ascii null char
	{
		keypress = charToHidMessage(s+i, 1);
		if(USB_Keyboard_SendKeys(keypress) != HAL_OK)
		{
			Error_Handler_Context(__FILE__, __LINE__);
		}
		HAL_Delay(15);
		if(USB_Keyboard_SendKeys(NO_KEY_PRESS) != HAL_OK)
		{
			Error_Handler_Context(__FILE__, __LINE__);
		}
		HAL_Delay(15);
		i++;
	}
}
