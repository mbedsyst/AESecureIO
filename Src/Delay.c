#include "Delay.h"

void delay(uint32_t ms)
{
	uint32_t i;

	SysTick->CTRL |= (1<<0) | (1<<2) ;
	SysTick->LOAD  = (SystemCoreClock/1000) - 1;
	for(i=0; i<ms; i++)
	{
    while(!(SysTick->CTRL & (1<<16)));
	}
	SysTick->CTRL &=~(1U<<0) ;
}
