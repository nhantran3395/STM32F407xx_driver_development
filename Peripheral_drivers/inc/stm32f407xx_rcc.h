#ifndef STM32F407XX_RCC_H
#define STM32F407XX_RCC_H

#include "stm32f407xx.h"                  // Device header
#include "stm32f407xx_common_macro.h"
#include <stdint.h>
#include <stdlib.h>

#define APB1 0
#define APB2 1

uint32_t RCC_get_PLL_output (void);
uint32_t RCC_get_PCLK_value(uint8_t APBx);

#endif