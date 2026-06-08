#ifndef _LED_CTRL_LOGO_H_
#define _LED_CTRL_LOGO_H_

#include "../8208_dual_mouse/AAA_app_config_Telink_QFN32.h"

#if LED_LOGO_OFF_WHEN_MOUSE_MOVE
	typedef enum {
		LED_LOGO_MOUSE_MOVE_NONE = 0x00,
		LED_LOGO_MOUSE_MOVE,
		LED_LOGO_MOUSE_MOVE_MAX,
	}led_logo_mouse_act_en;
#endif



#if LED_COLOR_LIGHTS_ENABLE

	typedef enum {
		LED_COLOR_ACT_NONE = 0x00,
		LED_COLOR_DPI_BAT,
		LED_COLOR_MOUSE_MOVE,
		LED_COLOR_ACT_MAX,
	}led_color_act_en;

#endif


#endif 

