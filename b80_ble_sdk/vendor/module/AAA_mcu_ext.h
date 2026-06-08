#ifndef AAA_MCU_EXT_H_
#define AAA_MCU_EXT_H_

#include "../8208_dual_mouse/AAA_app_config_Telink_QFN32.h"

#if (MODULE_MCU_EXTERNAL_ENABLE)

	#define G24_CODE_PAIR_ING      0x11
	#define G24_CODE_CONN_ING      0x14
	#define G24_CODE_CONN_SUCC     0x15

	#define LED_EVT_G24_OFF        0x16
	#define LED_EVT_G24_ON         0x26

	#define BLE1_CODE_PAIR_ING      0x21
	#define BLE1_CODE_CONN_ING      0x24
	#define BLE1_CODE_CONN_SUCC     0x25

	#define BLE2_CODE_PAIR_ING		0x17
	#define BLE2_CODE_CONN_ING		0x18
	#define BLE2_CODE_CONN_SUCC 	0x19

	#define LED_EVT_TEST_1          0x37
	#define LED_EVT_ALL_OFF         0x27

	#define LVD_CODE_ON             0x51
	#define LVD_CODE_OFF            0x52

	#define LOGO_CODE_ON            0x53
	#define LOGO_CODE_OFF           0x54

#if (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
	#define LOGO_DPI_ON             0x32
#else
	#define LOGO_DPI_ON             0x31
#endif

	#define MCU_LED_SEND_TIMES_MAX  1000
	
	#define EXT_IO_1  1
	#define EXT_IO_0  0
	
	#define EXT_IO_OUTPUT(x) gpio_write(EXTER_MCU_IO_PIN, x);

	typedef enum {
		EXT_LED_NONE = 0x00,
		EXT_LED_PAIR,
		EXT_LED_RECONN,
		
		EXT_LED_CONN_SUCC,
		EXT_LED_CONN_FAILED,
		EXT_LED_CONN_END,

		EXT_LED_MAX,
	}ext_led_conn_en;

	typedef enum {
		EXT_LED_EVT_NONE = 0x00,
		EXT_LED_EVT_CPI,
		EXT_LED_EVT_LVD,
		EXT_LED_EVT_LOGO,
		
		EXT_LED_EVT_MAX,
	}ext_led_evt_en;
	
	typedef struct {
		ext_led_conn_en conn;
		ext_led_evt_en  evt;
	}ext_mcu_status_t;

	typedef struct {
		unsigned char  lvd_status:1;
		unsigned char  lvd_act:1;
		unsigned char  lvd_send_times:2;
		
		unsigned char  logo_status:1;
		unsigned char  logo_act:1;
		unsigned char  logo_send_times:2;
	}ext_mcu_lvd_logo_t;

	extern ext_mcu_lvd_logo_t ext_mcu_lvd_logo;
	extern ext_mcu_status_t ext_mcu_io;

	
	void ext_mcu_pro(void);
#endif

#endif /* AAA_MCU_EXT_H_*/
