/*
 * stm32f407xx_gpio_driver.h
 *
 *  Created on: 20-Dec-2025
 *      Author: Dell
 */

#ifndef INC_STM32F407XX_GPIO_DRIVER_H_
#define INC_STM32F407XX_GPIO_DRIVER_H_

#include "stm32f407xx.h"
typedef struct
{
	uint8_t GPIO_PinNumber;
	uint8_t GPIO_PinMode;                /*!< possible values from @GPIO_PIN_MODES >*/
	uint8_t GPIO_PinSpeed;               /*!< possible values from @GPIO_SPEEDS >*/
	uint8_t GPIO_PinPuPdControl;
	uint8_t GPIO_PinOPType;
	uint8_t GPIO_PinAltFunMode;

}GPIO_PinConfig_t;

typedef struct
{
	// pointer to hold the base address of the GPIO peripheral
	GPIO_RegDef_t *pGPIOx;                      // this holds the base address of the GPIO port to which the pin belongs
	GPIO_PinConfig_t GPIO_PinConfig;            // this holds GPIO pin configuration settings
}GPIO_Handle_t;



/*
 * @GPIO_PIN_NUMBERS
 * GPIO pin possible numbers
 */
#define GPIO_PIN_NO_0          0
#define GPIO_PIN_NO_1          1
#define GPIO_PIN_NO_2          2
#define GPIO_PIN_NO_3          3
#define GPIO_PIN_NO_4          4
#define GPIO_PIN_NO_5          5
#define GPIO_PIN_NO_6          6
#define GPIO_PIN_NO_7          7
#define GPIO_PIN_NO_8          8
#define GPIO_PIN_NO_9          9
#define GPIO_PIN_NO_10         10
#define GPIO_PIN_NO_11         11
#define GPIO_PIN_NO_12         12
#define GPIO_PIN_NO_13         13
#define GPIO_PIN_NO_14         14
#define GPIO_PIN_NO_15         15

/*
 * @GPIO_PIN_MODES
 * GPIO pin possible modes
 */
#define GPIO_MODE_IN           0
#define GPIO_MODE_OUT          1
#define GPIO_MODE_ALTFN        2
#define GPIO_MODE_ANALOG       3
#define GPIO_MODE_IT_FT        4      // Falling edge
#define GPIO_MODE_IT_RT        5      // Rising edge
#define GPIO_MODE_IT_RFT       6      // Rising and Falling edge

/*
 * GPIO pin possible output types
 */
#define GPIO_OP_TYPE_PP        0       // push pull
#define GPIO_OP_TYPE_OD        1       // open drain

/*
 * @GPIO_SPEEDS
 * GPIO pin possible output types
 */
#define GPIO_SPEED_LOW         0
#define GPIO_SPEED_MEDIUM      1
#define GPIO_SPEED_FAST        3
#define GPIO_SPEED_HIGH        4

/*
 * GPIO pin possible pull up pull down config macros
 *
 */
#define GPIO_PIN_NO_PUPD           0
#define GPIO_PIN_PU            1
#define GPIO_PIN_PD            2

///**************************************************************************************************************************************************/
///****************************************************** APIs supported by this driver *************************************************************/
///**************************************************************************************************************************************************/
//
//// clock control
///*Documentation for each function
// define what are arguments etc get it from github
// https://github.com/niekiran/MasteringMCU.git
//
//
// pGPIOx- base address of the gpio peripheral
// */
//
//
//void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi){
//	if(EnorDi == ENABLE){
//
//		if(pGPIOx == GPIOA){
//			GPIOA_PCLK_EN();
//		}else if(pGPIOx == GPIOB){
//			GPIOB_PCLK_EN();
//		}else if(pGPIOx == GPIOC){
//			GPIOC_PCLK_EN();
//		}else if(pGPIOx == GPIOD){
//			GPIOD_PCLK_EN();
//		}else if(pGPIOx == GPIOE){
//			GPIOE_PCLK_EN();
//		}else if(pGPIOx == GPIOF){
//			GPIOF_PCLK_EN();
//		}else if(pGPIOx == GPIOG){
//			GPIOG_PCLK_EN();
//		}else if(pGPIOx == GPIOH){
//			GPIOH_PCLK_EN();
//		}else if(pGPIOx == GPIOI){
//			GPIOI_PCLK_EN();
//		}
//	}
//	else{
//
//		if(pGPIOx == GPIOA){ // correct it from github
//			GPIOA_PCLK_DI();
//		}else if(pGPIOx == GPIOB){
//			GPIOB_PCLK_EN();
//		}else if(pGPIOx == GPIOC){
//			GPIOC_PCLK_EN();
//		}else if(pGPIOx == GPIOD){
//			GPIOD_PCLK_EN();
//		}else if(pGPIOx == GPIOE){
//			GPIOE_PCLK_EN();
//		}else if(pGPIOx == GPIOF){
//			GPIOF_PCLK_EN();
//		}else if(pGPIOx == GPIOG){
//			GPIOG_PCLK_EN();
//		}else if(pGPIOx == GPIOH){
//			GPIOH_PCLK_EN();
//		}else if(pGPIOx == GPIOI){
//			GPIOI_PCLK_EN();
//		}
//	}
//}
//
//// init control
//void GPIO_Init(GPIO_Handle_t *pGPIOHandle){
//
//	//1. configure the mode of the gpio pin /*Why is there need to access GPIO mode via Handle and Pin mode?*/
//	   uint32_t temp=0;
//       if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG)
//       {
//    	   /*How do we know what is the value of Pin Mode, is it that the user creates a instance of PinConfig and handle structure and then passes them to this fn?*/
//    	   temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 *pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
//    	   pGPIOHandle->pGPIOx->MODER &= ~(0x3 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
//    	   pGPIOHandle->pGPIOx->MODER|=temp;
//    	   temp=0;
//       }else
//       {
//
//       }
//	//2. configure the speed
//       temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2 *pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
//       pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x3 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
//       pGPIOHandle->pGPIOx->OSPEEDR|=temp;
//	   temp=0;
//
//	//3. configure the pupd settings
//	   temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2 *pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
//	   pGPIOHandle->pGPIOx->PUPDR &= ~(0x3 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
//	   pGPIOHandle->pGPIOx->PUPDR|=temp;
//	   temp=0;
//
//	//4. configure the optype
//	   temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << (pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
//	   pGPIOHandle->pGPIOx->OTYPER &= ~(0x1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
//	   pGPIOHandle->pGPIOx->OTYPER|=temp;
//	   temp=0;
//
//	//5. configure the alt functionality
//		   if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN)
//	   {
//		// 0 to 7 ALT FN low register & 8 to 15 	ALT FN HIGH register
//		   uint8_t temp1=pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber/8; // pin no/8 if the pin no is 9 ie 9/8 =1 hence it should be placed in Alt Fn higher register
//		   uint8_t temp2=pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber%8;
//		   pGPIOHandle->pGPIOx->AFR[temp1] &= ~(0XF << (4 *temp2));
//		   pGPIOHandle->pGPIOx->AFR[temp1]|=(pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4 *temp2));
//		   temp=0;
//	   }
//
//}
//void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
//{
//
//	if(pGPIOx == GPIOA){
//		GPIOA_REG_RESET();
//	}else if(pGPIOx == GPIOB){
//		GPIOB_REG_RESET();
//	}else if(pGPIOx == GPIOC){
//		GPIOC_REG_RESET();
//	}else if(pGPIOx == GPIOD){
//		GPIOD_REG_RESET();
//	}else if(pGPIOx == GPIOE){
//		GPIOE_REG_RESET();
//	}else if(pGPIOx == GPIOF){
//		GPIOF_REG_RESET();
//	}else if(pGPIOx == GPIOG){
//		GPIOG_PCLK_EN();
//	}else if(pGPIOx == GPIOH){
//		GPIOH_REG_RESET();
//	}else if(pGPIOx == GPIOI){
//		GPIOI_REG_RESET();
//	}
//
//}
//
//// Data read and write
//uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber){
//
//	uint8_t temp;
//	temp = (uint8_t)((pGPIOx->IDR >> PinNumber)&0x00000001);
//	return temp;
//
//}
//uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx){
//
//    uint16_t value;
//	value = (uint16_t)pGPIOx->IDR;
//	return value;
//}
//void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value){
////	uint8_t temp = value << PinNumber;
////	pGPIOx->
//	if(Value == GPIO_PIN_SET){
//		pGPIOx->ODR |= ( 1 << PinNumber);
//	}
//	else{
//		pGPIOx->ODR &= ~( 1 << PinNumber);
//	}
//}
//
//void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value){
//	pGPIOx->ODR = Value;
//}
//
//void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber){
//	pGPIOx->ODR = pGPIOx->ODR ^ (1 << PinNumber);
//}
//
//// IRQ configuration and ISR handling
//void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t IRQPriority, uint8_t EnorDi);
//void GPIO_IRQHandling(uint8_t PinNumber);

/******************************************************************************************
 *								APIs supported by this driver
 *		 For more information about the APIs check the function definitions
 ******************************************************************************************/

/*
 * Peripheral Clock setup
 */
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi);

/*
 * Init and De-init
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);


/*
 * Data read and write
 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value);
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);


/*
 * IRQ Configuration and ISR handling
 */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void GPIO_IRQHandling(uint8_t PinNumber);


#endif /* INC_STM32F407XX_GPIO_DRIVER_H_ */
