/**
*@file stm32f407xx_uart.h
*@brief provide APIs for UART communications.
*
*This header file provide APIs for communication between stm32f407xx MCU and I/O devices through UART protocol.
*
*@author Tran Thanh Nhan
*@date 15/08/2019
*/

#ifndef STM32F407XX_UART_H
#define STM32F407XX_UART_H

#include "stm32f407xx.h"                  // Device header
#include "stm32f407xx_common_macro.h"
#include "stm32f407xx_rcc.h"
#include <stdint.h>
#include <stdlib.h>

/***********************************************************************
Macro definition
***********************************************************************/

/*
*@UART_BAUDRATE
*UART baudrate selection
*/
#define UART_BDR_1200 1200
#define UART_BDR_2400	2400
#define UART_BDR_4800	4800
#define UART_BDR_9600	9600
#define UART_BDR_14400	14400
#define UART_BDR_19200	19200
#define UART_BDR_38400	38400
#define UART_BDR_57600	57600
#define UART_BDR_115200	115200
#define UART_BDR_230400	230400
#define UART_BDR_460800	460800
#define UART_BDR_921600	921600

/*
*@UART_STOPBITS
*UART number of stop bit selection
*/
#define UART_STB_1 0
#define UART_STB_0_5 1
#define UART_STB_2 2
#define UART_STB_1_5 3

/*
*@UART_WORDLENGTH
*UART number of stop bit selection
*/
#define UART_WRDLEN_8_DT_BITS 0
#define UART_WRDLEN_9_DT_BITS 1

/*
*@UART_MODE
*UART device mode selection
*/
#define UART_TX 0
#define UART_RX 1
#define UART_TX_RX 2

/*
*@UART_PARITYCONTROL
*UART parity control
*/
#define UART_NO_PARCTRL 0
#define UART_ODD_PARCTRL 1
#define UART_EVEN_PARCTRL 2

/*
*@UART_FLOWCONTROL
*UART flow control
*/
#define UART_NO_FLOWCTRL 0
#define UART_RTS_FLOWCTRL 1
#define UART_CTS_FLOWCTRL 2
#define UART_RTS_CTS_FLOWCTRL 3

/*
*@UART_STATE
*State of UART transmitter/receiver
*/
#define UART_STATE_READY 0
#define UART_STATE_TX_BUSY 1
#define UART_STATE_RX_BUSY 2

/*
*@UART_EVENT
*Event in UART transmission/reception
*/
#define UART_EV_TX_COMPLETE
#define UART_EV_RX_COMPLETE

/***********************************************************************
UART structure definition
***********************************************************************/

typedef struct{
	uint32_t baudRate;	/*refer to @UART_BAUDRATE for possible value*/
	uint8_t stopBit;	/*refer to @UART_STOPBITS for possible value*/
	uint8_t wordLength;	/*refer to @@UART_WORDLENGTH for possible value*/	
	uint8_t mode;	/*refer to @UART_MODE for possible value*/
	uint8_t parityCtrl;	/*refer to @PARITYCONTROL for possible value*/
	uint8_t flowCtrl;	/*refer to @UART_FLOWCONTROL for possible value*/
}UART_Config_t;

typedef struct{
	USART_TypeDef *UARTxPtr;
	UART_Config_t *UARTxConfigPtr;
	uint8_t txState; /*state of transmitter, possible value: READY or TX_BUSY*/
	uint8_t rxState; /*state of receiver, possible value: READY or RX_BUSY*/
	uint8_t *txBufferPtr; /*pointer to sending data buffer*/
	uint8_t *rxBufferPtr; /*pointer to buffer to store received data*/
	uint32_t txLength; /*length of data to send*/
	uint32_t rxLength; /*length of data to receive*/
}UART_Handle_t;

/***********************************************************************
UART driver APIs prototype
***********************************************************************/

/**
*@brief UART peripheral clock enable/disable
*@param Pointer to base address of UART peripheral 's registers
*@param Enable or disable action
*@return none
*/
void UART_CLK_ctr(USART_TypeDef *UARTxPtr, uint8_t enOrDis);

/**
*@brief UART peripheral enable/disable
*
*Set or clear UE bit in CR1 register to enable/disable UART peripheral
*Call this function after UART_init to enable UART communication. 
*
*@param Pointer to base address of UART peripheral 's registers
*@param Enable or disable action
*@return none
*/
void UART_periph_ctr(USART_TypeDef *UARTxPtr, uint8_t enOrDis);

/**
*@brief Set/clear UART interrupt enable bit 
*
*Set or clear TXEIE,TCIE,RXNEIE,PEIE in CR1
*
*@param Pointer to base address of UART peripheral 's registers
*@param Set or clear action
*@return none
*/
void UART_intrpt_ENbit_ctrl(USART_TypeDef *I2CxPtr,uint8_t setOrClear);

/**
*@brief Initialize UART communication
*@param Pointer to UART handle struct
*@return none
*/
void UART_init(UART_Handle_t *UARTxHandlePtr);

/**
*@brief Deinitialize UART communication
*@param Pointer to UART Handle struct
*@return none
*/
void UART_deinit(USART_TypeDef *I2CxPtr);

/**
*@brief UART send data
*@param Pointer to UART handle struct
*@param Pointer to data to send
*@param Length of data (in bytes)
*@return none
*/
void UART_send(UART_Handle_t *UARTxHandlePtr, uint8_t *txBufferPtr,uint32_t Length);

/**
*@brief UART receive data
*@param Pointer to UART handle struct
*@param Pointer to memory region to store received data
*@param Length of data (in bytes)
*@return none
*/
void UART_receive(UART_Handle_t *UARTxHandlePtr, uint8_t *rxBufferPtr, uint32_t Length);

/**
*@brief UART send data(interrup base)
*@param Pointer to UART handle struct
*@param Pointer to data to send
*@param Length of data (in bytes)
*@return Status of UART peripheral (ready,busy_in_tx,busy_in_rx)
*/
uint8_t UART_send_intrpt (UART_Handle_t *UARTxHandlePtr, uint8_t *txData,uint32_t Length);

/**
*@brief UART receive data(interrupt base)
*@param Pointer to UART handle struct
*@param Pointer to memory region to store received data
*@param Length of data (in bytes)
*@return Status of UART peripheral (ready,busy_in_tx,busy_in_rx)
*/
uint8_t UART_receive_intrpt (UART_Handle_t *UARTxHandlePtr, uint8_t *rxData, uint32_t Length);

/**
*@brief Enable or disable UART peripheral 's interrupt vector in NVIC 
*@param IRQ number
*@param Enable or disable action
*@return none
*/
void UART_intrpt_vector_ctrl (uint8_t IRQnumber, uint8_t enOrDis);

/**
*@brief Config priority for UART peripheral 's interrupt 
*@param IRQ number
*@param Priority
*@return none
*/
void UART_intrpt_priority_config(uint8_t IRQnumber, uint8_t priority);

/**
*@brief UART interrupt handler
*@param Pointer to UART_Handle struct
*@return none
*/
void UART_intrpt_handler (UART_Handle_t *UARTxHandlePtr);

/**
*@brief close UART transmission
*@param Pointer to UART_Handle struct
*@return none
*/
void UART_close_send_data(UART_Handle_t *UARTxHandlePtr);

/**
*@brief close UART reception
*@param Pointer to UART_Handle struct
*@return none
*/
void UART_close_receive_data(UART_Handle_t *UARTxHandlePtr);

/**
*@brief inform application of UART event or error
*@param Pointer to UART_Handle struct
*@param Event/error macro
*@return none
*/
void UART_application_event_callback (UART_Handle_t *I2CxHandlePtr,uint8_t event);
#endif 