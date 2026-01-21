/*
 * stm32f407xx_spi_driver.c
 *
 *  Created on: 04-Jan-2026
 *      Author: Dell
 */

#include "stm32f407xx_spi_driver.h"




void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{

			if(pSPIx == SPI1){
				SPI1_PCLK_EN();
			}else if(pSPIx == SPI2){
				SPI2_PCLK_EN();
			}else if(pSPIx == SPI3){
				SPI3_PCLK_EN();
			}
//			}else if(pSPIx == SPI4){
//				SPI4_PCLK_EN();
//			}
	 }
	else
	{

			//todo
	}

}

uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
	if(pSPIx->SR & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}

void SPI_Init(SPI_Handle_t *pSPIHandle)
{

	SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);


    // configure the SPI_CR1 register
	uint32_t tempreg=0;
	// configure device mode
	tempreg |=(pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR);
	// bus config
	if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
	{
		// BIDI mode must be cleared
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);
	}else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		// BIDI mode should be set
		tempreg |= (1 << SPI_CR1_BIDIMODE);
	}else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RX_ONLY)
	{
		// BIDI mode should be cleared
		tempreg &= ~( 1 << SPI_CR1_BIDIMODE);
		// RXONLY bit must be set
		tempreg |= ( 1 << SPI_CR1_RXONLY);
	}
	// sclk config
	tempreg|=(pSPIHandle->SPIConfig.SPI_ScklSpeed << SPI_CR1_BR);// how does this work, in macro is it stored in bits?
	//
	tempreg|=(pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF);
	//
	tempreg|=(pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA);
	tempreg|=(pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL);  // why SPIConfig->SPI_CPOL is incorrect
	tempreg|=(pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM);

	pSPIHandle->pSPIx->CR1 = tempreg;


}
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len)
{
	while(Len > 0)
	{
		// firmware has no access to Tx or Rx buffer, indirect access via Data Register(28.5.4)
		   while (SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET );                               //!(pSPIx->SR & (1 << SPI_SR_TXE))

			if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
			{
                   // 16 bit data in to the DR
//				pSPIx->DR |= *pTxBuffer;
//				pTxBuffer++;
//				pSPIx->DR |= (*pTxBuffer << 8);   Is this method technically correct. How to understand that if i want to get only the first 16 or 8 bit
				                                  // that it can be done via type cast the pointer. I want to know this kind of embedded caviars
				                                  // How to gain such knowledge
				pSPIx->DR = *(uint16_t*)(pTxBuffer); // why * is added after uint16_t. Get explain about pointer of different type ie struct, uint32_t, int etc
				                                     // How they behave at memory level
				Len-=2;

				(uint16_t*)(pTxBuffer)++; // increments by 2

			}else
			{
				// 8 bit data in to the DR
				pSPIx->DR = *pTxBuffer;
				Len--;
				pTxBuffer++;


			}


     }
}


void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len)
{
	while(Len == 0)
		{
			// firmware has no access to Tx or Rx buffer, indirect access via Data Register(28.5.4)

		     // wait until RXNE is send
			while (SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_SET ){}                            //!(pSPIx->SR & (1 << SPI_SR_TXE))


				if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
				{

					 // Load data from DR to pRxBuffer addr
					*(uint16_t*)(pRxBuffer) = pSPIx->DR ;  // Loads two byte of data. How does it work and what different happens at memory level when *(uint16_t*) is removed
					// why * is added after uint16_t. Get explain about pointer of different type ie struct, uint32_t, int etc
					// How they behave at memory level
					Len-=2;

					(uint16_t*)(pRxBuffer)++; // increments by 2

				}else
				{
					// 8 bit data in to the DR
					*pRxBuffer = pSPIx->DR;
					Len--;
					pRxBuffer++;


				}


	}
}

void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SPE);
	}else
	{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
	}
}


/*********************************************************************
 * @fn      		  - SPI_SSIConfig
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
void  SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pSPIx->CR1 |=  (1 << SPI_CR1_SSI);
	}else
	{
		pSPIx->CR1 &=  ~(1 << SPI_CR1_SSI);
	}


}


void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pSPIx->CR2 |=  (1 << SPI_CR2_SSOE);
	}else
	{
		pSPIx->CR2 &=  ~(1 << SPI_CR2_SSOE);
	}
}









