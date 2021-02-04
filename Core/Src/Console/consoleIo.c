// Console IO is a wrapper between the actual in and output and the console code
// In an embedded system, this might interface to a UART driver.

#include <Console/consoleIo.h>
#include "main.h"
#include <stdio.h>
#include <string.h>

#define UART_BUFFER_SIZE 256 // Must be power of 2 if enabling interrupts

extern UART_HandleTypeDef huart4;


int _write(int file, char *outgoing, int len) {
   HAL_UART_Transmit(&huart4, (unsigned char *) outgoing, (uint16_t) len, (uint32_t) 100); //TODO: verify timeout duration
   return len;
}

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

#ifdef NONBLOCKING_MODE
	UART_Int_Init();
#endif

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

#ifdef NONBLOCKING_MODE

#define USART_Console UART4
#define USART_Console_IRQn UART4_IRQn
#define USART_Console_IRQHandler UART4_IRQHandler

#ifdef USART_CR1_TXEIE_TXFNFIE // FIFO Support (L4R9)
#define USART_CR1_TXEIE USART_CR1_TXEIE_TXFNFIE
#define USART_ISR_TXE USART_ISR_TXE_TXFNF
#define USART_CR1_RXNEIE USART_CR1_RXNEIE_RXFNEIE
#define USART_ISR_RXNE USART_ISR_RXNE_RXFNE
#endif

static volatile uint8_t rx_buf[UART_BUFFER_SIZE];
volatile uint16_t rx_buf_ins = 0;
uint16_t rx_buf_read = 0;

void UART_Int_Init()
{
	HAL_NVIC_SetPriority(USART_Console_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(USART_Console_IRQn);
	USART_Console->CR1 |= USART_CR1_RXNEIE; // Enable Interrupt
}

void UART_Int_Deinit()
{
	HAL_NVIC_DisableIRQ(USART_Console_IRQn);
}

void USART_Console_IRQHandler(void)
{
	if (USART_Console->ISR & USART_ISR_ORE) // Overrun Error
		USART_Console->ICR = USART_ICR_ORECF;
	if (USART_Console->ISR & USART_ISR_NE) // Noise Error
		USART_Console->ICR = USART_ICR_NCF;
	if (USART_Console->ISR & USART_ISR_FE) // Framing Error
		USART_Console->ICR = USART_ICR_FECF;
	if (USART_Console->ISR & USART_ISR_RXNE) // Received character
	{
		char rx = (char)(USART_Console->RDR & 0xFF);
		rx_buf[rx_buf_ins++] = rx; // Copy to buffer and increment
		rx_buf_ins &= (UART_BUFFER_SIZE-1);
	}
}

/*
 * Race condition: if the ISR triggers and wraps rx_buf_ins around rx_buf_read we have two behaviors
 * 			1) most of the time, this would drop UART_BUFFER_SIZE elements (already in the buffer)
 * 				and continue. This would only happen if the buffer is too small (peak data rate in)
 * 				or ConsoleIoReceive is called too infrequently (avg data rate out).
 * 			2) if the ISR runs between the n_available assignment and memcpy call, we would lose the
 * 				lower portion of the buffer (up to rx_buf_read).
 * Could have the ISR check for overrun (_ins ==  _read at end of routine) and set a flag for the
 * 	receiver to reallocate the buffer 2x as large? Hopefully, this is not needed complexity
 */
eConsoleError ConsoleIoReceive(uint8_t *buffer, const uint32_t bufferLength, uint32_t *readLength)
{
	uint8_t n_available;
	if ( rx_buf_ins > rx_buf_read )
	{
		n_available = rx_buf_ins - rx_buf_read; // Read all the data in the buffer
	} else if (rx_buf_ins < rx_buf_read)
	{
		n_available = UART_BUFFER_SIZE - rx_buf_read; // Read to the end of buffer, leave the rest for next call
	} else
	{
		*readLength = 0;
		return CONSOLE_SUCCESS;
	}

	*readLength = n_available;
	memcpy(buffer, (uint8_t*) &rx_buf[rx_buf_read], n_available); // copy out data
	rx_buf_read += n_available;
	rx_buf_read &= (UART_BUFFER_SIZE-1);

	return CONSOLE_SUCCESS;
}
#else
eConsoleError ConsoleIoReceive(uint8_t *buffer, const uint32_t bufferLength, uint32_t *readLength)
{
	HAL_UART_Receive( &huart4, buffer, bufferLength, 50); // 50ms is not tested
	*readLength = bufferLength - huart4.RxXferCount;
	return CONSOLE_SUCCESS;
}
#endif
