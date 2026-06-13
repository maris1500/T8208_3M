/********************************************************************************************************
 * @file     aaa_public.c
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
#include "AAA_led_rgb.h"
#include "AAA_LED.h"
#include "AAA_oled.h"
#include "AAA_24G_APP.h"

#include "main.h"

#if MODULE_OLED_ENABLE
	extern void OLED_Init(void);
#endif

#if (MODULE_DIGITAL_TUBE_ENABLE)
#include "../module/AAA_digital_tube.h"
#endif

#if LED_CODE_PWIR_DRIVE_ENABLE
	#include "../module/AAA_ws2812x_led.h"
#endif

#if (PROJECT_ID == PID_DMS157)
	 extern void rgb_led_pwm_init(void);
#endif

_attribute_data_retention_ u8 active_disconnect_reason = 0;
_attribute_data_retention_ u8 device_name_len = 0;
_attribute_data_retention_ u8 connect_ok = 0;

_attribute_data_retention_ custom_cfg_t user_cfg;
_attribute_data_retention_ u8 fun_mode = RF_1M_BLE_MODE;
_attribute_data_retention_ FLASH_DEV_INFO_AAA flash_dev_info;

_attribute_data_retention_ u8 suspend_wake_up_enable = 1;
_attribute_data_retention_ u8 deep_flag = POWER_ON_ANA_AAA;
_attribute_data_retention_ u8 pair_flag = 0;
_attribute_data_retention_ u8 ana_reg1_aaa = 0;
//_attribute_data_retention_ u8 has_been_paired_flag = 0;
_attribute_data_retention_ u8 dpi_value = 1;
_attribute_data_retention_ u8 mode_change_flag = 0;
_attribute_data_retention_ u8 device_switch_flag = 0;

//_attribute_data_retention_ u8 has_mode_key_press = 0;
//_attribute_data_retention_ u8 has_new_report_aaa;
_attribute_data_retention_ u8 has_new_key_event = 0;

//_attribute_data_retention_ u8 combination_flag = 0;

_attribute_data_retention_ u16 btn_value = 0;
_attribute_data_retention_ u16 last_btn_value = 0;
//_attribute_data_retention_ u8 mouse_btn_in_sensor;
_attribute_data_retention_ mouse_data_t ms_data;

//_attribute_data_retention_ u8 wheel_pre = 0;
//_attribute_data_retention_ u8 wheel_status;

_attribute_data_retention_user u32 loop_cnt;
_attribute_data_retention_user u32 idle_tick;
_attribute_data_retention_user u32 idle_count;
_attribute_data_retention_user u32 adv_begin_tick;
_attribute_data_retention_user u32 adv_count = 0;

MYFIFO_INIT (fifo_km, 12, 8); //The size must be a multiple of 4 bytes

//extern u8  mouse_send_need_f;
//extern u8  dongle_id_valid_f;

#if (PROJECT_ID == PID_G929) || (PROJECT_ID == PID_BG523)
	unsigned char gc_dpi_func2_flag = 0;
#endif

#if (KEY_FEATURE_DESKTOP_ENABLE)
	unsigned char ms_desktop_flag = 0;
#endif

#if (KEY_FEATURE_MIKE_AI_ENABLE)
	unsigned char ms_mike_ai_flag = 0;
	unsigned char ms_mike_ai_cpi_flag = 0;
#endif

#if (LED_BATT_CAP_LEVEL_ENABLE)
	extern void batt_charge_init(void);
#endif

#if KEY_ADC_MODE_MULTI_FUNC_ENABLE
	extern unsigned int adc_sample_voltale(void);
#endif

#if (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
	extern unsigned int adc_sample_voltale_high(void);
#endif

#if KEY_FEATURE_BOSSKEY_ENABLE
	key_function_special_en gc_key_special_sta = KEY_SPE_NONE;
#endif

#if G24_DELAY_ENTER_SLEEP_ENABLE
	extern void mouse_moved_time_clean(void);
#endif

#if DPI_3065XY_FOUR_LEVEL_ENABLE
	extern unsigned char dpi_3605_limit_level_flag;
#endif

#if	BATT_LOWER_LED_INDICATE_60S_ENABLE
	extern battery_indicate_mouse_moved(void);
#endif

FLASH_SIZE_T flash_size = FLASH_SIZE_USED_128K;
unsigned int flash_mid;


#if AUTO_CHECK_OS_TYPE
   extern  _attribute_data_retention_ u8 os_type;
#endif

#if SWITCH_MODE_ENABLE
   extern unsigned char g_mode_io_status;
#endif

#if USB_MODE_ENABLE
   extern u8 push_usb_fifo_aaa(u8 type,u8 *buf,u8 len);
#endif

extern void id_sleep_enable(int);

#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_FX282) || (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_DMS157) || VOLTAGE_TEMP_HANDLE_ENABLE
	extern unsigned char gc_vol_cap_reg_temp;
#endif

#if (PROJECT_ID == PID_0120)
	extern void led_code_factory_reset(void);
	extern void led_adv_all_off(void);
	extern unsigned char power_on_key_red(u32 pin);
#endif

#if (PROJECT_ID == PID_0120) || (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_M45)
	#if  LED_CODE_ENABLE
		extern void rgb3810_sleep_close(void);
	#endif
#endif

#if (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_M45)
	#if  LED_CODE_ENABLE
		extern void rgb3810_light_on_init(void);
	#endif
#endif

#if	LED_CODE_PWIR_DRIVE_ENABLE
	extern void ws2812_sleep_close(void);
	extern void ws2812_mode_change_para_clear(void);
#endif

#if WEB_HID_ENABLE
	extern int dev_web_info_idex;
#endif

BLE_DEVICE_ID_EN ble_mode_device_id_get(void)
{
	return( flash_dev_info.mast_id );
}

RF_MCU_MODE_EN mcu_mode_get(void)
{
	return( flash_dev_info.mode );
}

/**
 * @brief	Lock the flash to prevent data from being modified
 * @param	Flash_block: Block area that needs to be locked

 * @return	none
 */
void flash_lock_handle(FLASH_LOCK_BLOCK Flash_block)
{
#if FLASH_LOCK_ENABLE
	u8 retry_cnt = 5;

	mid1160c8_lock_block_e lock_block_1160c8;
	mid1360c8_lock_block_e lock_block_1360c8;
	mid136085_lock_block_e lock_block_136085;
	mid114485_lock_block_e lock_block_114485;
	
	switch(flash_mid)
	{
		case 0x1160c8: //GD25LD10C
			flash_size = FLASH_SIZE_USED_128K;
			
			switch(Flash_block)
			{
				case FLASH_LOCK_HALF_BLOCK:
					lock_block_1160c8 = FLASH_LOCK_LOW_64K_MID1160C8;
					break;
				case FLASH_LOCK_ALL_BLOCK:
					lock_block_1160c8 = FLASH_LOCK_ALL_128K_MID1160C8;
					break;
				case FLASH_LOCK_NONE_BLOCK:
				default:
					lock_block_1160c8 = FLASH_LOCK_NONE_MID1160C8;
					break;
			}

			if ((lock_block_1160c8 & FLASH_WRITE_STATUS_BP_MID1160C8) || (lock_block_1160c8 == FLASH_LOCK_NONE_MID1160C8))
			{
				while((lock_block_1160c8 != (flash_read_status_mid1160c8()& FLASH_WRITE_STATUS_BP_MID1160C8)) && ((--retry_cnt) > 0))
				{
					flash_lock_mid1160c8(lock_block_1160c8);

					sleep_us(20);
				}
			}
			break;

		case 0x1360c8: //GD25LD40C
			flash_size = FLASH_SIZE_USED_512K;

			
			switch(Flash_block)
			{
					case FLASH_LOCK_HALF_BLOCK:
						lock_block_1360c8 = FLASH_LOCK_LOW_256K_MID1360C8;
						break;
					case FLASH_LOCK_ALL_BLOCK:
						lock_block_1360c8 = FLASH_LOCK_ALL_512K_MID1360C8;
						break;
					case FLASH_LOCK_NONE_BLOCK:
					default:
						lock_block_1360c8 = FLASH_LOCK_NONE_MID1360C8;
						break;
			}
			

			if ((lock_block_1360c8 & FLASH_WRITE_STATUS_BP_MID1360C8) || (lock_block_1360c8 == FLASH_LOCK_NONE_MID1360C8))
			{
				while((lock_block_1360c8 != (flash_read_status_mid1360c8()& FLASH_WRITE_STATUS_BP_MID1360C8)) && ((--retry_cnt) > 0))
				{
					flash_lock_mid1360c8(lock_block_1360c8);

					sleep_us(20);
				}
			}
			break;

		case 0x136085: //P25Q40SU
			flash_size = FLASH_SIZE_USED_512K;
			
			switch(Flash_block)
			{
				case FLASH_LOCK_HALF_BLOCK:
					lock_block_136085 = FLASH_LOCK_LOW_256K_MID136085;
					break;
				case FLASH_LOCK_ALL_BLOCK:
					lock_block_136085 = FLASH_LOCK_ALL_512K_MID136085;
					break;
				case FLASH_LOCK_NONE_BLOCK:
				default:
					lock_block_136085 = FLASH_LOCK_NONE_MID136085;
					break;
			}

			if ((lock_block_136085  & FLASH_WRITE_STATUS_BP_MID136085) || (lock_block_136085 == FLASH_LOCK_NONE_MID136085))
			{
				while((lock_block_136085 != (flash_read_status_mid136085()& FLASH_WRITE_STATUS_BP_MID136085)) && ((--retry_cnt) > 0))
				{
					flash_lock_mid136085(lock_block_136085);

					sleep_us(20);
				}
			}
			break;

		case 0x114485: //P25D09U
			flash_size = FLASH_SIZE_USED_128K;
			
			switch(Flash_block)
			{
				case FLASH_LOCK_HALF_BLOCK:
					lock_block_114485 = FLASH_LOCK_LOW_64K_MID114485;
					break;
				case FLASH_LOCK_ALL_BLOCK:
					lock_block_114485 = FLASH_LOCK_ALL_128K_MID114485;
					break;
				case FLASH_LOCK_NONE_BLOCK:
				default:
					lock_block_114485 = FLASH_LOCK_NONE_MID114485;
					break;
			}

			if ((lock_block_114485 & FLASH_WRITE_STATUS_BP_MID114485) || (lock_block_114485 == FLASH_LOCK_NONE_MID114485))
			{
				while((lock_block_114485 != (flash_read_status_mid114485()& FLASH_WRITE_STATUS_BP_MID114485)) && ((--retry_cnt) > 0))
				{
					flash_lock_mid114485(lock_block_114485);

					sleep_us(20);
				}
			}
			break;

		default:
			//printf("\n****** Err Flash ******\n");
			flash_mid = flash_read_mid();
			break;
	}

	//printf((flash_size == FLASH_SIZE_USED_128K)?"---Flash size = 128k\n":"---Flash size = 512k\n");

	if (retry_cnt)
	{
		printf("---Flash lock success:%d\n", Flash_block);
	}
	else
	{
		printf("---Flash lock fail:%d\n", Flash_block);
	}
#endif
}

/**
 * @brief	Store data in flash
 * @param	addr: storage start flash address
 * @param	len: storage length
 * @param	buf: storage data
 * @return	none
 * @note 	Write eight bytes each time to prevent the write operation from taking too long and causing an interrupt failure
 */
void flash_write_page_user(unsigned long addr, unsigned long len, unsigned char *buf)
{
    u16 write_cnt = len / 8;
    u8 last_write_len = len % 8;

    for (int i = 0; i < write_cnt; i++) {
        flash_write_page((addr + 8 * i), 8, (buf + 8 * i));
        sleep_us(20);
    }

    if (last_write_len > 0)
        flash_write_page((addr + 8 * write_cnt), last_write_len, (buf + 8 * write_cnt));
}


#define MAX_SAVE_FLAH_LENGTH	max2(64,sizeof(FLASH_DEV_INFO_AAA))
#define SAVE_SUCCESS			0
#define SAVE_FAILED 			-1

/**
 * @brief	Save data to flash
 * @param	addr: first address of storage block
 * @param	len: storage length
 * @param	buf: storage data
 * @param	offset: offset of the location of the current data area
 * @return	save result
 */
int save_data_to_flash(unsigned long addr, unsigned long len, unsigned char *buf, int *offset)
{
	u8 tmp_buf[MAX_SAVE_FLAH_LENGTH];
	int tmp_offset = 0; //offset of save address

	if (len > MAX_SAVE_FLAH_LENGTH)
	{ //The storage length is exceeded
		return SAVE_FAILED;
	}

	if (*offset > (4096-2*len))
	{ //The storable block area is exceeded
		flash_erase_sector(addr); //Erase the block area for re-storage
	}
	else
	{
		tmp_offset = *offset+len;
	}
	
	for (u8 i=0; i<2; i++)
	{ //retry 2 times
		flash_write_page_user(addr+tmp_offset, len, buf);
		flash_read_page(addr+tmp_offset, len, tmp_buf);
		
		if (memcmp(tmp_buf, buf, len) == 0)
		{
			//write success, return success
			*offset = tmp_offset;
			return SAVE_SUCCESS;
		}
		else
		{
			//write fail, Erase storage area for retry again
			flash_erase_sector(addr);
			tmp_offset = 0;
		}
	}

	return SAVE_FAILED; //save fail
}

/**
 * @brief	save device information to flash
 * @param	none
 * @return	none
 */
void save_dev_info_flash()
{
	if (save_data_to_flash(CFG_DEVICE_MODE_ADDR, sizeof(FLASH_DEV_INFO_AAA), (u8 *)&flash_dev_info.dongle_id, &dev_info_idx) == SAVE_SUCCESS)
	{
		printf("---save success.\r\n");
	}
	else
	{
		printf("---save failed\r\n");
	}
}

#if WEB_HID_ENABLE

	void save_web_info_flash(void)
	{
		if (save_data_to_flash(CFG_WEB_DATA_ADDR, WEB_DATA_LENGTH_MAX,(u8*)&gc_web_data,&dev_web_info_idex) == SAVE_SUCCESS)
		{
			printf("web info save success.\r\n");
		}
		else
		{
			printf("web info save failed\r\n");
		}
	}
#endif

void ble_start_pair()
{
	set_pair_flag(); //set pair flag, when reboot finish will read back the flag and enter pair mode
	
	if (connect_ok)
	{
		//when BLE connecting, disconnect first and set disconnect reason = pair reboot
		active_disconnect_reason = BLE_PAIR_REBOOT_ANA_AAA;
	}
	else
	{
		//no connect, direct reboot
		user_reboot(BLE_PAIR_REBOOT_ANA_AAA);
	}
}

#if (KEY_MR_PAIR_NONE == KEY_MR_PAIR_OPTION)
	// do nothing
#else

void pair_key_check_aaa(u16 check_ms)
{
	static u32 key_press_hold_tick = 0;

#if (KEY_MR_PAIR_NONE == KEY_MR_PAIR_OPTION)
	if ( KEY_MR_PAIR_NONE )
#elif (KEY_MR_PAIR_G24_BLE == KEY_MR_PAIR_OPTION)
	#if ( PROJECT_ID == PID_BG523 )
		if ( ( pair_flag == 0 ) && ( COMB_BTN_LEFT_MIDDLE_RIGHT == (btn_value & COMB_BTN_LEFT_MIDDLE_RIGHT) ) )
	#elif ( PROJECT_ID == PID_M388 )
		if ( (fun_mode == RF_2M_2P4G_MODE) && ( pair_flag == 0 ) && ( COMB_BTN_LEFT_MIDDLE_RIGHT == (btn_value & COMB_BTN_LEFT_MIDDLE_RIGHT) ) )
	#else
		if ( (pair_flag == 0) && (btn_value == COMB_BTN_PAIR) )
	#endif
#elif (KEY_MR_PAIR_G24 == KEY_MR_PAIR_OPTION)

	#if (PROJECT_ID == PID_HM668)
		if ( (pair_flag == 0) && (btn_value & KEY_BTN_MODE) && (fun_mode == RF_2M_2P4G_MODE) )
	#else
		if ( (pair_flag == 0) && (btn_value == COMB_BTN_PAIR) && (fun_mode == RF_2M_2P4G_MODE) )
	#endif

#endif
	{
		//my_fifo_reset(&fifo_km); //clear fifo

		if ( clock_time_exceed(key_press_hold_tick, check_ms*1200) ) //press time enough, more than 3s
		{
			power_on_tick = 0;

		#if BLE_MODE_ENABLE
			if (fun_mode == RF_1M_BLE_MODE) 
			{
				ble_start_pair(); //call ble_start_pair();
			}
		#endif

		#if G24_MODE_ENABLE
			if (fun_mode == RF_2M_2P4G_MODE)
			{
				d24_start_pair(); //call d24_start_pair();
			}
		#endif
		}
	}
	else
	{
	 	key_press_hold_tick = clock_time(); //clear press count
	}

}

#endif

/**
 * @brief	mode change key check
 * @param	check_ms
 * @return	none
 */

unsigned char ble_switch_count_flag = 0;

void mode_change_check_aaa(u16 check_ms)
{
#if KEY_MODE_CHANGE_ENABLE
	static u32 key_press_hold_tick = 0;
	static u32 key_release_hold_tick = 0;
	static u8 mode_btn_release_cnt = 0;
	unsigned char need_change_mode = 0;
#endif

#if (SWITCH_MODE_ENABLE)

	if ( fun_mode == RF_2M_2P4G_MODE )
	{
		return;
	}

#endif

#if (PROJECT_ID == PID_601)
	if ( fun_mode == RF_USB_MODE )
	{
		return;
	}
#endif


#if (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)

	static u32 key_press_hold_tick = 0;

	if ( btn_value & KEY_BTN_MODE )
	{
		if ( (pair_flag == 0) && ( clock_time_exceed(key_press_hold_tick, 3*1000*1000) ) )
		{
			btn_value = 0;
			key_press_hold_tick = clock_time();

			if (fun_mode == RF_1M_BLE_MODE )
			{
				ble_start_pair();
			}
		}
	}
	else
	{
		key_press_hold_tick = clock_time();
	}

#endif


#if KEY_MODE_CHANGE_ENABLE
	if ( btn_value & KEY_BTN_MODE ) // key press
	{
	#if (KEY_MODE_PAIR_NONE == KEY_MODE_PAIR_OPTION)
		if ( KEY_MODE_PAIR_NONE )
	#elif (KEY_MODE_PAIR_BLE == KEY_MODE_PAIR_OPTION)
		if (  (pair_flag == 0) && ( clock_time_exceed(key_press_hold_tick, check_ms*1200) ) && (fun_mode == RF_1M_BLE_MODE) )
	#elif (KEY_MODE_PAIR_G24_BLE == KEY_MODE_PAIR_OPTION)
		if (  (pair_flag == 0) && clock_time_exceed(key_press_hold_tick, check_ms*1200) )
	#endif
		{
			power_on_tick = 0;
			btn_value = 0;
			last_btn_value = 0;

			if (fun_mode == RF_1M_BLE_MODE )
			{
			#if BLE_MODE_ENABLE
				ble_start_pair();
			#endif
			}
			else if (fun_mode == RF_2M_2P4G_MODE )
			{
			#if G24_MODE_ENABLE
				d24_start_pair();
			#endif
			}
		}

		key_release_hold_tick = clock_time();
	}
	else if( last_btn_value & KEY_BTN_MODE )
	{
		mode_btn_release_cnt++;
		if ( (pair_flag == 1) && (deep_flag == BLE_PAIR_REBOOT_ANA_AAA || deep_flag == CLEAR_FLAG_ANA_AAA) )
		{
			if( mode_btn_release_cnt >= 2 )
			{
				need_change_mode=1;
				printf("mode chage 11xx \n");
			}
		}
	#if (PROJECT_ID == PID_8693)
		else if ( deep_flag == DEEP_SLEEP_ANA_AAA  )
		{
			if( mode_btn_release_cnt >= 2 )
			{
				need_change_mode=1;
			}
		}
	#endif
		else
		{
			need_change_mode=1;
			printf("mode chage 22xx \n");
		}

	#if (SWITCH_MODE_ENABLE)
		if ( fun_mode == RF_1M_BLE_MODE )
	#elif KEY_MODE_CHANGE_G24_BLE_ENABLE
		if ( !KEY_MODE_CHANGE_G24_BLE_ENABLE )
	#elif KEY_MODE_ONLY_BLE_ENABLE
		if ( KEY_MODE_ONLY_BLE_ENABLE )
	#else
		if ( (fun_mode == RF_1M_BLE_MODE) && (flash_dev_info.mast_id < 1) )
	#endif
		{
			if ( (device_switch_flag == 0) && (need_change_mode==1) )
			{
				device_switch_flag = 1;

				flash_dev_info.mast_id ++;
				flash_dev_info.mode = RF_1M_BLE_MODE;

				if ( flash_dev_info.mast_id >= BLE_DEVICE_ID_MAX )
				{
					flash_dev_info.mast_id = BLE_DEVICE_ID_0;
				}

				if (connect_ok)
				{ //when BLE connecting, disconnect first and set disconnect reason = switch muti_channel
					active_disconnect_reason = MUTI_DEVICE_REBOOT_ANA_AAA;

					ble_switch_count_flag ++;
				}
				else
				{ //no connect, direct reboot
					clear_pair_flag();
					save_dev_info_flash();

					sleep_ms(50);
					user_reboot(MUTI_DEVICE_REBOOT_ANA_AAA);
				}
			}
		}
		else if ( (mode_change_flag == 0) && (need_change_mode==1) )
		{

			mode_change_flag = 1;
			clear_pair_flag();
			if (fun_mode == RF_1M_BLE_MODE)
			{
				if (connect_ok)
				{ //when BLE connecting, disconnect first and set disconnect reason = switch mode change
					active_disconnect_reason = MODE_CHANGE_REBOOT_ANA_AAA;

					ble_switch_count_flag ++;
				}
				else
				{
					flash_dev_info.mode = RF_2M_2P4G_MODE;

					save_dev_info_flash();
					sleep_ms(50);
					user_reboot(MODE_CHANGE_REBOOT_ANA_AAA);
				}
			}
			else //when 2.4G mode, direct reboot
			{ 
				flash_dev_info.mode = RF_1M_BLE_MODE;
				flash_dev_info.mast_id = 0;

				save_dev_info_flash();
				sleep_ms(50);
				user_reboot(MODE_CHANGE_REBOOT_ANA_AAA);
			}
		}
	}
	else
	{
		if (mode_change_flag || device_switch_flag )
		{
			if (key_release_hold_tick == 0)
			{
				key_release_hold_tick = clock_time();
			}
			else if (clock_time_exceed(key_release_hold_tick, 100*1000)) //release time enough
			{
				mode_change_flag = 0;
				device_switch_flag = 0;
			}
		}
		key_press_hold_tick = clock_time(); //clear press count
	}
#endif


}

#if KEY_PRESS_CHANGE_MODE_ENABLE

void key_pressed_mode_change_pro(void)
{
	static u32 key_release_hold_tick_2 = 0;
	static u8 mode_btn_release_cnt_2 = 0;

	unsigned char need_change_mode = 0;

	if ( btn_value & KEY_BTN_MODE ) // key press
	{
		mode_btn_release_cnt_2++;
		if ( (pair_flag == 1) && (deep_flag == BLE_PAIR_REBOOT_ANA_AAA || deep_flag == CLEAR_FLAG_ANA_AAA) )
		{
			if( mode_btn_release_cnt_2 >= 2 )
			{
				need_change_mode=1;
			}
		}
		else
		{
			need_change_mode=1;
		}

		if ( (mode_change_flag == 0) && (need_change_mode==1) )
		{
			mode_change_flag = 1;
			clear_pair_flag();
			if (fun_mode == RF_1M_BLE_MODE)
			{
				if (connect_ok)
				{ //when BLE connecting, disconnect first and set disconnect reason = switch mode change
					active_disconnect_reason = MODE_CHANGE_REBOOT_ANA_AAA;

					ble_switch_count_flag ++;
				}
				else
				{
					flash_dev_info.mode = RF_2M_2P4G_MODE;

					save_dev_info_flash();
					sleep_ms(50);
					user_reboot(MODE_CHANGE_REBOOT_ANA_AAA);
				}
			}
			else //when 2.4G mode, direct reboot
			{
				flash_dev_info.mode = RF_1M_BLE_MODE;
				flash_dev_info.mast_id = 0;

				save_dev_info_flash();
				sleep_ms(50);
				user_reboot(MODE_CHANGE_REBOOT_ANA_AAA);
			}
		}
	}
	else
	{
		if (mode_change_flag || device_switch_flag )
		{
			if (key_release_hold_tick_2 == 0)
			{
				key_release_hold_tick_2 = clock_time();
			}
			else if (clock_time_exceed(key_release_hold_tick_2, 100*1000)) //release time enough
			{
				mode_change_flag = 0;
				device_switch_flag = 0;
			}
		}
	}

}

#endif

#if (PROJECT_ID == PID_G929)

void g929_mode_change_check(void)
{
	static u32 key_press_hold_tick = 0;
	static u32 key_release_hold_tick = 0;
	static u8 mode_btn_release_cnt = 0, mode_btn_change_flag = 0;
	unsigned char need_change_mode = 0;

	if ( (btn_value & KEY_BTN_MODE) && (0 == mode_btn_change_flag) ) // key press
	{
		mode_btn_release_cnt++;

		if ( (pair_flag == 1) && (deep_flag == BLE_PAIR_REBOOT_ANA_AAA || deep_flag == CLEAR_FLAG_ANA_AAA) )
		{
			if( mode_btn_release_cnt >= 2 )
			{
				need_change_mode=1;
				mode_btn_change_flag = 1;
			}
		}
	#if (PROJECT_ID == PID_8693)
		else if ( deep_flag == DEEP_SLEEP_ANA_AAA  )
		{
			if( mode_btn_release_cnt >= 2 )
			{
				need_change_mode=1;
			}
		}
	#endif
		else
		{
			need_change_mode=1;
			mode_btn_change_flag = 1;
		}


		if ( (fun_mode == RF_1M_BLE_MODE) && (flash_dev_info.mast_id < 1) )
		{
			if ( (device_switch_flag == 0) && (need_change_mode==1) )
			{
				device_switch_flag =1;

				flash_dev_info.mast_id ++;
				flash_dev_info.mode = RF_1M_BLE_MODE;

				if ( flash_dev_info.mast_id >= BLE_DEVICE_ID_MAX )
				{
					flash_dev_info.mast_id = BLE_DEVICE_ID_0;
				}

				if (connect_ok)
				{ //when BLE connecting, disconnect first and set disconnect reason = switch muti_channel
					active_disconnect_reason = MUTI_DEVICE_REBOOT_ANA_AAA;

					ble_switch_count_flag ++;
				}
				else
				{ //no connect, direct reboot
					clear_pair_flag();
					save_dev_info_flash();

					sleep_ms(50);
					user_reboot(MUTI_DEVICE_REBOOT_ANA_AAA);
				}
			}
		}
		else if ( (mode_change_flag == 0) && (need_change_mode==1) )
		{

			mode_change_flag = 1;
			clear_pair_flag();
			if (fun_mode == RF_1M_BLE_MODE)
			{
				if (connect_ok)
				{ //when BLE connecting, disconnect first and set disconnect reason = switch mode change
					active_disconnect_reason = MODE_CHANGE_REBOOT_ANA_AAA;

					ble_switch_count_flag ++;
				}
				else
				{
					flash_dev_info.mode = RF_2M_2P4G_MODE;

					save_dev_info_flash();
					sleep_ms(50);
					user_reboot(MODE_CHANGE_REBOOT_ANA_AAA);
				}
			}
			else //when 2.4G mode, direct reboot
			{
				flash_dev_info.mode = RF_1M_BLE_MODE;
				flash_dev_info.mast_id = 0;

				save_dev_info_flash();
				sleep_ms(50);
				user_reboot(MODE_CHANGE_REBOOT_ANA_AAA);
			}
		}
	}
	else
	{
		if (mode_change_flag || device_switch_flag)
		{
			if (key_release_hold_tick == 0)
			{
				key_release_hold_tick = clock_time();
			}
			else if (clock_time_exceed(key_release_hold_tick, 100*1000)) //release time enough
			{
				mode_change_flag = 0;
				device_switch_flag = 0;
			}
		}
		key_press_hold_tick = clock_time(); //clear press count
	}

}
#endif


#if (SWITCH_MODE_ENABLE)

static unsigned short sg_io_change_count = 0;

void key_switch_change_pro(void)
{
	unsigned char sw_state = gpio_read(MODE_SWITCH_PIN);

	#if (SWITCH_MODE_IO_TOGGLE_ENABLE)
		sw_state = !sw_state;
	#endif

	if (  g_mode_io_status == sw_state )
	{
		sg_io_change_count = 0;
	}
	else
	{
		sg_io_change_count++;
	}

	if ( sg_io_change_count >= IO_CHANGE_COUNT_MAX )
	{
		clear_pair_flag();
		if (fun_mode == RF_1M_BLE_MODE)
		{
			if (connect_ok)
			{ //when BLE connecting, disconnect first and set disconnect reason = switch mode change
				active_disconnect_reason = MODE_CHANGE_REBOOT_ANA_AAA;
			}
			else
			{ //no connect, direct reboot
				flash_dev_info.mode = RF_2M_2P4G_MODE;
			#if (SWITCH_MODE_ENABLE)
				// do noyhing
			#else
				save_dev_info_flash();
			#endif

				user_reboot(MODE_CHANGE_REBOOT_ANA_AAA);
			}
		}
		else //when 2.4G mode, direct reboot
		{ 
			flash_dev_info.mode = RF_1M_BLE_MODE;

		#if (SWITCH_MODE_ENABLE)
			// do noyhing
		#else
			save_dev_info_flash();
		#endif

			user_reboot(MODE_CHANGE_REBOOT_ANA_AAA);
		}
	}

}
#endif

/**
 * @brief	user_button_status_check
 * @param	none
 * @return	0:button invalid, 1:button valid
 */
u8 user_button_status_check(void)
{
	u8 button_status_valid = 0;
	
	switch (btn_value)
	{
		case COMB_BTN_PAIR: 	//pair key
		case COMB_BTN_EMI: 		//EMI key
		case MS_BTN_AUTO_DRAW: 	//mode change key
		case KEY_BTN_K5: 		//mode change key
		case MS_BTN_REPORT_RATE://BLE channel switch key
		case KEY_BTN_K4: 		//BLE channel switch key
			button_status_valid = 1;
			break;
		default:
			break;
	}

	return button_status_valid;
}


#if (MOUSE_CODE_LOCK_ENABLE)
	void BG523_active_action(void)
	{
		static unsigned int sg_bg5233_time = 0;

		if ( 0xAA != flash_dev_info.active_flag )
		{
			if ( KEY_LEFT_RIGHT_MIDDLE_COMBINE == (btn_value & KEY_LEFT_RIGHT_MIDDLE_COMBINE)  )
			{
				if ( clock_time_exceed(sg_bg5233_time, 5*1000*1000)  )
				{
					sg_bg5233_time = clock_time();
					flash_dev_info.active_flag = 0xAA;
					save_dev_info_flash();
					gpio_write( GPIO_PA4, LED_ON );
					gpio_write( GPIO_PA3, LED_ON );
				}
			}
			else
			{
				sg_bg5233_time = clock_time();
			}
		}
	}
#endif

/**
 * @brief	Key detection and processing
 * @param	none
 * @return	none
 */

#if (KEY_DPI_DEFAULT_ENABLE)
	static unsigned char sg_dpi_default_flag = 0;
	static unsigned int sg_dpi_default_time = 0;
#endif

#if M0018_KEY_DPI_COMBINE_ENABLE

	static unsigned char m0018_dpi_set_flag = 0;
	static unsigned int m0018_dpi_default_time = 0;

	void m0018_dpi_none_set_dpi(void)
	{
		if ( 0 == connect_ok )
		{
			m0018_dpi_set_flag = 0;
			m0018_dpi_default_time = clock_time() | 0x01;
			return;
		}

		if ( KEY_DPI_SET == (btn_value & KEY_DPI_SET)  )
		{
			if ( !m0018_dpi_set_flag && clock_time_exceed(m0018_dpi_default_time, 3*1000*1000) )
			{
				m0018_dpi_set_flag = 1;
				m0018_dpi_default_time = clock_time() | 0x01;
				btn_dpi_set();
			}
		}
		else
		{
			m0018_dpi_set_flag = 0;
			m0018_dpi_default_time = clock_time() | 0x01;
		}
	}

#endif

#if KEY_LEFT_RIGHT_MIDDLE_G24_PAIR_ENABLE

	static unsigned int sg_g24_pair_lrm_time = 0;

	void key_pair_g24_LRM(void)
	{
		if ( KEY_LEFT_RIGHT_MIDDLE_COMBINE == (btn_value & KEY_LEFT_RIGHT_MIDDLE_COMBINE) )
		{
			if ( (RF_2M_2P4G_MODE == fun_mode) && (0 == pair_flag) && clock_time_exceed(sg_g24_pair_lrm_time, 3*1000*1000) )
			{
				sg_g24_pair_lrm_time = clock_time();
			#if G24_MODE_ENABLE
				d24_start_pair();
			#endif
			}
		}
		else
		{
			sg_g24_pair_lrm_time = clock_time();
		}
	}
#endif

#if KEY_RIGHT_MIDDLE_COMBINE_ENABLE

	static unsigned int sg_check_lrm_time = 0;
	static unsigned char sg_code_sw_counter = 0;

	void key_middle_right_set_func(void)
	{

		if ( 0 == connect_ok )
		{
			sg_check_lrm_time = clock_time();
			sg_code_sw_counter = 0;
		}

		if ( KEY_RIGHT_MIDDLE_COMBINE == (btn_value & KEY_RIGHT_MIDDLE_COMBINE) )
		{
			if ( clock_time_exceed(sg_check_lrm_time, 100*1000) )
			{
				sg_check_lrm_time = clock_time();

				if ( sg_code_sw_counter < 100 )
				{
					sg_code_sw_counter++;
				}

				if ( sg_code_sw_counter == 30 )
				{
					flash_dev_info.code_led_sw = ( flash_dev_info.code_led_sw ) ? (0):(1);

					if ( 0 == flash_dev_info.code_led_sw )
					{
						LED_CODE_POWER_ON();
					}
					else
					{
						LED_CODE_POWER_OFF();
					}

					printf("led_code 11 = %d, %d \n", flash_dev_info.code_led_sw, flash_dev_info.code_led_mode);
					save_dev_info_flash();

				}
			}
		}
		else
		{
			if ( sg_code_sw_counter >= 1 && sg_code_sw_counter <= 15 )
			{
				if ( 0 == flash_dev_info.code_led_sw )
				{
					if ( ++flash_dev_info.code_led_mode >= RGB_MODE_LIGHTS_MAX )
					{
						flash_dev_info.code_led_mode = RGB_MODE_LIGHTS_NUION;
					}
					printf("led_code 22 = %d, %d \n", flash_dev_info.code_led_sw, flash_dev_info.code_led_mode);
					save_dev_info_flash();
				}
			}

			sg_check_lrm_time = clock_time();
			sg_code_sw_counter = 0;
		}
	}

#endif

#if MOUSE_FACTORY_RESET_ENABLE

static unsigned int sg_factory_time = 0;
static factory_step_e sg_factory_flag = FACTORY_KEY_NONE;
static unsigned char sg_factory_counter = 0;

void mouse_factory_release_times(void)
{
	if ( sg_factory_counter <= MOUSE_FACTORY_KEY_RELEASE_TIMES )
	{
		sg_factory_counter++;
	}
}

factory_step_e mouse_factory_reset_flag(void)
{
	return (sg_factory_flag);
}

void mouse_factory_power_on_flag(void)
{
	sg_factory_flag = FACTORY_KEY_POWER_UP;
}

void mouse_factory_reset_action(void)
{
#if (MODULE_WATCHDOG_ENABLE)
	wd_clear();
#endif

#if (MODULE_32K_WATCHDOG_ENABLE)
	wd_32k_clear();
#endif

    flash_dev_info.mode = RF_2M_2P4G_MODE;
    flash_dev_info.mast_id = 0;

    flash_dev_info.slave_mac_addr[0] = 0;
    flash_dev_info.slave_mac_addr[1] = 0;
    flash_dev_info.slave_mac_addr[2] = 0;
    flash_dev_info.slave_mac_addr[3] = 0;
    flash_dev_info.dongle_id = 0;

	flash_dev_info.d24g_dpi = 0xFF;
	flash_dev_info.ble_dpi[0] = 0xFF;
	flash_dev_info.ble_dpi[1] = 0xFF;
	flash_dev_info.ble_dpi[2] = 0xFF;

	flash_erase_sector(FLASH_ADR_SMP_PAIRING);
	flash_erase_sector(CFG_MAST_ADDR);
	flash_erase_sector(CFG_ADR_MAC);

	save_dev_info_flash();

	sleep_ms(100);
}

void mouse_factory_reset_loop(void)
{
#if (PROJECT_ID == PID_0120)

	unsigned char i = 0, keysta = 0;

	if ( power_on_key_red( PIN_BTN_MIDDLE ) && power_on_key_red(PIN_BTN_RIGHT) )
	{

		if ( FACTORY_KEY_POWER_UP == sg_factory_flag )
		{
			#if (MODULE_32K_WATCHDOG_ENABLE)
				wd_32k_clear();
			#endif

			do
			{
				sleep_ms(10);
				i++;
				keysta = 0;
				if ( power_on_key_red( PIN_BTN_MIDDLE ) && power_on_key_red(PIN_BTN_RIGHT) )
				{
					keysta = 1;
				}

			}while( (i <= 10) && (1 == keysta) );

			sg_factory_time = clock_time() | 0x01;
			sg_factory_flag = FACTORY_KEY_RELEASE;

			if ( (i >= 10) && (1 == keysta) )
			{
				led_adv_all_off();
				mouse_factory_reset_action();
				led_code_factory_reset();
				user_reboot( FACTORY_RESET_AAA );
			}
		}
	}
	else
	{
		sg_factory_time = clock_time() | 0x01;
		sg_factory_flag = FACTORY_KEY_NONE;
	}

#else

	if ( MOUSE_FACTORY_RESET_PIN == (btn_value & MOUSE_FACTORY_RESET_PIN) )
	{
		if ( (FACTORY_KEY_POWER_UP == sg_factory_flag) && clock_time_exceed(sg_factory_time, 1*1000*1000) )
		{
			sg_factory_time = clock_time() | 0x01;
			sg_factory_flag = FACTORY_KEY_RELEASE;
		}
	}
	else
	{
		sg_factory_time = clock_time() | 0x01;
		if ( FACTORY_KEY_RELEASE != sg_factory_flag )
		{
			sg_factory_flag = FACTORY_KEY_NONE;
		}
	}

	if ( FACTORY_KEY_RELEASE == sg_factory_flag )
	{
		if ( connect_ok )
		{
			if ( sg_factory_counter >= MOUSE_FACTORY_KEY_RELEASE_TIMES )
			{
				sg_factory_counter = 0;
				sg_factory_flag = FACTORY_REBOOT;
			}
		}
		else
		{
			sg_factory_flag = FACTORY_REBOOT;
		}

		return;
	}

	if ( FACTORY_REBOOT == sg_factory_flag )
	{
		sg_factory_flag = FACTORY_OVER;
		if ( connect_ok )
		{
			if ( RF_2M_2P4G_MODE == fun_mode )
			{
				mouse_factory_reset_action();
				user_reboot(POWER_ON_ANA_AAA);
			}
			else
			{
				active_disconnect_reason = BLE_PAIR_REBOOT_ANA_AAA;
			}
		}
		else
		{
			mouse_factory_reset_action();
			user_reboot(POWER_ON_ANA_AAA);
		}

		return;
	}

#endif

}

#endif


#if LED_COLOR_MODE_ENABLE

void code_led_color_mode_change(void)
{
	static u32 gc_code_led_color_time = 0;
	static u8 gc_code_led_color_set_flag = 0;

	if ( 0 == connect_ok )
	{
		gc_code_led_color_time = clock_time();
		return;
	}

	if ( (btn_value == LED_COLOR_SET_KEY) && (0 == gc_code_led_color_set_flag) )
	{
		if ( clock_time_exceed(gc_code_led_color_time, 3*1000*1000) )
		{
			gc_code_led_color_set_flag = 1;

			if ( ++flash_dev_info.code_led_mode >= RGB_MODE_LIGHTS_MAX )
			{
				flash_dev_info.code_led_mode = RGB_MODE_LIGHTS_NUION;
			}

			#if LED_CODE_PWIR_DRIVE_ENABLE
				ws2812_mode_change_para_clear();
			#else
				coled_led_mode_change_para_clear();
			#endif

			save_dev_info_flash();
		}
	}
	else
	{
		gc_code_led_color_time = clock_time();
		gc_code_led_color_set_flag = 0;
	}

}

#endif


void user_button_check_proc(void)
{

#if LED_COLOR_MODE_ENABLE
	code_led_color_mode_change();
#endif

#if M0018_KEY_DPI_COMBINE_ENABLE
	m0018_dpi_none_set_dpi();
#endif

#if	MOUSE_FACTORY_RESET_ENABLE
	mouse_factory_reset_loop();
#endif

#if (KEY_DPI_DEFAULT_ENABLE)
	if ( KEY_DEFAULT_VALUE == (btn_value & KEY_DEFAULT_VALUE)  )
	{
		if ( !sg_dpi_default_flag  && clock_time_exceed(sg_dpi_default_time, 5*1000*1000) )
		{
			sg_dpi_default_flag = 1;
			sg_dpi_default_time = clock_time() | 0x01;
			sensor_dpi_default();
		}
	}
	else
	{
		sg_dpi_default_flag = 0;
		sg_dpi_default_time = clock_time() | 0x01;
	}
#endif

#if KEY_LEFT_RIGHT_MIDDLE_G24_PAIR_ENABLE
	key_pair_g24_LRM();
#endif

#if KEY_RIGHT_MIDDLE_COMBINE_ENABLE
	key_middle_right_set_func();
#endif

#if (KEY_MR_PAIR_NONE == KEY_MR_PAIR_OPTION)
	// do nothing
#elif (KEY_MR_PAIR_G24 == KEY_MR_PAIR_OPTION) || (KEY_MR_PAIR_G24_BLE == KEY_MR_PAIR_OPTION)
	pair_key_check_aaa(KEY_PRESS_HOLD_CHECK_TIME3);
#endif

#if (MOUSE_CODE_LOCK_ENABLE)
	BG523_active_action();
#endif

	mode_change_check_aaa(KEY_PRESS_HOLD_CHECK_TIME3); //KEY_BTN_MODE, press 3s to enter

#if KEY_PRESS_CHANGE_MODE_ENABLE
	key_pressed_mode_change_pro();
#endif

#if (PROJECT_ID == PID_G929)
	g929_mode_change_check();
#endif

#if (SWITCH_MODE_ENABLE)
	key_switch_change_pro();
#endif

	if (power_on_tick && (!clock_time_exceed(power_on_tick, (KEY_PRESS_HOLD_CHECK_TIME3 + 200) * 1000)))
	{ /********** check button during power-on ***********/
		//pair_key_check_aaa(KEY_PRESS_HOLD_CHECK_TIME2); //(KEY_BTN_LEFT|KEY_BTN_MIDDLE|KEY_BTN_RIGHT)

	#if EMI_TEST_FUN_ENABLE_AAA
		emi_key_check_aaa(KEY_PRESS_HOLD_CHECK_TIME3); //(KEY_BTN_RIGHT | KEY_BTN_MIDDLE), press 3s to enter
	#endif
	}
	else
	{ /********** check button after power-on is complete***********/
		power_on_tick = 0; //clear power on tick
    #if G24_MODE_ENABLE
		#if (ENTER_PAIR_WHEN_NEVER_PAIRED_ENABLE == 0)
			/*  when a unpaired device is powered on, enters deep sleep */
			if ((fun_mode == RF_2M_2P4G_MODE) && (pair_flag == 0) && (dongle_id == 0) && (device_led.repeatCount == 0) && (device_led.isOn == 0))
			{
				printf("(fun_mode == RF_2M_2P4G_MODE) && (pair_flag == 0)\n");
				enter_deep_aaa(DEEP_SLEEP_ANA_AAA);
			}
		#endif
    #endif
	}

#if G24_MODE_ENABLE

	/* when dongle_id is valid, if reconnect overtime(200ms), then switch to normal mode */
	if ((device_status == STATE_SYSNC) && (gc_mouse_sta.dongle_id_valid_f == 1))
	//if ((device_status == STATE_SYSNC) && (g24_flags.dongle_id_valid == 1))
	{
		if (clock_time_exceed(adv_begin_tick, 200*1000)) //reconnect over time
		{
			//if (btn_value == 0)
			{
				printf("******** Device return to normal mode ********.\n");

			#if (AES_METHOD == 1)
				u32 device_id = ((user_cfg.dev_mac & 0xFFFFFF00) | DEVICE_TYPE_INDEX);
				memcpy((u8*)&private_key[0], (u8*)&device_id, 4);
				memcpy((u8*)&private_key[4], (u8*)&flash_dev_info.key[0], 12);
			#endif

				device_status = STATE_NORMAL;

				//my_fifo_reset(&fifo_km); //Clear fifo

				gc_mouse_sta.mouse_send_need_f = 0; //clear send flag
				//g24_flags.mouse_send_need = 0;
				
				extern volatile u32 no_ack;
				no_ack = 0; //clear no_ack count
			}
		}
	}
#endif
}


/**
 * @brief	button process
 * @param	event_new
 * @return	none
 */
void button_process(u8 event_new)
{
#if (PROJECT_ID == PID_G929) || (PROJECT_ID == PID_4028) || (PROJECT_ID == PID_BG523) || (PROJECT_ID == PID_4027) || (PROJECT_ID == PID_HM668)
	// do nothing
#elif (PROJECT_ID == PID_4027_DRY) && (M4027_DRY_H2_ENABLE)
	// do nothing
#elif (PROJECT_ID == PID_S600)
	// do nothing
#else
    if ( (last_btn_value & KEY_BTN_CPI) && event_new )
    {
        if (connect_ok)
        {
            btn_dpi_set();
            printf("---->dpi set led dis\n");
        }
    }
#endif

#if (PROJECT_ID == PID_MS13)
    static unsigned char sc_dpi_default_flag = 0;
    static unsigned int sc_dpi_default_time = 0;

    if ( last_btn_value & KEY_BTN_MIDDLE )
    {
    	if ( connect_ok )
    	{
    		if ( 0 == sc_dpi_default_flag && clock_time_exceed(sc_dpi_default_time, 5*1000*1000) )
    		{
    			sensor_dpi_default();
    			sc_dpi_default_time = clock_time();
    			sc_dpi_default_flag = 1;
    		}
    	}

    	if ( 0 == connect_ok || 1 == sc_dpi_default_flag )
    	{
    		sc_dpi_default_time = clock_time();
    	}
    }
    else
    {
    	sc_dpi_default_flag = 0;
    	sc_dpi_default_time = clock_time();
    }
#endif


#if (PROJECT_ID == PID_G929) || (PROJECT_ID == PID_BG523)
    static unsigned int  sv_cpi_press_time = 0;
    static unsigned char sv_cpi_press_cnt = 0, sv_cpi_flag = 0;


    if ( 0 == sv_cpi_flag || 0 == gc_dpi_func2_flag )
    {
    	if ( (last_btn_value & KEY_BTN_CPI) || (btn_value & KEY_BTN_CPI) )
    	{
    		sv_cpi_flag = 0;
    	}
    	else
    	{
    		sv_cpi_flag = 1;
    	}
    	return;
    }

	if ( last_btn_value != KEY_BTN_CPI || btn_value != KEY_BTN_CPI )
	{
		sv_cpi_press_time = clock_time();
	}
	else
	{
		if( clock_time_exceed(sv_cpi_press_time, 20*1000) )
		{
			sv_cpi_press_time = clock_time();
			if ( sv_cpi_press_cnt < 250 && ++sv_cpi_press_cnt >= 110 )
			{
				sv_cpi_press_cnt = 250;
				btn_value |= KEY_BTN_MODE;
			}
		}
	}

	if ( (last_btn_value & KEY_BTN_CPI) && event_new )
	{
		if ( sv_cpi_press_cnt >= 1 && sv_cpi_press_cnt <= 50 )
		{
			if ( connect_ok )
			{
				btn_dpi_set();
			}
		}
		sv_cpi_press_cnt = 0;
	}
#endif


#if (PROJECT_ID == PID_4028) && (0 == M4028_BLE_G24_MODE_ENABLE)
    static unsigned int  sv_cpi_press_time = 0;
    static unsigned char sv_cpi_press_cnt = 0, sv_cpi_flag = 0;

    if ( 0 == sv_cpi_flag )
    {
    	if ( (last_btn_value & KEY_BTN_CPI) || (btn_value & KEY_BTN_CPI) )
    	{
    		sv_cpi_flag = 0;
    	}
    	else
    	{
    		sv_cpi_flag = 1;
    	}
    	return;
    }

	if ( last_btn_value != KEY_BTN_CPI || btn_value != KEY_BTN_CPI )
	{
		sv_cpi_press_time = clock_time();
	}
	else
	{
		if( clock_time_exceed(sv_cpi_press_time, 20*1000) )
		{
			sv_cpi_press_time = clock_time();
			if ( sv_cpi_press_cnt < 250 && ++sv_cpi_press_cnt >= 150 )
			{
				sv_cpi_press_cnt = 250;
				if ( 0 == pair_flag )
				{
					ble_start_pair();
				}
			}
		}
	}

	if ( (last_btn_value & KEY_BTN_CPI) && event_new )
	{
		if ( sv_cpi_press_cnt >= 1 && sv_cpi_press_cnt <= 50 )
		{
			if ( connect_ok )
			{
				btn_dpi_set();
			}
		}
		sv_cpi_press_cnt = 0;
	}
#endif



#if (PROJECT_ID == PID_4027) || (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_S600)

    static unsigned int  sv_cpi_press_time = 0;
    static unsigned char sv_cpi_press_cnt = 0, sv_cpi_flag = 0;

    if ( 0 == sv_cpi_flag )
    {
    	if ( (last_btn_value & KEY_BTN_CPI) || (btn_value & KEY_BTN_CPI) )
    	{
    		sv_cpi_flag = 0;
    	}
    	else
    	{
    		sv_cpi_flag = 1;
    	}
    	return;
    }

	if ( last_btn_value != KEY_BTN_CPI || btn_value != KEY_BTN_CPI )
	{
		sv_cpi_press_time = clock_time();
	}
	else
	{
		if( clock_time_exceed(sv_cpi_press_time, 20*1000) )
		{
			sv_cpi_press_time = clock_time();
			if ( sv_cpi_press_cnt < 250 && ++sv_cpi_press_cnt >= 95 )
			{
				sv_cpi_press_cnt = 250;

			#if (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_S600) || (PROJECT_ID == PID_S600)
				if ( connect_ok )
				{
					flash_dev_info.code_led_sw = ( flash_dev_info.code_led_sw ) ? (0):(1);

					if ( 0 == flash_dev_info.code_led_sw )
					{
						LED_CODE_POWER_ON();
					}
					else
					{
						LED_CODE_POWER_OFF();
					}

					save_dev_info_flash();
				}
			#else
				if ( 0 == pair_flag )
				{
					if ( RF_1M_BLE_MODE == fun_mode )
					{
						ble_start_pair();
					}
					else
					{
						d24_start_pair();
					}
				}
			#endif
			}
		}
	}

	if ( (last_btn_value & KEY_BTN_CPI) && event_new )
	{
		if ( sv_cpi_press_cnt >= 1 && sv_cpi_press_cnt <= 40 )
		{
			if ( connect_ok )
			{
				btn_dpi_set();
			}
		}
		sv_cpi_press_cnt = 0;
	}

#endif



#if (PROJECT_ID == PID_4027_DRY) && (M4027_DRY_H2_ENABLE)

    static unsigned int  sv_cpi_press_time = 0;
    static unsigned char sv_cpi_press_cnt = 0, sv_cpi_flag = 0;

    if ( 0 == sv_cpi_flag )
    {
    	if ( (last_btn_value & KEY_BTN_CPI) || (btn_value & KEY_BTN_CPI) )
    	{
    		sv_cpi_flag = 0;
    	}
    	else
    	{
    		sv_cpi_flag = 1;
    	}
    	return;
    }

	if ( last_btn_value != KEY_BTN_CPI || btn_value != KEY_BTN_CPI )
	{
		sv_cpi_press_time = clock_time();
	}
	else
	{
		if( clock_time_exceed(sv_cpi_press_time, 20*1000) )
		{
			sv_cpi_press_time = clock_time();
			if ( sv_cpi_press_cnt < 250 && ++sv_cpi_press_cnt >= 95 )
			{
				sv_cpi_press_cnt = 250;
				if ( 0 == pair_flag )
				{
					if ( RF_1M_BLE_MODE == fun_mode )
					{
						ble_start_pair();
					}
					else
					{
						d24_start_pair();
					}
				}
			}
		}
	}

	if ( (last_btn_value & KEY_BTN_CPI) && event_new )
	{
		if ( sv_cpi_press_cnt >= 1 && sv_cpi_press_cnt <= 40 )
		{
			if ( connect_ok )
			{
				btn_dpi_set();
			}
		}
		sv_cpi_press_cnt = 0;
	}

#endif


#if (LED_COLOR_MODE_KEY_ENABLE)

    static unsigned int  sv_cm_press_time = 0;
    static unsigned char sv_cm_press_cnt = 0, sv_cm_flag = 0;

    if ( 0 == sv_cm_flag )
    {
    	if ( (last_btn_value & KEY_COLOR_VALUE) || (btn_value & KEY_COLOR_VALUE) )
    	{
    		sv_cm_flag = 0;
    	}
    	else
    	{
    		sv_cm_flag = 1;
    	}
    	return;
    }

	if ( last_btn_value != KEY_COLOR_VALUE || btn_value != KEY_COLOR_VALUE )
	{
		sv_cm_press_time = clock_time();
	}
	else
	{
		if( clock_time_exceed(sv_cm_press_time, 20*1000) )
		{
			sv_cm_press_time = clock_time();
			if ( sv_cm_press_cnt < 250 && ++sv_cm_press_cnt >= 95 )
			{
				sv_cm_press_cnt = 250;

				if ( connect_ok )
				{
					flash_dev_info.code_led_sw = ( flash_dev_info.code_led_sw ) ? (0):(1);

					if ( 0 == flash_dev_info.code_led_sw )
					{
						LED_CODE_POWER_ON();
					}
					else
					{
						LED_CODE_POWER_OFF();
					}

					save_dev_info_flash();
				}

			}
		}
	}

	if ( (last_btn_value & KEY_COLOR_VALUE) && event_new )
	{
		if ( sv_cm_press_cnt >= 1 && sv_cm_press_cnt <= 40 )
		{
			if ( ++flash_dev_info.code_led_mode >= RGB_MODE_LIGHTS_MAX )
			{
				flash_dev_info.code_led_mode = RGB_MODE_LIGHTS_NUION;
			}
		#if LED_CODE_PWIR_DRIVE_ENABLE
			ws2812_mode_change_para_clear();
		#else
			coled_led_mode_change_para_clear();
		#endif
			save_dev_info_flash();
		}
		sv_cm_press_cnt = 0;
	}

#endif

}

u8 btn_get_value()
{
    u8 ret = 0;
    _attribute_data_retention_  static u8 debounce = 0;
    u16 now_value = 0;
    _attribute_data_retention_ static u16 last_value = 0;

    now_value = btn_scan(); // get button value

    if (last_value != now_value) // has new button value
    {
        reset_idle_status();
        debounce = 1; // start debounce times
        last_value = now_value;
    }

    if (debounce)
    {
        debounce++; 
        if (debounce >= 3) // debounce times == 3
        {
            ret = NEW_KEY_EVENT_AAA; // new button valid

        #if (KEY_WEB_ROLLER_ENABLE)
            ms_data.wheel_level = 0;
            if( (MS_BTN_ROLLER_LEFT == now_value) || (MS_BTN_ROLLER_RIGHT == now_value) )
            {
                ms_data.wheel_level = (MS_BTN_ROLLER_LEFT == now_value)?(1):(-1);
                now_value = 0;
            }
       #endif

        #if (KEY_FEATURE_DESKTOP_ENABLE)
            ms_data.btn = now_value & 0x1F; 
            if( fun_mode == RF_1M_BLE_MODE || fun_mode == RF_2M_2P4G_MODE)
            {
                if ( MS_BTN_DESKTOP == now_value ) {
                    ms_desktop_flag = 1;
                }
                else {
                    ms_desktop_flag = 0;
                }
            }
		#elif KEY_FEATURE_BOSSKEY_ENABLE

			ms_data.btn = now_value & 0x1F;

            if ( now_value == KEY_FEATURE_BOSSKEY_VALUE )
            {
            	if ( RF_2M_2P4G_MODE == fun_mode )
            	{
            		ms_data.btn = 0x80; // boss key
            	}
				else if ( RF_1M_BLE_MODE == fun_mode )
				{
					gc_key_special_sta = KEY_SPE_BOSSKEY;
				}
            }
        #elif (PID_LV600_KEY_ENABLE)
            if ( KEY_LPV600_CTRLZ & now_value )
            {
                if ( fun_mode == RF_1M_BLE_MODE )
                {
                    gc_lpv600_key_spec_flag = PID_LPV600_KEY_CTR_Z;
                    ms_data.btn = now_value & 0x1F;
                }
                else
                {
                    ms_data.btn = 0x20;
                }
            }
            else if (KEY_LPV600_ENTER & now_value )
            {
                if ( fun_mode == RF_1M_BLE_MODE )
                {
                    gc_lpv600_key_spec_flag = PID_LPV600_KEY_ENTER;
                    ms_data.btn = now_value & 0x1F;
                }
                else
                {
                    ms_data.btn = 0x40;
                }
            }
        #else
            ms_data.btn = now_value & 0x1F; //get button value
        #endif


            if (now_value) 
            {
                printf("value:0x%2x, btn:0x%2x \n", now_value, ms_data.btn);
            }

            btn_value = now_value;

            debounce = 0; //clear debounce times
        }
    }

    button_process(ret); //process new button

    return ret;
}

ble_sts_t	app_km_notify_data_to_host  (u16 attHandle, u8 *p, int len)
{
	 u8 txFifoNumber = blc_ll_getTxFifoNumber();
	 if (txFifoNumber >=(ACL_TX_FIFO_NUM - 2))
	 {
	    printf("txFifoNumber >=(ACL_TX_FIFO_NUM - 2)\n");
	 	return BLE_SUCCESS;
	 }	
#if HID_MOUSE_ATT_ENABLE
	if(attHandle==HID_MOUSE_REPORT_INPUT_DP_H)
	{
		if(ccc_flag&MOUSE_REPORT_CCC_EN)
	   {
	        //printf("p[0]=0x%01x, p[1]=0x%01x, p[2]=0x%01x, p[3]=0x%01x, p[4]=0x%01x, p[5]=0x%01x\n",  p[0], p[1], p[2], p[3], p[4], p[5]);
			return blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, attHandle, p, len);		
	   }
	}
#endif
#if HID_KEYBOARD_ATT_ENABLE
	if(attHandle==HID_NORMAL_KB_REPORT_INPUT_DP_H)
	{
		if(ccc_flag&KB_NORMAL_KEY_CCC_EN)
	   {
			return blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, attHandle, p, len);		
	   }
	}
	else if(attHandle==HID_CONSUME_REPORT_INPUT_DP_H)
	{
		if(ccc_flag&KB_CONSUME_KEY_CCC_EN)
	   {
			return blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, attHandle, p, len);		
	   }
	}
#endif
	return BLE_SUCCESS;
}

/**
 * @brief	data notify process
 * @param	none
 * @return	none
 */
void ble_notify_data_proc_aaa()
{
    u8 need_ms_notify = 0;
    mouse_data_t buf;
    u8 txFifoNumber = blc_ll_getTxFifoNumber(); //get TX FIFO (used)

    if ( (my_fifo_get(&fifo_km) != 0) || ms_data.x || ms_data.y || ms_data.wheel || ms_data.wheel_level)
    { //KM_data FIFO have data or sensor have data or wheel have data
        loop_cnt = 0; //clear loop_cnt

        if (my_fifo_get(&fifo_km) != 0) // KM_data FIFO have data
        { 
            u8 *p =  my_fifo_get(&fifo_km);
            buf.btn = p[0];
        }
        else // KM_data FIFO empty
        {
            buf.btn = ms_data.btn;
        }
        buf.x = ms_data.x;
        buf.y = ms_data.y;
        buf.wheel = ms_data.wheel;

		//if(ms_data.wheel_level)
		//	printf("ms_data.wheel_temp = %d\n", ms_data.wheel_level);

		buf.wheel_level  = ms_data.wheel_level;
        need_ms_notify = 1; //set notify flag
    }

#if MOUSE_FACTORY_RESET_ENABLE
	if ( FACTORY_KEY_RELEASE == mouse_factory_reset_flag() )
	{
		buf.btn = 0x00;
		buf.x = 0;
		buf.y = 0;
		buf.wheel = 0;
		need_ms_notify = 1;

		if ( FACTORY_KEY_RELEASE == mouse_factory_reset_flag() )
		{
			mouse_factory_release_times();
			printf("-->RET 22\n");
		}
	}
#endif

    if (active_disconnect_reason) // active_disconnect_reason > 0, means need disconnect with host
    {
        buf.btn = 0;
        buf.x = 0;
        buf.y = 0;
        buf.wheel = 0;
        buf.wheel_level = 0;
        need_ms_notify = 1;
        blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, HID_MOUSE_REPORT_INPUT_DP_H, &buf.btn, MOUSE_DATA_LEN_AAA); //notify empty data
        bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN); //disconnect
        return;
    }

    if (need_ms_notify) // data need notify
    {
        if( ms_data.btn )
        {
            // printf("notify 333, 0x%02x, 0x%02x\n", ms_data.btn, buf.btn);
        }

        u8 status = app_km_notify_data_to_host(HID_MOUSE_REPORT_INPUT_DP_H, &buf.btn, MOUSE_DATA_LEN_AAA);
        if (status == BLE_SUCCESS)
        {
            if (my_fifo_get(&fifo_km) != 0) {
                my_fifo_pop(&fifo_km); //POP FIFO
            }
            ms_data.wheel = 0;
            ms_data.wheel_level = 0;
        }
    #if DEBUG_MODE
        else
        {
            printf("notify_fail_reason:0x%01x.\r\n", status);
        }
    #endif
    }
#if BATT_CHECK_ENABLE
    else if ( bat_flag.data_notify ) // battery need notify
    {
        if( ccc_flag&BATTERY_LEVEL_CCC_EN )
        {
            if ( txFifoNumber < (ACL_TX_FIFO_NUM - 2) ) //FIFO not full
            {
                u8 status = blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, BATT_LEVEL_INPUT_DP_H, my_batVal, 1);
                if (status == BLE_SUCCESS)
                {
                    bat_flag.data_notify = 0;
                }
            }
        }
    }
#endif
}

/**
 * @brief	push empty data
 * @param	none
 * @return	none
 */
u8 clear_press_key_aaa()
{
    u8 buf[sizeof(mouse_data_t)] = {0, 0, 0, 0};
	printf("fifo 222\n");
    my_fifo_push(&fifo_km, buf, sizeof(mouse_data_t));
    return 1;
}


/**
 * @brief	get current sensor dpi_value from flash
 * @param	none
 * @return	none
 */
void get_current_cpi_value_handle(void)
{
	if (fun_mode == RF_1M_BLE_MODE)
	{
		dpi_value = flash_dev_info.ble_dpi[flash_dev_info.mast_id];
	}
	else
	{
		dpi_value = flash_dev_info.d24g_dpi;
	}

	if (dpi_value >= dpi_max_level)
	{
	#if (PROJECT_ID == PID_HM660) || (PROJECT_ID == PID_660) || (PROJECT_ID == PID_BG523)
		dpi_value = 0;
	#elif (PROJECT_ID == PID_CM2057B)
		if (SENSOR_PAW3212 == sensor_type )
		{
			dpi_value = 2;
		}
		else
		{
			dpi_value = 1;
		}
	#elif (PROJECT_ID == PID_DMS03A)
		if ( SENSOR_S201B == sensor_type )
		{
			dpi_value = 0;
		}
		else
		{
			dpi_value = 1;
		}
	#elif (PROJECT_ID == PID_HM668)
		if ( SENSOR_S201B == sensor_type || SENSOR_3065XY == sensor_type )
		{
			dpi_value = 0;
		}
		else
		{
			dpi_value = 1;
		}
	#elif (PROJECT_ID == PID_DMS157)
		dpi_value = 0;
	#elif (PROJECT_ID == PID_104)
		if ( SENSOR_PAW3212 == sensor_type )
		{
			dpi_value = 2;
		}
		else
		{
			dpi_value = 1;
		}
	#else
		dpi_value = 1;
	#endif
	}
}

/**
 * @brief	save current sensor dpi value to flash
 * @param	none
 * @return	none
 */
void set_current_cpi_value_handle(void)
{
#if (PROJECT_ID == PID_MS13) || (PROJECT_ID == PID_TB903)
	flash_dev_info.ble_dpi[0] = 1;
	flash_dev_info.ble_dpi[1] = 1;
	flash_dev_info.d24g_dpi = 1;
#else
	if (fun_mode == RF_1M_BLE_MODE)
	{
		flash_dev_info.ble_dpi[flash_dev_info.mast_id] = dpi_value;
	}
	else
	{
		flash_dev_info.d24g_dpi = dpi_value;
	}
#endif
	save_dev_info_flash();
}


unsigned char mouse_wheel_moved(void)
{
	if ( ms_data.x || ms_data.y || ms_data.btn ) 
		return (1);

	return (0);
}

/**
 * @brief	get data process
 * @param	none
 * @return	none
 */
u8 get_ble_data_report_aaa()
{
#if WHEEL_FUN_ENABLE_AAA
    u32 wheel_prepare_tick = mouse_wheel_prepare_tick(); //record current time
#endif

#if BUTTON_FUN_ENABLE_AAA
     has_new_key_event |= btn_get_value(); //get button value
#endif

#if SENSOR_FUN_ENABLE_AAA

#if (MOUSE_CODE_LOCK_ENABLE)
	if ( (0xAA == flash_dev_info.active_flag) && OPTSensor_motion_report(0) )
#else
    if ( OPTSensor_motion_report(0) ) //get sensor data
#endif
    { //have new data
        has_new_key_event |= SENSOR_DATA_EVENT_AAA; //set event flag
        mouse_xy_multiple(); //modify DPI by soft

        user_cfg.sensor_direct = SENSOR_DIRECTION_OPTION;

        check_sensor_dircet(user_cfg.sensor_direct); //modify data by sensor direct
        adaptive_smoother(); //Smooth sensor data

	#if	BATT_LOWER_LED_INDICATE_60S_ENABLE
        battery_indicate_mouse_moved();
	#endif
    }
    else
    { //have no data
        ms_data.x = 0;
        ms_data.y = 0;
    }
#endif

#if WHEEL_FUN_ENABLE_AAA
    has_new_key_event |= wheel_get_value(wheel_prepare_tick); //get wheel value
#endif

    if (has_new_key_event & NEW_KEY_EVENT_AAA)
    {
        // has_new_report_aaa |= HAS_MOUSE_REPORT;

		if (connect_ok && (ble_status_aaa == T5S_CONNECTED_STATUS_AAA)) //connected
		{
			// printf("fifo 888\n");
			my_fifo_push(&fifo_km, &ms_data.btn, sizeof(mouse_data_t)); //push KM_DATA to KM_FIFO
		}

	#if	BATT_LOWER_LED_INDICATE_60S_ENABLE
		battery_indicate_mouse_moved();
	#endif

    }
    return has_new_key_event;
}



#if USB_MODE_ENABLE

void get_usb_data_report_aaa()
{
	static u32 tick_loop_usb = 0x00;//tick 24g

	 ms_data.wheel=0;//wheel 0

	 if(clock_time_exceed(tick_loop_usb,7000))//7ms
	 {
		 tick_loop_usb=clock_time();//update

		 //get_24g_data_report_aaa();
	#if WHEEL_FUN_ENABLE_AAA //wheel en
		 u32 wheel_prepare_tick;//pre tick
		 wheel_prepare_tick = mouse_wheel_prepare_tick();//prepare wheel tick
	#endif

	#if BUTTON_FUN_ENABLE_AAA //btn en
		 has_new_key_event |= btn_get_value();//get btn action
	#endif

	#if WHEEL_FUN_ENABLE_AAA //wheel en
		 has_new_key_event |= wheel_get_value(wheel_prepare_tick);//wheel action
	#endif

		 device_led_process(); //led display
	 }

	//mouse_task_when_rf();//sensor action
#if SENSOR_FUN_ENABLE_AAA
    if (OPTSensor_motion_report(0))
    { //have sensor data
		if (ms_data.x || ms_data.y)
		{
        	has_new_key_event |= SENSOR_DATA_EVENT_AAA; //set event flag

			user_cfg.sensor_direct = SENSOR_DIRECTION_OPTION;
				
        	mouse_xy_multiple(); //modify DPI by soft
        	
        	check_sensor_dircet(user_cfg.sensor_direct); //modify data by sensor direct

        	/* smoother mouse sensor data */
        	adaptive_smoother(); //Smooth sensor data
		}
    }
    else
    { //have no data
        ms_data.x = 0;
        ms_data.y = 0;
    }
#endif

	if(has_new_key_event)//has new evnt
	{
		has_new_key_event = 0;//0 // 
#if (PROJECT_ID == PID_330) 
	if ( fun_mode == USB_MODE  )
	{
		if ( kb_desktop_flag )
		{
			usb_send_button_flag = 1;
			if (connect_ok && (fun_mode == USB_MODE) )
			{
				push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, kb_buf, sizeof(kb_buf));//push to fifo
			}
		}
		else 
		{
			if (usb_send_button_flag)
			{
				usb_send_button_flag = 0;
				push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, release_buf, sizeof(release_buf));//push to fifo
			}
			else
			{
				push_usb_fifo_aaa(MOUSE_DATA_TYPE, &ms_data.btn, sizeof(mouse_data_t));//push to fifo
			}
		}
	}
#elif (PROJECT_ID == PID_82501) || (PROJECT_ID == PID_82501_OLED) || (PROJECT_ID == PID_82501E_OLED) 
	if ( connect_ok && fun_mode == USB_MODE  )
	{
		if ( 1 == kb_special_value)
		{
			push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, (u8 *)kb_homedesk_buf, sizeof(kb_homedesk_buf));//push to fifo
			printf("-->kb_homedesk_buf\n");
			sv_special_key_relase = 1;
			kb_special_value = 0;
		}

		if ( 2 == kb_special_value )
		{
			push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, (u8 *)kb_screenshot_buf, sizeof(kb_screenshot_buf));//push to fifo
			printf("-->kb_screenshot_buf\n");
			sv_special_key_relase = 1;
			kb_special_value = 0;
		}

		if ( 3 == kb_special_value )
		{
			push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, (u8 *)kb_screenlock_buf, sizeof(kb_screenlock_buf));//push to fifo
			printf("-->kb_screenlock_buf\n");
			sv_special_key_relase = 1;
			kb_special_value = 0;
		}

		if ( 4 == kb_special_value  )
		{
			push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, (u8 *)kb_creatsecktop_buf, sizeof(kb_creatsecktop_buf));//push to fifo
			printf("-->kb_screenlock_buf\n");
			sv_special_key_relase = 1;
			kb_special_value = 0;
		}

		if ( sv_special_key_relase )
		{
			sv_special_key_relase++;
			if ( sv_special_key_relase > 2 )
			{
				printf("-->release_buf\n");
				push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, release_buf, sizeof(release_buf));//push to fifo
			}

			if ( sv_special_key_relase >= 5 )
			{
				sv_special_key_relase = 0;
			}
		}
		else
		{
			push_usb_fifo_aaa(MOUSE_DATA_TYPE, &ms_data.btn, sizeof(mouse_data_t));//push to fifo
		}
	}
#else
	push_usb_fifo_aaa(MOUSE_DATA_TYPE, &ms_data.btn, sizeof(mouse_data_t));//push to fifo
#endif

	}

}

#endif

void id_sleep_enable(int enable)
{
}

/**
 * @brief	set button wake up from suspend
 * @param	enable 0:disable wake up , 1:enable wake up
 * @return	none
 */
void btn_set_wakeup_level_suspend(u8 enable)
{
#if 1 //(0 == DEBUG_MODE)
	if ( 0 == gpio_read(PIN_BTN_LEFT) )
	{
		cpu_set_gpio_wakeup(PIN_BTN_LEFT,  1,  enable);
	}
	else
	{
		cpu_set_gpio_wakeup(PIN_BTN_LEFT,  0,  enable);
	}
#endif

	if ( 0 == gpio_read(PIN_BTN_RIGHT) )
	{
		cpu_set_gpio_wakeup(PIN_BTN_RIGHT,  1,  enable);
	}
	else
	{
		cpu_set_gpio_wakeup(PIN_BTN_RIGHT,  0,  enable);
	}


	#if (0 == DEBUG_MODE)
		if ( 0 == gpio_read(PIN_BTN_MIDDLE) )
		{
			cpu_set_gpio_wakeup(PIN_BTN_MIDDLE,  1,  enable);
		}
		else
		{
			cpu_set_gpio_wakeup(PIN_BTN_MIDDLE,  0,  enable);
		}
	#endif

	#if (ADC_TO_GPIO_MODE_EN)

		if ( 0 == gpio_read(PIN_BTN_MODE) )
		{
			cpu_set_gpio_wakeup(PIN_BTN_MODE,  1,  enable);
		}
		else
		{
			cpu_set_gpio_wakeup(PIN_BTN_MODE,  0,  enable);
		}

	#endif

	#if KEY_MODE_INDEPENDENT_ENABLE

		if ( 0 == gpio_read(PIN_BTN_MODE) )
		{
			cpu_set_gpio_wakeup(PIN_BTN_MODE,  1,  enable);
		}
		else
		{
			cpu_set_gpio_wakeup(PIN_BTN_MODE,  0,  enable);
		}

	#endif

	#if KEY_CPI_INDEPENDENT_ENABLE
		if ( 0 == gpio_read(KEY_DPI_PIN) )
		{
			cpu_set_gpio_wakeup(KEY_DPI_PIN,  1,  enable);
		}
		else
		{
			cpu_set_gpio_wakeup(KEY_DPI_PIN,  0,  enable);
		}
	#endif

	#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_FX282)
		cpu_set_gpio_wakeup(CHARGE_PIN, !gpio_read(CHARGE_PIN), enable);
	#endif

#if KEY_K4_Independence_CTRL_EN
	if ( 0 == gpio_read(KEY_K4_PIN) )
	{
		cpu_set_gpio_wakeup(KEY_K4_PIN,  1,  enable);
	}
	else
	{
		cpu_set_gpio_wakeup(KEY_K4_PIN,  0,  enable);
	}
#endif

#if KEY_K4_Independence_CTRL_EN
	if ( 0 == gpio_read(KEY_K5_PIN) )
	{
		cpu_set_gpio_wakeup(KEY_K5_PIN,  1,  enable);
	}
	else
	{
		cpu_set_gpio_wakeup(KEY_K5_PIN,  0,  enable);
	}
#endif

#if USB_MODE_ENABLE || USB_5V_CHECK_ENABLE

	if ( 0 == gpio_read(USB_IN_CHECK_PIN) )
	{
		cpu_set_gpio_wakeup(USB_IN_CHECK_PIN,  1,  enable);
	}
	else
	{
		cpu_set_gpio_wakeup(USB_IN_CHECK_PIN,  0,  enable);
	}

#endif

#if (LED_COLOR_MODE_KEY_ENABLE)

	if ( 0 == gpio_read(LED_COLOR_KEY_PIN) )
	{
		cpu_set_gpio_wakeup(LED_COLOR_KEY_PIN,  1,  enable);
	}
	else
	{
		cpu_set_gpio_wakeup(LED_COLOR_KEY_PIN,  0,  enable);
	}

#endif

#if CHARGE_ENABLE

	if ( 0 == gpio_read(CHARGE_PIN) )
	{
		cpu_set_gpio_wakeup(CHARGE_PIN,  1,  enable);
	}
	else
	{
		cpu_set_gpio_wakeup(CHARGE_PIN,  0,  enable);
	}

#endif

	id_sleep_enable(enable);
}

/**
 * @brief	get GPIO status
 * @param	pin GPIO need be check
 * @return	GPIO status 0:no status, 1:connect GND, 2:connect VDD
 * @note	GPIO sets pull-down to detect high levels and pull-up to detect low levels
 */
static inline u8 button_get_status(u32 pin)
{
    u8 value = 0; //no button press
    u32 delay_clock;

#if EMI_TEST_FUN_ENABLE_AAA
    emi_cd_push_fifo();
#endif

#if M388_K4_K5_LRKEY_CHANGE_ENABLE
    if ( PIN_BTN_LEFT == pin || PIN_BTN_MIDDLE == pin )
    {
        if ( gpio_read(pin) )
        { //check connect to GND
            value = 2; //connect GND
        }
        else
        { //check connect to VDD
            gpio_write(pin, 1); //init pin high
            gpio_setup_up_down_resistor(pin, PM_PIN_PULLUP_10K); //pullup 1m

            sleep_us(100);

            if ( !gpio_read(pin) )
            {
                value = 1; //connect VDD
            }

            gpio_setup_up_down_resistor(pin, PM_PIN_PULLDOWN_100K); //pull down 100k
            gpio_write(pin, 0);
        }

        return value;
    }
#endif


    if (!gpio_read(pin))
    { //check connect to GND
        value = 1; //connect GND
    }
    else
    { //check connect to VDD
        gpio_setup_up_down_resistor(pin, PM_PIN_PULLDOWN_100K); //pull down 100k
        gpio_write(pin, 0);
	 	delay_clock = clock_time();
		do{
		#if (EMI_TEST_FUN_ENABLE_AAA) //emi fun en
			emi_cd_push_fifo();
		#endif
		}while(!clock_time_exceed(delay_clock, 100));

        if (gpio_read(pin))
        {
            value = 2; //connect VDD
        }

        gpio_write(pin, 1); //init pin high
        gpio_setup_up_down_resistor(pin, PM_PIN_PULLUP_1M); //pullup 1m
    }

#if EMI_TEST_FUN_ENABLE_AAA
	emi_cd_push_fifo();
#endif

    return value;
}

#if (PROJECT_ID == PID_0120)
	unsigned char power_on_key_red(u32 pin)
	{
		return ( button_get_status(pin) );
	}
#endif

_attribute_ram_code_ u16 btn_scan()
{
	u16 now_value = 0;
	u8 button_status = 0;

#if 1 // !DEBUG_MODE
	button_status = button_get_status(PIN_BTN_LEFT); //check pin left
	if (button_status == 1)
	{
	#if M388_K4_K5_LRKEY_CHANGE_ENABLE
		now_value |= KEY_LEFT_VALUE_2;
	#else
		now_value |= KEY_LEFT_VALUE;
	#endif
	}
	else if (button_status == 2) 
	{
	#if M388_K4_K5_LRKEY_CHANGE_ENABLE
		now_value |= KEY_LEFT_VALUE;
	#else
		now_value |= KEY_LEFT_VALUE_2;
	#endif
	}
#endif


	button_status = button_get_status(PIN_BTN_RIGHT); //check pin right
	if (button_status == 1)
	{
		now_value |= KEY_RIGHT_VALUE;
	}
	else if (button_status == 2)
	{
		now_value |= KEY_RIGHT_VALUE_2;
	}
	else
	{
	#if (PROJECT_ID == PID_G929) || (PROJECT_ID == PID_BG523)
		gc_dpi_func2_flag = 1;
	#endif
	}

	button_status = button_get_status(PIN_BTN_MIDDLE); //check pin middle
	if (button_status == 1)
	{
	#if M388_K4_K5_LRKEY_CHANGE_ENABLE
		now_value |= KEY_MIDDLE_VALUE_2;
	#else
		now_value |= KEY_MIDDLE_VALUE;
	#endif
	}
	else if (button_status == 2) 
	{
	#if M388_K4_K5_LRKEY_CHANGE_ENABLE
		now_value |= KEY_MIDDLE_VALUE;
	#else
		now_value |= KEY_MIDDLE_VALUE_2;
	#endif
	}


#if KEY_ADC_MODE_MULTI_FUNC_ENABLE
	if ( adc_sample_voltale() < KEY_ADC_VOLTAGE_VALUE)
	{
		 now_value |= KEY_BTN_MODE;
	}
#endif

#if (PROJECT_ID == PID_4028)
	if ( 0 == gpio_read(PIN_BTN_MODE) )
	{
	#if M4028_BLE_G24_MODE_ENABLE
		now_value |= KEY_BTN_MODE;
	#else
		now_value |= KEY_BTN_CPI;
	#endif
	}
#elif (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_M45) || (PROJECT_ID == PID_104)  || KEY_PAIR_USED_POWERUP_ENABLE
	if ( 0 == gc_hm668_mode_key_used )
	{
		if ( 0 == gpio_read(PIN_BTN_MODE) )
		{
			now_value |= KEY_BTN_MODE;
		}
	}
	else
	{
		if ( gpio_read(PIN_BTN_MODE) )
		{
			if ( ++gc_hm668_mode_key_used >= 10 )
			{
				gc_hm668_mode_key_used = 0;
			}
		}
		else
		{
			gc_hm668_mode_key_used = 1;
		}
	}
#else
	#if (KEY_MODE_INDEPENDENT_ENABLE)
		if( 0 == gpio_read(PIN_BTN_MODE) )
		{
			now_value |= KEY_BTN_MODE;
		}
	#endif
#endif

#if (ADC_TO_GPIO_MODE_EN)
	#if KEY_PAIR_USED_POWERUP_ENABLE
		// do nothing
	#else
		if( 0 == gpio_read(PIN_BTN_MODE) )
		{
			now_value |= KEY_BTN_MODE;
		}
	#endif
#endif

#if KEY_MODE_COMBINE_ENABLE
	button_status = button_get_status(PIN_BTN_MODE);//read middle btn
    if (button_status == 1)
    {
        now_value |= KEY_BTN_MODE;
    }
	else if (button_status == 2)
	{
	#if KEY_FEATURE_BOSSKEY_ENABLE
		now_value |= KEY_FEATURE_BOSSKEY_VALUE;
	#endif
	}
#endif

#if (KEY_CPI_INDEPENDENT_ENABLE)
	if ( 0 == gpio_read(KEY_DPI_PIN) )
	{
		now_value |= KEY_BTN_CPI;
	}
#endif

#if KEY_K4_Independence_CTRL_EN
	if ( 0 == gpio_read(KEY_K4_PIN) )
	{
		now_value |= KEY_BTN_K4;
	}
#endif

#if KEY_K5_Independence_CTRL_EN
	if ( 0 == gpio_read(KEY_K5_PIN) )
	{
		now_value |= KEY_BTN_K5;
	}
#endif

#if LED_COLOR_MODE_KEY_ENABLE
	if ( 0 == gpio_read(LED_COLOR_KEY_PIN) )
	{
		now_value |= KEY_COLOR_VALUE;
	}
#endif

#if	KEY_FEATURE_BOSSKEY_ENABLE
	#if (PROJECT_ID == PID_DMS157)
		if ( 0 == gpio_read(KEY_FEATURE_BOSSKEY_PIN) )
		{
			now_value |= KEY_FEATURE_BOSSKEY_VALUE;
		}
	#endif
#endif

#if (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
	if ( adc_sample_voltale_high() >= KEY_ADC_VOLTAGE_VALUE )
	{
		now_value |= KEY_BTN_MODE;
	}
#endif

	return  now_value;
}

/**
 * @brief	set wheel wake up from suspend
 * @param	enable 0:disable wake up , 1:enable wake up
 * @return	none
 */
void wheel_set_wakeup_level_suspend(u8 enable)
{
    if (gpio_read(PIN_WHEEL_1))
    {
        cpu_set_gpio_wakeup(PIN_WHEEL_1, 0, enable); //low wakeup suspend
    }
    else
    {
        cpu_set_gpio_wakeup(PIN_WHEEL_1, 1, enable);
    }

    if (gpio_read(PIN_WHEEL_2))
    {
        cpu_set_gpio_wakeup(PIN_WHEEL_2, 0, enable); //low wakeup suspend
    }
    else
    {
        cpu_set_gpio_wakeup(PIN_WHEEL_2, 1, enable);
    }
}

static void wheel_init_hw()
{
    write_reg8(0xd2, WHEEL_ADDRES_D2); // different gpio different value
    write_reg8(0xd3, WHEEL_ADDRES_D3);

	rc_32k_cal();
    BM_SET(reg_clk_en0, FLD_CLK0_QDEC_EN);

    write_reg8(0xd7, 0x01);  //must
    write_reg8(0xd1, 0x01);  //filter   00-07 01 is best
}

u32 mouse_wheel_prepare_tick(void)
{
    write_reg8(0xd8, 0x01);
    return clock_time();
}

/**
 * @brief	get wheel encoder result
 * @param	wheel_prepare_tick: start time
 * @return	encoder result
 */
s8 mouse_wheel_process(u32 wheel_prepare_tick)
{
    s8 ret = 0;
	
    while(read_reg8(0xd8) & 0x01)
    { //encoder working
        if (clock_time_exceed(wheel_prepare_tick, 260))	////4 cylce is enough: 4*1/32k = 1/8 ms
        { //over time
            write_reg8(0xd6, 0x01); //reset d6[0]
            write_reg8(0xd6, 0x00);
            break;
        }
    }

#if 1	//(WHEEL_TWO_STEP_PROC)
    _attribute_data_retention_user static signed char accumulate_wheel_cnt;
    _attribute_data_retention_user static signed char wheel_cnt;
	
    wheel_cnt = read_reg8(0xd0);
    wheel_cnt += accumulate_wheel_cnt;

    if (wheel_cnt & 1) //
    {
        accumulate_wheel_cnt = wheel_cnt > 0 ? 1 : -1;
    }
    else  //
    {
        accumulate_wheel_cnt = 0;
    }
	
    ret = (wheel_cnt / 2);
    if (wheel_cnt || accumulate_wheel_cnt || ret)
    {
    	// printf("-->%d, %d, %d \n", wheel_cnt, accumulate_wheel_cnt, ret);
    }
#else
    ret = read_reg8(0xd0);
#endif

    return ret;
}

/**
 * @brief	get wheel value
 * @param	wheel_prepare_tick: start time
 * @return	wheel value
 */
u8 wheel_get_value(u32 wheel_prepare_tick)
{
    u8 ret = 0;
    s8 wheel_now = 0;

    wheel_now = mouse_wheel_process(wheel_prepare_tick);

    if (user_cfg.wheel_direct != U8_MAX)
    { //modify wheel value by wheel_direct
        wheel_now = -wheel_now;
    }

    ms_data.wheel = wheel_now;

    if (wheel_now != 0)
    {
        ret = WHEEL_DATA_EVENT_AAA; //set event flag
        printf("wheel=%1x\n", wheel_now);
    }

    return ret;
}


static void sensor_postion_init(void)
{
	OPTSensor_Init(1); //initial optical sensor

	dpi_max_level = 3;

#if (PROJECT_ID == PID_MS13)
	if ( sensor_type == SENSOR_SG8670 )
	{
		dpi_max_level = 4;
	}
	else
	{
		dpi_max_level = 3;
	}
#endif

#if (PROJECT_ID == PID_8693) || (PROJECT_ID == PID_G929)
	if ( SENSOR_S201B == sensor_type || SENSOR_PAW3212 == sensor_type )
	{
		dpi_max_level = 4;
	}
	else
	{
		dpi_max_level = 3;
	}
#endif

#if (PROJECT_ID == PID_DMS06)
	if ( SENSOR_S201B == sensor_type || SENSOR_SG8925 == sensor_type || SENSOR_3065XY == sensor_type)
	{
		dpi_max_level = 4;
	}
	else
	{
		dpi_max_level = 3;
	}
#endif

#if (PROJECT_ID == PID_HM660)
	if ( SENSOR_SG8925 == sensor_type )
	{
		dpi_max_level = 4;
	}
	else
	{
		dpi_max_level = 3;
	}
#endif

#if (PROJECT_ID == PID_HYC351)
	if ( SENSOR_3065XY == sensor_type )
	{
		dpi_max_level = 4;
	}
	else
	{
		dpi_max_level = 3;
	}
#endif

#if DPI_S210B_3065XY_DEFAULT_ENABLE
	if ( SENSOR_3065XY == sensor_type || SENSOR_S201B == sensor_type )
	{
		dpi_max_level = 4;
	}
#endif

#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_DMS06)
	if (SENSOR_S101UL == sensor_type)
	{
		dpi_max_level = 5;
	}
#endif

#if (PROJECT_ID == PID_DMS157)
	if (SENSOR_S101UL == sensor_type)
	{
		dpi_max_level = 4;
	}
#endif

#if (PROJECT_ID == PID_CM2057B)
	if (SENSOR_PAW3212 == sensor_type )
	{
		dpi_max_level = 6;
	}
#endif

#if (PROJECT_ID == PID_BG523)
	if (SENSOR_SG8670 == sensor_type )
	{
		dpi_max_level = 4;
	}
#endif

#if (PROJECT_ID == PID_M388)
	#if BLuetoothMouse1_DPI_ENABLE
		if ( SENSOR_SG8670 == sensor_type || SENSOR_PAW3212 == sensor_type )
		{
			dpi_max_level = 5;
		}
	#else
		if (SENSOR_SG8670 == sensor_type)
		{
			dpi_max_level = 4;
		}
		else if (SENSOR_PAW3212 == sensor_type)
		{
			dpi_max_level = 5;
		}
	#endif
#endif

#if (PROJECT_ID == PID_MS2360)
	if (SENSOR_PAW3212 == sensor_type || SENSOR_PAW3220DB_TJDL == sensor_type )
	{
		dpi_max_level = 4;
	}
#endif

#if (PROJECT_ID == PID_TB903)
	if ( SENSOR_S205 == sensor_type || SENSOR_SG8670 == sensor_type  )
	{
		dpi_max_level = 6;
	}
#endif

#if (PROJECT_ID == PID_4027)
	if ( SENSOR_SG8670 == sensor_type )
	{
		dpi_max_level = 5;
	}
#endif

#if (PROJECT_ID == PID_M0018)
	if ( SENSOR_KA8G2 == sensor_type )
	{
		dpi_max_level = 3;
	}
#endif

#if (PROJECT_ID == PID_2187)
	if ( SENSOR_KA8G2 == sensor_type || SENSOR_SG8670 == sensor_type )
	{
		dpi_max_level = 5;
	}
#endif

#if (PROJECT_ID == PID_E368_CHARGE)
	if (SENSOR_PAW3212 == sensor_type )
	{
		dpi_max_level = 4;
	}
#endif

#if (PROJECT_ID == PID_0120)
	if ( SENSOR_PAW3212 == sensor_type )
	{
		dpi_max_level = 5;
	}
#endif

#if (PROJECT_ID == PID_104)
	if ( SENSOR_PAW3212 == sensor_type )
	{
		dpi_max_level = 4;
	}
#endif

#if (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
	if ( SENSOR_3065XY == sensor_type )
	{
		dpi_max_level = 3;
	}
#endif

#if (PROJECT_ID == PID_Q15)
	if ( SENSOR_3311 == sensor_type )
	{
		dpi_max_level = 6;
	}
#endif

#if DPI_SAVE_FLASH
	get_current_cpi_value_handle(); //get current sensor dpi_value from flash
#endif

    sensor_dpi_set(dpi_value); //set dpi_value to sensor

#if SHOW_MAST_REAL_MAC_DEBUG
    output_dev_info.sensor_type = sensor_type;
    output_dev_info.sensor_pd1 = product_id1;
    output_dev_info.sensor_pd2 = product_id2;
    output_dev_info.sensor_pd3 = product_id3;
#endif

#if TEST_MCU_CURRENT_DEBUG
    OPTSensor_Shutdown();
#endif

}

void protect_led_ram_on(void)
{
#if (PROJECT_ID == PID_601)
	#if LED_G24_ENABLE
		gpio_output_config_status(PIN_24G_LED, LED_OFF);
	#endif

	#if LED_BT1_ENABLE
		gpio_output_config_status(PIN_BLE1_LED, LED_OFF);
	#endif

	#if LED_BT2_ENABLE
		gpio_output_config_status(PIN_BLE2_LED, LED_OFF);
	#endif
#endif
}

/**
 * @brief	initial hardware
 * @param	none
 * @return	none
 */
void hw_init()
{
#if USB_SWITCH_ENABLE
	gpio_input_config_status(USB_SWITCH_PIN, PM_PIN_UP_DOWN_FLOAT );
#endif

#if (PROJECT_ID == PID_104)
	gpio_input_config_status(GPIO_PC0, PM_PIN_PULLUP_1M );
#endif

#if KEY_FEATURE_BOSSKEY_ENABLE
	#if (PROJECT_ID == PID_DMS157)
		gpio_input_config_status(KEY_FEATURE_BOSSKEY_PIN, PM_PIN_PULLUP_1M );
	#endif
#endif

#if (PROJECT_ID == PID_DMS157) ||  (PROJECT_ID == PID_RM12)
	gpio_input_config_status(PIN_BTN_LEFT, PM_PIN_PULLUP_1M );
	gpio_input_config_status(PIN_BTN_RIGHT, PM_PIN_PULLUP_1M );
	gpio_input_config_status(PIN_BTN_MIDDLE, PM_PIN_PULLUP_1M );
#endif

#if (PROJECT_ID == PID_DMS157)
	gpio_input_config_status(PIN_WHEEL_1,  PM_PIN_PULLUP_10K);
	gpio_input_config_status(PIN_WHEEL_2,  PM_PIN_PULLUP_10K);
#endif

#if (PROJECT_ID == PID_M388)
	#if M388_K4_K5_LRKEY_CHANGE_ENABLE
	 	 gpio_input_config_status(PIN_BTN_MIDDLE, PM_PIN_PULLDOWN_100K );
	 	 gpio_input_config_status(PIN_BTN_LEFT,   PM_PIN_PULLDOWN_100K );
	#endif
#endif

#if (PROJECT_ID == PID_HM668)
	gpio_input_config_status(PIN_SIF_SDA,  PM_PIN_PULLUP_10K);
	gpio_output_config_status(PIN_SIF_SCL, 1);
#endif


#if LED_CODE_SW_CHECK_ENABLE
	gpio_input_config_status(LED_CODE_SW_CHECK_PIN,  PM_PIN_UP_DOWN_FLOAT);
#endif

#if LED_CODE_POWER_ENABLE
	gpio_output_config_status(LED_CODE_POWER_PIN,  1);
#endif

#if HM668_TEST_ENABLE
	gpio_input_config_status(GPIO_PC7,  PM_PIN_PULLUP_10K);
#endif


#if (PROJECT_ID == PID_0120)
	power_on_key_red( PIN_BTN_MIDDLE );
	power_on_key_red( PIN_BTN_RIGHT );
#endif

#if MODULE_OLED_ENABLE
	OLED_Init();
#endif

#if (PROJECT_ID == PID_XT27)
	gpio_output_config_status(LED_RGB_NULL_PIN, LED_RGB_DPI_OFF);
#endif

#if CHARGE_ENABLE
	gpio_input_config_status(CHARGE_PIN, PM_PIN_PULLUP_10K);
#endif

#if LED_COLOR_LIGHTS_ENABLE
	gpio_output_config_status(PIN_COLOR_LOGO_LED, LED_COLOR_LOGO_OFF);
#endif

#if USB_MODE_ENABLE || USB_5V_CHECK_ENABLE || USB_5V_ENABLE
	gpio_input_config_status(USB_IN_CHECK_PIN, PM_PIN_UP_DOWN_FLOAT);
#endif

#if KEY_K4_Independence_CTRL_EN
	gpio_input_config_status(KEY_K4_PIN, PM_PIN_PULLUP_10K);
#endif

#if KEY_K5_Independence_CTRL_EN
	gpio_input_config_status(KEY_K5_PIN, PM_PIN_PULLUP_10K);
#endif

#if (MODULE_DIGITAL_TUBE_ENABLE)
	digital_tube_init();
#endif

#if	USB_5V_CHARGE_LED_ENABLE
	gpio_output_config_status(USB_5V_CHARGE_LED_PIN, LED_OFF);
#endif

#if ADC_TO_GPIO_MODE_EN
	gpio_input_config_status(PIN_BTN_MODE, PM_PIN_PULLUP_10K);
#endif

#if LED_MODE_MIX_CTRL_ENABLE
	#if LED_G24_ENABLE
		gpio_higsesistance_config(PIN_24G_LED);
	#endif

	#if LED_BT1_ENABLE
		gpio_higsesistance_config(PIN_BLE1_LED);
	#endif

	#if LED_BT2_ENABLE
		gpio_higsesistance_config(PIN_BLE2_LED);
	#endif

	#if LED_BAT_VOLTAGE_ENABLE || LED_DPI_INDICATE_ENABLE
		#if (PROJECT_ID == PID_BM2060)
			//do nothing
		#else
			gpio_higsesistance_config(PIN_BAT_DPI_LED);
		#endif
	#endif
#else
	#if LED_G24_ENABLE
		#if (PROJECT_ID == PID_NR300) || (PROJECT_ID == PID_535) || (PROJECT_ID == PID_S600)
			gpio_output_config_status(PIN_24G_LED, LED_ON);
		#elif (PROJECT_ID == PID_XT27) || (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
			// do nothing
		#else
			gpio_output_config_status(PIN_24G_LED, LED_OFF);
		#endif
	#endif

	#if LED_BT1_ENABLE
		#if (PROJECT_ID == PID_NR300) || (PROJECT_ID == PID_535) || (PROJECT_ID == PID_S600)
			gpio_output_config_status(PIN_BLE1_LED, LED_ON);
		#elif (PROJECT_ID == PID_XT27) || (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
			// do nothing
		#else
			gpio_output_config_status(PIN_BLE1_LED, LED_OFF);
		#endif
	#endif

	#if LED_BT2_ENABLE
		#if (PROJECT_ID == PID_NR300) || (PROJECT_ID == PID_535) || (PROJECT_ID == PID_S600)
			gpio_output_config_status(PIN_BLE2_LED, LED_ON);
		#else
			gpio_output_config_status(PIN_BLE2_LED, LED_OFF);
		#endif
	#endif

	#if LED_BAT_VOLTAGE_ENABLE || LED_DPI_INDICATE_ENABLE
		#if (PROJECT_ID == PID_NR300)
			gpio_output_config_status(PIN_BAT_DPI_LED, LED_ON);
		#elif (PROJECT_ID == PID_0120) || (PROJECT_ID == PID_DMS157) || (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
			// do nothing
		#else
			gpio_output_config_status(PIN_BAT_DPI_LED, LED_OFF);
		#endif
	#endif
#endif


#if	KEY_DPI_IN_ENABLE
	gpio_input_config_status(KEY_DPI_PIN, PM_PIN_PULLUP_1M);
#endif

#if	KEY_K4_IN_ENABLE
	gpio_input_config_status(KEY_K4_PIN, PM_PIN_PULLUP_1M);
#endif

#if	KEY_K5_IN_ENABLE
	gpio_input_config_status(KEY_K5_PIN, PM_PIN_PULLUP_1M);
#endif


#if (KEY_MODE_INDEPENDENT_ENABLE) || KEY_MODE_COMBINE_ENABLE
	gpio_input_config_status(PIN_BTN_MODE, PM_PIN_PULLUP_1M);
#endif

#if (KEY_CPI_INDEPENDENT_ENABLE)
	gpio_input_config_status(KEY_DPI_PIN, PM_PIN_PULLUP_1M);
#endif


#if LED_COLOR_MODE_KEY_ENABLE
	gpio_input_config_status(LED_COLOR_KEY_PIN, PM_PIN_PULLUP_1M);
#endif


#if LED_RGB_DPI_ACTION_ENABLE
	gpio_output_config_status(LED_RGB_DPI_PIN_R, LED_RGB_DPI_OFF);
	gpio_output_config_status(LED_RGB_DPI_PIN_G, LED_RGB_DPI_OFF);
	gpio_output_config_status(LED_RGB_DPI_PIN_B, LED_RGB_DPI_OFF);
#endif

#if LED_RGB_BAT_DPI_ENABLE
	gpio_output_config_status(LED_RGB_DPI_BAT_PIN_R, LED_RGB_DPI_BAT_OFF);
	gpio_output_config_status(LED_RGB_DPI_BAT_PIN_B, LED_RGB_DPI_BAT_OFF);
#endif

#if LED_LOGO_ACTION_ENABLE
	gpio_output_config_status(PIN_LOGO_LED, LED_LOGO_OFF);
#endif

#if (LED_BATT_CAP_LEVEL_ENABLE)
	batt_charge_init();
#endif


#if (WHEEL_LEFT_RIGHT_ENABLE)
	gpio_input_config_status(VOICE_A_PIN, PM_PIN_UP_DOWN_FLOAT);
	gpio_input_config_status(VOICE_B_PIN, PM_PIN_UP_DOWN_FLOAT);

	gpio_set_interrupt(VOICE_A_PIN, POL_FALLING);
	gpio_set_interrupt(VOICE_B_PIN, POL_FALLING);
#endif

#if WHEEL_FUN_ENABLE_AAA
    wheel_init_hw(); //initial wheel
#endif

#if BLT_APP_LED_ENABLE
    led_hw_init(); //initial led
#endif

#if (LED_GRB_NUM16_ENABLE)
	led_rgb_num16_init();
#endif

#if (LED_GRB_NUM05_ENABLE)
	led_rgb_num05_init();
#endif

#if (LED_GRB_LOGO_ENABLE)
	rgb_led_logo_init();
#endif

	sensor_postion_init();

#if LED_CODE_ENABLE || LED_CODE_PWIR_DRIVE_ENABLE
	#if HM668_TEST_ENABLE
		// do thing
	#elif (PROJECT_ID == PID_M45) || (PROJECT_ID == PID_104)
		// do thing
	#else
	 	 gpio_output_config_status(LED_CODE_PIN, LED_OFF);
	#endif
#endif

#if (PROJECT_ID == PID_0120)
	rgb_code_dpi_set( RGB_CODE_STATUS_DPI_START,  RGB_CODE_ACT_SET );
#endif

#if (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_M45) || (PROJECT_ID == PID_S600)
	#if LED_CODE_PWIR_DRIVE_ENABLE
		// do nothing
	#else
		rgb3810_light_on_init();
	#endif
#endif

#if LED_CODE_PWIR_DRIVE_ENABLE
	pwm_set_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);

	gpio_set_func(WS2812_PWM0_PIN, PWM0);
	pwm_set_mode(PWM0_ID, PWM_IR_DMA_FIFO_MODE);

	pwm_set_cycle_and_duty(PWM0_ID, WS2812_BIT_MAX_TICK,    WS2812_T0H_TICK);
	pwm_set_pwm0_shadow_cycle_and_duty(WS2812_BIT_MAX_TICK, WS2812_T1H_TICK);
#endif

#if (PROJECT_ID == PID_DMS157)
	 rgb_led_pwm_init();
#endif

#if (MODULE_MCU_EXTERNAL_ENABLE)
	gpio_output_config(EXTER_MCU_IO_PIN);
	gpio_write(EXTER_MCU_IO_PIN, 0);
#endif
}

/**
 * @brief	modify DPI by soft
 * @param	none
 * @return	none
 */
void mouse_xy_multiple()
{
#if (PROJECT_ID == PID_BG523)
    s32 x = 0, y = 0;

    if ( 2 == dpi_value )
    {
        x = ms_data.x;
        y = ms_data.y;

        x = (x << 2) / 3;
        y = (y << 2) / 3;

        ms_data.x = x;
        ms_data.y = y;
    }
    else if ( 3 == dpi_value )
    {
        x = ms_data.x;
        y = ms_data.y;
		
        x = (x * 5) / 3;
        y = (y * 5) / 3;

        ms_data.x = x;
        ms_data.y = y;
    }
#endif

#if (PROJECT_ID == PID_M388)

    s32 x = 0, y = 0;
#if BLuetoothMouse1_DPI_ENABLE
    if ( sensor_type == SENSOR_PAW3212 || sensor_type == SENSOR_SG8670 )
    {
		if ( 3 == dpi_value )
		{
			x = ms_data.x;
			y = ms_data.y;

			x = (x * 4) / 3;
			y = (y * 4) / 3;

			ms_data.x = x;
			ms_data.y = y;
		}
		else if ( 4 == dpi_value )
		{
			x = ms_data.x;
			y = ms_data.y;

			x = (x * 5) / 3;
			y = (y * 5) / 3;

			ms_data.x = x;
			ms_data.y = y;
		}
    }
#else
    if (sensor_type == SENSOR_PAW3212)
    {
    	if ( 3 == dpi_value )
    	{
			x = ms_data.x;
			y = ms_data.y;

			x = (x * 4) / 3;
			y = (y * 4) / 3;

			ms_data.x = x;
			ms_data.y = y;
    	}
    	else if ( 4 == dpi_value )
    	{
			x = ms_data.x;
			y = ms_data.y;

			x = x * 2;
			y = y * 2;

			ms_data.x = x;
			ms_data.y = y;
    	}
    }
    else if (sensor_type == SENSOR_SG8670)
    {
	   if ( 2 == dpi_value )
	   {
			x = ms_data.x;
			y = ms_data.y;

			x = (x << 2) / 3;
			y = (y << 2) / 3;

			ms_data.x = x;
			ms_data.y = y;
		}
		else if ( 3 == dpi_value )
		{
			x = ms_data.x;
			y = ms_data.y;

			x = (x * 5) / 3;
			y = (y * 5) / 3;

			ms_data.x = x;
			ms_data.y = y;
		}
    }
#endif

#endif

#if (PROJECT_ID == PID_MS13)
    s32 x = 0, y = 0;

    if ( 3 == dpi_value )
    {
        x = ms_data.x;
        y = ms_data.y;

        x = (x << 2) / 3;
        y = (y << 2) / 3;

        ms_data.x = x;
        ms_data.y = y;
    }
#endif

#if (PROJECT_ID == PID_8693) || (PROJECT_ID == PID_G929)
    s32 x = 0, y = 0;
    if ( sensor_type == SENSOR_PAW3212 )
    {
		if ( 3 == dpi_value )
		{
			x = ms_data.x;
			y = ms_data.y;

			x = (x * 5) / 3;
			y = (y * 5) / 3;

			ms_data.x = x;
			ms_data.y = y;
		}
    }
#endif

#if DPI_3065XY_FOUR_LEVEL_ENABLE
	if (  SENSOR_3065XY == sensor_type && 2 == dpi_value && 1 == dpi_3605_limit_level_flag )
	{
		s32 x = 0, y = 0;

		x = ms_data.x;
		y = ms_data.y;

		x = (x * 3) / 2;
		y = (y * 3) / 2;

		ms_data.x = x;
		ms_data.y = y;
	}
#endif

#if (PROJECT_ID == PID_CM2057B)
    if ( sensor_type == SENSOR_PAW3212 )
    {
    	s32 x1 = 0, y1 = 0;
		if ( 4 == dpi_value )
		{
			x1 = ms_data.x;
			y1 = ms_data.y;

			x1 = (x1 << 2) / 3;
			y1 = (y1 << 2) / 3;

			ms_data.x = x1;
			ms_data.y = y1;
		}
		else if (5 == dpi_value)
		{
			x1 = ms_data.x;
			y1 = ms_data.y;

			x1 = x1 << 1;
			y1 = y1 << 1;

			ms_data.x = x1;
			ms_data.y = y1;
		}
    }
#endif

#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_DMS06)
	s32 x1 = 0, y1 = 0;
    if ( sensor_type == SENSOR_S101UL )
    {
		if ( 4 == dpi_value )
		{
			x1 = ms_data.x;
			y1 = ms_data.y;

			x1 = (x1 * 48 ) / 37;
			y1 = (y1 * 48 ) / 37;

			ms_data.x = x1;
			ms_data.y = y1;
		}
    }
    else if (sensor_type == SENSOR_S201B)
    {
		x1 = ms_data.x;
		y1 = ms_data.y;

		if ( 0 == dpi_value )
		{
			x1 = (x1 * 60 ) / 49;
			y1 = (y1 * 60 ) / 49;
		}
		else if ( 1 == dpi_value )
		{
			x1 = (x1 * 16 ) / 13;
			y1 = (y1 * 16 ) / 13;
		}
		else if ( 2 == dpi_value )
		{
			x1 = (x1 * 48 ) / 39;
			y1 = (y1 * 48 ) / 39;
		}
		else if ( 3 == dpi_value )
		{
			x1 = (x1 * 16 ) / 13;
			y1 = (y1 * 16 ) / 13;
		}

		ms_data.x = x1;
		ms_data.y = y1;
    }
#endif

#if (PROJECT_ID == PID_MS2360)
    if ( sensor_type == SENSOR_PAW3212 )
    {
    	s32 x1 = 0, y1 = 0;
		if ( 3 == dpi_value )
		{
			x1 = ms_data.x;
			y1 = ms_data.y;

			x1 = (x1 << 2) / 3;
			y1 = (y1 << 2) / 3;

			ms_data.x = x1;
			ms_data.y = y1;
		}
    }
#endif

#if (PROJECT_ID == PID_4027)
    if ( sensor_type == SENSOR_SG8670 )
    {
    	s32 x1 = 0, y1 = 0;

		if ( 4 == dpi_value )
		{
			x1 = ms_data.x;
			y1 = ms_data.y;

			x1 = (x1 * 5) / 3;
			y1 = (y1 * 5) / 3;

			ms_data.x = x1;
			ms_data.y = y1;
		}
		else if ( 5 == dpi_value )
		{
			x1 = ms_data.x;
			y1 = ms_data.y;

			x1 = (x1 * 10) / 3;
			y1 = (y1 * 10) / 3;

			ms_data.x = x1;
			ms_data.y = y1;
		}
    }
#endif

#if (PROJECT_ID == PID_2187)
    if ( sensor_type == SENSOR_SG8670 || sensor_type == SENSOR_KA8G2 )
    {
    	s32 x1 = 0, y1 = 0;

		if ( 4 == dpi_value )
		{
			x1 = ms_data.x;
			y1 = ms_data.y;

			x1 = (x1 * 7) / 4;
			y1 = (y1 * 7) / 4;

			ms_data.x = x1;
			ms_data.y = y1;
		}
    }
#endif


#if (PROJECT_ID == PID_E368_CHARGE)
    if ( sensor_type == SENSOR_PAW3212 )
    {
    	s32 x1 = 0, y1 = 0;

		if ( 3 == dpi_value )
		{
			x1 = ms_data.x;
			y1 = ms_data.y;

			x1 = (x1 * 4) / 3;
			y1 = (y1 * 4) / 3;

			ms_data.x = x1;
			ms_data.y = y1;
		}
    }
#endif

#if (PROJECT_ID == PID_0120)
    if ( sensor_type == SENSOR_PAW3212 )
    {
    	s32 x1 = 0, y1 = 0;

		if ( 4 == dpi_value )
		{
			x1 = ms_data.x;
			y1 = ms_data.y;

			x1 = (x1 * 7) / 4;
			y1 = (y1 * 7) / 4;

			ms_data.x = x1;
			ms_data.y = y1;
		}
    }
#endif

#if (PROJECT_ID == PID_TB903)
    if ( sensor_type == SENSOR_SG8670 )
    {
    	s32 x1 = 0, y1 = 0;

		if ( 3 == dpi_value || 4 == dpi_value )
		{
			x1 = ms_data.x;
			y1 = ms_data.y;

			x1 = ( x1 << 1 );
			y1 = ( y1 << 1 );

			ms_data.x = x1;
			ms_data.y = y1;
		}
		else if ( 5 == dpi_value )
		{
			x1 = ms_data.x;
			y1 = ms_data.y;

			x1 = ( x1 << 3 ) / 3;
			y1 = ( y1 << 3 ) / 3;

			ms_data.x = x1;
			ms_data.y = y1;
		}
    }
#endif

#if (PROJECT_ID == PID_RM12)
    if ( sensor_type == SENSOR_S201B )
    {
#if RM12_S2012_DPI_2_ENABLE
    	s32 x1 = 0, y1 = 0;

		if ( 0 == dpi_value )
		{
			x1 = ms_data.x;
			y1 = ms_data.y;

			x1 = ( x1 * 6 ) / 5;
			y1 = ( y1 * 6 ) / 5;

			ms_data.x = x1;
			ms_data.y = y1;
		}
		else if ( 1 == dpi_value )
		{
			x1 = ms_data.x;
			y1 = ms_data.y;

			x1 = ( x1 * 8 ) / 7;
			y1 = ( y1 * 8 ) / 7;

			ms_data.x = x1;
			ms_data.y = y1;
		}
		else if ( 2 == dpi_value )
		{
			x1 = ms_data.x;
			y1 = ms_data.y;

			x1 = ( x1 * 6 ) / 5;
			y1 = ( y1 * 6 ) / 5;

			ms_data.x = x1;
			ms_data.y = y1;
		}
		else if ( 3 == dpi_value )
		{
			x1 = ms_data.x;
			y1 = ms_data.y;

			x1 = ( x1 * 8 ) / 7;
			y1 = ( y1 * 8 ) / 7;

			ms_data.x = x1;
			ms_data.y = y1;
		}
#endif
    }
#endif

}

/**
 * @brief	2.4G main loop task
 * @param	none
 * @return	none
 */
void mouse_task_when_rf()
{
#if SENSOR_FUN_ENABLE_AAA

#if (MOUSE_CODE_LOCK_ENABLE)
	if ( (0xAA == flash_dev_info.active_flag) && OPTSensor_motion_report(0) )
#else
    if ( OPTSensor_motion_report(0) )
#endif
    { //have sensor data
		if (ms_data.x || ms_data.y)
		{
        	has_new_key_event |= SENSOR_DATA_EVENT_AAA; //set event flag
        	mouse_xy_multiple(); //modify DPI by soft

		#if G24_DELAY_ENTER_SLEEP_ENABLE
        	 mouse_moved_time_clean();
		#endif

            user_cfg.sensor_direct = SENSOR_DIRECTION_OPTION;

            check_sensor_dircet(user_cfg.sensor_direct); //modify data by sensor direct
		}
    }
    else
    { //have no data
        ms_data.x = 0;
        ms_data.y = 0;
    }
#endif

#if (MOUSE_CODE_LOCK_ENABLE)
    if ( 0xAA != flash_dev_info.active_flag )
    {
        ms_data.x = 0;
        ms_data.y = 0;
    }
#endif

#if 0//TEST_DRAW_A_SQUARE
    has_new_key_event |= Draw_a_square_test(); //
#endif
}


/**
 * @brief	clear_pair_flag
 * @param	none
 * @return	none
 */
void clear_pair_flag()
{
    pair_flag = 0;
    analog_write(USED_DEEP_ANA_REG1, ana_reg1_aaa & CLEAR_PAIR_ANA_FLAG);
}

/**
 * @brief	set_pair_flag
 * @param	none
 * @return	none
 */
void set_pair_flag()
{
    pair_flag = 1;
    analog_write(USED_DEEP_ANA_REG1, ana_reg1_aaa | PAIR_ANA_FLG);
}

/**
 * @brief	write deep reason to analog register(DEEP_ANA_REG9)
 * @param	none
 * @return	none
 */
void write_deep_ana0(u8 buf)
{
    deep_flag = buf;
    analog_write(DEEP_ANA_REG9, buf);
}

/**
 * @brief	reboot
 * @param	reason: reboot reason
 * @return	none
 * @note	reboot by cpu_sleep_wakeup can not lose the value stored in the analog register
 */
void user_reboot(u8 reason)
{
    write_deep_ana0(reason); //write reason to analog register
    //start_reboot();
    sleep_ms(10);
#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_FX282) || (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_DMS157) || VOLTAGE_TEMP_HANDLE_ENABLE
	analog_write(BATTERY_CAP_REG, gc_vol_cap_reg_temp);
#endif
	cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_TIMER, clock_time() + 50*CLOCK_16M_SYS_TIMER_CLK_1MS); //reboot
}

/**
 * @brief	clear idle count
 * @param	none
 * @return	none
 */
void reset_idle_status()
{
    if (pair_flag)
        return;

    idle_count = 0;
    loop_cnt = 0;
    idle_tick = clock_time();
}

/**
 * @brief	idle count poll
 * @param	none
 * @return	none
 */
void idle_status_poll()
{
    u8 n;

    n = ((u32)(clock_time() - idle_tick)) / CLOCK_16M_SYS_TIMER_CLK_1S;

    idle_tick += n * CLOCK_16M_SYS_TIMER_CLK_1S;

    idle_count += n;
}

/**
 * @brief	ADV count poll
 * @param	none
 * @return	none
 */
void adv_count_poll()
{
    u8 n;
	
    n = ((u32)(clock_time() - adv_begin_tick)) / CLOCK_16M_SYS_TIMER_CLK_1S;

    adv_begin_tick += n * CLOCK_16M_SYS_TIMER_CLK_1S;

    adv_count += n;
}

#if (OLED_SCREEN_ENABLE)
extern void oled_close(void);
#endif


#if (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
	extern void ext_mcu_enter_sleep(void);
#endif

/**
 * @brief	Enter deep sleep process
 * @param	reason: Enter deep sleep reason
 * @return	none
 */
void enter_deep_aaa(ANA_STATUS_AAA reason)
{
    printf("---->enter_deep_aaa in\n");

#if (OLED_SCREEN_ENABLE)
	oled_close();
#endif

#if (PROJECT_ID == PID_0120)
	rgb3810_sleep_close();
#endif

#if (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
	ext_mcu_enter_sleep();
#endif

#if (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_M45) || (PROJECT_ID == PID_104)
	#if LED_CODE_PWIR_DRIVE_ENABLE
		ws2812_sleep_close();
	#else
		// rgb3810_sleep_close();
	#endif
#endif

#if (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)

	gpio_input_config_status(GPIO_PB5, PM_PIN_PULLDOWN_100K);
	sleep_us(200);
	if ( 0 == gpio_read(GPIO_PB5) )
	{
		cpu_set_gpio_wakeup(GPIO_PB5,  1,  1);
	}
	else
	{
		cpu_set_gpio_wakeup(GPIO_PB5,  0,  1);
	}
#endif

    write_deep_ana0(reason);

    analog_write(USED_DEEP_ANA_REG1, (fun_mode << 1));
	
#if ENTER_SHUWDOWN_WHEN_POWER_IS_CUT_OFF_ENABLE
	if (reason == BATTERT_SHUTDOWN_ENTER_DEEPSLEEP_ANA_AAA)
	{ //Enter shutdown, disallow wake up
	#if BUTTON_FUN_ENABLE_AAA
		btn_set_wakeup_level_suspend(0); //disable button wake up from deep sleep
	#endif

	#if WHEEL_FUN_ENABLE_AAA
		wheel_set_wakeup_level_suspend(0); //disable wheel wake up from deep sleep
	#endif

	#if SENSOR_FUN_ENABLE_AAA
		sensor_set_wakeup_level_suspend(0); //disable sensor wake up from deep sleep
		OPTSensor_Shutdown();
	#endif
	}
	else
#endif
	{ //Enter deep sleep, allow wake up
	#if BUTTON_FUN_ENABLE_AAA
		btn_set_wakeup_level_suspend(1); //enable button wake up from deep sleep
	#endif


	#if WHEEL_FUN_ENABLE_AAA
		wheel_set_wakeup_level_suspend(0); //enable wheel wake up from deep sleep
	#endif

	#if KEY_K4_Independence_CTRL_EN
		if ( 0 == gpio_read(KEY_K4_PIN) )
		{
			cpu_set_gpio_wakeup(KEY_K4_PIN,  1,  0);
		}
		else
		{
			cpu_set_gpio_wakeup(KEY_K4_PIN,  0,  0);
		}
	#endif

	#if KEY_K4_Independence_CTRL_EN
		if ( 0 == gpio_read(KEY_K5_PIN) )
		{
			cpu_set_gpio_wakeup(KEY_K5_PIN,  1,  0);
		}
		else
		{
			cpu_set_gpio_wakeup(KEY_K5_PIN,  0,  0);
		}
	#endif

	#if KEY_MODE_INDEPENDENT_ENABLE
		#if (PROJECT_ID == PID_0120) || (PROJECT_ID == PID_535) || (PROJECT_ID == PID_104)
			// do nothing
		#else
			cpu_set_gpio_wakeup(PIN_BTN_MODE,  !gpio_read(PIN_BTN_MODE),  0);
		#endif
	#endif

	#if KEY_CPI_INDEPENDENT_ENABLE
		#if (PROJECT_ID == PID_104)
			// do nothing
		#else
			cpu_set_gpio_wakeup(KEY_DPI_PIN, !gpio_read(KEY_DPI_PIN), 0);
		#endif
	#endif

	#if (LED_COLOR_MODE_KEY_ENABLE)
		if ( 0 == gpio_read(LED_COLOR_KEY_PIN) )
		{
			cpu_set_gpio_wakeup(LED_COLOR_KEY_PIN,  1,  0);
		}
		else
		{
			cpu_set_gpio_wakeup(LED_COLOR_KEY_PIN,  0,  0);
		}
	#endif

	#if (ADC_TO_GPIO_MODE_EN)
		cpu_set_gpio_wakeup(PIN_BTN_MODE,  !gpio_read(PIN_BTN_MODE),  0);
	#endif

	#if (PROJECT_ID == PID_8693) || (PROJECT_ID == PID_BM2060)
		gpio_input_config_status(GPIO_PB5, PM_PIN_PULLUP_10K);
		cpu_set_gpio_wakeup(GPIO_PB5,  !gpio_read(GPIO_PB5),  1);
	#endif

	#if SENSOR_FUN_ENABLE_AAA
		#if (SENSOR_SHUT_DOWN_ENABLE==0)
			sensor_set_wakeup_level_suspend(1); //enable sensor wake up from deep sleep
			sensor_gpio_powerDownConfig();
		#else
			OPTSensor_Shutdown();
		#endif
	#endif
	}

    clear_pair_flag(); //clear pair flag

#if SENSOR_MOTION_NONE_NEED_MOVE_WAKEUP_ENABLE
	if ( BATTERT_SHUTDOWN_ENTER_DEEPSLEEP_ANA_AAA == reason )
	{
		cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);
	}
#else
    cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0); //Enter deep sleep
#endif
}

