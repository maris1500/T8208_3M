/********************************************************************************************************
 * @file     AAA_usb_default.h
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

#ifndef _AAA_USB_DEFAULT_H_
#define _AAA_USB_DEFAULT_H_


//////////// product  Information  //////////////////////////////
typedef enum
{ 	
	IDLE,
	USB_DEVICE_CONNECT_PC,
	USB_DEVICE_CHECK_PC_SLEEP,
	USB_DEVICE_DISCONECT_PC,//may be pc power off
	USB_DEVICE_UNPLUG,
}USB_DEVICE_STATUS;

#define	FLOW_NO_OS				    1
#define APPLICATION_DONGLE			1

//#if(APPLICATION_DONGLE)
	#define	USB_PRINTER_ENABLE 		0
    #define	USB_MOUSE_ENABLE 		0
    #define	USB_KEYBOARD_ENABLE 	0
    #define	USB_MIC_ENABLE 			0
	#define	USB_SPEAKER_ENABLE 		0
	#define USB_CDC_ENABLE          0

	#define	USB_SOMATIC_ENABLE      0   //  when USB_SOMATIC_ENABLE, USB_EDP_PRINTER_OUT disable
	#define USB_CUSTOM_HID_REPORT	1
	#define	USB_CUSTOM_HID_REPORT_REG_ACCESS		0

	#define USB_DESCRIPTOR_MY_SELF   1
//#endif

//////////////////// Audio /////////////////////////////////////
#define MIC_RESOLUTION_BIT		16
#define MIC_SAMPLE_RATE			16000//set sample for mic and spk
#define MIC_CHANNLE_COUNT		1
#define	MIC_ENOCDER_ENABLE		0

#define SPK_RESOLUTION_BIT		16

#if(USB_MIC_ENABLE||USB_SPEAKER_ENABLE)
	#define USB_MODE_AUDIO_EN	1
#endif

#define ID_VENDOR		0x248a			// for report

#if(USB_CDC_ENABLE)
	#define ID_PRODUCT		0x8002
#else
	#define ID_PRODUCT		0x8006
#endif

#define  ID_VERDION     0x0100

#ifndef STRING_VENDOR
	#define STRING_VENDOR	L"Telink Semi-conductor Ltd, Co"
#endif

#ifndef STRING_PRODUCT
	#define STRING_PRODUCT	L"Telink No Product"
#endif

#ifndef STRING_SERIAL
	#define STRING_SERIAL	L"USB demo"
#endif

///////////////////  USB   /////////////////////////////////
#ifndef IRQ_USB_PWDN_ENABLE
	#define	IRQ_USB_PWDN_ENABLE  	1
#endif


#ifndef USB_PRINTER_ENABLE
	#define	USB_PRINTER_ENABLE 		0
#endif

#ifndef USB_SPEAKER_ENABLE
	#define	USB_SPEAKER_ENABLE 		0
#endif

#ifndef USB_MIC_ENABLE
	#define	USB_MIC_ENABLE 			0
#endif

#ifndef USB_MOUSE_ENABLE
	#define	USB_MOUSE_ENABLE 		0
#endif

#ifndef USB_KEYBOARD_ENABLE
	#define	USB_KEYBOARD_ENABLE 	0
#endif

#ifndef USB_SOMATIC_ENABLE
	#define	USB_SOMATIC_ENABLE 		0
#endif

#ifndef USB_CUSTOM_HID_REPORT
	#define	USB_CUSTOM_HID_REPORT 	0
#endif

#ifndef USB_AUDIO_441K_ENABLE
	#define USB_AUDIO_441K_ENABLE  	0
#endif

#ifndef USB_MASS_STORAGE_ENABLE
	#define USB_MASS_STORAGE_ENABLE  0
#endif

#ifndef MIC_CHANNLE_COUNT
	#define MIC_CHANNLE_COUNT  		1
#endif

#ifndef USB_DESCRIPTER_CONFIGURATION_FOR_KM_DONGLE
	#define USB_DESCRIPTER_CONFIGURATION_FOR_KM_DONGLE  0
#endif

#ifndef USB_ID_AND_STRING_CUSTOM
	#define USB_ID_AND_STRING_CUSTOM  	0
#endif

#define KEYBOARD_RESENT_MAX_CNT			3
#define KEYBOARD_REPEAT_CHECK_TIME		300000	// in us
#define KEYBOARD_REPEAT_INTERVAL		100000	// in us
#define KEYBOARD_SCAN_INTERVAL			16000	// in us
#define MOUSE_SCAN_INTERVAL				8000	// in us
#define SOMATIC_SCAN_INTERVAL     		8000

#define USB_KEYBOARD_POLL_INTERVAL		10		// in ms	USB_KEYBOARD_POLL_INTERVAL < KEYBOARD_SCAN_INTERVAL to ensure PC no missing key
#define USB_MOUSE_POLL_INTERVAL			4		// in ms
#define USB_SOMATIC_POLL_INTERVAL     	8		// in ms

#define USB_KEYBOARD_RELEASE_TIMEOUT    (450000) // in us
#define USB_MOUSE_RELEASE_TIMEOUT       (200000) // in us
#define USB_SOMATIC_RELEASE_TIMEOUT     (200000) // in us

extern u8 bin_crc[];

#if USB_DESCRIPTOR_MY_SELF
	unsigned short usbmouse_get_report_desc_size(void);
	unsigned char* usbmouse_get_report_desc(void);

	unsigned short usbkb_get_report_desc_size(void);
	unsigned char* usbkb_get_report_desc(void);

	unsigned short usb_desc_get_product_size();
	unsigned short  usb_desc_get_serial_size(void);
	unsigned short  usb_desc_get_vendor_size(void);
	unsigned short  usb_desc_get_configuration_size();

	u8* usb_get_HID_DTYPE_HID(u8 index,u16* g_response_len);
	u8* usb_get_HID_DTYPE_Report(u8 index,u16* g_response_len);

	#define  SPP_REORT_ID 7
	#define  OTA_REPORT_ID 6
	#define  SPP_REORT_LEN 22

	enum CMD_TYPE
	{
		ID_CMD,
		OTA_CMD,
		BREATH_CMD,
		MOUSE_INF_CMD,

		BTN_CFG_CMD,
		DPI_CFG_CMD,
		REPORT_RATE_CFG_CMD,
		DEBUG_REPORT_RATE_CMD,
	};

	typedef struct
	{
		u8 len;
		u8 type;
		u8 buf[SPP_REORT_LEN-2];
	}USB_PC_DEVICE;

	extern USB_PC_DEVICE d_to_p_dat;

	#define FROM_PC_FIFO_NUM  4
	#define USB_FIFO_MAX_LEN  9
	
	typedef struct
	{
		u8 fifo[FROM_PC_FIFO_NUM][23];
		u8 wptr;
		u8 rptr;
	}DEVICE_PC_FIFO_DATA_S;
	
	extern  DEVICE_PC_FIFO_DATA_S  from_pc_dat;
	extern  DEVICE_PC_FIFO_DATA_S  to_pc_dat;
#endif


#endif


