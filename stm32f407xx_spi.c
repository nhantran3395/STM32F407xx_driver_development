/**
*@file stm32f407xx_spi.c
*@brief provide functions for SPI communications.
*
*This source file provide functions for communication between stm32f407xx MCU and I/O devices through SPI protocol.
*
*@author Tran Thanh Nhan
*@date 28/07/2019
*/

#include "stm32f407xx_spi.h"

void SPI_CLK_ctr(SPI_TypeDef *SPIxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE)
	{
		if(SPIxPtr == SPI1){
			RCC->APB2ENR |= 1<<12;
		}else if(SPIxPtr == SPI2){
			RCC->APB2ENR |= 1<<14;
		}else if(SPIxPtr == SPI3){
			RCC->APB2ENR |= 1<<15;
	}	
	}else{
		if(SPIxPtr == SPI1){
			RCC->APB2ENR &= ~(1<<12);
		}else if(SPIxPtr == SPI2){
			RCC->APB2ENR &= ~(1<<14);
		}else if(SPIxPtr == SPI3){
			RCC->APB2ENR &= ~(1<<15);
		}	
	}
}

void SPI_init(SPI_Handle_t *SPIxHandlePtr)
{
	SPI_CLK_ctr(SPIxHandlePtr->SPIxPtr,ENABLE);
	
	/*device mode: master or slave*/
	SPIxHandlePtr->SPIxPtr->CR1 &= ~(SPI_CR1_MSTR);
	SPIxHandlePtr->SPIxPtr->CR1 |= SPIxHandlePtr->SPIxConfigPtr->deviceMode << SPI_CR1_MSTR_Pos;
	
	if(SPIxHandlePtr->SPIxConfigPtr->busConfig == SPI_BUS_HALF_DUPLEX){
		/*configuration for half duplex*/
		SPIxHandlePtr->SPIxPtr->CR1 |= SPI_CR1_BIDIMODE;
	}else{
		/*general configuration for full duplex and simplex receive only*/
		SPIxHandlePtr->SPIxPtr->CR1 &= ~(SPI_CR1_BIDIMODE);
		
		if(SPIxHandlePtr->SPIxConfigPtr->busConfig == SPI_BUS_FULL_DUPLEX){
			/*full duplex*/
			SPIxHandlePtr->SPIxPtr->CR1 &= ~(SPI_CR1_RXONLY);
		}else{
			/*simplex receive only*/
			SPIxHandlePtr->SPIxPtr->CR1 |= SPI_CR1_RXONLY;
		}
	}
	
	/*data frame: 8 bits or 16 bits*/
	SPIxHandlePtr->SPIxPtr->CR1 &= ~(SPI_CR1_DFF);
	SPIxHandlePtr->SPIxPtr->CR1 |= SPIxHandlePtr->SPIxConfigPtr->dataFrame << SPI_CR1_DFF_Pos;

	/*clock phase: 1st edge or 2nd edge*/
	SPIxHandlePtr->SPIxPtr->CR1 &= ~(SPI_CR1_CPHA);
	SPIxHandlePtr->SPIxPtr->CR1 |= SPIxHandlePtr->SPIxConfigPtr->clkPhase << SPI_CR1_CPHA_Pos;	
	
	/*clock polarity: high idle or low idle*/
	SPIxHandlePtr->SPIxPtr->CR1 &= ~(SPI_CR1_CPOL);
	SPIxHandlePtr->SPIxPtr->CR1 |= SPIxHandlePtr->SPIxConfigPtr->clkPol << SPI_CR1_CPOL_Pos;
	
	/*clock speed: f_pCLK/2 -> f_pCLK/256*/
	SPIxHandlePtr->SPIxPtr->CR1 &= ~(SPI_CR1_BR);
	SPIxHandlePtr->SPIxPtr->CR1 |= SPIxHandlePtr->SPIxConfigPtr->clkPol << SPI_CR1_BR_Pos;	
}

void SPI_ctr(SPI_TypeDef *SPIxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE){
		SPIxPtr->CR1 |= SPI_CR1_SPE;
	}else{
		SPIxPtr->CR1 &= ~(SPI_CR1_SPE);
	}
}

void SPI_deinit(SPI_TypeDef *SPIxPtr)
{
	if(SPIxPtr == SPI1){
		RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
		RCC->AHB2RSTR &= ~(RCC_APB2RSTR_SPI1RST);
	}else if(SPIxPtr == SPI2){
		RCC->APB1RSTR |= RCC_APB1RSTR_SPI2RST;
		RCC->AHB1RSTR &= ~(RCC_APB1RSTR_SPI2RST);
	}else if(SPIxPtr == SPI3){
		RCC->APB1RSTR |= RCC_APB1RSTR_SPI3RST;
		RCC->AHB1RSTR &= ~(RCC_APB1RSTR_SPI3RST);
	}
}

uint8_t SPI_receive_data (SPI_TypeDef *SPIxPtr, uint8_t *rxBufferPtr,uint32_t Length)
{
			while(Length){
			/* wait until rx buffer is not empty*/
			while(SPIxPtr->SR & SPI_SR_RXNE);
			
			if(!(SPIxPtr->CR1 & SPI_CR1_DFF)){
				/*8 bit data frame*/
				*rxBufferPtr = SPIxPtr->DR;
				rxBufferPtr++;
				Length--;
			}else{
				/*16 bit data frame*/
				*((uint16_t*)rxBufferPtr) = SPIxPtr->DR ;
				rxBufferPtr+=2;
				Length-=2;
			}
		}
}

void SPI_send_data (SPI_TypeDef *SPIxPtr, uint8_t *txBufferPtr, uint32_t Length)
{
		while(Length){
			/* wait until tx buffer is empty*/
			while(!SPIxPtr->SR & SPI_SR_TXE);
			
			if(!(SPIxPtr->CR1 & SPI_CR1_DFF)){
				/*8 bit data frame*/
				SPIxPtr->DR |= *txBufferPtr;
				txBufferPtr++;
				Length--;
			}else{
				/*16 bit data frame*/
				SPIxPtr->DR |= *((uint16_t*)txBufferPtr);
				txBufferPtr+=2;
				Length-=2;
			}
		}
}

uint8_t SPI_receive_data_intrpt (SPI_Handle_t *SPIxHandlePtr, uint8_t *rxBufferPtr,uint32_t Length)
{
	if(SPIxHandlePtr->rxState != SPI_BUSY_IN_TX){
		SPIxHandlePtr->rxBufferPtr = rxBufferPtr;
		SPIxHandlePtr->rxLength = Length;
		SPIxHandlePtr->rxState = SPI_BUSY_IN_RX;
		SPIxHandlePtr->SPIxPtr->CR2 |= SPI_CR2_ERRIE;
		/*Enable RXNEIE bit to get interrupt when ever RXNE flag is set*/
		SPIxHandlePtr->SPIxPtr->CR2 |= SPI_CR2_RXNEIE;
	}
	return SPIxHandlePtr->rxState;
}

uint8_t SPI_send_data_intrpt (SPI_Handle_t *SPIxHandlePtr, uint8_t *txBufferPtr, uint32_t Length)
{
	if(SPIxHandlePtr->txState != SPI_BUSY_IN_RX){
		SPIxHandlePtr->txBufferPtr = txBufferPtr;
		SPIxHandlePtr->txLength = Length;
		SPIxHandlePtr->txState = SPI_BUSY_IN_TX;
		SPIxHandlePtr->SPIxPtr->CR2 |= SPI_CR2_ERRIE;
		/*Enable TXEIE bit to get interrupt when ever TXE flag is set*/
		SPIxHandlePtr->SPIxPtr->CR2 |= SPI_CR2_TXEIE;
	}
	return SPIxHandlePtr->txState;
}


void SPI_intrpt_ctrl (uint8_t IRQnumber, uint8_t enOrDis)
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

void SPI_intrpt_priority_config(uint8_t IRQnumber, uint8_t priority)
{
	uint8_t registerNo = IRQnumber/4;
	uint8_t section = IRQnumber%4;
	
	NVIC->IP[registerNo] &= ~(0xFF << (8*section));
	NVIC->IP[registerNo] |= (priority << (8*section + NUM_OF_IPR_BIT_IMPLEMENTED));
}

void SPI_intrpt_handler (SPI_Handle_t *SPIxHandlePtr)
{
	if(SPIxHandlePtr->SPIxPtr->CR1 & SPI_SR_RXNE){
	/*interrupt triggered due to data received in rx buffer*/
		if(SPIxHandlePtr->SPIxConfigPtr->dataFrame == SPI_DATA_8BITS){
			*(SPIxHandlePtr->rxBufferPtr) = SPIxHandlePtr->SPIxPtr->DR;
			SPIxHandlePtr->rxLength--;
			SPIxHandlePtr->rxBufferPtr++;
		}else{
			*((uint16_t*)SPIxHandlePtr->rxBufferPtr) = SPIxHandlePtr->SPIxPtr->DR;
			SPIxHandlePtr->rxLength-=2;
			SPIxHandlePtr->rxBufferPtr+=2;
		}
		if(!SPIxHandlePtr->rxLength){
			SPIxHandlePtr->rxState = SPI_READY;
			SPIxHandlePtr->SPIxPtr->CR2 &= ~(SPI_CR2_RXNEIE);
		}
	}else if(SPIxHandlePtr->SPIxPtr->CR1 & SPI_SR_TXE){
	/*interrupt triggered due to tx buffer is empty*/
		if(SPIxHandlePtr->SPIxConfigPtr->dataFrame == SPI_DATA_8BITS){
			SPIxHandlePtr->SPIxPtr->DR = *(SPIxHandlePtr->txBufferPtr);
			SPIxHandlePtr->txLength--;
			SPIxHandlePtr->txBufferPtr++;
		}else{
			SPIxHandlePtr->SPIxPtr->DR = *((uint16_t*)SPIxHandlePtr->rxBufferPtr);
			SPIxHandlePtr->txLength-=2;
			SPIxHandlePtr->txBufferPtr+=2;
		}
		if(!SPIxHandlePtr->txLength){
			SPIxHandlePtr->txState = SPI_READY;
			SPIxHandlePtr->SPIxPtr->CR2 &= ~(SPI_CR2_TXEIE);
		}
	}else if(SPIxHandlePtr->SPIxPtr->CR2 & SPI_SR_OVR){
	/*interrupt triggered due to overrun error*/
		uint8_t temp;
		temp = SPIxHandlePtr->SPIxPtr->DR;
		temp = SPIxHandlePtr->SPIxPtr->SR;
	}
}