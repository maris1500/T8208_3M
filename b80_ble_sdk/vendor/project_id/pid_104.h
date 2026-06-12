#ifndef _PID_104_H_
#define _PID_104_H_

#define WEB_FLASH_TEST_EN  1

#define WEB_HID_ENABLE  1
#define WEB_HID_LENGTH  32

#define MCU_QFN40_ENABLE        1
#define UL1001_DPI_2_ENABLE     0

#define FRE_RC_48MHZ_ENABLE  1

#define TUBE_BAT_LOWER_ENABLE 0

#define DEVICE_KEYBOARD     2
#define DEVICE_MOUSE        1
#define DEVICE_TYPE_INDEX   DEVICE_MOUSE //1:mouse, 2:keyboard

#define G24_MODE_ENABLE     1
#define BLE_MODE_ENABLE     1
#define USB_MODE_ENABLE     1

#define LED_G24_ENABLE      0
#define LED_BT1_ENABLE      0
#define LED_BT2_ENABLE      0

#define LED_BAT_VOLTAGE_ENABLE      1
#define LED_DPI_INDICATE_ENABLE     1

#define LED_DPI_BAT_SINGLE_CTRL_ENABLE   0
#define LED_MODE_MIX_CTRL_ENABLE         0

#define LED_G24_INIT_HIGH_ENABLE    1
#define LED_BLE1_INIT_HIGH_ENABLE   1
#define LED_BLE2_INIT_HIGH_ENABLE   1

#define LED_DPI_HIGH_ENABLE         0
#define LED_DPI_Independence_ENABLE 0

#define LED_DEVICE_POLAR_ENABLE      0

#define LED_OFF    0
#define LED_ON     1

#define SENSOR_DIRECTION_CLOCK_12    0
#define SENSOR_DIRECTION_CLOCK_3     1
#define SENSOR_DIRECTION_CLOCK_9     2
#define SENSOR_DIRECTION_CLOCK_6     3
#define SENSOR_DIRECTION_OPTION      SENSOR_DIRECTION_CLOCK_6

#define	BATTERY_CAP_1500MV     1
#define	BATTERY_CAP_3000MV     2
#define	BATTERY_CAP_3700MV     3
#define BATTERY_CAP_OPTION     BATTERY_CAP_3700MV

#define BATTERY_CAP_TIME_RESET_NONE_ENABLE  1

#define KEY_ADC_MODE_MULTI_FUNC_ENABLE  0
#define KEY_ADC_VOLTAGE_VALUE           200

#define BATT_VOLTAGE_SMOOTH_ENABLE     1
#define BATT_SHUTDOWN_NONE_ENABLE      0

#define BATT_CACL_4200mV_ENABLE  1
#define BATT_CACL_1500mV_ENABLE  0
#define BATT_CACL_3000mV_ENABLE  0

#define DEVICE_NAME_1  "BT1 Mouse"
#define DEVICE_NAME_2  "BT2 Mouse"

#define MOUSE_REPORT_250HZ_ENABLE       1

#define SWITCH_MODE_ENABLE              0
#define SWITCH_MODE_IO_TOGGLE_ENABLE    0
#define SWITCH_MODE_IO_FLOAT_ENABLE     1
#define SWITCH_MODE_IO_PULLUP_ENABLE    0
#define SWITCH_MODE_IO_PULLDOWN_ENABLE  0

#define IO_CHANGE_COUNT_MAX            20

#define KEY_MODE_CHANGE_ENABLE         1
#define KEY_MODE_CHANGE_G24_BLE_ENABLE 0

#define KEY_MODE_INDEPENDENT_ENABLE    1
#define KEY_MODE_COMBINE_ENABLE        0
#define KEY_CPI_INDEPENDENT_ENABLE     1

#define KEY_FEATURE_BOSSKEY_ENABLE     0
#define KEY_FEATURE_HOME_ENABLE        0
#define KEY_FEATURE_DESKTOP_ENABLE     0
#define KEY_FEATURE_MIKE_AI_ENABLE     0
#define KEY_LEFT_MIDDLE_RIGHT_ENABLE   0

#define PM_SYS_LOW_POWER_ENABLE        1


#define LED_RGB_RUN_TIME          60
#define LED_LOGO_ON_TIME          20

#define KEY_MR_PAIR_NONE          0
#define KEY_MR_PAIR_G24           1
#define KEY_MR_PAIR_BLE           2
#define KEY_MR_PAIR_G24_BLE       3

#define KEY_MR_PAIR_OPTION        KEY_MR_PAIR_NONE

#define KEY_MODE_PAIR_NONE        0
#define KEY_MODE_PAIR_G24         1
#define KEY_MODE_PAIR_BLE         2
#define KEY_MODE_PAIR_G24_BLE     3

#define KEY_MODE_PAIR_OPTION      KEY_MODE_PAIR_G24_BLE


#define AES_METHOD				0 //1:Enable the AES-128 encryption and decryption function
#define DATA_3_CHOOSE_1_ENABLE	0 //1:Enable select a valid data from three data, advised to disable this function

#if (AES_METHOD == 1 && DATA_3_CHOOSE_1_ENABLE == 1)
	#error Not support AES and DATA_3_CHOOSE_1 work at the same time
#endif

#define ENTER_SHUWDOWN_WHEN_POWER_IS_CUT_OFF_ENABLE	0 //0:when shutdown battery allow wakeup, 1:when shutdown battery disallow wakeup

#define ADAPT_ALL_HW_AUTO_DRAW_IN_D24G_MODE_DEBUG	0 //

#define MOUSE_DATA_LEN_AAA      6

#define TEST_LOST_RATE			0 //test 2.4g rf lost rate
#define BLE_SNIFF_DEBUG			0 //for ti packet sniff   if enable mouse do not enter sleep for ever
#define BLE_OTA_LED_DEBUG		0 //led show  ota result
#define DEBUG_GPIO_AAA			0 //no use

#define DEVICE_NAME_INCLUDE_MAC_DEBUG	1 //1:Add MAC to the BLE name
#define MUTI_DEVICE_SWITCH_DEBUG		1 //1:Enable BLE multi-channel switch

#define TEST_MCU_CURRENT_DEBUG			0 //test mcu current, in the mode,sensor shut down
#define SHOW_MAST_REAL_MAC_DEBUG		0 //for test get master real addr
#define FLASH_LOCK_ENABLE				0 //1:lock fireware block, 0:unlock
#define BATTERT_STORAGE_ENABLE			0 //1:storage battery value to flash


/* Power Management */
#define BLE_APP_PM_ENABLE				1 //1:Enable power management "pwm function protected"

#define PM_DEEPSLEEP_RETENTION_ENABLE   0 //0:disable enter deepsleep retention

#define BLE_ADV_ENTER_DEEPSLEEP_AFTER_TIME_OUT_ENABLE_AAA		1 //1:BLE ADV overtime enter deep sleep
#define BLE_CONNECT_ENTER_DEEPSLEEP_AFTER_TIME_OUT_ENABLE_AAA	1 //1:BLE connect overtime enter deep sleep

#define D24G_ADV_ENTER_DEEPSLEEP_AFTER_TIME_OUT_ENABLE_AAA		1 //1:2.4G ADV overtime enter deep sleep
#define D24G_CONNECT_ENTER_DEEPSLEEP_AFTER_TIME_OUT_ENABLE_AAA	1 //1:2.4G connect overtime enter deep sleep

#define BLE_ADV_TIMER_OUT  				180 // unit 1s 180
#define D24G_ADV_TIMER_OUT  			60  // unit 1s 60

#define BLE_CONNECT_TIME_OUT  			600 // unit 1s 600
#define D24G_CONNECT_TIME_OUT  			600 // unit 1s 600

#if PM_DEEPSLEEP_RETENTION_ENABLE
	#define _attribute_data_retention_user	_attribute_data_retention_sec_
#else
	#define _attribute_data_retention_user
#endif

#define MOUSE_PIPE1_DATA_WITH_DID	1 //If you want to fit the old hamster dongle (firmware version :v5.3) it must set 0

#define EMI_TEST_FUN_ENABLE_AAA		0 //enable emi test function

#define BATT_CHECK_ENABLE			1 //enable battery sample function
#define WHEEL_FUN_ENABLE_AAA		1 //enable wheel encoder function
#define BUTTON_FUN_ENABLE_AAA		1 //enable button function

#define SENSOR_FUN_ENABLE_AAA		1 //enable optical sensor function
#define SENSOR_CS_ENABLE			0 //0-CS PIN not be used, 1-CS PIN is used
#define SENSOR_MISO_ENABLE          0
#define SENSOR_MOTION_ENABLE		1 //0-MOTION PIN not be used, 1-MOTION PIN is used
#define SENSOR_SHUT_DOWN_ENABLE 	0 //0-sensor enter sleep2 when deep sleep, 1-sensor enter shutdown when deep sleep

#define BLT_APP_LED_ENABLE          1 //anable led display
#define ADC_TO_GPIO_MODE_EN         0
#define ADC_VBAT_TO_BTN             0//(1&&(!ADC_TO_GPIO_EN))


#define DIRECT_ADV_TO_UNDIRECT_ENABLE   0 //1:Auto switching between directed and undirected adv, 0:just directed adv
#define AUTO_CHECK_OS_TYPE   			0 //Identify the host operating system when ble connect
#define Microsoft_Swift_Pairing_ENABLE	1 //if enable then device_name_len<=11
#define DEVICE_NAME_LENGTH_MAX			22//max length of BLE name
#define	BLT_SOFTWARE_TIMER_ENABLE		0 //BLE soft timer

#define MUTI_SENSOR_ENABLE              0 //1:support all optical sensor
#define DPI_SAVE_FLASH                  1 //1:enable store dpi value into flash

#define DO_TASK_WHEN_RF_IN_24G_MODE_ENALBE	1 //1:normal function, 0:debug function
#define ENTER_PAIR_WHEN_NEVER_PAIRED_ENABLE	1 //1:when a unpaired device is powered on, enters pairing mode
#define REBOOT_WHEN_SWITCH_MODE_AND_CHANNEL_ENABLE 1

#if (AES_METHOD==1)
	#define D24G_PAIR_TIMER_OUT             410 //unit 1us
	#define D24G_COMMUNICATION_TIMER_OUT    280 //unit 1us
#else
	#define D24G_PAIR_TIMER_OUT             350 //unit 1us
	#define D24G_COMMUNICATION_TIMER_OUT    280 //unit 1us
#endif

#define KEY_PRESS_HOLD_CHECK_TIME3			2800 //unit 1ms

#if SENSOR_FUN_ENABLE_AAA
	#if SENSOR_MOTION_ENABLE
		#define PIN_SIF_MOTION          GPIO_PC0    //39a30_v1.0 j4 S-tx
		#define PC0_INPUT_ENABLE        1
		#define PC0_OUTPUT_ENABLE       0
		#define PC0_DATA_OUT            1
		#define PULL_WAKEUP_SRC_PC0     PM_PIN_PULLUP_1M
	#endif

	#if SENSOR_CS_ENABLE
		#define PIN_SENSOR_CS          GPIO_PD6    //39a30_v1.0 j4 S-tx
		#define PD6_INPUT_ENABLE       0
		#define PD6_OUTPUT_ENABLE      1
		#define PD6_DATA_OUT           1
		#define PULL_WAKEUP_SRC_PD6    PM_PIN_PULLUP_1M
	#endif

	#define PIN_SIF_SCL                GPIO_PB7    //39a30_v1.0  j15
	#define PB7_INPUT_ENABLE           0
	#define PB7_OUTPUT_ENABLE          1
	#define PB7_DATA_OUT               1
	#define PULL_WAKEUP_SRC_PB7        PM_PIN_PULLUP_1M

	#define PIN_SIF_SDA                GPIO_PB6    //39a30_v1.0  j15
	#define PB6_INPUT_ENABLE           1
	#define PB6_OUTPUT_ENABLE          1
	#define PB6_DATA_OUT               1
	#define PULL_WAKEUP_SRC_PB6        PM_PIN_PULLUP_10K

#if (SENSOR_MISO_ENABLE)
	#define PIN_SIF_SDA_READ           GPIO_PA0    //39a30_v1.0  j15
	#define PA0_INPUT_ENABLE           1
	#define PA0_OUTPUT_ENABLE          0
	#define PA0_DATA_OUT               1
	#define PULL_WAKEUP_SRC_PA0        PM_PIN_PULLUP_10K
#endif

#endif

#if BUTTON_FUN_ENABLE_AAA

	#define KEY_NONE        0x00
	#define KEY_BTN_LEFT    0x01
	#define KEY_BTN_RIGHT   0x02
	#define KEY_BTN_MIDDLE  0x04
	#define KEY_BTN_K5      0x08
	#define KEY_BTN_K4      0x10

	#define KEY_BTN_MODE    0x20
	#define KEY_BTN_CPI     0x40

	#if KEY_FEATURE_BOSSKEY_ENABLE
		#define KEY_FEATURE_BOSSKEY_VALUE 0x100
	#endif

	#define KEY_RIGHT_MIDDLE_COMBINE_ENABLE 0

	#define KEY_LEFT_RIGHT_MIDDLE_COMBINE  (KEY_BTN_LEFT | KEY_BTN_RIGHT | KEY_BTN_MIDDLE)
	#define KEY_RIGHT_MIDDLE_COMBINE    (KEY_BTN_RIGHT | KEY_BTN_MIDDLE)

	#define KEY_LEFT_VALUE    		KEY_BTN_LEFT
	#define KEY_LEFT_VALUE_2    	KEY_NONE

	#define KEY_RIGHT_VALUE   		KEY_BTN_RIGHT
	#define KEY_RIGHT_VALUE_2   	KEY_NONE

	#define KEY_MIDDLE_VALUE  		KEY_BTN_MIDDLE
	#define KEY_MIDDLE_VALUE_2  	KEY_NONE

	#define BTN_PM_WAKEUP_SRC       PM_PIN_PULLUP_1M

	#define PIN_BTN_LEFT            GPIO_PB3
	#define PB3_INPUT_ENABLE        1
	#define PB3_OUTPUT_ENABLE       0
	#define PB3_DATA_OUT            1
	#define PULL_WAKEUP_SRC_PB3     BTN_PM_WAKEUP_SRC

	#define PIN_BTN_RIGHT           GPIO_PB0
	#define PB0_INPUT_ENABLE        1
	#define PB0_OUTPUT_ENABLE       0
	#define PB0_DATA_OUT            1
	#define PULL_WAKEUP_SRC_PB0     BTN_PM_WAKEUP_SRC

	#define PIN_BTN_MIDDLE          GPIO_PA7
	#define PA7_INPUT_ENABLE        1
	#define PA7_OUTPUT_ENABLE       0
	#define PA7_DATA_OUT            1
	#define PULL_WAKEUP_SRC_PA7     BTN_PM_WAKEUP_SRC
#endif

#define KEY_DPI_IN_ENABLE  1
#define KEY_K4_IN_ENABLE   0
#define KEY_K5_IN_ENABLE   0

#define KEY_K4_Independence_CTRL_EN  0
#define KEY_K5_Independence_CTRL_EN  0

#if KEY_DPI_IN_ENABLE || KEY_CPI_INDEPENDENT_ENABLE
	#define KEY_DPI_PIN		GPIO_PB1
#endif

#if KEY_K4_IN_ENABLE || KEY_K4_Independence_CTRL_EN
	#define KEY_K4_PIN		GPIO_PC0
#endif

#if KEY_K5_IN_ENABLE || KEY_K5_Independence_CTRL_EN
	#define KEY_K5_PIN		GPIO_PB7
#endif


#define DEBUG_MODE  0

#if DEBUG_MODE
	#define PRINT_BAUD_RATE       100000   // 1M baud rate,should Not bigger than 1Mb/s
	#define DEBUG_INFO_TX_PIN     GPIO_PA3 // Software uart printing gpio
#endif

#if WHEEL_FUN_ENABLE_AAA
	#define PIN_WHEEL_1             GPIO_PC2    //39a30_v1.0 j15
	#define PC2_INPUT_ENABLE        1
	#define PC2_OUTPUT_ENABLE       0
	#define PC2_DATA_OUT            0
	#define PULL_WAKEUP_SRC_PC2     PM_PIN_PULLUP_1M

	#define PIN_WHEEL_2             GPIO_PC3    //39a30_v1.0 j22 d7
	#define PC3_INPUT_ENABLE        1
	#define PC3_OUTPUT_ENABLE       0
	#define PC3_DATA_OUT            0
	#define PULL_WAKEUP_SRC_PC3     PM_PIN_PULLUP_1M

	#define WHEEL_ADDRES_D2         0x05
	#define WHEEL_ADDRES_D3         0x04
#endif


#if (LED_G24_ENABLE)
	#define PIN_24G_LED     GPIO_PD6
#endif

#if (LED_BT1_ENABLE)
	#define PIN_BLE1_LED    GPIO_PD6
//	#define PB1_INPUT_ENABLE       0
//	#define PB1_OUTPUT_ENABLE      1
//	#define PB1_DATA_OUT           0
#endif

#if (LED_BT2_ENABLE)
	#define PIN_BLE2_LED    GPIO_PD6
#endif

#if (LED_BAT_VOLTAGE_ENABLE) || LED_DPI_INDICATE_ENABLE
	#define PIN_BAT_DPI_LED     GPIO_PD6
#endif


#if (BATT_CHECK_ENABLE)
	#define GPIO_VBAT_DETECT       ADC_GPIO_PB4
#endif

#if (ADC_TO_GPIO_MODE_EN)
	#define PIN_BTN_MODE            GPIO_PB5
#endif

#if (KEY_MODE_INDEPENDENT_ENABLE) || KEY_MODE_COMBINE_ENABLE
	#define PIN_BTN_MODE            GPIO_PC1
#endif


#if SWITCH_MODE_ENABLE
	#define MODE_SWITCH_PIN          GPIO_PD3
#endif

#if (MODULE_VOLICE_ADJUST_ENABLE)
	#define VOICE_A_PIN             GPIO_PB5
	#define PB5_INPUT_ENABLE        1
	#define PB5_OUTPUT_ENABLE       0
	#define PB5_DATA_OUT            0
	#define PULL_WAKEUP_SRC_PB5     PM_PIN_PULLUP_1M

	#define VOICE_B_PIN             GPIO_PB4
	#define PB4_INPUT_ENABLE        1
	#define PB4_OUTPUT_ENABLE       0
	#define PB4_DATA_OUT            0
	#define PULL_WAKEUP_SRC_PB4     PM_PIN_PULLUP_1M
#endif

#define DPI_S210B_3065XY_DEFAULT_ENABLE  1
#define DPI_3065XY_FOUR_LEVEL_ENABLE     1

#define LED_DPI_VBAT_DIFF_ENABLE   0

#if LED_DPI_VBAT_DIFF_ENABLE
	#define LED_DPI_DIFF_ON    LED_ON
	#define LED_BAT_DIFF_ON    LED_OFF
#endif

#define LED_LOGO_ACTION_ENABLE   0
#define LED_LOGO_OFF_WHEN_MOUSE_MOVE 0
#define LED_LOGO_OFF_MOUSE_MOVE_TIMEOUT 800*1000U

#if LED_LOGO_ACTION_ENABLE

	#define LED_LOGO_ACTION_MAX_TIME   60

	#define PIN_LOGO_LED			GPIO_PB4

	#define LED_LOGO_ON 			LED_ON
	#define LED_LOGO_OFF			LED_OFF

	#define LED_LOGO_ACT(x) 		gpio_write(PIN_LOGO_LED,x)
#endif

#define LED_RGB_DPI_ACTION_ENABLE   0

#if LED_RGB_DPI_ACTION_ENABLE
	#define LED_RGB_DPI_PIN_R   GPIO_PB4
	#define LED_RGB_DPI_PIN_G   GPIO_PB5
	#define LED_RGB_DPI_PIN_B   GPIO_PB6

	#define LED_RGB_DPI_ON      LED_OFF
	#define LED_RGB_DPI_OFF     LED_ON

	#define LED_DPI_R_ACT(x)   gpio_write(LED_RGB_DPI_PIN_R,x)
	#define LED_DPI_G_ACT(x)   gpio_write(LED_RGB_DPI_PIN_G,x)
	#define LED_DPI_B_ACT(x)   gpio_write(LED_RGB_DPI_PIN_B,x)
#endif


#define MODULE_DIGITAL_TUBE_ENABLE	0

#if MODULE_DIGITAL_TUBE_ENABLE

	#define TUBE_LOOP_CYCLE	 1000
	#define TUBE_ON_TIME_MAX 60
	#define TUBE_SCAN_TIMES  7

	#define TUBE_SCAN_NOMAL_5_ENABLE  1

	#define TUBE_HIGH	1
	#define TUBE_LOW	0

	#define TUBE_NUM	7
	#define CODE_NUM	10

	#define TUBE_PIN_1	GPIO_PA2
	#define TUBE_PIN_2	GPIO_PA4
	#define TUBE_PIN_3	GPIO_PA5
	#define TUBE_PIN_4	GPIO_PA6
	#define TUBE_PIN_5	GPIO_PA7
	#define TUBE_PIN_6	GPIO_PB2
	#define TUBE_PIN_7	GPIO_PB4

	#define TUBE_PIN_LIST	{TUBE_PIN_1, TUBE_PIN_2, TUBE_PIN_3, TUBE_PIN_4, TUBE_PIN_5, TUBE_PIN_6, TUBE_PIN_7}
	#define TUBE_CODE_LIST	{0x3f,/*0*/  0x06,/*1*/  0x5b,/*2*/  0x4f,/*3*/  0x66,/*4*/  0x6d,/*5*/  0x7d,/*6*/  0x07,/*7*/  0x7f,/*8*/  0x6f/*9*/}

#endif


#define LED_RGB_BAT_DPI_ENABLE   0

#if LED_RGB_BAT_DPI_ENABLE
	#define LED_RGB_DPI_BAT_PIN_R   GPIO_PA3
	#define LED_RGB_DPI_BAT_PIN_B   GPIO_PA4

	#define LED_RGB_DPI_BAT_ON      LED_OFF
	#define LED_RGB_DPI_BAT_OFF     LED_ON

	#define LED_DPI_BAT_R_ACT(x)   gpio_write(LED_RGB_DPI_BAT_PIN_R,x)
	#define LED_DPI_BAT_B_ACT(x)   gpio_write(LED_RGB_DPI_BAT_PIN_B,x)
#endif

#if USB_MODE_ENABLE
	#define USB_IN_CHECK_PIN  	GPIO_PA4
	#define USB_IN()			gpio_read(USB_IN_CHECK_PIN)
	#define USB_OUT()			!gpio_read(USB_IN_CHECK_PIN)
#endif


#define CHARGE_ENABLE  0
#define USB_5V_ENABLE  0

#if CHARGE_ENABLE
	#define CHARGE_PIN  	GPIO_PD6
	#define CHARGE_IN()		!gpio_read(CHARGE_PIN)
	#define CHARGE_OUT()	gpio_read(CHARGE_PIN)
	#define CHARGE_FULL()	gpio_read(CHARGE_PIN)
#endif

#if USB_5V_ENABLE
	#define USB_IN_CHECK_PIN  GPIO_PD5
	#define USB_IN()	gpio_read(USB_IN_CHECK_PIN)
	#define USB_OUT()	!gpio_read(USB_IN_CHECK_PIN)
#endif


#define PIN_DEBUG_RF_TX_LEVEL(x)
#define PIN_DEBUG_RF_RX_LEVEL(x)
#define PIN_DEBUG_RF_RX_TIMEOUT_LEVEL(x)
#define PIN_DEBUG_RF_RX_CRC_OK_LEVEL(x)
#define PIN_DEBUG_RF_RX_DATA_OK_LEVEL(x)
#define PIN_DEBUG_RF_CHN_NEXT_TOGGLE


#define LED_CODE_PWIR_DRIVE_ENABLE  1

#if LED_CODE_PWIR_DRIVE_ENABLE
	// do nothing
#else
	#define LED_CODE_ENABLE  1
#endif

#define LED_CODE_SW_CHECK_ENABLE  0
#define LED_CODE_POWER_ENABLE     1


#if LED_CODE_ENABLE
	#define LED_CODE_PIN  GPIO_PB2
	#define CODE_LED_OUT_H  gpio_write(LED_CODE_PIN, LED_ON)
	#define CODE_LED_OUT_L  gpio_write(LED_CODE_PIN, LED_OFF)

	#define CODE_LED_ON_TIME_MAX 60

#endif

#define LED_CODE_SW_CLOSE_ENABLE  1

#if  LED_CODE_PWIR_DRIVE_ENABLE

	#define WS2812_PWM0_PIN  GPIO_PB2

	#define WS2812_BIT_FREQ     800000
	#define WS2812_BIT_MAX_TICK 58   // (CLOCK_SYS_CLOCK_HZ / ARGB_BIT_FREQ) // 1.21us at 48MHz => 58 ticks
	#define WS2812_T0H_TICK     14   // 14 ~ 0.29us /
	#define WS2812_T1H_TICK     29   // 29 ~ 0.60us  /
	#define WS2812_RESET_TICK   150  // 100 ~ 125us   / 80 ~ 100us 80 > 50us low level, here about 100us
	/* Keep legacy demo symbols for optional pingpong test code. */
	#define IR_DMA_MAX_TICK           WS2812_BIT_MAX_TICK
	#define IR_DMA_SHADOW_MAX_TICK    WS2812_BIT_MAX_TICK

	#define WS2812_PIXEL_NUMBER    24
	#define WS2812_LED_NUMBER      10
	#define WS2812_COLOR_MAX       7
	#define CODE_LED_ON_TIME_MAX   60

	#define WS2812_RGB_BIT_LEN_MAX  (WS2812_PIXEL_NUMBER*WS2812_LED_NUMBER)

	#define WS2812_COLOR_PIXE   0xFF
	#define WS2812_COLOR_NONE   0x00

	#define WS2812_NONE   0x00000000
	#define WS2812_GREEN  0x00FF0000
	#define WS2812_RED    0x0000FF00
	#define WS2812_BLUE   0x000000FF

	#define WS2812_YELLOW  WS2812_GREEN | WS2812_RED
	#define WS2812_CYAN    WS2812_GREEN | WS2812_BLUE
	#define WS2812_PURPLE  WS2812_RED | WS2812_BLUE
	#define WS2812_WHITE   WS2812_RED | WS2812_BLUE | WS2812_GREEN
	#define WS2812_BLACK   WS2812_NONE

	#define WS2812_COMBIN(r, g, b) (g<<16)|(r<<8)|(b)
	#define WS2812_ADD(rr, gg, bb) {.r=rr, .g=gg, .b=bb}

	#define WS2812_COLOR_LIST_NUMBER  7

#endif


#if LED_CODE_SW_CHECK_ENABLE
	#define LED_CODE_SW_CHECK_PIN  GPIO_PD4
#endif

#if LED_CODE_POWER_ENABLE
	#define LED_CODE_POWER_PIN    GPIO_PB5
	#define LED_CODE_POWER_ON()   gpio_write(LED_CODE_POWER_PIN, LED_ON)
	#define LED_CODE_POWER_OFF()  gpio_write(LED_CODE_POWER_PIN, LED_OFF)
#endif

#define LED_COLOR_MODE_ENABLE  0

#if LED_COLOR_MODE_ENABLE
	#define  LED_COLOR_SET_KEY  (KEY_BTN_RIGHT | KEY_BTN_MIDDLE)
#endif

#define LED_COLOR_MODE_KEY_ENABLE  0

#if LED_COLOR_MODE_KEY_ENABLE
	#define  LED_COLOR_KEY_PIN  GPIO_PA7
	#define  KEY_COLOR_VALUE    0x80
#endif


#endif


