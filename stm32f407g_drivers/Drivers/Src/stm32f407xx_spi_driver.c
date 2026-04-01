/*
 * stm32f407xx_spi_driver.c
 *
 *  Created on: 04-Jan-2026
 *      Author: Dell
 */

#include "stm32f407xx_spi_driver.h"

// static is used to make the fn private
static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_rnxe_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle);

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
	tempreg|=(pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR);// how does this work, in macro is it stored in bits?
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
		   while (SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET ) {}                              //!(pSPIx->SR & (1 << SPI_SR_TXE))

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
	while(Len > 0)
		{
			// firmware has no access to Tx or Rx buffer, indirect access via Data Register(28.5.4)

		     // wait until RXNE is send
			while (SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET ){}                            //!(pSPIx->SR & (1 << SPI_SR_TXE))


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


//  Non-Blocking(interrupt based)
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->TxState;
	if(state != SPI_BUSY_IN_TX)
	{
		// save the Tx buffer address and Len info in some global variables
		pSPIHandle->pTxBuffer = pTxBuffer;
		pSPIHandle->TxLen = Len;



		// Makr the SPI state as busy in tx so that no other code can take over same SPI peripherals util tx is over
		pSPIHandle->TxState = SPI_BUSY_IN_TX;
		// Enable TxEIE
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXDEIE);

		// Data tx will be handle by IRQ
	}


	return state;
}

uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->RxState;
		if(state != SPI_BUSY_IN_RX)
		{
			// save the Tx buffer address and Len info in some global variables
			pSPIHandle->pRxBuffer = pRxBuffer;
			pSPIHandle->RxLen = Len;



			// Makr the SPI state as busy in tx so that no other code can take over same SPI peripherals util tx is over
			pSPIHandle->RxState = SPI_BUSY_IN_RX;
			// Enable TxEIE
			pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);

			// Data tx will be handle by IRQ
		}


		return state;
}



void SPI_IRQHandling(SPI_Handle_t *pHandle)
{
	// interrupt can be cause due to 6 different events in SPI
	// we have to find which one and address it

	uint8_t temp1,temp2;

    // first TXE
	temp1 = pHandle->pSPIx->SR & (1<<SPI_SR_TXE);
	temp2 = pHandle->pSPIx->CR2 & (1<<SPI_CR2_TXDEIE);

	if(temp1 && temp2)
	{
		// handle TXE .. He was saying this fn will not be exposed to user or app
		spi_txe_interrupt_handle(pHandle);
	}

    // RXNE
	temp1 = pHandle->pSPIx->SR & (1<<SPI_SR_RXNE);
	temp2 = pHandle->pSPIx->CR2 & (1<<SPI_CR2_RXNEIE);

	if(temp1 && temp2)
	{
		// handle TXE .. He was saying this fn will not be exposed to user or app
		spi_rnxe_interrupt_handle(pHandle);
	}

	// OVR
	temp1 = pHandle->pSPIx->SR & (1<<SPI_SR_OVR);
	temp2 = pHandle->pSPIx->CR2 & (1<<SPI_CR2_ERRIE);

	if(temp1 && temp2)
	{
		// handle TXE .. He was saying this fn will not be exposed to user or app
		spi_ovr_err_interrupt_handle(pHandle);
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



/*********************************************************************
 * @fn      		  - IRQ handling
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

// IRQ configuration and ISR handling
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
   // cortex -M4 Device doc
	if(EnorDi == ENABLE)
		{
			if(IRQNumber <= 31)
			{
				//program ISER0 register
				*NVIC_ISER0 |= ( 1 << IRQNumber );

			}else if(IRQNumber > 31 && IRQNumber < 64 ) //32 to 63
			{
				//program ISER1 register
				*NVIC_ISER1 |= ( 1 << (IRQNumber % 32) );
			}
			else if(IRQNumber >= 64 && IRQNumber < 96 )
			{
				//program ISER2 register //64 to 95
				*NVIC_ISER2 |= ( 1 << (IRQNumber % 64) );
			}
		}else
		{
			if(IRQNumber <= 31)
			{
				//program ICER0 register
				*NVIC_ICER0 |= ( 1 << IRQNumber );
			}else if(IRQNumber > 31 && IRQNumber < 64 )
			{
				//program ICER1 register
				*NVIC_ICER1 |= ( 1 << (IRQNumber % 32) );
			}
			else if(IRQNumber >= 64 && IRQNumber < 96 )
			{
				//program ICER2 register
				*NVIC_ICER2 |= ( 1 << (IRQNumber % 64) );
			}
		}
}

void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	uint8_t iprx = IRQNumber/4;
	uint8_t iprx_section = IRQNumber%4;
	uint8_t shiftamount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDR + (iprx)) |= IRQPriority << shiftamount;// since NVIC Base addr is uint32 ie 4 bytes. each time 1 is added it is incremented by 1 byte instead of bit hence *4 was remove section 29 3rd lecture 28-12-2025

}






static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF))
	{
		  // 16 bit
		pSPIHandle->pSPIx->DR = *(uint16_t*)(pSPIHandle->pTxBuffer); // why * is added after uint16_t. Get explain about pointer of different type ie struct, uint32_t, int etc
											 // How they behave at memory level
		pSPIHandle->TxLen-=2;

		(uint16_t*)(pSPIHandle->pTxBuffer)++; // increments by 2

	}else
	{
		// 8 bit data in to the DR
		pSPIHandle->pSPIx->DR = *pSPIHandle->pTxBuffer;
		pSPIHandle->TxLen--;
		pSPIHandle->pTxBuffer++;


	}
	if(!pSPIHandle->TxLen )
	{
		// Tx len=0 close the communication and inform the app that Tx is over

		//deactivate TX interrupt bit futhure interrupt is prevent after communication is over
		SPI_CloseTransimission(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_CMPLT);
	}
}


static void spi_rnxe_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF))
		{
			  // 16 bit
		    *(uint16_t*)(pSPIHandle->pRxBuffer) = pSPIHandle->pSPIx->DR;
			pSPIHandle->RxLen-=2;

			(uint16_t*)(pSPIHandle->pRxBuffer)--; // increments by 2

		}else
		{
			// 8 bit data in to the DR
			*pSPIHandle->pRxBuffer = pSPIHandle->pSPIx->DR;
			pSPIHandle->RxLen--;
			pSPIHandle->pRxBuffer++;


		}
		if(!pSPIHandle->RxLen )
		{
			// Tx len=0 close the communication and inform the app that Tx is over

			//deactivate TX interrupt bit futhure interrupt is prevent after communication is over
			SPI_CloseReception(pSPIHandle);
			SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_RX_CMPLT);
		}
}
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle)
{

	uint8_t temp;
	//clear the ovr flag  .... I didnt understand why TX state has to be cheacked
	if(pSPIHandle->TxState != SPI_BUSY_IN_TX)
	{
		temp = pSPIHandle->pSPIx->DR; // as mentioned in rm009 doc to clear OVR
		temp = pSPIHandle->pSPIx->SR;
	}
	(void)temp;
	// inform the application
	SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_OVR_ERR);

}



void SPI_CloseTransimission(SPI_Handle_t *pSPIHandle)
{
		//deactivate TX interrupt bit futhure interrupt is prevent after communication is over
		pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXDEIE);
		pSPIHandle->pTxBuffer = NULL;
		pSPIHandle->TxLen = 0;
		pSPIHandle->TxState = SPI_READY;
}

void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{
		//deactivate TX interrupt bit futhure interrupt is prevent after communication is over
		pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);
		pSPIHandle->pRxBuffer = NULL;
		pSPIHandle->RxLen = 0;
		pSPIHandle->RxState = SPI_READY;

}


void SPI_ClearOVRFlag(SPI_Handle_t *pSPIHandle)
{
	uint8_t temp;
	temp = pSPIHandle->pSPIx->DR; // as mentioned in rm009 doc to clear OVR
	temp = pSPIHandle->pSPIx->SR;
	(void)temp; // temp is unused
}


__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv)
{
	// this is a weak implementation which the user can overwrite
}




