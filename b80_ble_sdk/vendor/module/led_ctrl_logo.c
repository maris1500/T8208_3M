#include "led_ctrl_logo.h"

#if (LED_LOGO_ACTION_ENABLE) || LED_COLOR_LIGHTS_ENABLE
	#include "../8208_dual_mouse/AAA_public_config.h"
	#include "../8208_dual_mouse/main.h"
#endif

#if LED_LOGO_ACTION_ENABLE || LED_COLOR_LIGHTS_ENABLE
	extern u8 connect_ok;
#endif

#if LED_COLOR_LIGHTS_ENABLE
	static led_color_act_en sg_led_color_sta = LED_COLOR_ACT_NONE;
	static unsigned char sg_color_logo_onoff = 0;
	extern unsigned char mouse_wheel_moved(void);
#endif

#if LED_LOGO_OFF_WHEN_MOUSE_MOVE
	extern unsigned char mouse_wheel_moved(void);
#endif

#if LED_LOGO_ACTION_ENABLE

	#if LED_LOGO_OFF_WHEN_MOUSE_MOVE
		static unsigned int sg_logo_move_time = 0;
		static led_logo_mouse_act_en sg_logo_move_sta = LED_LOGO_MOUSE_MOVE_NONE;
	#endif

void led_logo_action(void)
{
	if ( 0 == connect_ok || DEVICE_LED_BUSY ) 
	{
		LED_LOGO_ACT(LED_LOGO_OFF);
		return;
	}

#if LED_LOGO_OFF_WHEN_MOUSE_MOVE
	if ( mouse_wheel_moved() )
	{
		LED_LOGO_ACT(LED_LOGO_OFF);
		sg_logo_move_sta = LED_LOGO_MOUSE_MOVE;
		sg_logo_move_time = clock_time();
		return;
	}
#endif

#if LED_LOGO_OFF_WHEN_MOUSE_MOVE
	if ( LED_LOGO_MOUSE_MOVE == sg_logo_move_sta  )
	{
		if ( clock_time_exceed(sg_logo_move_time, LED_LOGO_OFF_MOUSE_MOVE_TIMEOUT) )
		{
			sg_logo_move_sta = LED_LOGO_MOUSE_MOVE_NONE;
			sg_logo_move_time = clock_time();
		}
		return;
	}
#endif

	if ( idle_count >= LED_LOGO_ACTION_MAX_TIME )
	{
		LED_LOGO_ACT(LED_LOGO_OFF);
		return;
	}

	LED_LOGO_ACT(LED_LOGO_ON);
}

#endif

#if LED_COLOR_LIGHTS_ENABLE

	static unsigned int sg_color_logo_time = 0;

	void led_color_logo_status_set(led_color_act_en sta)
	{
		if ( sta >= LED_COLOR_ACT_MAX )
		{
			return;
		}

		sg_led_color_sta = sta;
	}

	unsigned char led_color_logo_onoff_get(void)
	{
		return ( sg_color_logo_onoff );
	}

	void led_color_logo_action(void)
	{
		if ( 0 == connect_ok || DEVICE_LED_BUSY )
		{
			sg_color_logo_time = clock_time();
			return;
		}

		if (  led_batt_dpi_working_on() )
		{
			sg_color_logo_time = clock_time();
			sg_led_color_sta = LED_COLOR_DPI_BAT;
			return;
		}

		if ( mouse_wheel_moved() )
		{
			LED_COLOR_LOGO_ACT(LED_COLOR_LOGO_OFF);
			sg_led_color_sta = LED_COLOR_MOUSE_MOVE;
			sg_color_logo_onoff = LED_COLOR_LOGO_OFF;
			sg_color_logo_time = clock_time();
		}

		if ( (LED_COLOR_MOUSE_MOVE == sg_led_color_sta) && clock_time_exceed(sg_color_logo_time, 1*1000*1000) )
		{
			sg_led_color_sta = LED_COLOR_ACT_NONE;
		}

		if ( (LED_COLOR_DPI_BAT == sg_led_color_sta) && clock_time_exceed(sg_color_logo_time, 3*1000*1000) )
		{
			sg_led_color_sta = LED_COLOR_ACT_NONE;
		}

		if ( LED_COLOR_MOUSE_MOVE == sg_led_color_sta || LED_COLOR_DPI_BAT == sg_led_color_sta )
		{
			sg_color_logo_onoff = LED_COLOR_LOGO_OFF;
			return;
		}

		if ( idle_count > LED_COLOR_LOGO_ACTION_MAX_TIME )
		{
			LED_COLOR_LOGO_ACT(LED_COLOR_LOGO_OFF);
			sg_color_logo_onoff = LED_COLOR_LOGO_OFF;
			return;
		}

		LED_COLOR_LOGO_ACT(LED_COLOR_LOGO_ON);
		sg_color_logo_onoff = LED_COLOR_LOGO_ON;
		sg_color_logo_time = clock_time();
	}
#endif

