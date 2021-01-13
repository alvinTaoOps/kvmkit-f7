// Console IO is a wrapper between the actual in and output and the console code
// In an embedded system, this might interface to a UART driver.

#include "consoleIo.h"
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
volatile uint8_t rx_buf_ins = 0;

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

eConsoleError ConsoleIoReceive(uint8_t *buffer, const uint32_t bufferLength, uint32_t *readLength)
{
	if (rx_buf_ins > 0)
	{
		uint8_t n_available = MIN(rx_buf_ins, bufferLength);
		*readLength = n_available;
		memcpy(buffer, (uint8_t*) rx_buf, n_available); // copy out data
		memcpy((uint8_t*) rx_buf, (uint8_t*) &rx_buf[n_available], UART_BUFFER_SIZE - n_available); // shift remaining data down
		// if the interrupt runs here then we lose that data
		rx_buf_ins -= n_available; // insert pointer continues where it left off
	} else
	{
		*readLength = 0;
	}

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
