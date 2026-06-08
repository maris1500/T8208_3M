#include "code_led.h"

#if LED_CODE_ENABLE

#if MODULE_DIGITAL_TUBE_ENABLE
	extern void digital_tube_pro(void);
#endif

#define  CODE_DELAY_320NS	CLOCK_DLY_2_CYC
#define  CODE_DELAY_640NS	CLOCK_DLY_10_CYC

//#define  CODE_DELAY_250NS	 CLOCK_DLY_6_CYC
//#define  CODE_DELAY_850NS	 CLOCK_DLY_21_CYC
//#define  CODE_DELAY_400NS	 CLOCK_DLY_10_CYC
//#define  CODE_DELAY_1000NS   CLOCK_DLY_24_CYC

//	#define  CODE_DELAY_300NS	CLOCK_DLY_10_CYC
//	#define  CODE_DELAY_600NS	CLOCK_DLY_20_CYC
//	#define  CODE_DELAY_L_600NS	CLOCK_DLY_20_CYC

#if (PROJECT_ID == PID_HM668)
	#define M45_CODE_LED_FRE_11  0
#else
	#define M45_CODE_LED_FRE_11  1
#endif


#if FRE_RC_48MHZ_ENABLE
	#define  CODE_DELAY_300NS	CLOCK_DLY_12_CYC  // 14.4
	#define  CODE_DELAY_600NS	CLOCK_DLY_26_CYC  // 28.8
	#define  CODE_DELAY_790NS	CLOCK_DLY_36_CYC  // 37.92
	#define  CODE_DELAY_L_600NS	CLOCK_DLY_26_CYC  // 28.8
	#define  CODE_DELAY_900NS	CLOCK_DLY_41_CYC  // 43.2

	#define  CODE_DELAY_250NS	CLOCK_DLY_10_CYC    // 12
	#define  CODE_DELAY_850NS	CLOCK_DLY_38_CYC    // 40.8
	#define  CODE_DELAY_400NS	CLOCK_DLY_17_CYC    // 19.2
	#define  CODE_DELAY_L_1000NS	CLOCK_DLY_47_CYC //CLOCK_DLY_48_CYC

#else
	#define  CODE_DELAY_300NS	CLOCK_DLY_5_CYC
	#define  CODE_DELAY_600NS	CLOCK_DLY_12_CYC
	#define  CODE_DELAY_L_600NS	CLOCK_DLY_14_CYC

	#define  CODE_DELAY_250NS	CLOCK_DLY_6_CYC
	#define  CODE_DELAY_850NS	CLOCK_DLY_21_CYC
	#define  CODE_DELAY_400NS	CLOCK_DLY_10_CYC
	#define  CODE_DELAY_L_1000NS	CLOCK_DLY_24_CYC

#endif

static code_led_scan_state  sg_code_sacn_sta = {0};

const unsigned int sg_color_7_list[] = {

 LED_RED,
 LED_GREEN,
 LED_BLUE,
 LED_YELLOW,

 LED_CYAN,
 LED_PURPLE,
 LED_WHITE,

};

#define COLOR_LIST_NUMBER 7

static unsigned char sg_rgb_status = 0;
static unsigned int  sg_start_time = 0;

static unsigned char sg_breath_value = 0;

void colde_led_off_parameter_clear(void)
{
	sg_code_sacn_sta.index = 0;
	sg_code_sacn_sta.open  = 0;
	sg_code_sacn_sta.times = 0;
	sg_code_sacn_sta.position = 0;
	sg_code_sacn_sta.state = 0;

	sg_code_sacn_sta.power_off = 1;

	sg_breath_value = 0;
}

void coled_led_mode_change_para_clear(void)
{
	colde_led_off_parameter_clear();
	sg_code_sacn_sta.power_off = 0;
}


#if (PROJECT_ID == PID_0120)

	static rgb3810_indicate_sta_en gc_rgb_indicate_sta = RGB_CODE_STATUS_DPI_START;
	static rgb3810_charge_en sg_rgb_charge_state = RGB_CHARGE_NONE;
	static rgb3810_lvd_en sg_rgb_lvd_state = RGB_LVD_NONE;

	static unsigned int sg_rgb_code_start_time = 0;

	static unsigned char sg_rgb_code_lvd_flick_times = 0;
	static unsigned char sg_rgb_code_lvd_timeout = 0;

	extern unsigned char battery_low_flag_status(void);

#endif



void rgb3810_write_reset(void)
{
	CODE_LED_OUT_L;
#if (PROJECT_ID == PID_HM668)
	sleep_us(300);
#else
	sleep_us(150);
#endif
}

void rgb3810_write_pixel(unsigned int color)
{
	unsigned char i = 0;

	for ( i = 0; i < CODE_PIXEL_NUMBER; i++ )
	{
		if ( color & 0x00800000 )
		{
			CODE_LED_OUT_H;
		#if (PROJECT_ID == PID_0120)
			CODE_DELAY_850NS;
		#elif (PROJECT_ID == PID_HM668)

			CODE_DELAY_900NS;
			CODE_LED_OUT_L;
			CODE_DELAY_300NS;

		#elif (PROJECT_ID == PID_M45) || (PROJECT_ID == PID_S600)
			#if M45_CODE_LED_FRE_11
				CODE_DELAY_600NS;
				CODE_LED_OUT_L;
				CODE_DELAY_300NS;
			#else
				CODE_DELAY_850NS;
				CODE_LED_OUT_L;
				CODE_DELAY_400NS;
			#endif
		#endif
		}
		else
		{
			CODE_LED_OUT_H;
		#if (PROJECT_ID == PID_0120)
			CODE_DELAY_250NS;
		#elif (PROJECT_ID == PID_HM668)

			 CODE_DELAY_300NS;
			 CODE_LED_OUT_L;
			 CODE_DELAY_900NS;

		#elif (PROJECT_ID == PID_M45) || (PROJECT_ID == PID_S600)
			#if M45_CODE_LED_FRE_11
				CODE_DELAY_300NS;
				CODE_LED_OUT_L;
				CODE_DELAY_600NS;
			#else
				CODE_DELAY_250NS;
				CODE_LED_OUT_L;
				CODE_DELAY_L_1000NS;
			#endif
		#endif
		}

		#if (PROJECT_ID == PID_0120)
			CODE_LED_OUT_L;
			CODE_DELAY_1000NS;
		#elif (PROJECT_ID == PID_HM668)
			 // CODE_LED_OUT_L;
			 // CODE_DELAY_L_600NS;
		#elif (PROJECT_ID == PID_M45)
			//
		#endif

		 color <<= 1;
	}
}

void rgb3810_write_data(unsigned int color)
{
	unsigned int dat = 0x00;
	unsigned char i = 0, j = 0;

	if ( LED_BLACK == color )
	{
		sg_rgb_status = 0;
	}
	else
	{
		sg_rgb_status = 1;
	}

	for ( j = 0; j < CODE_LED_NUMBER; j++ )
	{
		dat = color;

		for ( i = 0; i < CODE_PIXEL_NUMBER; i++ )
		{
			if ( dat & 0x00800000 )
			{
				CODE_LED_OUT_H;
			#if (PROJECT_ID == PID_0120)
				CODE_DELAY_850NS;
			#elif (PROJECT_ID == PID_HM668)

				 CODE_DELAY_900NS;
				 CODE_LED_OUT_L;
				 CODE_DELAY_300NS;

			#elif (PROJECT_ID == PID_M45) || (PROJECT_ID == PID_S600)
				#if M45_CODE_LED_FRE_11
					CODE_DELAY_600NS;
					CODE_LED_OUT_L;
					CODE_DELAY_300NS;
				#else
					CODE_DELAY_850NS;
					CODE_LED_OUT_L;
					CODE_DELAY_400NS;
				#endif
			#endif
			}
			else
			{
				CODE_LED_OUT_H;
			#if (PROJECT_ID == PID_0120)
				CODE_DELAY_250NS;
			#elif (PROJECT_ID == PID_HM668)

				 CODE_DELAY_300NS;
				 CODE_LED_OUT_L;
				 CODE_DELAY_900NS;

			#elif (PROJECT_ID == PID_M45) || (PROJECT_ID == PID_S600)
				#if M45_CODE_LED_FRE_11
					CODE_DELAY_300NS;
					CODE_LED_OUT_L;
					CODE_DELAY_600NS;
				#else
					CODE_DELAY_250NS;
					CODE_LED_OUT_L;
					CODE_DELAY_L_1000NS;
				#endif
			#endif
			}

			#if (PROJECT_ID == PID_0120)
				CODE_LED_OUT_L;
				CODE_DELAY_1000NS;
			#elif (PROJECT_ID == PID_HM668)
				 //CODE_LED_OUT_L;
				 //CODE_DELAY_L_600NS;
			#elif (PROJECT_ID == PID_M45)
				//
			#endif

			dat <<= 1;

		}
	}

}

void rgb3810_led_stop_off(void)
{
	unsigned char i = 0;

	for ( i = 0; i < CODE_LED_ACT_NUM; i++ )
	{
	#if (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_M45)
		rgb3810_write_reset();
	#endif
		rgb3810_write_data( LED_BLACK );
		sleep_ms(CODE_LED_DELAY);
	}
}

void rgb3810_sleep_close(void)
{
	if ( sg_rgb_status )
	{
		rgb3810_led_stop_off();

	#if (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_M45) || (PROJECT_ID == PID_S600)
		LED_CODE_POWER_OFF();
		colde_led_off_parameter_clear();
	#endif
	}
}

void rgb3810_light_on_together(void)
{
	#define HOLD_TIMES_1  10

	 unsigned char  k = 0;

	 if ( sg_code_sacn_sta.power_off )
	 {
		 sg_start_time = clock_time();
		 return;
	 }

	if ( clock_time_exceed(sg_start_time, 200*1000) )
	{
		if ( 0 == sg_code_sacn_sta.open )
		{
			for ( k = 0; k < 3; k++)
			{

				rgb3810_write_data( sg_color_7_list[ sg_code_sacn_sta.index ] );

			#if MODULE_DIGITAL_TUBE_ENABLE
				 digital_tube_pro();
			#endif

				 rgb3810_write_reset();
			}

			CODE_LED_OUT_H;
		}

		if ( sg_code_sacn_sta.times > 5 )
		{
			sg_code_sacn_sta.open = 1;
		}

		if ( ++sg_code_sacn_sta.times >= HOLD_TIMES_1 )
		{
			sg_code_sacn_sta.times = 0;
			sg_code_sacn_sta.open = 0;

			if ( ++sg_code_sacn_sta.index >= COLOR_LIST_NUMBER )
			{
				sg_code_sacn_sta.index = 0;
			}
		}

		sg_start_time = clock_time();
	}
}

void rgb3810_light_on_init(void)
{
	unsigned char i = 0, k = 0;

	for ( k = 0; k < 5; k++)
	{

		for (i = 0; i < CODE_LED_NUMBER; i++)
		{
			rgb3810_write_pixel( LED_BLACK );
		}

		rgb3810_write_reset();
	}

	CODE_LED_OUT_H;
}

void rgb3810_light_on_push(void)
{
	#define HOLD_TIMES_2  3

	 unsigned char i = 0, k = 0;

	 if ( sg_code_sacn_sta.power_off )
	 {
		 sg_start_time = clock_time();
		 return;
	 }

	if ( clock_time_exceed(sg_start_time, 200*1000) )
	{
		if ( 0 == sg_code_sacn_sta.open )
		{
			for ( k = 0; k < 3; k++)
			{

				for (i = 0; i < CODE_LED_NUMBER; i++)
				{
					if (  i <= sg_code_sacn_sta.position )
					{
						rgb3810_write_pixel( sg_color_7_list[ sg_code_sacn_sta.index ] );
					}
					else
					{
						 rgb3810_write_pixel( LED_BLACK );
					}
				}

				sg_rgb_status = 1;

			#if MODULE_DIGITAL_TUBE_ENABLE
				 digital_tube_pro();
			#endif

				rgb3810_write_reset();
			}

			CODE_LED_OUT_H;

		}


		if ( sg_code_sacn_sta.times >= 2 )
		{
			sg_code_sacn_sta.open = 1;
		}

		if ( ++sg_code_sacn_sta.times >= HOLD_TIMES_2 )
		{
			sg_code_sacn_sta.times = 0;
			sg_code_sacn_sta.open = 0;

			if ( ++sg_code_sacn_sta.position >= CODE_LED_NUMBER )
			{
				sg_code_sacn_sta.position = 0;
				if ( ++sg_code_sacn_sta.index >= COLOR_LIST_NUMBER )
				{
					sg_code_sacn_sta.index = 0;
				}
			}
		}

		sg_start_time = clock_time();
	}
}

void rgb3810_light_on_push_back(void)
{
	#define HOLD_TIMES_3  3

	 unsigned char i = 0, k = 0;

	 if ( sg_code_sacn_sta.power_off )
	 {
		 sg_start_time = clock_time();
		 return;
	 }

	if ( clock_time_exceed(sg_start_time, 200*1000) )
	{
		for ( k = 0; k <= 5; k++)
		{

			for (i = 0; i < CODE_LED_NUMBER; i++)
			{
				if (  i <= sg_code_sacn_sta.position )
				{
					rgb3810_write_pixel( sg_color_7_list[ sg_code_sacn_sta.index ] );
				}
				else
				{
					 rgb3810_write_pixel( LED_BLACK );
				}
			}

			sg_rgb_status = 1;

			#if MODULE_DIGITAL_TUBE_ENABLE
				 digital_tube_pro();
			#endif

			rgb3810_write_reset();
		}

		CODE_LED_OUT_H;

		if ( ++sg_code_sacn_sta.times >= HOLD_TIMES_3 )
		{
				sg_code_sacn_sta.times = 0;

				if ( 0 == sg_code_sacn_sta.state )
				{
					if ( ++sg_code_sacn_sta.position >= (CODE_LED_NUMBER - 1) )
					{
						sg_code_sacn_sta.position = (CODE_LED_NUMBER-1);
						sg_code_sacn_sta.state = 1;
					}
				}
				else
				{

					if ( 0 == sg_code_sacn_sta.position )
					{
						if ( ++sg_code_sacn_sta.index >= COLOR_LIST_NUMBER )
						{
							sg_code_sacn_sta.index = 0;
						}
						sg_code_sacn_sta.state = 0;
					}

					if ( sg_code_sacn_sta.position )
					{
						sg_code_sacn_sta.position--;
					}
				}
		 }

		sg_start_time = clock_time();
	}
}

void rgb3810_light_on_single(void)
{
	#define HOLD_TIMES_4  5

	 unsigned char i = 0, k = 0;

	 if ( sg_code_sacn_sta.power_off )
	 {
		 sg_start_time = clock_time();
		 return;
	 }

	if ( clock_time_exceed(sg_start_time, 200*1000) )
	{

		if ( 0 == sg_code_sacn_sta.open )
		{
			sg_code_sacn_sta.open = 1;

			for ( k = 0; k < 5; k++)
			{

				for (i = 0; i < CODE_LED_NUMBER; i++)
				{
					if (  i == sg_code_sacn_sta.position )
					{
						rgb3810_write_pixel( sg_color_7_list[ sg_code_sacn_sta.index ] );
					}
					else
					{
						 rgb3810_write_pixel( LED_BLACK );
					}
				}

				sg_rgb_status = 1;

				#if MODULE_DIGITAL_TUBE_ENABLE
					 digital_tube_pro();
				#endif

				rgb3810_write_reset();

			}

			 CODE_LED_OUT_H;
		}


		if ( ++sg_code_sacn_sta.times >= HOLD_TIMES_4 )
		{
			sg_code_sacn_sta.times = 0;
			sg_code_sacn_sta.open = 0;

			if ( sg_code_sacn_sta.position >= (CODE_LED_NUMBER - 1) )
			{
				sg_code_sacn_sta.position = 0;
				if ( ++sg_code_sacn_sta.index >= COLOR_LIST_NUMBER )
				{
					sg_code_sacn_sta.index = 0;
				}
			}
			else
			{
				sg_code_sacn_sta.position ++;
			}
		 }

		sg_start_time = clock_time();
	}
}

void rgb3810_light_on_colorful(void)
{
	#define HOLD_TIMES_4  5

	 unsigned char i = 0, k = 0;

	 if ( sg_code_sacn_sta.power_off )
	 {
		 sg_start_time = clock_time();
		 return;
	 }

	if ( clock_time_exceed(sg_start_time, 100*1000) )
	{
		if ( 0 == sg_code_sacn_sta.open )
		{
			sg_code_sacn_sta.open = 1;

			for ( k = 0; k < 5; k++)
			{

				for (i = 0; i < CODE_LED_NUMBER; i++)
				{
					rgb3810_write_pixel( sg_color_7_list[ rand() % COLOR_LIST_NUMBER ] );
				}

				sg_rgb_status = 1;

				#if MODULE_DIGITAL_TUBE_ENABLE
					 digital_tube_pro();
				#endif

				rgb3810_write_reset();
			}

			CODE_LED_OUT_H;
		}


		if ( ++sg_code_sacn_sta.times >= HOLD_TIMES_4 )
		{
			sg_code_sacn_sta.times = 0;
			sg_code_sacn_sta.open = 0;
		 }

		sg_start_time = clock_time();
	}
}



void rgb3810_light_on_breath(void)
{
	 unsigned char i = 0, k = 0;
	 unsigned int  temp_cc = 0;

	 if ( sg_code_sacn_sta.power_off )
	 {
		 sg_start_time = clock_time();
		 return;
	 }

	if ( clock_time_exceed(sg_start_time, 10*1000) )
	{
		sg_start_time = clock_time();

		if ( 0 == sg_code_sacn_sta.open )
		{
			sg_breath_value++;

			if ( sg_breath_value >= 250 )
			{
				sg_code_sacn_sta.open = 1;
			}
		}
		else
		{
			sg_breath_value--;

			if ( sg_breath_value <= 0 )
			{
				sg_code_sacn_sta.open = 0;

				if ( ++sg_code_sacn_sta.index >= COLOR_LIST_NUMBER )
				{
					sg_code_sacn_sta.index = 0;
				}

				return;
			}
		}

		temp_cc = sg_breath_value;

		switch ( sg_code_sacn_sta.index )
		{
			case 0:  temp_cc <<= 8;  break;

			case 1:	 temp_cc <<= 16; break;

			case 2:	 break;

			case 3:	temp_cc <<= 8; 	temp_cc |= sg_breath_value;  temp_cc <<= 8; break;

			case 4: temp_cc <<= 16; temp_cc |= sg_breath_value;  break;

			case 5: temp_cc <<= 8; 	temp_cc |= sg_breath_value;  break;

			case 6: temp_cc <<= 8; 	temp_cc |= sg_breath_value;  temp_cc <<= 8;  temp_cc |= sg_breath_value;  break;

			default: rgb3810_light_on_colorful(); break;
		}


		for ( k = 0; k < 5; k++)
		{

			for (i = 0; i < CODE_LED_NUMBER; i++)
			{
				rgb3810_write_pixel( temp_cc );
			}

			sg_rgb_status = 1;

			#if MODULE_DIGITAL_TUBE_ENABLE
				 digital_tube_pro();
			#endif

			rgb3810_write_reset();
		}

		CODE_LED_OUT_H;
	}

}


#if (PROJECT_ID == PID_0120)

void rgb_code_dpi_set(rgb3810_indicate_sta_en sta, rgb3810_action_en action)
{
	unsigned char ii = 0;

	if ( RGB_CODE_ACT_SET == action )
	{
		gc_rgb_indicate_sta |= sta;
	}

	if ( RGB_CODE_STATUS_DPI_START == sta )
	{
		for ( ii = 0; ii < CODE_LED_ACT_NUM; ii++ )
		{
			if ( 0 == dpi_value )
			{
				rgb3810_write_data( LED_RED );
			}
			else if ( 1 == dpi_value )
			{
				rgb3810_write_data( LED_GREEN );
			}
			else if ( 2 == dpi_value )
			{
				rgb3810_write_data( LED_BLUE );
			}
			else if ( 3 == dpi_value )
			{
				rgb3810_write_data( LED_YELLOW );
			}
			else if ( 4 == dpi_value )
			{
				rgb3810_write_data( LED_CYAN );
			}
			sleep_ms(CODE_LED_DELAY);
		}
		
		if ( RGB_CODE_ACT_SET == action )
		{
			sg_rgb_code_start_time = clock_time();
		}
	}
}

void rgb3810_charge_ctrl(rgb3810_charge_en sta)
{
	unsigned char i = 0;
	
	if ( RGB_CHARGE_LED_ON == sta )
	{
		for ( i = 0; i < CODE_LED_ACT_NUM; i++ )
		{
			rgb3810_write_data( LED_RED );
			sleep_ms(CODE_LED_DELAY);
		}
	}
	else if ( RGB_CHARGE_LED_OFF == sta )
	{
		for ( i = 0; i < CODE_LED_ACT_NUM; i++ )
		{
			rgb3810_write_data( LED_BLACK );
			sleep_ms(CODE_LED_DELAY);
		}
	}
}

void rgb3810_lvd_ctrl(rgb3810_lvd_en sta)
{
	unsigned char i = 0;

	if ( RGB_LVD_LED_ON == sta )
	{
		for ( i = 0; i < CODE_LED_ACT_NUM; i++ )
		{
			rgb3810_write_data( LED_RED );
			sleep_ms(CODE_LED_DELAY);
		}
	}
	else if ( RGB_LVD_LED_OFF == sta )
	{
		for ( i = 0; i < CODE_LED_ACT_NUM; i++ )
		{
			rgb3810_write_data( LED_BLACK );
			sleep_ms(CODE_LED_DELAY);
		}
	}
}

void rgb3810_factory_ctrl(rgb3810_lvd_en sta)
{
	unsigned char i = 0;

	if ( RGB_LVD_LED_ON == sta )
	{
		for ( i = 0; i < CODE_LED_ACT_NUM; i++ )
		{
			rgb3810_write_data( LED_GREEN );
			sleep_ms(CODE_LED_DELAY);
		}
	}
	else if ( RGB_LVD_LED_OFF == sta )
	{
		for ( i = 0; i < CODE_LED_ACT_NUM; i++ )
		{
			rgb3810_write_data( LED_BLACK );
			sleep_ms(CODE_LED_DELAY);
		}
	}
}

void led_code_factory_reset(void)
{
	unsigned char i = 0;

	for ( i = 0; i < 5; i++ )
	{
		rgb3810_factory_ctrl( RGB_FCY_LED_ON );
		sleep_ms(150);

		rgb3810_factory_ctrl( RGB_FCY_LED_OFF );
		sleep_ms(150);

	#if (MODULE_32K_WATCHDOG_ENABLE)
		wd_32k_clear();
	#endif
	}
}

#endif


void rgb_code_loop_pro(void)
{

#if	FRE_RC_48MHZ_ENABLE
	if ( pm_is_MCU_deepRetentionWakeup() )
	{
		if ( 0 == mcu_sleep_status.frequency_48Hz_flag )
		{
			rc_48m_cal();
			mcu_sleep_status.frequency_48Hz_flag = 1;
		}
	}
#endif

#if (PM_SYS_LOW_POWER_ENABLE)
	if ( connect_ok )
	{
		if ( idle_count >= CODE_LED_ON_TIME_MAX + 2 )
		{
			mcu_sleep_status.enter_lower = 1;

		#if LED_CODE_ENABLE
			rgb3810_sleep_close();
		#endif

		#if	FRE_RC_48MHZ_ENABLE
			if ( mcu_sleep_status.frequency_48Hz_flag )
			{
				rc_24m_cal();
				mcu_sleep_status.frequency_48Hz_flag = 0;
			}
		#endif

		}
		else
		{
			#if (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_M45)  || (PROJECT_ID == PID_S600)
				if (  sg_code_sacn_sta.power_off )
				{
					LED_CODE_POWER_ON();
					sg_code_sacn_sta.power_off = 0;
				}
			#endif
		}

		if ( idle_count < CODE_LED_ON_TIME_MAX && mcu_sleep_status.enter_lower )
		{
			if ( ++mcu_sleep_status.resume_cnt >= 5 )
			{
				mcu_sleep_status.resume_cnt = 0;
				mcu_sleep_status.enter_lower = 0;
			}
		}
	}
#endif


#if (PROJECT_ID == PID_HM668)  || (PROJECT_ID == PID_M45) || (PROJECT_ID == PID_S600)
	if ( (connect_ok) && (!flash_dev_info.code_led_sw) )
	{
		switch ( flash_dev_info.code_led_mode )
		{
			case RGB_MODE_LIGHTS_NUION: 
				 rgb3810_light_on_together();
				 break;

			case RGB_MODE_LIGHTS_PUSH:		
				 rgb3810_light_on_push();
				 break;

			case RGB_MODE_LIGHTS_PUSH_BACK:
				 rgb3810_light_on_push_back();
				 break;

			case RGB_MODE_LIGHTS_SINGLE:	
				 rgb3810_light_on_single();
				 break;

			case RGB_MODE_LIGHTS_COLORFUL: 
				 rgb3810_light_on_colorful();
				 break;

			case RGB_MODE_LIGHTS_BREATH: 
				 rgb3810_light_on_breath();
				 break;

			default: rgb3810_light_on_colorful(); break;
		}
	}
#endif


#if (PROJECT_ID == PID_0120)

	if ( RGB_CODE_STATUS_DPI_START & gc_rgb_indicate_sta )
	{
		if ( clock_time_exceed(sg_rgb_code_start_time, 3*1000*1000) )
		{
			sg_rgb_code_start_time = clock_time();

			rgb3810_led_stop_off();

			gc_rgb_indicate_sta &= RGB_CODE_STATUS_DPI_START_STOP;

			printf("dpi over \n");
		}

		return;
	}

	if ( CHARGE_IN() )
	{
		if ( RGB_CHARGE_NONE == sg_rgb_charge_state )
		{
			sg_rgb_charge_state = RGB_CHARGE_LED_ON;
			rgb3810_charge_ctrl( RGB_CHARGE_LED_ON );
		}
		
		if ( clock_time_exceed(sg_rgb_code_start_time, 500*1000) )
		{
			sg_rgb_code_start_time = clock_time();

			sg_rgb_charge_state = ( sg_rgb_charge_state == RGB_CHARGE_LED_OFF ) ? RGB_CHARGE_LED_ON_CHANGE : RGB_CHARGE_LED_OFF_CHANGE;

		}

		if ( RGB_CHARGE_LED_ON_CHANGE == sg_rgb_charge_state )
		{
			sg_rgb_charge_state = RGB_CHARGE_LED_ON;
			rgb3810_charge_ctrl( RGB_CHARGE_LED_ON );
		}
		else if ( RGB_CHARGE_LED_OFF_CHANGE == sg_rgb_charge_state )
		{
			sg_rgb_charge_state = RGB_CHARGE_LED_OFF;
			rgb3810_charge_ctrl( RGB_CHARGE_LED_OFF );
		}

		return;
	}

	if ( CHARGE_OUT() )
	{
		if ( RGB_CHARGE_NONE != sg_rgb_charge_state )
		{
			sg_rgb_charge_state = RGB_CHARGE_NONE;
			rgb3810_charge_ctrl( RGB_CHARGE_LED_OFF );
			return;
		}
	}

	if ( battery_low_flag_status() )
	{
		if ( RGB_LVD_NONE == sg_rgb_lvd_state )
		{
			sg_rgb_lvd_state = RGB_LVD_LED_ON;
			rgb3810_lvd_ctrl( RGB_LVD_LED_ON );
		}
		
		if ( clock_time_exceed(sg_rgb_code_start_time, 1000*1000) )
		{
			sg_rgb_code_start_time = clock_time();
			sg_rgb_code_lvd_timeout++;
			sg_rgb_lvd_state = ( sg_rgb_lvd_state == RGB_LVD_LED_OFF ) ? RGB_LVD_LED_ON_CHANGE : RGB_LVD_LED_OFF_CHANGE;
		}

		if ( sg_rgb_code_lvd_flick_times < 10 )
		{
			if ( RGB_LVD_LED_ON_CHANGE == sg_rgb_lvd_state )
			{
				sg_rgb_lvd_state = RGB_LVD_LED_ON;
				rgb3810_lvd_ctrl( RGB_LVD_LED_ON );
				printf("lvd on \n");

			}
			else if ( RGB_LVD_LED_OFF_CHANGE == sg_rgb_lvd_state )
			{
				sg_rgb_lvd_state = RGB_LVD_LED_OFF;
				rgb3810_lvd_ctrl( RGB_LVD_LED_OFF );
				printf("lvd off \n");
				sg_rgb_code_lvd_flick_times++;
			}
		}

		 if ( sg_rgb_code_lvd_timeout >= 60 )
		 {
			 sg_rgb_code_lvd_flick_times = 0;
			 sg_rgb_code_lvd_timeout = 0;
			 sg_rgb_lvd_state = RGB_LVD_LED_OFF;
		 }

		 return;
	}
	else
	{
		if ( RGB_LVD_LED_ON == sg_rgb_lvd_state )
		{
			sg_rgb_lvd_state = RGB_LVD_NONE;
			sg_rgb_code_lvd_flick_times = 0;
			rgb3810_lvd_ctrl( RGB_LVD_LED_OFF );
		}
	}

	sg_rgb_code_start_time = clock_time();

#endif

}

#endif


