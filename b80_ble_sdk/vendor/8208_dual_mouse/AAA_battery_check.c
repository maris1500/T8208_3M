/********************************************************************************************************
 * @file     AAA_battery_check.c
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
#include "AAA_battery_check.h"

#if (MODULE_MCU_EXTERNAL_ENABLE)
#include "../module/AAA_mcu_ext.h"
#endif

#if LED_BATT_CAP_LEVEL_ENABLE
	#include "../module/AAA_batt_cap.h"
#endif

#if (LED_BATT_CAP_LEVEL_ENABLE)
	extern unsigned char batt_charg_ing(void);
#endif

extern void user_batt_check_proc(void);


unsigned char vbatt_lower_connected(void)
{
	return (bat_flag.connect_bat_lower_flag);
}

void vbatt_lower_clean(void)
{
	bat_flag.connect_bat_lower_flag = 0;
}

#if	BATT_LOWER_LED_INDICATE_60S_ENABLE

	static unsigned int sg_less_60s_timer = 0;
	static unsigned char sg_less_60s_flag = 0;

	void battery_indicate_mouse_moved(void)
	{
		sg_less_60s_flag = 0;
		sg_less_60s_timer = clock_time() | 0x01;
	}

	unsigned char battery_indicate_lower_led_action(void)
	{
		if ( 0 == sg_less_60s_timer )
		{
			sg_less_60s_timer =  clock_time() | 0x01;
		}

		if ( 0 == sg_less_60s_flag )
		{
			if ( clock_time_exceed(sg_less_60s_timer, 60*1000*1000) )
			{
				sg_less_60s_flag = 1;
			}
		}

		return (sg_less_60s_flag);
	}
#endif

#include "AAA_LED.h"

#if BATT_CHECK_ENABLE


bat_status_flag_t bat_flag = {
	.power_on  = 1,
};

bat_data_mv_t bat_mv;

unsigned char batty_cap_not_low(void)
{
	if( 0 == bat_flag.low_batt ) {
		return(1);
	}

	return(0);
}

unsigned char battery_low_flag_status(void)
{
	return (bat_flag.low_batt);
}

void battery_low_flag_clear(void)
{
	bat_flag.low_batt = 0;
}

unsigned char battery_voltage_percent(void)
{
	return (bat_mv.percent);
}

unsigned char battary_cacl_cap_new(unsigned short vol)
{
	u8 Baifen = 0;
	u8 max_perc = 0, min_perc = 0, k_tem = 0;
	u16 max_vol_mv = 0, min_vol_mv = 0;

#if (BATT_CACL_4200mV_ENABLE)

	#if MODULE_DIGITAL_TUBE_ENABLE
		if ( vol >= 4000 )
		{
			Baifen = 100;
		}
		else if ( vol > 3000 )
		{
			if ( vol >= 3800 )
			{
				max_vol_mv = 4000; min_vol_mv = 3800;
				max_perc   = 100;  min_perc   = 80;
			}
			else if ( vol >= 3600 )
			{
				max_vol_mv = 3800; min_vol_mv = 3600;
				max_perc   = 80;   min_perc   = 45;
			}
			else if ( vol >= 3400 )
			{
				max_vol_mv = 3600; min_vol_mv = 3400;
				max_perc   = 45;   min_perc   = 15;
			}
			else if ( vol >= 3200 )
			{
				max_vol_mv = 3400; min_vol_mv = 3200;
				max_perc   = 15;   min_perc   = 5;
			}
			else
			{
				max_vol_mv = 3200; min_vol_mv = 3000;
				max_perc   = 5;   min_perc   = 1;
			}
			k_tem =  (max_vol_mv - min_vol_mv)  / (max_perc - min_perc) ;
			Baifen = ( vol - min_vol_mv ) / k_tem + min_perc;
		}
		else
		{
			Baifen = 1;
		}
	#elif (PROJECT_ID == PID_2187) || (PROJECT_ID == PID_0120)  || (PROJECT_ID == PID_S600)
		if ( vol >= 4100 )
		{
			Baifen = 100;
		}
		else if ( vol > 3100 )
		{
			if ( vol >= 3800 )
			{
				max_vol_mv = 4000; min_vol_mv = 3800;
				max_perc   = 100;  min_perc   = 70;
			}
			else if ( vol >= 3600 )
			{
				max_vol_mv = 3800; min_vol_mv = 3600;
				max_perc   = 70;   min_perc   = 40;
			}
			else if ( vol >= 3400 )
			{
				max_vol_mv = 3600; min_vol_mv = 3400;
				max_perc   = 40;   min_perc   = 6;
			}
			else
			{
				max_vol_mv = 3400; min_vol_mv = 3100;
				max_perc   = 5;   min_perc   = 1;
			}
			k_tem =  (max_vol_mv - min_vol_mv)  / (max_perc - min_perc) ;
			Baifen = ( vol - min_vol_mv ) / k_tem + min_perc;
		}
		else
		{
			Baifen = 1;
		}
	#elif (PROJECT_ID == PID_MS2360) || (PROJECT_ID == PID_104)
		if ( vol >= 4100 )
		{
			Baifen = 100;
		}
	    else if ( vol > 3200 )
		{
			if ( vol >= 3800 )
			{
				max_vol_mv = 4100; min_vol_mv = 3800;
				max_perc   = 100;  min_perc   = 70;
			}
			else if ( vol >= 3600 )
			{
				max_vol_mv = 3800; min_vol_mv = 3600;
				max_perc   = 70;   min_perc   = 35;
			}
			else if ( vol >= 3400 )
			{
				max_vol_mv = 3600; min_vol_mv = 3400;
				max_perc   = 35;   min_perc   = 10;
			}
			else
			{
				max_vol_mv = 3400; min_vol_mv = 3200;
				max_perc   = 10;   min_perc   = 1;
			}
			k_tem =  (max_vol_mv - min_vol_mv)  / (max_perc - min_perc) ;
			Baifen = ( vol - min_vol_mv ) / k_tem + min_perc;
	  }
	  else
	  {
	    	Baifen = 1;
	  }
	#elif BATT_CACL_METHOD_ENABLE
		if ( vol >= 4100 )
		{
			Baifen = 100;
		}
	    else if ( vol > 3000 )
		{
			if ( vol >= 3800 )
			{
				max_vol_mv = 4100; min_vol_mv = 3800;
				max_perc   = 100;  min_perc   = 70;
			}
			else if ( vol >= 3600 )
			{
				max_vol_mv = 3800; min_vol_mv = 3600;
				max_perc   = 70;   min_perc   = 35;
			}
			else if ( vol >= 3400 )
			{
				max_vol_mv = 3600; min_vol_mv = 3400;
				max_perc   = 35;   min_perc   = 10;
			}
			else
			{
				max_vol_mv = 3400; min_vol_mv = 3000;
				max_perc   = 10;   min_perc   = 1;
			}
			k_tem =  (max_vol_mv - min_vol_mv)  / (max_perc - min_perc) ;
			Baifen = ( vol - min_vol_mv ) / k_tem + min_perc;
	  }
	  else
	  {
	    	Baifen = 1;
	  }
	#else
		if ( vol >= 4000 )
		{
			Baifen = 100;
		}
		else if ( vol > 3000 )
		{
			if ( vol >= 3800 )
			{
				max_vol_mv = 4000; min_vol_mv = 3800;
				max_perc   = 100;  min_perc   = 70;
			}
			else if ( vol >= 3600 )
			{
				max_vol_mv = 3800; min_vol_mv = 3600;
				max_perc   = 70;   min_perc   = 35;
			}
			else if ( vol >= 3300 )
			{
				max_vol_mv = 3600; min_vol_mv = 3300;
				max_perc   = 35;   min_perc   = 10;
			}
			else
			{
				max_vol_mv = 3300; min_vol_mv = 3000;
				max_perc   = 10;   min_perc   = 2;
			}
			k_tem =  (max_vol_mv - min_vol_mv)  / (max_perc - min_perc) ;
			Baifen = ( vol - min_vol_mv ) / k_tem + min_perc;
		}
		else
		{
			Baifen = 1;
		}
	#endif
#elif (BATT_CACL_1500mV_ENABLE)

	#if  (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
		if (vol >= 1450)
		{
			Baifen = 100;
		}
		else if ( vol > 900 )
		{
			if ( vol >= 1350 )
			{
				max_vol_mv = 1450;  min_vol_mv = 1350;
				max_perc   = 100;   min_perc   = 55;
			}
			else if ( vol >= 1200 )
			{
				max_vol_mv = 1350; min_vol_mv = 1200;
				max_perc   = 55;   min_perc   = 20;
			}
			else if ( vol >= 1100 )
			{
				max_vol_mv = 1200; min_vol_mv = 1100;
				max_perc   = 20;   min_perc   = 10;
			}
			else
			{
				max_vol_mv = 1100; min_vol_mv = 900;
				max_perc   = 10;    min_perc   = 2;
			}
			k_tem = (max_vol_mv - min_vol_mv) / (max_perc - min_perc) ;
			Baifen = ( vol - min_vol_mv ) / k_tem + min_perc;
		}
		else
		{
			Baifen = 1;
		}
	#else
		if (vol >= 1450)
		{
			Baifen = 100;
		}
		else if ( vol > 1000 )
		{
			if ( vol >= 1350 )
			{
				max_vol_mv = 1450;  min_vol_mv = 1350;
				max_perc   = 100;   min_perc   = 55;
			}
			else if ( vol >= 1200 )
			{
				max_vol_mv = 1350; min_vol_mv = 1200;
				max_perc   = 55;   min_perc   = 20;
			}
			else if ( vol >= 1100 )
			{
				max_vol_mv = 1200; min_vol_mv = 1100;
				max_perc   = 20;   min_perc   = 10;
			}
			else
			{
				max_vol_mv = 1100; min_vol_mv = 1000;
				max_perc   = 10;    min_perc   = 2;
			}
			k_tem = (max_vol_mv - min_vol_mv) / (max_perc - min_perc) ;
			Baifen = ( vol - min_vol_mv ) / k_tem + min_perc;
		}
		else
		{
			Baifen = 1;
		}
	#endif

#elif BATT_CACL_3000mV_ENABLE
	if ( vol >= 2900 )
	{
		Baifen = 100;
	}
	else if ( vol >= 2100 )
	{
		if ( vol >= 2800 )
		{
			max_vol_mv = 3000; min_vol_mv = 2800;
			max_perc   = 100;  min_perc   = 70;
		}
		else if ( vol >= 2600 )
		{
			max_vol_mv = 2800; min_vol_mv = 2600;
			max_perc   = 70;   min_perc   = 35;
		}
		else if ( vol >= 2400 )
		{
			max_vol_mv = 2800; min_vol_mv = 2400;
			max_perc   = 35;   min_perc   = 15;
		}
		else
		{
			max_vol_mv = 2400; min_vol_mv = 2100;
			max_perc   = 15;   min_perc   = 2;
		}

		k_tem = (max_vol_mv - min_vol_mv) / (max_perc - min_perc) ;
		Baifen = ( vol - min_vol_mv ) / k_tem + min_perc;
	}
	else
	{
		Baifen = 1;
	}
#endif

	if ( Baifen >= 100 ) { Baifen = 100; }
	if ( Baifen <= 1 )   { Baifen = 1;   }

	return (Baifen);
}


#if KEY_ADC_MODE_MULTI_FUNC_ENABLE
	unsigned int adc_sample_voltale(void)
	{
		unsigned int temp = 0;

		adc_power_on_sar_adc(1);
		temp = adc_sample_and_get_result();
		adc_power_on_sar_adc(0);

		return (temp);
	}
#endif

#if (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
	unsigned int adc_sample_voltale_high(void)
	{
		unsigned int temp = 0;

		adc_power_on_sar_adc(1);
		temp = adc_sample_and_get_result();
		adc_power_on_sar_adc(0);

		return (temp);
	}
#endif

void battery_voltage_compensation(unsigned short int vol)
{
#if (PROJECT_ID == PID_HYC351)
	if (fun_mode == RF_2M_2P4G_MODE )
	{
		bat_mv.batt_vol += 248;
	}
	else
	{
		bat_mv.batt_vol -= 30;
	}
#endif

#if (PROJECT_ID == PID_SQ220)
	bat_mv.batt_vol += 335;
#endif
}

#if (PROJECT_ID == PID_MS2360)
	extern void led_flick_times(unsigned char,  unsigned int);
#endif

#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_FX282)  || (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_DMS157) || VOLTAGE_TEMP_HANDLE_ENABLE
 	extern unsigned char gc_vol_cap_reg_temp;
#endif

#if USB_5V_CHECK_ENABLE || CHARGE_ENABLE
	unsigned char battery_shut_check_charge(void)
	{
		#if USB_5V_CHECK_ENABLE
			if ( USB_IN() )
			{
				return 1;
			}
		#endif

		#if CHARGE_ENABLE
			if ( CHARGE_IN() )
			{
				return 1;
			}
		#endif

		return 0;
	}
#endif

void user_battery_power_check()
{
	#define BATT_PRINTF_ENABLE	0

	static u16 batt_vol_curr = 0;

#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_FX282) || (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_DMS157)  || VOLTAGE_TEMP_HANDLE_ENABLE
	static unsigned char sc_reg_use_flag = 0;
#endif

	adc_power_on_sar_adc(1);
	bat_mv.batt_vol = adc_sample_and_get_result();
	adc_power_on_sar_adc(0);

	battery_voltage_compensation(bat_mv.batt_vol);

#if (PROJECT_ID == PID_601)
	if ( fun_mode == RF_2M_2P4G_MODE )
		bat_mv.batt_vol += 73;
	else
		bat_mv.batt_vol += 80;
#endif

#if (PROJECT_ID == PID_S600)
	if ( fun_mode == RF_1M_BLE_MODE )
		bat_mv.batt_vol += 285;
#endif

#if (PROJECT_ID == PID_BM2060)
	bat_mv.batt_vol += 20;
#endif

#if (PROJECT_ID == PID_104)
	bat_mv.batt_vol += 310;
#endif

#if (PROJECT_ID == PID_HM660)
	if ( fun_mode == RF_2M_2P4G_MODE )
	{
		bat_mv.batt_vol += 40;
	}
	else
	{
		bat_mv.batt_vol += 333;
	}
#endif

#if (PROJECT_ID == PID_DMS157)
	if ( fun_mode == RF_2M_2P4G_MODE )
	{
		bat_mv.batt_vol += 345;
	}
	else
	{
		bat_mv.batt_vol += 335;
	}
#endif

#if (PROJECT_ID == PID_RM12)
	if ( fun_mode == RF_2M_2P4G_MODE )
	{
		bat_mv.batt_vol += 331;
	}
	else
	{
		bat_mv.batt_vol += 315;
	}
#endif


#if (PROJECT_ID == PID_2223)
	bat_mv.batt_vol += 55;
#endif

#if (PROJECT_ID == PID_MS13)
	if ( fun_mode == RF_2M_2P4G_MODE )
	{
		bat_mv.batt_vol += 30;
	}
	else
	{
		bat_mv.batt_vol += 320;
	}
#endif

#if (PROJECT_ID == PID_DMS06)
	 if ( fun_mode == RF_1M_BLE_MODE )
	 {
		 bat_mv.batt_vol = bat_mv.batt_vol + 315;
	 }
#endif

#if (PROJECT_ID == PID_V106)
	if ( fun_mode == RF_1M_BLE_MODE )
	{
		bat_mv.batt_vol += 255;
	}
	else
	{
		bat_mv.batt_vol -= 45;
	}
#endif

#if (PROJECT_ID == PID_660)

	#if MOUSE_660_HW_2_ENABLE
		bat_mv.batt_vol += 325;
	#else
		if ( fun_mode == RF_1M_BLE_MODE )
		{
			 bat_mv.batt_vol += 280;
		}
		else
		{
			if ( bat_mv.batt_vol > 3500 )
				bat_mv.batt_vol -= 42;
		}
	#endif

#endif


#if (PROJECT_ID == PID_HM668)
	if (  (connect_ok) && (!flash_dev_info.code_led_sw) )
	{
		if ( idle_count <= BATT_DISPLAY_TUBE_ON_TIME_MAX )
		{
			bat_mv.batt_vol += 395;
		}
		else
		{
			bat_mv.batt_vol += 342;
		}
	}
	else
	{
		bat_mv.batt_vol += 342;
	}

	if ( fun_mode == RF_2M_2P4G_MODE )
	{
		if ( gc_hm668_battery_try_times <= 2 )
		{
			bat_mv.batt_vol += 20;
		}
	}
#endif


#if (PROJECT_ID == PID_S008)
	if ( fun_mode == RF_1M_BLE_MODE )
	{
		 bat_mv.batt_vol += 300;
	}
#endif

#if (PROJECT_ID == PID_FX282)
	if ( fun_mode == RF_2M_2P4G_MODE )
	{
		if ( bat_mv.batt_vol < 3365 )
			bat_mv.batt_vol += 321;
		else
			bat_mv.batt_vol += 315;
	}
	else
	{
		bat_mv.batt_vol += 335;
	}
#endif

#if (PROJECT_ID == PID_XT27)
	bat_mv.batt_vol += 255;
#endif

#if (PROJECT_ID == PID_4027)
	bat_mv.batt_vol += 325;
#endif

#if (PROJECT_ID == PID_E368_CHARGE)
	if ( fun_mode == RF_2M_2P4G_MODE )
	{
		bat_mv.batt_vol += 43;
	}
	else
	{
		bat_mv.batt_vol += 345;
	}
#endif

#if (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
	if (bat_mv.batt_vol >= KEY_ADC_VOLTAGE_VALUE)
	{
		return;
	}
#endif

#if BATT_PRINTF_ENABLE
	 printf("orin = %d ", bat_mv.batt_vol);
#endif

#if KEY_ADC_MODE_MULTI_FUNC_ENABLE
	if (bat_mv.batt_vol < KEY_ADC_VOLTAGE_VALUE)
	{
		printf("batt_vol = %d, return \n", bat_mv.batt_vol);
		return;
	}
#endif

	if (batt_vol_curr == 0) // battery value == 0, means it's the first sample
	{

	#if (PROJECT_ID == PID_HM668)
		bat_flag.data_notify = 0;
	#else
		bat_flag.data_notify = 1; 
	#endif

		bat_flag.data_notify  = 1;
		bat_flag.sample_first = 1;
		batt_vol_curr = bat_mv.batt_vol;
	}
	else
	{

	#if(PROJECT_ID == PID_TB129)
		batt_vol_curr = (batt_vol_curr*9 + bat_mv.batt_vol)/10;
	#endif
	}
	
#if (PROJECT_ID == PID_HM668)
	batt_vol_curr = (batt_vol_curr*9 + bat_mv.batt_vol)/10;
#endif

#if BATT_VOLTAGE_SMOOTH_ENABLE
	batt_vol_curr = (batt_vol_curr*9 + bat_mv.batt_vol)/10;
#endif

#if (PROJECT_ID == PID_AM8690) 
	if ( batt_vol_curr > bat_mv.shutdown_vol )
	{
		sg_mouse_shutdown_flag = 0;
	}
#endif

#if BATT_PRINTF_ENABLE
	 printf("batt:%d ", batt_vol_curr);
#endif

	if (batt_vol_curr <= bat_mv.shutdown_vol) 
	{
		// printf("low bat enter deep\n");

		#if (LED_BATT_CAP_LEVEL_ENABLE)
			if ( !batt_charg_ing() )
			{
				enter_deep_aaa(BATTERT_SHUTDOWN_ENTER_DEEPSLEEP_ANA_AAA);
			}
		#elif BATT_SHUTDOWN_NONE_ENABLE
			bat_flag.low_batt = 1;
		#else

			#if (PROJECT_ID == PID_MS2360)
				led_flick_times(10, 100);
			#endif

		#if USB_5V_CHECK_ENABLE || CHARGE_ENABLE

			#if (PROJECT_ID == PID_660)

				if ( battery_shut_check_charge() )
				{
					// do nothing
				}
				else
				{
					enter_deep_aaa(BATTERT_SHUTDOWN_ENTER_DEEPSLEEP_ANA_AAA);
				}

			#else

				#if USB_5V_CHECK_ENABLE && CHARGE_ENABLE
					if ( USB_IN() || CHARGE_IN() )
				#elif USB_5V_CHECK_ENABLE
					if ( USB_IN() )
				#elif USB_MODE_ENABLE
					if ( USB_OUT() || CHARGE_IN() )
				#elif CHARGE_ENABLE
					if ( CHARGE_IN() )
				#endif
					{
						// do nothing
					}
					else
					{
					//	enter_deep_aaa(BATTERT_SHUTDOWN_ENTER_DEEPSLEEP_ANA_AAA);
					}

			#endif
		
		#else

			enter_deep_aaa(BATTERT_SHUTDOWN_ENTER_DEEPSLEEP_ANA_AAA);

		#endif

		#endif

		#if (PROJECT_ID == PID_PCM8409)
			 bat_flag.low_batt = 1;
		#endif
	}
	else if (batt_vol_curr <= bat_mv.alarm_vol )
	{
	#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_FX282)
		if ( CHARGE_IN() )
		{
			bat_flag.low_batt = 0;
		}
		else
		{
			bat_flag.low_batt = 1;
		}
	#else
		bat_flag.low_batt = 1; // alarm voltage
	#endif
	}
	else
	{
	#if (MODULE_MCU_EXTERNAL_ENABLE)
		ext_mcu_lvd_logo.lvd_act = 0;
		ext_mcu_lvd_logo.lvd_send_times = 0;
	#endif
		bat_flag.low_batt = 0; // normal voltage
	}
	
	bat_flag.power_on = 0;

#if LED_BATT_CAP_LEVEL_ENABLE
	CHARGE_OPEN();
	batt_charge_switch_set( BATT_CHARGE_OPEN );
#endif

	bat_mv.percent = battary_cacl_cap_new( batt_vol_curr );

#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_FX282) || (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_DMS157) || VOLTAGE_TEMP_HANDLE_ENABLE

	if ( 0 == sc_reg_use_flag )
	{
		sc_reg_use_flag = 1;
		if ( gc_vol_cap_reg_temp < 100 && gc_vol_cap_reg_temp >= 1 )
		{
			bat_mv.percent = ( gc_vol_cap_reg_temp*19 + bat_mv.percent ) / 20;
		}
		analog_write(BATTERY_CAP_REG, 0x00);
	}

	gc_vol_cap_reg_temp = bat_mv.percent;
#endif

#if BATT_PRINTF_ENABLE
	printf("per = %d \n", bat_mv.percent);
#endif

#if (LED_BATT_CAP_LEVEL_ENABLE)
	if ( batt_charg_ing() )
	{
		bat_flag.low_batt = 0;
	}
#endif


#if BATT_LOWER_LED_INDICATE_60S_ENABLE
	if ( bat_flag.low_batt )
	{
		if ( battery_indicate_lower_led_action() )
		{
			return;
		}
	}
	else
	{
		battery_indicate_mouse_moved();
	}
#endif


#if (PROJECT_ID == PID_HM668)
	if ( gc_hm668_battery_try_times < 2 )
	{
		return;
	}
#endif

#if (PROJECT_ID == PID_535)
	extern void vbat_lower_22_led_show(void);

	if ( connect_ok && bat_flag.low_batt )
	{
		if ( idle_count <= 10 )
		{
			vbat_lower_22_led_show();
		}
	}

#endif

#if LED_BAT_VOLTAGE_ENABLE
	#if (PROJECT_ID == PID_M710)
		// do nothing
	#else
		static unsigned char sl_test_vabat_count = 0;
		if ( 1 == bat_flag.low_batt )
		{
			if ( connect_ok )
			{
				if (sl_test_vabat_count <= 2)
				{
					sl_test_vabat_count++;
				}

				if( (connect_ok == 1) && (sl_test_vabat_count >= 2) )
				{
					#if LED_DPI_VBAT_DIFF_ENABLE
						 sc_bat_dpi_flag = LED_DIFF_BATTERY_LOW;
					#endif

				#if (PROJECT_ID == PID_M388)
				    if ( 0 == bat_flag.connect_bat_lower_flag )
				    {
				    	idle_count = 0;// do nothing
				    }
				#elif (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_DMS157) || (PROJECT_ID == PID_RM12)
				    // do nothing
				#elif (PROJECT_ID == PID_535)
				   //
				#elif (MODULE_MCU_EXTERNAL_ENABLE)
					ext_mcu_lvd_logo.lvd_act = 1;
					ext_mcu_lvd_logo.lvd_send_times = 0;
				#else
					if ( 0 == vbat_led_show() )
					{
					#if LED_DPI_VBAT_DIFF_ENABLE
						 sc_bat_dpi_flag = LED_DIFF_NONE;
					#endif
					}
				#endif

					bat_flag.connect_bat_lower_flag = 1;
					// printf("---->vbat low\n");
				}
			}
			else
			{
				sl_test_vabat_count = 0;
				bat_flag.connect_bat_lower_flag = 0;
			}
		}
	#endif
#endif


	if ( ABS(my_batVal[0], bat_mv.percent) >= BATT_CAP_DIFF_VALUE )
	{
	#if BATTERT_STORAGE_ENABLE /* When in use, only allow battery percentage reduce */
		if (my_batVal[0] > Baifen)
		{
			my_batVal[0]--;
			flash_dev_info.battery_value = my_batVal[0];
			save_data_to_flash(CFG_DEVICE_MODE_ADDR, sizeof(FLASH_DEV_INFO_AAA), (u8 *)&flash_dev_info.dongle_id, &dev_info_idx);
			bat_flag.data_notify = 1; // notify battery value to master
		}
	#else /* Use the current sample value as the battery value */
		my_batVal[0] = bat_mv.percent;

		bat_flag.data_notify = 1;

	#endif
	}

#if (PROJECT_ID == PID_AM8690) 
	if ( sg_mouse_shutdown_flag )
	{
		my_batVal[0] = Baifen;
		bat_flag.data_notify = 1;
	}
#endif

}

/**
 * @brief	Battery detection initial
 * @param	none
 * @return	none
 */
void user_batt_check_init()
{
	adc_init();

	adc_base_init(GPIO_VBAT_DETECT);
	adc_set_ain_pre_scaler(ADC_PRESCALER_1F8);//ADC pre_scaling default value is ADC_PRESCALER_1F8, it can change after adc_base_init().
	adc_power_on_sar_adc(1);
	adc_sample_and_get_result();
	adc_power_on_sar_adc(0);

	bat_mv.batt_detc_tick = clock_time();

}

void adc_to_mode_gpio_pin_init(void)
{
#if (ADC_TO_GPIO_MODE_EN)
	gpio_input_config_status(PIN_BTN_MODE, PM_PIN_PULLUP_1M);
	sleep_us(50);
#endif

}

void batt_check_time_reload(void)
{
	 bat_mv.batt_detc_tick = clock_time();
}

void charge_plugin_check(void)
{
#if USB_MODE_ENABLE || USB_5V_CHECK_ENABLE
	if ( USB_IN() )
	{
		bat_flag.charge_ing = 1;
		bat_flag.charge_ful = 0;

		adv_count = 1;
		idle_count = 1;

	#if CHARGE_ENABLE
		if ( CHARGE_FULL()  )
		{
			bat_flag.charge_ful = 1;
			bat_flag.charge_ing = 0;
		}
	#endif
	}

	if ( USB_OUT() )
	{
		bat_flag.charge_ing = 0;
		bat_flag.charge_ful = 0;
	}
#endif
}

/**
 * @brief 	Battery detection polling
 * @param	none
 * @return	none
 */
void user_batt_check_proc()
{
#if (PROJECT_ID == PID_HM668)
	unsigned int HM668_BATT_TIME_CHECK = 0;
#endif

#if LED_BATT_CAP_LEVEL_ENABLE
	#define BATT_SAMPLING_TIME   500000UL
#elif (PROJECT_ID == PID_535)
	#define BATT_SAMPLING_TIME   300000UL
#else
	#define BATT_SAMPLING_TIME   20*1000*1000
#endif


#if (LED_BATT_CAP_LEVEL_ENABLE)
	if ( batt_charge_stop_read_cap() )
	{
		return;
	}
#endif

#if (PROJECT_ID == PID_G3750) || (PROJECT_ID == PID_G3760) || (PROJECT_ID == PID_TB114)
	 bat_flag.power_on = 0;
#endif

#if (PROJECT_ID == PID_HM668)

	 if ( gc_hm668_battery_try_times <= 2 )
	 {
		 HM668_BATT_TIME_CHECK = 400*1000;
	 }
	 else
	 {
		 HM668_BATT_TIME_CHECK = 10*1000*1000;
	 }

	 if( bat_flag.power_on  || clock_time_exceed(bat_mv.batt_detc_tick, HM668_BATT_TIME_CHECK) ) //100000
#else
	if( bat_flag.power_on  || clock_time_exceed(bat_mv.batt_detc_tick, BATT_SAMPLING_TIME) ) //100000
#endif
	{
		#if (PROJECT_ID == PID_2223)
			user_batt_check_init();
		#endif

		bat_mv.batt_detc_tick = clock_time();

		#if (PROJECT_ID == PID_HM668)
			 if ( gc_hm668_battery_try_times <= 2 )
			 {
				 gc_hm668_battery_try_times++;
			 }
		#endif

		#if (ADC_TO_GPIO_MODE_EN)
			bat_flag.power_on = 0;
			adc_to_mode_gpio_pin_init();
			if( 0 == gpio_read(PIN_BTN_MODE) )
			{
				printf("---->vbat return \n");
				 return;
			}
		#endif

		#if (ADC_TO_GPIO_MODE_EN)
			gpio_higsesistance_config(PIN_BTN_MODE);
			adc_base_init(GPIO_VBAT_DETECT);
			adc_set_ain_pre_scaler(ADC_PRESCALER_1F8);
		#endif

		user_battery_power_check();
	
		#if (ADC_TO_GPIO_MODE_EN)
			adc_to_mode_gpio_pin_init();
		#endif

		bat_mv.batt_detc_tick = clock_time();
	}
}


#endif
