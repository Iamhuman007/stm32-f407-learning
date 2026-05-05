/*
 * lcd.c
 *
 *  Created on: 16-Apr-2026
 *      Author: Prathamesh
 */

#include "lcd.h"

static void write_4_bits(uint8_t value);
static void lcd_enable(void);
static void mdelay(uint32_t cnt);
static void udelay(uint32_t cnt);


void lcd_send_command(uint8_t cmd)
{
	/*RS=0 for LCD command*/
	 GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_RESET);

	 /*RW=0 for LCD write*/
	 GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);

	 write_4_bits(cmd >> 4);

	 write_4_bits(cmd & 0x0F);

}

void lcd_print_char(uint8_t data)
{
	/*RS=1 for LCD data*/
	 GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_SET);

	 /*RW=0 for LCD write*/
	 GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);

	 write_4_bits((data >> 4)); // Higher Nibble

	 write_4_bits((data & 0x0F));  // Lower Nibble

	 // No delay required for data
}

void lcd_print_string(char *message)
{
	do
	{
		lcd_print_char((uint8_t)*message++);

	}while(*message != '\0');
}

void lcd_init(void)
{

	//1. Configure the gpio pins which are used for lcd connections

	GPIO_Handle_t lcd_signal;

	lcd_signal.pGPIOx = LCD_GPIO_PORT;
	lcd_signal.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_RS;
	lcd_signal.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	lcd_signal.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;
	lcd_signal.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_RW;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_EN;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D4;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D5;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D6;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D7;
	GPIO_Init(&lcd_signal);

	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D4, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D5, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D6, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D7, GPIO_PIN_RESET);

	//2. Do the LCD initialization

    mdelay(40); // delay after power on

    /* RS = 0 , for LCD command*/
    GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_RESET);

    /* RW = 0 , for LCD command*/
    GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);

    write_4_bits(0x03); // 0 0 1 1 ;

    mdelay(5); // waiting for 4.1ms

    write_4_bits(0x03); // 0 0 1 1 ;

    udelay(150);

    write_4_bits(0x03); // 0 0 1 1 ;
    write_4_bits(0x02); // 0 0 1 0 ;


    // Function set commands
    lcd_send_command(LCD_CMD_4DL_2N_5X8F);

    // display on and cursor on
    lcd_send_command(LCD_CMD_DON_CURON_BLNK);

    // display clear
    lcd_display_clear();


    //Entry mode st
    lcd_send_command(LCD_CMD_DIS_RETURN_HOME);

}

static void write_4_bits(uint8_t value)
{
//	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D7, ((value >> 3) & 0x1));
//	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D6, ((value >> 2) & 0x1));
//	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D5, ((value >> 1) & 0x1));
//	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D4, ((value >> 0) & 0x1));

	GPIO_WriteToOutputPin(LCD_GPIO_PORT,LCD_GPIO_D4, ((value >> 0) & 0x1) );
	GPIO_WriteToOutputPin(LCD_GPIO_PORT,LCD_GPIO_D5, ((value >> 1) & 0x1) );
	GPIO_WriteToOutputPin(LCD_GPIO_PORT,LCD_GPIO_D6, ((value >> 2) & 0x1) );
	GPIO_WriteToOutputPin(LCD_GPIO_PORT,LCD_GPIO_D7, ((value >> 3) & 0x1) );

	lcd_enable(); // High to low transition on EN pin
}

static void lcd_enable(void)
{
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_SET);
    udelay(10);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_RESET);
	udelay(100);
}

void lcd_display_clear(void)
{
	lcd_send_command(LCD_CMD_DIS_CLEAR);
	mdelay(2);
}

/* Cursor returns to home position*/
void lcd_display_return_home(void)
{
	lcd_send_command(LCD_CMD_DIS_RETURN_HOME);

	mdelay(2);
}


/**
  *   Set Lcd to a specified location given by row and column information
  *   Row Number (1 to 2)
  *   Column Number (1 to 16) Assuming a 2 X 16 characters display
  *   The first row command is 80 (ie D7 bit is 1 which is ORED with the 00)
  *   The second row command is C0 ie D7  is set to 1 and ORED with 40 refer pg NO 24 set DDRAM address
  *   This is JUST SET cursor and not shift cursor
  */
void lcd_set_cursor(uint8_t row, uint8_t column)
{
  column--;
  switch (row)
  {
    case 1:
      /* Set cursor to 1st row address and add index*/
      lcd_send_command((column |= 0x80));
      break;
    case 2:
      /* Set cursor to 2nd row address and add index*/
        lcd_send_command((column |= 0xC0));
      break;
    default:
      break;
  }
}


static void mdelay(uint32_t cnt)
{
	for(uint32_t i=0; i<(cnt*1000); i++);
}

static void udelay(uint32_t cnt)
{
	for(uint32_t i=0; i<(cnt*1); i++);
}
