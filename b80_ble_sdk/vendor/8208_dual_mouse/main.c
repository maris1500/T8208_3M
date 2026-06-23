/********************************************************************************************************
 * @file     main.c
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
#include "AAA_public_config.h"
#include "AAA_LED.h"
#include "main.h"
#include "AAA_led_rgb.h"

#if	MODULE_OLED_ENABLE
	extern void oled_display(void);
#endif

#if USB_MODE_ENABLE
	extern u8 usb_has_judge;
	extern void usb_main_loop(void);
	extern void usb_user_init(void);
#endif

#if (IIC_DRIVE_ENABLE)
	#include "AAA_iic.h"
#endif

#if (MODULE_DIGITAL_TUBE_ENABLE)
	#include "../module/AAA_digital_tube.h"
#endif

#if LED_LOGO_ACTION_ENABLE
 	extern void led_logo_action(void);
 #endif

#if (PROJECT_ID == PID_601)
 	extern void protect_led_ram_on(void);
#endif

#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_FX282) || (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_DMS157) || VOLTAGE_TEMP_HANDLE_ENABLE
 	unsigned char gc_vol_cap_reg_temp = 0;
#endif

#if (PROJECT_ID == PID_M388)
	extern void batt_dpi_two_loop(void);
#endif

#if WEB_HID_ENABLE
	 web_data_cfg_t gc_web_data;
#endif

mouse_sta_t gc_mouse_sta;

#if (PM_SYS_LOW_POWER_ENABLE)
	mcu_pm_flag_t  mcu_sleep_status;
#endif

#if (MODULE_DIGITAL_TUBE_ENABLE)
	extern void digital_tube_pro(void);
#endif

#if (KEY_FEATURE_DESKTOP_ENABLE)
	extern unsigned char sk_m115_logo_led_on_flag;
	extern void key_homekey_judement(void);
#endif

#if (PROJECT_ID == PID_535)
	extern void vbat_lower_loop_proc(void);
#endif

#if (PROJECT_ID == PID_M0018)
	extern void led_connect_none_status_clear(void);
#endif

#if (MODULE_MCU_EXTERNAL_ENABLE)
	extern void ext_mcu_pro(void);
#endif

#if LED_COLOR_LIGHTS_ENABLE
	extern void led_color_logo_action(void);
#endif

#if OTA_ENABLE_AAA
	extern unsigned char run_app_code(void);
#endif

#if (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
	extern void ext_mcu_prepare(void);
#endif

#if (PROJECT_ID == PID_MS13) || (PROJECT_ID == PID_8693) || (PROJECT_ID == PID_HM660) || (PROJECT_ID == PID_DMS06) || MOUSE_REPORT_250HZ_ENABLE
	_attribute_data_retention_user u8 report_rate = 4;	//2.4G reporting rate 8--250hz
#elif MOUSE_REPORT_500HZ_ENABLE
	_attribute_data_retention_user u8 report_rate = 2;
#elif MOUSE_REPORT_20HZ_ENABLE
	_attribute_data_retention_user u8 report_rate = 50;
#else
	_attribute_data_retention_user u8 report_rate = 8;	//2.4G reporting rate 8--125hz
#endif

_attribute_data_retention_user int dev_info_idx;	//The offset value of the location where the data is stored

#if WEB_HID_ENABLE
	int dev_web_info_idex = 0;
#endif

#if (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_M45) || (PROJECT_ID == PID_104) || (PROJECT_ID == PID_S600)
	#if LED_CODE_PWIR_DRIVE_ENABLE
		extern void ws2812_code_loop_pro(void);
	#else
		extern void rgb_code_loop_pro(void);
	#endif
#endif

#if (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_104)  || KEY_PAIR_USED_POWERUP_ENABLE
	unsigned char gc_hm668_mode_key_used = 0;
	unsigned char gc_hm668_battery_try_times = 0;
#endif

#if (PROJECT_ID == PID_M45)
	unsigned char gc_hm668_mode_key_used = 0;
#endif


#if 0
_attribute_data_retention_user u8 pub_key[16] =
{
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
};
#endif

#if SHOW_MAST_REAL_MAC_DEBUG
_attribute_data_retention_user OUTPUT_DEV_INFO_AAA output_dev_info =
{
    0,//u32 bin_crc;
    {2, 4, 4}, //u8
    0,//u8 sensor_type;
    0,//u8 sensor_pd1;
    0,//u8 sensor_pd2;
    0,//u8 sensor_pd3;
    MOUSE_DATA_LEN_AAA,//u8 ms_data_len;
};
#endif

#if SWITCH_MODE_ENABLE
	unsigned char g_mode_io_status = 0;
#endif

#if LED_DPI_VBAT_DIFF_ENABLE
	led_differ_en sc_bat_dpi_flag = LED_DIFF_NONE;
#endif

#if (WHEEL_METHOD_2_ENABLE)
	volatile char sg_wheel_aa_value  = 0;
#endif

#if (LED_BATT_CAP_LEVEL_ENABLE)
	extern void led_cap_level_indicate(void);
#endif

unsigned int digital_diff_value(unsigned int digit1, unsigned int digit2)
{
	unsigned int temp = 0;
	
	if ( digit1 > digit2 )
		temp = digit1 - digit2;
	else
		temp = digit2 - digit1;
	
	temp = temp / 1000;
	
	return (temp);
}
/**
 * @brief   IRQ handler
 * @param   none.
 * @return  none.
 */

#if (WHEEL_METHOD_2_ENABLE)
	static volatile unsigned int  sg_wheel_aa_times = 0, sg_wheel_bb_times = 0;
	static volatile wheel_action_dir_en sg_wheel_ab_status = WHEEL_ACTION_DIR_AA;
	unsigned int sg_printf_wheel_time = 0;
#endif

_attribute_ram_code_ void irq_handler(void)
{
#if G24_MODE_ENABLE
    if (fun_mode == RF_2M_2P4G_MODE)
        irq_handle_private_2m(); //2.4G rf receiving and sending processing
    else
#endif
        blc_sdk_irq_handler(); //BLE rf receiving and sending processing

#if LED_CODE_PWIR_DRIVE_ENABLE
	if ( pwm_get_interrupt_status(PWM_IRQ_PWM0_IR_DMA_FIFO_DONE) )
	{
		 pwm_clear_interrupt_status(PWM_IRQ_PWM0_IR_DMA_FIFO_DONE);
    }
#endif
}

#if (WHEEL_METHOD_2_ENABLE)
char wheel_wheel_check(void)
{
	char sg_wheel_temp = 0;
	if ( connect_ok )
	{
		sg_wheel_temp = sg_wheel_aa_value;
		sg_wheel_aa_value = 0;
		 return ( sg_wheel_temp );
	}
	
	return (0);
}
#endif

/**
 * @brief	Reads data of a specified length from a specified address
 * @param	s_addr is specified address
 * @param	d_addr is the address where the read data is stored
 * @param	len is specified length
 * @return	Offset of the location of the current storage area
 */
int flash_info_load_aaa(u32 s_addr, u8 *d_addr,  int len)
{
    int idx;
    u32 buf;

    /* Polling the storage area, U32_MAX indicates that the storage area is not in use */
    for (idx = 0; idx < (4096 - len); idx += len)
    {
        flash_read_page((u32)(s_addr + idx), 4, (u8 *)(&buf));
        if (buf == U32_MAX)
        {
            break;
        }
    }

    /* idx = (idx-len), indicates the offset location of the last used storage address.
	 * if the idx < 0, the flash does not store data and returns it directly */
    idx -= len;
    if (idx < 0) // no binding
    {
        return idx;
    }

    /* Read stored data*/
    flash_read_page((u32)(s_addr + idx), len, d_addr);

    /* If the data location offset exceeds 3200, save the data from the beginning */
    if (idx > 3200)
    {  //when idx > 3200, erase flash and storage from begin
        flash_erase_sector((u32)s_addr);

        sleep_us(10);

        flash_write_page((u32)s_addr, len, d_addr);
		
        idx = 0;
    }

    return idx;
}

#if USB_MODE_ENABLE

unsigned char usb_status_changed_need_reboot(void)
{
#if USB_SWITCH_ENABLE
	if ( ( RF_USB_MODE == fun_mode ) && ( USB_OUT() || USB_SWITCH_OFF() ) )
#else
	if ( RF_USB_MODE == fun_mode && USB_OUT() )
#endif
	{
		usb_has_judge = USB_MODE_OUT;
		analog_write(DEEP_ANA_REG7, usb_has_judge);
		return (1);
	}

#if USB_SWITCH_ENABLE
	if ( RF_USB_MODE != fun_mode && USB_IN() && USB_SWITCH_ON() && (USB_MODE_IN_FAILED != usb_has_judge) )
#else
	if ( RF_USB_MODE != fun_mode && USB_IN() && (USB_MODE_IN_FAILED != usb_has_judge) )
#endif
	{
		return (1);
	}

#if USB_SWITCH_ENABLE
	if ( ( RF_USB_MODE != fun_mode ) && ( USB_OUT() || USB_SWITCH_OFF() ) )
#else
	if ( RF_USB_MODE != fun_mode && USB_OUT() )
#endif
	{
		usb_has_judge = USB_MODE_OUT;
		analog_write(DEEP_ANA_REG7, usb_has_judge);
	}

	return (0);
}

#endif
/**
 * @brief	Initialize user configuration parameters
 * @param	none
 * @return	none
 * @note	Read user configuration information from CFG_ADR_MAC, if user configuration information is not burned, the default value is used
 */
void user_config(void)
{
	/* Read user configuration information from CFG_ADR_MAC */
	flash_read_page(CFG_ADR_MAC, sizeof(custom_cfg_t), (u8 *)&user_cfg.dev_mac);

	/* If the MAC address is not burned, 4-byte MAC is randomly generated and written to the corresponding address */
	if (user_cfg.dev_mac == U32_MAX)
	{
		generateRandomNum(4, (u8 *) &user_cfg.dev_mac);
		flash_write_page(CFG_ADR_MAC, 4, (u8 *)&user_cfg.dev_mac);
	}

	/* If pairing/communication /EMI transmit power is not burned, use the default value */
#if 0
	if (user_cfg.paring_tx_power == U8_MAX)
	{
		user_cfg.paring_tx_power = DEFAULT_PAIR_TX_POWER;
	}
	if (user_cfg.tx_power == U8_MAX)
	{
		user_cfg.tx_power = DEFAULT_NORMAL_TX_POWER;
	}
	if (user_cfg.emi_tx_power == U8_MAX)
	{
		user_cfg.emi_tx_power = DEFAULT_EMI_TX_POWER;
	}
#else
	user_cfg.paring_tx_power = DEFAULT_PAIR_TX_POWER;
	user_cfg.tx_power 		 = DEFAULT_NORMAL_TX_POWER;
	user_cfg.emi_tx_power	 = DEFAULT_EMI_TX_POWER;
#endif


#if (AES_METHOD == 1)
	/* If the AES public key is not burned (that is, full FF), use the default value as the AES public key,
	 * if it is burned (not all FF), use the burned information as the public key */
	for (u8 i=0; i<16; i++)
	{
		if (user_cfg.pub_key[i] != 0xff)
		{
			memcpy(pub_key, &user_cfg.pub_key[0], 16);
			break;
		}
	}
#endif

	memset(&user_cfg.device_name[0], 0x00, 29);
	if (flash_dev_info.mast_id == 0)
	{
		device_name_len = sizeof(DEVICE_NAME_1) - 1;

		if (device_name_len > DEVICE_NAME_LENGTH_MAX)
		{
			device_name_len = DEVICE_NAME_LENGTH_MAX;
		}
		memcpy(&user_cfg.device_name[0], DEVICE_NAME_1, device_name_len);
	}
	else
	{
		device_name_len = sizeof(DEVICE_NAME_2) - 1;

		if (device_name_len > DEVICE_NAME_LENGTH_MAX)
		{
			device_name_len = DEVICE_NAME_LENGTH_MAX;
		}
		memcpy(&user_cfg.device_name[0], DEVICE_NAME_2, device_name_len);
	}

#if BATT_CHECK_ENABLE
	/* The GPIO ADC is equipped with an external 1/2 divider resistor
	 * So the sampling value corresponds to 1/2 of the voltage value */

	#if (BATTERY_CAP_1500MV == BATTERY_CAP_OPTION)
		user_cfg.bat_type = BATT_TYPE_15V_LITHIUM;
	#elif (BATTERY_CAP_3000MV == BATTERY_CAP_OPTION)
		user_cfg.bat_type = BATT_TYPE_30V_LITHIUM;
	#elif (BATTERY_CAP_3700MV == BATTERY_CAP_OPTION)
		user_cfg.bat_type = BATT_TYPE_37V_LITHIUM;
	#else
		user_cfg.bat_type = BATT_TYPE_37V_LITHIUM;
	#endif


	if( user_cfg.bat_type == BATT_TYPE_37V_LITHIUM )
	{
	#if (PROJECT_ID == PID_MS13)
		bat_mv.shutdown_vol = 3000;  // shut down
		bat_mv.alarm_vol = 3200;     // alarm
		bat_mv.min_vol   = 3200;     //
		bat_mv.max_vol   = 4000;     // full vol
	#elif (PROJECT_ID == PID_660)
		bat_mv.shutdown_vol = 3000;  // shut down
		bat_mv.alarm_vol = 3300;     // alarm
		bat_mv.min_vol   = 3300;     //
		bat_mv.max_vol   = 4050;     // full vol
	#elif (PROJECT_ID == PID_MS2360)
		bat_mv.shutdown_vol = 3200;  // shut down
		bat_mv.alarm_vol = 3400;     // alarm
		bat_mv.min_vol   = 3400;     //
		bat_mv.max_vol   = 4100;     // full vol
	#elif (PROJECT_ID == PID_2187) || (PROJECT_ID == PID_0120) || (PROJECT_ID == PID_S600)
		bat_mv.shutdown_vol = 3100;  // shut down
		bat_mv.alarm_vol = 3400;     // alarm
		bat_mv.min_vol   = 3400;     //
		bat_mv.max_vol   = 4100;     // full vol
	#elif (PROJECT_ID == PID_104)
		bat_mv.shutdown_vol = 3200;  // shut down
		bat_mv.alarm_vol = 3400;     // alarm
		bat_mv.min_vol   = 3400;     //
		bat_mv.max_vol   = 4100;     // full vol
	#elif BATT_CACL_METHOD_ENABLE
		bat_mv.shutdown_vol = 3050;  // shut down
		bat_mv.alarm_vol = 3300;     // alarm
		bat_mv.min_vol   = 3300;     //
		bat_mv.max_vol   = 4100;     // full vol
	#else
		bat_mv.shutdown_vol = 3000;  // shut down 
		bat_mv.alarm_vol = 3300;     // alarm 
		bat_mv.min_vol   = 3300;     // 
		bat_mv.max_vol   = 4000;     // full vol
	#endif
	}
	else if (user_cfg.bat_type == BATT_TYPE_15V_LITHIUM) 
	{
	#if (PROJECT_ID == PID_PCM8409)
		bat_mv.shutdown_vol = 950; // 0.9v
		bat_mv.alarm_vol = 1000;   // 1.1vV
		bat_mv.min_vol   = 1000;   // 1.1V
		bat_mv.max_vol   = 1500;   // 1.5V
	#elif (PROJECT_ID == PID_BG523)
		bat_mv.shutdown_vol = 850; // 0.85v
		bat_mv.alarm_vol = 1100;   // 1.1v
		bat_mv.min_vol   = 1100;   // 1.1V
		bat_mv.max_vol   = 1450;   // 1.5V
	#elif (PROJECT_ID == PID_TB903)
		bat_mv.shutdown_vol = 900; // 0.90v
		bat_mv.alarm_vol = 1050;   // 1.1v
		bat_mv.min_vol   = 1050;   // 1.1V
		bat_mv.max_vol   = 1450;   // 1.5V
	#elif (PROJECT_ID == PID_M0018)
		bat_mv.shutdown_vol = 1000; // 0.90v
		bat_mv.alarm_vol = 1100;   // 1.1v
		bat_mv.min_vol   = 1100;   // 1.1V
		bat_mv.max_vol   = 1450;   // 1.5V
	#elif (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
		bat_mv.shutdown_vol = 900; // 0.90v
		bat_mv.alarm_vol = 1100;   // 1.1v
		bat_mv.min_vol   = 1100;   // 1.1V
		bat_mv.max_vol   = 1450;   // 1.5V
	#else
		bat_mv.shutdown_vol = 1050; // 0.9v
		bat_mv.alarm_vol = 1100;   // 1.1vV
		bat_mv.min_vol   = 1100;   // 1.1V
		bat_mv.max_vol   = 1450;   // 1.5V
	#endif
	} 
	else if (user_cfg.bat_type == BATT_TYPE_30V_LITHIUM)
	{
	#if (M8259A_V2)
		bat_mv.shutdown_vol = 1950;
		bat_mv.alarm_vol = 2150;
		bat_mv.min_vol   = 2150;
		bat_mv.max_vol   = 3000;
	#elif BATT_NO_SHUT_DOWN_ENABLE
		bat_mv.shutdown_vol = 2000;
		bat_mv.alarm_vol = 2200;
		bat_mv.min_vol   = 2200;
		bat_mv.max_vol   = 3000;
	#else
		bat_mv.shutdown_vol = 2050;
		bat_mv.alarm_vol = 2300;
		bat_mv.min_vol   = 2300;
		bat_mv.max_vol   = 2950;
	#endif
	}
	else 
	{
		bat_mv.shutdown_vol = 2000;
		bat_mv.alarm_vol = 1900; // 3.8V
		bat_mv.min_vol   = 1850; // 3.7V
		bat_mv.max_vol   = 2050; // 4.1V
	}
#endif

	//if (user_cfg.internal_cap!=0)
	//{
	//	blc_app_setExternalCrystalCapEnable(1);
	//}
}


void mouse_mode_init(void)
{
#if (SWITCH_MODE_ENABLE)

	unsigned char sw_state = 0;

	#if SWITCH_MODE_IO_FLOAT_ENABLE
		gpio_input_config_status(MODE_SWITCH_PIN, PM_PIN_UP_DOWN_FLOAT);
	#elif SWITCH_MODE_IO_PULLUP_1M_ENABLE
		gpio_input_config_status(MODE_SWITCH_PIN, PM_PIN_PULLUP_1M);
	#elif SWITCH_MODE_IO_PULLUP_10K_ENABLE
		gpio_input_config_status(MODE_SWITCH_PIN, PM_PIN_PULLUP_10K);
	#elif SWITCH_MODE_IO_PULLDOWN_ENABLE
		gpio_input_config_status(MODE_SWITCH_PIN, PM_PIN_PULLDOWN_100K);
	#else
		gpio_input_config_status(MODE_SWITCH_PIN, PM_PIN_UP_DOWN_FLOAT);
	#endif

	sleep_us(100);

	sw_state = gpio_read(MODE_SWITCH_PIN);

	#if (SWITCH_MODE_IO_TOGGLE_ENABLE)
		sw_state = !sw_state;
	#endif

	if ( 0 == sw_state )
	{
		fun_mode = RF_2M_2P4G_MODE;
		flash_dev_info.mode = RF_2M_2P4G_MODE;
		g_mode_io_status = 0;
	}
	else
	{
		fun_mode = RF_1M_BLE_MODE;
		flash_dev_info.mode = RF_1M_BLE_MODE;
		g_mode_io_status = 1;
	}

#if (PROJECT_ID == PID_MS13) || (PROJECT_ID == PID_DMS027) || (PROJECT_ID == PID_DM509)
	flash_dev_info.mast_id = BLE_DEVICE_ID_0;
#endif

#else
	fun_mode = flash_dev_info.mode;
#endif
}


int main(void) //run in ramcode
{
	/* Use internal 32K RC crystal oscillator */
#if (SAVE_RAM_CODE_ENABLE)
	blc_pm_select_internal_32k_crystal_save_ram(); //save ram enable
#else
	blc_pm_select_internal_32k_crystal();
#endif

#if (BLE_OTA_SERVER_ENABLE || BOOTLOADER_OTA_ENABLE)
	/* Set the firmware size and OTA startup address */
	#if MCU_QFN40_ENABLE
		blc_ota_setFirmwareSizeAndBootAddress(128, MULTI_BOOT_ADDR_0x20000);
	#elif (FLASH_SIZE_OPTION == FLASH_SIZE_OPTION_128K)
		blc_ota_setFirmwareSizeAndBootAddress(52, MULTI_BOOT_ADDR_0x10000);
	#elif (FLASH_SIZE_OPTION == FLASH_SIZE_OPTION_512K)
		blc_ota_setFirmwareSizeAndBootAddress(128, MULTI_BOOT_ADDR_0x20000);
	#endif
#endif

	/* Initialize the external 24M crystal oscillator */
	cpu_wakeup_init(EXTERNAL_XTAL_24M);

	/* Initialize the system clock: 48MHz */
	clock_init(SYS_CLK_TYPE);

	/* Initialize the GPIO, if the GPIO state is not set, the default value is used */
	gpio_init(1);

	/* load customized freq_offset CAP value and TP value. */
	blc_app_loadCustomizedParameters();

#if (FIRMWARES_SIGNATURE_ENABLE)
	/* Check whether the firmware signature is correct.
	* If it is correct, the program will run down. If it is wrong, it will be stuck in an endless loop */
	blt_firmware_signature_check();
#endif

#if (BATT_CHECK_ENABLE)
	/* Load the ADC detection two-point calibration value */
	if (user_cfg.adc_calibrate_en)
	{
		printf("adc_calibrate_en blc_app_loadADCParameters\n");
		blc_app_loadADCParameters();
	}
#endif

	/* Used to generate random numbers */
	random_generator_init(); //this is must

#if OTA_ENABLE_AAA
	run_app_code();
#endif

	deep_flag    = analog_read(DEEP_ANA_REG9); // get deep_flag
	ana_reg1_aaa = analog_read(USED_DEEP_ANA_REG1);
	pair_flag = ana_reg1_aaa & PAIR_ANA_FLG;        // get pair_flag

//	dpi_value = analog_read(USED_DPI_DEEP_ANA_REG); // get dpi_value

#if USB_MODE_ENABLE
    usb_has_judge =  analog_read(DEEP_ANA_REG7);
#endif

#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_FX282) || (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_DMS157) || VOLTAGE_TEMP_HANDLE_ENABLE
    gc_vol_cap_reg_temp = analog_read(BATTERY_CAP_REG);
#endif

	printf("Mouse sys on deep_flag=0x%1x, pair_flag=0x%1x \n", deep_flag, pair_flag);

	dev_info_idx = flash_info_load_aaa(CFG_DEVICE_MODE_ADDR, (u8 *)&flash_dev_info.dongle_id, SAVE_MAX_IN_FLASH);
	if (dev_info_idx < 0)
	{
		/* if dev_info_idx < 0, indicates that the device has not been paired. The default value is set */
		flash_dev_info.mode = RF_2M_2P4G_MODE;//RF_1M_BLE_MODE; //Enter BLE mode
		flash_dev_info.mast_id = 0;

		flash_dev_info.slave_mac_addr[0] = 0;
		flash_dev_info.slave_mac_addr[1] = 0;
		flash_dev_info.slave_mac_addr[2] = 0;
		flash_dev_info.slave_mac_addr[3] = 0;
		flash_dev_info.dongle_id = 0;

	#if (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_104) || (PROJECT_ID == PID_S600)
		flash_dev_info.code_led_sw = 0;
		flash_dev_info.code_led_mode = 0;
	#endif

	#if (BATTERT_STORAGE_ENABLE)
		flash_dev_info.battery_value = 0xFF;
	#endif

	#if (DPI_SAVE_FLASH)
		flash_dev_info.d24g_dpi = 0xFF;
		flash_dev_info.ble_dpi[0] = 0xFF;
		flash_dev_info.ble_dpi[1] = 0xFF;
		flash_dev_info.ble_dpi[2] = 0xFF;
		flash_dev_info.ble_dpi[3] = 0xFF;
	#endif
	}

#if WEB_HID_ENABLE
	dev_web_info_idex = flash_info_load_aaa(CFG_WEB_DATA_ADDR, (u8 *)&gc_web_data, WEB_DATA_LENGTH_MAX);
	if (dev_web_info_idex < 0)
	{

	}

	gc_web_data.key[0].type = 0x10; gc_web_data.key[0].value = 0x01;
	gc_web_data.key[1].type = 0x10; gc_web_data.key[1].value = 0x02;
	gc_web_data.key[2].type = 0x10; gc_web_data.key[2].value = 0x04;
	gc_web_data.key[3].type = 0x10; gc_web_data.key[3].value = 0x08;
	gc_web_data.key[4].type = 0x10; gc_web_data.key[4].value = 0x10;
	gc_web_data.key[5].type = 0x40; gc_web_data.key[5].value = 0x02;

	printf("webtemp: %d\n", WEB_DATA_LENGTH_MAX);
#endif

	user_config();

#if (BLE_SNIFF_DEBUG)
	flash_dev_info.mode = RF_1M_BLE_MODE;
#endif

	mouse_mode_init();

#if (PROJECT_ID == PID_4028)
	#if	(0 == M4028_BLE_G24_MODE_ENABLE)
		flash_dev_info.mode = RF_1M_BLE_MODE;
		fun_mode = RF_1M_BLE_MODE;
	#endif

	flash_dev_info.mast_id = BLE_DEVICE_ID_0;
#endif

#if (PROJECT_ID == PID_PCM8266)
	flash_dev_info.mode = RF_1M_BLE_MODE;
	fun_mode = RF_1M_BLE_MODE;
#endif

#if KEY_MODE_CHANGE_G24_BLE_ENABLE || KEY_PRESS_CHANGE_MODE_ENABLE
	flash_dev_info.mast_id = BLE_DEVICE_ID_0;
#endif

#if (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
	flash_dev_info.mast_id = BLE_DEVICE_ID_0;
#endif

	if ( flash_dev_info.mode >= RF_MAX_MODE )
	{
		flash_dev_info.mode = RF_2M_2P4G_MODE;
		fun_mode = RF_2M_2P4G_MODE;
	}

	if ( flash_dev_info.mast_id >= BLE_DEVICE_ID_MAX )
	{
		flash_dev_info.mast_id = BLE_DEVICE_ID_0;
	}

	#if (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_M45) || (PROJECT_ID == PID_S600)
		if ( flash_dev_info.code_led_sw >= 2 )
		{
			flash_dev_info.code_led_sw = 0;
		}

		if ( flash_dev_info.code_led_mode >= RGB_MODE_LIGHTS_MAX )
		{
			flash_dev_info.code_led_mode = RGB_MODE_LIGHTS_NUION;
		}
	#endif

	#if (PROJECT_ID == PID_S600)
		flash_dev_info.code_led_mode = RGB_MODE_LIGHTS_BREATH;
	#endif

	#if (PROJECT_ID == PID_104)
		if ( flash_dev_info.code_led_sw >= 2 )
		{
			flash_dev_info.code_led_sw = 0;
		}

		if ( flash_dev_info.code_led_mode >= RGB_MODE_LIGHTS_MAX )
		{
			flash_dev_info.code_led_sw = 0;
		}
	#endif

//	printf("--- fun mode: %s\n", fun_mode ? "RF_2M_2P4G_MODE" : "RF_1M_BLE_MODE");
//	printf("--- mast_id: %d\n", flash_dev_info.mast_id);

#if (BATT_CHECK_ENABLE)
	 user_batt_check_init();
#endif

	hw_init();

#if USB_MODE_ENABLE
	cpu_set_gpio_wakeup(USB_IN_CHECK_PIN, 1, 1);

	if ( USB_OUT() )
	{
		usb_has_judge = USB_MODE_OUT;
		analog_write(DEEP_ANA_REG7, usb_has_judge);
	}
	else
	{
	#if (G24_MODE_ENABLE || BLE_MODE_ENABLE)
		if ( USB_MODE_OUT == usb_has_judge || USB_MODE_IN_SUCC == usb_has_judge )
		{
		#if USB_SWITCH_ENABLE
			if ( USB_SWITCH_ON() )
			{
				fun_mode = RF_USB_MODE;
				printf("USB MODE :RF_USB_MODE \n ");
			}
		#else
			fun_mode = RF_USB_MODE;
			printf("USB MODE :RF_USB_MODE \n ");
		#endif
		}
	#else
		fun_mode = RF_USB_MODE;
		printf("USB MODE :RF_USB_MODE \n ");
	#endif
	}
	printf("USB:%d %d \n ", gpio_read(USB_IN_CHECK_PIN), SAVE_MAX_IN_FLASH);
#endif

#if (BLE_MODE_ENABLE)
	if (fun_mode == RF_1M_BLE_MODE)
	{
		rf_ble_1m_param_init();
		user_init_normal(); //initial BLE mode
	}
#endif

#if G24_MODE_ENABLE
	if (fun_mode == RF_2M_2P4G_MODE) 
	{
		rf_drv_private_2m_init();
		d24_user_init(); //initial 2.4G mode
	}
#endif

#if USB_MODE_ENABLE
	if (fun_mode == RF_USB_MODE)
	{
		usb_user_init();
		pm_set_suspend_power_cfg(PM_POWER_USB, 1);
	}
	else
	{
		usb_dp_pullup_en (0);
	}
#endif

#if (FLASH_LOCK_ENABLE)
	/* Lock the Flash area of the storage where the firmware resides to prevent firmware damage */
	flash_mid = flash_read_mid();
	flash_lock_handle(FLASH_LOCK_HALF_BLOCK); //just lock firmware block
#endif

#if (MODULE_WATCHDOG_ENABLE)
	wd_set_interval_ms(WATCHDOG_INIT_TIMEOUT, CLOCK_SYS_CLOCK_1MS);
	wd_start();
#endif

	irq_enable(); //Interrupt enable

#if (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_M45) || (PROJECT_ID == PID_104) || KEY_PAIR_USED_POWERUP_ENABLE
	if ( 0 == gpio_read(PIN_BTN_MODE) )
	{
		gc_hm668_mode_key_used = 1;
	}
#endif

	if ( deep_flag == POWER_ON_ANA_AAA )
	{
		power_on_tick = clock_time() | 1; //Power-on restart
		
	#if MOUSE_FACTORY_RESET_ENABLE
 		mouse_factory_power_on_flag();
	#endif

		printf("power up set 111 \n");
	}
	else if (deep_flag == MODE_CHANGE_REBOOT_ANA_AAA)
	{
		mode_change_flag = 1; //2.4G or BLE mode switch restart
	}
	else if (deep_flag == MUTI_DEVICE_REBOOT_ANA_AAA)
	{
		device_switch_flag = 1; //BLE multi-channel switch restart
	}

	adv_begin_tick = clock_time()|1; //start ADV count

#if (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
	ext_mcu_prepare();
#endif

	while (1) {

	#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear normal watch dog
	#endif

	#if (MODULE_32K_WATCHDOG_ENABLE)
		wd_32k_clear(); //clear 32k watch dog
	#endif

	#if (BLE_MODE_ENABLE)
		if (fun_mode == RF_1M_BLE_MODE)
		{
			 main_loop();
		}
	#endif
	
	#if G24_MODE_ENABLE
		if (fun_mode == RF_2M_2P4G_MODE)
		{
			 d24_main_loop();
		}
	#endif

	#if (USB_MODE_ENABLE)
		if (fun_mode == RF_USB_MODE)
		{
			usb_main_loop();
		}
	#endif

	#if USB_MODE_ENABLE
		if( usb_status_changed_need_reboot() )
		{
			clear_pair_flag();
			user_reboot(MODE_CHANGE_REBOOT_ANA_AAA);
		}
	#endif

	#if (BATT_CHECK_ENABLE)
		 user_batt_check_proc();
	#endif

	#if BUTTON_FUN_ENABLE_AAA
		 user_button_check_proc();
	#endif

	#if	MODULE_OLED_ENABLE
		 oled_display();
	#endif

	#if MODULE_DIGITAL_TUBE_ENABLE
		 digital_tube_pro();
	#endif

	#if LED_LOGO_ACTION_ENABLE
 		led_logo_action();
 	#endif

	#if LED_COLOR_LIGHTS_ENABLE
 		led_color_logo_action();
	#endif

	last_btn_value = btn_value;

	#if (PROJECT_ID == PID_M388)
		batt_dpi_two_loop();
	#endif

	#if (PROJECT_ID == PID_M0018) || (PROJECT_ID == PID_2187)
		led_connect_none_status_clear();
	#endif

	#if (PROJECT_ID == PID_0120) || (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_M45) || (PROJECT_ID == PID_104) || (PROJECT_ID == PID_S600)
 	 	 #if LED_CODE_PWIR_DRIVE_ENABLE
			 ws2812_code_loop_pro();
		 #else
			 rgb_code_loop_pro();
		#endif
	#endif


#if (DEBUG_MODE)
    static u32 main_tick = 0;
    if ( clock_time_exceed(main_tick, 1000*1000) )
    {
       main_tick = clock_time();

       if (fun_mode == RF_1M_BLE_MODE)
       {
		  // suspend_wake_up_enable && (blc_ll_getCurrentState() == BLS_LINK_STATE_CONN
    	  // printf("m=%d,%d,%d,%d,%d \n", suspend_wake_up_enable, blc_ll_getCurrentState(), bls_ll_getConnectionInterval(), bls_ll_getConnectionLatency(), bls_ll_getConnectionTimeout());
       }

     //  printf("REG02=%1x 07=%1x\n\n", I2C_PAN3204LL_ReadRegister(0x02), I2C_PAN3204LL_ReadRegister(0x07));
    }
#endif

	}
}


