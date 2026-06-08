#include "AAA_digital_tube.h"

#if (MODULE_DIGITAL_TUBE_ENABLE)
	#include "../8208_dual_mouse/AAA_public_config.h"
	#include "../8208_dual_mouse/main.h"
#endif

#if (PROJECT_ID == PID_DMS157)
	#include "AAA_led_rgb_two.h"
#endif

#if LED_CODE_ENABLE
	extern void rgb3810_sleep_close(void);
#endif

#if (PROJECT_ID == PID_DMS157)
	extern void led_reg_dpi_display_alloff(void);
#endif

#if (PROJECT_ID == PID_DMS157)
	extern void dms157_rgb_pwm_loop(void);
#endif

#if (MODULE_DIGITAL_TUBE_ENABLE)

const static unsigned int  sg_tube_tab[TUBE_NUM] = TUBE_PIN_LIST;
const static unsigned char sg_code_tab[CODE_NUM] = TUBE_CODE_LIST;

static tube_code_t sg_tube_code;
static tube_disp_sta_e tube_disp_status = TUBE_DISP_BATTERY;

tube_mode_t tube_sta = {
	.counter   = 0,
	.new_mode  = TUBE_LED_MAX,
	.last_mode = TUBE_LED_MAX,
	.led_sta   = LED_OFF,
	.on_flag   = 0,
};

#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_HM668)
	extern unsigned char gc_digital_dpi_flag;
	extern unsigned short int  gc_digital_dpi_delay;
#endif

#if TUBE_DPI_DISP_ENABLE
	extern unsigned char gc_digital_dpi_flag;
	extern unsigned short int  gc_digital_dpi_delay;
#endif

#if	TUBE_BAT_LOWER_ENABLE
	static unsigned char sg_tube_battery_counter = 0;
	static unsigned char sg_tube_battery_times = 0;
#endif

extern unsigned char battery_low_flag_status(void);

#define TUBE_1_LEVEL(x) 	digital_tube_io_out(TUBE_PIN_1, x)
#define TUBE_2_LEVEL(x) 	digital_tube_io_out(TUBE_PIN_2, x)
#define TUBE_3_LEVEL(x) 	digital_tube_io_out(TUBE_PIN_3, x)
#define TUBE_4_LEVEL(x) 	digital_tube_io_out(TUBE_PIN_4, x)
#define TUBE_5_LEVEL(x) 	digital_tube_io_out(TUBE_PIN_5, x)

#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_HM668)
	#define TUBE_6_LEVEL(x) 	digital_tube_io_out(TUBE_PIN_6, x)
	#define TUBE_7_LEVEL(x) 	digital_tube_io_out(TUBE_PIN_7, x)
#endif

void digital_tube_init(void)
{
	unsigned char i = 0;
	for ( i = 0; i < TUBE_NUM; i++ )
	{
		gpio_higsesistance_config(sg_tube_tab[i]);
	}
}

void digital_tube_io_default(void)
{
	unsigned char i = 0;
	for ( i = 0; i < TUBE_NUM; i++ )
	{
		gpio_set_output_en(sg_tube_tab[i], 0);
	}
}


void digital_tube_g24_status(tube_led_e mode)
{
	tube_sta.new_mode = mode;
}

void digital_tube_ble_status(tube_led_e mode)
{
	tube_sta.new_mode = mode;
}

void digital_tube_usb_pair(void)
{
	tube_sta.new_mode = TUBE_LED_PAIR;
}

void digital_tube_usb_succ(void)
{
	tube_sta.new_mode = TUBE_LED_SUCC;
}

void digital_tube_g24_act(void)
{
	static unsigned char sg_fliker_11_times = 0;
	
	if ( RF_2M_2P4G_MODE != flash_dev_info.mode ) return;
		
	if ( TUBE_LED_PAIR == tube_sta.new_mode )
	{
		if ( ++tube_sta.counter >= TUBE_PAIR_FLICK_CYCLE )
		{
			tube_sta.counter = 0;
			tube_sta.led_sta = !tube_sta.led_sta;
		#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_HM668)
			sg_tube_code.num5 = (tube_sta.led_sta == LED_ON)?(H_IN):(NONE_IN);
		#else
			sg_tube_code.num4 = (tube_sta.led_sta == LED_ON)?(H_IN):(NONE_IN);
		#endif
		}

		if ( connect_ok )
		{
			digital_tube_g24_status(TUBE_LED_SUCC);
		}
		
	}
	else if ( TUBE_LED_RECONN == tube_sta.new_mode )
	{
		if ( ++tube_sta.counter >= TUBE_RECONN_FLICK_CYCLE )
		{
			tube_sta.counter = 0;
			tube_sta.led_sta = !tube_sta.led_sta;
		#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_HM668)
			sg_tube_code.num5 = (tube_sta.led_sta == LED_ON)?(H_IN):(NONE_IN);
		#else
			sg_tube_code.num4 = (tube_sta.led_sta == LED_ON)?(H_IN):(NONE_IN);
		#endif
		}

	}
	else if ( TUBE_LED_SUCC == tube_sta.new_mode )
	{
		if ( ++tube_sta.counter >= TUBE_SUCC_FLICK_CYCLE )
		{
			tube_sta.counter = 0;
			tube_sta.led_sta = !tube_sta.led_sta;
			
			sg_tube_code.num4 = (tube_sta.led_sta == LED_ON)?(H_IN):(NONE_IN);

			if ( ++sg_fliker_11_times >= TUBE_SUCC_FLICK_CYCLE )
			{
				sg_fliker_11_times = 0;
				tube_sta.new_mode = TUBE_LED_ALL_ON;
			}
		}
		
	}
	else if (TUBE_LED_ALL_ON == tube_sta.new_mode)
	{
	#if (PROJECT_ID == PID_660)
		sg_tube_code.num5 = H_IN;
	#elif (PROJECT_ID == PID_HM668)
		sg_tube_code.num5 = NONE_IN;
	#elif (PROJECT_ID == PID_DMS157)
		sg_tube_code.num4 = H_IN;
	#else
		sg_tube_code.num4 = H_IN;
	#endif
		tube_sta.counter = 0;
		
	}

	if (TUBE_LED_SUCC != tube_sta.new_mode)
	{
		sg_fliker_11_times = 0;
	}
}

void digital_tube_ble_act(void)
{
	static unsigned char sg_fliker_22_times = 0;
	
	if ( RF_1M_BLE_MODE != flash_dev_info.mode ) return;
	
	if ( TUBE_LED_PAIR == tube_sta.new_mode )
	{
		if ( ++tube_sta.counter >= TUBE_PAIR_FLICK_CYCLE )
		{
			tube_sta.counter = 0;
			tube_sta.led_sta = !tube_sta.led_sta;
		#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_HM668)
			sg_tube_code.num5 = (tube_sta.led_sta == LED_ON)?(I_IN):(NONE_IN);
		#elif (PROJECT_ID == PID_DMS157) || (PROJECT_ID == PID_RM12)
			if ( BLE_DEVICE_ID_0 == ble_mode_device_id_get() )
			{
				sg_tube_code.num4 = (tube_sta.led_sta == LED_ON)?(I_IN):(NONE_IN);
			}
			else
			{
				sg_tube_code.num4 = (tube_sta.led_sta == LED_ON)?(J_IN):(NONE_IN);
			}
		#else
			sg_tube_code.num4 = (tube_sta.led_sta == LED_ON)?(I_IN):(NONE_IN);
		#endif
		}
		
	}
	else if ( TUBE_LED_RECONN == tube_sta.new_mode )
	{
		if ( ++tube_sta.counter >= TUBE_RECONN_FLICK_CYCLE )
		{
			tube_sta.counter = 0;
			tube_sta.led_sta = !tube_sta.led_sta;
		#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_HM668)
			sg_tube_code.num5 = (tube_sta.led_sta == LED_ON)?(I_IN):(NONE_IN);
		#elif (PROJECT_ID == PID_DMS157) || (PROJECT_ID == PID_RM12)
			if ( BLE_DEVICE_ID_0 == ble_mode_device_id_get() )
			{
				sg_tube_code.num4 = (tube_sta.led_sta == LED_ON)?(I_IN):(NONE_IN);
			}
			else
			{
				sg_tube_code.num4 = (tube_sta.led_sta == LED_ON)?(J_IN):(NONE_IN);
			}
		#else	
			sg_tube_code.num4 = (tube_sta.led_sta == LED_ON)?(I_IN):(NONE_IN);
		#endif
		}

	}
	else if ( TUBE_LED_SUCC == tube_sta.new_mode )
	{
		if ( ++tube_sta.counter >= TUBE_SUCC_FLICK_CYCLE )
		{
			tube_sta.counter = 0;
			tube_sta.led_sta = !tube_sta.led_sta;

		#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_HM668)
			sg_tube_code.num5 = (tube_sta.led_sta == LED_ON)?(I_IN):(NONE_IN);
		#elif (PROJECT_ID == PID_DMS157) || (PROJECT_ID == PID_RM12)
			if ( BLE_DEVICE_ID_0 == ble_mode_device_id_get() )
			{
				sg_tube_code.num4 = (tube_sta.led_sta == LED_ON)?(I_IN):(NONE_IN);
			}
			else
			{
				sg_tube_code.num4 = (tube_sta.led_sta == LED_ON)?(J_IN):(NONE_IN);
			}
		#else
			sg_tube_code.num4 = (tube_sta.led_sta == LED_ON)?(I_IN):(NONE_IN);
		#endif
			if ( ++sg_fliker_22_times >= TUBE_SUCC_FLICK_CYCLE )
			{
				sg_fliker_22_times = 0;
				tube_sta.new_mode = TUBE_LED_ALL_ON;
			}
		}
	}
	else if (TUBE_LED_ALL_ON == tube_sta.new_mode)
	{
	#if (PROJECT_ID == PID_660)
		sg_tube_code.num5 = I_IN;
	#elif (PROJECT_ID == PID_HM668)
		sg_tube_code.num5 = NONE_IN;
	#elif (PROJECT_ID == PID_DMS157) || (PROJECT_ID == PID_RM12)
		if ( BLE_DEVICE_ID_0 == ble_mode_device_id_get() )
		{
			sg_tube_code.num4 = I_IN;
		}
		else
		{
			sg_tube_code.num4 = J_IN;
		}
	#else
		sg_tube_code.num4 = I_IN;
	#endif
		tube_sta.counter = 0;
	}

	if (TUBE_LED_SUCC != tube_sta.new_mode)
	{
		sg_fliker_22_times = 0;
	}
}

#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_HM668)
	void digital_tube_display_dpi(void)
	{
		if ( 0 == gc_digital_dpi_flag ) return;

		if ( 0 == dpi_value )
		{
		#if (PROJECT_ID == PID_HM668)
			if ( SENSOR_S201B == sensor_type || SENSOR_3065XY == sensor_type )
			{
				sg_tube_code.num1 = sg_code_tab[ 1 ];
				sg_tube_code.num2 = sg_code_tab[ 2 ];
			}
			else
			{
				sg_tube_code.num1 = NONE_IN;
				sg_tube_code.num2 = sg_code_tab[ 8 ];
			}
		#else
			sg_tube_code.num1 = sg_code_tab[ 1 ];
			sg_tube_code.num2 = sg_code_tab[ 2 ];
		#endif
		}
		else if ( 1 == dpi_value )
		{
		#if (PROJECT_ID == PID_HM668)
			if ( SENSOR_S201B == sensor_type || SENSOR_3065XY == sensor_type )
			{
				sg_tube_code.num1 = sg_code_tab[ 1 ];
				sg_tube_code.num2 = sg_code_tab[ 6 ];
			}
			else
			{
				sg_tube_code.num1 = sg_code_tab[ 1 ];
				sg_tube_code.num2 = sg_code_tab[ 2 ];
			}
		#else
			sg_tube_code.num1 = sg_code_tab[ 1 ];
			sg_tube_code.num2 = sg_code_tab[ 6 ];
		#endif
		}
		else if ( 2 == dpi_value )
		{
		#if (PROJECT_ID == PID_HM668)
			if ( SENSOR_S201B == sensor_type || SENSOR_3065XY == sensor_type )
			{
				sg_tube_code.num1 = sg_code_tab[ 2 ];
				sg_tube_code.num2 = sg_code_tab[ 4 ];
			}
			else
			{
				sg_tube_code.num1 = sg_code_tab[ 1 ];
				sg_tube_code.num2 = sg_code_tab[ 6 ];
			}
		#else
			sg_tube_code.num1 = sg_code_tab[ 2 ];
			sg_tube_code.num2 = sg_code_tab[ 4 ];
		#endif
		}
		else if ( 3 == dpi_value )
		{
		#if (PROJECT_ID == PID_HM668)
			if ( SENSOR_S201B == sensor_type || SENSOR_3065XY == sensor_type )
			{
				sg_tube_code.num1 = sg_code_tab[ 3 ];
				sg_tube_code.num2 = sg_code_tab[ 2 ];
			}
		#else
			sg_tube_code.num1 = sg_code_tab[ 3 ];
			sg_tube_code.num2 = sg_code_tab[ 2 ];
		#endif
		}
		else if ( 4 == dpi_value )
		{
			sg_tube_code.num1 = sg_code_tab[ 4 ];
			sg_tube_code.num2 = sg_code_tab[ 8 ];
		}

		sg_tube_code.num3 = sg_code_tab[ 0 ];
		sg_tube_code.num4 = sg_code_tab[ 0 ];

		if ( ++gc_digital_dpi_delay >= 3500 )
		{
			gc_digital_dpi_delay = 0;
			gc_digital_dpi_flag = 0;
		}
	}
#endif


#if TUBE_DPI_DISP_ENABLE
	void digital_tube_display_dpi_gear(void)
	{
		if ( 0 == gc_digital_dpi_flag ) return;

		sg_tube_code.num1 = NONE_IN;
		sg_tube_code.num2 = NONE_IN;
		sg_tube_code.num3 = sg_code_tab[ dpi_value + 1];
		
		if ( ++gc_digital_dpi_delay >= 3000 )
		{
			gc_digital_dpi_delay = 0;
			gc_digital_dpi_flag = 0;
		}
	}
#endif

void digital_tube_pro(void)
{
	static unsigned int  sv_tube_cunt = 0;
	static unsigned char sv_tube_num = 1;
	static unsigned char sv_charge_flag = 0;

	unsigned char perc = battery_voltage_percent();

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

	#if (CHARGE_ENABLE)
		#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_DMS157) || CHARGE_IC_ENABLE
			if ( CHARGE_IN() )
			{
				idle_count = 0;
				adv_count  = 0;
				sv_charge_flag = 1;
			}
		#elif (PROJECT_ID == PID_FX282)
			if ( USB_IN() )
			{
				idle_count = 0;
			    adv_count  = 0;
			    if ( CHARGE_IN() )
			    {
			    	sv_charge_flag = 1;
				#if USB_5V_CHARGE_LED_ENABLE
			    	USB_5V_CHARGE_LED_ON();
				#endif
			    }
			}
		#else
			if ( USB_IN() && CHARGE_ING() )
			{
				idle_count = 0;
				adv_count  = 0;
				sv_charge_flag = 1;
			}
		#endif
	#endif

	#if (PROJECT_ID == PID_601)
		if ( USB_IN() )
		{
			idle_count = 0;
			adv_count  = 0;
			sv_charge_flag = 1;
		}
	#endif

	#if (PM_SYS_LOW_POWER_ENABLE)
		if ( connect_ok )
		{
			if ( idle_count >= TUBE_ON_TIME_MAX+2 )
			{

			#if (PROJECT_ID == PID_DMS157)
				if ( 0 == mcu_sleep_status.enter_lower )
				{
					led_rgb_stop();
				}
			#endif

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

			if ( idle_count < TUBE_ON_TIME_MAX && mcu_sleep_status.enter_lower )
			{
				if ( ++mcu_sleep_status.resume_cnt >= 5 )
				{
					mcu_sleep_status.resume_cnt = 0;
					mcu_sleep_status.enter_lower = 0;

				#if (PROJECT_ID == PID_DMS157)
					led_rgb_start();
				#endif
				}
			}

			#if (PROJECT_ID == PID_DMS157)
				if ( 0 == gc_digital_dpi_flag )
				{
					dms157_rgb_pwm_loop();
				}
			#endif
		}

		if ( 1 == mcu_sleep_status.enter_lower )
		{
			digital_tube_init();
			sv_tube_num  = 1;
			sv_tube_cunt = clock_time();
			return;
		}
	#endif

	if ( clock_time_exceed(sv_tube_cunt, TUBE_LOOP_CYCLE) )
	{
			digital_tube_io_default();

			sv_tube_cunt = clock_time();

			if ( 1 == sv_charge_flag )
			{
			#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_HM668)
				sg_tube_code.num1 = NONE_IN;
				sg_tube_code.num2 = NONE_IN;

				sg_tube_code.num3 = 0x39;
				sg_tube_code.num4 = 0x76;
			#else
				sg_tube_code.num1 = NONE_IN;

				sg_tube_code.num2 = 0x39;
				sg_tube_code.num3 = 0x76;

				sg_tube_code.num3 &= J_CLEAN;
			#endif
			}
			else
			{
				if ( 100 == perc )
				{
				#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_HM668)
					sg_tube_code.num1 = NONE_IN;
					sg_tube_code.num2 = sg_code_tab[ 1 ];
					sg_tube_code.num3 = sg_code_tab[ 0 ];
					sg_tube_code.num4 = sg_code_tab[ 0 ];
				#else
					sg_tube_code.num1 = B1_IN | C1_IN;
					sg_tube_code.num2 = sg_code_tab[ 0 ];
					sg_tube_code.num3 = sg_code_tab[ 0 ];
				#endif
				}
				else if ( 0 == perc )
				{
				#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_HM668)
					sg_tube_code.num1 = NONE_IN;
					sg_tube_code.num2 = NONE_IN;
					sg_tube_code.num3 = NONE_IN;
					sg_tube_code.num4 = sg_code_tab[ 0 ];
				#else
					sg_tube_code.num1 = NONE_IN;
					sg_tube_code.num2 = NONE_IN;
					sg_tube_code.num3 = sg_code_tab[ 0 ];
				#endif
				}
				else
				{
				#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_HM668)
					sg_tube_code.num1 = NONE_IN;
					sg_tube_code.num2 = NONE_IN;

					if ( perc/10 )
					{
						sg_tube_code.num3 = sg_code_tab[ perc/10 ];
					}
					else
					{
						sg_tube_code.num3 = NONE_IN;
					}

					sg_tube_code.num4 = sg_code_tab[ perc%10 ];

				#else
				
					sg_tube_code.num1 = NONE_IN;

					if ( perc/10 )
					{
						sg_tube_code.num2 = sg_code_tab[ perc/10 ];
					}
					else
					{
						sg_tube_code.num2 = NONE_IN;
					}

					sg_tube_code.num3 = sg_code_tab[ perc%10 ];
				#endif
				}
			}

			#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_HM668)
				digital_tube_display_dpi();
			#endif

			#if TUBE_DPI_DISP_ENABLE
				digital_tube_display_dpi_gear();
			#endif

		#if TUBE_BAT_LOWER_ENABLE
			if ( (1 == sv_tube_num) && (TUBE_DISP_BATTERY == tube_disp_status) )
		#else
			if ( 1 == sv_tube_num  )
		#endif
			{
			#if TUBE_SCAN_NOMAL_5_ENABLE
				if ( sg_tube_code.num1 & A1_IN )  TUBE_2_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num1 & B1_IN )  TUBE_3_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num1 & C1_IN )  TUBE_4_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num1 & D1_IN )  TUBE_5_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num1 & E1_IN )  TUBE_6_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num1 & F1_IN )  TUBE_7_LEVEL(TUBE_HIGH);

				TUBE_1_LEVEL(TUBE_LOW);
			#elif TUBE_SCAN_NOMAL_4_ENABLE
				if ( sg_tube_code.num3 & B3_IN )  TUBE_2_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num3 & D3_IN )  TUBE_3_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num3 & F3_IN )  TUBE_4_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num3 & G3_IN )  TUBE_5_LEVEL(TUBE_HIGH);

				TUBE_1_LEVEL(TUBE_LOW);
			#else
				if ( sg_tube_code.num1 & B1_IN )  TUBE_3_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num1 & C1_IN )  TUBE_2_LEVEL(TUBE_HIGH);

				if ( sg_tube_code.num2 & G2_IN )  TUBE_5_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num3 & E3_IN )  TUBE_1_LEVEL(TUBE_HIGH);

				TUBE_4_LEVEL(TUBE_LOW);
			#endif
			}
		#if TUBE_BAT_LOWER_ENABLE
			if ( (2 == sv_tube_num) && (TUBE_DISP_BATTERY == tube_disp_status) )
		#else
			else if ( 2 == sv_tube_num  )
		#endif
			{
			#if TUBE_SCAN_NOMAL_5_ENABLE
				if ( sg_tube_code.num1 & G1_IN )  TUBE_1_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num2 & A2_IN )  TUBE_3_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num2 & B2_IN )  TUBE_4_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num2 & C2_IN )  TUBE_5_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num2 & D2_IN )  TUBE_6_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num2 & E2_IN )  TUBE_7_LEVEL(TUBE_HIGH);

				TUBE_2_LEVEL(TUBE_LOW);
			#elif TUBE_SCAN_NOMAL_4_ENABLE
				if ( sg_tube_code.num3 & A3_IN )  TUBE_1_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num2 & B2_IN )  TUBE_3_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num2 & D2_IN )  TUBE_4_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num2 & E2_IN )  TUBE_5_LEVEL(TUBE_HIGH);

				TUBE_2_LEVEL(TUBE_LOW);
			#else
				if ( sg_tube_code.num2 & A2_IN )  TUBE_2_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num2 & C2_IN )  TUBE_4_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num2 & F2_IN )  TUBE_5_LEVEL(TUBE_HIGH);

				if ( sg_tube_code.num3 & C3_IN )  TUBE_1_LEVEL(TUBE_HIGH);

				TUBE_3_LEVEL(TUBE_LOW);
			#endif
			}
		#if TUBE_BAT_LOWER_ENABLE
			if ( (3 == sv_tube_num) && (TUBE_DISP_BATTERY == tube_disp_status) )
		#else
			else if ( 3 == sv_tube_num  )
		#endif
			{
			#if TUBE_SCAN_NOMAL_5_ENABLE
				if ( sg_tube_code.num2 & F2_IN )  TUBE_1_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num2 & G2_IN )  TUBE_2_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num3 & A3_IN )  TUBE_4_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num3 & B3_IN )  TUBE_5_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num3 & C3_IN )  TUBE_6_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num3 & D3_IN )  TUBE_7_LEVEL(TUBE_HIGH);

				TUBE_3_LEVEL(TUBE_LOW);	
			#elif TUBE_SCAN_NOMAL_4_ENABLE
				if ( sg_tube_code.num3 & C3_IN )  TUBE_1_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num2 & A2_IN )  TUBE_2_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num2 & C2_IN )  TUBE_4_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num2 & F2_IN )  TUBE_5_LEVEL(TUBE_HIGH);

				TUBE_3_LEVEL(TUBE_LOW);
			#else
				if ( sg_tube_code.num2 & B2_IN )  TUBE_3_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num2 & D2_IN )  TUBE_4_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num2 & E2_IN )  TUBE_5_LEVEL(TUBE_HIGH);

				if ( sg_tube_code.num3 & A3_IN )  TUBE_1_LEVEL(TUBE_HIGH);

				TUBE_2_LEVEL(TUBE_LOW);
			#endif
			}
		#if TUBE_BAT_LOWER_ENABLE
			if ( (4 == sv_tube_num) && (TUBE_DISP_BATTERY == tube_disp_status) )
		#else
			else if ( 4 == sv_tube_num )
		#endif
			{
			#if TUBE_SCAN_NOMAL_5_ENABLE
				if ( sg_tube_code.num3 & E3_IN )  TUBE_1_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num3 & F3_IN )  TUBE_2_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num3 & G3_IN )  TUBE_3_LEVEL(TUBE_HIGH);

				if ( sg_tube_code.num4 & A4_IN )  TUBE_5_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num4 & B4_IN )  TUBE_6_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num4 & C4_IN )  TUBE_7_LEVEL(TUBE_HIGH);

				TUBE_4_LEVEL(TUBE_LOW);	
			#elif TUBE_SCAN_NOMAL_4_ENABLE
				if ( sg_tube_code.num3 & E3_IN )  TUBE_1_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num1 & C1_IN )  TUBE_2_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num1 & B1_IN )  TUBE_3_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num2 & G2_IN )  TUBE_5_LEVEL(TUBE_HIGH);

				TUBE_4_LEVEL(TUBE_LOW);
			#else
				if ( sg_tube_code.num3 & B3_IN )  TUBE_2_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num3 & D3_IN )  TUBE_3_LEVEL(TUBE_HIGH);

				if ( sg_tube_code.num3 & F3_IN )  TUBE_4_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num3 & G3_IN )  TUBE_5_LEVEL(TUBE_HIGH);

				TUBE_1_LEVEL(TUBE_LOW);
			#endif
			}
		#if TUBE_BAT_LOWER_ENABLE
			if ( 5 == sv_tube_num && (TUBE_DISP_BATTERY == tube_disp_status) )
		#else
			else if ( 5 == sv_tube_num )
		#endif
			{
			#if TUBE_SCAN_NOMAL_5_ENABLE

				digital_tube_g24_act();
				digital_tube_ble_act();

				if ( sg_tube_code.num4 & D4_IN )  TUBE_1_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num4 & E4_IN )  TUBE_2_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num4 & F4_IN )  TUBE_3_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num4 & G4_IN )  TUBE_4_LEVEL(TUBE_HIGH);

				if ( sg_tube_code.num5 & H_IN )  TUBE_6_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num5 & I_IN )  TUBE_7_LEVEL(TUBE_HIGH);

				TUBE_5_LEVEL(TUBE_LOW);	
			#elif TUBE_SCAN_NOMAL_4_ENABLE

				digital_tube_g24_act();
				digital_tube_ble_act();

			#if (PROJECT_ID == PID_DMS157) || (PROJECT_ID == PID_RM12)
				// do nothing
			#else
				if ( sv_charge_flag )
				{
					sg_tube_code.num4 &= J_CLEAN;
				}
				else
				{
					sg_tube_code.num4 |= J_IN;
				}
			#endif

				if ( sg_tube_code.num4 & H_IN )  TUBE_4_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num4 & I_IN )  TUBE_3_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num4 & J_IN )  TUBE_2_LEVEL(TUBE_HIGH);

				TUBE_5_LEVEL(TUBE_LOW);
			#else
				digital_tube_g24_act();
				digital_tube_ble_act();

			#if (PROJECT_ID == PID_601)
				if ( USB_OUT() )
					sg_tube_code.num4 |= J_IN;
			#endif

				if ( sg_tube_code.num4 & P_IN )  TUBE_1_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num4 & H_IN )  TUBE_4_LEVEL(TUBE_HIGH);

				if ( sg_tube_code.num4 & I_IN )  TUBE_3_LEVEL(TUBE_HIGH);
				if ( sg_tube_code.num4 & J_IN )  TUBE_2_LEVEL(TUBE_HIGH);

				TUBE_5_LEVEL(TUBE_LOW);
			#endif
			}
		#if TUBE_BAT_LOWER_ENABLE
			if ( (6 == sv_tube_num) && (TUBE_DISP_BATTERY == tube_disp_status) )
		#else
			else if ( 6 == sv_tube_num )
		#endif
			{
			#if TUBE_SCAN_NOMAL_5_ENABLE
				#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_HM668)
					if ( 0 == gc_digital_dpi_flag  && 0 == sv_charge_flag )
					{
						TUBE_2_LEVEL(TUBE_HIGH);

						TUBE_6_LEVEL(TUBE_LOW);
					}
				#endif
			#else
				// do nothing
			#endif
			}
			else
			{
				#if TUBE_BAT_LOWER_ENABLE

				#endif
			}

			#if (CHARGE_ENABLE)

				#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_FX282) || (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_DMS157) || CHARGE_IC_ENABLE
					if ( sv_charge_flag && CHARGE_OUT() )
					{
						sv_charge_flag = 0;
					#if USB_5V_CHARGE_LED_ENABLE
						USB_5V_CHARGE_LED_OFF();
					#endif
						printf("usb out or charge full \n");
					}
				#else
					if ( sv_charge_flag && CHARGE_OFF() )
					{
						sv_charge_flag = 0;
						printf("usb out or charge full \n");
					}
				#endif

			#endif

			#if (PROJECT_ID == PID_601)
				if ( USB_OUT() )
				{
					sv_charge_flag = 0;
				}
			#endif

			#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_FX282) || (PROJECT_ID == PID_HM668)
				if ( CHARGE_OUT() )
				{
					sv_charge_flag = 0;
				#if USB_5V_CHARGE_LED_ENABLE
					USB_5V_CHARGE_LED_OFF();
				#endif
				}
			#endif

			#if TUBE_BAT_LOWER_ENABLE
				if ( ++sv_tube_num >= TUBE_SCAN_TIMES )
				{
					sv_tube_num = 1;
					if ( ++sg_tube_battery_counter >= 60 ) // 60
					{
						sg_tube_battery_counter = 0;

						if ( bat_flag.connect_bat_lower_flag )
						{
							tube_disp_status = (TUBE_DISP_BATTERY == tube_disp_status)?(TUBE_DISP_BATT_LOWER):(TUBE_DISP_BATTERY);
							if ( ++sg_tube_battery_times >= 6 )
							{
								sg_tube_battery_times = 0;
								bat_flag.connect_bat_lower_flag = 0;
								tube_disp_status = TUBE_DISP_BATTERY;
							}
						}
					}
				}
			#else
				if ( ++sv_tube_num >= TUBE_SCAN_TIMES )
				{
					sv_tube_num = 1;
				}
			#endif
	}
}

#endif
