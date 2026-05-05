/*
 * 0x17_rtc_led.c
 *
 *  Created on: 16-Apr-2026
 *      Author: Prathamesh
 */

#include <stdio.h>

#include "..\bsp\ds1307.h"
#include "..\bsp\lcd.h"

#define SYSTICK_TIM_CLK 16000000UL

void init_systick_timer(uint32_t tick_hz) // tick_hz is the freq at which the counter becomes 0 in our case its 1 sec
{
	uint32_t *pSRVR = (uint32_t*)0xE000E014;
	uint32_t *pSCSR = (uint32_t*)0xE000E010;

    /* calculation of reload value */
    uint32_t count_value = (SYSTICK_TIM_CLK/tick_hz)-1;

    //Clear the value of SVR
    *pSRVR &= ~(0x00FFFFFFFF);

    //load the value in to SVR
    *pSRVR |= count_value;

    //do some settings
    *pSCSR |= ( 1 << 1); //Enables SysTick exception request:
    *pSCSR |= ( 1 << 2);  //Indicates the clock source, processor clock source

    //enable the systick
    *pSCSR |= ( 1 << 0); //enables the counter

}

static void mdelay(uint32_t cnt)
{
	for(uint32_t i=0; i<(cnt*1000); i++);
}

char* get_day(uint8_t i)
{
	char* days[] = {"SUNDAY","MONDAY", "TUESDAY", "WEDNESDAY" , "THRUSDAY" , "FRIDAY" ,"SATURDAY"};

	return days[i - 1];
}


void number_to_string(uint8_t number, char* buf)
{
    if(number < 99)
    {
		buf[0] = (number/10) + 48;
		buf[1] = (number%10) + 48;
    }
    else
    {
		buf[0] = 'x';
		buf[1] = 'x';
    }

}

char* time_to_string(RTC_time_t *current_time)
{
	static char s[9]; // why is this static here and what issue it prevents when returning
	s[2] = ':';
	s[5] = ':';
	s[8] = '\0';
	number_to_string(current_time->hours, &s[0]);
	number_to_string(current_time->minutes, &s[3]);
	number_to_string(current_time->seconds, &s[6]); // different in &buf[0], buf[0]

	return s;
}

char* date_to_string(RTC_date_t *current_date)
{
	static char s[11];
	s[2] = '/';
	s[5] = '/';
	s[10] = '\0';

	number_to_string(current_date->date, &s[0]);
	number_to_string(current_date->month, &s[3]);
	number_to_string(current_date->year, &s[6]);

	return s;
}


int main()
{
	RTC_time_t current_time;
	RTC_date_t current_date;

	lcd_init();
	lcd_print_char('A');
	lcd_print_string("Rtc_Test...");

	mdelay(2000);
	lcd_display_clear();
	lcd_display_return_home();

	printf("test\n");
	if(ds1307_init())
	{
	   printf("RTC init Failed \n");
	   while(1);
	}

	init_systick_timer(1);

	current_date.day = FRIDAY;
	current_date.date = 2;
	current_date.month = 5;
	current_date.year = 26;

	current_time.time_format = TIME_FORMAT_12HRS_PM ;
	current_time.hours = 11;
	current_time.minutes = 59;
	current_time.seconds = 45;

	ds1307_set_current_date(&current_date);
	ds1307_set_current_time(&current_time);

	ds1307_get_current_time(&current_time);
	ds1307_get_current_date(&current_date);

	char *am_pm;
	if(current_time.time_format != TIME_FORMAT_24HRS)
	{
		am_pm = (current_time.time_format == TIME_FORMAT_12HRS_PM) ? "PM": "AM";
//		printf("CURRENT TIME = %s %s  \n", time_to_string(&current_time), am_pm); // 4:45:34 AM
		lcd_print_string(time_to_string(&current_time));
		lcd_print_string(am_pm);
	}
	else
//		printf("CURRENT TIME = %s  \n", time_to_string(&current_time));  // 13:45:34
		lcd_print_string(time_to_string(&current_time));

//	printf("CURRENT DATE = %s  %s \n", date_to_string(&current_date), get_day(current_date.day));  // 13:45:34
	lcd_set_cursor(2, 1);
	lcd_print_string(date_to_string(&current_date));


	while(1);
	return 0;
}


void SysTick_Handler(void)
{
	RTC_time_t current_time;
	RTC_date_t current_date;

	lcd_display_clear();
	lcd_display_return_home();

	lcd_set_cursor(1, 1);
	ds1307_get_current_time(&current_time);

	char *am_pm;
	if(current_time.time_format != TIME_FORMAT_24HRS)
	{
		am_pm = (current_time.time_format == TIME_FORMAT_12HRS_PM) ? "PM": "AM";
//		printf("CURRENT TIME = %s %s  \n", time_to_string(&current_time), am_pm); // 4:45:34 AM
		lcd_print_string(time_to_string(&current_time));
		lcd_print_char(' ');
		lcd_print_string(am_pm);
	}
	else
		lcd_print_string(time_to_string(&current_time));
//		printf("CURRENT TIME = %s  \n", time_to_string(&current_time));  // 13:45:34



	ds1307_get_current_date(&current_date);
//	printf("CURRENT DATE = %s  %s \n", date_to_string(&current_date), get_day(current_date.day));  // 13:45:34
	lcd_set_cursor(2, 1);
	lcd_print_string(date_to_string(&current_date));
	lcd_print_char(' ');
	lcd_print_string(get_day(current_date.day));

}



