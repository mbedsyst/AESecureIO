#ifndef __UART_H__
#define __UART_H__

#define SYS_FREQ		16000000
#define APB1_CLK		SYS_FREQ
#define UART_BAUDRATE	115200

void UART_Tx_init(void);
static uint16_t UART_Compute_BaudRate(uint32_t periph_clk, uint32_t baudrate);
static void UART_Set_BaudRate(uint32_t periph_clk, uint32_t baudrate);
static void UART_Write(int ch);

#endif
