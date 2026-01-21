/*
 * 000x3_led_Btn.c
 *
 *  Created on: 25-Dec-2025
 *      Author: Dell
 */


#include"stm32f407xx.h"

void delay(void){

	for(uint32_t i=0; i<500000/2; i++);
}

int main(void)
{
	GPIO_Handle_t GPIO_Led, GPIO_Btn;

	GPIO_Btn.pGPIOx=GPIOA;
	GPIO_Btn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GPIO_Btn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIO_Btn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_Btn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	GPIO_Led.pGPIOx = GPIOD ;
	GPIO_Led.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_Led.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GPIO_Led.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;  // when PUPD control and op type PUSH PULL config is used?
	//  if i use PUPDCONTROL as Pull up can i use output type as open drain?
	// why do i have to build the code two times for successful build????
	GPIO_Led.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GPIO_Led.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;


	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_PeriClockControl(GPIOD, ENABLE);
	GPIO_Init(&GPIO_Btn);
	GPIO_Init(&GPIO_Led);

	   while(1){

	      if(GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0)){
	       delay();
		   GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_7);

	      }
	   }
	return 0;
}
