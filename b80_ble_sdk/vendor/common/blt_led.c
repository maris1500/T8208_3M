/********************************************************************************************************
 * @file     blt_led.c
 *
 * @brief    This is the source file for BLE SDK
 *
 * @author	 BLE GROUP
 * @date         12,2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/
#include "tl_common.h"
#include "drivers.h"
#include "../common/blt_led.h"
#include "../8208_dual_mouse/AAA_public_Config.h"
#include "../module/AAA_digital_tube.h"

device_led_t device_led;


#if (0)
	extern void ws2812_red_all_on(void);
	extern void ws2812_green_all_on(void);
	extern void ws2812_cyan_all_on(void);
	extern void ws2812_all_off(void);
#endif


void gpio_input_config(GPIO_PinTypeDef pin)
{
    gpio_set_func(pin, AS_GPIO);
    gpio_set_input_en(pin, 1);
    gpio_set_output_en(pin,0);
    gpio_setup_up_down_resistor(pin, PM_PIN_PULLUP_10K);
}

void gpio_input_config_status(GPIO_PinTypeDef pin, GPIO_PullTypeDef staus)
{
    gpio_set_func(pin, AS_GPIO);
    gpio_set_input_en(pin, 1);
    gpio_set_output_en(pin,0);
    gpio_setup_up_down_resistor(pin, staus);
}

void gpio_output_config(GPIO_PinTypeDef pin)
{
    gpio_set_func(pin, AS_GPIO);
    gpio_set_input_en(pin, 0);
    gpio_set_output_en(pin,1);
}

void gpio_output_config_status(GPIO_PinTypeDef pin, unsigned char led_sta)
{
    gpio_set_func(pin, AS_GPIO);
    gpio_set_input_en(pin, 0);
    gpio_set_output_en(pin,1);
    gpio_write( pin, led_sta );
}

void gpio_higsesistance_config(GPIO_PinTypeDef pin)
{
    gpio_shutdown(pin);
    gpio_set_func(pin, AS_GPIO);
    gpio_set_input_en(pin, 0);
    gpio_set_output_en(pin,0);
    gpio_setup_up_down_resistor(pin, PM_PIN_UP_DOWN_FLOAT);
}

#if MODULE_DIGITAL_TUBE_ENABLE
	void digital_tube_io_out(GPIO_PinTypeDef pin, unsigned char led_sta)
	{
		gpio_set_output_en(pin, 1);
		gpio_write( pin, led_sta );
	}
#endif

void device_led_on_off(u8 on)
{
	if ( on == LED_ON )
	{
	#if LED_MODE_MIX_CTRL_ENABLE
		gpio_write( device_led.gpio_led, on^device_led.polar );
		gpio_set_output_en(device_led.gpio_led, on);
	#elif LED_RGB_BAT_DPI_ENABLE
		if ( RF_2M_2P4G_MODE == mcu_mode_get() )
		{
			LED_DPI_BAT_R_ACT( LED_RGB_DPI_BAT_ON );
		}
		else
		{
		#if (PROJECT_ID == PID_XT27)
			LED_DPI_BAT_B_ACT( LED_RGB_DPI_BAT_ON );
		#else
			if ( BLE_DEVICE_ID_0 == ble_mode_device_id_get() )
			{
				LED_DPI_BAT_B_ACT( LED_RGB_DPI_BAT_ON );
			}
			else
			{
				LED_DPI_BAT_R_ACT( LED_RGB_DPI_BAT_ON );
				LED_DPI_BAT_B_ACT( LED_RGB_DPI_BAT_ON );
			}
		#endif
		}
	#elif (PROJECT_ID == PID_NR300) || (PROJECT_ID == PID_535)
		if ( RF_2M_2P4G_MODE == mcu_mode_get() )
		{
			gpio_write( device_led.gpio_led, LED_OFF );
		#if (PROJECT_ID == PID_535)
			gpio_write( PIN_BLE1_LED, LED_ON );
			gpio_write( PIN_BLE2_LED, LED_ON );
		#endif
		}
		else
		{
			if ( BLE_DEVICE_ID_0 == ble_mode_device_id_get() )
			{
				gpio_write( PIN_BLE1_LED, LED_OFF );
			#if (PROJECT_ID == PID_535)
				gpio_write( PIN_24G_LED, LED_ON );
				gpio_write( PIN_BLE2_LED, LED_ON );
			#endif
			}
			else
			{
			#if (PROJECT_ID == PID_535)
				gpio_write( PIN_24G_LED,  LED_ON );
				gpio_write( PIN_BLE1_LED, LED_ON );

				gpio_write( PIN_BLE2_LED, LED_OFF );
			#else
				gpio_write( PIN_BLE1_LED, LED_OFF );
				gpio_write( PIN_24G_LED, LED_OFF );
			#endif
			}
		}
	#elif (PROJECT_ID == PID_4027)
		// do nothing
	#elif (PROJECT_ID == PID_104)
		if ( RF_2M_2P4G_MODE == mcu_mode_get() )
		{
			// ws2812_red_all_on();
		}
		else
		{
			if ( BLE_DEVICE_ID_0 == ble_mode_device_id_get() )
			{
				// ws2812_green_all_on();
			}
			else
			{
				// ws2812_cyan_all_on();
			}
		}
	#elif (PROJECT_ID == PID_S600)
		gpio_write( device_led.gpio_led, LED_OFF );
	#else
		gpio_write( device_led.gpio_led, LED_ON );
	#endif
	}
	else if (on == LED_OFF )
	{
	#if LED_MODE_MIX_CTRL_ENABLE
		gpio_higsesistance_config( device_led.gpio_led );
	#elif LED_RGB_BAT_DPI_ENABLE
		LED_DPI_BAT_R_ACT( LED_RGB_DPI_BAT_OFF );
		LED_DPI_BAT_B_ACT( LED_RGB_DPI_BAT_OFF );
	#elif (PROJECT_ID == PID_NR300)
		if ( RF_2M_2P4G_MODE == mcu_mode_get() )
		{
			gpio_write( device_led.gpio_led, LED_ON );
		}
		else
		{
			if ( BLE_DEVICE_ID_0 == ble_mode_device_id_get() )
			{
				gpio_write( PIN_BLE1_LED, LED_ON );
			}
			else
			{
				gpio_write( device_led.gpio_led, LED_ON );
				gpio_write( PIN_24G_LED, LED_ON );
			}
		}
	#elif (PROJECT_ID == PID_535)
		gpio_write( PIN_24G_LED,  LED_ON );
		gpio_write( PIN_BLE1_LED, LED_ON );
		gpio_write( PIN_BLE2_LED, LED_ON );
	#elif (PROJECT_ID == PID_4027)
		// do nothing
	#elif (PROJECT_ID == PID_104)
		// ws2812_all_off();
	#elif (PROJECT_ID == PID_S600)
		gpio_write( device_led.gpio_led, LED_ON );
	#else
		gpio_write( device_led.gpio_led, LED_OFF );
	#endif
	}

	device_led.isOn = on;
}


void device_led_init(u32 gpio,u8 polarity) {  //polarity: 1 for high led on, 0 for low led on

	device_led.gpio_led = gpio;
	device_led.polar    = !polarity;

#if LED_MODE_MIX_CTRL_ENABLE
    gpio_set_func(device_led.gpio_led,     AS_GPIO);
    gpio_set_input_en(device_led.gpio_led, 0);
    gpio_set_output_en(device_led.gpio_led,0);

	#if LED_DEVICE_POLAR_ENABLE
    	device_led.polar = !polarity;
	#else
    	device_led.polar = polarity;
	#endif
#else
    // do nothing
#endif

    device_led_on_off(0);
}

void device_led_struct_clear(void)
{
	device_led.repeatCount = 0;
	device_led.priority = 0;
}

int device_led_setup(led_cfg_t led_cfg)
{
	if ( device_led.repeatCount &&  device_led.priority >= led_cfg.priority)
	{
		return 0; //new led event priority not higher than the not ongoing one
	}
	else
	{
		device_led.onTime_ms   = led_cfg.onTime_ms;
		device_led.offTime_ms  = led_cfg.offTime_ms;
		device_led.repeatCount = led_cfg.repeatCount;
		device_led.priority = led_cfg.priority;

		if ( led_cfg.repeatCount == 0xff ) //for long on/long off
		{ 
			device_led.repeatCount = 0;
		}
		else //process one of on/off Time is zero situation
		{ 
			if ( !device_led.onTime_ms ) // onTime is zero
			{
				device_led.offTime_ms *= device_led.repeatCount;
				device_led.repeatCount = 1;
			}
			else if ( !device_led.offTime_ms )
			{
				device_led.onTime_ms *= device_led.repeatCount;
				device_led.repeatCount = 1;
			}
		}
		
		device_led.startTick = clock_time();
		device_led_on_off(device_led.onTime_ms ? 1 : 0);
		
		return 1;
	}
}


void led_proc(void)
{
	if ( device_led.isOn )
	{
		if ( clock_time_exceed(device_led.startTick, (device_led.onTime_ms)*1000) )
		{
			device_led_on_off(0);
			if ( device_led.offTime_ms ) //offTime not zero
			{
				device_led.startTick += (device_led.onTime_ms * CLOCK_16M_SYS_TIMER_CLK_1MS);
			}
			else
			{
				device_led.repeatCount = 0;
			}
		}
	}
	else
	{
		if ( clock_time_exceed(device_led.startTick, (device_led.offTime_ms)*1000) )
		{
			if ( --device_led.repeatCount )
			{
				device_led_on_off(1);
				device_led.startTick += (device_led.offTime_ms*CLOCK_16M_SYS_TIMER_CLK_1MS);
			}
		}
	}

#if (PROJECT_ID == PID_104)
	if ( 0 == device_led.repeatCount )
	{
		if ( LED_ADV_SUCC == led_adv_sta )
		{
			led_adv_sta = LED_ADV_OFF;
		}
	}
#endif

}


