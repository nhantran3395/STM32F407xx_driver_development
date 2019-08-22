/**
*@brief test SPI send data APIs by sending data from STM32F4xx to Arduino using SPI
*
*This program send data from STM32F4 to Arduino through SPI2 peripheral whenever user button on STM32F4 board is press. Purpose is to test SPI send data API.
*SPI configuration: 
*	Full duplex 
*	STM32F4xx is master, Arduino is slave
*	8-bits data frame
*	Hardware slave management
*	SCLK speed = 2MHz
*
*@author Tran Thanh Nhan
*@date 01/08/2019
*/

/*
*@PIN_MAPPING
*Pin mapping
*User_button PA0
*SPI2_MOSI PB15
*SPI2_MISO PB14
*SPI2_SCLK PB13
*SPI2_NSS PB12
*/

#include "stm32f4xx.h"                  // Device header
#include "../Peripheral_drivers/inc/stm32f407xx_gpio.h"
#include "../Peripheral_drivers/inc/stm32f407xx_spi.h"
#include "string.h"

void SPI2_GPIO_pin_init (void)
{	
	GPIO_Pin_config_t GPIO_SPI_pin_config = {.mode = GPIO_MODE_ALTFN,.speed = GPIO_OUTPUT_HIGH_SPEED,.outType = GPIO_OUTPUT_TYPE_PP,.puPdr = GPIO_PU,.altFunc = 5};
	GPIO_Handle_t GPIO_SPI_pin_handle;
	GPIO_SPI_pin_handle.GPIOxPtr = GPIOB;
	
	/*MOSI*/
	GPIO_SPI_pin_config.pinNumber = GPIO_PIN_NO_15;
	GPIO_SPI_pin_handle.GPIO_Pin_config = GPIO_SPI_pin_config;
	GPIO_init(&GPIO_SPI_pin_handle);
	
//	/*MISO*/
//	GPIO_SPI_pin_config.pinNumber = GPIO_PIN_NO_14;
//	GPIO_SPI_pin_handle.GPIO_Pin_config = GPIO_SPI_pin_config;
//	GPIO_init(&GPIO_SPI_pin_handle);

	/*SCLK*/
	GPIO_SPI_pin_config.pinNumber = GPIO_PIN_NO_13;
	GPIO_SPI_pin_handle.GPIO_Pin_config = GPIO_SPI_pin_config;
	GPIO_init(&GPIO_SPI_pin_handle);

	/*NSS*/
	GPIO_SPI_pin_config.pinNumber = GPIO_PIN_NO_12;
	GPIO_SPI_pin_handle.GPIO_Pin_config = GPIO_SPI_pin_config;
	GPIO_init(&GPIO_SPI_pin_handle);	
}

void SPI2_init (void)
{
	SPI_Config_t SPI2Config = {.deviceMode = SPI_MODE_MASTER,.busConfig = SPI_BUS_FULL_DUPLEX,.dataFrame = SPI_DATA_8BITS,.clkSpeed = SPI_CLK_SPEED_DIV8};
	SPI_Handle_t SPI2Handle={SPI2,&SPI2Config};
	SPI_init(&SPI2Handle);
}

void user_button_init (void)
{
	GPIO_Pin_config_t GPIO_PA0_config = {.pinNumber=GPIO_PIN_NO_0,.mode=GPIO_MODE_IN,.puPdr=GPIO_PDR};
	GPIO_Handle_t	GPIO_PA0_handle = {GPIOA,GPIO_PA0_config};
	GPIO_init(&GPIO_PA0_handle);
}	

uint8_t user_button_read (void)
{
	return GPIO_read_pin(GPIOA,GPIO_PIN_NO_0);
}

void green_led_init (void)
{
	GPIO_Pin_config_t GPIO_PD12_config={.pinNumber=GPIO_PIN_NO_12,.mode=GPIO_MODE_OUT,.speed = GPIO_OUTPUT_LOW_SPEED,.outType = GPIO_OUTPUT_TYPE_PP,.puPdr=GPIO_NO_PUPDR};
	GPIO_Handle_t GPIO_PD12_Handle = {GPIOD,GPIO_PD12_config};
	GPIO_init(&GPIO_PD12_Handle);
}

void green_led_on (void)
{
	GPIO_write_pin(GPIOD,GPIO_PIN_NO_12,SET);
}

void green_led_off (void)
{
	GPIO_write_pin(GPIOD,GPIO_PIN_NO_12,CLEAR);
}

int main (void)
{
	/*initilize green led on PD12*/
	green_led_init();
	
	/*Initialize user button on PA0*/
	user_button_init();
	
	/*Initilize SPI2 on PB15:PB12*/
	SPI2_GPIO_pin_init();
	SPI2_init();
	SPI_NSS_pin_ctr(SPI2,ENABLE);
	
	char *dataPtr = "HelloWorld!";
	uint8_t dataLength = strlen(dataPtr);
	
	while(1){
		if(user_button_read()){
			green_led_on();
			SPI_periph_ctr(SPI2,ENABLE);
			
			/*send to Arduino size of data to receive. then send the data*/
			SPI_send_data(SPI2,&dataLength,1);
			SPI_send_data(SPI2,(uint8_t*)dataPtr,dataLength);
			
			/*check whether SPI2 is still busy. if not, disable SPI2*/
			while(SPI_busy_check(SPI2));
			SPI_periph_ctr(SPI2,DISABLE);
		}else{
			green_led_off();
		}
	}
}