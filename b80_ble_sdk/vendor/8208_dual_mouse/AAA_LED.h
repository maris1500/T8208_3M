#ifndef AAA_LED_H_
#define AAA_LED_H_

#include "AAA_app_config_Telink_QFN32.h"

typedef enum {
	INDEX_BAT = 0,
#if DPI_BATT_DISPLAY_DIFFER_ENABLE
	INDEX_DPI,
#endif

#if BATT_LOWER_METHOD_22_ENABLE
	INDEX_BATTERY_LOWER,
#endif

	INDEX_MAX,
}LED_TYPE_NUM_EN;

typedef enum {
	LED_TYPE_ALL_OFF = 0,
	LED_TYPE_CAP_LOW,
	LED_TYPE_MIDDLE,
	LED_TYPE_HIGH,
	LED_TYPE_SHUT_DOWN,
	LED_TYPE_MAX,
}LED_TYPE_DIS_EN;

typedef enum {
	CONN_REASON_RECONNECT = 0,
	CONN_REASON_PAIR,
}PAIR_CONN_REASON_EN;


#define LED_INDEX_MAX INDEX_MAX

typedef struct {
	unsigned char g24_on:1;
	unsigned char bat_on:1;
	
	unsigned char g24_conned:1;
	unsigned char led_conn_next:1;
	unsigned char led_low_bat_on:1;

	unsigned char conn_reason:1;
	unsigned char ble_connected:1;
	
	unsigned char led_all_on:1;
}led_flag_t;

typedef struct {
	unsigned short on_time;
	unsigned short off_time;
	unsigned short repeat_times;
	unsigned int   start_tick;
}led_timeout_t;

unsigned char led_batt_dpi_working_on(void);

void led_g24_conn_status_set(void);
void led_g24_conned_succ_display(void);
void led_low_batt_insert(unsigned char flag);
void led_battery_capacity_display(LED_TYPE_DIS_EN type);
void bat_charging_led_indicate(LED_TYPE_DIS_EN type);


#if (PROJECT_ID == PID_G3750)
	void dpi_led_three_insert(unsigned char times);
	void dpi_led_three_pro(void);
#endif

#endif /* AAA_LED_H_*/

