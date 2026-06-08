#ifndef AAA_DIGITAL_TUBE_H_
#define AAA_DIGITAL_TUBE_H_

#include "../8208_dual_mouse/AAA_app_config_Telink_QFN32.h"

#if (MODULE_DIGITAL_TUBE_ENABLE)

#define TUBE_PAIR_FLICK_CYCLE	 20
#define TUBE_RECONN_FLICK_CYCLE	 80
#define TUBE_SUCC_FLICK_CYCLE	 10

#define A1_IN  0x01
#define B1_IN  0x02
#define C1_IN  0x04
#define D1_IN  0x08
#define E1_IN  0x10
#define F1_IN  0x20
#define G1_IN  0x40

#define A2_IN  0x01
#define B2_IN  0x02
#define C2_IN  0x04
#define D2_IN  0x08
#define E2_IN  0x10
#define F2_IN  0x20
#define G2_IN  0x40

#define A3_IN  0x01
#define B3_IN  0x02
#define C3_IN  0x04
#define D3_IN  0x08
#define E3_IN  0x10
#define F3_IN  0x20
#define G3_IN  0x40

#define A4_IN  0x01
#define B4_IN  0x02
#define C4_IN  0x04
#define D4_IN  0x08
#define E4_IN  0x10
#define F4_IN  0x20
#define G4_IN  0x40

#define NONE_IN  0x00

#define K_IN  0x01
#define H_IN  0x02
#define I_IN  0x04
#define J_IN  0x08
#define P_IN  0x10

#define J_CLEAN 0xF7

typedef enum {
	TUBE_DISP_BATTERY = 0,
	TUBE_DISP_BATT_LOWER,
	TUBE_DISP_DPI,
	TUBE_DISP_MAX,
}tube_disp_sta_e;

typedef enum {
	TUBE_LED_PAIR = 0x00,
	TUBE_LED_RECONN,
	TUBE_LED_SUCC,
	TUBE_LED_ALL_ON,
	TUBE_LED_MAX,
}tube_led_e;
	
typedef struct {
	unsigned char num1;
	unsigned char num2;
	unsigned char num3;
	unsigned char num4;
	unsigned char num5;
}tube_code_t;

typedef struct {
	unsigned char  counter;
	unsigned char  led_sta;
	tube_led_e     new_mode;
	tube_led_e     last_mode;
	unsigned char  on_flag;
}tube_mode_t;

extern tube_mode_t tube_sta;

void digital_tube_init(void);
void digital_tube_io_default(void);
void digital_tube_g24_status(tube_led_e mode);
void digital_tube_ble_status(tube_led_e mode);
void digital_tube_pro(void);

#endif

#endif /* AAA_DIGITAL_TUBE_H_*/
