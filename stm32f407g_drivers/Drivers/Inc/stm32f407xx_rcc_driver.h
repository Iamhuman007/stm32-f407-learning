/*
 * stm32f407xx_rcc_driver.h
 *
 *  Created on: 03-Mar-2026
 *      Author: Prathamesh
 */

#ifndef INC_STM32F407XX_RCC_DRIVER_H_
#define INC_STM32F407XX_RCC_DRIVER_H_

#include "stm32f407xx.h"

// this returns APB1 clock value
uint32_t RCC_GetPCLK1Value(void);

// this returns APB2 clock value
uint32_t RCC_GetPCLK2Value(void);

#endif /* INC_STM32F407XX_RCC_DRIVER_H_ */
