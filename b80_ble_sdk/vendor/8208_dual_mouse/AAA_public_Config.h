/********************************************************************************************************
 * @file     aaa_public_config.h
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

#ifndef _AAA_CONFIG_H_
#define _AAA_CONFIG_H_

#include "AAA_app_config_Telink_QFN32.h"
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "app_buffer.h"
#include "aaa_sensor_pix.h"
#include "aaa_sensor.h"
#include "aaa_24g_rf_frame.h"
#include "aaa_24g_rf.h"
#include "aaa_battery_check.h"
#include "aaa_emi.h"
#include "aaa_app_config.h"

extern my_fifo_t fifo_km;

typedef enum
{
    BLE_DEVICE_ID_0 = 0,
    BLE_DEVICE_ID_1 = 1,
    BLE_DEVICE_ID_MAX,
}BLE_DEVICE_ID_EN;

typedef enum
{
    RF_1M_BLE_MODE = 0,
    RF_2M_2P4G_MODE = 1,
#if USB_MODE_ENABLE
	RF_USB_MODE = 2,
#endif
    RF_MAX_MODE,
}RF_MCU_MODE_EN;

#if LED_DPI_VBAT_DIFF_ENABLE
typedef enum {
	LED_DIFF_NONE = 0,
	LED_DIFF_BATTERY_LOW,
	LED_DIFF_DPI_DISPLAY,
	LED_DIFF_DPI_DEFAULT,
	LED_DIFF_MAX,
}led_differ_en;

extern led_differ_en sc_bat_dpi_flag;

#endif

#if KEY_FEATURE_BOSSKEY_ENABLE
	typedef enum {
		KEY_SPE_NONE = 0,
		KEY_SPE_BOSSKEY,
		KEY_SPE_MAX,
	}key_function_special_en;

	extern key_function_special_en gc_key_special_sta;
#endif

#if MOUSE_FACTORY_RESET_ENABLE

#define MOUSE_FACTORY_KEY_RELEASE_TIMES 5

typedef enum {
	FACTORY_KEY_NONE = 0x00,
	FACTORY_KEY_POWER_UP,
	FACTORY_KEY_RELEASE,
	FACTORY_REBOOT,
	FACTORY_OVER,
}factory_step_e;

extern factory_step_e mouse_factory_reset_flag(void);
extern void mouse_factory_reset_action(void);
extern void mouse_factory_release_times(void);
extern void mouse_factory_power_on_flag(void);

#endif


#if (PROJECT_ID == PID_0120)

typedef enum {

	RGB_CODE_STATUS_NONE = 0x00,
		
	RGB_CODE_STATUS_DPI_START  = 0x01,
	RGB_CODE_STATUS_DPI_OVER   = 0x02,
	
	RGB_CODE_STATUS_LVD_START  = 0x04,
	RGB_CODE_STATUS_LVD_OVER   = 0x08,

	RGB_CODE_STATUS_CHARG_START = 0x10,
	RGB_CODE_STATUS_CHARG_OVER =  0x20,

	RGB_CODE_STATUS_DPI_START_STOP = 0xFE,
	RGB_CODE_STATUS_DPI_OVER_STOP  = 0xFD,
}rgb3810_indicate_sta_en;

typedef enum {

	RGB_CODE_ACT_SET  = 0x00,
	RGB_CODE_ACT_SCAN = 0x01,

}rgb3810_action_en;

typedef enum {

	RGB_CHARGE_NONE = 0x00,
	RGB_CHARGE_LED_ON,
	RGB_CHARGE_LED_OFF,

	RGB_CHARGE_LED_ON_CHANGE,
	RGB_CHARGE_LED_OFF_CHANGE,
}rgb3810_charge_en;

typedef enum {

	RGB_LVD_NONE = 0x00,
	RGB_LVD_LED_ON,
	RGB_LVD_LED_OFF,

	RGB_LVD_LED_ON_CHANGE,
	RGB_LVD_LED_OFF_CHANGE,
	RGB_LVD_LED_OFF_STOP,
}rgb3810_lvd_en;

typedef enum {

	RGB_FCY_NONE = 0x00,
	RGB_FCY_LED_ON,
	RGB_FCY_LED_OFF,

}rgb3810_factory_en;

extern void rgb_code_dpi_set(rgb3810_indicate_sta_en sta, rgb3810_action_en act);
extern void rgb_code_loop_pro(void);

#endif

typedef enum {
	RGB_MODE_LIGHTS_NUION = 0x00,
	RGB_MODE_LIGHTS_PUSH,
	RGB_MODE_LIGHTS_PUSH_BACK,
	RGB_MODE_LIGHTS_SINGLE,
	RGB_MODE_LIGHTS_COLORFUL,
	RGB_MODE_LIGHTS_BREATH,
	RGB_MODE_LIGHTS_MAX,
}rgb3810_mode_en;

#if (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_M45) || (PROJECT_ID == PID_104) || KEY_PAIR_USED_POWERUP_ENABLE
	extern unsigned char gc_hm668_mode_key_used;
	extern unsigned char gc_hm668_battery_try_times;
#endif

extern void coled_led_mode_change_para_clear(void);

#if WEB_KEY_FEATURE_ENABLE

	typedef enum {

		KEY_TRIGGER_NONE  = 0,
		KEY_TRIGGER_OFFICE,
		KEY_TRIGGER_MEDIA,
	}web_trigger_en;

	typedef struct {
		unsigned short trigger:3;
		unsigned short reserve:13;
	}web_set_status_t; 

	typedef enum {

		KEY_START_INDEX  = 0,

		KEY_LEFT_INDEX   = 1,
		KEY_RIGHT_INDEX  = 2,
		KEY_MIDDLE_INDEX = 3,

		KEY_K5_INDEX  = 4,
		KEY_K4_INDEX  = 5,
		KEY_DPI_INDEX = 6,

		KEY_MAX_INDEX,

	}web_key_index_en;

	typedef enum {

		WEB_KEY_NOMAL = 0x10,
		WEB_KEY_FIRE  = 0x30,
		WEB_KEY_DPI   = 0x40,

		WEB_KEY_DPI_LOCK = 0x50,
		WEB_KEY_SPECIAL  = 0x60,
		WEB_KEY_OFFICE   = 0x70,
		WEB_KEY_MEDIA    = 0x80,
		WEB_KEY_MACRO    = 0x90,
	}web_key_type_en;

	extern web_set_status_t gc_web_sta_list;
		
	extern unsigned char web_key_special_tab[6];

	extern char web_key_nomal(unsigned char index);
	extern char web_key_dpi(unsigned char index);
	extern char web_key_office(unsigned char index);
	extern char web_key_media(unsigned char index);

	extern unsigned short int web_key_left_function(void);
	extern unsigned short int web_key_right_function(void);
	extern unsigned short int web_key_middle_function(void);
	extern unsigned short int web_key_k4_function(void);
	extern unsigned short int web_key_k5_function(void);
	extern unsigned short int web_key_dpi_function(void);

#endif


#if (PROJECT_ID == PID_104)

	typedef enum {
		LED_ADV_OFF = 0,
		LED_ADV_ON,
		LED_ADV_VBAT_DPI,
		LED_ADV_SUCC,
		LED_ADV_MAX,
	}LED_ADV_STA_EN;

	extern LED_ADV_STA_EN led_adv_sta;

#endif

#if LED_CODE_PWIR_DRIVE_ENABLE

typedef enum {
	PWM_MODE_NORMAL = 1,
	PWM_MODE_COUNT,
	PWM_MODE_IR,
	PWM_MODE_IR_FIFO,
	PWM_MODE_IR_DMA_FIFO,
}pwm_mode_en;

#define	 PWM_MODE_OPTION	PWM_MODE_IR_DMA_FIFO

#endif

typedef struct
{
    /** Minimum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
    u16 Min;
    /** Maximum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
    u16 Max;
    /** Number of LL latency connection events (0x0000 - 0x03e8) */
    u16 latency;
    /** Connection Timeout (0x000A - 0x0C80 * 10 ms) */
    u16 timeout;
} gap_periConnectParams_t;

//--------------------Button function define----------------------------------------
#define MS_BTN_DESKTOP       0x80
#define MS_BTN_AI            0x0100
#define MS_BTN_REPORT_RATE   0X0200
#define MS_BTN_AUTO_DRAW     0x0800
#define MS_BTN_ROLLER_LEFT   0x1000
#define MS_BTN_ROLLER_RIGHT  0x2000

#define MS_BTN_LEFT_MIDDLE  (KEY_BTN_LEFT|KEY_BTN_MIDDLE)
#define COMB_BTN_EMI  		(KEY_BTN_LEFT | KEY_BTN_MIDDLE | KEY_BTN_RIGHT)
#define COMB_BTN_PAIR		(KEY_BTN_RIGHT | KEY_BTN_MIDDLE)
#define COMB_BTN_LEFT_MIDDLE_RIGHT (KEY_BTN_LEFT | KEY_BTN_MIDDLE | KEY_BTN_RIGHT)

//------------tx power define-------------------

#if DEVICE_ME_PRODUCTION
	#define DEFAULT_NORMAL_TX_POWER   RF_POWER_P5p92dBm
	#define DEFAULT_PAIR_TX_POWER	  RF_POWER_N50dBm
	#define DEFAULT_EMI_TX_POWER      RF_POWER_P0p59dBm
#else
	#define DEFAULT_NORMAL_TX_POWER   RF_POWER_P5p92dBm
	#define DEFAULT_PAIR_TX_POWER	  RF_POWER_P0p59dBm
	#define DEFAULT_EMI_TX_POWER      RF_POWER_P0p59dBm
#endif


//--------------------------------------------------
//----------------------------------------------------------------------------
#define ABS(a,b)	((a>b)?(a-b):(b-a))


typedef struct
{
    u32  dev_mac; 			//00~03
    
    u32  manufacturer_addr;	// 04~07
    
    u8   bat_type;			//08
    u8	 tx_power;			//09
    u8	 paring_tx_power;	//0a
    u8   emi_tx_power;   	//0b
    
    u8	 wheel_direct;	 	//0c
    u8	 internal_cap;   	//0d
    u8   sensor_direct;  	//0e
    u8 	 aes_enable; 	 	//0f

	u8   adc_calibrate_en;	//10
	u8   debug_en;			//11
	u16  rf_vid;			//12~13
	
#if (AES_METHOD == 1)
    u8   pub_key[16];       //14~23
#endif
    u8   device_name[29];	//24~40
} custom_cfg_t;
extern u8 report_rate;

extern custom_cfg_t user_cfg;

#if (AES_METHOD == 1)
	extern u8 pub_key[];
#endif

extern u8 device_name_len;
extern u8 tbl_advData_aaa[];

extern u8 sensor_type;
extern u8 product_id1;
extern u8 product_id2;
extern u8 product_id3;
extern u8 connect_ok;


typedef enum
{
	FLASH_SIZE_USED_128K,
    FLASH_SIZE_USED_512K,
} FLASH_SIZE_T;
extern FLASH_SIZE_T flash_size;

typedef enum
{
    FLASH_LOCK_NONE_BLOCK = 0,
	FLASH_LOCK_HALF_BLOCK,
    FLASH_LOCK_ALL_BLOCK,
} FLASH_LOCK_BLOCK;

extern unsigned int flash_mid;
extern void flash_lock_handle(FLASH_LOCK_BLOCK Flash_block);


typedef enum
{
    STATE_POWERON = 0,
	STATE_SYSNC,
    STATE_PAIRING,
    STATE_NORMAL,
} MOUSE_MODE;

typedef enum{
	RF_IDLE_STATUS = 0,
    RF_TX_START_STATUS = 1,
    RF_TX_END_STATUS = 2,
    RF_RX_START_STATUS = 3,
    RF_RX_END_STATUS = 4,
	RF_RX_TIMEOUT_STATUS = 5,
}RF_STATUS_USER;

extern _attribute_data_retention_user volatile unsigned char rf_state;
extern _attribute_data_retention_user volatile unsigned char device_ack_received;

extern u8 device_channel;

typedef enum
{
    TWO_SPEED_SWITCH = 0,
    THREE_SPEED_SWITCH = 1,
} SWITCH_TYPE_AAA;

typedef enum
{
    POWER_ON_ANA_AAA = 0,
    DEEP_SLEEP_ANA_AAA = 1,
    MODE_CHANGE_REBOOT_ANA_AAA = 2,
    BLE_PAIR_REBOOT_ANA_AAA = 3,
    CONN_PARAM_FAIL_REBOOT_ANA_AA = 4,
    CLEAR_FLAG_ANA_AAA = 5,
    MUTI_DEVICE_REBOOT_ANA_AAA = 6,
    MODE_CHANGE_TO_CHARGING_REBOOT_ANA_AAA = 7,
    MODE_CHANGE_TO_USB_REBOOT_ANA_AAA = 8,
    BATTERT_LOW_ENTER_DEEPSLEEP_ANA_AAA = 9,
    BATTERT_SHUTDOWN_ENTER_DEEPSLEEP_ANA_AAA = 10,
    FACTORY_RESET_AAA = 11,
} ANA_STATUS_AAA;

typedef enum
{
    POWER_ON_STATUS_AAA = 0,
    HIGH_ADV_STATUS_AAA = 1,
    LOW_ADV_STATUS_AAA = 2,
    BEGIN_CONNECTED_STATUS_AAA = 3,
    OK_CONNECTED_STATUS_AAA = 4,
    T5S_CONNECTED_STATUS_AAA = 5,
    DEEP_SLEEPE_STATUS_AAA = 6,
    DEEP_TERMINATE_STATUS_AAA = 7,
   // MODE_CHANGE_TERMINATE_STATUS_AAA = 8,
   // CONN_PARAM_FAIL_TERMINATE_STATUS_AAA = 9,
   // MUTI_DEVICE_CHANGE_STATUS_AAA = 10,
} BLE_STATUS_AAA;
	
typedef enum
{
    KEY_PRESS_EVENT_AAA = BIT(0),
    NEW_KEY_EVENT_AAA = BIT(1),
    SENSOR_DATA_EVENT_AAA = BIT(2),
    WHEEL_DATA_EVENT_AAA = BIT(3),
} key_EVENT_AAA;
	
typedef enum
{
    UNKNOW_OS_TYPE = 0,
    APPLE_OS_TYPE = 1,
    ANDROID_OS_TYPE = 2,
    WINDOWS_OS_TYPE = 3,
    MAC_OS_TYPE = 4,
} PC_OS_TYPE_AAA;
	
typedef enum
{
    HAS_MOUSE_REPORT = BIT(0),
    HAS_KEYBOARD_REPORT = BIT(1),
    HAS_CONSUMER_REPORT = BIT(2),
    HAS_JOYSTIC_REPORT = BIT(3),
} HAS_REPORT_TYPE_AAA;

typedef enum
{
    MULTIPIPE_1 = 0,
    MULTIPIPE_1_DOT_5 = 1,
    MULTIPIPE_2_DOT_0 = 2,
    MULTIPIPE_2_DOT_5 = 3,
    MULTIPIPE_3_DOT_0 = 4,
} SENSOR_MULTIPIPE_XY;


typedef struct
{
    u32 dongle_id;	//4

#if (AES_METHOD == 1)
    u8 key[12];		//16
#endif

    u8 slave_mac_addr[4];
    u8 mode;
    u8 mast_id;

#if BATTERT_STORAGE_ENABLE
	u8 battery_value;
	u8 temp1;
#elif (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_M45) || (PROJECT_ID == PID_104) || (PROJECT_ID == PID_S600)
	u8 code_led_sw;
	u8 code_led_mode;
#else
	#if MOUSE_CODE_LOCK_ENABLE
		u8 active_flag;
		u8 temp1;
	#else
		u16 temp1; 		//24
	#endif
#endif

#if DPI_SAVE_FLASH
	u8 d24g_dpi;
	u8 ble_dpi[4];
	u8 temp2[3]; 	//32
#else
	u8 temp2[8]; 	//32
#endif
} FLASH_DEV_INFO_AAA;

#define SAVE_MAX_IN_FLASH	sizeof(FLASH_DEV_INFO_AAA)

extern FLASH_DEV_INFO_AAA flash_dev_info;

#if SHOW_MAST_REAL_MAC_DEBUG
typedef struct
{
    u32 bin_crc;
    u8 rsv_1[3];
    u8 sensor_type;
    u8 sensor_pd1;
    u8 sensor_pd2;
    u8 sensor_pd3;
    u8 ms_data_len;
#if SHOW_MAST_REAL_MAC_DEBUG
    u8 master_mac[6];
#endif
} OUTPUT_DEV_INFO_AAA;

extern OUTPUT_DEV_INFO_AAA output_dev_info;
#endif


extern u8 deep_flag;
extern u8 pair_flag;
extern u8 ana_reg1_aaa;

extern u8 mode_change_flag;
extern u8 device_switch_flag;

extern u8 suspend_wake_up_enable;
extern u8 has_new_key_event;

extern u8 fun_mode;


extern u32 power_on_tick;
extern u32 start_tick;

extern u8 active_disconnect_reason;
extern u8 ble_status_aaa;

extern ble_sts_t  blc_att_setServerDataPendingTime_upon_ClientCmd(u8 num_10ms);


// TODO: extern _attribute_ram_code_sec_  ll_mac_t  bltMac;
extern u8 my_batVal[1] ;

extern volatile unsigned int rf_rx_timeout_us;

extern int binding_master_addr_idx;

extern mouse_data_t ms_data;


// extern u8 has_new_report_aaa;

extern u32 adv_begin_tick;
extern u32 adv_count;
extern u32 idle_tick;
extern u32 idle_count;
extern u32 loop_cnt;


extern u16 btn_value;
extern u16 last_btn_value;
extern u8  device_status;
extern u32 wakeup_next_tick;
extern u8  dpi_value;

#if SENSOR_FUN_ENABLE_AAA
    extern u8 sensor_fun;
#endif

// extern u8  xy_multiple_flag;
extern int dev_info_idx;

extern void user_init_normal();

extern void main_loop(void);
extern void save_dev_info_flash();
extern int cccWrite(void *p);

extern BLE_DEVICE_ID_EN ble_mode_device_id_get(void);
extern RF_MCU_MODE_EN mcu_mode_get(void);

extern void clear_pair_flag();

extern void set_pair_flag();
extern void user_reboot(u8 reason);

extern void d24_start_pair();
extern void reset_idle_status();
extern void idle_status_poll();
extern void adv_count_poll();

extern u8   wheel_get_value(u32 wheel_prepare_tick);
extern void sensor_gpio_powerDownConfig();
extern void	rf_power_enable(int en);
extern u8   adaptive_smoother();

extern void ble_notify_data_proc_aaa();
extern u16  btn_scan();
extern u8   btn_get_value();
extern void btn_set_wakeup_level_suspend(u8 enable);
extern void sensor_set_wakeup_level_suspend(u8 enable);
extern void sensor_set_wakeup_level_deepsleep(u8 enable);

extern void wheel_set_wakeup_level_suspend(u8 enable);

extern void check_sensor_dircet(u8 sensor_dir);

extern u8 clear_press_key_aaa();
extern void d24_main_loop();
extern void d24_user_init();
extern void enter_deep_aaa(ANA_STATUS_AAA reason);

extern void btn_dpi_set();
extern void sensor_dpi_set(u8 dpi);

extern void emi_process();
extern int flash_info_load_aaa(u32 s_addr, u8 *d_addr,  int len);
extern void flash_write_page_user(unsigned long addr, unsigned long len, unsigned char *buf);


extern u8 get_ble_data_report_aaa();
extern void hw_init();
extern void led_hw_init();
extern void gpio_input_config(GPIO_PinTypeDef pin);
extern void gpio_output_config(GPIO_PinTypeDef pin);
extern void gpio_higsesistance_config(GPIO_PinTypeDef pin);
extern void gpio_input_config_status(GPIO_PinTypeDef pin, GPIO_PullTypeDef staus);
extern void gpio_output_config_status(GPIO_PinTypeDef pin, unsigned char led_sta);

extern void led_ble_ConnectedStatus();

extern void led_ble_Adv_poll();
extern void led_2p4_Adv_poll();

extern void vbat_led_on(u8 on);
extern int vbat_led_show();

extern void vbat_led_proc();
extern int dpi_led_show(unsigned char times);
extern void device_led_struct_clear(void);


extern u32 mouse_wheel_prepare_tick(void);

extern unsigned int OPTSensor_motion_report(u32 no_overflow);
extern void OPTSensor_Shutdown(void);
extern void mouse_xy_multiple(void);

extern void ui_loop(void);

#if BLT_SOFTWARE_TIMER_ENABLE
	extern void delet_soft_time(void);
	extern void add_soft_time(void);
	extern void check_softe_time(void);
#endif


extern void btn_dpi_lock_set(unsigned char value);
extern void btn_dpi_increase_set(void);
extern void btn_dpi_lock_set(unsigned char value);
extern void btn_dpi_reduce_set(void);

extern void ble_pm_aaa();

extern void user_batt_check_init();
extern void user_batt_check_proc();
extern u8 user_button_status_check(void);


extern int speckWrite(void *p);
extern void write_deep_ana0(u8 buf);
extern void user_battery_power_check();
extern void user_button_check_proc(void);
extern void emi_key_check_aaa(u16 check_ms);
extern void pair_key_check_aaa(u16 check_ms);
extern void mode_change_check_aaa(u16 check_ms);
extern void BufToHexString(u8 *rp, u8 *sp, u8 len);
extern int save_data_to_flash(unsigned long addr, unsigned long len, unsigned char *buf,int *offset);
extern void get_current_cpi_value_handle(void);
extern void set_current_cpi_value_handle(void);

extern unsigned char led_batt_dpi_working_on(void);
extern void batt_check_time_reload(void);
extern void vbat_dpi_struct_clean(void);

extern void led_g24_conn_status_set(void);
extern void led_g24_conned_succ_display(void);
extern void led_g24_conned_succ_dis_next(void);

extern unsigned char battery_voltage_percent(void);

#if MODULE_DIGITAL_TUBE_ENABLE
	extern void digital_tube_io_out(GPIO_PinTypeDef pin, unsigned char led_sta);
#endif

typedef enum
{
	MOUSE_REPORT_CCC_EN=0x01,
	BATTERY_LEVEL_CCC_EN=0X02,
	KB_CONSUME_KEY_CCC_EN=0X04,
	KB_NORMAL_KEY_CCC_EN=0X08,
}Host_CCC_EN_ENUM;
//extern  u32 ccc_flag;

extern  u16 ccc_flag;

#endif

