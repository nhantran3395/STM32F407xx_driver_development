#include "../inc/stm32f407xx_rcc.h"

/***********************************************************************
get PLL output value
***********************************************************************/
uint32_t RCC_get_PLL_output (void)
{
	return 0;
}

/***********************************************************************
calculate APB bus clock value  
***********************************************************************/
uint32_t RCC_get_PCLK_value(uint8_t APBx)
{
	uint32_t sysClk = 0, PCLK = 0;
	uint8_t sysClkStatus =	(RCC->CFGR >> RCC_CFGR_SWS_Pos) & 0x03;
	uint8_t AHBdiv = 0, APBdiv = 0;
	uint16_t AHBdivArray[] = {2,4,8,16,64,128,256,512};
	uint8_t APBdivArray[]	=	{2,4,8,16};
	uint8_t AHBdivStatus = (RCC->CFGR >> RCC_CFGR_HPRE_Pos)	& 0x0F;
	uint8_t APBdivStatus;
	if(APBx == APB1){
		APBdivStatus = (RCC->CFGR >> RCC_CFGR_PPRE1_Pos)	& 0x07;
	}else{
		APBdivStatus = (RCC->CFGR >> RCC_CFGR_PPRE2_Pos)	& 0x07;
	}
	
	
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
	if(APBdivStatus <= 3){
		APBdiv = 1;
	}else{
		APBdiv = APBdivArray[APBdivStatus-4];
	}
	
	/*derive PCLK1*/ 
	PCLK = (sysClk/AHBdiv)/APBdiv;
	return PCLK;
}
