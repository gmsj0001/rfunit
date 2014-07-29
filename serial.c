#include <pic.h>
#include <conio.h>
#include "serial.h"

static char in_buff[16];
static char in_pos = 0;
static char out_buff[16];
static char out_pos = 0;

static void send_update()
{	
	while (TXIF && out_pos > 0)
	{
		TXREG = out_buff[0];
		for (char i = 0; i < out_pos - 1; ++i)
		{
			out_buff[i] = out_buff[i + 1];
		}
		--out_pos;	
	}
}
	
void putch(char c)
{
	while (out_pos >= sizeof(out_buff))
		send_update();
	if (c == '\n')
		out_buff[out_pos++] = '\r';
	out_buff[out_pos++] = c;
}

void putul(unsigned long val, char base)
{
	while (out_pos != 0)
		send_update();
	unsigned long v;
	char c;
	v = val;
	do
	{
		v /= base;
		++out_pos;
	} while (v != 0);
	char* buf = &out_buff[out_pos] - 1;
	do
	{
		c = val % base;
		val /= base;
		if (c >= 10)
			c += 'a' - '0' - 10;
		c += '0';
		*buf-- = c;
	} while (val != 0);
}	

void serial_update()
{
	while (RCIF)
	{
		char c = RCREG;
		if (c >= 0x20 && c <= 0x7e && in_pos < sizeof(in_buff) - 1)
		{
			putch(c);
			in_buff[in_pos++] = c;	
		}
		else if (c == '\r')
		{
			putch('\n');
			in_buff[in_pos] = '\0';
			in_pos = 0;
			serial_command(in_buff);
			cputs("> ");
		}
	}
	send_update();
}				