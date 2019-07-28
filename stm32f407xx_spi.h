/**
*@file stm32f407xx_spi.h
*@brief provide functions for SPI communications.
*
*This header file provide functions for communication between stm32f407xx MCU and I/O devices through SPI protocol.
*
*@author Tran Thanh Nhan
*@date 28/07/2019
*/

#ifndef STM32F407XX_SPI_H
#define STM32F407XX_SPI_H

#include "stm32f407xx.h"                  // Device header
#include <stdint.h>

typedef struct{
	uint8_t deviceMode;
	uint8_t busConfig;
	uint8_t dataFrame;
	uint8_t clkPhase;
	uint8_t clkPol;
	uint8_t swSlaveManage;
	uint8_t clkSpeed;
}SPI_Config_t;

typedef struct{
	SPI_TypeDef *SPIxPtr;
	SPI_Config_t SPIxConfigPtr;
}SPI_Handle_t;

void SPI_init(void);
uint8_t SPI_read_data (void);
void SPI_write_data (void);
void SPI_intrpt_ctrl (void);
void SPI_intrpt_handler (void);
#endif 