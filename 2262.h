#define RF_OUT RA2
#define RF_OUT_TRIS TRISA2
#define RF_IN RA5
#define RF_IN_TRIS	TRISA5

extern unsigned long rf_in_value;
extern unsigned long rf_out_value;
extern char rf_out_timer;

extern void rf_init();
extern void rf_out_update();
extern void rf_in_update();

#define rf_init() \
	RF_IN_TRIS = 1; \
	RF_OUT_TRIS = 0

