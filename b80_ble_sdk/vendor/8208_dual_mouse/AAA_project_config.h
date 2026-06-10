/********************************************************************************************************
 * @file     AAA_project_config.h
 *
 * @brief    This is the header file for KMD SDK
 *
 * @author	 KMD GROUP
 * @date         01,2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#pragma once
/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif
#include "driver.h"

#include "application/usbstd/usbdesc.h"

#include "application/usbstd/usb.h"
#include "../common/string.h"

#include "AAA_24G_rf_frame.h"
#include "AAA_usb_default.h"
//#include "AAA_Public_KeyValue.h"
#include "AAA_app_config.h"

//#include "../common/ota.h"
#include "../common/flash_fw_check.h"

/**************************************/
#if _CHIP_IS_OTP_  // ECLIPS SET
	#define HW_IS_FLASH     			0
	#define ID_SAVE_STORAGE_ADDR  		0X3E00
	#define CFG_MAC_ADDR				0X3F00
	#define	PARING_MAX_NUM				(256/ID_LEN)
	#define EMI_TEST_FUN_ENABLE_AAA		1	//EMI Enable
	#define OTA_ENABLE_AAA				0 	//USB OTA enable
#else
    #define HW_IS_FLASH     			1
	#define ID_SAVE_STORAGE_ADDR  		0X1A000
	#define CFG_MAC_ADDR				0X1F000
	#define	PARING_MAX_NUM				(4096/ID_LEN)
//    #define EMI_TEST_FUN_ENABLE_AAA		1	//EMI Enable
    #define OTA_ENABLE_AAA				0 	//USB OTA enable
#endif

#ifndef OTA_ENABLE_AAA
    #define OTA_ENABLE_AAA              0
#endif

#define CHECK_DEVICE_ID_ENABLE			1 //Check whether the ID of the packet is the same as that bound locally
#define KM_DATA_HANDLE_CALL_BACK_ENABLE	0 //0-Processing in interrupt 1-Processing in main loop
#define GOLD_TEST_DONGLE				0

#define AES_METHOD						0
#define DATA_3_CHOOSE_1_ENABLE			0


#if (AES_METHOD == 1 && DATA_3_CHOOSE_1_ENABLE == 1)
    #error Not support AES and DATA_3_CHOOSE_1 work at the same time
#endif

#define DEFAULT_NORMAL_TX_POWER   		RF_POWER_P8p99dBm 
#define DEFAULT_EMI_TX_POWER			RF_POWER_P0p59dBm

/**************************************/
#define abs(a)   (((a)>0)?((a)):(-(a)))

#define U32_MAX ((u32)0xffffffff)
#define U16_MAX ((u16)0xffff)
#define U8_MAX ((u8)0xff)
#define U31_MAX ((u32)0x7fffffff)
#define U15_MAX ((u16)0x7fff)
#define U7_MAX ((u8)0x7f)
#define abs_aaa(a)   (((a)>0)?((a)):(-(a)))

//#define   SAVE_MULIT_MOUSE_KB_ID_COUNT   1  //max 8

#define  PAIR_CHECK_RSSI_ENABLE_AAA			1	//RSSIl filter switch in pairing mode
#define  GLOD_DONGLE_RSSI_ENABLE_AAA		1
#define  GET_RX_RSSI(P)  (P[P[0]+2])
/***************rf ll************/
#define	CHANNEL_HOPPING_ALWAYS				1

#define		PKT_BUFF_SIZE		80

#define		FH_CHANNEL_PKT_RCVD_MAX			8
#define		LL_CHANNEL_SYNC_TH				2
#define		LL_CHANNEL_SEARCH_TH			60
#define		LL_CHANNEL_SEARCH_FLAG			BIT(16)

#define		LL_NEXT_CHANNEL(c)				((c + 6) & 14)


#define  FRQ_HOPPING_MODE_SWITCH_ENABLE    	1

#define  FRQ_HOPPING_MODE_NORMAL   		0  //normal hopping
#define  FRQ_HOPPING_MODE_SLOW     		1  //slow hopping for kb only

#define  PKT_RCVD_FLG_MOUSE_LOCAL     	0x01
#define  PKT_RCVD_FLG_MOUSE_GLOBAL    	0x10
#define  PKT_RCVD_FLG_MOUSE				(PKT_RCVD_FLG_MOUSE_LOCAL | PKT_RCVD_FLG_MOUSE_GLOBAL)
#define  PKT_RCVD_FLG_KB_LOCAL       	0x02
#define  PKT_RCVD_FLG_KB_GLOBAL      	0x20
#define  PKT_RCVD_FLG_KB                (PKT_RCVD_FLG_KB_LOCAL | PKT_RCVD_FLG_KB_GLOBAL)

#define  PKT_RCVD_FLG_MOUSE_KB_LOCAL	(PKT_RCVD_FLG_MOUSE_LOCAL | PKT_RCVD_FLG_KB_LOCAL)

#define  MOUSE_PKT_NEVER_COME   (!(frq_hopping_data.device_pktRcv_flg & PKT_RCVD_FLG_MOUSE_GLOBAL))
#define  MOUSE_PKT_COME           (frq_hopping_data.device_pktRcv_flg & PKT_RCVD_FLG_MOUSE_LOCAL)
#define  KB_PKT_COME           	  (frq_hopping_data.device_pktRcv_flg & PKT_RCVD_FLG_KB_LOCAL)
#define  MOUSE_PKT_5S_NO_COME	  (clock_time_exceed(frq_hopping_data.mouse_pktRcv_sysTick,5000000))

#if OTA_ENABLE_AAA
	typedef struct
	{
	    u32 bin_crc;
	    u32 fw_version;
	} OUTPUT_DEV_INFO_AAA;
	extern OUTPUT_DEV_INFO_AAA output_dev_info;
#endif

typedef enum
{
	SUCCESS_OTA=0,
	BIN_CRC_ERROR_OTA=1,
	DATA_CRC_ERROR_OTA=2,
	FW_SIZE_ERROR_OTA=3,
	LOST_DATA_OTA=4,
}OTA_ERROR;

#define USB_OTA_LENGTH  64

typedef struct{
	u8	report_id;
	u8 	opcode;
	u16	length;					
	u8	dat[60];
}usb_data_t;

typedef struct
{
	u16 cmd;
	u8 buf[16];
	u16 crc;
}ota_data_st;


typedef struct{
	u8 frq_hp_mode;
	u8 frq_hp_chn_pkt_rcvd_max;
	u8 frq_hp_hit_diff_num;
	u8 device_pktRcv_flg;

	u32 fre_hp_always_time_us;
	u32 mouse_pktRcv_sysTick;
}frqHopping_data_t;
extern frqHopping_data_t  frq_hopping_data;
/***********************************/
#define CONFIG_GOLDEN_DONGLE_TEST	0	//1 is used for golden dongle

typedef enum
{
   MANUAL_PARING_TYPE=0Xfb,
   AUTO_LIMIT_TIME_PARING_TYPE=0xa7,
   GOLDEN_DONGLE_PARING_TYPE=0x6f,
}PAIRING_TYPE_AAA;

#if 0
typedef struct{ //The length has to be a multiple of 4!!!
	u32  mac_addr;		  	  // 00~0x03 device id

	u32  manufacturer_addr;	  // 04~0x07

	u16  vendor_id;		// 08-09
	u16  prodct_id;		// 0A-0B

	u8	 rssi_threshold;	// 0C
	u8   paring_limit_t;	// 0D manual_paring_time_set
	u8	 paring_type;		// 0E paring_type
	u8	 support_type;		// 0F

	u8   tx_power_emi;		// 10
	u8   access_code_type;	// 11 0xff: MAC is converted to access code, 1: MAC is used directly as access code
	u16  rf_id;    //12-13

#if (AES_METHOD==1)
	u8  encKey[16];     //14-23 aes128 key 16bytes
#endif
} custom_cfg_t;
extern custom_cfg_t user_config;
#endif

/***********************************/
#if 1
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
    EMI_TEST_OVER_AAA = 9,
    OTA_STOP_AAA = 10,
	FACTORY_RESET_AAA = 11,
} ANA_STATUS_AAA;
#endif

typedef enum
{
	SUPPORT_KEYBOARD_DEVICE = 0,
}SUPPORT_DEVIE_TYPE;

#if USB_MODE_ENABLE

	#define USB_FIFO_NUM      4
	#undef  USB_FIFO_MAX_LEN
	#define USB_FIFO_MAX_LEN  9

	typedef struct
	{
		u8 fifo[USB_FIFO_NUM][USB_FIFO_MAX_LEN];
		u8 wptr;
		u8 rptr;
	}USB_FIFO_DATA_S;
	extern USB_FIFO_DATA_S  usb_fifo_aaa;

	typedef struct
	{
		u8 type;
		u8 buf[USB_FIFO_MAX_LEN-1];
	}USB_DATA_S;

#endif

#define RF_RX_FIFO_NUM   32
#define RF_RX_FIFO_MAX_LEN  64

typedef struct
{
	u8 fifo[RF_RX_FIFO_NUM][RF_RX_FIFO_MAX_LEN];
	u8 wptr;
	u8 rptr;
}RF_RX_FIFO_DATA_S;

#define MAX_STRING_LEN  22

typedef struct
{
	u8 Size;
	u8 Type;
	wchar_t str[MAX_STRING_LEN];
}KM_USB_Descriptor_String_t;

typedef struct{
	u8 sensor_inf;//bit0~4=sensor_type|bit5~it7=sensor_dpi
	u8 report_rate;
	u8 bat;
}t_mouse_inf;

#if Dongle_Lark_A1_Flash_Enable
	extern unsigned char gc_lark_flash_pair;
#endif

extern u8 report_rate;

// extern t_mouse_inf mouse_inf;
// extern t_mouse_inf last_mouse_inf;
extern u8 usb_device_status;

extern u8 deep_flag;
//extern u32 tick_pc_breath;
extern u32 rx_tick;

extern  u8 usb_mouse_report_proto;
extern unsigned char usb_g_config;

extern  u8 host_keyboard_status;
extern  u32 resume_host_tick;
//extern  u8 allow_send_to_usb_flag;
extern  u8 need_enter_suspend_flag;
extern  u8 need_wake_up_host_flag;
extern  u32 tick_resume_host;

// extern u8 emi_flg;

extern u32 ota_program_offset;
extern u32 ota_program_bootAddr;

extern u8 run_app_code();
void rf_channel_poll();

extern  void irq_host_timer1();
extern  void rf_rx_irq_handle();
extern  void rf_tx_irq_handle();
extern void custom_init();
int usb_keyboard_hid_report_aaa(unsigned char *data);
int usb_mouse_hid_report_aaa(u8 report_id,unsigned char * p,u8 len);
#if USB_DESCRIPTOR_MY_SELF
void app_hid_set_report_handle(u8 data_request, u8 report_id, u16 length);
u16 usb_hid_fill_get_report(u16 wValue, u8 *buf, u16 buf_size);
void usb_hid_store_feature_report(u8 report_id, u8 *data, u8 len);
void usb_ep0_out_setup(u16 length);
void usb_ep0_out_update(u16 remaining);
void usb_hid_web_rx_reset(void);
void usb_hid_web_ep0_reset(void);
void usb_fifo_reset_aaa(void);
void usb_mouse_eps_reack(void);
void usb_data_eps_ready_on_config(void);
void usb_web_intf_eps_ready(void);
void usb_web_log_reply(u16 wValue, const u8 *buf, u16 len, const char *tag);
void usb_web_handshake_reset(void);
void usb_web_handshake_done(void);
void usb_web_grace_restart(void);
#endif
void usb_power_down_handle();
void usb_check_allow_send_to_usb();
void  usb_resume_host(void);
//void custom_set_usb_cfg_params(custom_cfg_t *p);
void custom_set_usb_cfg_params();
void user_resume_host();
void usb_host_cmd_proc(void);
void generateRandomNum(int len, unsigned char *data);

u8 spp_pull_from_pc();
u8 spp_pull_to_pc();

int usb_spp_to_pc(u8 *buf);
u8 spp_push_data(USB_PC_DEVICE *src,DEVICE_PC_FIFO_DATA_S *des );

extern void get_usb_data_report_aaa();

extern u8 kb_data_handle(u8 *buf);
u8 adaptive_smoother();
//void usb_user_init(void);
u8  special_btn_process(u8 btn);
/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
