#include "led_rgb_special.h"


#if LED_RGB_PWM_DMS157_ENABLE
	#include "../8208_dual_mouse/AAA_app_config_Telink_QFN32.h"
	#include "../module/AAA_led_rgb_two.h"
	#include "../../drivers/pwm.h"
#endif

#if LED_RGB_PWM_DMS157_ENABLE

extern unsigned char dpi_value;


void dms157_led_rgb_breath(void)
{
	#define LED_MAX_BREATH_CYCLE  1
	#define LED_MAX_1_DUTY        200

	static unsigned int   rgb_breath_tick = 0;
	static unsigned short rgb_breath_duty = 1000;
	static unsigned char  rgb_breath_flag = 1, rgb_breath_color = LED_RGB_CYAN, rgb_breath_suspend = 0;

	if ( clock_time_exceed(rgb_breath_tick, 10*1000) )
	{
		rgb_breath_tick = clock_time();

		if ( rgb_breath_suspend )
		{
			if ( ++rgb_breath_suspend >= 60 )
			{
				rgb_breath_suspend = 0;
			}
			return;
		}

		if ( rgb_breath_flag )
		{
			if (rgb_breath_duty > 700)
			{
				rgb_breath_duty -= 2;
			}
			else
			{
				rgb_breath_duty -= 20;
			}

			if ( rgb_breath_duty <= MIN_SCAL_CNT )
			{
				rgb_breath_flag = 0;
				rgb_breath_duty = 0;
			}
		}
		else
		{
			rgb_breath_duty += 10;

			if( rgb_breath_duty >= PWM_MAX_SCALE )
			{
				rgb_breath_flag = 1;
				rgb_breath_duty = PWM_MAX_SCALE;
				rgb_breath_suspend = 1;

				if ( ++rgb_breath_color >= LED_REB_COLOR_MAX )
				{
					rgb_breath_color = LED_RGB_CYAN;
				}
			}
		}

		led_rgb_set_color(rgb_breath_color, rgb_breath_duty);
	}
}

void led_reg_dpi_display(void)
{
   if ( 0 == dpi_value )
   {
	   led_rgb_set_color(LED_RGB_GREEN, MIN_SCAL_CNT);
   }
   else if ( 1 == dpi_value )
   {
	   led_rgb_set_color(LED_RGB_BLUE, MIN_SCAL_CNT);
   }
   else if ( 2 == dpi_value )
   {
	   led_rgb_set_color(LED_RGB_PURPLE, MIN_SCAL_CNT);
   }
   else if ( 3 == dpi_value )
   {
	   led_rgb_set_color(LED_RGB_WHITE, MIN_SCAL_CNT);
   }
}


void dms157_rgb_pwm_loop(void)
{
	 dms157_led_rgb_breath();
}


#endif

