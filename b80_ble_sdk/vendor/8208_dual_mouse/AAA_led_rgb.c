#include "AAA_led_rgb.h"

#if LED_RGB_ENABLE || DPI_RGB_SET_ENABLE
	#include "AAA_public_config.h"
	#include "AAA_LED.h"
	#include "main.h"
#endif



#if	LED_RGB_ENABLE || DPI_RGB_SET_ENABLE


void led_rgb_set_pwm(u16 r, u16 g, u16 b)
{
	pwm_set_cycle_and_duty(RGB_R_PWMID, (u16)(PWM_MAX_SCALE * CLOCK_SYS_CLOCK_1US),  (u16)(r * CLOCK_SYS_CLOCK_1US) );
	pwm_set_cycle_and_duty(RGB_G_PWMID, (u16)(PWM_MAX_SCALE * CLOCK_SYS_CLOCK_1US),  (u16)(g * CLOCK_SYS_CLOCK_1US) );
	pwm_set_cycle_and_duty(RGB_B_PWMID, (u16)(PWM_MAX_SCALE * CLOCK_SYS_CLOCK_1US),  (u16)(b * CLOCK_SYS_CLOCK_1US) );
}

void led_rgb_set_color(LED_RGB_COLOR_EN color, unsigned short duty)
{
#if RGB_ANODE_ENABLE
	switch( color )
	{
		case LED_RGB_CYAN:
			led_rgb_set_pwm(PWM_MAX_SCALE, duty, duty);
			break;

		case LED_RGB_BLUE:
			led_rgb_set_pwm(PWM_MAX_SCALE, PWM_MAX_SCALE, duty);
			break;

		case LED_RGB_PURPLE:
			led_rgb_set_pwm(duty, PWM_MAX_SCALE, duty);
			break;

		case LED_RGB_RED:
			led_rgb_set_pwm(duty, PWM_MAX_SCALE, PWM_MAX_SCALE);
			break;

		case LED_GRB_ORANGE:
			led_rgb_set_pwm(duty, duty, PWM_MAX_SCALE);
			break;

		case LED_RGB_GREEN:
			led_rgb_set_pwm(PWM_MAX_SCALE, duty, PWM_MAX_SCALE);
			break;

	#if (PROJECT_ID == PID_Q15)
		// nothing
	#else
		case LED_RGB_WHITE:
			led_rgb_set_pwm(duty, duty, duty);
			break;
	#endif

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

void led_rgb_stop(void)
{
#if RGB_ANODE_ENABLE
	led_rgb_set_pwm(PWM_MAX_SCALE, PWM_MAX_SCALE, PWM_MAX_SCALE);
#else
	led_rgb_set_pwm(PWM_INIT_DEFAULT, PWM_INIT_DEFAULT, PWM_INIT_DEFAULT);
#endif

	pwm_stop(RGB_R_PWMID);
	pwm_stop(RGB_G_PWMID);
	pwm_stop(RGB_B_PWMID);
}

#endif


#if LED_RGB_ENABLE

#define LED_RGB_ON    LED_ON
#define LED_RGB_OFF   LED_OFF

const unsigned int rgb_group_tab[LED_RGB_GROUP_NUM] = {LED_RGB_GROUP1, LED_RGB_GROUP2, LED_RGB_GROUP3, LED_RGB_GROUP4,LED_RGB_GROUP5, LED_RGB_GROUP6};
unsigned char sg_rgb_led_index = LED_RGB_TASK_BREATH;


void led_rgb_all_on(void)
{
	unsigned char i = 0;
	for (i = 0; i < LED_RGB_GROUP_NUM; i++)
	{
		gpio_write(rgb_group_tab[i], LED_RGB_ON);
	}
}

void led_rgb_all_off(void)
{
	unsigned char i = 0;
	for ( i = 0; i < LED_RGB_GROUP_NUM; i++)
	{
		gpio_write(rgb_group_tab[i], LED_RGB_OFF);
	}
}

LED_RGB_RUN_STATE_EN led_rgb_suspend(unsigned char act)
{
	static u32 sk_rgb_flow_x_tick = 0;

	if ( act )
	{
		sk_rgb_flow_x_tick = clock_time();
	}
	
	if ( clock_time_exceed(sk_rgb_flow_x_tick, 1000*1000) )
	{
		sk_rgb_flow_x_tick = clock_time();
		return (LED_RGB_PRO_SUSPEND_OVER);
	}

	return (LED_RGB_PRO_RUNING);
}

LED_RGB_RUN_STATE_EN led_rgb_breath_color(LED_RGB_TASK_TYPE_EN type, LED_RGB_COLOR_EN color)
{
	#define LED_MAX_BREATH_CYCLE  1
	#define LED_MAX_1_DUTY        200
	
	static u32 rgb_breath_tick = 0;
	static u16 rgb_breath_duty = 0;
	static u8  rgb_breath_flag = 0, rgb_breath_cycle = 0;

	if ( LED_RGB_TASK_BREATH != type )
	{
		rgb_breath_duty = 0;
		rgb_breath_flag = 0;
		rgb_breath_cycle = 0;
		rgb_breath_tick = clock_time();
		return (LED_RGB_PRO_IDEL);
	}

	if ( clock_time_exceed(rgb_breath_tick, 10*1000) )
	{
		rgb_breath_tick = clock_time();

		led_rgb_all_on();

		if ( rgb_breath_flag )
		{
			rgb_breath_duty -= 5;
			if ( rgb_breath_duty <= MIN_SCAL_CNT )
			{
				rgb_breath_flag = 0;
				rgb_breath_cycle++;
			}
		}
		else
		{
			if (rgb_breath_duty < LED_MAX_1_DUTY)
				rgb_breath_duty += 1;
			else
				rgb_breath_duty += 40;
			
			if( rgb_breath_duty >= MAX_SCAL_CNT )
			{
				rgb_breath_flag = 1;
				rgb_breath_duty = MAX_SCAL_CNT;
			}
		}

		led_rgb_set_color(color, rgb_breath_duty);
	}

	if ( rgb_breath_cycle >= LED_MAX_BREATH_CYCLE )
	{
		rgb_breath_duty = 0;
		rgb_breath_flag = 0;
		rgb_breath_cycle = 0;
		return (LED_RGB_PRO_OVER);
	}
	
	return (LED_RGB_PRO_RUNING);
}




void led_rgb_num16_init(void)
{
	unsigned char i = 0;

	for (i = 0; i < LED_RGB_GROUP_NUM; i++)
	{
		gpio_output_config(rgb_group_tab[i]);
		gpio_write(rgb_group_tab[i], LED_RGB_OFF); // close led rgb
	}

	pwm_set_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);

	gpio_set_func(LED_RGB_CTL_PWM0, PWM0);
	pwm_set_mode(PWM0_ID, PWM_NORMAL_MODE);
	pwm_set_cycle_and_duty(PWM0_ID, (unsigned short)(PWM_MAX_SCALE * CLOCK_SYS_CLOCK_1US),  (unsigned short)(100 * CLOCK_SYS_CLOCK_1US));

	gpio_set_func(LED_RGB_CTL_PWM1, PWM1);
	pwm_set_mode(PWM1_ID, PWM_NORMAL_MODE);
	pwm_set_cycle_and_duty(PWM1_ID, (unsigned short)(PWM_MAX_SCALE * CLOCK_SYS_CLOCK_1US),  (unsigned short)(100 * CLOCK_SYS_CLOCK_1US));

	gpio_set_func(LED_RGB_CTL_PWM2, PWM2);
	pwm_set_mode(PWM2_ID, PWM_NORMAL_MODE);
	pwm_set_cycle_and_duty(PWM2_ID, (unsigned short)(PWM_MAX_SCALE * CLOCK_SYS_CLOCK_1US),  (unsigned short)(100 * CLOCK_SYS_CLOCK_1US));

	pwm_start(PWM0_ID);
	pwm_start(PWM1_ID); 
	pwm_start(PWM2_ID); 
}

LED_RGB_RUN_STATE_EN led_rgb_breath_color(LED_RGB_TASK_TYPE_EN type, LED_RGB_COLOR_EN color)
{
	#define LED_MAX_BREATH_CYCLE  1
	#define LED_MAX_1_DUTY        200
	
	static u32 rgb_breath_tick = 0;
	static u16 rgb_breath_duty = 0;
	static u8  rgb_breath_flag = 0, rgb_breath_cycle = 0;

	if ( LED_RGB_TASK_BREATH != type )
	{
		rgb_breath_duty = 0;
		rgb_breath_flag = 0;
		rgb_breath_cycle = 0;
		rgb_breath_tick = clock_time();
		return (LED_RGB_PRO_IDEL);
	}

	if ( clock_time_exceed(rgb_breath_tick, 10*1000) )
	{
		rgb_breath_tick = clock_time();

		led_rgb_all_on();

		if ( rgb_breath_flag )
		{
			rgb_breath_duty -= 5;
			if ( rgb_breath_duty <= MIN_SCAL_CNT )
			{
				rgb_breath_flag = 0;
				rgb_breath_cycle++;
			}
		}
		else
		{
			if (rgb_breath_duty < LED_MAX_1_DUTY)
				rgb_breath_duty += 1;
			else
				rgb_breath_duty += 40;
			
			if( rgb_breath_duty >= MAX_SCAL_CNT )
			{
				rgb_breath_flag = 1;
				rgb_breath_duty = MAX_SCAL_CNT;
			}
		}

		led_rgb_set_color(color, rgb_breath_duty);
	}

	if ( rgb_breath_cycle >= LED_MAX_BREATH_CYCLE )
	{
		rgb_breath_duty = 0;
		rgb_breath_flag = 0;
		rgb_breath_cycle = 0;
		return (LED_RGB_PRO_OVER);
	}
	
	return (LED_RGB_PRO_RUNING);
}

LED_RGB_RUN_STATE_EN led_rgb_flow_1(LED_RGB_TASK_TYPE_EN type, LED_RGB_COLOR_EN color)
{
	#define LED_MAX_INDEX_1_ON	 3
	#define LED_MAX_INDEX_1_OFF  6

	const unsigned int flow_1_tab[] = {
		LED_RGB_GROUP1, LED_RGB_GROUP2, LED_RGB_GROUP3,
		LED_RGB_GROUP3, LED_RGB_GROUP2, LED_RGB_GROUP1 };

	static u32 rgb_flow_1_tick = 0;
	static u8  rgb_index_1 = 0; 
	u8 flag1 = 0;
	
	if ( LED_RGB_TASK_FLOW_1 != type )
	{
		rgb_index_1 = 0;
		rgb_flow_1_tick = clock_time();
		return (LED_RGB_PRO_IDEL);
	}

	if ( clock_time_exceed(rgb_flow_1_tick, 200*1000) )
	{
		rgb_flow_1_tick = clock_time();
		
		if ( rgb_index_1 < LED_MAX_INDEX_1_ON ) {
			gpio_write(flow_1_tab[rgb_index_1], LED_RGB_ON);
		}
		else {
			gpio_write(flow_1_tab[rgb_index_1], LED_RGB_OFF);
		}
		
		if ( ++rgb_index_1 >= LED_MAX_INDEX_1_OFF )
		{
			rgb_index_1 = 0;
			flag1 = 1;
		}
		led_rgb_set_color(color, PWM_DEFAULT_DUTY);
	}

	if ( flag1 )
	{
		return (LED_RGB_PRO_OVER);
	}
	
	return (LED_RGB_PRO_RUNING);
}

LED_RGB_RUN_STATE_EN led_rgb_flow_2(LED_RGB_TASK_TYPE_EN type, LED_RGB_COLOR_EN color)
{
	#define LED_MAX_INDEX_2_ON   3
	#define LED_MAX_INDEX_2_OFF  6

	const unsigned int flow_2_tab[] = {
		LED_RGB_GROUP1, LED_RGB_GROUP2, LED_RGB_GROUP3,
		LED_RGB_GROUP3, LED_RGB_GROUP2, LED_RGB_GROUP1 };

	static u32 rgb_flow_2_tick = 0;

	static u8  rgb_index_2 = 0; 
	
	u8 flag2 = 0, i = 0;
	
	if ( LED_RGB_TASK_FLOW_2 != type )
	{
		rgb_index_2 = 0;
		rgb_flow_2_tick = clock_time();
		return (LED_RGB_PRO_IDEL);
	}

	if ( clock_time_exceed(rgb_flow_2_tick, 200*1000) )
	{
		rgb_flow_2_tick = clock_time();

		if ( rgb_index_2 < LED_MAX_INDEX_2_ON )
		{
			for ( i = 0; i < LED_MAX_INDEX_2_ON; i++ )
			{
				if( i == rgb_index_2 ) {
					gpio_write(flow_2_tab[i], LED_RGB_ON);
				}
				else {
					gpio_write(flow_2_tab[i], LED_RGB_OFF);
				}
			}
		}
		else
		{
			for ( i = rgb_index_2; i < LED_MAX_INDEX_2_OFF; i++ )
			{
				if( i == rgb_index_2 ) {
					gpio_write(flow_2_tab[i], LED_RGB_ON);
				}
				else {
					gpio_write(flow_2_tab[i], LED_RGB_OFF);
				}
			}
		}

		if ( ++rgb_index_2 >= LED_MAX_INDEX_2_OFF )
		{
			flag2 = 1;
			rgb_index_2 = 0;
		}
		
		led_rgb_set_color(color, PWM_DEFAULT_DUTY);
	}

	if ( 1 == flag2 )
	{
		return (LED_RGB_PRO_OVER);
	}
	
	return (LED_RGB_PRO_RUNING);
}

LED_RGB_RUN_STATE_EN led_rgb_flow_3(LED_RGB_TASK_TYPE_EN type, LED_RGB_COLOR_EN color)
{
	#define LED_MAX_INDEX_3_ON   10

	static u32 sk_rgb_flow_3_tick = 0;
	static u8  sk_rgb_index_3 = 0;

	u8 flag3 = 0, i = 0;
	
	if ( LED_RGB_TASK_FLOW_3 != type )
	{
		sk_rgb_index_3 = 0;
		sk_rgb_flow_3_tick  = clock_time();
		return (LED_RGB_PRO_IDEL);
	}

	if ( clock_time_exceed(sk_rgb_flow_3_tick, 200*1000) )
	{
		sk_rgb_flow_3_tick = clock_time();
		for ( i = 0; i < LED_MAX_INDEX_3_ON; i++ )
		{
			if ( i != sk_rgb_index_3 ) {
				gpio_write(rgb_group_tab[i], LED_RGB_ON);
			}
			else {
				gpio_write(rgb_group_tab[i], LED_RGB_OFF);
			}
		}
		led_rgb_set_color(color, PWM_DEFAULT_DUTY);
		
		if (++sk_rgb_index_3 >= LED_MAX_INDEX_3_ON)
		{
			sk_rgb_index_3 = 0;
			flag3 = 1;
		}
	}

	if ( flag3 )
	{
		return (LED_RGB_PRO_OVER);
	}
	
	return (LED_RGB_PRO_RUNING);
}

LED_RGB_RUN_STATE_EN led_rgb_flow_4(LED_RGB_TASK_TYPE_EN type, LED_RGB_COLOR_EN color)
{
	#define LED_MAX_INDEX_4_ON   10

	static u32 sk_rgb_flow_4_tick = 0;
	static u8  sk_rgb_index_4 = 0;

	u8 flag4 = 0, i = 0;
	
	if ( LED_RGB_TASK_FLOW_4 != type )
	{
		sk_rgb_index_4 = 0;
		sk_rgb_flow_4_tick  = clock_time();
		return (LED_RGB_PRO_IDEL);
	}

	if ( clock_time_exceed(sk_rgb_flow_4_tick, 600*1000) )
	{
		sk_rgb_flow_4_tick = clock_time();
		for ( i = 0; i < LED_MAX_INDEX_4_ON; i++ )
		{
			if ( i != sk_rgb_index_4 ) {
				gpio_write(rgb_group_tab[i], LED_RGB_OFF);
			}
			else {
				gpio_write(rgb_group_tab[i], LED_RGB_ON);
			}
		}
		led_rgb_set_color(color, PWM_DEFAULT_DUTY);
		
		if (++sk_rgb_index_4 >= LED_MAX_INDEX_4_ON)
		{
			sk_rgb_index_4 = 0;
			flag4 = 1;
		}
	}

	if ( flag4 )
	{
		return (LED_RGB_PRO_OVER);
	}
	
	return (LED_RGB_PRO_RUNING);
}

LED_RGB_RUN_STATE_EN led_rgb_flow_5(LED_RGB_TASK_TYPE_EN type, LED_RGB_COLOR_EN color)
{	
	static u32 sk_rgb_flow_5_tick = 0;
	static u8  sk_rgb_index_5 = 0;

	u8 flag5 = 0;
	
	if ( LED_RGB_TASK_FLOW_5 != type )
	{
		sk_rgb_index_5 = 0;
		sk_rgb_flow_5_tick  = clock_time();
		return (LED_RGB_PRO_IDEL);
	}

	if ( clock_time_exceed(sk_rgb_flow_5_tick, 800*1000) )
	{
		sk_rgb_flow_5_tick = clock_time();
		
		if ( 0 == sk_rgb_index_5 ) 
		{
			led_rgb_all_on();
			sk_rgb_index_5 = 1;
		}
		else 
		{
			led_rgb_all_off();
			sk_rgb_index_5 = 0;
			flag5 = 1;
		}
		
		led_rgb_set_color(color, PWM_DEFAULT_DUTY);
	}

	if ( flag5 )
	{
		return (LED_RGB_PRO_OVER);
	}
	
	return (LED_RGB_PRO_RUNING);
}

void led_rgb_status_clear(void)
{
	led_rgb_breath_color(LED_RGB_TASK_NONE, LED_RGB_CYAN);
	led_rgb_flow_1(LED_RGB_TASK_NONE, LED_RGB_CYAN);
	led_rgb_flow_2(LED_RGB_TASK_NONE, LED_RGB_CYAN);
	led_rgb_flow_3(LED_RGB_TASK_NONE, LED_RGB_CYAN);
	led_rgb_flow_4(LED_RGB_TASK_NONE, LED_RGB_CYAN);
	led_rgb_flow_5(LED_RGB_TASK_NONE, LED_RGB_CYAN);
}


void led_rgb_pro(void)
{
	static unsigned char sk_led_color = LED_RGB_GREEN;
	LED_RGB_RUN_STATE_EN value = 0;

	if ( LED_RGB_TASK_NONE == sg_rgb_led_index || sg_rgb_led_index >= LED_RGB_TASK_MAX )
		sg_rgb_led_index = LED_RGB_TASK_BREATH;

	if ( 0 == connect_ok )
	{
		led_rgb_all_off();
		return;
	}
	
	if (idle_count >= LED_RGB_RUN_TIME + 2 ) 
	{
		mcu_sleep_status.mouse_enter_lower = 1;
	}
	else 
	{
		if ( mcu_sleep_status.mouse_moved || mcu_sleep_status.mouse_resume_cnt )
		{
			pwm_start(PWM0_ID);
			pwm_start(PWM1_ID);
			pwm_start(PWM2_ID);
			mcu_sleep_status.mouse_resume_cnt++;
			mcu_sleep_status.mouse_enter_lower = 0;
			mcu_sleep_status.mouse_moved = 0;
		}

		if ( mcu_sleep_status.mouse_resume_cnt >= 5 )
		{
			mcu_sleep_status.mouse_resume_cnt = 0;
		}
	}

	if ( mcu_sleep_status.mouse_enter_lower )
	{
		led_rgb_set_pwm(PWM_INIT_DEFAULT,PWM_INIT_DEFAULT,PWM_INIT_DEFAULT);
		led_rgb_status_clear();
		return;
	}

	if ( connect_ok && LED_RGB_SWITCH_ON() )
	{
		switch ( sg_rgb_led_index )
		{
			case LED_RGB_TASK_BREATH : 
				value = led_rgb_breath_color(LED_RGB_TASK_BREATH, sk_led_color);
				break;
			
			case LED_RGB_TASK_FLOW_1 : 
				value = led_rgb_flow_1(LED_RGB_TASK_FLOW_1, sk_led_color);
				break;

			case LED_RGB_TASK_FLOW_2 : 
				value = led_rgb_flow_2(LED_RGB_TASK_FLOW_2, sk_led_color);
				break;

			case LED_RGB_TASK_FLOW_3 : 
				value = led_rgb_flow_3(LED_RGB_TASK_FLOW_3, sk_led_color);
				break;

			case LED_RGB_TASK_FLOW_4 : 
				value = led_rgb_flow_4(LED_RGB_TASK_FLOW_4, sk_led_color);
				break;

			case LED_RGB_TASK_FLOW_5 : 
				value = led_rgb_flow_5(LED_RGB_TASK_FLOW_5, sk_led_color);
				break;
			
			case LED_RGB_TASK_SUSPEND : 
				value = led_rgb_suspend(0);
				break;
			
			default: break;
		}
			
		if ( LED_RGB_PRO_OVER == value )
		{
			sk_led_color = rand() % LED_REB_COLOR_MAX;
			
			if ( ++sg_rgb_led_index >= LED_RGB_TASK_MAX ) 
			{
				sg_rgb_led_index = LED_RGB_TASK_BREATH;
			}
			analog_write(DEEP_ANA_REG7, sg_rgb_led_index);
			led_rgb_status_clear();
			
			if ( (LED_RGB_TASK_BREATH == sg_rgb_led_index) && (LED_RGB_PRO_OVER == value) )
			{
				sg_rgb_led_index = LED_RGB_TASK_SUSPEND;
				led_rgb_suspend(1);
			}
			
			printf("current color = %d\n", sk_led_color);
		}

		if( (LED_RGB_TASK_SUSPEND == sg_rgb_led_index) && (LED_RGB_PRO_SUSPEND_OVER == value) )
		{
			sg_rgb_led_index = LED_RGB_TASK_BREATH;
		}
	}
	else
	{
		led_rgb_status_clear();
		led_rgb_all_off();
	}
}

#endif


#if DPI_RGB_SET_ENABLE

extern u8 dpi_value;
extern u8 gl_cpi_change_flag;

static u8  rgb_breath_cycle = 0, rgb_breath_init_flag = 0;

void dpi_rgb_port_init(void)
{
	pwm_set_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);

	gpio_set_func(DPI_RGB_R_PIN, RGB_R_IO_FUN);
	pwm_set_mode(RGB_R_PWMID, 	 PWM_NORMAL_MODE);
	pwm_set_cycle_and_duty(RGB_R_PWMID, (unsigned short)(PWM_MAX_SCALE * CLOCK_SYS_CLOCK_1US),  (unsigned short)(PWM_MAX_SCALE * CLOCK_SYS_CLOCK_1US));

	gpio_set_func(DPI_RGB_G_PIN, RGB_G_IO_FUN);
	pwm_set_mode(RGB_G_PWMID,    PWM_NORMAL_MODE);
	pwm_set_cycle_and_duty(RGB_G_PWMID, (unsigned short)(PWM_MAX_SCALE * CLOCK_SYS_CLOCK_1US),  (unsigned short)(PWM_MAX_SCALE * CLOCK_SYS_CLOCK_1US));

	gpio_set_func(DPI_RGB_B_PIN, RGB_B_IO_FUN);
	pwm_set_mode(RGB_B_PWMID,    PWM_NORMAL_MODE);
	pwm_set_cycle_and_duty(RGB_B_PWMID, (unsigned short)(PWM_MAX_SCALE * CLOCK_SYS_CLOCK_1US),  (unsigned short)(PWM_MAX_SCALE * CLOCK_SYS_CLOCK_1US));

	pwm_start(RGB_R_PWMID);
	pwm_start(RGB_G_PWMID);
	pwm_start(RGB_B_PWMID);

	rgb_breath_init_flag = 0;
}

void dpi_rgb_start(void)
{
	rgb_breath_cycle = 0;
}

void dpi_rgb_display(void)
{
	#define LED_MAX_BREATH_CYCLE  3
	#define LED_MAX_1_DUTY        500
	#define LED_MIN_2_DUTY        2

	static u32 rgb_breath_tick = 0;
	static u16 rgb_breath_duty = PWM_MAX_SCALE;
	static u8  rgb_breath_flag = 1;

	if ( 0 == gl_cpi_change_flag )
	{
		rgb_breath_tick = clock_time();
		return;
	}

	if ( 3 == rgb_breath_flag )
	{
		if ( clock_time_exceed(rgb_breath_tick, 800*1000) )
		{
			rgb_breath_tick = clock_time();
			rgb_breath_flag = 1;
		}
		return;
	}

	if ( clock_time_exceed(rgb_breath_tick, 10*1000) )
	{
		rgb_breath_tick = clock_time();

		if ( 1 == rgb_breath_flag )
		{
			if (rgb_breath_duty < LED_MAX_1_DUTY)
				rgb_breath_duty -= 5;
			else
				rgb_breath_duty -= 35; // 20

			if ( rgb_breath_duty <= LED_MIN_2_DUTY )
			{
				rgb_breath_flag = 2;
				rgb_breath_duty = LED_MIN_2_DUTY;
			}
		}
		else
		{
			rgb_breath_duty += 20;
			if ( rgb_breath_duty >= PWM_MAX_SCALE )
			{
				rgb_breath_flag = 3;
				rgb_breath_duty = PWM_MAX_SCALE;
				rgb_breath_cycle++;
			}
		}

		led_rgb_set_color(dpi_value, rgb_breath_duty);
	}

	if ( rgb_breath_cycle >= LED_MAX_BREATH_CYCLE )
	{
		rgb_breath_duty = PWM_MAX_SCALE;
		gl_cpi_change_flag = 0;
		rgb_breath_flag = 1;
		rgb_breath_cycle = 0;
	}

}

#endif
