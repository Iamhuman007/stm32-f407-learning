/*
 * 002Ledbutton.c
 *
 *  Created on: 24-Dec-2025
 *      Author: Dell
 */


/*
 * 001LED_TOOGLE.c
 *
 *  Created on: 23-Dec-2025
 *      Author: Dell
 */
#include "stm32f407xx.h"

void delay(void){

	for(uint32_t i=0; i<500000/2; i++);
}

int main(void)
{
   GPIO_Handle_t GpioLed,GPIObtn;
   GpioLed.pGPIOx=GPIOD;
   GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
   GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
   GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
   GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
   GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;



//   GPIO_PeriClockControl(GPIOD, ENABLE);
//   GPIO_Init(&GpioLed);

   GPIObtn.pGPIOx=GPIOA;
   GPIObtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
   GPIObtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
   GPIObtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

   GPIObtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;



   GPIO_PeriClockControl(GPIOA, ENABLE);
   GPIO_PeriClockControl(GPIOD, ENABLE);
   GPIO_Init(&GpioLed);
   GPIO_Init(&GPIObtn);

   while(1){

      if(GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0)){
       delay();
	   GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);

      }
   }
   return 0;
}
