/**
*@file stm32f407xx_gpio.h
*@brief provide functions for interfacing with GPIO pin
*
*This header file provide functions for interfacing with GPIO pins on stm32f407xx MCUs.
*
*@author Tran Thanh Nhan
*@date 23/07/2019
*/

#include "stm32f407xx.h"                  // Device header
#include <stdint.h>

typedef struct
{
	uint8_t portName;
	uint8_t pinNumber;
	uint8_t mode;
	uint8_t speed;
	uint8_t outType;
	uint8_t puPdr;
}GPIO_Pin_config_t;

typedef struct 
{
	GPIO_TypeDef *GPIOxPtr; /*This hold the base address of the GPIO port where the pin belong*/
	GPIO_Pin_config_t GPIO_Pin_config; /*This hold configuration of GPIO pin*/
}GPIO_Handle_t;

/**
*@brief GPIO port clock setup 
*@param none
*@return none
*/
void GPIO_CLK_ctr (void);

/**
*@brief Initialize GPIO pin
*@param none
*@return none
*/
void GPIO_init (void);

/**
*@brief Deinitilize GPIO pin
*@param none
*@return none
*/
void GPIO_deinit (void);

/**
*@brief Read data from GPIO input pin
*@param none
*@return none
*/
void GPIO_read_pin (void);

/**
*@brief Read data from GPIO input port
*@param none
*@return none
*/
void GPIO_read_port (void);

/**
*@brief Write data to GPIO output pin
*@param none
*@return none
*/
void GPIO_write_pin (void);

/**
*@brief Write data to GPIO output port
*@param none
*@return none
*/
void GPIO_write_port (void);

/**
*@brief Toggle GPIO output pin
*@param none
*@return none
*/
void GPIO_toggle_pin (void);

/**
*@brief Config IRQ 
*@param none
*@return none
*/
void GPIO_IRQ_config (void);

/**
*@brief Handler for GPIO pin 's interrupt
*@param none
*@return none
*/
void GPIO_IRQ_handler (void);