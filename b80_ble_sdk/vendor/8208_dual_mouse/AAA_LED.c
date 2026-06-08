/********************************************************************************************************
 * @file     aaa_led.c
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/

#include "AAA_public_config.h"
#include "AAA_LED.h"
#include "main.h"

#if (MODULE_MCU_EXTERNAL_ENABLE)
	#include "../module/AAA_mcu_ext.h"
#endif

#if (MODULE_DIGITAL_TUBE_ENABLE)
	#include "../module/AAA_digital_tube.h"
#endif

#if Dongle_Lark_A1_Flash_Enable
    extern unsigned char gc_lark_flash_pair;
#endif

#if LED_RGB_DPI_ACTION_ENABLE
	extern unsigned char battery_low_flag_status(void);
#endif

#if LED_COLOR_LIGHTS_ENABLE
	extern unsigned char led_color_logo_onoff_get(void);
#endif

#if (0)
	extern void ws2812_red_all_on(void);
	extern void ws2812_green_all_on(void);
	extern void ws2812_cyan_all_on(void);
	extern void ws2812_all_off(void);
#endif

typedef enum
{
    BAT_LVD_LED_PRIORITY,
    DPI_LED_PRIORITY,
    RECONN_LED_PRIORITY,
    ADV_LED_PRIORITY,
    MODE_LED_PRIORITY,
    CONNECTED_LED_PRIORITY,
    POWER_OFF_LED_PRIORITY,
} LED_CFG_ENUM_AAA;

static led_flag_t led_status_flag;
static led_timeout_t led_tab[LED_INDEX_MAX];

const static led_cfg_t sg_led_pair      = {200,  200,  1, ADV_LED_PRIORITY};
const static led_cfg_t sg_led_reconn    = {1000, 1000, 1, RECONN_LED_PRIORITY};
const static led_cfg_t sg_led_conn_succ = {100,  100,  5, CONNECTED_LED_PRIORITY};

#if (PROJECT_ID == PID_2187)
	const static led_cfg_t sg_led_conn_over = {3000, 100,  1, CONNECTED_LED_PRIORITY};
#elif (PROJECT_ID == PID_104)
	const static led_cfg_t sg_led_conn_over = {2000, 1000,  1, CONNECTED_LED_PRIORITY};
#else
	const static led_cfg_t sg_led_conn_over = {2000, 100,  1, CONNECTED_LED_PRIORITY};
#endif

#if (PROJECT_ID == PID_104)
	LED_ADV_STA_EN led_adv_sta = LED_ADV_OFF;
#endif

void led_connect_none_status_clear(void)
{
	if ( 0 == connect_ok )
	{
		led_status_flag.led_all_on = 0;
	}
}

#if (PROJECT_ID == PID_MS2360)
void led_flick_times(unsigned char N_times, unsigned int Delyatime)
{
	unsigned char i = 0, k = 0;

#if LED_BAT_VOLTAGE_ENABLE
	gpio_higsesistance_config(PIN_BAT_DPI_LED);
#endif

	for ( i = 0; i < N_times; i++,k++ )
	{
	#if LED_BAT_VOLTAGE_ENABLE
		if ( 0 == (k % 2) )
		{
			gpio_output_config_status(PIN_BAT_DPI_LED, LED_OFF);
		}
		else
		{
			gpio_higsesistance_config(PIN_BAT_DPI_LED);
		}
	#endif

		sleep_ms(Delyatime);

	#if (MODULE_32K_WATCHDOG_ENABLE)
		wd_32k_clear(); //clear 32k watch dog
	#endif
	}
}
#endif

void led_g24_conn_status_set(void)
{
	led_status_flag.g24_conned = 1;

#if (MODULE_MCU_EXTERNAL_ENABLE)
	ext_mcu_io.conn = EXT_LED_CONN_SUCC;

	led_status_flag.led_conn_next = 0;
	led_status_flag.g24_conned = 0;
#endif

}

unsigned char led_batt_dpi_working_on(void)
{
	if ( led_tab[INDEX_BAT].repeat_times )
	{
		return (1);
	}
	
	return (0);
}

void led_g24_conned_succ_display(void)
{
	if ( (0 == connect_ok) || (0 == led_status_flag.g24_conned) )
	{
		return;
	}

	if ( DEVICE_LED_BUSY )
	{
		return;
	}

	led_status_flag.g24_conned = 0;
	led_status_flag.led_conn_next = 1;

#if LED_COLOR_LIGHTS_ENABLE
	LED_COLOR_LOGO_ACT(LED_COLOR_LOGO_ON);
	led_status_flag.led_conn_next = 0;
#elif (PROJECT_ID == PID_M0018) || (PROJECT_ID == PID_2187) || (PROJECT_ID == PID_104)
	led_status_flag.led_conn_next = 0;
	device_led_setup(sg_led_conn_over);
#elif (MODULE_MCU_EXTERNAL_ENABLE)
	ext_mcu_io.conn = EXT_LED_CONN_SUCC;
	led_status_flag.led_conn_next = 0;
#else
	device_led_setup( sg_led_conn_succ );
#endif

#if MODULE_DIGITAL_TUBE_ENABLE
	digital_tube_g24_status(TUBE_LED_SUCC);
#endif

#if (PROJECT_ID == PID_104)
	led_adv_sta = LED_ADV_SUCC;
#endif

}

void led_g24_conned_succ_dis_next(void)
{
	if ( (0 == connect_ok) || (0 == led_status_flag.led_conn_next) )
	{
		return;
	}

	if ( DEVICE_LED_BUSY ) {
		return;
	}

	led_status_flag.led_conn_next = 0;

#if LED_COLOR_LIGHTS_ENABLE
	LED_COLOR_LOGO_ACT(LED_COLOR_LOGO_ON);
#elif (MODULE_MCU_EXTERNAL_ENABLE)
	ext_mcu_io.conn = EXT_LED_CONN_SUCC;
	led_status_flag.led_conn_next = 0;
#elif (PROJECT_ID == PID_M0018) || (PROJECT_ID == PID_2187) || (PROJECT_ID == PID_104)
	// do nothing
#else
	device_led_setup(sg_led_conn_over);
#endif

#if MODULE_DIGITAL_TUBE_ENABLE
	digital_tube_g24_status(TUBE_LED_ALL_ON);
#endif
}

void led_ble_conned_succ_dis_next(void)
{
	if ( (0 == connect_ok) || (0 == led_status_flag.led_conn_next) )
	{
		return;
	}

	if ( DEVICE_LED_BUSY )
	{
		return;
	}

	led_status_flag.led_conn_next = 0;

#if LED_COLOR_LIGHTS_ENABLE
	LED_COLOR_LOGO_ACT(LED_COLOR_LOGO_ON);
#elif (MODULE_MCU_EXTERNAL_ENABLE)
	ext_mcu_io.conn = EXT_LED_CONN_SUCC;
	led_status_flag.led_conn_next = 0;
#elif (PROJECT_ID == PID_M0018) || (PROJECT_ID == PID_2187) || (PROJECT_ID == PID_104)
	// device_led_setup(sg_led_conn_over);
#else
	device_led_setup(sg_led_conn_over);
#endif

#if MODULE_DIGITAL_TUBE_ENABLE
	digital_tube_ble_status(TUBE_LED_ALL_ON);
#endif

}


void led_ble_Adv_poll()
{
    if ( pair_flag && (connect_ok == 0) )
    {
	#if MODULE_DIGITAL_TUBE_ENABLE
		digital_tube_ble_status(TUBE_LED_PAIR);
	#endif

	#if MODULE_MCU_EXTERNAL_ENABLE
		ext_mcu_io.conn = EXT_LED_PAIR;
	#else
        device_led_setup(sg_led_pair);
	#endif
    }
    else
    {
    #if MODULE_DIGITAL_TUBE_ENABLE
		digital_tube_ble_status(TUBE_LED_RECONN);
	#endif

	#if (PROJECT_ID == PID_M0018) || (PROJECT_ID == PID_2187)
		if ( 0 == led_status_flag.led_conn_next && 0 == connect_ok )
		{
			 device_led_setup(sg_led_reconn);
		}
	#elif MODULE_MCU_EXTERNAL_ENABLE
		ext_mcu_io.conn = EXT_LED_RECONN;
	#else
        device_led_setup(sg_led_reconn);
	#endif
    }

#if (PROJECT_ID == PID_104)
	led_adv_sta = LED_ADV_ON;
#endif

}


void led_2p4_Adv_poll()
{
#if Dongle_Lark_A1_Flash_Enable
	if ( pair_flag || gc_lark_flash_pair )
#else
    if (pair_flag)
#endif
    {
    #if MODULE_DIGITAL_TUBE_ENABLE
    	digital_tube_g24_status(TUBE_LED_PAIR);
	#endif

	#if MODULE_MCU_EXTERNAL_ENABLE
    	ext_mcu_io.conn = EXT_LED_PAIR;
	#else
        device_led_setup(sg_led_pair);
	#endif
    }
    else
    {
    #if MODULE_DIGITAL_TUBE_ENABLE
    	digital_tube_g24_status(TUBE_LED_RECONN);
	#endif

	#if MODULE_MCU_EXTERNAL_ENABLE
    	ext_mcu_io.conn = EXT_LED_RECONN;
	#else
        device_led_setup(sg_led_reconn);
	#endif
    }

#if (PROJECT_ID == PID_104)
	led_adv_sta = LED_ADV_ON;
#endif
}


void led_ble_ConnectedStatus()
{
#if (PROJECT_ID == PID_M0018) || (PROJECT_ID == PID_2187)
	if ( led_status_flag.led_all_on ) return;
#endif

	device_led_struct_clear();

#if LED_COLOR_LIGHTS_ENABLE
	LED_COLOR_LOGO_ACT(LED_COLOR_LOGO_ON);
	led_status_flag.led_conn_next = 0;
#elif (PROJECT_ID == PID_M0018) || (PROJECT_ID == PID_2187)
	if ( 0 == led_status_flag.led_conn_next )
	{
		device_led_setup(sg_led_conn_over);
		led_status_flag.led_all_on = 1;
		led_status_flag.led_conn_next = 1;
	}
#elif (MODULE_MCU_EXTERNAL_ENABLE)
	ext_mcu_io.conn = EXT_LED_CONN_SUCC;
	led_status_flag.led_conn_next = 0;
#elif (PROJECT_ID == PID_104)
	device_led_setup(sg_led_conn_over);
#else
    device_led_setup(sg_led_conn_succ);
    led_status_flag.led_conn_next = 1;
#endif

#if MODULE_DIGITAL_TUBE_ENABLE
	digital_tube_ble_status(TUBE_LED_SUCC);
#endif

#if (PROJECT_ID == PID_104)
	led_adv_sta = LED_ADV_SUCC;
#endif
}

#if LED_RGB_DPI_ACTION_ENABLE
	void led_rgb_dpi_onoff(unsigned char sta)
	{
		if ( sta == 1 )
		{
		#if (PROJECT_ID == PID_601) || (PROJECT_ID == PID_XT27)
			if ( battery_low_flag_status() )
			{
				LED_DPI_R_ACT(LED_RGB_DPI_ON);
				LED_DPI_G_ACT(LED_RGB_DPI_OFF);
				LED_DPI_B_ACT(LED_RGB_DPI_OFF);
			}
			else
			{
				if ( dpi_value == 0 )
				{
					LED_DPI_R_ACT(LED_RGB_DPI_ON);
					LED_DPI_G_ACT(LED_RGB_DPI_OFF);
					LED_DPI_B_ACT(LED_RGB_DPI_OFF);
				}
				else if ( dpi_value == 1 )
				{
					LED_DPI_R_ACT(LED_RGB_DPI_OFF);
					LED_DPI_G_ACT(LED_RGB_DPI_ON);
					LED_DPI_B_ACT(LED_RGB_DPI_OFF);
				}
				else if ( dpi_value == 2 )
				{
					LED_DPI_R_ACT(LED_RGB_DPI_OFF);
					LED_DPI_G_ACT(LED_RGB_DPI_OFF);
					LED_DPI_B_ACT(LED_RGB_DPI_ON);
				}
				else if ( dpi_value == 3 )
				{
				#if (PROJECT_ID == PID_XT27)
					LED_DPI_R_ACT(LED_RGB_DPI_ON);
					LED_DPI_G_ACT(LED_RGB_DPI_ON);
					LED_DPI_B_ACT(LED_RGB_DPI_OFF);
				#else
					LED_DPI_R_ACT(LED_RGB_DPI_ON);
					LED_DPI_G_ACT(LED_RGB_DPI_OFF);
					LED_DPI_B_ACT(LED_RGB_DPI_ON);
				#endif
				}
				else if ( dpi_value == 4 )
				{
					LED_DPI_R_ACT(LED_RGB_DPI_ON);
					LED_DPI_G_ACT(LED_RGB_DPI_ON);
					LED_DPI_B_ACT(LED_RGB_DPI_ON);
				}
			}
		#else
			if ( battery_low_flag_status() )
			{
				LED_DPI_R_ACT(LED_RGB_DPI_ON);
				LED_DPI_G_ACT(LED_RGB_DPI_OFF);
				LED_DPI_B_ACT(LED_RGB_DPI_OFF);
			}
			else
			{
				if ( dpi_value == 0 )
				{
					LED_DPI_R_ACT(LED_RGB_DPI_ON);
					LED_DPI_G_ACT(LED_RGB_DPI_ON);
					LED_DPI_B_ACT(LED_RGB_DPI_ON);
				}
				else if ( dpi_value == 1 )
				{
					LED_DPI_R_ACT(LED_RGB_DPI_OFF);
					LED_DPI_G_ACT(LED_RGB_DPI_ON);
					LED_DPI_B_ACT(LED_RGB_DPI_OFF);
				}
				else if ( dpi_value == 2 )
				{
					LED_DPI_R_ACT(LED_RGB_DPI_OFF);
					LED_DPI_G_ACT(LED_RGB_DPI_OFF);
					LED_DPI_B_ACT(LED_RGB_DPI_ON);
				}
				else if ( dpi_value == 3 )
				{
					LED_DPI_R_ACT(LED_RGB_DPI_ON);
					LED_DPI_G_ACT(LED_RGB_DPI_ON);
					LED_DPI_B_ACT(LED_RGB_DPI_OFF);
				}
			}
		#endif
		}
		else
		{
			LED_DPI_R_ACT(LED_RGB_DPI_OFF);
			LED_DPI_G_ACT(LED_RGB_DPI_OFF);
			LED_DPI_B_ACT(LED_RGB_DPI_OFF);
		}
	}
#endif

void vbat_led_on(u8 sta)
{
#if (PROJECT_ID == PID_MS13)
	if ( sta == 1 )
	{
		if ( LED_DIFF_BATTERY_LOW == sc_bat_dpi_flag || LED_DIFF_DPI_DEFAULT == sc_bat_dpi_flag )
		{
			gpio_output_config_status(PIN_BAT_DPI_LED, LED_BAT_DIFF_ON);
		}
		else
		{
			gpio_output_config_status(PIN_BAT_DPI_LED, LED_DPI_DIFF_ON);
		}
	}
	else
	{
		gpio_higsesistance_config(PIN_BAT_DPI_LED);
	}
#elif (PROJECT_ID == PID_M0018)
	if ( sta == 1 )
	{
		if ( LED_DIFF_BATTERY_LOW == sc_bat_dpi_flag )
		{
			gpio_output_config_status(PIN_BAT_DPI_LED, LED_BAT_DIFF_ON);
		}
		else if ( LED_DIFF_DPI_DISPLAY == sc_bat_dpi_flag )
		{
			if ( RF_2M_2P4G_MODE == mcu_mode_get() )
			{
				gpio_output_config_status(PIN_BAT_DPI_LED, LED_BAT_DIFF_ON);
			}
			else
			{
				gpio_output_config_status(PIN_BAT_DPI_LED, LED_DPI_DIFF_ON);
			}
		}
	}
	else
	{
		if ( LED_DIFF_BATTERY_LOW == sc_bat_dpi_flag )
		{
			gpio_output_config_status(PIN_BAT_DPI_LED, LED_DPI_DIFF_ON);;
		}
		else
		{
			gpio_higsesistance_config(PIN_BAT_DPI_LED);
		}
	}
#elif LED_DPI_BAT_SINGLE_CTRL_ENABLE
	if ( sta == 1 )
	{
	#if (PROJECT_ID == PID_NR300)
		gpio_write( PIN_BAT_DPI_LED, LED_OFF );
	#else
		 gpio_write( PIN_BAT_DPI_LED, LED_ON );
	#endif
	}
	else
	{
	#if (PROJECT_ID == PID_NR300)
		gpio_write( PIN_BAT_DPI_LED, LED_ON );
	#else
		gpio_write( PIN_BAT_DPI_LED, LED_OFF );
	#endif
	}
#else

	if ( sta == 1 )
	{
	#if (LED_DPI_HIGH_ENABLE)
		gpio_output_config_status(PIN_BAT_DPI_LED, LED_ON);
	#elif LED_RGB_BAT_DPI_ENABLE
		#if (PROJECT_ID == PID_XT27)
			led_rgb_dpi_onoff(LED_ON);
		#else
			LED_DPI_BAT_R_ACT( LED_RGB_DPI_BAT_ON );
			LED_DPI_BAT_B_ACT( LED_RGB_DPI_BAT_OFF );
		#endif
	#elif LED_RGB_DPI_ACTION_ENABLE
		led_rgb_dpi_onoff(LED_ON);
	#elif LED_DPI_Independence_ENABLE
		gpio_output_config_status(PIN_BAT_DPI_LED, LED_ON);
	#elif (PROJECT_ID == PID_660) || (PROJECT_ID == PID_4027) || (PROJECT_ID == PID_0120) || (PROJECT_ID == PID_HM668)
		// do nothing
	#elif (PROJECT_ID == PID_4027_DRY)

		#if (M4027_DRY_H2_ENABLE)
			if ( RF_2M_2P4G_MODE == fun_mode )
			{
				gpio_output_config_status(PIN_BAT_DPI_LED, LED_ON);
			}
			else
			{
				gpio_output_config_status(PIN_BAT_DPI_LED, LED_OFF);
			}
		#else
			gpio_output_config_status(PIN_BAT_DPI_LED, LED_ON);
		#endif
	#elif (PROJECT_ID == PID_M45)
		gpio_write(PIN_BAT_DPI_LED, LED_ON);
	#elif (PROJECT_ID == PID_DMS157) || (PROJECT_ID == PID_RM12)
		// do nothing
	#elif (PROJECT_ID == PID_104)
		// ws2812_red_all_on();
	#else
		gpio_output_config_status(PIN_BAT_DPI_LED, LED_OFF);
	#endif
	}
	else
	{
	#if LED_RGB_DPI_ACTION_ENABLE
		led_rgb_dpi_onoff(LED_OFF);
	#elif LED_RGB_BAT_DPI_ENABLE
		LED_DPI_BAT_R_ACT( LED_RGB_DPI_BAT_OFF );
		LED_DPI_BAT_B_ACT( LED_RGB_DPI_BAT_OFF );
	#elif LED_DPI_Independence_ENABLE
		gpio_output_config_status(PIN_BAT_DPI_LED, LED_OFF);
	#elif (PROJECT_ID == PID_660) || (PROJECT_ID == PID_4027) || (PROJECT_ID == PID_0120) || (PROJECT_ID == PID_HM668)
		// do nothing
	#elif (PROJECT_ID == PID_4027_DRY)

		#if (M4027_DRY_H2_ENABLE)
			gpio_higsesistance_config(PIN_BAT_DPI_LED);
		#else
			gpio_higsesistance_config(PIN_BAT_DPI_LED);
		#endif
	#elif (PROJECT_ID == PID_M45) || (PROJECT_ID == PID_535)
		gpio_write(PIN_BAT_DPI_LED, LED_OFF);
	#elif (PROJECT_ID == PID_DMS157) || (PROJECT_ID == PID_RM12)
		// do nothing
	#elif(PROJECT_ID == PID_104)
		// ws2812_all_off();
	#else
		gpio_higsesistance_config(PIN_BAT_DPI_LED);
	#endif
	}

#endif

	led_status_flag.bat_on = sta;
}

void vbat_dpi_struct_clean(void)
{
	led_tab[INDEX_BAT].on_time   = 0;
	led_tab[INDEX_BAT].off_time  = 0;
	led_tab[INDEX_BAT].repeat_times = 0;
}

void vbat_led_off(void)
{
	led_tab[INDEX_BAT].on_time   = 0;
	led_tab[INDEX_BAT].off_time  = 0;
	led_tab[INDEX_BAT].repeat_times = 0;

	vbat_led_on( LED_OFF );
}

int vbat_led_show()
{
	if ( led_tab[INDEX_BAT].repeat_times )
	{
		return 0;
	}

	led_tab[INDEX_BAT].on_time   = 500;
	led_tab[INDEX_BAT].off_time  = 500;

#if	LED_COLOR_LIGHTS_ENABLE
	if ( led_color_logo_onoff_get() )
	{
		led_tab[INDEX_BAT].repeat_times = 4;
	}
	else
	{
		led_tab[INDEX_BAT].repeat_times = 3;
	}
#elif (PROJECT_ID == PID_104)
	led_tab[INDEX_BAT].repeat_times = 4;
#else
	led_tab[INDEX_BAT].repeat_times = 3;
#endif

	led_tab[INDEX_BAT].start_tick   = clock_time();

#if (PROJECT_ID == PID_104)
	vbat_led_on( LED_OFF );
#else
	vbat_led_on( LED_ON );
#endif

#if (PROJECT_ID == PID_104)
	led_adv_sta = LED_ADV_VBAT_DPI;
#endif

	return 1;
}

int dpi_led_show(unsigned char times)
{
	led_tab[INDEX_BAT].on_time   = 500;
	led_tab[INDEX_BAT].off_time  = 500;

#if LED_RGB_DPI_ACTION_ENABLE
	#if (PROJECT_ID == PID_DMS06)
		led_tab[INDEX_BAT].repeat_times = times;
	#else
		led_tab[INDEX_BAT].repeat_times = 3;
	#endif
#elif LED_COLOR_LIGHTS_ENABLE
		if ( led_color_logo_onoff_get() )
		{
			led_tab[INDEX_BAT].repeat_times = times + 1;
		}
		else
		{
			led_tab[INDEX_BAT].repeat_times = times;
		}
#elif (PROJECT_ID == PID_104)
	led_tab[INDEX_BAT].repeat_times = times + 1;
#else
	led_tab[INDEX_BAT].repeat_times = times;
#endif

	led_tab[INDEX_BAT].start_tick   = clock_time();

#if (PROJECT_ID == PID_104)
	vbat_led_on( LED_OFF );
#else
	vbat_led_on( LED_ON );
#endif

#if (PROJECT_ID == PID_104)
	led_adv_sta = LED_ADV_VBAT_DPI;
#endif

	return 1;
}

void vbat_led_proc()
{
#if (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
	return;
#endif

	led_timeout_t *led = &led_tab[INDEX_BAT];

	if ( LED_ON == led_status_flag.bat_on )
	{
		if ( clock_time_exceed(led->start_tick, led->on_time*1000) )
		{
			vbat_led_on( LED_OFF );

			if ( led->off_time )
			{
				led->start_tick += led->on_time*SYSTEM_TIMER_TICK_1MS;
			}
		}
	}
	else
	{
		if ( clock_time_exceed(led->start_tick, led->off_time*1000) )
		{
			if ( --led->repeat_times )
			{
				vbat_led_on( LED_ON );
				led->start_tick += led->off_time*SYSTEM_TIMER_TICK_1MS;
			}
		}
	}

	#if LED_DPI_VBAT_DIFF_ENABLE
		if ( 0 == led->repeat_times )
		{
			sc_bat_dpi_flag = LED_DIFF_NONE;
		#if (PROJECT_ID == PID_M0018)
			gpio_higsesistance_config(PIN_BAT_DPI_LED);
		#endif
		}
	#endif

#if (PROJECT_ID == PID_104)
	if ( 0 == led->repeat_times )
	{
		if ( LED_ADV_VBAT_DPI == led_adv_sta )
		{
			led_adv_sta = LED_ADV_OFF;
		}
	}
#endif

}

#if (PROJECT_ID == PID_0120)
	void led_adv_all_off(void)
	{
		gpio_write( PIN_24G_LED, LED_OFF );
		gpio_write( PIN_BLE1_LED, LED_OFF );
		gpio_write( PIN_BLE2_LED, LED_OFF );
	}
#endif

#if BATT_LOWER_METHOD_22_ENABLE

	static unsigned char sg_led_battery_sta = LED_OFF;

	void vbat_lower_22_led_on(unsigned char sta)
	{
		if ( sta == 1 )
		{
			gpio_write( PIN_BAT_DPI_LED, LED_ON );
		}
		else
		{
			gpio_write( PIN_BAT_DPI_LED, LED_OFF );
		}

		sg_led_battery_sta = sta;
	}

	int vbat_lower_22_led_show(void)
	{
		if ( led_tab[INDEX_BATTERY_LOWER].repeat_times )
		{
			return 0;
		}

		led_tab[INDEX_BATTERY_LOWER].on_time   = 150;
		led_tab[INDEX_BATTERY_LOWER].off_time  = 1000;

		led_tab[INDEX_BATTERY_LOWER].repeat_times = 10;

		led_tab[INDEX_BATTERY_LOWER].start_tick   = clock_time();

		vbat_lower_22_led_on(LED_ON);

		return 1;
	}

	void vbat_lower_22_led_proc(void)
	{
		led_timeout_t *led = &led_tab[INDEX_BATTERY_LOWER];

		if ( LED_ON == sg_led_battery_sta )
		{
			if ( clock_time_exceed(led->start_tick, led->on_time*1000) )
			{
				vbat_lower_22_led_on( LED_OFF );

				if ( led->off_time )
				{
					led->start_tick += led->on_time*SYSTEM_TIMER_TICK_1MS;
				}
			}
		}
		else
		{
			if ( clock_time_exceed(led->start_tick, led->off_time*1000) )
			{
				if ( --led->repeat_times )
				{
					vbat_lower_22_led_on( LED_ON );
					led->start_tick += led->off_time*SYSTEM_TIMER_TICK_1MS;
				}
			}
		}
	}

	void vbat_lower_loop_proc(void)
	{
		if ( 0 == led_tab[INDEX_BATTERY_LOWER].repeat_times )
		{
			return ;
		}

		if ( (0 == connect_ok) || (idle_count > 10) )
		{
			 led_tab[INDEX_BATTERY_LOWER].repeat_times = 0;
			 vbat_lower_22_led_on( LED_OFF );
		}

		vbat_lower_22_led_proc();
	}

#endif

void led_hw_init()
{
	if (fun_mode == RF_1M_BLE_MODE)
	{
		if( flash_dev_info.mast_id == BLE_DEVICE_ID_0 )
		{
		#if (LED_BT1_ENABLE)
			#if (LED_BLE1_INIT_HIGH_ENABLE)
				device_led_init(PIN_BLE1_LED, LED_ON);
			#else
				device_led_init(PIN_BLE1_LED, LED_OFF);
			#endif
		#endif
		}
		else
		{
		#if (LED_BT2_ENABLE)
			#if LED_BLE2_INIT_HIGH_ENABLE
				device_led_init(PIN_BLE2_LED, LED_ON);
			#else
				device_led_init(PIN_BLE2_LED, LED_OFF);
			#endif
		#endif
		}
	}
	else
	{
	#if (LED_G24_ENABLE)
		#if LED_G24_INIT_HIGH_ENABLE
			device_led_init(PIN_24G_LED, LED_ON);
		#else
			device_led_init(PIN_24G_LED, LED_OFF);
		#endif
	#endif
	}
}

