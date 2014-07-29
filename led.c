#include <pic.h>
#include "led.h"

extern unsigned int g_tick;

unsigned int led_pattern;

#define pat_on (led_pattern / 100)
#define pat_off (led_pattern % 100)

void led_update()
{
	static long timer;
	static bit voltage = 0;
	static unsigned int local_pattern;
	if (led_pattern != local_pattern)
	{
		local_pattern = led_pattern;
		timer = 0;
	}
	timer -= g_tick;	
	if (timer < 0)
	{
		if (voltage)	//µ±Ç°ÁÁ
			timer += (local_pattern % 100) * 10000l;	//cs
		else
			timer += (local_pattern / 100) * 10000l;
		voltage = ~voltage;
		LED_PORT = voltage;	
	}	
}	