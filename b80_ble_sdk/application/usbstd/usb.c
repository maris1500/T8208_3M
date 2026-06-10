/********************************************************************************************************
 * @file     usb.c
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

#include "vendor/8208_dual_mouse/AAA_project_config.h"


#define MODULE_USB_ENABLE  1
//#define FLOW_NO_OS         0




#ifndef USB_CUSTOM_HID_REPORT_REG_ACCESS
#define	USB_CUSTOM_HID_REPORT_REG_ACCESS		1
#endif

#if (MODULE_USB_ENABLE)//if usb not use

//#include "usb.h"
//#include "usbdesc.h"
#include "../usbstd/StdRequestType.h"
//#include "../../../../drivers.h"				// inline
//#include "../../../usbhw_i.h"

#if(USB_CDC_ENABLE)
#include "../usb_app/usbcdc_i.h"
#endif


#if (USB_MOUSE_ENABLE)
#include "../usb_app/usbmouse_i.h"
#endif

#if (USB_KEYBOARD_ENABLE)
#include "../usb_app/usbkb_i.h"
#endif

#if (USB_SOMATIC_ENABLE)
//#include "../usb_app/usbsomatic_i.h"
//#include "somatic_sensor.h"
#endif

#if (USB_SPEAKER_ENABLE || USB_MIC_ENABLE)
#include "../usb_app/usbaud_i.h"

#endif

#ifdef WIN32
#include <stdio.h>
#endif

usb_set_hid_report_t  usb_custom_set_hid_report_func=0;
//u8 ota_cmd_flag=0;
unsigned char usb_g_config = 0;

volatile unsigned char usb_g_feature=0;
extern u8 keyboard_interface_number, mouse_interface_number;
extern u8 usb_configure_ok;
extern u8 connect_ok;
extern u32 usb_mode_start_tick;
extern u8 usb_data_eps_ready;
u8		host_keyboard_status;
u8		host_cmd[8];
u8		host_cmd_paring_ok = 0;
static USB_Request_Hdr_t control_request;
static u8 * g_response = 0;
static u16 g_response_len = 0;
static int g_stall = 0;
u8 usb_mouse_report_proto = 1; //1: report protocol (Windows composite HID)
u8 g_rate = 0; //default 0 for all report
#if(MCU_CORE_B80)
static unsigned short usb_len_idx_0;
static unsigned short usb_len_idx_s;
static unsigned short usb_len_idx_h;
#endif
#if (USB_SPEAKER_ENABLE || USB_MIC_ENABLE)
u8 usb_alt_intf[USB_INTF_MAX];

#endif

void usb_register_set_report(usb_set_hid_report_t src)
{
	usb_custom_set_hid_report_func=src;
}

/**
 * @brief		This function serves to send response to USB host
 * @param[in] 	none
 * @return 		none
 */
void usb_send_response(void) {
	u16 n;
	if (g_response_len < 8) {
		n = g_response_len;
	} else {
		n = 8;
	}
	g_response_len -= n;
#if(MCU_CORE_B80)
	usb_len_idx_0=n;
#endif
	usbhw_reset_ctrl_ep_ptr();
	while (n-- > 0) {
		usbhw_write_ctrl_ep_data(*g_response);
		++g_response;
	}
}

#if USB_DESCRIPTOR_MY_SELF
extern u32 usb_web_grace_tick;
extern u8 usb_data_eps_ready;

/* Mouse-first: enable mouse IN immediately on config; never delay for Web. */
static void usb_mark_configured(void)
{
	if (connect_ok) {
		return;
	}
	usb_g_config = 1;
	connect_ok = 1;
	usb_mouse_report_proto = 1;
	usb_data_eps_ready_on_config();
	usb_mode_start_tick = clock_time() | 1;
	printf("usb cfg ok\n");
}
#endif

void usb_prepare_desc_data(void) {
	u8 value_l = (control_request.Value) & 0xff;
	u8 value_h = (control_request.Value >> 8) & 0xff;
	g_response = 0;
	g_response_len = 0;
	switch (value_h) {

	case DTYPE_Device:
#if(USB_ID_AND_STRING_CUSTOM)
		g_response = (u8*) (&device_desc_km);
#else
		g_response = usbdesc_get_device();

		//usb_configure_ok =1;
#endif
		g_response_len = sizeof(USB_Descriptor_Device_t);
#if OTA_ENABLE_AAA
		usbhw_data_ep_ack(USB_EDP_SPP_OUT);//must 
#endif
		break;

	case DTYPE_Configuration:
		
#if(USB_DESCRIPTER_CONFIGURATION_FOR_KM_DONGLE)
		g_response = (u8*) (&configuration_km_desc);
		g_response_len = configuration_km_desc[2];  //the third element is the len
#else
		g_response = usbdesc_get_configuration();

		usb_configure_ok = 1;
#if USB_DESCRIPTOR_MY_SELF
		printf("GET_CFG ok\n");
		usb_mark_configured();
#endif

		#if (USB_DESCRIPTOR_MY_SELF==0)

			g_response_len = sizeof(USB_Descriptor_Configuration_t);
		#else 
			g_response_len = usb_desc_get_configuration_size();
		#endif
#endif
		break;

	case DTYPE_String:
#if(USB_ID_AND_STRING_CUSTOM)
		if (USB_STRING_LANGUAGE == value_l) {
			g_response = usbdesc_get_language();
			g_response_len = sizeof(LANGUAGE_ID_ENG);
		} else if (USB_STRING_VENDOR == value_l) {
			g_response = (u8*) (&vendor_desc_km);
			g_response_len = vendor_desc_km.Size;
		} else if (USB_STRING_PRODUCT == value_l) {
			g_response = (u8*) (&prodct_desc_km);
			g_response_len = prodct_desc_km.Size;
		} else if (USB_STRING_SERIAL == value_l) {
			g_response = (u8*) (&serial_desc_km);
			g_response_len = serial_desc_km.Size;
#else
		if (USB_STRING_LANGUAGE == value_l) {
			g_response = usbdesc_get_language();
			g_response_len = sizeof(LANGUAGE_ID_ENG);
	#if (USB_DESCRIPTOR_MY_SELF==0)
		} else if (USB_STRING_VENDOR == value_l) {
			g_response = usbdesc_get_vendor();
			g_response_len = sizeof(STRING_VENDOR);
		} else if (USB_STRING_PRODUCT == value_l) {
			g_response = usbdesc_get_product();
			g_response_len = sizeof(STRING_PRODUCT);
		} else if (USB_STRING_SERIAL == value_l) {
			g_response = usbdesc_get_serial();
			g_response_len =sizeof(STRING_SERIAL);
	#else
		} else if (USB_STRING_VENDOR == value_l) {
			g_response = usbdesc_get_vendor();
			g_response_len = usb_desc_get_vendor_size();
		} else if (USB_STRING_PRODUCT == value_l) {
			g_response = usbdesc_get_product();
			g_response_len = usb_desc_get_product_size();
		} else if (USB_STRING_SERIAL == value_l) {
			g_response = usbdesc_get_serial();
			g_response_len =usb_desc_get_serial_size();
	#endif

#endif

#if (MS_OS_DESCRIPTOR_ENABLE)
		} else if (USB_STRING_MS_OS == value_l) {
			g_response = usbdesc_get_OS_descriptor();
			g_response_len = sizeof(STRING_MSFT);
#endif

		}
		else {
			g_stall = 1;
		}
		break;

	default:
		g_stall = 1;
		break;

	}
#if(MCU_CORE_B80)	
	usb_len_idx_s = g_response_len;
#endif
	if (control_request.Length < g_response_len) {
		g_response_len = control_request.Length;
	}

	return;
}

//standard interface request handle

void usb_handle_std_intf_req() {
	u8 value_h = (control_request.Value >> 8) & 0xff;
#if( USB_DESCRIPTOR_MY_SELF|USB_MIC_ENABLE || USB_SPEAKER_ENABLE || USB_MOUSE_ENABLE || USB_KEYBOARD_ENABLE || USB_SOMATIC_ENABLE)
	u8 index_l = (control_request.Index) & 0xff;
#endif
	switch (value_h) {
	case HID_DTYPE_HID:// HID Descriptor
#if USB_DESCRIPTOR_MY_SELF
	g_response=usb_get_HID_DTYPE_HID(index_l,(u16 *)&g_response_len);
	if (!g_response || !g_response_len) {
		g_stall = 1;
	} else {
		printf("GET_HID intf=%d len=%d\n", index_l, g_response_len);
	}
#else
  #if(USB_MOUSE_ENABLE)
   #if(USB_DESCRIPTER_CONFIGURATION_FOR_KM_DONGLE)
		if (index_l == mouse_interface_number)
		{
			g_response = (u8*) (&configuration_desc_mouse[9]);
	
			g_response_len = USB_HID_DESCRIPTOR_LENGTH;
				
			
		}
  #else
		if (index_l == USB_INTF_MOUSE) //index_l is the interface number
		{
			//mouse
			g_response = usbdesc_get_mouse();
			g_response_len = sizeof(USB_HID_Descriptor_HID_Mouse_t);
		}
  #endif
#endif
#if(USB_KEYBOARD_ENABLE)
#if(USB_DESCRIPTER_CONFIGURATION_FOR_KM_DONGLE)
		if (index_l == keyboard_interface_number)
		{
			g_response = (u8*) (&configuration_desc_keyboard[9]);
			g_response_len = USB_HID_DESCRIPTOR_LENGTH;
		}
#else
		if (index_l == USB_INTF_KEYBOARD) {
			//keyboard
			g_response = usbdesc_get_keyboard();
			g_response_len = sizeof(USB_HID_Descriptor_HID_Keyboard_t);
		}
#endif
#endif
#if(USB_SOMATIC_ENABLE )
		if (index_l == USB_INTF_SOMATIC) //index_l is the interface number
		{
			//SOMATIC
			g_response = usbdesc_get_somatic();
			g_response_len = sizeof(USB_HID_Descriptor_HID_Somatic_t);
		}
#endif
#endif
		break;
	case HID_DTYPE_Report://Report Descriptor
#if USB_DESCRIPTOR_MY_SELF
	if (index_l < 3) {
		g_response = usb_get_HID_DTYPE_Report(index_l, (u16 *)&g_response_len);
		if (!g_response || !g_response_len) {
			g_stall = 1;
		} else {
			printf("GET_RPT intf=%d len=%d\n", index_l, g_response_len);
			if (index_l == 1) {
				usb_web_intf_eps_ready();
			}
		}
	} else {
		g_stall = 1;
	}
#else
	 if(0)
   {
   }

#if(USB_KEYBOARD_ENABLE)
	else if (index_l == (USB_DESCRIPTER_CONFIGURATION_FOR_KM_DONGLE ? keyboard_interface_number : USB_INTF_KEYBOARD)) {
			//keyboard
			g_response = (u8*) usbkb_get_report_desc();
			g_response_len = usbkb_get_report_desc_size();
		}
#endif
#if(USB_MOUSE_ENABLE)
		else if (index_l == (USB_DESCRIPTER_CONFIGURATION_FOR_KM_DONGLE ? mouse_interface_number : USB_INTF_MOUSE)) {
			//mouse
			g_response = (u8*) usbmouse_get_report_desc();
			g_response_len = usbmouse_get_report_desc_size();
		}
#endif

#if(USB_SOMATIC_ENABLE)
		else if (index_l == USB_INTF_SOMATIC) {
			//somatic sensor
			g_response = (u8*) usbsomatic_get_report_desc();
			g_response_len = usbsomatic_get_report_desc_size();

		}
#endif
		else{
			g_stall = 1;
		}
#endif
		break;
	case 0x23:// Phisical Descriptor
		// TODO
		break;

	default:// other condition
		break;
	}

	if (control_request.Length < g_response_len) {
		g_response_len = control_request.Length;
	}

	return;
}

u32			custom_read_dat;
u32			custom_reg_cmd;

#if USB_DESCRIPTOR_MY_SELF
void usb_ep0_out_setup(u16 length)
{
#if(MCU_CORE_B80)
	usb_len_idx_s = length;
	usb_len_idx_0 = 0;
#endif
}

void usb_ep0_out_update(u16 remaining)
{
#if(MCU_CORE_B80)
	usb_len_idx_s = remaining;
	usb_len_idx_0 = 0;
#endif
}

static void usb_hid_web_set_report(u8 data_request)
{
	u8 report_id = control_request.Value & 0xff;
	u8 intf = control_request.Index & 0xff;

	if (data_request == USB_IRQ_SETUP_REQ) {
		usb_hid_web_ep0_reset();
		usb_ep0_out_setup(control_request.Length);
		printf("SET_REPORT setup id=0x%02x len=%d intf=%d wVal=0x%04x\r\n",
			report_id, control_request.Length, intf,
			control_request.Value);
	}

	if (usb_custom_set_hid_report_func) {
		usb_custom_set_hid_report_func(data_request, report_id, control_request.Length);
	}
}

static u8 hid_get_report_buf[HID_WEB_REPORT_WIRE_LEN];

static void usb_hid_prepare_get_report(void)
{
	u16 filled;

	g_response = hid_get_report_buf;
	filled = usb_hid_fill_get_report(control_request.Value, hid_get_report_buf, sizeof(hid_get_report_buf));
	{
		u8 rpt_type = (control_request.Value >> 8) & 0xff;
		if (rpt_type >= 2 || (control_request.Value & 0xff) >= REPORT_ID_STATUS_INPUT_AAA) {
			printf("GET_REPORT wVal=0x%02x len=%d fill=%d\r\n",
				control_request.Value, control_request.Length, filled);
		}
	}
	if (filled == 0) {
		printf("WEB REPLY FAIL wVal=0x%02x fill=0 stall\r\n",
			control_request.Value);
		g_stall = 1;
		g_response = 0;
		g_response_len = 0;
		return;
	}

	g_response_len = filled;
	if (control_request.Length < g_response_len) {
		g_response_len = control_request.Length;
	}
#if(MCU_CORE_B80)
	usb_len_idx_s = g_response_len;
	usb_len_idx_0 = 0;
#endif
}
#endif

void usb_handle_out_class_intf_req(int data_request) {
	u8 property = control_request.Request;
	u8 value_l = (control_request.Value) & 0xff;
	u8 value_h = (control_request.Value >> 8) & 0xff;
#if (USB_MIC_ENABLE || USB_SPEAKER_ENABLE)
	u8 Entity = (control_request.Index >> 8) & 0xff;
#endif

	switch (property) {
    #if(USB_CDC_ENABLE)//for cdc
	 // #if(0)//for cdc
	   case CDC_REQ_SetControlLine_State:
		  // if(USB_IRQ_SETUP_REQ==data_request)
		    //printf("SET_CONTROL_LINE_STATE:\r\n");
		break;
		case CDC_REQ_SetLine_Encoding:					//--2-2. set serial parameter:baudrate,check,stop and data

                  if(USB_IRQ_DATA_REQ==data_request)
                  {
					usbhw_reset_ctrl_ep_ptr();
					for(int i = 0; i < 7; ++i)
					{
					  LineCoding[i] = usbhw_read_ctrl_ep_data();
					}
					//-todo- according to LineCoding from USB HOST, user can set baud rate, stop bits, parity, data bit, etc.
                  }
				 break;



    #endif
	case HID_REQ_SetReport:
#if USB_DESCRIPTOR_MY_SELF
		{
			u8 intf = control_request.Index & 0xff;
			u8 rid = value_l;

			/* Web: wValue=0x0306 (id in low byte) or 0x0300 (id in data[0]), intf=1, len=32 */
			if ((rid != REPORT_ID_USER_FEATURE_AAA &&
			     rid != REPORT_ID_DRV_FEATURE_AAA) &&
			    (intf == 1) &&
			    (control_request.Length >= 8)) {
				rid = 0;
			}
			if (((rid == REPORT_ID_USER_FEATURE_AAA ||
			      rid == REPORT_ID_DRV_FEATURE_AAA) ||
			     (intf == 1 && control_request.Length >= 8)) &&
			    control_request.Length > 0) {
				usb_hid_web_set_report(data_request);
				break;
			}
		}
#endif
		switch (value_h) {
		case HID_REPORT_ITEM_In:
			break;
		case HID_REPORT_ITEM_Out:
			// usb_hid_set_report_ouput();
			break;
		case HID_REPORT_ITEM_Feature:
			if (data_request)
			{
				if((control_request.Value==0x0200)&&(control_request.Length==1))
				{
					host_keyboard_status = usbhw_read_ctrl_ep_data();
				}
			#if OTA_ENABLE_AAA
				else if((control_request.Value & 0xff00) == 0x0200)
				{
					u8 report_id=control_request.Value&0xff;
					u16 length=control_request.Length;

					if(usb_custom_set_hid_report_func)
					{
						usb_custom_set_hid_report_func(data_request,report_id,length);
					}
				}
			#endif
			}
#if(USB_SET_REPORT_FEATURE_SUPPORT)
		{
			usb_set_report_t rpt;
			rpt.report_id = value_l;
			rpt.len = control_request.Index;
			ev_emit_event_syn(EV_USB_SET_REPORT, (void*)(&rpt));	// send in report id
		}
#endif
			break;
		case HID_REPORT_CUSTOM:
#if (USB_CUSTOM_HID_REPORT)
		{	// Web sendFeatureReport() wValue=0x03xx
#if USB_DESCRIPTOR_MY_SELF
			if ((control_request.Index & 0xff) == 1 &&
			    control_request.Length >= 8) {
				usb_hid_web_set_report(data_request);
			}
#elif 1
			if (data_request) {
				int i = 0;

				usbhw_reset_ctrl_ep_ptr();
				for (i = 0; i < 8; i++) {
					host_cmd[i] = usbhw_read_ctrl_ep_data();
				}
				printf("USB HID OUT custom data:");
				for (i = 0; i < 8; i++) {
					printf(" %1x", host_cmd[i]);
				}
				printf("\r\n");
			}
#endif
#if 0//(USB_CUSTOM_HID_REPORT_REG_ACCESS)
				custom_reg_cmd = (host_cmd[1] & 0xf0) == 0xc0;
				if (custom_reg_cmd) {
					host_cmd[0] = 0;
					int adr = *((u16 *)(host_cmd + 2));
					int len = host_cmd[1] & 3;
					if (host_cmd[1] == 0xcc && adr == 0x5af0) { //re-enumerate device
						usb_dp_pullup_en (0);			//disable device
						sleep_us (300000);
						reg_ctrl_ep_irq_mode = 0xff;	//hardware mode
						usb_dp_pullup_en (1);			//enable device
					}
					else {
						adr += 0x800000;
					}

					if ((host_cmd[1] & 0x0c)==0) {	//write core register
						if (len == 0) {
							for (int k=0; k<4; k++) {
								custom_read_dat = (custom_read_dat >> 8) | (read_reg8 (adr++) << 24);
							}
			
						}
						else if (len == 1) {
							//write_reg8 (adr, host_cmd[4]);
						}
						else if (len == 2) {
							//write_reg16 (adr, *((u16 *)(host_cmd + 4)));
						}
						else {
							//write_reg32 (adr, *((u32 *)(host_cmd + 4)));
						}
					}
					else {	//read core register
						if (len == 0) {
							custom_read_dat = analog_read (host_cmd[2]);
						}
						else {
							//analog_write (host_cmd[2], host_cmd[4]);
						}
					}
				}
#endif
			}
			break;
#endif
		default:
			g_stall = 1;
			break;
		}
		break;

	case HID_REQ_SetIdle:
		if (data_request) {
			g_rate = usbhw_read_ctrl_ep_data();
		}
		g_rate = value_h;
		break;

	case HID_REQ_SetProtocol:
		/* Boot protocol only applies to interface 0 (mouse) */
		if ((control_request.Index & 0xff) == 0) {
			if (data_request) {
				usb_mouse_report_proto = usbhw_read_ctrl_ep_data();
			} else {
				usb_mouse_report_proto = value_l;
			}
#if USB_DESCRIPTOR_MY_SELF
			if (usb_g_config) {
				reg_usb_ep_ctrl(USB_EDP_MOUSE) = 0;
				usbhw_data_ep_ack(USB_EDP_MOUSE);
			}
#else
			reg_usb_ep_ctrl(USB_EDP_MOUSE) = 0;
			usbhw_data_ep_ack(USB_EDP_MOUSE);
#endif
		}
		break;
#if (0)
	case CDC_REQ_SetLine_Encoding:
	case CDC_REQ_SetControlLine_State:
		CDC_Device_ProcessControlRequest(control_request.Request, control_request.Value,
			control_request.Index, control_request.Length);
		break;
#endif
	default:
		g_stall = 1;
		break;
	}




#if (USB_MIC_ENABLE || USB_SPEAKER_ENABLE)
	if(0 == g_stall){		// already handled
		return;
	}
	g_stall = 0;
	switch(Entity){
		case USB_SPEAKER_FEATURE_UNIT_ID:
			usbaud_handle_set_speaker_cmd(value_h);
			break;
		case USB_MIC_FEATURE_UNIT_ID:
			usbaud_handle_set_mic_cmd(value_h);
			break;
		default:
			g_stall = 1;
			break;
	}
#endif
}



void usb_handle_in_class_intf_req() {
	u8 property = control_request.Request;
#if (USB_MIC_ENABLE || USB_SPEAKER_ENABLE)
	u8 value_h = (control_request.Value >> 8);
	u8 Entity = (control_request.Index >> 8);
#endif
	switch (property) {
		case 0x00:
			usbhw_write_ctrl_ep_data(0x00);
			break;

#if(USB_CDC_ENABLE)
//#if(1)
			case CDC_REQ_GetLine_Encoding:
							//printf("--2-1.GET_LINE_CODING.\r\n");
				usbhw_write_ctrl_ep_data(LineCoding[0]);
				usbhw_write_ctrl_ep_data(LineCoding[1]);
				usbhw_write_ctrl_ep_data(LineCoding[2]);
				usbhw_write_ctrl_ep_data(LineCoding[3]);
				usbhw_write_ctrl_ep_data(LineCoding[4]);
				usbhw_write_ctrl_ep_data(LineCoding[5]);
				usbhw_write_ctrl_ep_data(LineCoding[6]);


			break;

			case CDC_NOTIF_Serial_State:
							//printf("--2-2.SERIAL_STATE.\r\n");
			break;
#endif
		case HID_REQ_GetReport:
#if (USB_CUSTOM_HID_REPORT)
			/* multi-packet IN handled in usb_handle_request() */
			break;
#elif(USB_SOMATIC_ENABLE)
			if(usbsomatic_hid_report_type((control_request.Value & 0xff))){
			}
			else
			{	//  donot know what is this
			}
#else
			{	//  donot know what is this
			}
#endif
			break;
		case HID_REQ_GetIdle:
			usbhw_write_ctrl_ep_data(g_rate);
			break;
		case HID_REQ_GetProtocol:
			usbhw_write_ctrl_ep_data(usb_mouse_report_proto);
			break;


		default:
			g_stall = 1;
			break;
	}
#if (USB_MIC_ENABLE || USB_SPEAKER_ENABLE)
	if(0 == g_stall){		// already handled
		return;
	}
	g_stall = 0;
	switch(Entity){
		case USB_SPEAKER_FEATURE_UNIT_ID:
			if(usbaud_handle_get_speaker_cmd(property, value_h)){
				g_stall = 1;
			}
			break;
		case USB_MIC_FEATURE_UNIT_ID:
			if(usbaud_handle_get_mic_cmd(property, value_h)){
				g_stall = 1;
			}
			break;
		default:
			g_stall = 1;
			break;
	}
#endif

}


void usb_handle_in_class_endp_req() {

#if (USB_MIC_ENABLE || USB_SPEAKER_ENABLE)
	u8 property = control_request.Request;
	u8 ep_ctrl = control_request.Value >> 8;

	//u8 addr = (control_request.Index >> 8);

	if(ep_ctrl == AUDIO_EPCONTROL_SamplingFreq){
		switch(property){
		case AUDIO_REQ_GetCurrent:
			usbhw_write_ctrl_ep_data(MIC_SAMPLE_RATE & 0xff);
			usbhw_write_ctrl_ep_data(MIC_SAMPLE_RATE >> 8);
			usbhw_write_ctrl_ep_data(MIC_SAMPLE_RATE >> 16);
			break;
		default:
			break;
		}
	}
#endif
}

void usb_handle_out_class_endp_req(int data_request) {
	return;
#if 0
	u8 property = control_request.Request;
	u8 ep_ctrl = control_request.Value & 0xff;
#if (USB_MIC_ENABLE || USB_SPEAKER_ENABLE)
	u8 addr = (control_request.Index >> 8);
#endif
#endif
}


void usb_handle_set_intf() {
#if (USB_SPEAKER_ENABLE || USB_MIC_ENABLE)
	u8 value_l = (control_request.Value) & 0xff;
	u8 intf_index = (control_request.Index) & 0x07;
	//assert(intf_index < USB_INTF_MAX);
	usb_alt_intf[intf_index] = value_l;

#if (USB_MIC_ENABLE)
	if(USB_INTF_MIC == intf_index && value_l){
//		usbhw_reset_ep_ptr(USB_EDP_MIC);
//		reg_usb_ep_ptr(USB_EDP_MIC) = USB_MIC_CHANNELS_LEN;
//		reg_usb_ep_ctrl(USB_EDP_MIC) = (MIC_CHANNLE_COUNT == 2 ? 0x81 : 0xc1);
		reg_usb_ep_ptr(USB_EDP_MIC) = 0;
		reg_usb_ep_ctrl(USB_EDP_MIC) = BIT(0);		//ACK first packet
	}
#endif

#if (USB_SPEAKER_ENABLE)
	if(USB_INTF_SPEAKER == intf_index && value_l){
//		usbhw_reset_ep_ptr(USB_EDP_MIC);
//		reg_usb_ep_ptr(USB_EDP_MIC) = USB_MIC_CHANNELS_LEN;
//		reg_usb_ep_ctrl(USB_EDP_MIC) = (MIC_CHANNLE_COUNT == 2 ? 0x81 : 0xc1);
		reg_usb_ep_ptr(USB_EDP_SPEAKER) = 0;
		reg_usb_ep_ctrl(USB_EDP_SPEAKER) = BIT(0);		//ACK first packet
	}
#endif
#endif
	return;
}

#if (USB_SPEAKER_ENABLE || USB_MIC_ENABLE)
void usb_handle_get_intf() {
	u8 intf_index = (control_request.Index) & 0x07;
	//assert(intf_index < USB_INTF_MAX);

	usbhw_write_ctrl_ep_data(usb_alt_intf[intf_index]);

	return;
}
#endif


void usb_handle_request(u8 data_request) {
	u8 RequestType = control_request.RequestType;
	u8 Request = control_request.Request;

	usbhw_reset_ctrl_ep_ptr();
	switch (RequestType) {
	case (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE)://闁硅鲸娲戠拹鐔兼偣閻戝洦瀚�
		if (REQ_GetDescriptor == Request) {
			if (USB_IRQ_SETUP_REQ == data_request) {
				usb_prepare_desc_data();
			}
			usb_send_response();
		}
		break;

	case (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_INTERFACE)://闁硅鲸娲戠拹鐔封槈閿燂拷
		if (REQ_GetDescriptor == Request) {
			if (USB_IRQ_SETUP_REQ == data_request) {
				usb_handle_std_intf_req();
			}
			usb_send_response();

		}
#if (USB_SPEAKER_ENABLE || USB_MIC_ENABLE)
		else if (REQ_GetInterface == Request) {
			usb_handle_get_intf();
		}
#elif USB_DESCRIPTOR_MY_SELF
		else if (REQ_GetInterface == Request) {
			if (USB_IRQ_DATA_REQ == data_request) {
				usbhw_write_ctrl_ep_data(0);
			}
		}
#endif
		break;
#if (MS_OS_DESCRIPTOR_ENABLE)
	case (REQDIR_DEVICETOHOST | REQTYPE_VENDOR | REQREC_DEVICE):
	case (REQDIR_DEVICETOHOST | REQTYPE_VENDOR | REQREC_INTERFACE):
		 if ((Request ==  MS_VENDORCODE)) {//Retrieve an OS Feature Descriptor
			u8 index_l = control_request.Index&0xff;
			if (USB_IRQ_SETUP_REQ == data_request) {
				//usb_indexl==0x04 for Extended compat ID
				//usb_indexl==0x05 for Extended properties
				if(index_l==0x04 )
				{
					g_response = usbdesc_get_compatID(&g_response_len);
				}
				else if(index_l==0x05)
				{
					g_response = usbdesc_get_OSFeature(&g_response_len);
				}
				else
					g_stall = 1;
				if (control_request.Length < g_response_len) {
					g_response_len = control_request.Length;
				}
			}

			usb_send_response();
		}
		break;
#endif
	case (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE):
		usb_handle_out_class_intf_req(data_request);
		break;
	case (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_ENDPOINT):
		usb_handle_out_class_endp_req(data_request);
		break;
	case (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE):
#if (USB_CUSTOM_HID_REPORT)
		if (Request == HID_REQ_GetReport) {
			if (USB_IRQ_SETUP_REQ == data_request) {
				usb_hid_prepare_get_report();
			}
			if (!g_stall && g_response_len > 0) {
				usb_send_response();
			}
		} else
#endif
		{
			usb_handle_in_class_intf_req();
		}
		break;
	case (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_ENDPOINT):
		usb_handle_in_class_endp_req();
		break;

	case (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_INTERFACE):
		if (REQ_SetInterface == Request) {
			usb_handle_set_intf();
		}
		else if(REQ_SetFeature == Request) {//todo
			g_stall = 1;
		    usb_g_feature = 1;
				}
		break;
	case (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE):
		if (REQ_SetConfiguration == Request) {
			usb_g_config = (control_request.Value & 0xff) ? 1 : 0;
#if USB_DESCRIPTOR_MY_SELF
			if (usb_g_config) {
				usb_mark_configured();
			} else {
				connect_ok = 0;
			}
#endif
		}
		break;

	default:
		g_stall = 1;
		break;
	}

	return;
}


void usb_handle_ctl_ep_setup(void)
{
#if(MCU_CORE_B80)
	reg_usb_sups_cyc_cali=0x38;
#endif
	usbhw_reset_ctrl_ep_ptr();
	control_request.RequestType = usbhw_read_ctrl_ep_data();
	control_request.Request = usbhw_read_ctrl_ep_data();
	control_request.Value = usbhw_read_ctrl_ep_u16();
	control_request.Index = usbhw_read_ctrl_ep_u16();
	control_request.Length = usbhw_read_ctrl_ep_u16();
#if(MCU_CORE_B80)
	usb_len_idx_h = control_request.Length;
#endif
	g_stall = 0;
	usb_handle_request(USB_IRQ_SETUP_REQ);
	if (g_stall)
	{
		usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_STALL);
	}
	else
	{
		usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_ACK);
	}
}


void usb_handle_ctl_ep_data(void) {
#if(MCU_CORE_B80)
	reg_usb_sups_cyc_cali=0x38;
#endif
	usbhw_reset_ctrl_ep_ptr();
	g_stall = 0;
	usb_handle_request(USB_IRQ_DATA_REQ);
	if (g_stall)
	{
		usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_STALL);
	}
#if(MCU_CORE_B80)
	else if(!(control_request.RequestType & 0x80) &&
	        (usb_len_idx_s > 0) && (usb_len_idx_s % 8 == 0) &&
	        (usb_len_idx_0 == 0) && (usb_len_idx_s != usb_len_idx_h))
	{
		reg_usb_sups_cyc_cali=0x18;
		usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_ACK);
	}
#endif
	else
	{
		usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_ACK);
	}
}



void usb_handle_ctl_ep_status() {
#if(MCU_CORE_B80)
	reg_usb_sups_cyc_cali=0x38;
#endif
	if (g_stall)
	{
		usbhw_write_ctrl_ep_ctrl(FLD_EP_STA_STALL);
	}
	else
	{
		usbhw_write_ctrl_ep_ctrl(FLD_EP_STA_ACK);
	}
}

u8 usb_has_suspend_irq = 0;
u8  usb_just_wakeup_from_suspend = 1;
extern u8 rf_channel;
int usb_suspend_check(void){
	return 0;
}

#if(0)
void usb_resume_host(void)
{
#if (MCU_CORE_TYPE == MCU_CORE_3520)
#else
	reg_wakeup_en = FLD_WAKEUP_SRC_USB_RESM;
	reg_wakeup_en = 0;
#endif
	sleep_us(6000);
}
#endif
u8 edp_toggle[8];


void usb_handle_irq(void) {
	u32 irq = usbhw_get_ctrl_ep_irq();
	if (irq & FLD_CTRL_EP_IRQ_SETUP) {
		usbhw_clr_ctrl_ep_irq(FLD_CTRL_EP_IRQ_SETUP);
		usb_handle_ctl_ep_setup();
	//printf("\r\n FLD_CTRL_EP_IRQ_SETUP");
	}
	if (irq & FLD_CTRL_EP_IRQ_DATA) {
		usbhw_clr_ctrl_ep_irq(FLD_CTRL_EP_IRQ_DATA);
		usb_handle_ctl_ep_data();
		//printf("\r\n FLD_CTRL_EP_IRQ_DATA");
	}
	if(usb_g_feature==1){ sleep_us(1000);usb_g_feature=0;}//for Chapter 8 test
	if (irq & FLD_CTRL_EP_IRQ_STA) {
		usbhw_clr_ctrl_ep_irq(FLD_CTRL_EP_IRQ_STA);
		usb_handle_ctl_ep_status();
		//printf("\r\n FLD_CTRL_EP_IRQ_STA");
	}

#if(USB_CDC_ENABLE)
	usb_cdc_irq_data_process();
#endif

#if(USB_MIC_ENABLE||USB_SPEAKER_ENABLE)
	usb_audio_irq_data_process();
#endif


#if(USB_SOMATIC_ENABLE)
	u8 ep_irq = reg_usb_irq;							// data irq
	
	if(ep_irq & BIT((USB_EDP_SOMATIC_OUT & 0x07))){
		reg_usb_irq = BIT((USB_EDP_SOMATIC_OUT & 0x07));		// clear ime
		usbhw_reset_ep_ptr(USB_EDP_SOMATIC_OUT);

		ev_emit_event_syn(EV_USB_OUT_DATA, (void*)irq);

		usbhw_data_ep_ack(USB_EDP_SOMATIC_OUT);
	}
#endif

#if(USB_MOUSE_ENABLE)
	extern void usbmouse_report_frame(void);
	extern void usbmouse_release_check(void);
	//usbmouse_report_frame();
	//usbmouse_release_check();
#endif

#if(USB_KEYBOARD_ENABLE)
	extern void usbkb_report_frame(void);
	extern void usbkb_release_check(void);
	//usbkb_report_frame();
	//usbkb_release_check();
#endif
	//usb_hid_report_fifo_proc();

	if (reg_irq_src & FLD_IRQ_USB_RST_EN)
	{	printf("\r\n reset1");
		connect_ok = 0;
		usb_g_config = 0;
		usb_configure_ok = 0;
		usb_mode_start_tick = clock_time() | 1;
#if USB_DESCRIPTOR_MY_SELF
			extern u8 usb_data_eps_ready;
			extern u8 web_intf_eps_ready;
			usb_data_eps_ready = 0;
			web_intf_eps_ready = 0;
			usb_web_handshake_reset();
			usb_mouse_report_proto = 1;
			usb_hid_web_rx_reset();
			usb_fifo_reset_aaa();
			write_reg8(0x10e, 0);
#else
			usb_mouse_report_proto = 1;
#endif
			reg_irq_src |= FLD_IRQ_USB_RST_EN;					//Clear USB reset flag
			for (int i=0; i<8; i++) {
				reg_usb_ep_ctrl(i) = 0;
				edp_toggle[i]=0;
	}

    #if (USB_CDC_ENABLE)
    //must add ,if endpoint is reset and ack is not set,CDC out_irq will not be generated.
    //The packet capture phenomenon of the USB analyzer is: device does not return ack.(kaixin modify,2020-01-15)
    usbhw_data_ep_ack(USB_EDP_CDC_OUT);
    #endif
    }
#if 0
   if(IRQ_USB_PWDN_ENABLE && (reg_irq_src & FLD_IRQ_USB_PWDN_EN)){
   //	printf("\r\n power down");
		usb_has_suspend_irq = 1;
	}else{
		usb_has_suspend_irq = 0;
	}

#if (!USB_DESCRIPTER_CONFIGURATION_FOR_KM_DONGLE)
	if ((reg_irq_src & FLD_IRQ_USB_PWDN_EN))
	{
		return;
	}
#endif
#endif

#if 0//(USB_SOMATIC_ENABLE)	
	
	if(irq & BIT((USB_EDP_SPP_OUT & 0x07))){
		reg_usb_irq = BIT((USB_EDP_SPP_OUT & 0x07));
        
		u8 len = reg_usb_ep_ptr(USB_EDP_SPP_OUT & 0x07);
		usbhw_reset_ep_ptr(USB_EDP_SPP_OUT);
		

		u8 buf[SPP_REORT_LEN+1];

		
		for (u8 i=0; i<len; i++)
		{
		     buf[i] = reg_usb_ep_dat(USB_EDP_SPP_OUT);
		}
		usbhw_data_ep_ack(USB_EDP_SPP_OUT);
		tick_pc_breath=clock_time()|1;
		spp_push_data((USB_PC_DEVICE*)&buf[1],(DEVICE_PC_FIFO_DATA_S *)&from_pc_dat);
		
		
	}
#endif


}

void usb_init_interrupt(void)
{
	usbhw_enable_manual_interrupt(FLD_CTRL_EP_AUTO_STD | FLD_CTRL_EP_AUTO_DESC);
#if(MCU_CORE_B87||MCU_CORE_B80)
	usbhw_set_eps_en(FLD_USB_EDP8_EN|FLD_USB_EDP1_EN|FLD_USB_EDP2_EN|FLD_USB_EDP3_EN|FLD_USB_EDP4_EN|FLD_USB_EDP5_EN|FLD_USB_EDP6_EN|FLD_USB_EDP7_EN);
#endif

	/* set for chapter 8 test */
	//write_reg8(0x10e,0x04);//( only mouse EDP)
	 // write_reg8(0x10e,0xc6);//( audio, mouse and keyboard EDP)
     // write_reg8(0x10e,0xe7);//( printer, audio, mouse and keyboard EDP)
}

void usb_init() {

#if(USB_MOUSE_ENABLE)
    extern void usbmouse_init();
    usbmouse_init();
#endif
#if(USB_KEYBOARD_ENABLE)
    extern void usbkb_init();
    usbkb_init();
#endif

	usb_init_interrupt();
#if FLOW_NO_OS

#else
	usb_handle_irq();
#endif
}


#if(USB_CDC_ENABLE)

void usb_cdc_irq_data_process(void)
{
	unsigned char irq = usbhw_get_eps_irq();

	if(irq & FLD_USB_EDP4_IRQ ){
		usbhw_clr_eps_irq(FLD_USB_EDP4_IRQ);
		usbhw_reset_ep_ptr(USB_EDP_CDC_IN);
		usb_cdc_tx_cnt++;
		//printf("CDC DATA INPUT IRQ:%d ! \r\n",usb_cdc_tx_cnt); //USB HOST INPUT
	}
	//CDC host-to-device data Output IRQ
	if(irq & FLD_USB_EDP5_IRQ){
		reg_usb_irq=FLD_USB_EDP5_IRQ;//usbhw_clr_eps_irq(FLD_USB_EDP5_IRQ);
		//printf("CDC DATA OUTPUT IRQ! \r\n"); //USB HOST OUTPUT
		g_stall = 0;
		//receive data from usb host
		usb_cdc_rx_data_from_host(usb_cdc_data);
		printf("USB RX CDC data:");
		for (int i = 0; i < 8; i++) {
			printf(" %1x", usb_cdc_data[i]);
		}
		printf("\r\n");
		if(g_stall)
		{
			usbhw_data_ep_stall(USB_EDP_CDC_OUT);
			//printf(" STALL! \r\n");
		}
		else
		{
			usbhw_data_ep_ack(USB_EDP_CDC_OUT);
			//printf(" ACK \r\n");
		}
	}
}
   #endif

#endif
