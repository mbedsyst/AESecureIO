/*
	Title			:	SDIO_AES_Device
	Author			:	mbedsyst / Zalman Ul Fariz
	Description		:	Storing data received from the User via Serial terminal
						to an SD Card via DMA from a buffer after encrypting using
						AES and an already decided encryption key.
*/

#include <AESecureIO.h>
#include	"Delay.h"
#include	"Banner.h"
#include	<stm32f401xe.h>
#include	<stdio.h>
#include	<string.h>

int main(void)
{
	DisplayBanner();
	DisplayNote();
	DisplayInfo();

	while(1)
	{

	}
}
