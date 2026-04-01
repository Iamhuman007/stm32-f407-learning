/*
 * 0x15_usart_tx.c
 *
 *  Created on: 04-Mar-2026
 *      Author: Prathamesh
 */


#include"stm32f407xx.h"
#include"string.h"

// PA0 CTS
// PA1 RTS    AF7
// PA2 Tx
// PA3 Rx


char msg[50] = "I hate you \n";
USART_Handle_t usart2_handle;







void delay(void){

	for(uint32_t i=0; i<500000; i++);
}

void USART2_GPIOInit(void)
{
	GPIO_Handle_t USART_pin;

	USART_pin.pGPIOx = GPIOA;
	USART_pin.GPIO_PinConfig.GPIO_PinAltFunMode = 7;
	USART_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	USART_pin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	USART_pin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	USART_pin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	USART_pin.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2;
	// USART Tx
	GPIO_Init(&USART_pin);

	USART_pin.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3;
	// USART RX
	GPIO_Init(&USART_pin);
}


void USART2_Init(void)
{


	usart2_handle.pUSARTx = USART2;
	usart2_handle.USART_Config.USART_Baud = USART_STD_BAUD_115200;
	usart2_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
	usart2_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
	usart2_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
	usart2_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
	usart2_handle.USART_Config.USART_Mode = USART_MODE_ONLY_TX;

	USART_Init(&usart2_handle);
}

int main(void)
{
	USART2_GPIOInit();

	USART2_Init();

	USART_PeripheralControl(USART2, ENABLE);


	while(1)
	{
		while(!GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0)){}

		delay();

		USART_SendData(&usart2_handle, (uint8_t *)msg, strlen(msg));
	}

	return(0);
}
