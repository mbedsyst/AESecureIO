#include <AESecureIO.h>

uint8_t rxBuffer[512];
uint8_t txBuffer[512];

uint8_t plainBuffer[16];
uint8_t cipherBuffer[16];
uint8_t sdBuffer[512];

void DisplayBanner(void)
{
	printf("####################################################################################\r\n");
	printf("#                ______   _____                                 _____   ____       #\r\n");
	printf("#         /\    |  ____| / ____|                               |_   _| / __ \      #\r\n");
	printf("#        /  \   | |__   | (___    ___   ___  _   _  _ __   ___   | |  | |  | |     #\r\n");
	printf("#       / /\ \  |  __|   \___ \  / _ \ / __|| | | || '__| / _ \  | |  | |  | |     #\r\n");
	printf("#      / ____ \ | |____  ____) ||  __/| (__ | |_| || |   |  __/ _| |_ | |__| |     #\r\n");
	printf("#     /_/    \_\|______||_____/  \___| \___| \__,_||_|    \___||_____| \____/      #\r\n");
	printf("#                                                                                  #\r\n");
	printf("####################################################################################\r\n");
}

void DisplayNote(void)
{
	printf("@Note: \r\n");
	printf("AESecureIO is an embedded system application developed on an STM32F401 Nucleo-64    \r\n");
	printf("Development Board. It encrypts user-input data using AES-128 and stores it securely \r\n");
	printf("on an SD card using SDIO. The application also supports decryption of data from the \r\n");
	printf("SD card. The key is hard-coded and predetermined.");
}
void DisplayInfo(void)
{
	printf("Input Data Size Limit : 512 bytes\r\n");
	printf("Read Data Limit       : 512 bytes\r\n");
	printf("Encryption Algorithm  : AES-128\r\n");
	printf("Mode of Operation     : CBC\r\n");
	printf("Author                : mbedsyst\r\n");
	printf("For more Information, visit mbedsyst.blogspot.com\r\n");
}

/*	* No arguments
	* Inside a for loop from 0 to 511, read the content from UART Data Register
	* Increment counter to track received number of bytes.
	* Return UART Data Register contents
	* Return type uint8_t*/
uint8_t ReceiveData()
{
	uint8_t rxByte;
	while(!(USART2->SR & USART_SR_RXNE));
	rxByte = USART2->DR;
	return rxByte;
}
/*	* No arguments
	* Call ReceiveData() function for 512 times using a for loop.
	* Inside the loop, store the return value of ReceiveData() to an 8 bit variable and write it to rxBuffer.
	* When iteration = 512, set control variable to zero and print "Buffer Full".
	* No return type.*/
void Fill_rxBuffer()
{
	uint16_t i;
	for(i = 0; i < 512; i++)
	{
		rxBuffer[i] = rxDataFeed;
	}
	if(i == 512)
	{
		/*Print "Rx Buffer FULL"*/
		printf("Fill_rxBuffer(): rxBuffer FULL!\n\r");
	}
}

/*	* Argument 1 : Pointer to FROM buffer
	* Argument 2 : Pointer to TO buffer
	* Argument 3 : Length of Block
	* Inside a for() loop from 1 to <Length of Block>, store the content of FROM buffer into the TO buffer.
	* Update FROM buffer and TO buffer positions globally.
	* No return type.*/
void TransferData(uint8_t *FROM, uint8_t *TO, uint32_t len)
{
	uint32_t i;
	for(i = 0; i < len; i++)
	{
		TO[i] = FROM[i];
	}
	if(i == len)
	{
		/*Print BUFFER FULL*/
		printf("TrasnferData(): Buffer FULL!\n\r");
	}
}
/*	* Argument 1 : Pointer to sdBuffer
	* Argument 3 : Length of sdBuffer
	* Inside a for loop from 0 to <Length of Block - 1>, call the SDIO_SingleBlockWrite()
	* No return type.*/
void TransferDataTo_SDCard(uint8_t *sdBuffer, uint16_t len)
{
	SDIO_SingleBlock_Write(sdBuffer, len);
	/* Print Transfer to SD Card Complete*/
	printf("TransferDataTo_SDCard(): Data Transfer complete!\n\r");
}
/*	* Argument 1 : Pointer to sdBuffer
	* Argument 2 : Length of sdBuffer
	* Inside a for loop from 1 to <Length of Block>, call the SDIO_SingleBlockRead()
	* No return type.*/
void TransferDataFrom_SDCard(uint8_t *sdBuffer, uint16_t len)
{
	SDIO_SingleBlock_Read(sdBuffer, len);
	/* Print Transfer from SD Card Complete*/
	printf("TransferDataFrom_SDCard(): Data Transfer complete!\n\r");
}
/*	* Argument 1 : Pointer to plainBuffer
	* Argument 2 : Pointer to txBuffer
	* Argument 2 : Length of sdBuffer
	* Inside a for loop from 0 to <Length of Block - 1>, store the content of plainBuffer to txBuffer*/
void Fill_txBuffer(uint8_t *plainBuffer, uint8_t *txBuffer, uint16_t len)
{
	uint16_t i;
	for(i = 0; i < len; i++)
	{
		txBuffer[i] = plainBuffer[i];
	}
	if(i == len)
	{
		/* Print Tx Buffer Full */
		printf("Fill_txBuffer(): txBuffer FULL!\n\r");
	}
}
/*	* Argument 1 : Pointer to txBuffer
	* Argument 2 : Length of block
	* Inside a for loop from 0 to 511, write the content of txBuffer to UART Data Register
	* No return type*/
void TransmitData(uint8_t *data)
{
	while(!(USART2->SR & USART_SR_TXE));
	USART2->DR = data;
}

void Encrypt_WriteToSDCard()
{
	/* Reading data from Serial Terminal to writing to SD Card */
}
void Decrypt_ReadFromSDCard()
{
	/* Reading data from SD Card to writing to Serial Terminal */
}

