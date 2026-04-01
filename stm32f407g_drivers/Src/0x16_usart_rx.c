/*
 * 0x16_usart_rx.c
 *
 *  Created on: 08-Mar-2026
 *      Author: Prathamesh
 */

#include"stm32f407xx.h"



char *mesg[] = {"hihiahadad","Usart is working","Boy i can do this"};// *mesg is a pointer and [] creates an array. Hence in this array pointers to hada, usart .. is stored.

//reply from arduino will be stored here
char rx_buf[1024] ;

USART_Handle_t usart2_handle; // why is it a global variable and not define in init?

//This flag indicates reception completion
uint8_t rxCmplt = RESET;

uint8_t g_data = 0;

void delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i ++);
}

void USART2_GPIOInit(void)
{
	GPIO_Handle_t Usartpin;

	Usartpin.pGPIOx = GPIOA;
	Usartpin.GPIO_PinConfig.GPIO_PinAltFunMode = 7;
	Usartpin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	Usartpin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	Usartpin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU; // IN UART do pins are only pulled down? Ans they are pulled
	// up and down to but UART engine does it (UP/DOWN) but to ensure that in idle state the pin is pulled up the PULL UP is activated.
	Usartpin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	Usartpin.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2;
	GPIO_Init(&Usartpin);

	Usartpin.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3;
	GPIO_Init(&Usartpin);

}

void USART2_Init(void)
{
	usart2_handle.pUSARTx = USART2;
	usart2_handle.pUSARTx = USART2;
	usart2_handle.USART_Config.USART_Baud = USART_STD_BAUD_115200;
	usart2_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
	usart2_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
	usart2_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
	usart2_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
	usart2_handle.USART_Config.USART_Mode = USART_MODE_ONLY_TX;


	return(0);
}





int main (void)
{

	uint32_t cnt = 0;

	USART2_GPIOInit();

	USART2_Init();

	USART_PeripheralControl(USART2, ENABLE);

	printf("Application is running\n");

	while(1)
	{
		while(!GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0)){}

		delay();

		cnt = cnt%3;

		//First lets enable the reception in interrupt mode
		//this code enables the receive interrupt
		while ( USART_ReceiveDataIT(&usart2_handle,rx_buf,strlen(mesg[cnt])) != USART_READY ); // where have we initialize the state of the buffer


	}
	return (0);
}

