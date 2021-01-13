// Console IO is a wrapper between the actual in and output and the console code

#ifndef CONSOLE_IO_H
#define CONSOLE_IO_H

#include <stdint.h>
#include "stm32f7xx_hal.h"

#define NONBLOCKING_MODE
#define MIN(X, Y)		(((X) < (Y)) ? (X) : (Y))

typedef enum {CONSOLE_SUCCESS = 0u, CONSOLE_ERROR = 1u } eConsoleError;

extern UART_HandleTypeDef huart4; // Make this available so debug messages can be sent once the console starts

#ifdef NONBLOCKING_MODE
void UART_Int_Init();
void UART_Int_Deinit();
#endif
eConsoleError ConsoleIoInit(void);

eConsoleError ConsoleIoReceive(uint8_t *buffer, const uint32_t bufferLength, uint32_t *readLength);
eConsoleError ConsoleIoReceiveInt(uint8_t *buffer, const uint32_t bufferLength, uint32_t *readLength);
eConsoleError ConsoleIoSend(const uint8_t *buffer, const uint32_t bufferLength, uint32_t *sentLength);
eConsoleError ConsoleIoSendString(const char *buffer); // must be null terminated

#endif // CONSOLE_IO_H
