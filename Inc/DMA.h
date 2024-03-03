#ifndef	__DMA_H_
#define	__DMA_H__

#include	<stm32f401xe.h>

void DMA_Init(void);
void DMA_ConfigureTransfer(void);
void DMA_StartTransfer(void);
void DMA1_Stream6_IRQHandler(void);

#endif
