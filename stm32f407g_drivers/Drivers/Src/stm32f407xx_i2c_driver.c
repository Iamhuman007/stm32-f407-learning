/*
 * stm32f407xx_i2c_driver.c
 *
 *  Created on: 05-Feb-2026
 *      Author: Prathamesh
 */

#include "stm32f407xx_i2c_driver.h"

uint16_t AHB_PreScaler[8] = {2,4,8,16,64,128,256,512};
uint16_t APB1_PreScaler[8] = {2,4,8,16};



static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx);


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
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{

		if(pI2Cx == I2C1){
			I2C1_PCLK_EN();
		}else if(pI2Cx == I2C2){
			I2C2_PCLK_EN();
		}else if(pI2Cx == I2C3){
			I2C3_PCLK_EN();
		}
//			}else if(pI2Cx == I2C4){
//				I2C4_PCLK_EN();
//			}
	 }
	else
	{
		if(pI2Cx == I2C1){
			I2C1_PCLK_DI();
		}else if(pI2Cx == I2C2){
			I2C2_PCLK_DI();
		}else if(pI2Cx == I2C3){
			I2C3_PCLK_DI();
		}

	}

}




void I2C_Init(I2C_Handle_t *pI2CHandle)
{

	uint32_t tempreg = 0 ;

	//enable the clock for the i2cx peripheral
	I2C_PeriClockControl(pI2CHandle->pI2Cx,ENABLE);

	// Enable the Acking
	tempreg |= (pI2CHandle->I2C_Config.I2C_AckControl << I2C_CR1_ACK);
	pI2CHandle->pI2Cx->CR1 = tempreg;

	// configure the FREQ field of CR2
	tempreg = 0;
	tempreg |= RCC_GetPCLK1Value()/1000000U ;
	pI2CHandle->pI2Cx->CR2 |= (tempreg & 0x3F);

	// configure the device address (in slave mode) assuming only 7 bit (for 10 bit if else should be done)
	tempreg = 0;
	tempreg |= (pI2CHandle->I2C_Config.I2C_DeviceAddress << 1);
	tempreg |= (1 << 14); // Mandatory by software
	pI2CHandle->pI2Cx->OAR1 |= tempreg;

    // CCR calculations
	uint16_t ccr_value = 0;
	tempreg = 0;
	if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
	{
		// mode is Standard
		ccr_value = (RCC_GetPCLK1Value()/(2*pI2CHandle->I2C_Config.I2C_SCLSpeed));
		tempreg |= (ccr_value & 0xFFF);
	}
	else
	{
		// mode is fast mode
		tempreg |= (1 << 15); // DUTY cycle config
		tempreg |= (pI2CHandle->I2C_Config.I2C_FMDutyCycleSCLSpeed << 14);
		if(pI2CHandle->I2C_Config.I2C_FMDutyCycleSCLSpeed == I2C_FM_DUTY_2)
		{
			ccr_value = (RCC_GetPCLK1Value()/(3*pI2CHandle->I2C_Config.I2C_SCLSpeed));
		}
		else
		{
			ccr_value = (RCC_GetPCLK1Value()/(25*pI2CHandle->I2C_Config.I2C_SCLSpeed));
		}
		tempreg |= (ccr_value & 0xFFF);
	}
	pI2CHandle->pI2Cx->CCR |= tempreg;

	// Configure the rise time for I2C pins
}

void I2C_DeInit(I2C_RegDef_t *pI2Cx)
{
	// i think we just have to set all CR registers to 0
}


void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *Txbuffer, uint32_t Len, uint8_t SlaveAddr)
{
	// Generate START condition
	I2C_GenerateStartCondition();

	// Read (ie wait until SB is set) SB bit in SR1
	while(I2C_GETFlagStatus(I2C_Handle_t *pI2CHandle)){}
	// Write slave address in DR
	pI2Cx->DR |= pI2CHandle->
}


static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx)
{
	// set start bit in CR1
	pI2Cx->CR1 |= (1 << I2C_CR1_START);

}




// IRQ configuration and ISR handling
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
   // cortex -M4 Device doc
	if(EnorDi == ENABLE)
		{
			if(IRQNumber <= 31)
			{
				//program ISER0 register
				*NVIC_ISER0 |= ( 1 << IRQNumber );

			}else if(IRQNumber > 31 && IRQNumber < 64 ) //32 to 63
			{
				//program ISER1 register
				*NVIC_ISER1 |= ( 1 << (IRQNumber % 32) );
			}
			else if(IRQNumber >= 64 && IRQNumber < 96 )
			{
				//program ISER2 register //64 to 95
				*NVIC_ISER2 |= ( 1 << (IRQNumber % 64) );
			}
		}else
		{
			if(IRQNumber <= 31)
			{
				//program ICER0 register
				*NVIC_ICER0 |= ( 1 << IRQNumber );
			}else if(IRQNumber > 31 && IRQNumber < 64 )
			{
				//program ICER1 register
				*NVIC_ICER1 |= ( 1 << (IRQNumber % 32) );
			}
			else if(IRQNumber >= 64 && IRQNumber < 96 )
			{
				//program ICER2 register
				*NVIC_ICER2 |= ( 1 << (IRQNumber % 64) );
			}
		}
}

void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	uint8_t iprx = IRQNumber/4;
	uint8_t iprx_section = IRQNumber%4;
	uint8_t shiftamount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDR + (iprx)) |= IRQPriority << shiftamount;// since NVIC Base addr is uint32 ie 4 bytes. each time 1 is added it is incremented by 1 byte instead of bit hence *4 was remove section 29 3rd lecture 28-12-2025

}




void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
		pI2Cx->CR1 |= (1<<I2C_CR1_PE);
	else
		pI2Cx->CR1 &= ~(1<<I2C_CR1_PE);

}
