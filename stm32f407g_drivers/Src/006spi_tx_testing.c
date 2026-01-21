/*
 * 006spi_tx_testing.c
 *
 *  Created on: 09-Jan-2026
 *      Author: Dell
 */
//pB15 MOSI    pB14 MISO   PB13 SCLK    PB12 NSS
//AF5
#include"stm32f407xx.h"
#include"string.h"
//#include"stm32f407xx_spi_driver.c"
//#include"stm32f407xx_gpio_driver.c" already included in stm32f407xx.h


void SPI2_GPIOInits(void)
{
	GPIO_Handle_t SPIpins;
	SPIpins.pGPIOx = GPIOB;
	SPIpins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIpins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPIpins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;
	SPIpins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	SPIpins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	//SCLK
	SPIpins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&SPIpins);

	//MOSI
	SPIpins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&SPIpins);

	//MISO
	SPIpins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	GPIO_Init(&SPIpins);

	//NSS
	SPIpins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_Init(&SPIpins);

}

void SPI2_Inits(void)
{
	SPI_Handle_t SPI2handle;

	SPI2handle.pSPIx = SPI2;
	SPI2handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2handle.SPIConfig.SPI_CPOL= SPI_CPOL_LOW;
	SPI2handle.SPIConfig.SPI_CPHA= SPI_CPOL_LOW;
	SPI2handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI2handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2handle.SPIConfig.SPI_SSM =SPI_SSM_EN;
	SPI2handle.SPIConfig.SPI_ScklSpeed = SPI_SCLK_SPEED_DIV2;

	 SPI_Init(&SPI2handle);

}


int main(void)
{
	char user_data[] = "Hello world";
    SPI2_GPIOInits();

    SPI2_Inits();

    SPI_SSIConfig(SPI2, ENABLE);
    // to enable SPI SPE has to be set in control register, all the config has to be done while the SPE bit is reset
    SPI_PeripheralControl(SPI2, ENABLE);

    SPI_SendData(SPI2, (uint8_t*)user_data, strlen(user_data));

    // Before Disabling the SPI peripheral we have to enusre that BSY flag is reset
    while(SPI_GetFlagStatus(SPI2, SPI_BUSY_FLAG)){}

    // Disable the peripheral,
    SPI_PeripheralControl(SPI2, DISABLE);

    while(1){}





	return 0;
}
