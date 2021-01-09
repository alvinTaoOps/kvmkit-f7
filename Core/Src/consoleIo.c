// Console IO is a wrapper between the actual in and output and the console code
// In an embedded system, this might interface to a UART driver.

#include "consoleIo.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

#define UART_BUFFER_SIZE 256

extern UART_HandleTypeDef huart4;

volatile uint8_t rx_buf[UART_BUFFER_SIZE];
volatile uint8_t rx_buf_ins = 0;
volatile uint8_t overflow_count = 0;

uint32_t discard;

int _write(int file, char *outgoing, int len) {
   HAL_UART_Transmit(&huart4, (unsigned char *) outgoing, (uint16_t) len, (uint32_t) 100); //TODO: verify timeout duration
   return len;
}

#ifdef NONBLOCKING_MODE
#define MIN(X, Y)		(((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y)		(((X) > (Y)) ? (X) : (Y))
// Override HAL
void __HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	__HAL_UART_ENABLE_IT(&huart4, UART_IT_RXNE);
}

uint32_t readLength;
uint32_t availableBytes;

void PutCharOnRx()
{
	readLength = 0;
	availableBytes = MIN(huart4.RxXferCount, UART_BUFFER_SIZE - rx_buf_ins);
	overflow_count += MAX(0, rx_buf_ins + huart4.RxXferCount - UART_BUFFER_SIZE);
	HAL_UART_Receive( &huart4, (uint8_t*) &rx_buf[rx_buf_ins], availableBytes, 50); // 50ms is not tested
	rx_buf_ins += availableBytes - huart4.RxXferCount;
}
#endif

eConsoleError ConsoleIoInit(void)
{
	huart4.Instance = UART4;
	huart4.Init.BaudRate = 115200;
	huart4.Init.WordLength = UART_WORDLENGTH_8B;
	huart4.Init.StopBits = UART_STOPBITS_1;
	huart4.Init.Parity = UART_PARITY_NONE;
	huart4.Init.Mode = UART_MODE_TX_RX;
	huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart4.Init.OverSampling = UART_OVERSAMPLING_16;
	huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

	if (HAL_UART_Init(&huart4) != HAL_OK)
	{
		return CONSOLE_ERROR;
	}

	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);

	return CONSOLE_SUCCESS;
}

eConsoleError ConsoleIoReceive(uint8_t *buffer, const uint32_t bufferLength, uint32_t *readLength)
{
	HAL_UART_Receive( &huart4, buffer, bufferLength, 50); // 50ms is not tested
	*readLength = bufferLength - huart4.RxXferCount;
	return CONSOLE_SUCCESS;
}

eConsoleError ConsoleIoReceiveInt(uint8_t *buffer, const uint32_t bufferLength, uint32_t *readLength)
{
	uint8_t i = 0;
	while (( '\n' != rx_buf[i] ) && ( i < rx_buf_ins )) {
		i++;
	}

	// Can't validate vs rx_buf_ins because it could have incremented
	if ( '\n' != rx_buf[i] ){
		*readLength = 0;
	} else if (i > bufferLength){
		*readLength = i; // signal we got too much before error
		return CONSOLE_ERROR;
	} else {
		*readLength = i;
		memcpy(buffer, (uint8_t*) rx_buf, i); // copy out data
		memcpy((uint8_t*) rx_buf, (uint8_t*) &rx_buf[i], UART_BUFFER_SIZE-i); // shift remaining data down
		// if the interrupt runs here then we get one interrupt data len worth of substitution error
		rx_buf_ins -= i; // insert pointer continues where it left off
	}

	return CONSOLE_SUCCESS;
}

eConsoleError ConsoleIoSend(const uint8_t *buffer, const uint32_t bufferLength, uint32_t *sentLength)
{
	printf("%s", (char*) buffer);
	*sentLength = bufferLength;
	return CONSOLE_SUCCESS;
}

eConsoleError ConsoleIoSendString(const char *buffer)
{
	printf("%s", buffer);
	return CONSOLE_SUCCESS;
}


