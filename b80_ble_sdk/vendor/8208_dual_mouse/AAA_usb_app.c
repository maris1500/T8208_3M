/********************************************************************************************************
 * @file     AAA_app.c
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
#include "../../stack/ble/service/ota/ota.h"

#if USB_MODE_ENABLE

#include "main.h"

//typedef void (*callback_rx_func)(u8 *);
//u8 * raw_pkt=NULL;
volatile u32 kb_tick=0;
volatile u32 ms_tick=0;
u8 mouse_not_release=0;
//callback_rx_func p_post = NULL;
USB_FIFO_DATA_S  usb_fifo_aaa;
u32 tick_suspend_interval;
volatile u8 has_consume_key_press=0;
u8 usb_device_status;
u8 need_enter_suspend_flag=0;
//u8 allow_send_to_usb_flag=1;
u8 bin_crc[4];
volatile u8 has_normal_key_press=0;
volatile u8 has_system_key_press=0;
static mouse_data_t  mouse_last_data={0,0,0,0};
static u8 consume_last_data[2]={0,0};
static u8 sys_last_data=0;
static u8 normal_last_data[8]={0,0,0,0,0,0,0,0};

//callback_rx_func p_keyboard_data_handle_post = NULL;
//volatile u8 to_usb_keyboard_data[8];
//extern void key_data_handle_aaa_24g(void);
//extern const u16 consumer_list[];
//callback_rx_func p_mouse_data_handle_post = NULL;
//volatile u8 to_usb_mouse_data[8];
//_attribute_data_retention_user u32  usb_configuretimes =0x0;
u8 usb_configure_ok =0;
u8 usb_has_judge =0;

extern u8 connect_ok;

#if 0
	extern unsigned int mouse_get_report_rate(void);
#endif

#if OTA_ENABLE_AAA
	//bool fw_start_addr_offset=0;	//0-FW_ADDR:0X00000  1-FW_ADDR:0x10000
	#define FW_SECTOR_LENGTH		0X20000	//128k
	#define USER_FW_ADDR			(FW_SECTOR_LENGTH - ota_program_offset)
	#define START_UP_FLAG			(0x544c4e4b)
	#define MCU_RAM_START_ADDR		(0x840000)  // for 825x: 0x840000, don't change.

	OUTPUT_DEV_INFO_AAA output_dev_info =
	{
		0,			//u32 bin_crc
		0x01020304, //u32 fw version
	};

	static u32 notify_rsp_tick = 0;
	u32 blt_ota_start_tick = 0;
	u8 usb_start_flag = 0;

	MYFIFO_INIT(usb_to_pc_data, 80, 4);

	int notify_rsp_buf2hci(u8 *data, u16 length);
	void notify_rsp_update(void);

	extern u8 ui_ota_is_working;
#endif

#if OTA_ENABLE_AAA
	unsigned short crc16_user (unsigned char *pD, int len)
	{
		unsigned short crc16_poly1[2] = {0, 0xa001};
		unsigned short crc = 0xffff;
		int i,j;
	
		for (j=len; j>0; j--)
		{
			unsigned char ds = *pD++;
			for (i=0; i<8; i++)
			{
				crc = (crc >> 1) ^ crc16_poly1[(crc ^ ds ) & 1];
				ds = ds >> 1;
			}
		}
	
		return crc;
	}

	int usb_ota_resut(u8 result)
	{
		usb_data_t p;
		int count = 0;
		u32 tick = 0;
		int ret = 0;

		tick = clock_time();

		/* Fill response data for send to PC */
		p.report_id = OTA_REPORT_ID;
		p.opcode = 0x02;
		p.length = 3;
		p.dat[0] = 0x06;
		p.dat[1] = 0xff;
		p.dat[2] = result;

	    while(1)
		{
	    	/* USB IRQ process */
			usb_handle_irq();

			if (clock_time_exceed(tick, 50000))
			{
				tick = clock_time();

				if (count == 0)
				{
					/* send response data to PC */
					ret = notify_rsp_buf2hci(&p.report_id, USB_OTA_LENGTH);

					if (ret)
					{ //send success
						count=1;
					}
				}
				else
				{
					break;
				}
			}
		}

	    printf("---usb_ota_result=0x%01x.\r\n",result);

		//app_debug_ota_result(result);

		analog_write(SYS_DEEP_ANA_REG, OTA_STOP_AAA); //set deep_flag = OTA_STOP_AAA

		start_reboot(); //reboot

		return ret;
	}

	/**
	 * @brief	USB OTA polling
	 * @param	none
	 * @return	none
	 */
	void usb_update_loop()
	{
		//static u32 dbg_m_loop;
		//dbg_m_loop ++;

		/* if OTA is working but already overtime(120s), set OTA result = OTA_TIMEOUT */
		if ((usb_start_flag == 1) && clock_time_exceed(blt_ota_start_tick , 120000000))
		{
			usb_ota_resut(OTA_TIMEOUT); //means OTA failed
		}

		//proc_host();

		notify_rsp_update(); //check weather notify response data to PC
	}

	/**
	 * @brief	load next firmware to ram and run
	 * @param	addr_load�� firmware start address
	 * @param	ramcode_size�� firmware ramcode size
	 * @return	none
	 */
	void load_next_firmware(u32 addr_load, u32 ramcode_size)
	{
		//func_test();	// just for compile error indication when over flow, but make no sense now.

		irq_disable();	// must, can't enter irq, because cstartup will be changed.

		// copy ramcode of next firmware.
		#if 0
			boot_load_memcpy4((u8 *)MCU_RAM_START_ADDR, (u8 *)addr_load, ramcode_size);	// have been sure that ramcode_size is 4 bytes align.
		#else
			flash_read_page(addr_load, ramcode_size, (u8 *)MCU_RAM_START_ADDR); // if use flash read, all flash API need to be located at "_attribute_no_over_written_code_"
		#endif

		printf("---addr_load=0x%03x,ramcode_size=%d.\n",addr_load,ramcode_size);

		//while(1);

	#if 0 //no need restart
		// jump to next image
		WRITE_REG8(0x602, 0x88);	// reboot from RAM, and the IO setting will remain unchanged, such as GPIO's function of DP.
	    while(1){// have been reboot before, just make sure not go ahead.
	        //static volatile u32 boot_load_err;boot_load_err++; // can not use global variable here, incase there is some delay for reboot.
	    };
	#endif
	}

	/**
	 * @brief	get firmware information
	 * @param	none
	 * @param	none
	 * @return	none
	 */
	u8 run_app_code(void)
	{
		irq_disable();  // must, can't enter irq, because cstartup will be changed.

		u32 startup_flag = 0;

		/* get startup_flag value*/
		flash_read_page (8, sizeof(startup_flag), (u8 *)&startup_flag);

		if (START_UP_FLAG == startup_flag)
		{ //means firmware address in block A(0x00000), so OTA address in block B(0x10000)
			//fw_start_addr_offset = 1;
			ota_program_offset = FW_SECTOR_LENGTH;
		}

		//ota_program_offset = ota_program_bootAddr = USER_OTA_ADDR;

		printf("***********************************************************\r\n");
		printf("****** firmware_addr = 0x%03x, ota_addr = 0x%03x ******\r\n",USER_FW_ADDR,ota_program_offset);
		printf("***********************************************************\r\n");

		//crc32_init(0x04C11DB7, AA_crc_table);  //1600us

		u32 bin_size = 0;
		flash_read_page(USER_FW_ADDR + 0x18, 4, (u8 *)&bin_size);
		flash_read_page(USER_FW_ADDR + (bin_size - 4), 4, (u8 *)&output_dev_info.bin_crc);
		//flash_read_page(offset + 2, 4, (u8 *)&output_dev_info.fw_version);

		printf("---bin_crc=0x%04x.\r\n",output_dev_info.bin_crc);

		return 1;
	}

	void notify_rsp_update(void)
	{
		if (notify_rsp_tick && clock_time_exceed(notify_rsp_tick, 600))
		{ //It runs every 600us
			u8 *p = my_fifo_get(&usb_to_pc_data);
			
			if (p)
			{ //fifo has data
				int length = p[0] + (p[1]<<8);
				
				if (notify_rsp_buf2hci(&p[2], length))
				{ //notify success
					my_fifo_pop(&usb_to_pc_data); //pop fifo
					notify_rsp_tick = 0; //clear time_tick
				}
			}
			else
			{ //fifo empty
				notify_rsp_tick = 0; //clear time_tick
			}	
		}
	}

	/**
	 * @brief	initial notify FIFO
	 * @param	none
	 * @return	none
	 */
	void notify_rsp_buf_init()
	{
	    //memset(notify_rsp_buf, 0, sizeof(notify_rsp_buf));
	    //notify_rsp_buf_wptr = notify_rsp_buf_rptr = 0;
	    my_fifo_reset(&usb_to_pc_data);
	}

	/**
	 * @brief 	notify data to PC
	 * @param	data
	 * @param	length
	 * @return	none
	 */
	int notify_rsp_buf2hci(u8 *data, u16 length)
	{
		if (reg_irq_src & FLD_IRQ_USB_PWDN_EN)
		{
			//user_resume_host();
			reg_usb_ep_ctrl(USB_EDP_SPP_IN) = 0;
			return 1;
		}

		if (!reg_usb_host_conn)
		{
			reg_usb_ep_ctrl(USB_EDP_SPP_IN) = 0;
			return 1;
		}

		if (usbhw_is_ep_busy(USB_EDP_SPP_IN))
		{
			return 0;
		}

		reg_usb_ep_ptr(USB_EDP_SPP_IN) = 0;

		//printf("tx=0x");
		printf("USB send to PC len=%d data:", length);
		for (u8 i=0; i<length; i++)
		{
			printf(" %1x", data[i]);
			reg_usb_ep_dat(USB_EDP_SPP_IN) = data[i];
		}
		printf("\r\n");

		usbhw_data_ep_ack(USB_EDP_SPP_IN);

		return 1;
	}

	/**
	 * @brief 	enter OTA mode call back process
	 * @param	none
	 * @return	none
	 */
	void app_enter_usb_ota_mode(void)
	{
	#if MODULE_WATCHDOG_ENABLE
		wd_stop(); //stop normal watch dog
	#endif

	#if (MODULE_32K_WATCHDOG_ENABLE)
		wd_32k_stop(); //stop 32K watch dog
	#endif

	#if (FLASH_LOCK_ENABLE_AAA && (_CHIP_IS_OTP_==0))
		flash_lock_handle(FLASH_LOCK_NONE_BLOCK); //unlock flash
	#endif

		ui_ota_is_working = 1; //set OTA working flag

	#if (BLT_APP_LED_ENABLE && BLE_OTA_LED_DEBUG)
		gpio_write(PIN_BLE_LED, 1);
	#endif

		printf("---start ota\r\n");

		blt_ota_start_tick = clock_time();  //mark time

		usb_start_flag = 1; //set OTA start flag

		//bls_ota_setTimeout(120 * 1000 * 1000); //set OTA timeout  15 seconds
	}

	/**
	 * @brief 	End OTA mode call back process
	 * @param	result
	 * @return	none
	 */
	void app_debug_ota_result(int result)
	{
	    irq_disable();

	    //flash_erase_sector(CFG_DEVICE_MODE_ADDR);

	#if MODULE_WATCHDOG_ENABLE
	    wd_stop(); //stop normal watch dog
	#endif

	#if (MODULE_32K_WATCHDOG_ENABLE)
		wd_32k_stop(); //stop 32K watch dog
	#endif
	}

	u8 usb_ota_write(usb_data_t *p_usb)
	{
		static u16 ota_index = 0;
		static u16 start_index = 0;
		static u32 flash_write_addr = 0;
		static u8  first_data_buf[16];
		static u32 fw_size = 0;
		static u8  ota_error_flag = 0;
		ota_data_st *pd = (ota_data_st*)&p_usb->dat[0];

		if ((pd->cmd == CMD_OTA_START) && (p_usb->length == 2))
		{ //receive OTA Start CMD
			blt_ota_start_tick = clock_time();  //mark time
			usb_start_flag = 1;  //set OTA start flag
			notify_rsp_buf_init(); //initial notify FIFO
		#if MODULE_WATCHDOG_ENABLE
			wd_stop(); //stop normal watch dog
		#endif
		#if (MODULE_32K_WATCHDOG_ENABLE)
			wd_32k_stop(); //stop 32K watch dog
		#endif
			ota_index = 0;
			flash_write_addr = 0;
			start_index = 0;
			fw_size = 0;
			//fw_check_value = 0;
			//fw_cal_crc = 0xffffffff;
			ota_error_flag = OTA_SUCCESS;
			flash_erase_sector(ota_program_offset); //erase OTA start address
			app_enter_usb_ota_mode(); //enter OTA mode call back process

			printf("******** usb_ota_start at=0x%03x ********\r\n",ota_program_offset);

			return ota_error_flag;
		}
		else if (ota_error_flag == OTA_SUCCESS)
		{ //OTA working right
			if ((pd->cmd == CMD_OTA_END) && (p_usb->length == 6))
			{ //receive OTA End CMD
				printf("---usb_ota_end\r\n");

				/* check first packet data is right or not */
				u32 *telink_mark = (u32*)&first_data_buf[8];
				if (telink_mark[0] != 0x544c4e4b)
				{ //first packet data is wrong, return error flag
					ota_error_flag = OTA_FIRMWARE_MARK_ERR;
					return ota_error_flag;
				}

				u32 real_bin_size = 0;
				//real_bin_size = fw_size-4;
				real_bin_size = fw_size;
				printf("---real_bin_size=%d_%d, fw_size=%d.\n", real_bin_size, (start_index*16), fw_size);

				if ((real_bin_size%16) == 0)
				{ //if real_bin_size is a multiple of 16
					printf("---1 real_bin_size=%d_%d.\n", real_bin_size, ((start_index+1)*16));

					if (real_bin_size != ((start_index+1)*16)) //
					{ //real_bin_size wrong, return error flag
						ota_error_flag = OTA_FW_SIZE_ERR;
						return ota_error_flag;
					}
				}
				else
				{ //if real_bin_size is not a multiple of 16
					printf("---2 real_bin_size=%d_%d.\n", (real_bin_size-(real_bin_size%16)), ((start_index+1)*16));

					if ((real_bin_size - (real_bin_size%16)) != (start_index * 16))
					{ //real_bin_size wrong, return error flag
						ota_error_flag = OTA_FW_SIZE_ERR;
						return ota_error_flag;
					}
				}

				//printf("******** write first packet to OTA addr:0x%03x ********\n",ota_program_offset);
				flash_write_page(ota_program_offset,16,first_data_buf);

				/* read back first packet to check weather write is right or not */
				u8 read_flash_buf[16];
				flash_read_page(ota_program_offset,16, read_flash_buf);

				if (memcmp(read_flash_buf, first_data_buf, 16))
				{  //do not equal, erase OTA start address data, return error flag
					flash_erase_sector(ota_program_offset);
					ota_error_flag = OTA_WRITE_FLASH_ERR;
					return ota_error_flag;
				}

				u32 flag = 0;
				/* check OTA data */
				u8 ret = flash_fw_check(0xffffffff,ota_program_offset);

				if (ret==0)
				{ //OTA data is right, run OTA success result
					//extern u32 fw_crc_init;
					//printf("---fw_crc_init=0x%04x\r\n",fw_crc_init);

					printf("---usb ota success, erase Firmware\r\n");
					flash_write_page(USER_FW_ADDR + 0x08, 4, (u8 *)&flag);	//set invalid flag to firmware block
					usb_ota_resut(OTA_SUCCESS);
				}
				else
				{ //OTA data is wrong, return error flag

					printf("---usb ota fail, erase OTA\r\n");
					flash_erase_sector(ota_program_offset);
					ota_error_flag = OTA_FW_CHECK_ERR;
					return ota_error_flag;

				}
			}
			else
			{ //receive OTA normal data, normal data lengt = 2byte CMD(packet's count) + 16byte valid data + 2byte crc16 = 20byte
				//check data packet length is 20 or not
				if ((p_usb->length % 20) != 0)
				{
					ota_error_flag = OTA_PDU_LEN_ERR;
					return ota_error_flag;
				}

				//get data packet's count
				u8 cnt = p_usb->length/20;

				//check every packet is right or not
				for(u8 i=0; i<cnt; i++)
				{
					pd = (ota_data_st*)&p_usb->dat[20*i];
					//printf("pd_crc=%02x, user_crc=0x%02x.\n", pd->crc, crc16_user((u8*)&pd->cmd,18));

					if (crc16_user((u8*)&pd->cmd, 18) == pd->crc)
					{ //The calculated crc16 is equal to the crc16 in the packet, so the data is right
						if (pd->cmd == 0x0000)
						{ //first packet
							memcpy(first_data_buf,pd->buf,16); //save first packet data
							start_index = 0; //clear packet count
						}
						else
						{ //else packet
							if ((start_index+1) != pd->cmd)
							{ //if received packet count plus 1 is not equal to CMD(packet's count), means packet count is wrong, return wrong flag
								ota_error_flag = OTA_DATA_PACKET_SEQ_ERR;
								return ota_error_flag;
							}

							start_index = pd->cmd; //save new received packet
							//printf("---start_index=%d.\n",start_index);

							if (pd->cmd == 0x0001) //second packet
							{
								/* get firmware size */
								fw_size = pd->buf[8] | (pd->buf[9] <<8) |(pd->buf[10]<<16) | (pd->buf[11]<<24);
								printf("---fw_size=%d.\n",fw_size);

								if (fw_size > FW_SECTOR_LENGTH)//128k
								{ //if fw_size more than OTA block, return error flag
									ota_error_flag = OTA_FW_SIZE_ERR;
									return ota_error_flag;
								}
							}

							if (fw_size < flash_write_addr)
							{ //if fw_size less then received data, return error flag
								ota_error_flag = OTA_FW_SIZE_ERR;
								return ota_error_flag;
							}

							if (ota_error_flag == OTA_SUCCESS)
							{
								if ((flash_write_addr % 4096) == 0)
								{ //if received data is a multiple of 4096, erase next flash block
									flash_erase_sector(flash_write_addr + ota_program_offset);
								}

								//write current packet's data to flash
								flash_write_page(flash_write_addr+ota_program_offset, 16, pd->buf);
							}

						}

						//received data plus 16
						flash_write_addr += 16;
					}
					else
					{
						//The calculated crc16 is not equal to the crc16 in the packet, return error flag
						ota_error_flag = OTA_DATA_CRC_ERR;
						return ota_error_flag;
					}
				}
			}
		}

		return ota_error_flag;
	}

	void usb_data_handle(usb_data_t *p,u16 length)
	{
		if ((p->report_id == OTA_REPORT_ID) && (length == USB_OTA_LENGTH))
		{ //if received USB data is OTA data
			if ((p->opcode == 0x01) && (p->length == 0))
			{ //PC get firmware version,
				p->length = 8;
				memcpy(&p->dat[0], (u8 *)&output_dev_info.fw_version, 4);
				memcpy(&p->dat[4], (u8 *)&output_dev_info.bin_crc, 4);
			}
			else if(p->opcode == 0x02)
			{ //PC send OTA data, check OTA data is right or wrong, if right write data to flash ,if wrong return ota_error_flag
				u8 ota_error_flag = usb_ota_write(p);

				if(ota_error_flag)
				{ //OTA data is wrong, run OTA result call back
					usb_ota_resut(ota_error_flag);
				}
			}

			/* ACK received packet data to PC */
			if (notify_rsp_buf2hci(&p->report_id, length) == 0)
			{ //notify fail, mark time and push ACK data to fifo, wait for next notify, until notify success or OTA timeout fail
				notify_rsp_tick = clock_time()|1;
				my_fifo_push(&usb_to_pc_data, &p->report_id, length);
			}
		}
	}

#endif

u8 push_usb_fifo_aaa(u8 type,u8 *buf,u8 len)
{
#if 1
	/* get next buffer first address p */
	USB_DATA_S *p = (USB_DATA_S*)usb_fifo_aaa.fifo[usb_fifo_aaa.wptr & (USB_FIFO_NUM-1)];

	p->type = type; //save type
	memcpy(p->buf, buf, len); //save buffer

	usb_fifo_aaa.wptr++; //skip to next buffer

	/* get used fifo's num */
	int fifo_use = (usb_fifo_aaa.wptr - usb_fifo_aaa.rptr) & (USB_FIFO_NUM*2-1);

	if (fifo_use > USB_FIFO_NUM)
	{ //used fifo's num overflow
		usb_fifo_aaa.rptr++; //overlap older data
	}
#endif

	return 1;
}

#if 0
void call_mouse_data_handle(u8 *buf)
{
    ms_tick=clock_time();
    mouse_data_t *ms_dat=(mouse_data_t*)&buf[0];

    check_first_normal_data();

    if(ms_dat->x||ms_dat->y)
    {
		x_smoother=ms_dat->x;
		y_smoother=ms_dat->y;
		adaptive_smoother();
		ms_dat->x=x_smoother;
		ms_dat->y=y_smoother;
    }
	if(memcmp(&mouse_last_data.btn,&ms_dat->btn,sizeof(mouse_data_t))||(ms_dat->x!=0)||(ms_dat->y!=0)||(ms_dat->wheel!=0))
	{
		memcpy(&mouse_last_data.btn, &ms_dat->btn,sizeof(mouse_data_t));
		push_usb_fifo_aaa(MOUSE_DATA_TYPE,&ms_dat->btn,sizeof(mouse_data_t));
	}
}
#endif

/**
 * @brief	special_key_wake_up_pc
 * @param	none
 * @return	none
 */
void special_key_wake_up_pc()
{
#if 1
	/* for example  fn conusmer key  wake up window pc
	 * The disadvantage is that an extra L is sent_ ctrl
	 * the user decides whether to need this function */
	if (usb_device_status == USB_DEVICE_CHECK_PC_SLEEP)
    {
		if (need_enter_suspend_flag)
		{
		  	u8 x[8]={0,0,0,0,0,0,0,0};
		#if 0
			x[0]=1;//l_ctrl
		  	push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, x, 8);
		  	x[0]=0;
		  	push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, x, 8);
		#else
			x[1]=0;
			push_usb_fifo_aaa(MOUSE_DATA_TYPE, x, 6);
		#endif
		}
    }
#endif
}


u32 need_enter_suspend_tick = 0;
u8  usb_device_status_last;

u8 kb_data_handle(u8 *buf)
{
	u16 consume_key=0;
	u8 system_key=0;
	u8 normal_key[8]={0,0,0,0,0,0,0,0};
	u8 nk_offset=2;
	u8 len=0;

	for(u8 i=0;i<6;i++)
	{
	if(buf[i]!=0)
		len++;
	else
		break;
	}

	if(len==0)//release
	{
		has_normal_key_press=0;
		has_consume_key_press=0;
		has_system_key_press=0;

//		if (need_enter_suspend_tick == 0)
//		{
//			special_key_wake_up_pc(); //for fn
//		}
	}
	else
	{
		for(u8 i=0;i<len;i++)
		{
			if((buf[i]>=0xa0)&&(buf[i]<=0xa2))
			{
				system_key=1<<(buf[i]-0xa0);
				has_system_key_press=1;
			}
//			else if((buf[i]>=C_INDEX_START)&&(buf[i]<=C_INDEX_END))
//			{
//				consume_key=consumer_list[buf[i]-C_INDEX_START];
//				has_consume_key_press=1;
//			}
			else if((buf[i]>=0xe0)&&(buf[i]<=0xe7))
			{
				normal_key[0] |= (1 << (buf[i]-0xe0));
				has_normal_key_press=1;
			}
			else
			{
				normal_key[nk_offset++]=buf[i];
				has_normal_key_press=1;
			}
		}
	}

	kb_tick=clock_time();
	if(memcmp(normal_last_data, normal_key,8))
	{
		memcpy(normal_last_data, normal_key,8);
		special_key_wake_up_pc(); //for fn
		push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, normal_key, 8);
	}
	if(memcmp(consume_last_data, (u8*)&consume_key,2))
	{
		memcpy(consume_last_data, (u8*)&consume_key,2);
		special_key_wake_up_pc();//for fn
		push_usb_fifo_aaa(CONSUME_DATA_TYPE, (u8*)&consume_key, 2);
	}
	if(sys_last_data!=system_key)
	{
		sys_last_data=system_key;
		special_key_wake_up_pc();//for fn
		push_usb_fifo_aaa(SYSTEM_DATA_TYPE, &system_key, 1);
	}

	return 1;
}



//void call_keyboard_data_handle(u8 *buf)
//{
//	//check_first_normal_data();
//	kb_data_handle(buf);
//}


/**
 * @brief       This function init notify rsp buf
 * @return
 * @note
 */
//void notify_rsp_buf_init()
//{
    //memset(notify_rsp_buf, 0, sizeof(notify_rsp_buf));
    //notify_rsp_buf_wptr = notify_rsp_buf_rptr = 0;
    //my_fifo_reset(&usb_to_pc_data);//reset usb to pc data
//}


/**
 * @brief   PC set report process
 * @param	data_request
 * @param	report_id
 * @param	length
 * @return	none
 */
#if OTA_ENABLE_AAA
void app_hid_set_report_handle(u8 data_request, u8 report_id, u16 length)
{
    static u16 ep0_out_data_len = 0;
    static u8  ep0_out_data_buf[64] = {0};
    u8 bytes_to_read;
    u8 i;

    if (data_request) // PC set report happened
    {
        if (ep0_out_data_len < length)
        {
            bytes_to_read = length - ep0_out_data_len;
            if (bytes_to_read > 8) {
                bytes_to_read = 8;
            }

            for (i = 0; i < bytes_to_read; i++)
            {
                ep0_out_data_buf[ep0_out_data_len++] = usbhw_read_ctrl_ep_data();
            }

            if (ep0_out_data_len == length)
            { // Data received completely
                usb_data_t *p = (usb_data_t *)&ep0_out_data_buf[0];

                /* clear index and data length for next Set_Report transfer */
                ep0_out_data_len = 0;

                if (report_id == OTA_REPORT_ID && length == USB_OTA_LENGTH)
                {
                    /* OTA feature report ID 0x06: process OTA packet */
                    usb_data_handle(p, length);
                }
                else if (report_id == 0x05)
                {
                    /* Custom feature report ID 0x05: driver communication data */
                    printf("USB Feature Report 0x05 received, len=%d data:", length);
                    for (i = 0; i < length; i++)
                    {
                        printf(" %01x", ep0_out_data_buf[i]);
                    }
                    printf("\r\n");
                }
                else
                {
                    /* Other feature reports: just log and drop for now */
                    printf("USB Feature Report 0x%02x received, len=%d\r\n", report_id, length);
                }
            }
        }
    }
    else
    {
        /* status stage of Set_Report: reset buffer state */
        ep0_out_data_len = 0;
        printf("set_report_cmd=0x%01x,length=%d.\r\n", report_id, length);
    }
}
#endif

void usb_user_init(void)
{
#if 1
	//usb_dp_pullup_en (0);//dp 0
#if (PROJECT_ID == PID_330) || (PROJECT_ID == PID_82501E_OLED)
	// do nothing
#else
	custom_set_usb_cfg_params();
	// mouse like keyboard send data, desktop func can work
#endif
	/* enable USB manual interrupt(in auto interrupt mode, USB device would be USB printer device) */
	usb_init_interrupt();

	#if USB_DESCRIPTOR_MY_SELF
		#if OTA_ENABLE_AAA
			usb_register_set_report(app_hid_set_report_handle); //register set report

			write_reg8(0x10e,(1<<USB_EDP_MOUSE)|(1<<USB_EDP_KEYBOARD_IN)|(1<<USB_EDP_SPP_IN));

			notify_rsp_buf_init(); //initial notify FIFO
		#else
			write_reg8(0x10e,(1<<USB_EDP_MOUSE)|(1<<USB_EDP_KEYBOARD_IN));
		#endif
	#endif

	//deepsleep_dp_dm_gpio_low_wake_enable(); // TODO: check B80 have not this interface

	/* enable USB DP pull up 1.5k */
	usb_set_pin_en();
#endif
}

#if 0
void timer1_init_aaa()
{
#if (MY_CHANNEL_ENABLE==0)
	timer1_set_mode(TIMER_MODE_SYSCLK,0,8* CLOCK_SYS_CLOCK_1MS);
	timer_start(TIMER1);
#endif
}
#endif

//extern void set_deepsleep_wakeup_level(u8 en);
void mcu_enter_suspend()
{
	if (clock_time_exceed (tick_suspend_interval, 40000))
	{
		u8 r = irq_disable();

		cpu_sleep_wakeup (SUSPEND_MODE, PM_WAKEUP_TIMER|PM_WAKEUP_CORE_USB, clock_time() + 200*CLOCK_16M_SYS_TIMER_CLK_1MS);

		//rf_user_init();

		//timer1_init_aaa();

		irq_restore(r);

		tick_suspend_interval = clock_time()|1;

		irq_enable();
	}
}


#if 1
void usb_host_status_check(void)
{
	static u8 current_status = 0xff;
	static u8 last_status = 0xff;
	static u32 status_change_tick;

	if (reg_usb_host_conn & BIT(7))
	{
	#if 0
		if ((reg_usb_mdev & BIT(2)) && (reg_irq_src & FLD_IRQ_USB_PWDN_EN))
		{
			current_status = USB_DEVICE_CHECK_PC_SLEEP;
		}
		else
		{
			current_status = USB_DEVICE_CONNECT_PC;
		}
	#else
		if ((reg_irq_src & FLD_IRQ_USB_PWDN_EN))
		{
			if(reg_usb_mdev & BIT(2))
			{
				current_status = USB_DEVICE_CHECK_PC_SLEEP;
			}
			else
			{
				current_status = USB_DEVICE_UNPLUG;
			}
		}
		else
		{

			current_status = USB_DEVICE_CONNECT_PC;
		}
	#endif
	}
	else
	{
		current_status = USB_DEVICE_DISCONECT_PC;
	}

	if (last_status != current_status)
	{
		printf("---1 usb_device_status=%d.\r\n", current_status);

		last_status = current_status;; //save current status

		status_change_tick = clock_time()|1; //mark status change tick

		if ((current_status == USB_DEVICE_CHECK_PC_SLEEP) || (current_status == USB_DEVICE_DISCONECT_PC))
		{
			need_enter_suspend_tick = clock_time()|1; //mart enter sleep tick
		}
	}
	else if (status_change_tick && (clock_time_exceed(status_change_tick, 200*1000)))
	{ //debounce status 200ms !!!
		status_change_tick = 0; //clear status change tick

		if (usb_device_status != current_status)
		{
			printf("---2 usb_device_status=%d.\r\n", current_status);

			usb_device_status = current_status; //save current status

			/* current status is connect, last status is disconnect, maybe PC has reboot, if has normal key, notify to PC*/
	        if (usb_device_status == USB_DEVICE_CONNECT_PC)
			{
				//led_status_out(host_keyboard_status);

				//allow_send_to_usb_flag = 1;

				if (usb_device_status_last == USB_DEVICE_DISCONECT_PC)
				{
					//usb_io_printf("pc_power_on");
					if (has_normal_key_press)
					{
						push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, normal_last_data, 8);
					}
				}

				need_enter_suspend_flag = 0;
				need_enter_suspend_tick = 0;
			}
			else if (usb_device_status == USB_DEVICE_CHECK_PC_SLEEP)
			{
				need_enter_suspend_flag = 1;
				need_enter_suspend_tick = clock_time()|1;
				//led_status_out(0);
			}
			else if (usb_device_status == USB_DEVICE_DISCONECT_PC)
			{
				need_enter_suspend_flag = 1;
				need_enter_suspend_tick = clock_time()|1;
				//led_status_out(0);
			}

			usb_device_status_last = usb_device_status; //save host status
		}
	}

	if ((usb_device_status == USB_DEVICE_CHECK_PC_SLEEP) && need_enter_suspend_flag)
	{ //enter suspend
		mcu_enter_suspend();
	}
}
#endif

//
void pull_usb_data()
{
	int success=1;

	if(usb_fifo_aaa.wptr!=usb_fifo_aaa.rptr)
	{
		USB_DATA_S *p=(USB_DATA_S*)usb_fifo_aaa.fifo[usb_fifo_aaa.rptr&(USB_FIFO_NUM-1)];
        //mouse_data_t *now_ms=(mouse_data_t *)&p->buf[0];
		if(p->type==MOUSE_DATA_TYPE)
		{
			//if(memcmp_aaa(&mouse_last_data.btn, p->buf,sizeof(mouse_data_t))||(now_ms->x!=0)||(now_ms->y!=0)||(now_ms->wheel!=0))
			{
				//memcpy(&mouse_last_data.btn, p->buf,sizeof(mouse_data_t));
				success=usb_mouse_hid_report_aaa(1, p->buf, sizeof(mouse_data_t));
			}

     		if((mouse_last_data.btn==0)&&(mouse_last_data.wheel==0))
			{
				mouse_not_release=0;
			}
			else
			{
				mouse_not_release=1;
			}
		}
		else if(p->type==NORMAL_KB_DATA_TYPE)
		{
			//if(memcmp_aaa(normal_last_data, p->buf,8))
			{
				//memcpy(normal_last_data, p->buf,8);
				success=usb_keyboard_hid_report_aaa(p->buf);
			}
		}
		else if(p->type==CONSUME_DATA_TYPE)
		{
			//if(memcmp_aaa(consume_last_data, p->buf,2))
			{
				//memcpy(consume_last_data, p->buf,2);
				success=usb_mouse_hid_report_aaa(2,p->buf, 2);
			}
		}
		else if(p->type==SYSTEM_DATA_TYPE)
		{
			//if(sys_last_data!=p->buf[0])
			{
				//sys_last_data=p->buf[0];
				success=usb_mouse_hid_report_aaa(3,p->buf, 1);
			}
		}

		//
		if(success)
		{
			usb_fifo_aaa.rptr++;
		}
	}
#if 1
	u8 release_data[8] = {0,0,0,0,0,0,0,0};
	if(mouse_not_release&&clock_time_exceed(ms_tick,2000000))
	{
		if(usb_mouse_hid_report_aaa(1,release_data, 6))
		{
			mouse_not_release=0;
		}
	}
	else if(has_consume_key_press&&clock_time_exceed(kb_tick,2000000))
	{
		if(usb_mouse_hid_report_aaa(2,release_data, 2))
		{
			has_consume_key_press=0;
		}
	}
	else if(has_system_key_press&&clock_time_exceed(kb_tick,2000000))
	{
		if(usb_mouse_hid_report_aaa(3,release_data, 1))
		{
			has_system_key_press=0;
		}
	}

	if(has_normal_key_press&&clock_time_exceed(kb_tick,2000000))
	{
		if(usb_keyboard_hid_report_aaa(release_data))
		{
			has_normal_key_press=0;
		}
	}
#endif
}

u32 usb_mode_start_tick;

extern void user_reboot(u8 reason);
extern u8 get_data_report_aaa();

extern unsigned char led_batt_dpi_working_on(void);
extern void vbat_led_proc(void);
extern void batt_check_time_reload(void);

void usb_main_loop(void)
{
	static u32 tick_usb_loop;

	u32 temp = 8000;

	usb_handle_irq();//must first

	usb_host_status_check();

	if ( usb_configure_ok == 1 )
	{
		if ( 0 == connect_ok )
		{
			printf("usb ok \n");
		}
		connect_ok = 1;
	#if MOUSE_REPORT_250HZ_ENABLE
		temp = 4000;
	#else
		temp = 8000; //unit 1us
	#endif
	}

	if ( clock_time_exceed(usb_mode_start_tick, 6*1000*1000) )
	{
		usb_mode_start_tick = clock_time();

		if ( connect_ok == 0 )
		{
			usb_has_judge = USB_MODE_IN_FAILED;
			analog_write(DEEP_ANA_REG7, usb_has_judge);
			user_reboot(MODE_CHANGE_REBOOT_ANA_AAA);
		}
		else
		{
			usb_has_judge = USB_MODE_IN_SUCC;
			analog_write(DEEP_ANA_REG7, usb_has_judge);
		}
	}

	if (clock_time_exceed(tick_usb_loop, temp) )
	{
		tick_usb_loop += report_rate*CLOCK_16M_SYS_TIMER_CLK_1MS;//loop update
 		//is_new_event = get_data_report_aaa();
		get_usb_data_report_aaa();
	}

	if ( connect_ok )
	{
    	if ( led_batt_dpi_working_on() )
    	{
    		vbat_led_proc();
    		batt_check_time_reload();
    	}
	}

//	if(is_new_event)//has new event
//	{
//		reset_idle_status();//reset idle params
//	}
//	idle_status_poll();//poll idle params

	pull_usb_data();

	if (need_enter_suspend_tick && clock_time_exceed(need_enter_suspend_tick, 3200000))
	{
		need_enter_suspend_tick = 0;
	}

}


#endif


