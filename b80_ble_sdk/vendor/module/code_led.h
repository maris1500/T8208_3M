#ifndef _CODE_LED_H_
#define _CODE_LED_H_

#include "../8208_dual_mouse/AAA_app_config_Telink_QFN32.h"

#if LED_CODE_ENABLE

#include "../../drivers/clock.h"
#include "../../drivers/timer.h"

#include "../8208_dual_mouse/AAA_public_Config.h"
#include "../8208_dual_mouse/main.h"

#define CODE_LED_ACTION_TIMES    10
#define CODE_LED_SCAN_TIMER  	 (200*1000)
#define CODE_LED_BREATH_TIMER  	 (30*1000)
#define CODE_LED_KEYPRESS_TIMER  (50*1000)

#if (PROJECT_ID == PID_0120)
	#define CODE_PIXEL_NUMBER  24
	#define CODE_LED_NUMBER    1
	#define CODE_LED_ACT_NUM   15
	#define CODE_LED_DELAY     1
#elif (PROJECT_ID == PID_HM668)
	#define CODE_PIXEL_NUMBER  24
	#define CODE_LED_NUMBER    5
	#define CODE_LED_ACT_NUM   10
	#define CODE_LED_DELAY     1
#elif (PROJECT_ID == PID_M45) || (PROJECT_ID == PID_S600)
	#define CODE_PIXEL_NUMBER  24
	#define CODE_LED_NUMBER    6
	#define CODE_LED_ACT_NUM   10
	#define CODE_LED_DELAY     1
#endif

#define COLOR_PIXE   0xFF
#define COLOR_NONE   0x00

#define LED_NONE   0x00000000
#define LED_GREEN  0x00FF0000
#define LED_RED    0x0000FF00
#define LED_BLUE   0x000000FF

#define LED_YELLOW  LED_GREEN|LED_RED
#define LED_CYAN    LED_GREEN|LED_BLUE
#define LED_PURPLE  LED_RED|LED_BLUE
#define LED_WHITE   LED_RED|LED_BLUE|LED_GREEN
#define LED_BLACK   LED_NONE

#define RGB_COMBIN(r, g, b) (g<<16)|(r<<8)|(b)
#define RGB_ADD(rr, gg, bb) {.r=rr, .g=gg, .b=bb}

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
}code_led_color_t;


typedef enum {
	RGB_POWER_HIGHEST = 0x00,
	RGB_POWER_HIGH,
	RGB_POWER_MIDDLE,
	RGB_POWER_LEVEL_MAX,
}rgb3810_brightness_en;

typedef struct {
	unsigned short open:1;
	unsigned short index:4;

	unsigned char position:3;

	unsigned short power_off:1;
	unsigned short state:1;

	unsigned short times:6;

}code_led_scan_state;

void rgb3810_write_data(unsigned int color);
void rgb3810_run_mode_set(void);
void rgb3810_whiteless_set(void);
void rgb3810_param_clen(void);

#endif

#endif

