#include <pic.h>
#include "2262.h"
#include "led.h"

#define MIN_CLOCK 32	//R=1.2M 4a=125us a=31us
#define MAX_CLOCK 128	//R=4.7M 4A=455us a=114us
#define DEF_CLOCK 80

extern unsigned int g_tick;

unsigned long rf_in_value = 0;
unsigned long rf_out_value = 0;
char rf_out_timer = 0;	//ds
	
void rf_in_update()
{
	static long timer = 0;
	static bit voltage = 0;
	static signed char nbit = 24;	//sync bit
	static unsigned short long value;
	static char clock;
	
	timer += g_tick;
	if (RF_IN == voltage)
		return;
	
	if (voltage == 0)		//L->H
	{
		if (timer > MIN_CLOCK * 124 && timer < MAX_CLOCK * 124)	//got sync
		{
			clock = timer / 124;
			nbit = 23;
		}
		else if (nbit < 24)
		{
			if (timer > clock * 2 && timer < clock * 6)	//got 1
			{
				value |= 1ul << nbit;
				--nbit;
			}
			else if (timer > clock * 10 && timer < clock * 14)	//got 0
			{
				value &= ~(1ul << nbit);
				--nbit;
			}
			else
				nbit = 24;
			if (nbit == -1)		//completed
			{
				if (rf_in_value == 0)
					rf_in_value = clock * 0x1000000l + value;
			}					
		}
		else	//nothing
			rf_in_value = 0;		
	}	
	timer = 0;
	voltage = ~voltage;
}
	
void rf_out_update()
{
	static signed char nbit;
	static int timer;
	static bit voltage;
	static char clock;
	static long rf_timer = 0;

	if (rf_out_value == 0)
	{
		if (rf_timer > 0)
		{
			rf_timer = 0;
			led_pattern = PAT_IDLE;
		}
		nbit = 25;
		RF_OUT = 0;	//necessary
		return;
	}
	if (rf_timer == 0)
		led_pattern = PAT_BUSY;
	rf_timer += g_tick;
	if (rf_out_timer != 0xff && rf_timer > rf_out_timer * 100000l)
		rf_out_value = 0;
	if (nbit == 25)
	{
		clock = rf_out_value >> 24;
		if (clock == 0)
			clock = DEF_CLOCK;
		voltage = 0;
		timer = 0;
	}
	timer -= g_tick;
	if (timer < 0)
	{
		if (voltage == 1)
		{
			if (nbit == 24)
				timer += clock * 124;
			else if (rf_out_value & (1ul << nbit))
				timer += clock * 4;
			else
				timer += clock * 12;
		}
		else
		{
			--nbit;
			if (nbit == -1)
				nbit = 25;
			else if (nbit == 24 || !(rf_out_value & (1ul << nbit)))
				timer += clock * 4;
			else
				timer += clock * 12;
		}
		voltage = ~voltage;
		RF_OUT = voltage;		
	}														
}	