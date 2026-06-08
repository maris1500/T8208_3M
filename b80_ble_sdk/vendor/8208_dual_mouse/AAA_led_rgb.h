#ifndef AAA_LED_RGB_H_
#define AAA_LED_RGB_H_

#include "AAA_app_config_Telink_QFN32.h"

#if (LED_RGB_ENABLE)

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

typedef enum {
	LED_RGB_CYAN = 0x00,
	LED_RGB_BLUE,
	LED_RGB_PURPLE,
	LED_RGB_RED,
	LED_GRB_ORANGE,
	LED_RGB_GREEN,
	LED_RGB_WHITE,
	LED_REB_COLOR_MAX,
}LED_RGB_COLOR_EN;

#define PWM_INIT_DEFAULT    0
#define MIN_SCAL_CNT        0
#define MAX_SCAL_CNT        990
#define PWM_DEFAULT_DUTY    750
#define PWM_MAX_SCALE       1000

void led_rgb_num16_init(void);
void led_rgb_pro(void);
void led_rgb_stop(void);


#endif

#endif

