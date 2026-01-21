/*
 * 005Button_Interrupt.c
 *
 *  Created on: 28-Dec-2025
 *      Author: Dell
 */

#include <string.h>
#include "stm32f407xx.h"

void delay(uint32_t delay){

	for(uint32_t i=0; i<delay; i++);
}

int main(void)
{
   GPIO_Handle_t GpioLed,GPIObtn;
   // Here the init fn is causing problem (LED not blinking)
   /* the GPIOBtn does not configure output type and garbage value are stored in that field we also dont check if the op is initialized in INIT fn
    * once GpioLed is configured as op type the same register "OPtype" is overwritten by garbage values causing the led to stop working
    * Solution: Clear each struct before initialization to clear any garbage value */
   memset(&GpioLed,0,sizeof(GpioLed));
   memset(&GPIObtn,0,sizeof(GPIObtn));


   GpioLed.pGPIOx=GPIOD;
   GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
   GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
   GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
   GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
   GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;



//   GPIO_PeriClockControl(GPIOD, ENABLE);
//   GPIO_Init(&GpioLed);

   GPIObtn.pGPIOx=GPIOD;
   GPIObtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
   GPIObtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
   GPIObtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

   GPIObtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;



//   GPIO_PeriClockControl(GPIOA, ENABLE);
   GPIO_PeriClockControl(GPIOD, ENABLE); //both peripheral are at port D

   GPIO_Init(&GPIObtn);
   GPIO_Init(&GpioLed);

   // IRQ configurations
   GPIO_IRQPriorityConfig(23, 15);
   GPIO_IRQInterruptConfig(23, ENABLE);// if interrupt from pin 5 to 9 is connected to IRQ 23 then who decides which pin among 9 to 5 interrupts
   // in the case where pin 5 and 7 are configured for interrupt what will happen?



   while(1);

   return 0;
}

void  EXTI9_5_IRQHandler(void)
 {
     delay(500000);// deboucning
	 GPIO_IRQHandling(GPIO_PIN_NO_5);
	 GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);
 }
