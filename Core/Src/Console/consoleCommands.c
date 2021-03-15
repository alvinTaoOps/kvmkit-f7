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
static eCommandResult_T ConsoleCommandSetMouse(const char buffer[]);
static eCommandResult_T ConsoleCommandPutOneKey(const char buffer[]);
static eCommandResult_T ConsoleCommandPutLineKey(const char buffer[]);
static eCommandResult_T ConsoleCommandFlushKeys(const char buffer[]);

static const sConsoleCommandTable_T mConsoleCommandTable[] =
{
    {";", &ConsoleCommandComment, HELP("Comment! You do need a space after the semicolon. ")},
    {"help", &ConsoleCommandHelp, HELP("Lists the commands available")},
    {"ver", &ConsoleCommandVer, HELP("Get the version string")},
    {"csm", &ConsoleCommandSetMouse, HELP("Set the mouse state: csm <buttons> <x pos> <y pos> <wheel>")},
    {"cpk", &ConsoleCommandPutOneKey, HELP("Send one keypress: cpk <mods> <key1> <k2> <k3> <k4> <k5> <k6>")},
    {"cpl", &ConsoleCommandPutLineKey, HELP("Send a series of keypresses")},
	{"flsh", &ConsoleCommandFlushKeys, HELP("Flush key buffer")},

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

	int16_t buttonState;
	int16_t mouseX;
	int16_t mouseY;
	int16_t mouseWheel;

	result |= ConsoleReceiveParamInt16(buffer, 1, &buttonState);
	result |= ConsoleReceiveParamInt16(buffer, 2, &mouseX);
	result |= ConsoleReceiveParamInt16(buffer, 3, &mouseY);
	result |= ConsoleReceiveParamInt16(buffer, 4, &mouseWheel);
	if ( COMMAND_SUCCESS == result )
	{
		SetMouseState((uint8_t) buttonState, mouseX, mouseY, mouseWheel);
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

	// get our key values as decimal
	while ( result == COMMAND_SUCCESS && param_count < 7 )
	{
		result = GetParamUInt8(buffer, param_count + 1, &param[param_count]);
		param_count++;
	}

	if ( COMMAND_SUCCESS == result ) // Got modifiers and all 6 keys
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
		ConsoleIoSendString("Parameter Error: Make sure cpk arguments are unsigned integers");
	}

	return result;
}

static eCommandResult_T ConsoleCommandPutLineKey(const char buffer[])
{
	char cmdArg[CONSOLE_COMMAND_MAX_LENGTH];
	eCommandResult_T result;
	uint32_t argLen = CONSOLE_COMMAND_MAX_LENGTH;

	result = ConsoleReceiveParamAllArgs(buffer, &argLen, cmdArg);
	if ( COMMAND_SUCCESS == result )
	{
		InsertCharacters(cmdArg, argLen);
	}
	return result;
}

static eCommandResult_T ConsoleCommandFlushKeys(const char buffer[])
{
	FlushKeyQueue();

	return COMMAND_SUCCESS;
}


const sConsoleCommandTable_T* ConsoleCommandsGetTable(void)
{
	return (mConsoleCommandTable);
}


