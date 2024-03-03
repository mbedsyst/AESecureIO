#include "SDIO.h"

#include <stm32f4xx.h>
#include "UART.h"
#include "Delay.h"

/* Character array Buffer of size 1024 bytes */
static uint8_t		DataBuf[512*2];
/* Pointer to the Character array Buffer */
static uint8_t		*pDataBuf = DataBuf;
/* COunter to track the buffer */
static uint32_t		BufCount = 0;

#define DataTimeOut		(0xFFFFFF)

/* Flag to indicate ongoing transmission */
static volatile uint32_t SDIO_TxRx = 0;

/* @Note:	To display Error message and Block the code execution.
 * 			Flash the status LEDs as indicator	*/
static void SDIO_ERR(uint8_t *message)
{
	uint32_t i = 0;
	UART_Tx_String((char *)message);

	while(1)
	{
		/* Flash LED infinitely */
	}
}

/*	@Note:  To send Command to the SD Card. It will return
 * 			the SDIO_STA register. */
static uint32_t SDIO_CMD(uint32_t cmd, uint32_t rsp, uint32_t arg)
{
	/* Clear the command flags in SDIO_ICR register */
	SDIO->ICR |= (SDIO_STA_CCRCFAIL | SDIO_STA_CTIMEOUT | SDIO_STA_CMDREND | SDIO_STA_CMDSENT);

	/* Write the argument to SDIO_ARG register */
	SDIO->ARG |= arg;

	/* Write Command Index and Response Type to SDIO_CMD register */
	SDIO->CMD |=(uint32_t)((cmd & SDIO_CMD_CMDINDEX) | (rsp & SDIO_CMD_WAITRESP) | (0x400));

	/* Block execution till response is received by checking for expected flags */
	if(rsp == NO_RSP)
	{
		/* For NORESP, CMDSENT flag can be expected */
		while(!(SDIO->STA &  (SDIO_STA_CTIMEOUT | SDIO_STA_CMDSENT)));
	}
	else
	{
		/* For SHRESP, LNRESP and R3RESP, CMDREND and CCRCFAIL flags can be expected */
		while(!(SDIO->STA & (SDIO_STA_CTIMEOUT | SDIO_STA_CMDREND | SDIO_STA_CCRCFAIL)));
	}

	/* Check if received response is valid */
	if(SDIO->STA & SDIO_STA_CTIMEOUT)
	{
		/* Block execution by printing TIMEOUT ERROR */
		SDIO_ERR("Command Timeout ERROR\n");
	}
	else if((SDIO->STA & SDIO_STA_CCRCFAIL) && (rsp != R3_RSP ) )
	{
		/* Block execution by printing CRC FAIL ERROR */
		SDIO_ERR("CRC Fail ERROR\n");
	}

	/* Return the value of SDIO_STA register */
	return SDIO->STA;
}

static uint32_t SDIO_RSP(uint32_t *response, uint32_t type)
{
	if((type == RSP_R1) || (type == RSP_R1b))
	{
		*response = SDIO->RESP1;
		if(SDIO->RESP1 & (uint32_t)0xFDFFE008)
		{
			SDIO_ERR("SDIO: Response Error\n");
		}
		return (*response & 0x1F00)>>8;
	}
	else if(type == RSP_R2)
	{
		*response++ = SDIO->RESP1;
		*response++ = SDIO->RESP2;
		*response++ = SDIO->RESP3;
		*response = SDIO->RESP4;

		return 0;
	}
	else if(type == RSP_R3)
	{
		if(SDIO->RESPCMD != 0x3F)
		{
			SDIO_ERR("SDIO: Unexpected Command Error\n");
		}
		*response = SDIO->RESP1;
		return 0;
	}
	else if(type ==RSP_R6)
	{
		if(SDIO->RESPCMD != 0x03)
		{
			SDIO_ERR("SDIO: Unexpected Command Error\n");
		}
		*response = SDIO->RESP1;
		return (*response)>>16;
	}
	else
	{
		if(SDIO->RESPCMD != 0x08)
		{
			SDIO_ERR("SDIO: Unexpected Command Error\n");
		}
		*response = SDIO->RESP1;

		if((*response & 0xFF) != 0xAA)
		{
			SDIO_ERR("SDIO: Pattern did not Match\n");
		}
		return ((*response) & 0xFF00)>>8;
	}
}

/*	@Note:  Initialize the GPIO pins for SDIO Data, Clock and Command Pins.
 * 			Enable clock and Power up the SDIO peripheral */
void SDIO_Init(void)
{
	/* SDIO Peripheral Pin Mappings
	 * 	PC8		-	SDIO_D0
	 * 	PC9		-	SDIO_D1
	 * 	PC10	-	SDIO_D2
	 * 	PC11	-	SDIO_D3
	 * 	PC12	-	SDIO_CLK
	 * 	PD2		-	SDIO_CMD
	 */
	/* Enable clock to GPIOC and GPIOD */
	RCC->AHB1ENR	|= RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN;
	/* Set PC8-PC12 to Alternate Function Mode */
	GPIOC->MODER 	|= (0x02<<16) | (0x02<<18) | (0x02<<20) | (0x02<<22) | (0x02<<24);
	/* Set PD2 to Alternate Function Mode */
	GPIOD->MODER 	|= (0x02<<4);
	/* Pull-Up the data pins */
	GPIOC->PUPDR 	|= (0x01<<16) | (0x01<<18) | (0x01<<20) | (0x01<<22);

	/* Enable clock to SDIO */
	RCC->APB2ENR	|= RCC_APB2ENR_SDIOEN;
	/* Enable SDIO_CK */
	SDIO->CLKCR		|= SDIO_CLKCR_CLKEN;
	/* Set CLKDIV to 0x26 */
	SDIO->CLKCR		|= (0x26<<0);
	/* Power up SDIO peripheral */
	SDIO->POWER		|= 0x03;
}

/*	@Note:  Configure the SDIO peripheral to the requirement */
void SDIO_Configure(void)
{
	uint32_t response;
	uint32_t command;
	uint32_t TIMEOUT = 0xFFFF;

	SDIO_CMD(CMD0, NO_RSP, 0);

	SDIO_CMD(CMD8, SH_RSP, 0x000001AA);
	SDIO_RSP(&response, RSP_R7);

	while(1)
	{
		SDIO_CMD(CMD55, SH_RSP, 0);
		SDIO_RSP(&response, RSP_R1);

		SDIO_CMD(ACMD41, R3_RSP, (uint32_t)0x80100000 | (uint32_t)0x40000000);
		SDIO_RSP(&response, RSP_R3);

		if((response>>31) == 1)
		{
			SD_Type = (response & 0x40000000)>>30;
			break;
		}
		else
		{
			TIMEOUT--;
			if(!TIMEOUT)
			{
				SDIO_ERR("SDIO: ACMD41 Timeout\n");
			}
		}
	}

	SDIO_CMD(CMD2, LN_RSP, 0);

	SDIO_CMD(CMD3, SH_RSP, 0);
	SDIO_RSP(&response, RSP_R6);
	RCA = response>>16;

	SDIO_CMD(CMD9, LN_RSP, (RCA << 16));

	SDIO_CMD(CMD7, SH_RSP, (RCA << 16));
	SDIO_RSP(&response, RSP_R1);

	SDIO_CMD(ACMD6, SH_RSP, 0x02);
	SDIO_RSP(&response, RSP_R1);

	SDIO->CLKCR |= (0x01<<11);
	SDIO->CLKCR |= SDIO_CLKCR_CLKEN;
}

/*	@Note:  Start Transmission of Data */
static void SDIO_Tx_Start(uint8_t buf, uint32_t count, uint32_t dir)
{
	/* Pad count to multiples of 512 */
	count = 0x01FFFFFF & ((count >> 8) << 8);
	/* Disable DMA2 Stream3 */
	DMA2_Stream3->CR = 0;
	/* Clear Interrupt Flags */
	DMA2->LIFCR |= (1U<<27) | (1U<<26) | (1U<<25) | (1U<<24) | (1U<<22);
	/* Set Peripheral Address */
	DMA2_Stream3->PAR	= ((uint32_t) 0x40012C80);
	/* Set Memory Address */
	DMA2_Stream3->M0AR	= ((uint32_t) buf);
	/* Set Number of Data items to Transfer */
	DMA2_Stream3->NDTR	= 0;
	/* Select Channel 4 as SDIO */
	DMA2_Stream3->CR |= (0x04<<25);
	/* Memory Burst Configuration - Incremental Burst of 4 beats */
	DMA2_Stream3->CR |= (0x01<<23);
	/* Peripheral Burst Configuration - Incremental Burst of 4 beats */
	DMA2_Stream3->CR |= (0x01<<21);
	/* Disable Double Buffer Mode */
	DMA2_Stream3->CR |= (0x00<<18);
	/* Set Priority to VERY HIGH */
	DMA2_Stream3->CR |= (0x03<<16);
	/* Peripheral Increment Offset size is linked to PSIZE */
	DMA2_Stream3->CR |= (0x00<<15);
	/* Memory Data size set to WORD */
	DMA2_Stream3->CR |= (0x02<<13);
	/* Peripheral Data size set to WORD */
	DMA2_Stream3->CR |= (0x02<<11);
	/* Memory Increment Mode Enabled */
	DMA2_Stream3->CR |= (0x01<<10);
	/* Peripheral Increment Mode Disabled */
	DMA2_Stream3->CR |= (0x00<<9);
	/* Circular Mode Disabled */
	DMA2_Stream3->CR |= (0x00<<8);
	/* Set Peripheral as the Flow Controller */
	DMA2_Stream3->CR |= (0x01<<5);
	/* Disable FIFO Error Interrupt */
	DMA2_Stream3->FCR |= (0x00<<7);
	/* Disable Direct Mode */
	DMA2_Stream3->FCR |= (0x01<<2);
	/* Set Full FIFO as Threshold */
	DMA2_Stream3->FCR |= (0x03<<0);
	/* Set Transfer Direction */
	if(dir == UserMode2SD)
	{
		DMA2_Stream3->CR |= (0x01<<6);
	}
	else if(dir == SD2UserMode)
	{
		DMA2_Stream3->CR |= (0x00<<6);
	}
	/* Enable DMA Stream */
	DMA2_Stream3->CR |= (0x01<<0);

	/* Clear SDIO Status Flags */
	SDIO->ICR |= (1U<<1) | (1U<<2) | (1U<<4) | (1U<<5) | (1U<<8) | (1U<<9) | (1U<<10);
	/* Set the Data Timeout Period */
	SDIO->DTIMER = (uint32_t)DataTimeOut;
	/* Set Data Length value in bytes */
	SDIO->DLEN = count;
	/* Set Block size as 512 bytes */
	SDIO->DCTRL |= (9U<<4);
	/* Enable DMA in SDIO */
	SDIO->DCTRL |= (1U<<3);
	/* Select Block Transfer Mode */
	SDIO->DCTRL |= (0<<2);
	/* Set Data Transfer Direction */
	SDIO->DCTRL |= (dir<<1);
	/* Enable Data Transfer */
	SDIO->DCTRL |= (1U<<0);
	/* Update Transmission flag */
	SDIO_TxRx = 1;
}

/*	@Note:  Wait for the Data Transmission to end. */
void SDIO_Tx_Wait(void)
{
	/* Wait till DMA is disabled. */
	while(DMA2_Stream3->CR & (0x01<<0)) {};
	/* Check if a previous transmission occurred */
	if(SDIO_TxRx)
	{
		/* Wait for DMA Interrupt Flags */
		if(DMA2->LISR & ((1U<<27) | (1U<<25) | (1U<<24) | (1U<<22)))
		{
			/* If Transfer didn't complete, print Error */
			if(!(DMA2->LISR & (1U<<27)))
			{
				SDIO_ERR("SDIO: DMA Error\n\r");
			}
		}
	}
	/* Wait till SDIO is inactive */
	while(SDIO->STA & ((1U<<12) | (1U<<13))) {};
	/* Check if a previous transmission occurred */
	if(SDIO_TxRx)
	{
		/* Wait for SDIO Status Flags */
		while(!(SDIO->STA & ((1U<<1) | (1U<<3) | (1U<<9) | (1U<<10)))) {};
		/* Print Error if Data Block is not Send/Received */
		if(!(SDIO->STA & (1U<<10)))
		{
			SDIO_ERR("SDIO: Data Transmission Error\n\r");
		}
	}
	/* Update SDIO Transmission Flag */
	SDIO_TxRx = 0;
}

/*	@Note:  Write a single block of data to the SD Card */
void SDIO_SingleBlock_Write(uint8_t *buf, uint32_t block)
{
	uint32_t WriteAddress;
	uint32_t response;
	/* Check if SD Card is High Capacity or Standard Capacity */
	if(SD_Type == 1)
	{
		WriteAddress = block;
	}
	else if(SD_Type == 0)
	{
		WriteAddress = block*512;
	}
	/* Send Command to Write Single Block */
	SDIO_CMD(CMD24, SH_RSP, WriteAddress);
	SDIO_RSP(&response, RSP_R1);
	/* Start Data Transmission */
	SDIO_Tx_Start(buf, 512, UserMode2SD);
	/* Wait for Transmission to Finish */
	SDIO_Tx_Wait();
}

/*	@Note:  Read a Single Block of data from the SD Card */
void SDIO_SingleBlock_Read(uint8_t *buf, uint32_t block)
{
	uint32_t ReadAddress;
	uint32_t response;
	/* Check if SD Card is High Capacity or Standard Capacity */
	if(SD_Type == 1)
	{
		ReadAddress = block;
	}
	else if(SD_Type == 0)
	{
		ReadAddress = block*512;
	}
	/* Send Command to Read Single Block */
	SDIO_CMD(CMD17, SH_RSP, ReadAddress);
	SDIO_RSP(&response, RSP_R1);
	/* Start Data Transmission */
	SDIO_Tx_Start(buf, 512, SD2UserMode);
	/* Wait for Transmission to Finish */
	SDIO_Tx_Wait();
}

/*	@Note:  Start writing Multiple Block of Data to SD Card */
void SDIO_MultiBlockWrite_Start(uint32_t block)
{
	uint32_t WriteAddress;
	uint32_t response;
	/* Check if SD Card is High Capacity or Standard Capacity */
	if(SD_Type == 1)
	{
		WriteAddress = block;
	}
	else if(SD_Type == 0)
	{
		WriteAddress = block*512;
	}
	/* Send Command to Write Multiple Blocks of Data to Card */
	SDIO_CMD(CMD25, SH_RSP, WriteAddress);
	SDIO_RSP(&response, RSP_R1);
	/* Clear Flags in SDIO_ICR register */
	SDIO->ICR = (uint32_t)0xA003FF;
	return;
}

/*	@Note:   */
void SDIO_WriteData(uint8_t *buf, uint32_t count)
{
	while(count > 0)
	{
		/* Copy data to internal buffer */
		*pDataBuf = *buf;
		pDataBuf++;
		buf++;
		count--;
		BufCount++;
		/* Check if Buffer is Half Filled, then Write Buffer to SD Card */
		if(BufCount = 512)
		{
			/* Wait for any ongoing Transmission to finish */
			SDIO_Tx_Wait();
			/* Start Transmission of Buffer */
			SDIO_Tx_Start(pDataBuf - 512, 512, UserMode2SD);

			if(pDataBuf == (DataBuf + (512*2)))
			{
				pDataBuf = DataBuf;
			}
			/* Reset Buffer Counter to 0 */
			BufCount = 0;
		}
	}
}

/*	@Note: Stop writing Multiple Block of Data to SD Card  */
void SDIO_MultiBlockWrite_Stop(void)
{
	uint32_t response;
	/* Wait for Transmission to Finish */
	SDIO_Tx_Wait();
	/* Send Command to Stop Transmission */
	SDIO_CMD(CMD12, SH_RSP, 0);
	SDIO_RSP(&response, RSP_R1b);
	/* Remove the Untransferred Buffer */
	pDataBuf = DataBuf;
	/* Set Buffer Counter to 0 */
	BufCount = 0;

	while(((response & 0x100)>>8) == 0)
	{
		SDIO_CMD(CMD13, SH_RSP, RCA<<16);
		SDIO_RSP(&response, RSP_R1b);
	}
}
