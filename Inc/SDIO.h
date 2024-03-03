#ifndef __SDIO_H__
#define	__SDIO_H__

#include	<stm32f401xe.h>

/* SDIO Commands */
#define CMD0		((uint8_t)0)
#define CMD8		((uint8_t)8)
#define CMD55		((uint8_t)55)
#define ACMD41		((uint8_t)41)
#define CMD2		((uint8_t)2)
#define CMD3		((uint8_t)3)
#define CMD9		((uint8_t)9)
#define CMD7		((uint8_t)7)
#define ACMD6		((uint8_t)6)
#define CMD24		((uint8_t)24)
#define CMD25		((uint8_t)25)
#define CMD12		((uint8_t)12)
#define CMD13		((uint8_t)13)
#define CMD17		((uint8_t)17)
#define CMD18		((uint8_t)18)

/* SDIO Response Types */
#define NO_RSP		(0x00)
#define SH_RSP		(0x40)
#define LN_RSP		(0xC0)
#define	R3_RSP		(0xF40)

/* SDIO Response Formats */
#define RSP_R1		(0x01)
#define RSP_R1b		(0x02)
#define RSP_R2		(0x03)
#define RSP_R3		(0x04)
#define RSP_R6		(0x05)
#define RSP_R7		(0x06)

/* SDIO Data Transfer Direction */
#define UserMode2SD	(0x00)
#define SD2UserMode	(0x02)

/* Defines the SD Card Type : High Capacity or Standard Capacity*/
static uint32_t 	SD_Type;
/* Store Relative Card Address */
static uint32_t 	RCA;

/* SDIO Pin Configuration */
void SDIO_Init(void);
/* SDIO Peripheral Configuration */
void SDIO_Configure(void);
/* Reading and Writing Single Block Data via SDIO */
void SDIO_SingleBlock_Write(uint8_t *buf, uint32_t block);
void SDIO_SingleBlock_Read(uint8_t *buf, uint32_t block);
/* Start Transmission and Wait for Transmission to End */
static void SDIO_Tx_Start(uint8_t buf, uint32_t count, uint32_t dir);
void SDIO_Tx_Wait(void);
/* SDIO Multiple Block Write */
void SDIO_MultiBlockWrite_Start(uint32_t block);
void SDIO_WriteData(uint8_t *buf, uint32_t count);
void SDIO_MultiBlockWrite_Stop(void);
/* SDIO Command, Response and Error */
static uint32_t SDIO_CMD(uint32_t cmd, uint32_t rsp, uint32_t arg);
static uint32_t SDIO_RSP(uint32_t *response, uint32_t type);
static void SDIO_ERR(uint8_t *message);

#endif
