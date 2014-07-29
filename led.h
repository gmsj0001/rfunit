
#define LED_TRIS TRISA4
#define LED_PORT RA4

extern void led_update();
extern unsigned int led_pattern;

#define PAT_IDLE	9999
#define PAT_BUSY	202
#define PAT_WAIT	9900

#define led_init() \
	LED_TRIS = 0; \
	led_pattern = PAT_IDLE

