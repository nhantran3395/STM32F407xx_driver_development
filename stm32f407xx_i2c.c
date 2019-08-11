/**
*@file stm32f407xx_i2c.c
*@brief provide APIs for I2C communications.
*
*This source file provide APIs for communication between stm32f407xx MCU and I/O devices through I2C protocol.
*
*@author Tran Thanh Nhan
*@date 31/07/2019
*/

#include "stm32f407xx_i2c.h"

static void I2C_start_stop_generation(I2C_TypeDef *I2CxPtr, uint8_t startOrStop)
{	
	if(startOrStop == START)
	{
		I2CxPtr->CR1 |= I2C_CR1_START;
	}else{
		I2CxPtr->CR1 |= I2C_CR1_STOP;
	}			
}

static void I2C_address_phase_execute(I2C_TypeDef *I2CxPtr, uint8_t slaveAddr, uint8_t readOrWrite)
{
	uint8_t sendData = slaveAddr << 1;
	
	if(readOrWrite == READ){
		sendData |= 1;
	}else{
		sendData &= ~1;
	}
	
	I2CxPtr->DR = sendData;
}

static void I2C_clear_ADDRflag(I2C_TypeDef *I2CxPtr)
{
	uint16_t tempData = I2CxPtr->SR1;
	tempData = I2CxPtr->SR2;
	(void) tempData;
}

static void I2C_ACK_ctr(I2C_TypeDef *I2CxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE){
		I2CxPtr->CR1 |= I2C_CR1_ACK;
	}else{
		I2CxPtr->CR1 &= ~(I2C_CR1_ACK);
	}

}

static void I2C_read_data(I2C_TypeDef *I2CxPtr,uint8_t *rxBufferPtr, uint32_t *LengthPtr)
{
	while(!(I2CxPtr->SR1 & I2C_SR1_RXNE));
	*rxBufferPtr = I2CxPtr->DR ;
	(*LengthPtr)--;
}

static void I2C_send_data(I2C_TypeDef *I2CxPtr,uint8_t *txBufferPtr, uint32_t *LengthPtr)
{
	while(!(I2CxPtr->SR1 & I2C_SR1_TXE));
	I2CxPtr->DR = *txBufferPtr;
	(*LengthPtr)--;
}

static uint32_t RCC_get_PLL_output (void)
{
	return 0;
}

static uint32_t RCC_get_PCLK1_value (void)
{
	uint32_t sysClk = 0, PCLK1 = 0;
	uint8_t sysClkStatus =	(RCC->CFGR >> RCC_CFGR_SWS_Pos) & 0x03;
	uint8_t AHBdiv = 0, APB1div = 0;
	uint16_t AHBdivArray[] = {2,4,8,16,64,128,256,512};
	uint8_t APB1divArray[]	=	{2,4,8,16};
	uint8_t AHBdivStatus = (RCC->CFGR >> RCC_CFGR_HPRE_Pos)	& 0x0F;
	uint8_t APB1divStatus = (RCC->CFGR >> RCC_CFGR_PPRE1_Pos)	& 0x07;
	
	/*get system clock value*/
	if(sysClkStatus == 0){
		sysClk = 16000000;
	}else if(sysClkStatus == 1){
		sysClk = 8000000;
	}else if(sysClkStatus == 2){
		sysClk = RCC_get_PLL_output();
	}	
	
	/*get AHB prescaler*/
	if(AHBdivStatus <= 7){
		AHBdiv = 1;
	}else{
		AHBdiv = AHBdivArray[AHBdivStatus-8];
	}
	
	/*get APB1 prescaler*/
	if(APB1divStatus <= 3){
		APB1div = 1;
	}else{
		APB1div = APB1divArray[APB1divStatus-4];
	}
	
	/*derive PCLK1*/ 
	PCLK1 = (sysClk/AHBdiv)/APB1div;
	return PCLK1;
}

void I2C_CLK_ctr(I2C_TypeDef *I2CxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE)
	{
		if(I2CxPtr == I2C1){
			RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
		}else if(I2CxPtr == I2C2){
			RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
		}else if(I2CxPtr == I2C3){
			RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
	}	
	}else{
		if(I2CxPtr == I2C1){
			RCC->APB1ENR &= ~(RCC_APB1ENR_I2C1EN);
		}else if(I2CxPtr == I2C2){
			RCC->APB1ENR &= ~(RCC_APB1ENR_I2C2EN);
		}else if(I2CxPtr == I2C3){
			RCC->APB1ENR &= ~(RCC_APB1ENR_I2C3EN);
		}	
	}
}

void I2C_periph_ctr(I2C_TypeDef *I2CxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE){
		I2CxPtr->CR1 |= I2C_CR1_PE;
	}else{
		I2CxPtr->CR1 &= ~(I2C_CR1_PE);
	}
}

void I2C_init(I2C_Handle_t *I2CxHandlePtr)
{
	/*enable clock for I2Cx peripheral*/
	I2C_CLK_ctr(I2CxHandlePtr->I2CxPtr,ENABLE);
	
	/*disable I2Cx peripheral for initilization*/
	I2C_periph_ctr(I2CxHandlePtr->I2CxPtr,DISABLE);
	
	/*set I2C peripheral input frequency*/
	uint32_t fPCLK1 = RCC_get_PCLK1_value();
	I2CxHandlePtr->I2CxPtr->CR2 &= ~(I2C_CR2_FREQ);
	I2CxHandlePtr->I2CxPtr->CR2 |=	(fPCLK1/1000000) << I2C_CR2_FREQ_Pos;
	
	/*config I2C SCL line speed (standard mode or fast mode). if fast mode, config duty cycle*/
	I2CxHandlePtr->I2CxPtr->CCR &= ~(I2C_CCR_FS);
	I2CxHandlePtr->I2CxPtr->CCR |= I2CxHandlePtr->I2CxConfigPtr->SCLspeed << I2C_CCR_FS_Pos;
	
	uint16_t CCRval = 0;
	
	if(I2CxHandlePtr->I2CxConfigPtr->SCLspeed <= I2C_FSCL_SM){
		/*slow mode*/
		CCRval = fPCLK1/(2*I2C_FSCL_SM);
	}else if(I2CxHandlePtr->I2CxConfigPtr->SCLspeed > I2C_FSCL_SM && I2CxHandlePtr->I2CxConfigPtr->SCLspeed <= I2C_FSCL_FM){
		/*fast mode*/		
		if(I2CxHandlePtr->I2CxConfigPtr->FMdutyCycle == I2C_FMduty_2){
			/*t_low:t_high 2:1*/
			I2CxHandlePtr->I2CxPtr->CR2 &= ~(I2C_CCR_DUTY);
			CCRval = fPCLK1/(3*I2C_FSCL_FM);
		}else{
			/*t_low:t_high 2:1 16:9*/
			I2CxHandlePtr->I2CxPtr->CR2 |= I2C_CCR_DUTY;
			CCRval = fPCLK1/(25*I2C_FSCL_FM);
		}
	}
	 
	I2CxHandlePtr->I2CxPtr->CCR &= ~(I2C_CCR_CCR);
	I2CxHandlePtr->I2CxPtr->CCR |= CCRval << I2C_CCR_CCR_Pos;
	
	/*program device own address*/ 
	I2CxHandlePtr->I2CxPtr->OAR1	&= ~(I2C_OAR1_ADD1_7);
	I2CxHandlePtr->I2CxPtr->OAR1	|= I2CxHandlePtr->I2CxConfigPtr->deviceAddress <<	I2C_OAR1_ADD1_Pos;
	
	/*config t_rise value*/
	uint8_t regVal = 0;
	uint16_t tRiseMax = 0;
	
	if(I2CxHandlePtr->I2CxConfigPtr->SCLspeed <= I2C_FSCL_SM){
		tRiseMax = 1000;
	}else if(I2CxHandlePtr->I2CxConfigPtr->SCLspeed > I2C_FSCL_SM && I2CxHandlePtr->I2CxConfigPtr->SCLspeed <= I2C_FSCL_FM){
		tRiseMax = 300;
	}
	
	regVal = ((double)tRiseMax/1000000000)*fPCLK1+1;
	I2CxHandlePtr->I2CxPtr->TRISE = (regVal&0x3F);
}

void I2C_deinit(I2C_TypeDef *I2CxPtr)
{
	if(I2CxPtr == I2C1){
		RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
		RCC->AHB1RSTR &= ~(RCC_APB1RSTR_I2C1RST);
	}else if(I2CxPtr == I2C2){
		RCC->APB1RSTR |= RCC_APB1RSTR_I2C2RST;
		RCC->AHB1RSTR &= ~(RCC_APB1RSTR_I2C2RST);
	}else if(I2CxPtr == I2C3){
		RCC->APB1RSTR |= RCC_APB1RSTR_I2C3RST;
		RCC->AHB1RSTR &= ~(RCC_APB1RSTR_I2C3RST);
	}
}

void I2C_intrpt_ctrl (uint8_t IRQnumber, uint8_t enOrDis)
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

void I2C_master_receive (I2C_Handle_t *I2CxHandlePtr, uint8_t *rxBufferPtr,uint32_t Length,uint8_t slaveAddr)
{
	/*enable acking for master*/
	I2C_ACK_ctr(I2CxHandlePtr->I2CxPtr, ENABLE);
	
	/*generate start condition and wait for SB flag to be set*/
	I2C_start_stop_generation(I2CxHandlePtr->I2CxPtr,START);
	while(!(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_SB));
	
	/*execute addressing phase and wait for ADDR flag to be set*/
	I2C_address_phase_execute(I2CxHandlePtr->I2CxPtr,slaveAddr,READ);
	while(!(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_ADDR));
	
	if(Length == 1){
		/*case slave only send 1 byte of data, disable ACK, clear ADDR flag,generate stop condition before reading data*/
		I2C_ACK_ctr(I2CxHandlePtr->I2CxPtr,DISABLE);
		I2C_clear_ADDRflag(I2CxHandlePtr->I2CxPtr);
		I2C_start_stop_generation(I2CxHandlePtr->I2CxPtr,STOP);
		I2C_read_data(I2CxHandlePtr->I2CxPtr, rxBufferPtr, &Length);
	}else{
		/*case slave send multiple data bytes, clear ADDR flag*/
		I2C_clear_ADDRflag(I2CxHandlePtr->I2CxPtr);
		
		while(Length){
			if(Length == 1){
				/*when there are 2 bytes left to receive, disable ACK and generate stop condition*/
				I2C_ACK_ctr(I2CxHandlePtr->I2CxPtr,DISABLE);
				I2C_start_stop_generation(I2CxHandlePtr->I2CxPtr,STOP);
			}
			I2C_read_data(I2CxHandlePtr->I2CxPtr, rxBufferPtr, &Length);
			rxBufferPtr++;
		}
	}
}

void I2C_master_send(I2C_Handle_t *I2CxHandlePtr, uint8_t *txBufferPtr, uint32_t Length, uint8_t slaveAddr)
{
	/*generate start condition and wait for SB flag to be set*/
	I2C_start_stop_generation(I2CxHandlePtr->I2CxPtr,START);
	while(!(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_SB));
	
	/*execute addressing phase*/
	I2C_address_phase_execute(I2CxHandlePtr->I2CxPtr,slaveAddr,WRITE);
	
	/*wait for ADDR flag to be set then clear*/
	while(!(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_ADDR));
	I2C_clear_ADDRflag(I2CxHandlePtr->I2CxPtr);
	
	/*send data until length = 0*/
	while(Length){
		I2C_send_data(I2CxHandlePtr->I2CxPtr,txBufferPtr,&Length);
		txBufferPtr++;
	}
	
	/*wait for TXE and BTF flag to be set before generating stop condition*/
	while(!(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_TXE));
	while(!(I2CxHandlePtr->I2CxPtr->SR1 & I2C_SR1_BTF));
	
	/*generate stop condition*/
	I2C_start_stop_generation(I2CxHandlePtr->I2CxPtr,STOP);
}

void I2C_intrpt_priority_config(uint8_t IRQnumber, uint8_t priority)
{
	uint8_t registerNo = IRQnumber/4;
	uint8_t section = IRQnumber%4;
	
	NVIC->IP[registerNo] &= ~(0xFF << (8*section));
	NVIC->IP[registerNo] |= (priority << (8*section + NUM_OF_IPR_BIT_IMPLEMENTED));
}