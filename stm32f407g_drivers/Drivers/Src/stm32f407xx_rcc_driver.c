/*
 * stm32f407xx_rcc_driver.c
 *
 *  Created on: 03-Mar-2026
 *      Author: Prathamesh
 */
#include "stm32f407xx_rcc_driver.h"

uint16_t AHB_PreScaler[8] = {2,4,8,16,64,128,256,512};
uint16_t APB1_PreScaler[8] = {2,4,8,16};


uint32_t RCC_GetPLLOutputClock(void)
{
	uint32_t x=0;
	return x;
}


uint32_t RCC_GetPCLK1Value(void)
{
	// calculates the clock value of APB bus

	uint32_t pclk1, SystemClk;
	uint16_t ahbp, apb1p;
	uint8_t clksrc, temp;
	clksrc = ((RCC->CFGR >> 2) & 0x3); // reads the 3rd and 4th bits which tells what is the clksrc(HSE, HSI, PLL)
	if (clksrc == 0)
	{
		SystemClk = 16000000;
	}else if (clksrc == 1)
	{
		SystemClk = 8000000;// for this discovery board
	}else if (clksrc == 2)
	{
		SystemClk = RCC_GetPLLOutputClock();
	}

	// ahbp
	temp = ((RCC->CFGR >> 4) & 0xF);
	if(temp < 8)
	{
		ahbp = 1;
	}else
	{
		ahbp = AHB_PreScaler[temp-8];
	}

	// apb1
	temp = ((RCC->CFGR >> 10) & 0x7); // bit 10 11 12
	if(temp < 4)
	{
		apb1p = 1;
	}else
	{
		apb1p = APB1_PreScaler[temp-4];
	}

	pclk1 = (SystemClk / ahbp) / apb1p;
	return pclk1;
}


uint32_t RCC_GetPCLK2Value(void)
{
	// calculates the clock value of APB bus

	uint32_t pclk1, SystemClk;
	uint16_t ahbp, apb2p;
	uint8_t clksrc, temp;
	clksrc = ((RCC->CFGR >> 2) & 0x3); // reads the 3rd and 4th bits which tells what is the clksrc(HSE, HSI, PLL)
	if (clksrc == 0)
	{
		SystemClk = 16000000;
	}else if (clksrc == 1)
	{
		SystemClk = 8000000;// for this discovery board
	}else if (clksrc == 2)
	{
		SystemClk = RCC_GetPLLOutputClock();
	}
// what are ahbp and apb2: these are two prescalers, ahbp is common to both apb1 and apb2.
	// ahbp
	temp = ((RCC->CFGR >> 4) & 0xF);
	if(temp < 8)
	{
		ahbp = 1;
	}else
	{
		ahbp = AHB_PreScaler[temp-8];
	}

	// apb2
	temp = ((RCC->CFGR >> 13) & 0x7); // bit 13 14 15
	if(temp < 4)
	{
		apb2p = 1;
	}else
	{
		apb2p = APB1_PreScaler[temp-4]; // APB2 both have same prescalers
	}

	pclk1 = (SystemClk / ahbp) / apb2p;
	return pclk1;
}



