#include <ctype.h>
#include <pic.h>

static char range(char c, char base)
{	
	if (isdigit(c))
		c -= '0';
	else
	{
		if (isupper(c))
			c = tolower(c);
		if (isalpha(c))
			c = c - 'a' + 10;
		else
			return 0xff;
	}
	if (c >= base)
		return 0xff;
	return c;	
}	

unsigned long sgetul(char *buf, char base)	//lite implement of strtol
{
	unsigned long a = 0;
	char c = *buf;
	while ((c = range(c, base)) != 0xff)
	{
		a = a * base + c;
		c = *++buf;
	}
	if (*buf != 0)	//fail when invaild char
		a = -1l;
	return a;		
}

void cputul(unsigned long val, char base)	//lite implement of itoa and printf
{
	extern void putul(unsigned long val, char base);
	putul(val, base);
}

char *gettok(char* s)	//lite implement of strtok
{
	static char *sp = 0;
	if (s == 0)
		s = sp;
	if (s == 0)
		return 0;
	while (*s && *s == ' ')		//trim space
		++s;
	if (!*s)
		return sp = 0;
	sp = s;
	while (*sp && *sp != ' ')
		++sp;
	if (*sp)
		*sp++ = 0;
	else
		sp = 0;
	return s;
}	
