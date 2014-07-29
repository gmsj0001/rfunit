
#define BAUD 9600
#define FOSC 16000000ul

extern void serial_command(char *s);	//由宿主实现
extern void putch(char c);
extern void putul(unsigned long val, char base);
extern void serial_update();

#define serial_init() \
	SYNC = 0; \
	BRGH = 1; \
	TXEN = 1; \
	SPBRG = FOSC / 16 / BAUD - 1; \
	SPEN = 1; \
	CREN = 1
	//RXDTSEL = TXCKSEL = 1; /*12F1823*/