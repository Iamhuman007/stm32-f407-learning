/*
 * 0x10I2CmasterTxTesting.c
 *
 *  Created on: 10-Feb-2026
 *      Author: Prathamesh
 */


#include<stdio.h>
#include<string.h>
#include "stm32f407xx.h"

void delay(void){

	for(uint32_t i=0; i<500000/2; i++);
}

// PB6 -> SCL
// PB9 -> SDA

I2C_Handle_t I2C1Handle; // why global instead on local to init fn like buttons

uint8_t somedata[] = "We are testing I2C master Tx \n"; // due to arduino serial side limitation connot send more than 32 bytes
uint8_t SLAVEADDR = 0x68; // have to check via arduino code


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
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_9;
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
	// pin inits
	I2C1_GPIOInits();

	buttoninits();

	// I2C peripheral config
	I2C_Inits();

	// enable the I2C peripheral
	I2C_PeripheralControl(I2C1, ENABLE);

	 while(1)
	{

		while(!GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0)){}
		delay();

		I2C_MasterSendData(&I2C1Handle, somedata, strlen((char*)somedata), SLAVEADDR);

	}

	while(1);
}
