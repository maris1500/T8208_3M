#ifndef MAIN_H_
#define MAIN_H_

#if USB_MODE_ENABLE

	typedef enum {
		USB_MODE_OUT = 0x00,
		USB_MODE_IN_FAILED,
		USB_MODE_IN_SUCC,
		USB_MODE_IN_MAX,
	}usb_change_status_en;

#endif

#if (PM_SYS_LOW_POWER_ENABLE)

typedef struct {
	unsigned char enter_lower:1;
	unsigned char moved:1;
	unsigned char resume_cnt:4;
	
	unsigned char frequency_48Hz_flag:1;

	unsigned char reserve:1;
}mcu_pm_flag_t;

extern mcu_pm_flag_t  mcu_sleep_status;

#endif

#if (WHEEL_METHOD_2_ENABLE)
	
	#define WHEEL_ACTION_DIFF_ERR    6000
	
	typedef enum {
		WHEEL_ACTION_DIR_NONE = 0,
		WHEEL_ACTION_DIR_AA,
		WHEEL_ACTION_DIR_BB,
		WHEEL_ACTION_DIR_MAX,
	}wheel_action_dir_en;
	
	
#endif

#if WEB_HID_ENABLE

	typedef struct {
		unsigned char level_max:4;
		unsigned char level_cur:4;
	}web_dpi_t;

	typedef struct {
		unsigned int charge_mode:2;
		unsigned int battery:8;

		unsigned int rate_max_support:2;
		unsigned int rate_cur:4;

		unsigned int light_mode:4;
		unsigned int sensor_type:3;

		unsigned int active_time:4;

		unsigned int reserve:5;
	}web_sta_t;

	typedef struct
	{
		web_dpi_t  dpi;
		web_sta_t  sta;

		unsigned char temp[11];
	}web_data_cfg_t;

	extern web_data_cfg_t gc_web_data;

	#define  WEB_DATA_LENGTH_MAX  sizeof(web_data_cfg_t)

#endif


typedef struct {
	unsigned short dongle_id_valid_f:1;
	unsigned short pair_success_flag:1;
	unsigned short dongle_id_need_save_flag:1;
	unsigned short need_suspend_flag:1;
	unsigned short pair_success:1;
	unsigned short ui_ota_is_working:1;

	unsigned short mouse_send_need_f:2;
	
}mouse_sta_t;

extern mouse_sta_t gc_mouse_sta;

#endif /* MAIN_H_*/
