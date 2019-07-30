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
#include "stm32f407xx_common_macro.h"
#include <stdint.h>

/*
*@SPI_DEVICE_MODE
*SPI device mode configuration
*/
#define SPI_MODE_MASTER 0
#define SPI_MODE_SLAVE 1

/*
*@SPI_BUS_CONFIG
*SPI bus mode configuration
*/
#define SPI_BUS_FULL_DUPLEX 0
#define SPI_BUS_HALF_DUPLEX 1
#define SPI_BUS_SIMPLEX_RXONLY 2

/*
*@SPI_DATA_FRAME
*SPI data frame configuration
*/
#define SPI_DATA_8BITS 0
#define SPI_DATA_16BITS 1

/*
*@SPI_CLK_PHASE
*SPI clock phase configuration
*/
#define SPI_CLK_PHASE_1ST_E 0
#define SPI_CLK_PHASE_2ND_E 1

/*
*@SPI_CLK_POL
*SPI clock polarity configuration
*/
#define SPI_CLK_POL_HIDLDE 0 
#define SPI_CLK_POL_LIDLE 1

/*
*@SPI_SSM_CTR
*SPI slave software management enable/disable
*/
#define SPI_SSM_DIS 0
#define SPI_SSM_EN 1
/*
*@SPI_CLK_SPEED
*SPI clock speed configuration
*/
#define SPI_CLK_SPEED_DIV2 0
#define SPI_CLK_SPEED_DIV4 1
#define SPI_CLK_SPEED_DIV8 2
#define SPI_CLK_SPEED_DIV16 3
#define SPI_CLK_SPEED_DIV32 4
#define SPI_CLK_SPEED_DIV64 5
#define SPI_CLK_SPEED_DIV128 6
#define SPI_CLK_SPEED_DIV256 7

/*
*@SPI_COMMUNICATIONS_STATE
*Possible SPI communication state
*/
#define	SPI_READY 0
#define SPI_BUSY_IN_RX 1
#define SPI_BUSY_IN_TX 2

typedef struct{
	uint8_t deviceMode;	/*Refer to @SPI_DEVICE_MODE for possible value*/
	uint8_t busConfig;	/*Refer to @SPI_BUS_CONFIG for possible value*/
	uint8_t dataFrame;	/*Refer to @SPI_DATA_FRAME for possible value*/
	uint8_t clkPhase;	/*Refer to @SPI_CLK_PHASE for possible value*/
	uint8_t clkPol;	/*Refer to @SPI_CLK_POL for possible value*/
	uint8_t swSlaveManage;	/*Refer to @SPI_SSM_CTR for possible value*/
	uint8_t clkSpeed;	/*Refer to @SPI_CLK_SPEED for possible value*/
}SPI_Config_t;

typedef struct{
	SPI_TypeDef *SPIxPtr;
	SPI_Config_t *SPIxConfigPtr;
	uint8_t *txBufferPtr;	/*To store application TxBuffer address*/
	uint8_t *rxBufferPtr;	/*To store application RxBuffer address*/
	uint32_t txLength;	/*To store Tx Length*/
	uint32_t rxLength;	/*To store Rx Length*/
	uint8_t txState;	/*To store Tx State: BUSY_IN_TX or READY*/
	uint8_t rxState;	/*To store Rx State: BUSY_IN_RX or READY*/
}SPI_Handle_t;

/**
*@brief SPI clock enable/disable
*@param Pointer to base address of SPI registers
*@param Enable or disable action
*@return none
*/
void SPI_CLK_ctr(SPI_TypeDef *SPIxPtr, uint8_t enOrDis);

/**
*@brief Initialize SPI bus
*@param Pointer to SPI_Handle struct
*@return none
*/
void SPI_init(SPI_Handle_t *SPIxHandlePtr);

/**
*@brief SPI enable/disable
*@param Pointer to base address of SPI registers
*@param Enable or disable action
*@return none
*/
void SPI_ctr(SPI_TypeDef *SPIxPtr, uint8_t enOrDis);

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
uint8_t SPI_receive_data (SPI_TypeDef *SPIxPtr, uint8_t *rxBufferPtr,uint32_t Length);

/**
*@brief Send data to SPI bus 
*@param Pointer to base address of SPI registers
*@param Pointer to data to send
*@param Length of data
*@return none
*/
void SPI_send_data(SPI_TypeDef *SPIxPtr, uint8_t *txBufferPtr, uint32_t Length);

/**
*@brief Receive data from SPI bus (interrup base)
*@param Pointer to SPI handle struct
*@param Pointer to memory region to store received data
*@param Length of data
*@return Status of Rx
*/
uint8_t SPI_receive_data_intrpt (SPI_Handle_t *SPIxHandlePtr, uint8_t *rxBufferPtr,uint32_t Length);

/**
*@brief Send data to SPI bus (interrupt base)
*@param Pointer to SPI handle struct
*@param Pointer to data to send
*@param Length of data
*@return Status of Tx
*/
uint8_t SPI_send_data_intrpt (SPI_Handle_t *SPIxHandlePtr, uint8_t *txBufferPtr, uint32_t Length);

/**
*@brief Enable or disable SPI 's interrupt 
*@param IRQ number
*@param Enable or disable action
*@return none
*/
void SPI_intrpt_ctrl (uint8_t IRQnumber, uint8_t enOrDis);

/**
*@brief Config interrupt priority for SPI peripheral
*@param IRQ number
*@param Priority
*@return none
*/
void SPI_intrpt_priority_config(uint8_t IRQnumber, uint8_t priority);

/**
*@brief Handler for SPI peripheral
*@param Pointer to SPI_Handle struct
*@return none
*/
void SPI_intrpt_handler (SPI_Handle_t *SPIxHandlePtr);
#endif 