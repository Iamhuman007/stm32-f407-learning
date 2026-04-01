/*
 * 0x13_i2c_slave_tx_string.c
 *
 *  Created on: 31-Mar-2026
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


//receive buffer
uint8_t TxBuff[32] = "STM32 Slave Mode Testing";

//#define CMD1 0x51
//#define CMD2 0x52
uint8_t CMD_Len = 0x51;
uint8_t CMD_Data = 0x52;
uint8_t commandcode = 0;
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
	I2C1Handle.I2C_Config.I2C_DeviceAddress = SLAVEADDR;// addr of stm

	I2C_Init(&I2C1Handle);


}

int main(void)
{

//	uint8_t Len=0;


	// pin inits
	I2C1_GPIOInits();

	buttoninits();

	// I2C peripheral config
	I2C_Inits();

	// I2C_IRQ_Configurations,     I think its enabling NVIC
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);

	I2C_SlaveENDISCallbackEvents(I2C1Handle.pI2Cx, ENABLE); // Enables the interrupt control bits in the CR2 register, previously MASTER send and rcv fn did it

	// enable the I2C peripheral ie sets PE bit
	I2C_PeripheralControl(I2C1, ENABLE);

	// For ACK to be enabled PE bit has to be 1
	I2C_ManageAcking(I2C1, ENABLE);

	while(1){}
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
    static uint8_t Cnt = 0;

	if (AppEv == I2C_EV_DATA_REQ)
     {
    	 // Master wants  some data, slave have to send it

    	 if(commandcode == 0x51)
    	 {
    		 // Send master lenght info
    		 I2C_SlaveSendData(pI2CHandle->pI2Cx, strlen((char*)TxBuff));
    	 }
    	 else if(commandcode == 0x52)
    	 {
    		 // Send master the string
    		 I2C_SlaveSendData(pI2CHandle->pI2Cx,TxBuff[Cnt++]);
    	 }

     }else if (AppEv == I2C_EV_DATA_RCV)
     {
    	 // Master wants to send data, slave have to recieve it
    	 commandcode = I2C_SlaveReceiveData(pI2CHandle->pI2Cx);

     }else if (AppEv == I2C_ERROR_AF)
     {
    	 // This happens only in slave txing
    	 // Maste has sent NACK so the slave has to understand that the master doesnt need more data
    	 commandcode = 0xff; // this happends after slve send data is executes after reciving 51 and 52 command
    	 Cnt = 0;
     }else if (AppEv == I2C_EV_STOP)
     {
    	 // This happens only during slave reception
    	 // Master has enabled the I2C communication with the slave
     }
}
