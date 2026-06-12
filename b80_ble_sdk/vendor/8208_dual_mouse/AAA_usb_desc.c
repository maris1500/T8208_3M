/********************************************************************************************************
 * @file     AAA_usb_desc.c
 *
 * @brief    This is the source file for KMD SDK
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

#include "AAA_project_config.h"

#if USB_MODE_ENABLE


#if USB_DESCRIPTOR_MY_SELF

static const u8 mouse_report_desc[] = 
{
/*******************mouse report=66************************/
	0x05, 0x01,  // Usage Page (Generic Desktop)
	0x09, 0x02,  // Usage (Mouse)
	0xA1, 0x01,  // Collection (Application)
	0x85, 0X01,  // Report Id
	0x09, 0x01,  //   Usage (Pointer)
	0xA1, 0x00,  //   Collection (Physical)
	0x05, 0x09,  //  Usage Page (Buttons)
	0x19, 0x01,  //  Usage Minimum (01) - Button 1
	0x29, 0x05,  //  Usage Maximum (03) - Button 3
	0x15, 0x00,  //  Logical Minimum (0)
	0x25, 0x01,  //  Logical Maximum (1)
	0x75, 0x01,  //  Report Size (1)
	0x95, 0x05,  //  Report Count (3)
	0x81, 0x02,  //  Input (Data, Variable, Absolute) - Button states
	0x75, 0x03,  //  Report Size (5)
	0x95, 0x01,  //  Report Count (1)
	0x81, 0x01,  //  Input (Constant) - Padding or Reserved bits

	0x05, 0x01,  //  Usage Page (Generic Desktop Control)
	0x09, 0x30,  // Usage (X)
	0x09, 0x31,  // Usage (Y)

#if (MOUSE_DATA_LEN_AAA==6) || (MOUSE_DATA_LEN_AAA == 7)

	0x16, 0x01, 0x80, //  LOGICAL_MINIMUM(0)
	0x26, 0xff, 0x7f,
	0x75, 0x10, //	Report Size (16)
	0x95, 0x02, //	Report Count (2)
#else
    0x15, 0x81, //  LOGICAL_MINIMUM(0)
    0x25, 0x7f,
    0x75, 0x08, //  Report Size (8)
    0x95, 0x02, //  Report Count (2)
#endif


	0x81, 0x06, //	Input (Data, Variable, Relative)

	//0x05,0x01,		 //  Usage Page (Generic Desktop Control)
	0x09, 0x38, 		 //  Usage (Wheel)
	0x15, 0x81, 		 //  Logical Minimum (-4)
	0x25, 0x7F, 		 //  Logical Maximum (3)
	0x75, 0x08, 		 //  Report Size (3)
	0x95, 0x01, 		 //  Report Count (1)
	0x81, 0x06, 		 //  Input (Data, Variable, Relative)

#if (WHEEL_ROLLER_ENABLE)
	0x95, 0x01,
	0x05, 0x0c,
	0x0a, 0x38, 0x02,
	0x81, 0x06,  //  Input (Data, Variable, Relative)
#endif

	0xC0,		  //   End Collection
	0xC0,		  // End Collection

/*******************consume report=25************************/
	0x05, 0x0C,   // Usage Page (Consumer)
	0x09, 0x01,   // Usage (Consumer Control)
	0xA1, 0x01,   // Collection (Application)
	0x85, 0x02,   // Report Id
	0x75, 0x10,   // global, report size 16 bits
	0x95, 0x01,   // global, report count 1
	0x15, 0x01,   // global, min  0x01
	0x26, 0x8c, 0x02, // global, max  0x28c
	0x19, 0x01, 	  // local, min   0x01
	0x2a, 0x8c, 0x02, // local, max	  0x28c
	0x81, 0x00, 	  // main,  input data varible, absolute
	0xc0,		      // main, end collection

/*******************system power report=33************************/
	0x05, 0x01, 	//gobal,  USAGE_PAGE 1 (Generic Desktop)
	0x09, 0x80, 	//local, usage ID 0x80 system control
	0xa1, 0x01, 	//main conllection
	0x85, 0x03, 	//global report ID 0x4
	
	0x09, 0x82, 	//local usage ID 0x82 system sleep
	0x09, 0x81, 	//local usage ID 0x81 system power down
	0x09, 0x83, 	//local usage ID 0x83 system wakeup

	0x15, 0x00, 	//global min 01
	0x25, 0x01, 	//gobal, max 3
	0x19, 0x01,     //Usage Minimum (01) - Button 1
	0x29, 0x03,     //Usage Maximum (03) - Button 3
	
	0x75,0x01,	   //global, report size 2
	0x95,0x03,	   //report count  1	
	0x81,0x02,	   //main, input data, var, abs, No Prefer, NULL state
	//0x75,0x03,   //Report Size (5)
	0x95,0x05,	   //Report Count (1)
	0x81,0x01,	   //Input (Constant) - Padding or Reserved bits

	0xc0,		  //end of collection
	
#if 0//HID_KB_ALL_KEY_ATT_ENABLE
/*******************user define 26key************************/
  //keyboard report in
   0x05, 0x01,  // Usage Pg (Generic Desktop)
   0x09, 0x06,  // Usage (Keyboard)
   0xA1, 0x01,  // Collection: (Application)
   0x85, SPP_REORT_ID,//REPORT_ID_ALL_KEY_INPUT_AAA,  // Report Id (keyboard)

   0x05, 0x07,  // Usage Pg (Key Codes)
   0x19, 0xE0,  // Usage Min (224)  VK_CTRL:0xe0
   0x29, 0xE7,  // Usage Max (231)  VK_RWIN:0xe7
   0x15, 0x00,  // Log Min (0)
   0x25, 0x01,  // Log Max (1)
   //
   // Modifier byte
   0x75, 0x01,  // Report Size (1) 1 bit * 8
   0x95, 0x08,  // Report Count (8)
   0x81, 0x02,  // Input: (Data, Variable, Absolute)

   // Key arrays (6 bytes)
   0x15, 0x00,  // Log Min (0)
   0x25, 0x01,  // Log Max (241)
   0x05, 0x07,  // Usage Pg (Key Codes)
   0x19, 0x00,  // Usage Min (0)
   0x2A, 0xFF,0x00, // Usage Max (241)

   0x75, 0x01,  // Report Size (8)
   0x95, 152,  // Report Count (6)
   0x81, 0x02,  // Input: (Data, Array)

   0xC0,    // End Collection
#endif

#if 1
/*******************vendor define feature report ID 5************************/
	0x06,  0x00,  0xff, 	// Usage Page (Vendor-defined),
	0x09,  0x00,	// USAGE ()
	0xa1,  0x01,	// Collection (Application),
	0x85,  0x05,	// Report ID (5)
	0x09,  0x00,	// Usage,
	0x15,  0x00,	// Logical Minimum (0),
	0x26,  0xff,  0x00,		// Logical Maximum (255),
	0x75,  0x08,	// Report Size (8),
#if WEB_HID_ENABLE
	0x95,  0x1F,
#else
	0x95,  0x07,	// Report Count (7),
#endif
	0xB1,  0x02,   	// Feature (Data, Variable, Absolute)
	0xc0,	// END_COLLECTION
	/*******************vendor define input report ID 4************************/
	0x06,  0x00,  0xff, 	// Usage Page (Vendor-defined)
	0x09,  0x00,	// USAGE ()
	0xa1,  0x01,	// Collection (Application)
	0x85,  0x04,	// Report ID (4)
	0x09,  0x01,	// Usage,
	0x15,  0x00,	// Logical Minimum (0),
	0x26,  0xff,  0x00,		// Logical Maximum (255),
	0x75,  0x08,	// Report Size (8),
#if WEB_HID_ENABLE
	0x95,  0x1F,
#else
	0x95,  0x07,	// Report Count (7),
#endif

	0x81,  0x02,	// Input
	0xc0,           // END_COLLECTION
	/*******************vendor define feature report ID 6************************/
	0x06,  0x00,  0xff, 	// Usage Page (Vendor-defined)
	0x09,  0x00,	 // USAGE ()
	0xa1,  0x01,	// Collection (Application)
	0x85,  0x06,	// Report ID (6)
	0x09,  0x01,	// Usage,
	0x15,  0x00,	// Logical Minimum (0),
	0x26,  0xff,  0x00,	  // Logical Maximum (255),
	0x75,  0x08,	      // Report Size (8),
#if WEB_HID_ENABLE
	0x95,  0x1F,
#else
	0x95,  0x07,	// Report Count (7),
#endif

	0xB1,  0x02,	// Feature
	0xc0,	        // END_COLLECTION
#else
/*******************user define input************************/
	0x05,0x01,	   //global,  USAGE_PAGE 1 (Generic Desktop)
	0x09,0x00,	   //usage undefined
	0xa1,0x01,	   //main collection
	0x85,CUSTOM_OUTPUT_REPORT_ID,	   //global report ID 0x5
	0x06,0x00,0xff,  //global usage page
	0x09,0x01,	   //local,  usage ID 01  Consumer Control
	0x15,0x81,	   //global min 81
	0x25,0x7f,	   //global, max 7f
	0x75,0x08,	   //global, report size 8
	0x95,0x07,	   //report count  7
	0xb1,0x02,	   //feature (data, var, abs)
	0xc0,		  //main, end collection
	
	/*******************user define output************************/
	#if 0
		0x05,0x01,	   //global,  USAGE_PAGE 1 (Generic Desktop)
		0x09,0x00,	   //usage undefined
		0xa1,0x01,	   //main collection
		0x85,CUSTOM_INPUT_REPORT_ID,	   //global report ID 0x5
		0x06,0x01,0xff,  //global usage page
		0x09,0x01,	   //local,  usage ID 01  Consumer Control
		0x15,0x81,	   //global min 81
		0x25,0x7f,	   //global, max 7f
		0x75,0x08,	   //global, report size 8
		0x95,0x07,	   //report count  7
		0xb1,0x02,	   //feature (data, var, abs)
		0xc0,		  //main, end collection
	#endif
#endif

};

static const u8 kb_report_desc[] = 
{
#if 1
//keyboard report in
	0x05, 0x01,	 // Usage Pg (Generic Desktop)
	0x09, 0x06,	 // Usage (Keyboard)
	0xA1, 0x01,	 // Collection: (Application)
	//0x85, 0x01,//REPORT_ID_KEYBOARD_INPUT_AAA,	 // Report Id (keyboard)
			     //
	0x05, 0x07,	 // Usage Pg (Key Codes)
	0x19, 0xE0,	 // Usage Min (224)  VK_CTRL:0xe0
	0x29, 0xE7,	 // Usage Max (231)  VK_RWIN:0xe7
	0x15, 0x00,	 // Log Min (0)
	0x25, 0x01,	 // Log Max (1)
			     //
			     // Modifier byte
	0x75, 0x01,	 // Report Size (1)   1 bit * 8
	0x95, 0x08,	 // Report Count (8)
	0x81, 0x02,	 // Input: (Data, Variable, Absolute)
			     //
			     // Reserved byte
	0x95, 0x01,	 // Report Count (1)
	0x75, 0x08,	 // Report Size (8)
	0x81, 0x01,	 // Input: (Constant)

//keyboard output
	//5 bit led ctrl: NumLock CapsLock ScrollLock Compose kana
	0x95, 0x05,	//Report Count (5)
	0x75, 0x01,	//Report Size (1)
	0x05, 0x08,	//Usage Pg (LEDs )
	0x19, 0x01,	//Usage Min
	0x29, 0x05,	//Usage Max
	0x91, 0x02,	//Output (Data, Variable, Absolute)
	//3 bit reserved
	0x95, 0x01,	//Report Count (1)
	0x75, 0x03,	//Report Size (3)
	0x91, 0x01,	//Output (Constant)

	// Key arrays (6 bytes)
	0x95, 0x06,	 // Report Count (6)
	0x75, 0x08,	 // Report Size (8)
	0x15, 0x00,	 // Log Min (0)
	0x26, 0xF1,0x00,	 // Log Max (241)
	0x05, 0x07,	 // Usage Pg (Key Codes)
	0x19, 0x00,	 // Usage Min (0)
	0x2a, 0xf1,0x00,	 // Usage Max (241)
	0x81, 0x00,	 // Input: (Data, Array)

	0xC0,			  // End Collection
#endif
};

#if OTA_ENABLE_AAA
static const u8 spp_report_desc[] = 
{
	/*******************ota and ************************/
	0x06,  0xEF,  0xff,  //global usage page
	0x09,  0x00,	     //usage undefined
	0xa1,  0x01,	     //main collection
	0x85,  OTA_REPORT_ID, //global report ID 0x6
	0x15,  0x00,          //LOGICAL_MINIMUM (0)
	0x26,  0xff,  0x00,   //LOGICAL_MAXIMUM (255)
	0x95,  (USB_OTA_LENGTH-1),	//Report Count (32) //MTU_SIZE=23
	0x75,  0x08,  //Report Size (8)
	0x09,  0x01,
	0x81,  0x02,  //INPUT (Data,Var,Abs)
	0x09,  0x02,
	0x91,  0x02,  //Output (Data, Variable, Absolute)
	0x09,  0x03,
	0xB1,  0x02,  //feature (Data, Variable, Absolute)
	0xc0,	 //main, end collection
};
#endif

#if WEB_HID_ENABLE
/* intf1: status Input Report ID 4 only (32B wire); Web EP0 stays on intf0 mouse desc */
static const u8 vendor_status_in_report_desc[] = {
	0x06, 0x00, 0xff,
	0x09, 0x00,
	0xa1, 0x01,
	0x85, REPORT_ID_GAMEPAD_INPUT_AAA,
	0x09, 0x01,
	0x15, 0x00,
	0x26, 0xff, 0x00,
	0x75, 0x08,
	0x95, HID_WEB_DATA_MAX_LEN,
	0x81, 0x02,
	0xc0,
};
#endif

u8 device_desc_km[18] = 
{
    0x12,  		// sizeof(USB_Descriptor_Device_t)
    0x01,  		// Type = DTYPE_Device
    0x10,0x01,	// 0x0110 : USBSpecification, USB 1.1
    0x00,       // Class : USB_CSCP_NoDeviceClass
    0x00, 		// SubClass : USB_CSCP_NoDeviceSubclass
    0x00, 		// Protocol : USB_CSCP_NoDeviceProtocol
    0x08,       // Endpoint0Size, Maximum Packet Size for Zero Endpoint. Valid Sizes are 8, 16, 32, 64
#if 0//_CHIP_IS_OTP_
    0x8A,0x24, 	// VendorID 0x248a   8  9
    0x08,0x82, 	// ProductID defult value   mouse only:2781  mouse_keyboard_kit:2782
#elif 0
    0xAC,0x05, 	// VendorID 0x248a   8  9
    0x4F,0x02, 	// ProductID defult value   mouse only:2781  mouse_keyboard_kit:2782
#elif 0
    0x58,0x04, 	// VendorID 0x248a   8  9
    0x9D,0x01, 	// ProductID defult value   mouse only:2781  mouse_keyboard_kit:2782
#elif 0
    0x8A,0x24, 	// VendorID 0x248a   8  9
    0x73,0x83, 	// ProductID defult value   mouse only:2781  mouse_keyboard_kit:2782
#else
	0xF3, 0x04,
	0x6E, 0x02,
#endif
    (u8)FW_VERSION,(u8)(FW_VERSION>>8),	// 0x0100 : ReleaseNumber
    0x01, 		// ManufacturerStrIndex = USB_STRING_VENDOR   @@@ pay attention
    0x02,		// ProductStrIndex = USB_STRING_PRODUCT		  @@@ pay attention
    0x00, 		// SerialNumStrIndex=USB_STRING_SERIAL
    0x01         // NumberOfConfigurations
};

#if OTA_ENABLE_AAA
	u8 km_cfg_desc[9+9+9+7+9+9+7+9+9+7]=		//have spp
#elif WEB_HID_ENABLE
	u8 km_cfg_desc[9+9+9+7+9+9+7+9+9+7]=		//mouse + vendor web + keyboard
#else
	u8 km_cfg_desc[9+9+9+7+9+9+7]=
#endif
{
//USB_Descriptor_Configuration_Header_t
	0x09,			//sizeof(USB_Descriptor_Configuration_Header_t)
	0x02,  			//type= DTYPE_Configuration
	sizeof(km_cfg_desc),0x00,     //TotalConfigurationSize,  @@@@ will change @@@@
#if OTA_ENABLE_AAA
	0x03,			//TotalInterfaces
#elif WEB_HID_ENABLE
	0x03,
#else
	0x02,
#endif
	0x01,      //Configuration index
	0x00,      //ConfigurationStrIndex = NO_DESCRIPTOR
	0xa0,      //ConfigAttributes = USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_REMOTEWAKEUP
	0x32,      //MaxPowerConsumption = USB_CONFIG_POWER_MA(50)  MaxPower = 100mA

//--------------0 Interface_t Descriptor--------------------------------------
	0x09, 	//size = sizeof(USB_Descriptor_Interface_t)
	0x04, 	//type = DTYPE_Interface
	0x00,  	//InterfaceNumber
	0x00,   //AlternateSetting = 0
	0x01,   //TotalEndpoints = 1
	0x03, 	//Class = HID_CSCP_HIDClass
	0x01, 	//SubClass = HID_CSCP_BootSubclass,
	0x02,	//Protocol = HID_CSCP_MouseBootProtocol
	0,      //InterfaceStrIndex = NO_DESCRIPTOR
//----------------HID_Descriptor---------------------------------------------
	0x09,   //size = sizeof(USB_HID_Descriptor_HID_t)
	0x21,   //type = HID_DTYPE_HID
	0x11,0x01,		 //HIDSpec = 0x0111
	0x21,			 //CountryCode = USB_HID_COUNTRY_US
	0x01,		     //TotalReportDescriptors
	0x22,			 //HIDReportType = HID_DTYPE_Report
	sizeof(mouse_report_desc), (u8)(sizeof(mouse_report_desc)>>8),	  //HIDReportLength[2] = {sizeof(mouse_report_desc), 0x00}

//------------------Endpoint Description-----------------
	0x07,			//size = sizeof(USB_Descriptor_Endpoint_t)
	0x05,			//type = DTYPE_Endpoint
	0x80| USB_EDP_MOUSE, //EndpointAddress = ENDPOINT_DIR_IN | USB_EDP_MOUSE,
	0x03,			//Attributes = EP_TYPE_INTERRUPT
	0x08,0x00,		//EndpointSize = 0x0008
	0x01,			//PollingIntervalMS = USB_MOUSE_POLL_INTERVAL

#if WEB_HID_ENABLE
//--------------1 Interface: vendor Web status IN (EP4/0x84) ------------------
	0x09, 0x04, 0x01, 0x00, 0x01, 0x03, 0x00, 0x00, 0,
	0x09, 0x21, 0x11, 0x01, 0x21, 0x01, 0x22,
	(u8)sizeof(vendor_status_in_report_desc), (u8)(sizeof(vendor_status_in_report_desc)>>8),
	0x07, 0x05, 0x80 | USB_EDP_WEB_IN, 0x03, 0x40, 0x00, 0x0a,
#endif

#if OTA_ENABLE_AAA
//--------------1 Interface_t Descriptor--------------------------------------
	0x09,			 //size = sizeof(USB_Descriptor_Interface_t)
	0x04,			 //type = DTYPE_Interface
	0x02,			 //InterfaceNumber			@@@@ will change @@@@
	0,				 //AlternateSetting = 0
	1,				 //TotalEndpoints = 1
	0x03,			 //Class = HID_CSCP_HIDClass
	0x00,			 //SubClass = HID_CSCP_BootSubclass,
	0x00,			 //Protocol = HID_CSCP_KeyboardBootProtocol,
	0,				 //InterfaceStrIndex = NO_DESCRIPTOR
//----------------HID_Descriptor---------------------------------------------
	0x09,			 //size = sizeof(USB_HID_Descriptor_HID_t)
	0x21,			 //type = HID_DTYPE_HID
	0x11, 0x01, 	 // HIDSpec = 0x0111
	0x21,			 //CountryCode = USB_HID_COUNTRY_US
	1,				 //TotalReportDescriptors
	0x22,			 //HIDReportType = HID_DTYPE_Report
	sizeof(spp_report_desc), 0x00,		//Report size
//------------------Endpoint Description-----------------
	0x07,			 //size = sizeof(USB_Descriptor_Endpoint_t)
	0x05,			 //type = DTYPE_Endpoint
	0x80 | USB_EDP_SPP_IN, //EndpointAddress = ENDPOINT_DIR_IN | USB_EDP_KEYBOARD_IN,
	0x03,			 //Attributes = EP_TYPE_INTERRUPT
	0x40,0x00,		 //EndpointSize = 0x0008
	0x0a,			 //PollingIntervalMS = USB_KEYBOARD_POLL_INTERVAL = 10
#endif

//USB_Descriptor_Interface_t keyboardInterface
	0x09, 			 //size = sizeof(USB_Descriptor_Interface_t)
	0x04, 	 		 //type = DTYPE_Interface
#if WEB_HID_ENABLE
	2,				//InterfaceNumber
#else
	1,				//InterfaceNumber
#endif
	0,     			 //AlternateSetting = 0
	1,  			 //TotalEndpoints = 1
	0x03,			 //Class = HID_CSCP_HIDClass
	0x01,            //SubClass = HID_CSCP_BootSubclass,
	0x01,            //Protocol = HID_CSCP_KeyboardBootProtocol,
	0,           	 //InterfaceStrIndex = NO_DESCRIPTOR
//USB_HID_Descriptor_HID_t   keyboard_hid
	0x09,     		 //size = sizeof(USB_HID_Descriptor_HID_t)
	0x21,            //type = HID_DTYPE_HID
	0x11, 0x01, 	 // HIDSpec = 0x0111
	0x21, 			 //CountryCode = USB_HID_COUNTRY_US
	1, 		  		 //TotalReportDescriptors
	0x22, 			 //HIDReportType = HID_DTYPE_Report
	sizeof(kb_report_desc), (u8)(sizeof(kb_report_desc)>>8),      //HIDReportLength[2] = {sizeof(keyboard_report_desc), 0x00}
//USB_Descriptor_Endpoint_t  keyboard_in_endpoint
	0x07, 			 //size = sizeof(USB_Descriptor_Endpoint_t)
	0x05, 	    	 //type = DTYPE_Endpoint
	ENDPOINT_DIR_IN | USB_EDP_KEYBOARD_IN, //EndpointAddress = ENDPOINT_DIR_IN | USB_EDP_KEYBOARD_IN,
	0x03,    		 //Attributes = EP_TYPE_INTERRUPT
	0x08,0x00,		 //EndpointSize = 0x0008
	0x04,   		 //PollingIntervalMS = USB_KEYBOARD_POLL_INTERVAL = 10
};

KM_USB_Descriptor_String_t  km_vendor_desc = {
		14,               //sizeof(L"Telink")=14
		0x03,			  //DTYPE_String=0x03
		L"Telink"
};

KM_USB_Descriptor_String_t  km_product_desc = {
		20,               //sizeof(L"Wireless Receiver")=36
		0x03,			  //DTYPE_String=0x03
		L"USB Mouse"
};

KM_USB_Descriptor_String_t  km_serial_desc = {
		18,               //sizeof(L"TLSR8566")=18
		0x03,			  //DTYPE_String=0x03
		L"TLSR8278"
};

const USB_Descriptor_String_t km_language_desc = { {
		sizeof(USB_Descriptor_Hdr_t) + 2, DTYPE_String },
		{ LANGUAGE_ID_ENG } };

/**
 * @brief	set_string_from_addr
 * @param	none
 * @return	none
 */
void set_string_from_addr(KM_USB_Descriptor_String_t *ptr, u32 addr)
{
	u8 *p;

#if(CUSTOM_DATA_ERR_FIX_ENABLE)
	for(int i=0;i<CUSTOM_DATA_MAX_COUNT;i++){
		p = (u8 *)(addr+(i<<5));
		if(*p != 0){
			break;
		}
	}
#else
	p = (u8 *)(addr);
#endif

	int j = 0;
	if(*p != 0xff){
		while(*p != 0xff){
			ptr->str[j++] = *p++;
		}
		ptr->str[j] = 0;
		ptr->Size = (j+1)<<1;
	}

	/*
	u8 len;
	u8 *p = (u8 *)(addr);
	if(*p != U8_MAX){
		len = (*p++)%MAX_STRING_LEN;
		ptr->Size = (len+1)<<1;
		for(int i=0;i<len;i++){
			ptr->str[i] = *p++;
		}
		ptr->str[len] = 0;
	}
	*/
}

/**
 * @brief	custom_set_usb_cfg_params
 * @param	none
 * @return	none
 */
//void custom_set_usb_cfg_params(custom_cfg_t *p)
void custom_set_usb_cfg_params()
{
//	if(p->vendor_id!=U16_MAX)
//	{
//		device_desc_km[8] =  p->vendor_id&0xff;
//		device_desc_km[9] =  (p->vendor_id>>8)&0xff;
//	}
//
//	if(p->prodct_id!=U16_MAX)
//	{
//		device_desc_km[10] =  p->prodct_id&0xff;
//		device_desc_km[11] =  (p->prodct_id>>8)&0xff;
//	}
	
//	if(SUPPORT_KEYBOARD_DEVICE)//1:support kb&ms
//	{
//		//device_desc_km[10] +=1;//set pid
//		//km_cfg_desc[4]= 2;	//interface num config
//	}
//	else
	{
		//device_desc_km[10] -=1;//set pid
#if WEB_HID_ENABLE
		km_cfg_desc[4] = 3;
#elif OTA_ENABLE_AAA
		km_cfg_desc[2]=km_cfg_desc[2]-25;
		km_cfg_desc[4]=2;//interface  num
#else
		km_cfg_desc[2]=km_cfg_desc[2]-25;
		km_cfg_desc[4]=1;//interface  num
#endif
	}

//	if(p->report_type==0)//mouse report  250
//	{
//		km_cfg_desc[33]=1; //PollingIntervalMS = USB_MOUSE_POLL_INTERVAL
//	}

	//set_string_from_addr(&km_vendor_desc,CFG_ADR_MAC+0x200);
	//set_string_from_addr(&km_product_desc,CFG_ADR_MAC+0x300);
	//set_string_from_addr(&km_serial_desc,CFG_ADR_MAC+0x400);

}

unsigned char *usbdesc_get_device(void) {
	return (unsigned char *)(&device_desc_km);
}

unsigned char *usbdesc_get_configuration(void) {
	return (unsigned char *)(km_cfg_desc);
}
unsigned short usb_desc_get_configuration_size(){
	return km_cfg_desc[2];//len_total_km_cfg_desc;
}

u8 * usb_get_HID_DTYPE_Report(u8 index,u16 *g_response_len)
{
	if(index==0)
	{
		g_response_len[0]=sizeof(mouse_report_desc);
		return (u8*)&mouse_report_desc[0];
	}
#if WEB_HID_ENABLE
	else if(index==1)
	{
		g_response_len[0]=sizeof(vendor_status_in_report_desc);
		return (u8*)&vendor_status_in_report_desc[0];
	}
	else if(index==2)
	{
		g_response_len[0]=sizeof(kb_report_desc);
		return (u8*)&kb_report_desc[0];
	}
#elif OTA_ENABLE_AAA
	else if(index==1)
	{
		g_response_len[0]=sizeof(kb_report_desc);
		return (u8*)&kb_report_desc[0];
	}
	else //if(index==2)
	{
		g_response_len[0]=sizeof(spp_report_desc);
		return (u8*)&spp_report_desc[0];
	}
#else
	else if(index==1)
	{
		g_response_len[0]=sizeof(kb_report_desc);
		return (u8*)&kb_report_desc[0];
	}
#endif
}

u8 * usb_get_HID_DTYPE_HID(u8 index,u16 *g_response_len)
{
	if(index==0)
	{
		g_response_len[0]=9+7;
		return (unsigned char*) (&km_cfg_desc[9+9]);
	}
#if WEB_HID_ENABLE
	else if(index==1)
	{
		g_response_len[0]=9+7;
		return (unsigned char*) (&km_cfg_desc[9+9+9+7+9]);
	}
	else if(index==2)
	{
		g_response_len[0]=9+7;
		return (unsigned char*) (&km_cfg_desc[9+9+9+7+9+9+7+9]);
	}
#elif OTA_ENABLE_AAA
	else if(index==1)
	{
		g_response_len[0]=9+7;
		return (unsigned char*) (&km_cfg_desc[9+9+9+7+9]);
	}
	else //if(index==2)
	{
		g_response_len[0]=9+7;
		return (unsigned char*) (&km_cfg_desc[9+9+9+7+9+9+7+9]);
	}
#else
	else if(index==1)
	{
		g_response_len[0]=9+7;
		return (unsigned char*) (&km_cfg_desc[9+9+9+7+9]);
	}
#endif
}

#if 0
unsigned char* usbmouse_get_report_desc(void) {
	return (unsigned char*) (mouse_report_desc);
}
unsigned short usbmouse_get_report_desc_size(void) {
	return sizeof(mouse_report_desc);
}
unsigned char* usbkb_get_report_desc(void) {
	return (unsigned char*) (kb_report_desc);
}
unsigned short usbkb_get_report_desc_size(void) {
	return sizeof(kb_report_desc);
}

unsigned char* usbdesc_get_mouse(void) {
	return (unsigned char*) (&km_cfg_desc[9+9]);
}
unsigned char* usbdesc_get_keyboard(void) {
	return (unsigned char*) (&km_cfg_desc[9+9+9+7+9]);
}
#endif

unsigned char *usbdesc_get_language(void) {
	return (unsigned char *)(&km_language_desc);
}
unsigned char *usbdesc_get_vendor(void) {
	return (unsigned char *)(&km_vendor_desc);
}
unsigned short usb_desc_get_vendor_size(void) {
	return km_vendor_desc.Size;
}
unsigned char *usbdesc_get_product(void) {
	return (unsigned char *)(&km_product_desc);
}
unsigned short usb_desc_get_product_size(void) {
	return km_product_desc.Size;
}
unsigned char *usbdesc_get_serial(void) {
	return (unsigned char *)(&km_serial_desc);
}
unsigned short usb_desc_get_serial_size(void) {
	return km_serial_desc.Size;
}
#endif

int usb_keyboard_hid_report_aaa(unsigned char *data)
{

    if(usb_device_status==USB_DEVICE_CHECK_PC_SLEEP)
    {
		usb_resume_host();
		
		//usb_io_printf("resume usb host\r\n");
		return 0;
    }
	else if((usb_device_status!=USB_DEVICE_CONNECT_PC))
    {
    	return 1;
    }

	//if(user_config.support_type!=SUPPORT_KEYBOARD_DEVICE)
	//{
		//return 1;
	//}
   

	 
	if(usbhw_is_ep_busy(USB_EDP_KEYBOARD_IN))
		return 0;

    //printf("\n KeyBoard HID report:");

	reg_usb_ep_ptr(USB_EDP_KEYBOARD_IN) = 0;

	for(int i = 0; i < 8; ++i){
    	reg_usb_ep_dat(USB_EDP_KEYBOARD_IN) = data[i];
    	//printf("0x%1x ",data[i]);
    }
	//	reg_usb_ep_ctrl(USB_EDP_KEYBOARD_IN) = FLD_EP_DAT_ACK;		// ACK

	usbhw_data_ep_ack(USB_EDP_KEYBOARD_IN);

	return 1;
}

extern unsigned char connect_ok;
extern u8 usb_data_eps_ready;

int usb_mouse_hid_report_aaa(u8 report_id,unsigned char * p,u8 len)
{
	static u32 mouse_ep_busy_tick;

	if (!connect_ok) {
		return 0;
	}

	if (!usb_data_eps_ready) {
		return 0;
	}

	if(usb_device_status==USB_DEVICE_CHECK_PC_SLEEP)
    {
        usb_resume_host();
        return 0;
    }
	else if((usb_device_status==USB_DEVICE_DISCONECT_PC) ||
	        (usb_device_status==USB_DEVICE_UNPLUG))
    {
        return 1;
    }

	if(usbhw_is_ep_busy(USB_EDP_MOUSE)){
		if (!mouse_ep_busy_tick) {
			mouse_ep_busy_tick = clock_time() | 1;
		} else if (clock_time_exceed(mouse_ep_busy_tick, 50000)) {
			reg_usb_ep_ctrl(USB_EDP_MOUSE) = 0;
			mouse_ep_busy_tick = 0;
		} else {
			return 0;
		}
    } else {
		mouse_ep_busy_tick = 0;
	}

	if ((usb_mouse_report_proto == 0) && (report_id != 1)) {
        return 1;
    }

	reg_usb_ep_ptr(USB_EDP_MOUSE) = 0;

	if ((usb_mouse_report_proto == 0) && (report_id == 1)) {
		reg_usb_ep_dat(USB_EDP_MOUSE) = p[0];
		reg_usb_ep_dat(USB_EDP_MOUSE) = p[1];
#if (MOUSE_DATA_LEN_AAA==6) || (MOUSE_DATA_LEN_AAA==7)
		reg_usb_ep_dat(USB_EDP_MOUSE) = p[3];
#elif (MOUSE_DATA_LEN_AAA==4)
		reg_usb_ep_dat(USB_EDP_MOUSE) = p[2];
#endif
	} else {
		reg_usb_ep_dat(USB_EDP_MOUSE) = report_id;
		if (report_id == 1) {
#if (MOUSE_DATA_LEN_AAA==6) || (MOUSE_DATA_LEN_AAA==7)
			reg_usb_ep_dat(USB_EDP_MOUSE) = p[0];
			reg_usb_ep_dat(USB_EDP_MOUSE) = p[1];
			reg_usb_ep_dat(USB_EDP_MOUSE) = p[2];
			reg_usb_ep_dat(USB_EDP_MOUSE) = p[3];
			reg_usb_ep_dat(USB_EDP_MOUSE) = p[4];
			reg_usb_ep_dat(USB_EDP_MOUSE) = p[5];
#elif (MOUSE_DATA_LEN_AAA==4)
			reg_usb_ep_dat(USB_EDP_MOUSE) = p[0];
			reg_usb_ep_dat(USB_EDP_MOUSE) = p[1];
			reg_usb_ep_dat(USB_EDP_MOUSE) = p[2];
			reg_usb_ep_dat(USB_EDP_MOUSE) = p[3];
#endif
		} else {
			for (u8 i = 0; i < len; i++) {
				reg_usb_ep_dat(USB_EDP_MOUSE) = p[i];
			}
		}
	}

	usbhw_data_ep_ack(USB_EDP_MOUSE);

	return 1;
}

/**
 * @brief	wake up host
 * @param	data
 * @return	result
 */
void usb_resume_host(void)
{
	if(need_enter_suspend_flag)
	{
		if ((reg_usb_mdev & BIT(2)) && (reg_irq_src & FLD_IRQ_USB_PWDN_EN) )
		//if(usb_device_status==USB_DEVICE_CHECK_PC_SLEEP)
		{  
			need_enter_suspend_flag=0;
			reg_wakeup_en = FLD_WAKEUP_SRC_USB_RESM;
			reg_wakeup_en = FLD_WAKEUP_SRC_USB;
			//u_printf("resume usb host\r\n");
		}
	}
}


#endif


