#include <pic.h>
#include <conio.h>
#include <ctype.h>
#include "util.h"
#include "2262.h"
#include "serial.h"
#include "led.h"

unsigned int g_tick;
static char read_idx;

void init()
{
	OSCCON = 0b01111000;	//16mhz
	ANSELA = 0;		//set RA digital
		
	TMR1ON = 1;
	TMR1CS0 = TMR1CS1 = 0;	//instruction clock
	T1CKPS0 = 0; T1CKPS1 = 1;	//1:4 prescale
	TMR1 = 0;
	
	led_init();
	serial_init();
	rf_init();
}

void save_rf(char index, unsigned long value)
{
	if (index < 1 || index > 32)
		return;	
	for (char i = 3; i != 0xff; --i)
	{
		eeprom_write((index - 1) * 4 + i, value & 0xff);
		value >>= 8;
	}	
}

unsigned long load_rf(char index)
{
	if (index < 1 || index > 32)
		return 0;
	unsigned long value = 0;
	for (char i = 0; i < 4; ++i)
	{
		value <<= 8;
		value += eeprom_read((index - 1) * 4 + i);
	}
	return value;
}	
	 	
void update()
{
	if (read_idx && rf_in_value)
	{
		save_rf(read_idx, rf_in_value);
		read_idx = 0;
		led_pattern = PAT_IDLE;
		cputs("\n");
		cputul(rf_in_value, 16);
		cputs(" ok\n");	
	}	
}
	
void main()
{
	init();
	while (1)
	{
		g_tick = TMR1;
		TMR1 = 0;
		rf_in_update();
		rf_out_update();
		serial_update();
		led_update();
		update();
	}	
}

void serial_command(char* s)
{
	char* token = gettok(s);
	unsigned long val;
	if (tolower(*token) == 'w' && !*++token)
	{
		token = gettok(NULL);
		if (!token)
			return;
		val = sgetul(token, 10);
		if (val >= 1 && val <= 32)
			val = load_rf(val);	
		else
		{
			val = sgetul(token, 16);
			if (val == -1l)
				return;
		}
		if (read_idx)
		{
			save_rf(read_idx, val);
			read_idx = 0;
			led_pattern = PAT_IDLE;
		}
		else
		{
			rf_out_value = val;
			token = gettok(NULL);
			if (token)	//time
				rf_out_timer = sgetul(token, 10);
			else
				rf_out_timer = 2;	//0.2s
		}
		cputs("ok\n");
	}
	else if (tolower(*token) == 'r' && !*++token)
	{
		if (read_idx)
		{
			read_idx = 0;
			led_pattern = PAT_IDLE;
			cputs("ok\n");
		}
		else
		{
			token = gettok(NULL);
			if (token)
			{
				val = sgetul(token, 10);
				if (val < 1 || val > 32)
					return;
				read_idx = val;
			}
			else
				read_idx = 0xff;
			led_pattern = PAT_WAIT;
			cputs("wait\n");
		}	
	}
}	