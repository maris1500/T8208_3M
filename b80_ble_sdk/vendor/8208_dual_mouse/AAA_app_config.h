/********************************************************************************************************
 * @file     AAA_app_config.h
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

#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif 

#include "AAA_app_config_Telink_QFN32.h"

///////////////////////// Feature Configuration////////////////////////////////////////////////
/**
 *  @brief  Feature select in BLE Sample project
 */
#define FLASH_SIZE_OPTION_128K			0x20000
#define FLASH_SIZE_OPTION_512K			0x80000

#if	USB_MODE_ENABLE || MCU_QFN40_ENABLE
	#define FLASH_SIZE_OPTION			FLASH_SIZE_OPTION_512K //very important, user need confirm !!!
#else
	#define FLASH_SIZE_OPTION			FLASH_SIZE_OPTION_128K //very important, user need confirm !!!
#endif

#if (FLASH_SIZE_OPTION == FLASH_SIZE_OPTION_128K)
	#define BOOTLOADER_OTA_ENABLE              		1
#elif (FLASH_SIZE_OPTION == FLASH_SIZE_OPTION_512K)
	#define	BLE_OTA_SERVER_ENABLE					1
#endif


#define APP_SECURITY_ENABLE      					1

#define SAVE_RAM_CODE_ENABLE						1 //Enable it if need save RAM code.
													  //Note: When this is enabled, the average power consumption increases.If user use deepsleep retention, user need to add DeepsleepRetentionEarlyWakeupTiming.

/* firmware signature check */
#define FIRMWARES_SIGNATURE_ENABLE     		 		0


//---------------------clock References-------------------------------------------------

#define CLOCK_SYS_CLOCK_HZ  	48000000

#if (CLOCK_SYS_CLOCK_HZ == 16000000)
	#define SYS_CLK_TYPE  							SYS_CLK_16M_Crystal
#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
	#define SYS_CLK_TYPE  							SYS_CLK_24M_Crystal
#elif (CLOCK_SYS_CLOCK_HZ == 32000000)
	#define SYS_CLK_TYPE  							SYS_CLK_32M_Crystal
#elif (CLOCK_SYS_CLOCK_HZ == 48000000)
	#define SYS_CLK_TYPE  							SYS_CLK_48M_Crystal
#else
	#error "unsupported system clock !"
#endif

enum
{
    CLOCK_SYS_CLOCK_1S = CLOCK_SYS_CLOCK_HZ,
    CLOCK_SYS_CLOCK_1MS = (CLOCK_SYS_CLOCK_1S / 1000),
    CLOCK_SYS_CLOCK_1US = (CLOCK_SYS_CLOCK_1S / 1000000),
};

typedef enum
{
   NORMAL_KB_DATA_TYPE=1,
   MOUSE_DATA_TYPE=2,
   CONSUME_DATA_TYPE=3,
   SYSTEM_DATA_TYPE=4,
}USB_DATA_TYPE_ENUM;

/**
 *  @brief  watchdog enable and timeout setting
 */
#define MODULE_WATCHDOG_ENABLE			0
#define MODULE_32K_WATCHDOG_ENABLE		1
#define WATCHDOG_INIT_TIMEOUT			1500  //Unit:ms


//-----------------------------DEEP SAVE FLG -----------------------------
#define USED_DEEP_ANA_REG1				DEEP_ANA_REG1 //u8,can save 8 bit info when deep
#define	PAIR_ANA_FLG					0x01// BIT 0
#define CLEAR_PAIR_ANA_FLAG				0xFE


//-----------------------------DPI SAVE FLG -----------------------------
#define USED_DPI_DEEP_ANA_REG     		DEEP_ANA_REG10 	//u8,can save 8 bit dpi info when deep


//---------------------------security References----------------------------------------
#define BLE_REMOTE_SECURITY_ENABLE		1
#define RF_TX_FIFO_ALLOW_NUM			5


///////////////////////////////////// ATT  HANDLER define ///////////////////////////////////////
#define HID_KEYBOARD_ATT_ENABLE   1
#define HID_MOUSE_ATT_ENABLE	  1

#define REPORT_ID_KEYBOARD_INPUT_AAA        1   //!< Keyboard input report ID
#define REPORT_ID_CONSUME_CONTROL_INPUT_AAA	2   //!< Consumer Control input report ID
#define REPORT_ID_MOUSE_INPUT_AAA           3   //!< Mouse input report ID
#define REPORT_ID_STATUS_INPUT_AAA			4   //!< Device status change input report ID
#define REPORT_ID_GAMEPAD_INPUT_AAA			REPORT_ID_STATUS_INPUT_AAA
#define REPORT_ID_LED_OUT_AAA               0   //!< LED output report ID
#define REPORT_ID_FEATURE_AAA               0   //!< Feature report ID
#define REPORT_ID_USER_FEATURE_AAA			5   //!< Web/driver feature report ID
#define REPORT_ID_DRV_FEATURE_AAA			6   //!< Driver feature report ID
#define REPORT_ID_SYSTEM_INPUT_AAA			REPORT_ID_DRV_FEATURE_AAA
#define REPORT_ID_SPP_INPUT_AAA				7   //!< Gamepad  input report ID
#define HID_WEB_DATA_MAX_LEN                31  //!< Payload bytes after Report ID (wire total=32)
#define HID_WEB_REPORT_WIRE_LEN             32  //!< SET/GET transfer size incl. Report ID (match competitor)
#define HID_STATUS_DATA_LEN                 7   //!< Status input report data length

typedef enum
{
    ATT_H_START = 0,

    /*********Gap*************************************************************************************/
    GenericAccess_PS_H, 					//UUID: 2800, 	VALUE: uuid 1800
    GenericAccess_DeviceName_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read | Notify
    GenericAccess_DeviceName_DP_H,			//UUID: 2A00,   VALUE: device name
    GenericAccess_Appearance_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read
    GenericAccess_Appearance_DP_H,			//UUID: 2A01,	VALUE: appearance
    CONN_PARAM_CD_H,						//UUID: 2803, 	VALUE:  			Prop: Read
    CONN_PARAM_DP_H,						//UUID: 2A04,   VALUE: connParameter

    /*****gatt *****************************************************************************************/
    GenericAttribute_PS_H,					//UUID: 2800,	VALUE: uuid 1801
    //GenericAttribute_ServiceChanged_CD_H,	//UUID: 2803,	VALUE:				Prop: Indicate
    //GenericAttribute_ServiceChanged_DP_H,	//UUID: 2A05,	VALUE: service change
    //GenericAttribute_ServiceChanged_CCB_H,	//UUID: 2902,	VALUE: serviceChangeCCC

    /****device information******************************************************************************************/
    DeviceInformation_PS_H,					//UUID: 2800, 	VALUE: uuid 180A
    DeviceInformation_pnpID_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read
    DeviceInformation_pnpID_DP_H,			//UUID: 2A50,	VALUE: PnPtrs

    /************Ota**********************************************************************************/
#if (BLE_OTA_SERVER_ENABLE||BOOTLOADER_OTA_ENABLE)
	OTA_PS_H,                               //UUID: 2800,   VALUE: telink ota service uuid
	OTA_CMD_OUT_CD_H,                       //UUID: 2803,   VALUE:              Prop: read | write_without_rsp
	OTA_CMD_OUT_DP_H,                       //UUID: telink ota uuid,  VALUE: otaData
	OTA_CMD_INF_CD_H,                       //UUID: 2901,   VALUE: otaName
	OTA_CMD_INF_DP_H,
#endif

    /**************Audio********************************************************************************/
#if (BLE_AUDIO_ENABLE)

    AUDIO_PS_H, 							//UUID: 2800,	VALUE: telink audio service uuid

    //mic
    AUDIO_MIC_INPUT_CD_H,					//UUID: 2803,	VALUE:				Prop: Read | Notify
    AUDIO_MIC_INPUT_DP_H,					//UUID: telink mic uuid,  VALUE: micData
    AUDIO_MIC_INPUT_CCB_H,					//UUID: 2A19	VALUE: micDataCCC
    //AUDIO_MIC_INPUT_DESC_H,					//UUID: 2901,	VALUE: micName

    //speaker
    AUDIO_SPEAKER_OUT_CD_H, 				//UUID: 2803,	VALUE:				Prop: write_without_rsp
    AUDIO_SPEAKER_OUT_DP_H, 				//UUID: telink speaker uuid,  VALUE: speakerData
    //AUDIO_SPEAKEROUT_DESC_H,				//UUID: 2901,	VALUE: speakerName
#endif


    /**********HID************************************************************************************/
    HID_PS_H, 								//UUID: 2800, 	VALUE: uuid 1812


    //HID_INCLUDE_H,							//UUID: 2802,	VALUE: include

    //protocol
    HID_PROTOCOL_MODE_CD_H,					//UUID: 2803, 	VALUE:  			Prop: read | write_without_rsp
    HID_PROTOCOL_MODE_DP_H,					//UUID: 2A4E,	VALUE: protocolMode

#if HID_KEYBOARD_ATT_ENABLE
    HID_BOOT_KB_REPORT_INPUT_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read | Notify
    HID_BOOT_KB_REPORT_INPUT_DP_H,			//UUID: 2A22, 	VALUE: bootKeyInReport
    HID_BOOT_KB_REPORT_INPUT_CCB_H,			//UUID: 2902, 	VALUE: bootKeyInReportCCC


    HID_BOOT_KB_REPORT_OUTPUT_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read | write| write_without_rsp
    HID_BOOT_KB_REPORT_OUTPUT_DP_H,		//UUID: 2A32, 	VALUE: bootKeyOutReport


    HID_CONSUME_REPORT_INPUT_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read | Notify
    HID_CONSUME_REPORT_INPUT_DP_H,			//UUID: 2A4D, 	VALUE: reportConsumerIn
    HID_CONSUME_REPORT_INPUT_CCB_H,			//UUID: 2902, 	VALUE: reportConsumerInCCC
    HID_CONSUME_REPORT_INPUT_REF_H, 		//UUID: 2908    VALUE: REPORT_ID_CONSUMER, TYPE_INPUT


    HID_NORMAL_KB_REPORT_INPUT_CD_H,		//UUID: 2803, 	VALUE:  			Prop: Read | Notify
    HID_NORMAL_KB_REPORT_INPUT_DP_H,		//UUID: 2A4D, 	VALUE: reportKeyIn
    HID_NORMAL_KB_REPORT_INPUT_CCB_H,		//UUID: 2902, 	VALUE: reportKeyInInCCC
    HID_NORMAL_KB_REPORT_INPUT_REF_H, 		//UUID: 2908    VALUE: REPORT_ID_KEYBOARD, TYPE_INPUT

    HID_NORMAL_KB_REPORT_OUTPUT_CD_H,		//UUID: 2803, 	VALUE:  			Prop: Read | write| write_without_rsp
    HID_NORMAL_KB_REPORT_OUTPUT_DP_H,  		//UUID: 2A4D, 	VALUE: reportKeyOut
    HID_NORMAL_KB_REPORT_OUTPUT_REF_H, 		//UUID: 2908    VALUE: REPORT_ID_KEYBOARD, TYPE_OUTPUT
#endif

#if HID_MOUSE_ATT_ENABLE
    HID_BOOT_MOUSE_REPORT_INPUT_CD_H,			//UUID: 2803,	VALUE:				Prop: Read | Notify
    HID_BOOT_MOUSE_REPORT_INPUT_DP_H,			//UUID: 2A22,	VALUE: bootKeyInReport
    HID_BOOT_MOUSE_REPORT_INPUT_CCB_H, 		//UUID: 2902,	VALUE: bootKeyInReportCCC


    HID_MOUSE_REPORT_INPUT_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read | Notify
    HID_MOUSE_REPORT_INPUT_DP_H,			//UUID: 2A4D, 	VALUE: reportConsumerIn
    HID_MOUSE_REPORT_INPUT_CCB_H,			//UUID: 2902, 	VALUE: reportConsumerInCCC
    HID_MOUSE_REPORT_INPUT_REF_H, 		//UUID: 2908    VALUE: REPORT_ID_CONSUMER, TYPE_INPUT
#endif

    HID_USER_REPORT_INPUT_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read | Notify
    HID_USER_REPORT_INPUT_DP_H,			//UUID: 2A4D, 	VALUE: reportConsumerIn
    HID_USER_REPORT_INPUT_CCB_H,			//UUID: 2902, 	VALUE: reportConsumerInCCC
    HID_USER_REPORT_INPUT_REF_H, 		//UUID: 2908    VALUE: REPORT_ID_CONSUMER, TYPE_INPUT

    HID_REPORT_MAP_CD_H,					//UUID: 2803, 	VALUE:  			Prop: Read
    HID_REPORT_MAP_DP_H,					//UUID: 2A4B, 	VALUE: reportKeyIn
    //HID_REPORT_MAP_EXT_REF_H,				//UUID: 2907 	VALUE: extService


    HID_INFORMATION_CD_H,					//UUID: 2803, 	VALUE:  			Prop: read
    HID_INFORMATION_DP_H,					//UUID: 2A4A 	VALUE: hidInformation

    //control point
    HID_CONTROL_POINT_CD_H,					//UUID: 2803, 	VALUE:  			Prop: write_without_rsp
    HID_CONTROL_POINT_DP_H,					//UUID: 2A4C 	VALUE: controlPoint
    //**********HID end********************************************************/

    /*********battery service *************************************************************************************/
    BATT_PS_H,								//UUID: 2800,	VALUE: uuid 180f
    BATT_LEVEL_INPUT_CD_H,					//UUID: 2803,	VALUE:				Prop: Read | Notify
    BATT_LEVEL_INPUT_DP_H,					//UUID: 2A19	VALUE: batVal
    BATT_LEVEL_INPUT_CCB_H, 				//UUID: 2902,	VALUE: batValCCC

    ATT_END_H,

} ATT_HANDLE;


#include "../common/default_config.h"
#include "../common/flash_fw_check.h"


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
