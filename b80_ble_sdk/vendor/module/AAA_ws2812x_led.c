#include "AAA_ws2812x_led.h"

#if LED_CODE_PWIR_DRIVE_ENABLE

#include "../../drivers/pwm.h"

static unsigned short sg_pwm_ir_dma_buff[WS2812_RGB_BIT_LEN_MAX+7] = {0};

static ws2812_led_scan_state sg_ws2812_sacn_sta =
{
	.status_bytes = 0,
};

unsigned int sg_color_ws2812_list[] = {

	WS2812_RED,
	WS2812_GREEN,
	WS2812_BLUE,
	WS2812_YELLOW,

	WS2812_CYAN,
	WS2812_PURPLE,
	WS2812_WHITE,
};

static unsigned int sg_ws2812_timer = 0, sg_ws2812_breath_value = 0;


static void ws2812_push_color(unsigned short *index, unsigned int color)
{
	unsigned char i = 0;

	for (i = 0; i < WS2812_PIXEL_NUMBER; i++)
	{
		if (color & 0x00800000)
		{
			sg_pwm_ir_dma_buff[ (*index)++ ] = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_SHADOW, 1);
		}
		else
		{
			sg_pwm_ir_dma_buff[ (*index)++ ] = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, 1);
		}

		color <<= 1;
	}
}

void ws2812_pwm_dma_wave_download(unsigned short len)
{
	unsigned short length = ( ( len << 1 ) - 4 );//The first four bytes are data length bytes, not included in the actual length to be sent
	unsigned char  *pbuff = (unsigned char*)(&sg_pwm_ir_dma_buff[0]);

	sg_pwm_ir_dma_buff[len++] = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, WS2812_RESET_TICK);

	pbuff[0] = length&0xff;
	pbuff[1] = (length>>8)&0xff;

	pbuff[2] = 0x00;
	pbuff[3] = 0x00;

	pwm_set_dma_address(&sg_pwm_ir_dma_buff);

	pwm_set_interrupt_enable(PWM_IRQ_PWM0_IR_DMA_FIFO_DONE);
	irq_set_mask(FLD_IRQ_SW_PWM_EN);
	pwm_clear_interrupt_status(PWM_IRQ_PWM0_IR_DMA_FIFO_DONE);
	pwm_start_dma_ir_sending();
}


void ws2812_off_parameter_clear(void)
{
	sg_ws2812_sacn_sta.status_bytes = 0x00;
	sg_ws2812_sacn_sta.bits.power_off = 1;

}

void ws2812_mode_change_para_clear(void)
{
	ws2812_off_parameter_clear();
	sg_ws2812_sacn_sta.bits.power_off = 0;
}


void ws2812_sleep_close(void)
{
	ws2812_off_parameter_clear();
	LED_CODE_POWER_OFF();
}


void ws2812_light_on_together(void)
{
	#define WS2812_ALL_ON_TIMES 4

	unsigned char k = 0;
	unsigned short index = 2;

	if ( clock_time_exceed(sg_ws2812_timer, 300*1000) )
	{
		sg_ws2812_timer = clock_time();

		for (k = 0; k < WS2812_LED_NUMBER; k++)
		{
			ws2812_push_color( &index, sg_color_ws2812_list[ sg_ws2812_sacn_sta.bits.index ] );
		}

		if ( ++sg_ws2812_sacn_sta.bits.times >= WS2812_ALL_ON_TIMES )
		{
			sg_ws2812_sacn_sta.bits.times = 0;

			if ( ++sg_ws2812_sacn_sta.bits.index >= WS2812_COLOR_MAX )
			{
				sg_ws2812_sacn_sta.bits.index = 0;
			}
		}

		ws2812_pwm_dma_wave_download( index );
	}
}

void ws2812_light_on_push(void)
{
	#define WS2812_PUSH_DELAY 4

	unsigned char k = 0;
	unsigned short index = 2;

	if ( clock_time_exceed(sg_ws2812_timer, 300*1000) )
	{
		sg_ws2812_timer = clock_time();

		for (k = 0; k < WS2812_LED_NUMBER; k++)
		{
			if ( k <= sg_ws2812_sacn_sta.bits.position )
			{
				ws2812_push_color( &index, sg_color_ws2812_list[ sg_ws2812_sacn_sta.bits.index ] );
			}
			else
			{
				ws2812_push_color( &index, WS2812_BLACK );
			}
		}

		if ( ++sg_ws2812_sacn_sta.bits.times >= WS2812_PUSH_DELAY )
		{
			sg_ws2812_sacn_sta.bits.times = 0;

			if ( ++sg_ws2812_sacn_sta.bits.position >= WS2812_LED_NUMBER )
			{
				sg_ws2812_sacn_sta.bits.position = 0;
				if ( ++sg_ws2812_sacn_sta.bits.index >= WS2812_COLOR_MAX )
				{
					sg_ws2812_sacn_sta.bits.index = 0;
				}
			}
		}

		ws2812_pwm_dma_wave_download(index);
	}
}

void ws2812_light_on_push_back(void)
{
	#define WS2812_PUSH_BACK_TIMES 4

	unsigned char k = 0;
	unsigned short index = 2;

	if ( clock_time_exceed(sg_ws2812_timer, 300*1000) )
	{
		sg_ws2812_timer = clock_time();

		for (k = 0; k < WS2812_LED_NUMBER; k++)
		{
			if ( k <= sg_ws2812_sacn_sta.bits.position )
			{
				ws2812_push_color( &index, sg_color_ws2812_list[ sg_ws2812_sacn_sta.bits.index ] );
			}
			else
			{
				ws2812_push_color( &index, WS2812_BLACK );
			}
		}

		if ( ++sg_ws2812_sacn_sta.bits.times >= WS2812_PUSH_BACK_TIMES )
		{
			sg_ws2812_sacn_sta.bits.times = 0;

			if ( 0 == sg_ws2812_sacn_sta.bits.open )
			{
				if ( ++sg_ws2812_sacn_sta.bits.position >= WS2812_LED_NUMBER )
				{
					sg_ws2812_sacn_sta.bits.open = 1;
					sg_ws2812_sacn_sta.bits.position = WS2812_LED_NUMBER - 2;
				}
			}
			else
			{
				if ( 0 == sg_ws2812_sacn_sta.bits.position )
				{
					sg_ws2812_sacn_sta.bits.open = 0;
					if ( ++sg_ws2812_sacn_sta.bits.index >= WS2812_COLOR_MAX )
					{
						sg_ws2812_sacn_sta.bits.index = 0;
					}
				}
				else
				{
					sg_ws2812_sacn_sta.bits.position--;
				}
			}
		}

		ws2812_pwm_dma_wave_download(index);
	}
}

void ws2812_light_on_single(void)
{
	#define WS2812_SINGGLE_ON_TIMES 4

	unsigned char k = 0;
	unsigned short index = 2;

	if ( clock_time_exceed(sg_ws2812_timer, 300*1000) )
	{
		sg_ws2812_timer = clock_time();

		for (k = 0; k < WS2812_LED_NUMBER; k++)
		{
			if ( k == sg_ws2812_sacn_sta.bits.position )
			{
				ws2812_push_color( &index, sg_color_ws2812_list[ sg_ws2812_sacn_sta.bits.index ] );
			}
			else
			{
				ws2812_push_color( &index, WS2812_BLACK );
			}
		}

		if ( ++sg_ws2812_sacn_sta.bits.times >= WS2812_SINGGLE_ON_TIMES )
		{
			sg_ws2812_sacn_sta.bits.times = 0;

			if ( ++sg_ws2812_sacn_sta.bits.position >= WS2812_LED_NUMBER )
			{
				sg_ws2812_sacn_sta.bits.position = 0;
				if ( ++sg_ws2812_sacn_sta.bits.index >= WS2812_COLOR_MAX )
				{
					sg_ws2812_sacn_sta.bits.index = 0;
				}
			}
		}

		ws2812_pwm_dma_wave_download(index);
	}
}

void ws2812_light_on_colorful(void)
{
	#define WS2812_COLORFULL_ON_TIMES  3

	unsigned char k = 0;
	unsigned short index = 2;

	if ( clock_time_exceed(sg_ws2812_timer, 300*1000) )
	{
		for ( k = 0; k < WS2812_LED_NUMBER; k++)
		{
			ws2812_push_color( &index, sg_color_ws2812_list[ rand() % WS2812_COLOR_MAX ] );
		}

		sg_ws2812_timer = clock_time();

		ws2812_pwm_dma_wave_download(index);
	}
}



void ws2812_light_on_breath(void)
{
	unsigned char k = 0;
	unsigned int  temp_cc = 0;
	unsigned short index = 2;

	if ( clock_time_exceed(sg_ws2812_timer, 10*1000) )
	{
		sg_ws2812_timer = clock_time();

		if ( 0 == sg_ws2812_sacn_sta.bits.open )
		{
			sg_ws2812_breath_value++;

			if ( sg_ws2812_breath_value >= 250 )
			{
				sg_ws2812_sacn_sta.bits.open = 1;
			}
		}
		else
		{
			sg_ws2812_breath_value--;

			if ( sg_ws2812_breath_value <= 0 )
			{
				sg_ws2812_sacn_sta.bits.open = 0;

				if ( ++sg_ws2812_sacn_sta.bits.index >= WS2812_COLOR_MAX )
				{
					sg_ws2812_sacn_sta.bits.index = 0;
				}

				return;
			}
		}

		temp_cc = sg_ws2812_breath_value;

		switch ( sg_ws2812_sacn_sta.bits.index )
		{
			case 0:  temp_cc <<= 8;  break;

			case 1:	 temp_cc <<= 16; break;

			case 2:	 break;

			case 3:	temp_cc <<= 8; 	temp_cc |= sg_ws2812_breath_value;  temp_cc <<= 8; break;

			case 4: temp_cc <<= 16; temp_cc |= sg_ws2812_breath_value;  break;

			case 5: temp_cc <<= 8; 	temp_cc |= sg_ws2812_breath_value;  break;

			case 6: temp_cc <<= 8; 	temp_cc |= sg_ws2812_breath_value;  temp_cc <<= 8;  temp_cc |= sg_ws2812_breath_value;  break;

			default: break;
		}

		for ( k = 0; k < WS2812_LED_NUMBER; k++)
		{
			ws2812_push_color( &index, temp_cc );
		}

		ws2812_pwm_dma_wave_download(index);
	}
}


void ws2812_code_loop_pro(void)
{
#if (PROJECT_ID == PID_104)
	static unsigned int gc_ws2812_led_delay_time = 0;
	static unsigned char gc_ws2812_led_start_flag = 0;
#endif

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

		#if LED_CODE_PWIR_DRIVE_ENABLE
			ws2812_sleep_close();
		#else
			ws2812_sleep_close();
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
			if ( sg_ws2812_sacn_sta.bits.power_off )
			{
				LED_CODE_POWER_ON();
				sg_ws2812_sacn_sta.bits.power_off = 0;
			}
		}

		if ( idle_count < CODE_LED_ON_TIME_MAX && mcu_sleep_status.enter_lower )
		{
			if ( ++mcu_sleep_status.resume_cnt >= 5 )
			{
				mcu_sleep_status.resume_cnt  = 0;
				mcu_sleep_status.enter_lower = 0;
			}
		}
	}
#endif


#if (PROJECT_ID == PID_104)
	if ( (0 == connect_ok) || led_adv_sta )
	{
		gc_ws2812_led_delay_time =  clock_time();
		gc_ws2812_led_start_flag = 1;
		return;
	}

	if ( gc_ws2812_led_start_flag )
	{
		if (  clock_time_exceed(gc_ws2812_led_delay_time, 1000*1000)  )
		{
			gc_ws2812_led_delay_time =  clock_time();
			gc_ws2812_led_start_flag = 0;
		}
		return;
	}
#endif


	if ( (connect_ok) && (!flash_dev_info.code_led_sw) )
	{
		switch ( flash_dev_info.code_led_mode )
		{
			case RGB_MODE_LIGHTS_NUION: 
				 ws2812_light_on_together();
				 break;

			case RGB_MODE_LIGHTS_PUSH:		
				 ws2812_light_on_push();
				 break;

			case RGB_MODE_LIGHTS_PUSH_BACK:
				 ws2812_light_on_push_back();
				 break;

			case RGB_MODE_LIGHTS_SINGLE:	
				 ws2812_light_on_single();
				 break;

			case RGB_MODE_LIGHTS_COLORFUL: 
				 ws2812_light_on_colorful();
				 break;

			case RGB_MODE_LIGHTS_BREATH: 
				 ws2812_light_on_breath();
				 break;

			default: ws2812_light_on_colorful(); break;
		}
	}

}

#endif

#if (0)
	void ws2812_red_all_on(void)
	{
		unsigned char k = 0;
		unsigned short index = 2;

		for (k = 0; k < WS2812_LED_NUMBER; k++)
		{
			ws2812_push_color( &index, sg_color_ws2812_list[ COLOR_RED ] );
		}
		ws2812_pwm_dma_wave_download(index);
	}

	void ws2812_green_all_on(void)
	{
		unsigned char k = 0;
		unsigned short index = 2;

		for (k = 0; k < WS2812_LED_NUMBER; k++)
		{
			ws2812_push_color( &index, sg_color_ws2812_list[ COLOR_GREEN ] );
		}
		ws2812_pwm_dma_wave_download(index);
	}

	void ws2812_cyan_all_on(void)
	{
		unsigned char k = 0;
		unsigned short index = 2;

		for (k = 0; k < WS2812_LED_NUMBER; k++)
		{
			ws2812_push_color( &index, sg_color_ws2812_list[ COLOR_CYAN ] );
		}
		ws2812_pwm_dma_wave_download(index);
	}

	void ws2812_all_off(void)
	{
		unsigned char k = 0;
		unsigned short index = 2;

		for (k = 0; k < WS2812_LED_NUMBER; k++)
		{
			ws2812_push_color( &index, WS2812_BLACK );
		}
		ws2812_pwm_dma_wave_download(index);
	}

#endif


