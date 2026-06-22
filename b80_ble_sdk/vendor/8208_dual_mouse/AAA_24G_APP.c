/********************************************************************************************************
 * @file     AAA_24G_APP.c
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

#include "AAA_public_config.h"
#include "main.h"


#if G24_MODE_ENABLE

#if Dongle_Lark_A1_Flash_Enable
	unsigned char gc_lark_flash_pair = 0;
#endif

extern void mouse_task_when_rf();

#if (AES_METHOD == 1)
	u8  ack_key[12]; //AES-128 private key byte[4]~byte[15]
#endif

u8  device_status = 0;
// u8  dongle_id_valid_f = 0;
// u8  mouse_send_need_f = 0;
u32 dongle_id;

volatile u32 no_ack = 0; //host ack count

rf_packet_t	rf_pair_buf =
{
    20,	// dma_len
    19,	// rf_len
};
pair_data_t *p_pair_dat = (pair_data_t*)&rf_pair_buf.dat[0]; //2.4G paired data packet

rf_packet_t rf_km_buf =
{
#if DATA_3_CHOOSE_1_ENABLE
	55,	// dma_len
    54,	// rf_len
#elif WEB_HID_ENABLE
    18,	// dma_len
    17,	// rf_len
#else
    14,	// dma_len
    13,	// rf_len
#endif
};
km_3_c_1_data_t *p_km_data = (km_3_c_1_data_t*)&rf_km_buf.dat[0]; //2.4G communication data packet

#if (AES_METHOD == 1)
	rf_packet_t rf_km_buf_enc =
	{
		19, // dma_len
		18, // rf_len
	};
	km_3_c_1_data_t *p_km_data_enc = (km_3_c_1_data_t*)&rf_km_buf_enc.dat[0]; //2.4G AES-128 Encrypted data packet

	u8 pub_key[16]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10}; //AES-128 public key
	u8 private_key[16] = //AES-128 Private key
	{
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
	};
#endif

u32 wakeup_next_tick = 0;
//u8  pair_success_flag = 0; //0:pair or reconnect fail, 1:pair or reconnect success
//u8  dongle_id_need_save_flag = 0; //0:no need save, 1:need save to flash


#if DATA_3_CHOOSE_1_ENABLE
/**
 * @brief	Calculate the CRC check value
 * @param	pD is the start address of the data
 * @param	len is the data length
 * @return	CRC check result
 */
_attribute_ram_code_sec_ unsigned short crc16_user (unsigned char *pD, int len)
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
#endif

/**
 * @brief	2.4G RF receiving processing
 * @param	p_rf_data, Received data
 * @return	0:Invalid received data, 1:Valid received data
 */
_attribute_ram_code_sec_ u8 rf_rx_process(rf_packet_t *p_rf_data)
{
	if (device_status <= STATE_PAIRING) //Pairing and reconnecting mode
	{
		pair_ack_data_t *pair_ack_dat_ptr = (pair_ack_data_t*)&p_rf_data->dat[0];

	#if (AES_METHOD == 1)
		if (((pair_ack_dat_ptr->cmd == PAIR_ACK_CMD) || (pair_ack_dat_ptr->cmd == RECONNECT_ACK_CMD)) && (memcmp((u8 *)&pair_ack_dat_ptr->did, (u8 *)&p_pair_dat->did, 16) == 0))
	#else
		if (((pair_ack_dat_ptr->cmd == PAIR_ACK_CMD) || (pair_ack_dat_ptr->cmd == RECONNECT_ACK_CMD)) && (pair_ack_dat_ptr->did == p_pair_dat->did))
	#endif
		{
        	dongle_id = pair_ack_dat_ptr->gid;	//get dongle id
			
			//ll_chn_mask = pair_ack_dat_ptr->chn;

		#if (AES_METHOD == 1)
			memcpy(ack_key, pair_ack_dat_ptr->key, 12);
		#endif

        	gc_mouse_sta.pair_success_flag = 1; //set pair_success_flag

			if (pair_ack_dat_ptr->cmd == PAIR_ACK_CMD)
			{ //Dongle ACK the pairing command
				gc_mouse_sta.dongle_id_need_save_flag = 1; //save dongle_id
			}
			else
			{ //Dongle ACK the reconnect command
				gc_mouse_sta.dongle_id_need_save_flag = 0; //not save dongle_id
			}

			printf("---pairing success:0x%04x\n", pair_ack_dat_ptr->gid);

        	return 1;
    	}
	}
	else //normal mode
	{
		km_ack_data_t *km_ack_dat_ptr = (km_ack_data_t*)&p_rf_data->dat[0];

		if (gc_mouse_sta.mouse_send_need_f == 2) //km ack
		{
		#if 0
			rf_packet_ack_mouse_t *tmp = (rf_packet_ack_mouse_t *)p_pkt;
			u16 crc_c = 0;
			for (u8 i=0; i<3; i++)
		    {
				crc_c = crc16_user(&tmp->dat[i].proto, sizeof(ack_mouse_rf_user_data_t)-2);
				if ((crc_c == tmp->dat[0].crc16) || (crc_c == tmp->dat[1].crc16) || (crc_c == tmp->dat[2].crc16))
				{
					if (tmp->dat[i].type == FRAME_TYPE_ACK_MOUSE)
					{
						 return 1;
					}
				
				}
		    }
		#endif

		#if (DEVICE_TYPE_INDEX==1)
			if (km_ack_dat_ptr->cmd == MOUSE_ACK_CMD)
			{
				return 1;
			}
		#endif

		#if (DEVICE_TYPE_INDEX==2)
			if (km_ack_dat_ptr->cmd == KB_ACK_CMD)
			{
				kb_led_status = km_ack_dat_ptr->host_led_status; //
				kb_led_out_aaa(kb_led_status); //
				return 1;
			}
		#endif
		}
	}

    return 0;
}

/**
 * @brief	Set the 2.4G pairing flag and restart
 * @param	none
 * @return	none
 */
void d24_start_pair()
{
	printf("---d24_start_pair.\r\n");
    set_pair_flag();
#if REBOOT_WHEN_SWITCH_MODE_AND_CHANNEL_ENABLE
    user_reboot(CLEAR_FLAG_ANA_AAA);
#else
	write_deep_ana0(CLEAR_FLAG_ANA_AAA); //write reason to analog register
	device_status = STATE_PAIRING;
	rf_rx_timeout_us = D24G_PAIR_TIMER_OUT;
	#if (AES_METHOD == 1)
		p_pair_dat->cmd = PAIR_CMD | BIT(7);
	#else
		#if Dongle_Lark_A1_Flash_Enable
			p_pair_dat->cmd = (RECONNECT_CMD | AUTO_PAIR_CMD);
			gc_lark_flash_pair = 1;
		#else
			p_pair_dat->cmd = PAIR_CMD; //Load PAIR_CMD in pairing packets
		#endif
	#endif
#endif
	adv_count = 0;
	adv_begin_tick = clock_time()|1;
}

/**
 * @brief	Initializes 2.4G related variables
 * @param	none
 * @return	none
 */
void d24_user_init()
{
	/* Replace the MAC low byte with the device type as the device id */
	u32 device_id = ((user_cfg.dev_mac&0xFFFFFF00) | DEVICE_TYPE_INDEX); //
	printf("---device_id = 0x%04x\n", device_id);
	
	/* Initializes the RF pairing packet */
#if (AES_METHOD == 1)
	p_pair_dat->cmd = PAIR_CMD | BIT(7);
#else
	p_pair_dat->cmd = PAIR_CMD;	//Load PAIR_CMD in pairing packets
#endif
	p_pair_dat->fw_version = FW_VERSION; //Load firmware version in pairing packets
	p_pair_dat->did = device_id; //Load DID in pairing packets

	/* Initializes the RF communication packet */
#if (AES_METHOD == 1)
	p_km_data->cmd = MOUSE_CMD | BIT(7);
#else
	p_km_data->cmd = MOUSE_CMD; //Load MOUSE_CMD in communication packets
#endif
	p_km_data->did = device_id;	//Load DID in communication packets

#if (AES_METHOD == 1)
	memcpy((u8*)&private_key[0], (u8*)&device_id, 4); //Use the device id as the first 4 bytes of the private key
#endif

    dongle_id = flash_dev_info.dongle_id; //get dongle_id
	printf("dongle_id:0x%04x\n", flash_dev_info.dongle_id);

	if ( (dongle_id == U32_MAX) || (dongle_id == 0) || (0x993c366c == dongle_id) )
	{
		gc_mouse_sta.dongle_id_valid_f = 0; //dongle_id is invalid
	}
	else
	{
		gc_mouse_sta.dongle_id_valid_f = 1; //dongle_id is valid
	}
	
#if  ENTER_PAIR_WHEN_NEVER_PAIRED_ENABLE
	/* If the dongle id is invalid, the device enters pairing mode */
	if (gc_mouse_sta.dongle_id_valid_f == 0)
	{
		set_pair_flag();
	}
#endif

#if (ADAPT_ALL_HW_AUTO_DRAW_IN_D24G_MODE_DEBUG==1)
	set_pair_flag();
#endif

#if Dongle_Lark_A1_Flash_Enable
	pair_flag = 0;
#endif

    if (pair_flag)
	{
		printf("---The device enters the pairing mode.\r\n");
		device_status = STATE_PAIRING;
		rf_rx_timeout_us = D24G_PAIR_TIMER_OUT;
	#if (AES_METHOD == 1)
		/* Generate 12 bytes of random number as the private key after 12 bytes */
		generateRandomNum(12, &private_key[4]);
		aes_encrypt(pub_key, private_key, (u8*)&p_pair_dat->did);
	#endif
	} 
	else if ((deep_flag == POWER_ON_ANA_AAA) || (gc_mouse_sta.dongle_id_valid_f == 0))
	{
		printf("---The device enters the sysnc mode.\r\n");

		#if Dongle_Lark_A1_Flash_Enable
			p_pair_dat->cmd = RECONNECT_CMD | AUTO_PAIR_CMD;
			gc_lark_flash_pair = 1;
		#else
			p_pair_dat->cmd = RECONNECT_CMD;
		#endif
		
		device_status = STATE_SYSNC;
		rf_rx_timeout_us = D24G_PAIR_TIMER_OUT;
	#if (AES_METHOD == 1)
		/* Generate 12 bytes of random number as the private key after 12 bytes */
		generateRandomNum(12, &private_key[4]);
		aes_encrypt(pub_key, private_key, (u8*)&p_pair_dat->did);
	#endif
    }
	else 
	{
		printf("---The device enters the normal mode.\r\n");
		device_status = STATE_NORMAL;
		rf_rx_timeout_us = D24G_COMMUNICATION_TIMER_OUT;
	#if (AES_METHOD == 1)
		/* Get the private key stored in the Flash */
		memcpy((u8*)&private_key[4], (u8*)&flash_dev_info.key[0], 12);
	#endif
	}

    wakeup_next_tick = clock_time();
    reset_idle_status();
}

////////////////////////////////////////////////////////////////////
// u8 need_suspend_flag = 0;
// u8 rptr = 0;
// u8 rx_fifo_count = 0;

/**
 * @brief	Detect the RF state and jump to the next state as needed
 * @param	none
 * @return	none
 * @note	Enter when RF data is received or receive timeout
 */
void check_rf_complet_status()
{
	rf_state = RF_IDLE_STATUS; //set rf_state to idle
	static u32 ack_miss_no = 0;
	
	if (device_ack_received) //RF receives data is valid
	{
		ack_miss_no = 0;
		no_ack = 0;

		if (device_status <= STATE_PAIRING) //Pairing and reconnecting mode
		{
			if (gc_mouse_sta.pair_success_flag)
			{
                device_status = STATE_NORMAL;
				rf_rx_timeout_us = D24G_COMMUNICATION_TIMER_OUT;

				if ( 0 == connect_ok )
				{
					led_g24_conn_status_set();
				}

                connect_ok = 1;

			#if (AES_METHOD == 1)
				if ((flash_dev_info.dongle_id != dongle_id) || (0 != memcmp(flash_dev_info.key, ack_key, 12)))
			#else
				if (flash_dev_info.dongle_id != dongle_id) //new dongle_id
			#endif
		        {
		            flash_dev_info.dongle_id = dongle_id; //save new dongle_id

				#if (AES_METHOD == 1)
					memcpy((u8*)&flash_dev_info.key[0], (u8*)&private_key[4], 12);
				#endif

					if (gc_mouse_sta.dongle_id_need_save_flag) //need to save dongle_id to flash
					{
						save_dev_info_flash();
						
					#if BLT_APP_LED_ENABLE
						// led_24g_ConnectedStatus();
					#endif
					}

					printf("g24 connect_ok \n");
		        }

				clear_pair_flag();
        		reset_idle_status();
			}

			my_fifo_reset(&fifo_km);
		}
		else //normal mode
		{
			if (gc_mouse_sta.mouse_send_need_f) // skip to next packet
			{
				my_fifo_pop(&fifo_km);
			}

			#if BLT_APP_LED_ENABLE
				if ( 0 == connect_ok )
				{
					// led_24g_ConnectedStatus();
					led_g24_conn_status_set();
				}
			#endif

			connect_ok = 1;
		}

		gc_mouse_sta.mouse_send_need_f = 0; //Send complete, clear send flag
	}
	else //RF receives data is invalid
	{
		no_ack ++;
		ack_miss_no ++;

		/* If more than two packets do not receive a response, frequency hopping is performed */
		if (ack_miss_no >= 3)
		{
			device_channel = get_next_channel_with_mask(0, device_channel);
		}

		/* If more than 1250 packets do not receive a response, disconnect from dongle */
		if (no_ack > 1250)
		{
			connect_ok = 0;
		}
	}
}

/**
 * @brief	2.4G RF status polling and send processing
 * @param	none
 * @return	none
 */
void d24g_rf_loop()
{
	u8 *ptr = 0;

	if (rf_state == RF_IDLE_STATUS)
	{
		if (device_status <= STATE_PAIRING) //Pairing and reconnecting mode
		{
			/* set pair access code, must be the same as dongle's pair access code*/
			if (user_cfg.rf_vid != U16_MAX)
				set_pair_access_code(rf_access_code_16to32(user_cfg.rf_vid)); //set customer access code
			else
				set_pair_access_code(0x39517695); //set default access code

			gc_mouse_sta.pair_success_flag = 0; //clear pair_success_flag

			if (device_status == STATE_PAIRING)
			{ //Pairing mode
			    rf_set_power_level_index(user_cfg.paring_tx_power);
			}
			else
			{ //Reconnecting mode
				rf_set_power_level_index(user_cfg.tx_power);
			}

			connect_ok = 0; //clear connect_ok

			ptr = (u8 *)&rf_pair_buf; //send rf_pair_buf packet

			gc_mouse_sta.mouse_send_need_f = 1; //Set send pairing packet flag
		}
		else //normal mode
		{
		#if (AES_METHOD == 1)
            ptr = (u8 *)&rf_km_buf_enc; //send rf_km_buf_enc packet
		#else
            ptr = (u8 *)&rf_km_buf; //send rf_km_buf packet
		#endif

            /* set communication access code, both dongle and device use dongle_id as the communication access code*/
			set_data_access_code(flash_dev_info.dongle_id);

			rf_set_power_level_index(user_cfg.tx_power);

			if (gc_mouse_sta.mouse_send_need_f == 0) //Send idle
			{ 
				u8 *p =  my_fifo_get (&fifo_km);
				
				if (p) //There's data in the FIFO
				{
					// printf("flashdongle_id = %4x \n", flash_dev_info.dongle_id);

				    gc_mouse_sta.mouse_send_need_f = 2; //Set send communication packet flag
				    
					u8 *tmp = (u8 *)&p[0];
					km_3_c_1_data_t  *km_dat1;

					u8 *src = (u8 *)&p_km_data->cmd;
					km_dat1 = (km_3_c_1_data_t*)&src[0];

				    memcpy(&km_dat1->km_dat[0], &tmp[0], 6); //copy km data from fifo

					//p_km_data->pn_no = 1;
					p_km_data->seq_no ++; //Data frame serial number plus 1

				#if (AES_METHOD == 1)
					/* AES-128 encryption is performed on the data */
					memcpy((u8 *)&p_km_data_enc->cmd, (u8 *)&p_km_data->cmd, sizeof(km_3_c_1_data_t));
					aes_encrypt(private_key, &p_km_data->pn_no, &p_km_data_enc->pn_no);
				#endif

				#if DATA_3_CHOOSE_1_ENABLE
					/* Copy valid data twice to add to the communication package */
                    u8  *km_dat2;
					u8  *km_dat3;
					km_dat2 = src + sizeof(km_3_c_1_data_t);
					km_dat3 = km_dat2 + sizeof(km_3_c_1_data_t);
					km_dat1->crc16 = crc16_user(&src[0], sizeof(km_3_c_1_data_t)-2);
					memcpy(&km_dat2[0], &src[0], sizeof(km_3_c_1_data_t));
					memcpy(&km_dat3[0], &src[0], sizeof(km_3_c_1_data_t));
				#endif
				}
			}
		}

		if (gc_mouse_sta.mouse_send_need_f) //send
		{
			rf_state = RF_TX_START_STATUS; //state = start send
			rf_set_tx_rx_off(); //must add
			device_ack_received = 0; //clear device_ack_received
            rf_stx_to_rx(ptr, rf_rx_timeout_us); //Set the sending data address and receiving timeout period
			reg_rf_irq_status = 0xffff; //clear irq_status
		}
	}
	else if (rf_state == RF_RX_END_STATUS)
	{
        irq_device_rx(); //Processing RF receiving data
		check_rf_complet_status(); //Detect the RF state and jump to the next state as needed
	}
	else if (rf_state == RF_RX_TIMEOUT_STATUS)
	{
		check_rf_complet_status(); //Detect the RF state and jump to the next state as needed
	}
}

/**
 * @brief	2.4G Task scheduling
 * @param	none
 * @return	none
 */
void ui_loop_24g()
{
	static u32 tick_loop_24g;
    u32 wheel_prepare_tick = 0;

#if (PROJECT_ID == PID_Q15)
    static unsigned short gc_check_dongle_data_counter = 0;
#endif

#if (PROJECT_ID == PID_601)
    unsigned int ui_g24_key_time = report_rate*1000 - 75;
#else
  //  unsigned int ui_g24_key_time = report_rate*1000;
#endif

	ms_data.wheel = 0; //clear wheel value

	// if ( clock_time_exceed(tick_loop_24g, ui_g24_key_time) )
	if ( clock_time_exceed(tick_loop_24g, 7*1000) )
	{
		tick_loop_24g = clock_time(); //save current time

	#if WHEEL_FUN_ENABLE_AAA
		wheel_prepare_tick = mouse_wheel_prepare_tick();//prepare wheel tick
	#endif

	#if BUTTON_FUN_ENABLE_AAA
		has_new_key_event |= btn_get_value(); //Get button value
	#endif

		idle_status_poll();//poll idle status parameters

		device_led_process();//led process

		//connect_ok = (device_status == STATE_NORMAL) ? 1 : 0;
	
		if (connect_ok == 0)//no connect
		{
			led_2p4_Adv_poll();//adv led
			adv_count_poll();//adv parameter poll
		}

		if ( led_batt_dpi_working_on() )
		{
			vbat_led_proc();
    		batt_check_time_reload();
		}
		
		led_g24_conned_succ_display();
		led_g24_conned_succ_dis_next();

	#if WHEEL_FUN_ENABLE_AAA
		has_new_key_event |= wheel_get_value(wheel_prepare_tick);
	#endif
	}
	
	 mouse_task_when_rf();	//get mouse sensor data

	if (device_status >= STATE_NORMAL)
	{

	#if (PROJECT_ID == PID_Q15)
		gc_check_dongle_data_counter++;
	#endif

		if (has_new_key_event) //has new mouse action
		{
			has_new_key_event = 0;//reset has new mouse action flag 0
			reset_idle_status();//reset idle parameters
			my_fifo_push(&fifo_km, &ms_data.btn, sizeof(mouse_data_t));//push btn data to fifo
		} 
	#if (PROJECT_ID == PID_Q15)
		else if ( (gc_check_dongle_data_counter > IDLE_REPOER_CYCLE) || (ms_data.btn))	//Idle count <3 or button action, send communication packet
	#else
		else if ((idle_count < 3) || (ms_data.btn))	//Idle count <3 or button action, send communication packet
	#endif
		{

		#if (PROJECT_ID == PID_Q15)
			gc_check_dongle_data_counter = 0;
		#endif

			u8 *p = my_fifo_get(&fifo_km);//get data
			
			if (p == 0)
			{
				my_fifo_push(&fifo_km, &ms_data.btn, sizeof(mouse_data_t));//push mouse data to fifo
			}
		}

		#if (KEY_FEATURE_BOSSKEY_ENABLE)
			if ( 0x20 == ms_data.btn || 0x40 == ms_data.btn || 0x80 == ms_data.btn )
			{
				ms_data.btn = 0x00;
			}
		#endif
	}
}

#if G24_DELAY_ENTER_SLEEP_ENABLE

	#define MOUSE_QUITE_TIMEOUT   (10*1000*1000)
	static unsigned int sg_mouse_move_time = 0;
	static unsigned char sg_mouse_move_flag = 0;

	void mouse_moved_time_clean(void)
	{
		sg_mouse_move_time = clock_time() | 0x01;
		sg_mouse_move_flag = 0;
	}

	unsigned char mouse_not_action_timeout(void)
	{
		if ( 1 == sg_mouse_move_flag )
		{
			return (1);
		}

		if ( 0 == sg_mouse_move_flag && clock_time_exceed(sg_mouse_move_time, MOUSE_QUITE_TIMEOUT) )
		{
			sg_mouse_move_flag = 1;
			return (1);
		}

		return (0);
	}
#endif

/**
 * @brief	Sleep task scheduling
 * @param	none
 * @return	none
 */
void pm_poll()
{
	u32 wake_src = 0; //wakeup source, PM_WAKEUP_TIMER or PM_WAKEUP_PAD
	u32 interval = 0; //wakeup interval, unit 1ms

	gc_mouse_sta.need_suspend_flag = 0; //clear enter suspend flag
	
	if (rf_state == RF_IDLE_STATUS)
	{
		if (device_status <= STATE_PAIRING) //Pairing and reconnecting mode
		{
		#if D24G_ADV_ENTER_DEEPSLEEP_AFTER_TIME_OUT_ENABLE_AAA
			/* if pairing or reconnecting overtime, enter deep sleep */
			if ((adv_count >= D24G_ADV_TIMER_OUT) || ((device_status == STATE_SYSNC) && (adv_count > 3) && (DEVICE_LED_BUSY == 0))) //30s
	   		{
				printf("deep 33 \n");
		   		enter_deep_aaa(DEEP_SLEEP_ANA_AAA);
	   		}
		 #endif

			/* Pairing or reconnecting under way, enter suspend */
		 	wake_src = PM_WAKEUP_TIMER; //wake up source = PM_WAKEUP_TIMER
		 	interval = 8; //wake up interval = 8ms
		    gc_mouse_sta.need_suspend_flag = 1; //set enter suspend flag
		}
		else //normal mode
		{
			if (my_fifo_get(&fifo_km) == 0) //fifo empty
			{
				if ((idle_count < 3) || DEVICE_LED_BUSY)
				{
					if ((report_rate == 4) || (report_rate == 8))
					{
						gc_mouse_sta.need_suspend_flag = 1; //set enter suspend flag
				 		wake_src = PM_WAKEUP_TIMER; //wake up source = PM_WAKEUP_TIMER
		 				interval = report_rate; //wake up interval = report_rate*1ms

		 				/* Disable pad wakeup */
						if (suspend_wake_up_enable)
				        {
						#if WHEEL_FUN_ENABLE_AAA
				            wheel_set_wakeup_level_suspend(0); //disable wheel wake up
						#endif

						#if BUTTON_FUN_ENABLE_AAA
				            btn_set_wakeup_level_suspend(0); //disable button wake up
						#endif

						#if SENSOR_FUN_ENABLE_AAA
				            sensor_set_wakeup_level_suspend(0); //disable sensor wake up
						#endif
				        }

				        suspend_wake_up_enable = 0;
					}
				}
				else
				{
					#if D24G_CONNECT_ENTER_DEEPSLEEP_AFTER_TIME_OUT_ENABLE_AAA
						/* if connecting overtime, enter deep sleep */
						if (idle_count >= D24G_CONNECT_TIME_OUT)
						{
							printf("deep 11 \n");
							enter_deep_aaa(DEEP_SLEEP_ANA_AAA);
						}
					#endif

						/* Enable pad wake up */
					 	//if (suspend_wake_up_enable == 0)
				        {
						#if WHEEL_FUN_ENABLE_AAA
				            wheel_set_wakeup_level_suspend(1); //enable wheel wake up
						#endif

						#if BUTTON_FUN_ENABLE_AAA
				            btn_set_wakeup_level_suspend(1); //enable button wake up
						#endif

						#if SENSOR_FUN_ENABLE_AAA
				            sensor_set_wakeup_level_suspend(1); //enable sensor wake up
						#endif
				        }

				        suspend_wake_up_enable = 1;

						gc_mouse_sta.need_suspend_flag = 1; //set enter suspend flag
						wake_src = PM_WAKEUP_TIMER|PM_WAKEUP_PAD; //wake up source = PM_WAKEUP_TIMER|PM_WAKEUP_PAD
					#if SENSOR_MOTION_ENABLE
						interval = 100; //wake up interval = 1000ms
					#else
						interval = 100; //wake up interval = 100ms
					#endif

					#if (PROJECT_ID == PID_0120)
						interval = 70;
					#endif

					#if (PROJECT_ID == PID_HM668)
						interval = 20;
					#endif

					#if (PROJECT_ID == PID_DMS157)
						interval = 20;
					#endif

				}
			}
			else //fifo have data waiting to be sent
			{

			#if USB_MODE_ENABLE || USB_5V_CHECK_ENABLE
				if ( USB_IN() )
				{
					if ( no_ack > 20000 )
					{
						no_ack = 20000;
					}
				}
			#endif

			#if (PROJECT_ID == PID_MS2360)
				if (no_ack > 79000)
			#elif (PROJECT_ID == PID_M388)
				if (no_ack > 14000)
			#elif (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
				if (no_ack > 11000)
			#elif (PROJECT_ID == PID_M0018) || (PROJECT_ID == PID_M0018) || (PROJECT_ID == PID_535)
				if (no_ack > 40500)
			#elif (PROJECT_ID == PID_0120)
				if (no_ack > 8000)
			#else
				if (no_ack > 88000) //no_ack(ack from dongle) > 4000
			#endif
				{
					no_ack = 98000; //set no_ack overflow

					/* Here can be modified to allow enter deep sleep without specifying the key, like the pairing button .eg*/
					if (0 == user_button_status_check()) 
					{ //if no valid key press
						printf("deep 00 \n");
						enter_deep_aaa(DEEP_SLEEP_ANA_AAA);
					}
				}
			}
		}

		#if (G24_DELAY_ENTER_SLEEP_ENABLE)
			if ( 0 == mouse_not_action_timeout() )
			{
				gc_mouse_sta.need_suspend_flag = 0;
			}
		#endif

		#if PM_SYS_LOW_POWER_ENABLE
			if ( 0 == mcu_sleep_status.enter_lower )
			{
				gc_mouse_sta.need_suspend_flag = 0;
			}
		#endif

		#if (PROJECT_ID == PID_535)
			if ( battery_low_flag_status() && connect_ok )
			{
				if ( idle_count <= 13 )
				{
					gc_mouse_sta.need_suspend_flag = 0;
				}
			}
		#endif

		/* If the need_suspend_flag is set, enter suspend */
		if (gc_mouse_sta.need_suspend_flag)
		{
			cpu_sleep_wakeup(SUSPEND_MODE, wake_src, (wakeup_next_tick + interval*CLOCK_16M_SYS_TIMER_CLK_1MS));
			wakeup_next_tick = clock_time();
			rf_drv_private_2m_init();
		}
	}
}

/**
 * @brief	2.4G main loop
 * @param	none
 * @return	none
 */
void d24_main_loop()
{
	u32 temp = 8000;
	static u32 tick_loop = 0;

	if (device_status == STATE_NORMAL)
	{
	#if (PROJECT_ID == PID_601) || (PROJECT_ID == PID_660)
		temp = report_rate*1000 - 75; //unit 1us
	#else
		temp = report_rate*1000;
	#endif
	}

	if (gc_mouse_sta.need_suspend_flag)
	{
		/* The device allow to enter suspend mode, the interval is determined by the suspend wake-up interval time*/
		ui_loop_24g(); //2.4G Task scheduling
		tick_loop = clock_time()|1;
	}
	else if (clock_time_exceed(tick_loop, temp))
	{
		/* The device disallow to enter suspend mode, the interval is determined by temp * 1us */
		//tick_loop += temp * CLOCK_16M_SYS_TIMER_CLK_1US;
		tick_loop = clock_time();
		wakeup_next_tick = clock_time()|1;
		ui_loop_24g(); //2.4G Task scheduling
	}

	d24g_rf_loop(); //2.4G RF status polling and send processing

	pm_poll(); //Sleep task scheduling
}
#endif
