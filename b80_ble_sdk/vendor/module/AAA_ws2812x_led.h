#ifndef _AAA_WS2812X_LED_H_
#define _AAA_WS2812X_LED_H_

#include "../8208_dual_mouse/AAA_app_config_Telink_QFN32.h"

#if LED_CODE_PWIR_DRIVE_ENABLE

#include "../../drivers/clock.h"
#include "../../drivers/timer.h"

#include "../8208_dual_mouse/AAA_public_Config.h"
#include "../8208_dual_mouse/main.h"

#if (PROJECT_ID == PID_M45)
	#define WS2812_RGB_BIT_LEN_MAX  150
#elif (PROJECT_ID == PID_104)
	// do nothing
#else
	#define WS2812_RGB_BIT_LEN_MAX  128
#endif

typedef union {

	struct
	{
		unsigned int open:1;
		unsigned int index:4;
		unsigned int position:4;

		unsigned int power_off:1;
		unsigned int state:1;

		unsigned int times:7;

		unsigned int temp:14;
   }bits;

   unsigned short status_bytes;

}ws2812_led_scan_state;


typedef enum {
	COLOR_RED = 0,
	COLOR_GREEN,
	COLOR_BLUE,
	COLOR_YELLOW,

	COLOR_CYAN,
	COLOR_PURPLE,
	COLOR_WHITE,

	COLOR_MAX,
}ws2812_color_en;

void ws2812_write_data(unsigned int color);
void ws2812_run_mode_set(void);
void ws2812_whiteless_set(void);
void ws2812_param_clen(void);

#endif

#endif

