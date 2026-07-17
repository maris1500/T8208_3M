#ifndef AAA_LED_RGB_H_
#define AAA_LED_RGB_H_

#include "AAA_app_config_Telink_QFN32.h"

#if LED_RGB_ENABLE || DPI_RGB_SET_ENABLE

typedef enum {
	LED_RGB_PRO_IDEL = 0x00,
	LED_RGB_PRO_RUNING,
	LED_RGB_PRO_OVER,
	LED_RGB_PRO_SUSPEND_OVER,
}LED_RGB_RUN_STATE_EN;

typedef enum {
	LED_RGB_TASK_NONE = 0x00,
	LED_RGB_TASK_SUSPEND,
	LED_RGB_TASK_BREATH,
	LED_RGB_TASK_FLOW_1,
	LED_RGB_TASK_FLOW_2,
	LED_RGB_TASK_FLOW_3,
	LED_RGB_TASK_FLOW_4,
	LED_RGB_TASK_FLOW_5,
	LED_RGB_TASK_MAX,
}LED_RGB_TASK_TYPE_EN;

typedef enum
{
	LED_RGB_RED = 0x00,
	LED_RGB_BLUE,
	LED_RGB_GREEN,
	LED_GRB_ORANGE,
	LED_RGB_CYAN,
	LED_RGB_PURPLE,

#if (PROJECT_ID == PID_Q15)
	// nothing
#else
	LED_RGB_WHITE,
#endif

	LED_REB_COLOR_MAX,
}LED_RGB_COLOR_EN;

void led_rgb_pro(void);
void led_rgb_stop(void);


#if DPI_RGB_SET_ENABLE
	void dpi_rgb_port_init(void);
#endif

#endif

#endif

