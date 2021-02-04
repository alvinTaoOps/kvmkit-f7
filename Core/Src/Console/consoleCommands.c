// ConsoleCommands.c
// This is where you add commands:
//		1. Add a prototype
//			static eCommandResult_T ConsoleCommandVer(const char buffer[]);
//		2. Add the command to mConsoleCommandTable
//		    {"ver", &ConsoleCommandVer, HELP("Get the version string")},
//		3. Implement the function, using ConsoleReceiveParam<Type> to get the parameters from the buffer.

#include <Console/console.h>
#include <Console/consoleCommands.h>
#include <Console/consoleIo.h>
#include <HID/keystroke.h>
#include <HID/mouse.h>
#include <string.h>
#include <stdlib.h>
#include "version.h"

#define IGNORE_UNUSED_VARIABLE(x)     if ( &x == &x ) {}

static eCommandResult_T ConsoleCommandComment(const char buffer[]);
static eCommandResult_T ConsoleCommandVer(const char buffer[]);
static eCommandResult_T ConsoleCommandHelp(const char buffer[]);
static eCommandResult_T ConsoleCommandParamExampleInt16(const char buffer[]);
static eCommandResult_T ConsoleCommandParamExampleHexUint16(const char buffer[]);
static eCommandResult_T ConsoleCommandSetMouse(const char buffer[]);
static eCommandResult_T ConsoleCommandPutOneKey(const char buffer[]);
static eCommandResult_T ConsoleCommandPutLineKey(const char buffer[]);

static const sConsoleCommandTable_T mConsoleCommandTable[] =
{
    {";", &ConsoleCommandComment, HELP("Comment! You do need a space after the semicolon. ")},
    {"help", &ConsoleCommandHelp, HELP("Lists the commands available")},
    {"ver", &ConsoleCommandVer, HELP("Get the version string")},
    {"int", &ConsoleCommandParamExampleInt16, HELP("How to get a signed int16 from params list: int -321")},
    {"u16h", &ConsoleCommandParamExampleHexUint16, HELP("How to get a hex u16 from the params list: u16h aB12")},
    {"csm", &ConsoleCommandSetMouse, HELP("Set the mouse state: csm <5b buttons> <x pos> <y pos> <wheel>")},
    {"cpk", &ConsoleCommandPutOneKey, HELP("Send one keypress: cpk <8b mods> <key1> <k2> <k3> <k4> <k5> <k6>")},
    {"cpl", &ConsoleCommandPutLineKey, HELP("Send a series of keypresses")},

	CONSOLE_COMMAND_TABLE_END // must be LAST
};

static eCommandResult_T ConsoleCommandComment(const char buffer[])
{
	// do nothing
	IGNORE_UNUSED_VARIABLE(buffer);
	return COMMAND_SUCCESS;
}

static eCommandResult_T ConsoleCommandHelp(const char buffer[])
{
	uint32_t i;
	uint32_t tableLength;
	eCommandResult_T result = COMMAND_SUCCESS;

    IGNORE_UNUSED_VARIABLE(buffer);

	tableLength = sizeof(mConsoleCommandTable) / sizeof(mConsoleCommandTable[0]);
	for ( i = 0u ; i < tableLength - 1u ; i++ )
	{
		ConsoleIoSendString(mConsoleCommandTable[i].name);
#if CONSOLE_COMMAND_MAX_HELP_LENGTH > 0
		ConsoleIoSendString(" : ");
		ConsoleIoSendString(mConsoleCommandTable[i].help);
#endif // CONSOLE_COMMAND_MAX_HELP_LENGTH > 0
		ConsoleIoSendString(STR_ENDLINE);
	}
	return result;
}

static eCommandResult_T ConsoleCommandParamExampleInt16(const char buffer[])
{
	int16_t parameterInt;
	eCommandResult_T result;
	result = ConsoleReceiveParamInt16(buffer, 1, &parameterInt);
	if ( COMMAND_SUCCESS == result )
	{
		ConsoleIoSendString("Parameter is ");
		ConsoleSendParamInt16(parameterInt);
		ConsoleIoSendString(" (0x");
		ConsoleSendParamHexUint16((uint16_t)parameterInt);
		ConsoleIoSendString(")");
		ConsoleIoSendString(STR_ENDLINE);
	}
	return result;
}

static eCommandResult_T ConsoleCommandParamExampleHexUint16(const char buffer[])
{
	uint16_t parameterUint16;
	eCommandResult_T result;
	result = ConsoleReceiveParamHexUint16(buffer, 1, &parameterUint16);
	if ( COMMAND_SUCCESS == result )
	{
		ConsoleIoSendString("Parameter is 0x");
		ConsoleSendParamHexUint16(parameterUint16);
		ConsoleIoSendString(STR_ENDLINE);
	}
	return result;
}

static eCommandResult_T ConsoleCommandVer(const char buffer[])
{
	eCommandResult_T result = COMMAND_SUCCESS;

    IGNORE_UNUSED_VARIABLE(buffer);

	ConsoleIoSendString(VERSION_STRING);
	ConsoleIoSendString(STR_ENDLINE);
	return result;
}

static eCommandResult_T ConsoleCommandSetMouse(const char buffer[])
{
	eCommandResult_T result = COMMAND_SUCCESS;

	char buttonArray[CONSOLE_COMMAND_MAX_LENGTH];
	uint8_t buttonState;
	uint32_t argLen = 5;
	int16_t mouseX;
	int16_t mouseY;
	int16_t mouseWheel;

	result |= ConsoleReceiveParamString(buffer, 1, &argLen, buttonArray);
	buttonArray[argLen] = 'a'; // argLen gets changed to be the length of the returned paramater
							   // strtol reads until it sees a non-integer character so this is our stop flag
	result |= ConsoleReceiveParamInt16(buffer, 2, &mouseX);
	result |= ConsoleReceiveParamInt16(buffer, 3, &mouseY);
	result |= ConsoleReceiveParamInt16(buffer, 4, &mouseWheel);
	if ( COMMAND_SUCCESS == result )
	{
		buttonState = (uint8_t) strtol(buttonArray, NULL, 2);	// turn binary buttons string (10110) into uint8
		SetMouseState(buttonState, mouseX, mouseY, mouseWheel);
		ConsoleIoSendString(STR_ENDLINE);
	}
	return result;
}

/**
 *
 * "Send one keypress: cpk <mods> <key1> [<k2> <k3> <k4> <k5> <k6>]"
 */
static eCommandResult_T ConsoleCommandPutOneKey(const char buffer[])
{
	uint8_t param[7];
	memset(param, 0, 7);
	eCommandResult_T result;
	uint8_t param_count = 0;
/*
	// get our modifier byte as a binary string and convert to int
	char modifierArray[CONSOLE_COMMAND_MAX_LENGTH];
	uint32_t argLen = 8;
	result = ConsoleReceiveParamString(buffer, 1, &argLen, modifierArray);
	if ( COMMAND_SUCCESS == result )
	{
		param[0] = (uint8_t) strtol(modifierArray, NULL, 2);	// turn binary modifier string (b00000010) into uint8
	}
*/

	// get our key values as decimal
	while ( result == COMMAND_SUCCESS && param_count < 7 )
	{
		result = GetParamUInt8(buffer, param_count + 1, &param[param_count]);
		param_count++;
	}

	ConsoleIoSendString("Check one ");
	ConsoleSendParamInt16((uint16_t) result);
	ConsoleIoSendString(" ");
	ConsoleSendParamInt16((uint16_t) param_count);
	ConsoleIoSendString(STR_ENDLINE);

	if ( COMMAND_SUCCESS == result ) // Got all 6 keys
	{
		InsertHidKey(param[0], &param[1], param_count - 1);
	}
	else if (COMMAND_PARAMETER_END == result && param_count > 2) // Got < 6 keys
	{
		InsertHidKey(param[0], &param[1], param_count - 2); // we iterate param_count an extra time for the
															// PARAMETER_END read into the buffer so -1 more
		result = COMMAND_SUCCESS;
	}
	else // COMMAND_PARAMETER_ERROR, COMMAND_ERROR, or COMMAND_PARAMETER_END before <key1>
	{
		ConsoleIoSendString("Parameter Error: Make sure cpk arguments are binary (mods) or unsigned integers (keys)");
	}

	return result;
}

static eCommandResult_T ConsoleCommandPutLineKey(const char buffer[])
{
	char cmdArg[CONSOLE_COMMAND_MAX_LENGTH];
	eCommandResult_T result;
	uint32_t argLen = 0;

	result = ConsoleReceiveParamAllArgs(buffer, &argLen, cmdArg);
	if ( COMMAND_SUCCESS == result )
	{
		InsertCharacter(cmdArg, argLen);
		ConsoleIoSendString(STR_ENDLINE);
	}
	return result;
}


const sConsoleCommandTable_T* ConsoleCommandsGetTable(void)
{
	return (mConsoleCommandTable);
}


