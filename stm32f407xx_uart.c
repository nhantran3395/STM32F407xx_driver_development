/**
*@file stm32f407xx_uart.c
*@brief provide APIs for UART communications.
*
*This header file provide APIs for communication between stm32f407xx MCU and I/O devices through UART protocol.
*
*@author Tran Thanh Nhan
*@date 15/08/2019
*/

#include "stm32f407xx_uart.h"

/***********************************************************************
UART clock enable/disable
***********************************************************************/
void UART_CLK_ctr(USART_TypeDef *UARTxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE)
	{
		if(UARTxPtr == USART1){
			RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
		}else if(UARTxPtr == USART2){
			RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
		}else if(UARTxPtr == USART3){
			RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
		}else if(UARTxPtr == UART4){
			RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
		}else if(UARTxPtr == UART5){
			RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
		}else if(UARTxPtr == USART6){
			RCC->APB2ENR |= RCC_APB2ENR_USART6EN;			
	}	
	}else{
		if(UARTxPtr == USART1){
			RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
		}else if(UARTxPtr == USART2){
			RCC->APB1ENR &= ~RCC_APB1ENR_USART2EN;
		}else if(UARTxPtr == USART3){
			RCC->APB1ENR &= ~RCC_APB1ENR_USART3EN;
		}else if(UARTxPtr == UART4){
			RCC->APB1ENR &= ~RCC_APB1ENR_UART4EN;
		}else if(UARTxPtr == UART5){
			RCC->APB1ENR &= ~RCC_APB1ENR_UART5EN;
		}else if(UARTxPtr == USART6){
			RCC->APB2ENR &= ~RCC_APB2ENR_USART6EN;		
		}	
	}
}

/***********************************************************************
I2C peripheral enable/disable
***********************************************************************/
void UART_periph_ctr(USART_TypeDef *UARTxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE){
		UARTxPtr->CR1 |= USART_CR1_UE;
	}else{
		UARTxPtr->CR1 &= ~(USART_CR1_UE);
	}
}

/***********************************************************************
Set/clear UART interrupt enable bit 
***********************************************************************/
void UART_intrpt_ENbit_ctrl(USART_TypeDef *UARTxPtr,uint8_t setOrClear)
{
}

/***********************************************************************
Initialize UART communication
***********************************************************************/
void UART_init(UART_Handle_t *UARTxHandlePtr)
{
}

/***********************************************************************
Deinitialize I2C communication
***********************************************************************/
void UART_deinit(USART_TypeDef *I2CxPtr)
{
}

/***********************************************************************
UART send data 
***********************************************************************/
void UART_send(USART_TypeDef *UARTxHandlePtr, uint8_t *txDataPtr,uint32_t Length);
	
/***********************************************************************
UART receive data
***********************************************************************/
void UART_receive(UART_Handle_t *UARTxHandlePtr, uint8_t *rxDataPtr, uint32_t Length);

/***********************************************************************
UART send data(interrup base)
***********************************************************************/
uint8_t UART_send_intrpt (UART_Handle_t *UARTxHandlePtr, uint8_t *txData,uint32_t Length);

/***********************************************************************
UART receive data(interrup base)
***********************************************************************/
uint8_t UART_receive_intrpt (UART_Handle_t *UARTxHandlePtr, uint8_t *rxData, uint32_t Length);

/***********************************************************************
Enable or disable UART peripheral 's interrupt vector in NVIC
***********************************************************************/
void UART_intrpt_vector_ctrl (uint8_t IRQnumber, uint8_t enOrDis)
{
	if(enOrDis == ENABLE){
		if(IRQnumber <= 31){
			NVIC->ISER[0] |= (1<<IRQnumber);
		}
		else if(IRQnumber > 31 && IRQnumber <= 63){
			NVIC->ISER[1] |= (1<<(IRQnumber%32));
		}
		else if(IRQnumber > 63 && IRQnumber <= 95){
			NVIC->ISER[2] |= (1<<(IRQnumber%64));
		}
	}else{
		if(IRQnumber <= 31){
			NVIC->ICER[0] |= (1<<IRQnumber);
		}
		else if(IRQnumber > 31 && IRQnumber <= 63){
			NVIC->ICER[1] |= (1<<(IRQnumber%32));
		}
		else if(IRQnumber > 63 && IRQnumber <= 95){
			NVIC->ICER[2] |= (1<<(IRQnumber%64));
		}
	}
}

/***********************************************************************
Config priority for UART peripheral 's interrupt 
***********************************************************************/
void UART_intrpt_priority_config(uint8_t IRQnumber, uint8_t priority)
{
	uint8_t registerNo = IRQnumber/4;
	uint8_t section = IRQnumber%4;
	
	NVIC->IP[registerNo] &= ~(0xFF << (8*section));
	NVIC->IP[registerNo] |= (priority << (8*section + NUM_OF_IPR_BIT_IMPLEMENTED));
}

/***********************************************************************
UART interrupt handler 
***********************************************************************/
void UART_intrpt_handler (UART_Handle_t *UARTxHandlePtr)
{
}

/***********************************************************************
inform application of UART event or error
@Note: this is to be define in user application
***********************************************************************/
__attribute__((weak)) void UART_application_event_callback (UART_Handle_t *I2CxHandlePtr,uint8_t event) 
{
}
