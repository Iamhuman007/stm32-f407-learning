/*
 * stm32f407xx_gpio_driver.c
 *
 *  Created on: 20-Dec-2025
 *      Author: Dell
 */


#include "stm32f407xx_gpio_driver.h"


/**************************************************************************************************************************************************/
/****************************************************** APIs supported by this driver *************************************************************/
/**************************************************************************************************************************************************/

// clock control
/*Documentation for each function
 define what are arguments etc get it from github
 https://github.com/niekiran/MasteringMCU.git


 pGPIOx- base address of the gpio peripheral
 */


void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi){
	if(EnorDi == ENABLE){

		if(pGPIOx == GPIOA){
			GPIOA_PCLK_EN();
		}else if(pGPIOx == GPIOB){
			GPIOB_PCLK_EN();
		}else if(pGPIOx == GPIOC){
			GPIOC_PCLK_EN();
		}else if(pGPIOx == GPIOD){
			GPIOD_PCLK_EN();
		}else if(pGPIOx == GPIOE){
			GPIOE_PCLK_EN();
		}else if(pGPIOx == GPIOF){
			GPIOF_PCLK_EN();
		}else if(pGPIOx == GPIOG){
			GPIOG_PCLK_EN();
		}else if(pGPIOx == GPIOH){
			GPIOH_PCLK_EN();
		}else if(pGPIOx == GPIOI){
			GPIOI_PCLK_EN();
		}
	}
	else{

		if(pGPIOx == GPIOA){ // correct it from github
			GPIOA_PCLK_DI();
		}else if(pGPIOx == GPIOB){
			GPIOB_PCLK_EN();
		}else if(pGPIOx == GPIOC){
			GPIOC_PCLK_EN();
		}else if(pGPIOx == GPIOD){
			GPIOD_PCLK_EN();
		}else if(pGPIOx == GPIOE){
			GPIOE_PCLK_EN();
		}else if(pGPIOx == GPIOF){
			GPIOF_PCLK_EN();
		}else if(pGPIOx == GPIOG){
			GPIOG_PCLK_EN();
		}else if(pGPIOx == GPIOH){
			GPIOH_PCLK_EN();
		}else if(pGPIOx == GPIOI){
			GPIOI_PCLK_EN();
		}
	}
}

// init control
void GPIO_Init(GPIO_Handle_t *pGPIOHandle){

	// Enable the peripheral clock
	GPIO_PeriClockControl(pGPIOHandle->pGPIOx, ENABLE);

	//1. configure the mode of the gpio pin /*Why is there need to access GPIO mode via Handle and Pin mode?*/
	   uint32_t temp=0;
       if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG)
       {
    	   /*How do we know what is the value of Pin Mode, is it that the user creates a instance of PinConfig and handle structure and then passes them to this fn?*/
    	   temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 *pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
    	   pGPIOHandle->pGPIOx->MODER &= ~(0x3 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    	   pGPIOHandle->pGPIOx->MODER|=temp;
    	   temp=0;
       }else
       {
          // interrupt handling
    	   if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT)
    	   {
    		  // configure the FTSR
    		   EXTI->FTSR |= (1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    		   // disable the RTSR bit, may be configured in previous iteration
    		   EXTI->RTSR &= ~(1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

    	   }else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT)
    	   {
    		   // configure the RTSR

			   EXTI->RTSR |= (1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			   // disable the FTSR bit, may be configured in previous iteration
			   EXTI->FTSR &= ~(1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

    	   }else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT)
    	   {
    		   // configure both FTSR and RTSR
			   EXTI->RTSR |= (1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

			   EXTI->FTSR |= (1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    	   }

    	   //2. configure the GPIO port selection in SYSCFG_EXTICR
    	   uint8_t temp1=pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber/4; // since each CR(CR1, CR2) the first 16 bits are divide in 4 parts ie group of 4 bits, where each bit is dedicated for each PIN_NO
    	   uint8_t temp2=pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber%4;
    	   uint8_t portcode = GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);
    	   SYSCFG_PCLK_EN();
    	   SYSCFG->EXTICR[temp1] = (portcode << 4*temp2);
    	   //3. enable the exti interrupt delivery using IMR
    	   EXTI->IMR |= 1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber;
       }
	//2. configure the speed
       temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2 *pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
       pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x3 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
       pGPIOHandle->pGPIOx->OSPEEDR|=temp;
	   temp=0;

	//3. configure the pupd settings
	   temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2 *pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	   pGPIOHandle->pGPIOx->PUPDR &= ~(0x3 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	   pGPIOHandle->pGPIOx->PUPDR|=temp;
	   temp=0;

	//4. configure the optype
	   temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << (pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	   pGPIOHandle->pGPIOx->OTYPER &= ~(0x1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	   pGPIOHandle->pGPIOx->OTYPER|=temp;
	   temp=0;

	//5. configure the alt functionality
		   if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN)
	   {
		// 0 to 7 ALT FN low register &    8 to 15 	ALT FN HIGH register
		   uint8_t temp1=pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber/8; // pin no/8 if the pin no is 9 ie 9/8 =1 hence it should be placed in Alt Fn higher register
		   uint8_t temp2=pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber%8;
		   pGPIOHandle->pGPIOx->AFR[temp1] &= ~(0XF << (4 *temp2));
		   pGPIOHandle->pGPIOx->AFR[temp1]|=(pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4 *temp2));
		   temp=0;
	   }

}
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{

	if(pGPIOx == GPIOA){
		GPIOA_REG_RESET();
	}else if(pGPIOx == GPIOB){
		GPIOB_REG_RESET();
	}else if(pGPIOx == GPIOC){
		GPIOC_REG_RESET();
	}else if(pGPIOx == GPIOD){
		GPIOD_REG_RESET();
	}else if(pGPIOx == GPIOE){
		GPIOE_REG_RESET();
	}else if(pGPIOx == GPIOF){
		GPIOF_REG_RESET();
	}else if(pGPIOx == GPIOG){
		GPIOG_PCLK_EN();
	}else if(pGPIOx == GPIOH){
		GPIOH_REG_RESET();
	}else if(pGPIOx == GPIOI){
		GPIOI_REG_RESET();
	}

}

// Data read and write
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber){

	uint8_t temp;
	temp = (uint8_t)((pGPIOx->IDR >> PinNumber)&0x00000001);
	return temp;

}
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx){

    uint16_t value;
	value = (uint16_t)pGPIOx->IDR;
	return value;
}
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value){
//	uint8_t temp = value << PinNumber;
//	pGPIOx->
	if(Value == GPIO_PIN_SET){
		pGPIOx->ODR |= ( 1 << PinNumber);
	}
	else{
		pGPIOx->ODR &= ~( 1 << PinNumber);
	}
}

void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value){
	pGPIOx->ODR = Value;
}

void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber){
	pGPIOx->ODR = pGPIOx->ODR ^ (1 << PinNumber);
}

// IRQ configuration and ISR handling
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
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


void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	uint8_t iprx = IRQNumber/4;
	uint8_t iprx_section = IRQNumber%4;
	uint8_t shiftamount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDR + (iprx)) |= IRQPriority << shiftamount;// since NVIC Base addr is uint32 ie 4 bytes. each time 1 is added it is incremented by 1 byte instead of bit hence *4 was remove section 29 3rd lecture 28-12-2025

}
void GPIO_IRQHandling(uint8_t PinNumber)
{
	// clear the exti pr register corresponding to the pin number
	if(EXTI->PR & (1 << PinNumber))
	{
		// clear
		EXTI->PR |= (1 << PinNumber);
	}
}
