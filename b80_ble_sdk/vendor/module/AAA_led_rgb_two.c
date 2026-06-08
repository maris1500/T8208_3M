#include "AAA_led_rgb_two.h"


#if LED_RGB_PWM_EN

#include "../8208_dual_mouse/AAA_app_config_Telink_QFN32.h"
#include "../8208_dual_mouse/AAA_public_Config.h"
#include "../../drivers/pwm.h"

const unsigned int rgb_group_tab[LED_RGB_GROUP_NUM] = PWM_RGB_TAB;

void led_rgb_set_pwm(unsigned short r, unsigned short g, unsigned short b)
{
	pwm_set_cycle_and_duty(PWM_RGB_R_ID, (unsigned short)(PWM_MAX_SCALE * CLOCK_SYS_CLOCK_1US),  (unsigned short)(r * CLOCK_SYS_CLOCK_1US) );
	pwm_set_cycle_and_duty(PWM_RGB_G_ID, (unsigned short)(PWM_MAX_SCALE * CLOCK_SYS_CLOCK_1US),  (unsigned short)(g * CLOCK_SYS_CLOCK_1US) );
	pwm_set_cycle_and_duty(PWM_RGB_B_ID, (unsigned short)(PWM_MAX_SCALE * CLOCK_SYS_CLOCK_1US),  (unsigned short)(b * CLOCK_SYS_CLOCK_1US) );
}

void led_rgb_stop(void)
{
	led_rgb_set_pwm(PWM_MAX_SCALE, PWM_MAX_SCALE, PWM_MAX_SCALE);

	pwm_stop(PWM_RGB_R_ID);
	pwm_stop(PWM_RGB_G_ID);
	pwm_stop(PWM_RGB_B_ID);

#if (PROJECT_ID == PID_DMS157)
	gpio_output_config_status(LED_RGB_DPI_PIN_R,  LED_RGB_DPI_OFF);
	gpio_output_config_status(LED_RGB_DPI_PIN_G,  LED_RGB_DPI_OFF);
	gpio_output_config_status(LED_RGB_DPI_PIN_B,  LED_RGB_DPI_OFF);
#endif

}


void led_rgb_start(void)
{
	rgb_led_pwm_init();
}

void led_rgb_set_color(LED_RGB_COLOR_EN color, unsigned short duty)
{
#if LED_RGB_AONODE_ENABLE

	switch( color )
	{
		case LED_RGB_CYAN:   led_rgb_set_pwm(PWM_MAX_SCALE, duty, duty);  			break;
		case LED_RGB_BLUE:   led_rgb_set_pwm(PWM_MAX_SCALE, PWM_MAX_SCALE, duty);   break;
		case LED_RGB_PURPLE: led_rgb_set_pwm(duty, PWM_MAX_SCALE, duty);  			break;
		case LED_RGB_RED:    led_rgb_set_pwm(duty, PWM_MAX_SCALE, PWM_MAX_SCALE);   break;
		case LED_GRB_ORANGE: led_rgb_set_pwm(duty, duty, PWM_MAX_SCALE);  			break;
		case LED_RGB_GREEN:  led_rgb_set_pwm(PWM_MAX_SCALE, duty, PWM_MAX_SCALE);   break;
		case LED_RGB_WHITE:  led_rgb_set_pwm(duty, duty, duty); 					break;
		default : break;
	}

#else

	switch( color )
	{
		case LED_RGB_CYAN:   led_rgb_set_pwm(0, duty, duty);  break;
		case LED_RGB_BLUE:   led_rgb_set_pwm(0, 0, duty);     break;
		case LED_RGB_PURPLE: led_rgb_set_pwm(duty, 0, duty);  break;
		case LED_RGB_RED:    led_rgb_set_pwm(duty, 0, 0);     break;
		case LED_GRB_ORANGE: led_rgb_set_pwm(duty, duty, 0);  break;
		case LED_RGB_GREEN:  led_rgb_set_pwm(0, duty, 0);     break;
		case LED_RGB_WHITE:  led_rgb_set_pwm(duty, duty, duty); break;
		default : break;
	}

#endif

}


void rgb_led_pwm_init(void)
{
	pwm_set_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);

	gpio_set_func(PWM_RGB_R_PIN, PWM_RGB_FUN_R);
	pwm_set_mode(PWM_RGB_R_ID, PWM_NORMAL_MODE);
	pwm_set_cycle_and_duty(PWM_RGB_R_ID, (unsigned short)(PWM_MAX_SCALE * CLOCK_SYS_CLOCK_1US),  (unsigned short)(PWM_INIT_VALUE * CLOCK_SYS_CLOCK_1US));

	gpio_set_func(PWM_RGB_G_PIN, PWM_RGB_FUN_G);
	pwm_set_mode(PWM_RGB_G_ID, PWM_NORMAL_MODE);
	pwm_set_cycle_and_duty(PWM_RGB_G_ID, (unsigned short)(PWM_MAX_SCALE * CLOCK_SYS_CLOCK_1US),  (unsigned short)(PWM_INIT_VALUE * CLOCK_SYS_CLOCK_1US));

	gpio_set_func(PWM_RGB_B_PIN, PWM_RGB_FUN_B);
	pwm_set_mode(PWM_RGB_B_ID, PWM_NORMAL_MODE);
	pwm_set_cycle_and_duty(PWM_RGB_B_ID, (unsigned short)(PWM_MAX_SCALE * CLOCK_SYS_CLOCK_1US),  (unsigned short)(PWM_INIT_VALUE * CLOCK_SYS_CLOCK_1US));

	pwm_start(PWM_RGB_R_ID);
	pwm_start(PWM_RGB_G_ID);
	pwm_start(PWM_RGB_B_ID);

}


#endif

