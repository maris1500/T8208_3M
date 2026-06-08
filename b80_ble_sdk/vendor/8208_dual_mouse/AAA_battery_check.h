/********************************************************************************************************
 * @file     AAA_battery_check.h
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

#ifndef AAA_BATTERY_CHECK_H_
#define AAA_BATTERY_CHECK_H_

#define BATT_CAP_SAMPING_CYCLES  14
#define BATT_CAP_DIS_TIMEOUT     2

#define BATT_CAP_CALCU_METHOD    1
#define BATT_VOL_DIFF_VALUE      50
#define BATT_CAP_DIFF_VALUE      1

#define BATT_CAP_HIGH        100
#define BATT_CAP_MIDDLE      66
#define BATT_CAP_LOW         10
#define BATT_CAP_SHUT_DOWN   5

typedef enum {
    BATT_TYPE_37V_LITHIUM = 0x00,
    BATT_TYPE_15V_LITHIUM,
    BATT_TYPE_30V_LITHIUM,
    BATT_TYPE_MAX,
}BATT_TYPE_EN;

typedef struct {
	unsigned char sample_first:1;
	unsigned char data_notify:1;
	unsigned char low_batt:1;
	unsigned char power_on:1;

	unsigned char charge_ing:1;
	unsigned char charge_ful:1;
	unsigned char charge_none:1;
	
	unsigned char connect_bat_lower_flag:1;
}bat_status_flag_t;

typedef struct {
	unsigned short batt_vol;
	unsigned short shutdown_vol;
	unsigned short alarm_vol;
	unsigned short max_vol;
	unsigned short min_vol;
	unsigned char  percent;
	unsigned int   batt_detc_tick;
}bat_data_mv_t;

u16  user_btn_adc_check_proc(u8 chn);

extern bat_status_flag_t bat_flag;
extern bat_data_mv_t bat_mv;

void led_batt_indicate_all_off(void);

#endif /* APP_BATTDET_H_ */
