/**
*@file stm32f407xx_i2c.h
*@brief provide APIs for I2C communications.
*
*This header file provide APIs for communication between stm32f407xx MCU and I/O devices through I2C protocol.
*
*@author Tran Thanh Nhan
*@date 31/07/2019
*/

#ifndef STM32F407XX_I2C_H
#define STM32F407XX_I2C_H

#include "stm32f407xx.h"                  // Device header
#include "stm32f407xx_common_macro.h"
#include <stdint.h>

/*
*@I2C_SCLspeed
*I2C SCL frequency selection
*/
#define I2C_FSCL_SM 100000
#define I2C_FSCL_FM 400000

/*
*@I2C_ACKctr
*I2C ACK control
*/
#define I2C_ACKctr_DISABLE	0
#define I2C_ACKctr_ENABLE	1 

/*
*@I2C_FMdutyCycle
*I2C fast mode duty cycle
*/
#define I2C_FMduty_2	0
#define I2C_FMduty_16_9	1 

typedef struct{
	uint32_t SCLspeed;	/*refer to @I2C_SCLspeed for possible value*/
	uint8_t deviceAddress;	
	uint8_t ACKctr;	/*refer to @I2C_ACKctr for possible value*/	
	uint8_t FMdutyCycle;	/*refer to @I2C_FMdutyCycle for possible value*/	
}I2C_Config_t;

typedef struct{
	I2C_TypeDef *I2CxPtr;
	I2C_Config_t *I2CxConfigPtr;
}I2C_Handle_t;

/**
*@brief I2C clock enable/disable
*@param Pointer to base address of I2C registers
*@param Enable or disable action
*@return none
*/
void I2C_CLK_ctr(I2C_TypeDef *I2CxPtr, uint8_t enOrDis);

/**
*@brief Initialize I2C communication
*@param Pointer to I2C Handle struct
*@return none
*/
void I2C_init(I2C_Handle_t *I2CxHandlePtr);

/**
*@brief Deinitialize I2C communication
*@param Pointer to I2C_Handle struct
*@return none
*/
void I2C_deinit(I2C_TypeDef *I2CxPtr);

/**
*@brief I2C peripheral enable/disable
*
*Set or clear PE bit in CR1 register to enable/disable I2C peripheral
*Call this function after calling I2C_init to enable I2C communication. 
*
*@param Pointer to base address of I2C registers
*@param Enable or disable action
*@return none
*/
void I2C_periph_ctr(I2C_TypeDef *I2CxPtr, uint8_t enOrDis);

/**
*@brief Master receive data
*@param Pointer to I2C handle struct
*@param Pointer to memory region to store received data
*@param Length of data
*@return none
*/
void I2C_master_receive (I2C_Handle_t *I2CxHandlePtr, uint8_t *rxBufferPtr,uint32_t Length,uint8_t slaveAddr);

/**
*@brief Master send data 
*@param Pointer to I2C handle struct
*@param Pointer to data to send
*@param Length of data
*@param Slave address (7 bit)
*@return none
*/
void I2C_master_send(I2C_Handle_t *I2CxHandlePtr, uint8_t *txBufferPtr, uint32_t Length, uint8_t slaveAddr);

/**
*@brief Slave receive data
*@param Pointer to base address of I2C registers
*@param Pointer to memory region to store received data
*@param Length of data
*@return Pointer to memory region to store received data
*/
uint8_t I2C_slave_receive (I2C_TypeDef *I2CxPtr, uint8_t *rxBufferPtr,uint32_t Length);

/**
*@brief Slave send data 
*@param Pointer to base address of I2C registers
*@param Pointer to data to send
*@param Length of data
*@return none
*/
void I2C_slave_send(I2C_TypeDef *I2CxPtr, uint8_t *txBufferPtr, uint32_t Length);

/**
*@brief Enable or disable I2C 's interrupt 
*@param IRQ number
*@param Enable or disable action
*@return none
*/
void I2C_intrpt_ctrl (uint8_t IRQnumber, uint8_t enOrDis);

/**
*@brief Config interrupt priority for I2C peripheral
*@param IRQ number
*@param Priority
*@return none
*/
void I2C_intrpt_priority_config(uint8_t IRQnumber, uint8_t priority);

/**
*@brief I2C error interrupt handler
*@param Pointer to I2C_Handle struct
*@return none
*/
void I2C_err_intrpt_handler (I2C_Handle_t *I2CxHandlePtr);

/**
*@brief I2C event interrupt handler
*@param Pointer to I2C_Handle struct
*@return none
*/
void I2C_event_intrpt_handler (I2C_Handle_t *I2CxHandlePtr);
#endif 