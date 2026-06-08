#ifndef AAA_LED_RGB_TWO_H_
#define AAA_LED_RGB_TWO_H_

#include "../8208_dual_mouse/AAA_app_config_Telink_QFN32.h"

#if LED_RGB_PWM_EN


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

void led_rgb_set_pwm(unsigned short r, unsigned short g, unsigned short b);
void led_rgb_stop(void);
void led_rgb_start(void);
void led_rgb_set_color(LED_RGB_COLOR_EN color, unsigned short duty);
void rgb_led_pwm_init(void);

#endif


#endif
