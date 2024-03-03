#ifndef __AESECUREIO_H__
#define __AESECUREIO_H__

#include <stm32f401xe.h>
#include <TinyAES.h>
#include	"UART.h"
#include	"DMA.h"
#include	"SDIO.h"
#include	"Delay.h"

void DisplayBanner(void);
void DisplayNote(void);
void DisplayInfo(void);

uint8_t ReceiveData();
void Fill_rxBuffer();
void TransferData(uint8_t *FROM, uint8_t *TO, uint32_t len);
void TransferDataTo_SDCard(uint8_t *sdBuffer, uint16_t len);
void TransferDataFrom_SDCard(uint8_t *sdBuffer, uint16_t len);
void Fill_txBuffer(uint8_t *plainBuffer, uint8_t *txBuffer, uint16_t len);
void TransmitData(uint8_t *data);

#endif
