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
	SPI_Config_t *SPIxConfigPtr;
}SPI_Handle_t;

/**
*@brief SPI clock enable/disable
*@param Pointer to base address of SPI registers
*@param Enable or disable action
*@return none
*/
void SPI_CLK_ctr(SPI_TypeDef *SPIxPtr);

/**
*@brief Initialize SPI bus
*@param Pointer to SPI_Handle struct
*@return none
*/
void SPI_init(SPI_Handle_t *SPIxHandlePtr);

/**
*@brief Deinitialize SPI bus
*@param Pointer to SPI_Handle struct
*@return none
*/
void SPI_deinit(SPI_TypeDef *SPIxPtr);

/**
*@brief Receive data from SPI bus
*@param Pointer to base address of SPI registers
*@param Pointer to memory region to store received data
*@param Length of data
*@return Pointer to memory region to store received data
*/
uint8_t SPI_read_data (SPI_TypeDef *SPIxPtr, uint8_t *rxBufferPtr,uint32_t Length);

/**
*@brief Write data to SPI bus
*@param Pointer to base address of SPI registers
*@param Pointer to data to write
*@param Length of data
*@return none
*/
void SPI_write_data (SPI_TypeDef *SPIxPtr, uint8_t *txBufferPtr, uint32_t Length);

/**
*@brief Enable or disable SPI 's interrupt 
*@param IRQ number
*@param Enable or disable action
*@return none
*/
void SPI_intrpt_ctrl (void);

/**
*@brief Config interrupt priority for SPI peripheral
*@param IRQ number
*@param Priority
*@return none
*/
void SPI_intrpt_priority_config(void);

/**
*@brief Handler for SPI peripheral
*@param Pointer to SPI_Handle struct
*@return none
*/
void SPI_intrpt_handler (SPI_Handle_t *SPIxHandlePtr);
#endif 