/*
 * 0x11_i2c_master_read.c
 *
 *  Created on: 25-Mar-2026
 *      Author: Prathamesh
 */




#include<stdio.h>
#include<string.h>
#include "stm32f407xx.h"

void delay(void){

	for(uint32_t i=0; i<500000/2; i++);
}

// PB6 -> SCL
// PB7 -> SDA

I2C_Handle_t I2C1Handle; // why global instead on local to init fn like buttons

//uint8_t somedata[] = "We are testing I2C master Tx \n"; // due to arduino serial side limitation connot send more than 32 bytes
uint8_t SLAVEADDR = 0x68; // have to check via arduino code

uint8_t rxComplt = RESET;

//receive buffer
uint8_t Data[32];

//#define CMD1 0x51
//#define CMD2 0x52
uint8_t CMD_Len = 0x51;
uint8_t CMD_Data = 0x52;

void I2C1_GPIOInits(void)
{
	GPIO_Handle_t I2CPins;
	I2CPins.pGPIOx = GPIOB;

	I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	I2CPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;

	// SCL
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&I2CPins);

	// SCL
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_Init(&I2CPins);


}

void buttoninits(void)
{
	   GPIO_Handle_t GPIObtn;
	   GPIObtn.pGPIOx=GPIOA;
	   GPIObtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	   GPIObtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	   GPIObtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	   GPIObtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;

	   GPIO_Init(&GPIObtn);
}

void I2C_Inits(void)
{
	I2C1Handle.pI2Cx = I2C1;
	I2C1Handle.I2C_Config.I2C_AckControl = I2C_ACK_ENABLE;
	I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
	I2C1Handle.I2C_Config.I2C_SCLSpeed    = I2C_SCL_SPEED_SM;
	I2C1Handle.I2C_Config.I2C_DeviceAddress = 0x61;// garbage

	I2C_Init(&I2C1Handle);


}

int main(void)
{

	uint8_t Len=0;


	// pin inits
	I2C1_GPIOInits();

	buttoninits();

	// I2C peripheral config
	I2C_Inits();

	// I2C_IRQ_Configurations,     I think its enabling NVIC
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);

	// enable the I2C peripheral ie sets PE bit
	I2C_PeripheralControl(I2C1, ENABLE);

	// For ACK to be enabled PE bit has to be 1
	I2C_ManageAcking(I2C1, ENABLE);

	 while(1)
	{

		while(!GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0)){}
		delay();

		while(I2C_MasterSendDataIT(&I2C1Handle, &CMD_Len, 1, SLAVEADDR,I2C_ENABLE_SR)!=I2C_READY){}
		while(I2C_MasterReceiveDataIT(&I2C1Handle, &Len, 1, SLAVEADDR, I2C_ENABLE_SR)!=I2C_READY){}
		rxComplt = RESET;
		printf("Length = %d\n", Len);


//		printf("Data Length is: %d \n", Len);

//		while(!GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0)){}
//		delay();
		printf("Sending 2nd command \n");
		while(I2C_MasterSendDataIT(&I2C1Handle, &CMD_Data, 1, SLAVEADDR, I2C_ENABLE_SR)!=I2C_READY){}
		while(I2C_MasterReceiveDataIT(&I2C1Handle, Data, Len, SLAVEADDR, I2C_DISABLE_SR)!=I2C_READY){}

		while(rxComplt == RESET){}

		Data[Len+1]='\0';

		printf("%s\n", Data);

		rxComplt = RESET;

	}

	while(1);
}



void I2C1_EV_IRQHandler(void)
{
	I2C_EV_IRQHandling(&I2C1Handle);
}

void I2C1_ER_IRQHandler()
{
	I2C_ER_IRQHandling(&I2C1Handle);
}

void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle,uint8_t AppEv)
{
     if(AppEv == I2C_EV_TX_CMPLT)
     {
    	 printf("Tx is completed\n");
     }else if (AppEv == I2C_EV_RX_CMPLT)
     {
    	 printf("Rx is completed\n");
    	 rxComplt = SET;
     }else if (AppEv == I2C_ERROR_AF)
     {
    	 printf("Error : Ack failure\n");
    	 //in master ack failure happens when slave fails to send ack for the byte
    	 //sent from the master.
    	 I2C_CloseSendData(pI2CHandle);

    	 //generate the stop condition to release the bus
    	 I2C_GenerateStopCondition(I2C1);

    	 //Hang in infinite loop
    	 while(1);
     }
}
