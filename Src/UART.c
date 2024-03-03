#include <stdio.h>
#include <stdint.h>
#include "UART.h"
#include "stm32f4xx.h"

int __io_putchar(int ch)
{
	UART_Write(ch);
	return ch;
}

static uint16_t UART_Compute_BaudRate(uint32_t periph_clk, uint32_t baudrate)
{
	return ((periph_clk + (baudrate/2U))/baudrate);
}
static void UART_Set_BaudRate(uint32_t periph_clk, uint32_t baudrate)
{
	USART2->BRR = UART_Compute_BaudRate(periph_clk, baudrate);
}

void UART_Tx_init(void)
{
	RCC->AHB1ENR |= 1<<0;
	RCC->APB1ENR |= 1<<17;

	GPIOA->MODER &= ~((1U<<4) |(1<<6));
	GPIOA->MODER |=  (1<<5) | (1<<7);
	GPIOA->AFR[0] |= (0x07<<8) | (0x07<<12);

	UART_Set_BaudRate(APB1_CLK, UART_BAUDRATE);
	USART2->CR1 |= (1<<13) | (1<<3) | (1<<2);
}

static void UART_Write(int ch)
{
	while (!(USART2->SR & (1<<7)));
	USART2->DR =  (ch & 0XFF);
}

