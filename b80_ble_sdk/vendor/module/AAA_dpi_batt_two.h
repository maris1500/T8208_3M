#ifndef AAA_DPI_BATT_TWO_H_
#define AAA_DPI_BATT_TWO_H_

#include "../8208_dual_mouse/AAA_app_config_Telink_QFN32.h"


#if BATT_DPI_DISP_TWO_ENABLE


typedef enum {
	VBAT_DPI_CHOOSE_VBAT = 0,
	VBAT_DPI_CHOOSE_DPI,
	VBAT_DPI_CHOOSE_MAX,
}vbat_dpi_type_en;

#define VBAT_DPI_NUM   VBAT_DPI_CHOOSE_MAX 

typedef struct {
	unsigned char status;
	unsigned char types;
	unsigned short ontime;
	unsigned short offtime;
	unsigned short repeats;
	unsigned int   ticks;
}led_action_t;

void vbat_dpi_led_two_insert(vbat_dpi_type_en type, unsigned char times);

#endif


#endif


