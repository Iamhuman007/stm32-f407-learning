/*
 * stm32f407xx_spi_driver.h
 *
 *  Created on: 04-Jan-2026
 *      Author: Dell
 */

#ifndef INC_STM32F407XX_SPI_DRIVER_H_
#define INC_STM32F407XX_SPI_DRIVER_H_

#include"stm32f407xx.h"

typedef struct
{
	uint8_t SPI_DeviceMode;
	uint8_t SPI_BusConfig;
	uint8_t SPI_ScklSpeed;
	uint8_t SPI_DFF;
	uint8_t SPI_CPOL;
	uint8_t SPI_CPHA;
	uint8_t SPI_SSM;
}SPI_Config_t;

typedef struct
{
	SPI_RegDef_t   *pSPIx;   /*<holds the base address of SPIxx(x:0,1,2) peripherals>*/
	SPI_Config_t    SPIConfig;
}SPI_Handle_t;


/*
 *@ SPI Device Mode
 */
#define SPI_DEVICE_MODE_MASTER                     1
#define SPI_DEVICE_MODE_SLAVE                      0

/*
 *@ SPI BusConfig
 */
#define SPI_BUS_CONFIG_FD                          1
#define SPI_BUS_CONFIG_HD                          2
#define SPI_BUS_CONFIG_SIMPLEX_RX_ONLY             3

/*
 *@ SPI Clock Speed
 */
#define SPI_SCLK_SPEED_DIV2                        0
#define SPI_SCLK_SPEED_DIV4                        1
#define SPI_SCLK_SPEED_DIV8                        2
#define SPI_SCLK_SPEED_DIV16                       3
#define SPI_SCLK_SPEED_DIV32                       4
#define SPI_SCLK_SPEED_DIV64                       5
#define SPI_SCLK_SPEED_DIV128                      6
#define SPI_SCLK_SPEED_DIV256                      7

/*
 *@ SPI DFF
 */
#define SPI_DFF_8BITS                              0
#define SPI_DFF_16BITS                             1

/*
 *@ SPI CPOL
 */

#define SPI_CPOL_HIGH                              1
#define SPI_CPOL_LOW                               0

/*
 *@ SPI CPHA
 */

#define SPI_CPHA_HIGH                              1
#define SPI_CPHA_LOW                               0

/*
 *@ SPI SSM
 */

#define SPI_SSM_EN                                 1
#define SPI_SSM_DI                                 0


/**
 * SPI related status flags definition
 */
#define SPI_TXE_FLAG                           (1 << SPI_SR_TXE)
#define SPI_RXNE_FLAG                          (1 << SPI_SR_RXNE)
#define SPI_BUSY_FLAG                          (1 << SPI_SR_BSY)



/******************************************************************************************
 *								APIs supported by this driver
 *		 For more information about the APIs check the function definitions
 ******************************************************************************************/

/*
 * Peripheral Clock setup
 */
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi);

/*
 * Init and De-init
 */
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_RegDef_t *pSPIx);

/*
 *  Data send and receive
 */
// Blocking(polling based) and Non-Blocking(interrupt based)
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len);

void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len);


/*
 * IRQ Configuration and ISR handling
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pHandle);


/*
 *  Other Peripheral Control APIs
 */
// enable or disable SPE bit
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi);
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx , uint32_t FlagName);

void  SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);

void  SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);


#endif /* INC_STM32F407XX_SPI_DRIVER_H_ */
