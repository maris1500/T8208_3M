/********************************************************************************************************
 * @file     aaa_app.c
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

#include "AAA_public_config.h"
#include "AAA_APP.h"
#include "AAA_LED.h"
#include "main.h"

#define CUST_SAVE_MASTER_INFO_ENABLE_AAA	1
#define LENGHT_USER_BOND_INF  				256

#if MODULE_DIGITAL_TUBE_ENABLE
	#define MY_ADV_INTERVAL_MIN			ADV_INTERVAL_100MS
	#define MY_ADV_INTERVAL_MAX			ADV_INTERVAL_150MS
#else
	#define MY_ADV_INTERVAL_MIN			ADV_INTERVAL_20MS
	#define MY_ADV_INTERVAL_MAX			ADV_INTERVAL_25MS
#endif

#define MY_APP_ADV_CHANNEL				BLT_ENABLE_ADV_ALL
#define	USER_OWN_ADDRESS_TYPE			OWN_ADDRESS_RANDOM

#define MOUSE_TIMER_SHORT_T			8000

#if (BLT_SOFTWARE_TIMER_ENABLE)
	extern blt_soft_timer_t	blt_timer;
#endif

#define RX_FIFO_SIZE	64
#define RX_FIFO_NUM		8

#define TX_FIFO_SIZE	40
#define TX_FIFO_NUM		16

typedef enum
{
    INIT_DONE_AAA,
    BEGIN_CONNECT_AAA,
    SMP_FIRST_CONNECT_DONE_AAA,
    SMP_RECONNECT_DONE_AAA,
    CONNECTED_DONE_AAA,
    CONNECTED_LOOP_AAA,
} BLE_MODE_CONNECT_STEP_AAA;;

#define SIG_PROC_ENABLE			0
#define LONG_SUSPEND_TIMER_AAA	400

#define PARAMS_GROUP_LEN		5 //Connection parameter update check times

#if BLE_PARA_UPDATA_CHANGE_ENABLE
	#define DEFAULT_INTERVAL	6
	#define DEFAULT_LATENCY		15 //0x2C 44
	#define DEFAULT_TIMEOUT		300
#elif (PROJECT_ID == PID_M388) || (PROJECT_ID == PID_0120) || (PROJECT_ID == PID_HM668) || (PROJECT_ID == PID_M45)
	#define DEFAULT_INTERVAL	6
	#define DEFAULT_LATENCY 	10 //0x2C 44
	#define DEFAULT_TIMEOUT 	200
#elif (PROJECT_ID == PID_DMS157)
	#define DEFAULT_INTERVAL	6
	#define DEFAULT_LATENCY 	15 //0x2C 44
	#define DEFAULT_TIMEOUT 	200
#else
	#define DEFAULT_INTERVAL	6
	#define DEFAULT_LATENCY 	44 //0x2C //44
	#define DEFAULT_TIMEOUT 	300
#endif

#if (MODULE_DIGITAL_TUBE_ENABLE)
	extern void digital_tube_pro(void);
#endif

extern void led_ble_conned_succ_dis_next(void);
extern void ble_status_changed(void);

_attribute_data_retention_ u32 power_on_tick;
_attribute_data_retention_ u8  pair_success = 0;
_attribute_data_retention_ u32 start_tick;

_attribute_data_retention_ u8  Switch_Adv_Type = 0;

_attribute_data_retention_ u32 loop_cnt;
_attribute_data_retention_ u32 connect_begin_tick;

_attribute_data_retention_ u8  ui_ota_is_working = 0;
//_attribute_data_retention_ u8  conn_params_cout = 0;
_attribute_data_retention_ u32 conn_params_tick;

_attribute_data_retention_ u8  conn_step = 0;

_attribute_data_retention_ u8  ble_status_aaa;
_attribute_data_retention_ u32 idle_tick;
_attribute_data_retention_ u32 idle_count;

#if BLT_SOFTWARE_TIMER_ENABLE
_attribute_data_retention_ u8  soft_time_flag = 0;
#endif

#if AUTO_CHECK_OS_TYPE
    _attribute_data_retention_ u8 os_type = 0;
#endif

//u32 ccc_flag = 0;
u16 ccc_flag = 0;

//_attribute_data_retention_ u8 temp_master_addr[8] = {0};

#if CUST_SAVE_MASTER_INFO_ENABLE_AAA
_attribute_data_retention_ _attribute_aligned_(4) smp_param_save_t smp_param_inf[4];//muti device addr
_attribute_data_retention_ int binding_master_addr_idx;
#endif

#if (BATT_CHECK_ENABLE)
extern void batt_check_time_reload(void);
#endif


#if (KEY_FEATURE_DESKTOP_ENABLE)
	extern unsigned char ms_desktop_flag;
#endif

#if (KEY_FEATURE_MIKE_AI_ENABLE)
	extern unsigned char ms_mike_ai_flag;
#endif

#if (WHEEL_LEFT_RIGHT_ENABLE)
	extern s8 voice_wheel_confirm(void);
#endif

#define Microsoft_POP_Redefine_ENABLE Microsoft_Swift_Pairing_ENABLE

//////////Adv Packet, Response Packet//////////////////////////////////////////////
#if (Microsoft_Swift_Pairing_ENABLE)

	#if Microsoft_POP_Redefine_ENABLE
		#define ADV_DEV_POSITION   17
		#define ADV_DEVICE_NAME_MAX_LEN  14
	#else
		#define ADV_DEV_POSITION   20
		#define ADV_DEVICE_NAME_MAX_LEN  11
	#endif

	_attribute_data_retention_ u8	tbl_advData_aaa[31] =
	{
	#if Microsoft_POP_Redefine_ENABLE
		0x03, 0x19, 0xc2, 0x03, // 3C2, Mouse, 3C1, key, Generic category,
	    0x03, 0x03, 0x12, 0x18, // incomplete list of service class UUIDs (0x1812-HID SERVICE, 180F-BATTERY)
		0x06, 0xff, 0x06, 0x00, 0x03, 0x00, 0x80, // Support Microsoft swift pair
		15, 0x09,				// device name
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	#else
			0x02, 0x01, 0x05, 		// BLE limited discoverable mode and BR/EDR not supported
			0x03, 0x03, 0x12, 0x18,	// incomplete list of service class UUIDs (0x1812-HID SERVICE, 180F-BATTERY)
		#if (BLE_MODE_DISPLAY_KEYBOARD_LOGO_EN)
			0x03, 0x19, 0xc1, 0x03, // 3C2, Mouse, 3C1, key, Generic category,
		#else
			0x03, 0x19, 0xc2, 0x03,
		#endif
			0x06, 0xff, 0x06, 0x00, 0x03, 0x00, 0x80, // Support Microsoft swift pair
			12, 0x09,				// device name
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	#endif
	};
#else
	#define ADV_DEV_POSITION   13
	#define ADV_DEVICE_NAME_MAX_LEN  18
	_attribute_data_retention_ u8	tbl_advData_aaa[31] =
	{
		0x02, 0x01, 0x05, 		// BLE limited discoverable mode and BR/EDR not supported
	#if (BLE_MODE_DISPLAY_KEYBOARD_LOGO_EN)
		0x03, 0x19, 0xc1, 0x03, // 3C2, Mouse, 3C1, key, Generic category,
	#else
		0x03, 0x19, 0xc2, 0x03,
	#endif
		0x03, 0x03, 0x12, 0x18,	// incomplete list of service class UUIDs (0x1812-HID SERVICE, 180F-BATTERY)
		19, 0x09,				// device name
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};
#endif

#if Microsoft_POP_Redefine_ENABLE
	#define SCAN_DEV_POSITION   5
#else
	#define SCAN_DEV_POSITION   2
#endif

_attribute_data_retention_ u8	tbl_scanData_aaa[31] =
{
#if Microsoft_POP_Redefine_ENABLE
    0x02, 0x01, 0x05,
	27,9,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0
#else
	30,9,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,
#endif
};


#if (DEVICE_NAME_INCLUDE_MAC_DEBUG)
/**
 * @brief	Converts hex to a string
 * @param	rp: target
 * @param	sp: source
 * @param	len: convert length
 * @return	none
 */
void BufToHexString(u8 *rp, u8 *sp, u8 len)
{
    u8  hex[17] = "0123456789ABCDEF";
    u8  count = 0;
    u8  i;

    for (i = len; i > 0; i--)
    {
        rp[count++] = hex[sp[i - 1] >> 4];
        rp[count++] = hex[sp[i - 1] & 0x0F];
    }
}
#endif


#if 0 // TODO:check
u8 blc_ll_getTxFifoNumber(void)
{
	u8 r = irq_disable();

	u8 fifo_num = ((reg_dma_tx_wptr - reg_dma_tx_rptr) & 15 )  +  ( (blt_txfifo.wptr - blt_txfifo.rptr) & 31 ) ;

	irq_restore(r);

	return  fifo_num;
}
#endif

/**
 * @brief	Set broadcast and scan response content
 * @param	none
 * @return	none
 */
void set_adv_scanRsp_data()
{
	if (pair_flag == 0)
	{
		/* reconnect mode, Set the content to empty to prevent other host from searching for it */
		u8 adv_scan_data[1];
		adv_scan_data[0]=0;
		bls_ll_setAdvData((u8 *)adv_scan_data, 1);
		bls_ll_setScanRspData((u8 *)adv_scan_data, 1);
	}
	else
	{
		/* pair mode, Set the content normal */
		if (device_name_len <= ADV_DEVICE_NAME_MAX_LEN)
		{
			/* load ble name to adv content */
			memcpy(&tbl_advData_aaa[ADV_DEV_POSITION], user_cfg.device_name, device_name_len); 
			tbl_advData_aaa[ADV_DEV_POSITION-2] = device_name_len + 1;
			bls_ll_setAdvData((u8 *)tbl_advData_aaa, device_name_len + ADV_DEV_POSITION);
		}
		else
		{
			/* not load ble name to adv content */
			bls_ll_setAdvData((u8 *)tbl_advData_aaa, ADV_DEV_POSITION - 2);
		}

		/* load ble name to scan response content */
		memcpy(&tbl_scanData_aaa[SCAN_DEV_POSITION], user_cfg.device_name, device_name_len); 
		tbl_scanData_aaa[SCAN_DEV_POSITION-2] = device_name_len + 1;
		bls_ll_setScanRspData((u8 *)tbl_scanData_aaa, device_name_len + SCAN_DEV_POSITION);
	}
}
#if 0
/**
 * @brief	Connection parameter update process
 * @param	none
 * @return	none
 * @note	Check whether the connection parameters are correct once per second
 */
void connect_params_proc()
{
	if (bls_ll_getConnectionInterval() < DEFAULT_INTERVAL)
	{
		conn_params_cout = 0;
		conn_params_tick = 0;
	}
	else if (conn_params_cout < PARAMS_GROUP_LEN)
	{
		if (conn_params_tick && clock_time_exceed(conn_params_tick, (conn_params_cout+1)*1000000))
		{
			conn_params_cout++;

			if (conn_params_cout < PARAMS_GROUP_LEN)
			{
				if (bls_ll_getConnectionLatency() != DEFAULT_LATENCY)
				{
					bls_l2cap_requestConnParamUpdate(DEFAULT_INTERVAL, DEFAULT_INTERVAL, DEFAULT_LATENCY, DEFAULT_TIMEOUT);
				}
			}
			else
			{
				clear_pair_flag();
				reset_idle_status();
			}
		}
	}
}
#endif
/**
 * @brief	Auto switching between directed and undirected adv
 * @param	none
 * @return	none
 */
void app_switch_to_indirect_adv(u8 e, u8 *p, int n)
{
	Switch_Adv_Type += 1;

	if (Switch_Adv_Type & 0x01)
	{
		/* set undirected adv */
		bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN,
							MY_ADV_INTERVAL_MAX,
							ADV_TYPE_CONNECTABLE_UNDIRECTED, 
							USER_OWN_ADDRESS_TYPE,
							0,
							NULL,
							MY_APP_ADV_CHANNEL,
							ADV_FP_ALLOW_SCAN_WL_ALLOW_CONN_WL);
	}
	else
	{
		/* set directed adv */
		bls_ll_setAdvParam( ADV_INTERVAL_3_75MS,
							ADV_INTERVAL_3_75MS,
							ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY, 
							USER_OWN_ADDRESS_TYPE,
							BLE_ADDR_PUBLIC,
							smp_param_inf[flash_dev_info.mast_id].peer_addr,
							BLT_ENABLE_ADV_ALL,
							ADV_FP_ALLOW_SCAN_WL_ALLOW_CONN_WL);
	}

	bls_ll_setAdvDuration(1000000, 1); //Current broadcast duration

	bls_ll_setAdvEnable(1); //must: set adv enable
}

/**
 * @brief	set rf power
 * @param	none
 * @return	none
 */
_attribute_ram_code_ void user_set_rf_power(u8 e, u8 *p, int n)
{
#if TEST_PRINTF_ENABLE
	printf("pe1=%d \n", e);
#endif

    rf_set_power_level_index (user_cfg.tx_power);
}

/**
 * @brief	callback when host read HID service
 * @param	none
 * @return	none
 */
int cccWrite(void *p)
{
	rf_packet_att_write_t *src = (rf_packet_att_write_t *)p;
   u8 *buf = &src->value;
   //u8 len = src->l2capLen - 3;
   u16 attHandle=src->handle+(src->handle1<<8);

   if(attHandle==BATT_LEVEL_INPUT_CCB_H)
   {
   		if(buf[0])
   	   {
	   		BM_SET(ccc_flag,BATTERY_LEVEL_CCC_EN);
   			
   	   }
	   else
	   {
	   		BM_CLR(ccc_flag,BATTERY_LEVEL_CCC_EN);		
	   }
   }
#if HID_MOUSE_ATT_ENABLE
   else if(attHandle==HID_MOUSE_REPORT_INPUT_CCB_H)
   {
   	   if(buf[0])
   	   {
	   		BM_SET(ccc_flag,MOUSE_REPORT_CCC_EN);
   			
   	   }
	   else
	   {
	   		BM_CLR(ccc_flag,MOUSE_REPORT_CCC_EN);		
	   }
   }
#endif
#if HID_KEYBOARD_ATT_ENABLE
   else if(attHandle==HID_NORMAL_KB_REPORT_INPUT_CCB_H)
   {
   		 if(buf[0])
   	   {
	   		BM_SET(ccc_flag,KB_NORMAL_KEY_CCC_EN);
   			
   	   }
	   else
	   {
	   		BM_CLR(ccc_flag,KB_NORMAL_KEY_CCC_EN);		
	   }
   }
    else if(attHandle==HID_CONSUME_REPORT_INPUT_CCB_H)
   {
   		 if(buf[0])
   	   {
	   		BM_SET(ccc_flag,KB_CONSUME_KEY_CCC_EN);
   			
   	   }
	   else
	   {
	   		BM_CLR(ccc_flag,KB_CONSUME_KEY_CCC_EN);		
	   }
   }
#endif
    clear_pair_flag();
    my_fifo_reset (&fifo_km);
	reset_idle_status();
    connect_begin_tick = clock_time() | 1;

    return 0;
}

#if AUTO_CHECK_OS_TYPE
typedef struct{
	u32 dma_len;

	u8  type   :4;
	u8  rfu1   :1;
	u8  chan_sel:1;
	u8  txAddr :1;
	u8  rxAddr :1;

	u8  rf_len;    //LEN(6)_RFU(2)
	u8 initA[6];   //scanA
	u8 advA[6];   //
	u8 accessCode[4];  // access code
	u8 crcinit[3];
	u8 winSize;
	u16 winOffset;
	u16 interval;
	u16 latency;
	u16 timeout;
	u8 chm[5];
	u8 hop;    //sca(3)_hop(5)
}rf_packet_connect_t;
#endif

/**
 * @brief	callback when connect with host
 * @param	none
 * @return	none
 */
void task_connect(u8 e, u8 *p, int n)
{
     if(pair_flag)
     {
	 	ccc_flag=0;
     }
	 else
	 {
	 	// ccc_flag=U32_MAX;
	 	 ccc_flag=U16_MAX;
	 }

	 if ( 0 == connect_ok )
	 {
		led_ble_ConnectedStatus();
	 }
	
	/* Set connection-related status and flags */
    connect_begin_tick = clock_time() | 1;
    my_fifo_reset (&fifo_km);
	idle_count = 0;
    loop_cnt = 0;
    connect_ok = 1;
    ui_ota_is_working = 0;
//    conn_params_cout = 0;
    conn_params_tick = 0;
    conn_step = BEGIN_CONNECT_AAA;
    bls_ll_setAdvDuration(0, 0);
    ble_status_aaa = BEGIN_CONNECTED_STATUS_AAA;
	pair_success = 0;

	printf("task_connect\n");

#if AUTO_CHECK_OS_TYPE
	rf_packet_connect_t *pc = (rf_packet_connect_t *)(p - 6);

	printf("txAddr=%d,interval=%d,timeout=%d.\n",pc->txAddr,pc->interval,pc->timeout);
	
	if (os_type == UNKNOW_OS_TYPE)
	{
		if (pc->txAddr)
		{
			if ((pc->interval >= 0x0c) && (pc->interval <= 0x18) && (pc->timeout <= 0xc8))
			{
				os_type = APPLE_OS_TYPE;
				printf("apple_os\r\n");
			}
			else
			{
				os_type = ANDROID_OS_TYPE;
				printf("android_os\r\n");
			}
		}
		else//public
		{
			//os_type maybe detect errors
			//because  some andriod phone is public for exampe huawei asw=4.4.3
			if (pc->timeout <= 0xc8)
			{
				os_type = MAC_OS_TYPE;
				printf("mac_os\r\n");
			}
			else
			{
				os_type = WINDOWS_OS_TYPE;
				printf("window_os\r\n");
			}
		}
	}
#endif
}

/**
 * @brief	callback when terminate with host
 * @param	none
 * @return	none
 */
 extern unsigned char ble_switch_count_flag;
void task_terminate(u8 e, u8 *p, int n) //*p is terminate reason
{
	bls_ll_setAdvEnable(0);


    //HCI_ERR_CONN_TIMEOUT:0x08
    //HCI_ERR_REMOTE_USER_TERM_CONN:0x13
    //HCI_ERR_CONN_TERM_MIC_FAILURE:0x3D
	// printf("terminate reaseon: 0x%01x, clear connect_ok = 0\n", *p);
	// printf("*ble_status_aaa= %d, active_disconnect_reason=%d \n", ble_status_aaa, active_disconnect_reason);
	// printf("*mode= %d, mast_id=%d, test_kk=%d \n", flash_dev_info.mode, flash_dev_info.mast_id, ble_switch_count_flag);

	printf("terminate reaseon: %1x \n", *p);

#if (PROJECT_ID == PID_M388)
	adv_count = 0;
#endif

    if (ble_status_aaa == DEEP_TERMINATE_STATUS_AAA)
    {
        ble_status_aaa = DEEP_SLEEPE_STATUS_AAA; //Wait for a deep sleep
    }
	else if (active_disconnect_reason == BLE_PAIR_REBOOT_ANA_AAA)
	{
	#if MOUSE_FACTORY_RESET_ENABLE
		if ( FACTORY_OVER == mouse_factory_reset_flag() )
		{
			mouse_factory_reset_action();
			user_reboot(POWER_ON_ANA_AAA); //Reboot to enter pair mode
		}
		else
		{
			user_reboot(BLE_PAIR_REBOOT_ANA_AAA);
		}
	#else
		printf("mode chage 55xx \n");
		user_reboot(BLE_PAIR_REBOOT_ANA_AAA); //Reboot to enter pair mode
	#endif
	}
	else if (active_disconnect_reason == MODE_CHANGE_REBOOT_ANA_AAA)
	{
	#if (PROJECT_ID == PID_M710) || KEY_MODE_CHANGE_G24_BLE_ENABLE || KEY_PRESS_CHANGE_MODE_ENABLE
		if ( RF_1M_BLE_MODE == flash_dev_info.mode && ble_switch_count_flag == 1 )
	#else
		if ( RF_1M_BLE_MODE == flash_dev_info.mode && flash_dev_info.mast_id >= 1 && ble_switch_count_flag == 1 )
	#endif
		{
			flash_dev_info.mode = RF_2M_2P4G_MODE;
			flash_dev_info.mast_id = 0;
		}
		save_dev_info_flash();
		sleep_ms(50);
		user_reboot(MODE_CHANGE_REBOOT_ANA_AAA); //Reboot to change to 2.4G mode
	}
	else if (active_disconnect_reason == MUTI_DEVICE_REBOOT_ANA_AAA)
	{
		printf("-> mode = %d, id = %d \n", flash_dev_info.mode, flash_dev_info.mast_id);
		save_dev_info_flash();
		user_reboot(MUTI_DEVICE_REBOOT_ANA_AAA); //Reboot to switch BLE channel
	}
    else
    {
        ble_status_aaa = POWER_ON_STATUS_AAA; //Wait for connection
    }

    /* Clear connection-related status and flags */
    active_disconnect_reason = 0;
    conn_step = 0;
    ui_ota_is_working = 0;
    connect_ok = 0;
	
#if AUTO_CHECK_OS_TYPE
    os_type = UNKNOW_OS_TYPE;
#endif

#if BLT_SOFTWARE_TIMER_ENABLE
    delet_soft_time();
#endif
}

#if SHOW_MAST_REAL_MAC_DEBUG
void get_master_real_mac()
{
    if (temp_master_addr[0] & 0x40) //OWN_ADDRESS_RANDOM;
    {
        smp_param_save_t  bondInfo = {0};
        u32 ret = blc_smp_param_loadByAddr(1, &temp_master_addr[2], &bondInfo);
        if (ret == 0) {
            memset(&output_dev_info.master_mac[0], 0, 6);
            return;
        } else
            memcpy(&output_dev_info.master_mac[0], &bondInfo.peer_id_addr[0], 6);
      
    } else {
        memcpy(&output_dev_info.master_mac[0], &temp_master_addr[2], 6);
    }

}
#endif

/**
 * @brief	callback when wakeup from suspend
 * @param	none
 * @return	none
 */
void task_gpio_eary_wakeup(u8 e, u8 *p, int n)
{
    if (e == BLT_EV_FLAG_GPIO_EARLY_WAKEUP)
    {
     #if(DEBUG_MODE)
    	// printf("early gpio wake up\n");
	 #endif
		ui_loop();
    }
}

/**
 * @brief	Save device pairing information (SMP) to the flash
 * @param	none
 * @return	none
 */
void save_smp_inf()
{
	u32 CurStartAddr;

    if (pair_success)
    {
		pair_success = 0;

		smp_param_save_t smp_param_new;

		/* Gets the first address of the currently paired device information stored in the Flash */
		extern int blc_smp_param_getCurStartAddr();
		CurStartAddr = blc_smp_param_getCurStartAddr();

		/* Read smp information about the currently paired device */
		flash_read_page(CurStartAddr + bond_device_flash_cfg_idx, 64, &smp_param_new.flag);
		memcpy(&smp_param_inf[flash_dev_info.mast_id].flag, &smp_param_new.flag, 64);

		/* Store device information to the local flash */
		binding_master_addr_idx += LENGHT_USER_BOND_INF;
		flash_write_page_user(CFG_MAST_ADDR + binding_master_addr_idx, LENGHT_USER_BOND_INF, (u8*)&smp_param_inf[0].flag);

	#if 0
		printf("peer addr type:%01x--- 0x%01x%01x%01x%01x%01x%01x\n", smp_param_new.peer_addr_type,	smp_param_new.peer_addr[0], smp_param_new.peer_addr[1], smp_param_new.peer_addr[2], smp_param_new.peer_addr[3], smp_param_new.peer_addr[4],smp_param_new.peer_addr[5]);
		printf("peer id addr type:%01x--- 0x%01x%01x%01x%01%01x%01x\n", smp_param_new.peer_id_adrType, smp_param_new.peer_id_addr[0], smp_param_new.peer_id_addr[1], smp_param_new.peer_id_addr[2],smp_param_new.peer_id_addr[3], smp_param_new.peer_id_addr[4],smp_param_new.peer_id_addr[5]);
	#endif

		/* mast_id is already plus 1 when enter pair mode(but not saved to flash),
		 * so mast_id plus 1 when pair successful and store it in flash */
		if (deep_flag == BLE_PAIR_REBOOT_ANA_AAA)
		{
			write_deep_ana0(CLEAR_FLAG_ANA_AAA);
			flash_dev_info.slave_mac_addr[flash_dev_info.mast_id]++;
			save_dev_info_flash();
		}
	}
}

/**
 * @brief	callback when the host request to update the connection parameters
 * @param	none
 * @return	none
 */
void task_conn_update_req(u8 e, u8 *p, int n)
{
    printf("conn_update_req %d %d %d\n", p[3] | p[4] << 8, p[5] | p[6] << 8, p[7] | p[8] << 8);

#if 0
    conn_params_pending = 1;

    u16 interval = p[3] | p[4] << 8;
    u16 latency = p[5] | p[6] << 8;

    if ((interval >= ConParams[conn_params_cout].Min) && (interval <= ConParams[conn_params_cout].Max) && latency > 0)
    {
        conn_params_cout = 0xff;

    }
    else
    {
        conn_params_cout++;
    }
#endif
}

/**
 * @brief	callback when the connection parameters is updated
 * @param	none
 * @return	none
 */
void task_conn_update_done(u8 e, u8 *p, int n)
{
	printf("conn_update_done %d %d %d\n", p[0]|p[1]<<8,p[2]|p[3]<<8,p[4]|p[5]<<8);

#if 0
    if (ble_status_aaa == T5S_CONNECTED_STATUS_AAA) // for windows
    {
        if ((conn_params_cout == 0) && (bls_ll_getConnectionInterval() > (ConParams[0].Min + 4)))
        {
            if (deep_flag != CONN_PARAM_FAIL_REBOOT_ANA_AA)
            {
                bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN);
                ble_status_aaa = CONN_PARAM_FAIL_TERMINATE_STATUS_AAA;
            }
        }
    }
#endif
}

_attribute_ram_code_ void ble_remote_set_sleep_wakeup(u8 e, u8 *p, int n)
{
#if TEST_PRINTF_ENABLE
	printf("pe2=%d \n", e);
#endif

    if (suspend_wake_up_enable && (blc_ll_getCurrentState() == BLS_LINK_STATE_CONN) && (((u32)(bls_pm_getSystemWakeupTick() - clock_time())) > 20 * CLOCK_16M_SYS_TIMER_CLK_1MS)) //suspend time > 30ms.add gpio wakeup
    {
        bls_pm_setWakeupSource(PM_WAKEUP_PAD);  //gpio pad wakeup suspend/deepsleep
	#if WHEEL_FUN_ENABLE_AAA
        wheel_set_wakeup_level_suspend(1);
	#endif
	#if BUTTON_FUN_ENABLE_AAA
        btn_set_wakeup_level_suspend(1);
	#endif
	#if (SENSOR_FUN_ENABLE_AAA && SENSOR_MOTION_ENABLE)
        sensor_set_wakeup_level_suspend(1);
	#endif
		//bls_pm_setManualLatency(100);
	#if(DEBUG_MODE)
        //if(((u32)(bls_pm_getSystemWakeupTick() - clock_time())) < 120 * CLOCK_16M_SYS_TIMER_CLK_1MS)
		//printf("systime wakeup less 120ms\n");
	#endif
    }
	#if(DEBUG_MODE&&0)
	static u32 suspend_tick =0;
	if(clock_time_exceed(suspend_tick,100*1000))
		{
			suspend_tick = clock_time();
			printf("enter suspend\n");
		}
	#endif
}

int att_sig_proc_handler(u16 connHandle, u8 *p)
{
#if 0//SIG_PROC_ENABLE

    rf_pkt_l2cap_sig_connParaUpRsp_t *pp = (rf_pkt_l2cap_sig_connParaUpRsp_t *)p;

#if 0//test debug
    foreach (i, 16)
    {
        PrintHex(*((u8 *)pp + i));
    }
    printf(".\n");
#endif

    u8 sig_conn_param_update_rsp[9] = { 0x0A, 0x06, 0x00, 0x05, 0x00, 0x13, 0x01, 0x02, 0x00 };
    if (!memcmp(sig_conn_param_update_rsp, &pp->rf_len, 9) && ((pp->type & 0b11) == 2)) //l2cap data pkt, start pkt
    {
        if (pp->result == 0x0000)
        {
#if PRINT_DEBUG_INFO
            printf("Host has accepted conn parameters\r\n");

#endif
            conn_params_pending = 0;
            conn_params_cout = 0xff;
        }
        else if (pp->result == 0x0001)
        {
            conn_params_pending = 0;
            conn_params_cout++;
#if PRINT_DEBUG_INFO

            printf("Host has rejected conn parameters\r\n");
#endif
        }
    }
#endif
    return 0;
}


#if (BLE_OTA_SERVER_ENABLE)
/**
 * @brief	callback when OTA start
 * @param	none
 * @return	none
 */
void app_enter_ota_mode(void)
{
	//printf("---app_enter_ota_mode.\n");

    ui_ota_is_working = 1;
	
	#if (BLT_APP_LED_ENABLE && BLE_OTA_LED_DEBUG)
	    gpio_write(PIN_BLE_LED, 1);
	#endif
	
	#if FLASH_LOCK_ENABLE
		flash_lock_handle(FLASH_LOCK_NONE_BLOCK);
	#endif

    bls_ota_setTimeout(160 * 1000 * 1000); //set OTA timeout  15 seconds
}

/**
 * @brief	callback when OTA over
 * @param	none
 * @return	none
 */
void app_ota_result(int result)
{
	printf("---app_ota_result=%d.\r\n", result);
	
    irq_disable();
#if (MODULE_WATCHDOG_ENABLE)	
    wd_stop();
#endif

#if (MODULE_32K_WATCHDOG_ENABLE)
		wd_32k_stop();
#endif


#if (BLE_OTA_LED_DEBUG && BLT_APP_LED_ENABLE)
    gpio_set_output_en(PIN_BLE_LED, 1);
    if (result == OTA_SUCCESS)  //OTA success
    {
        gpio_write(PIN_BLE_LED, 1);
        sleep_us(2000000);  //led on for 2 second
        gpio_write(PIN_BLE_LED, 0);
        printf("ota success\r\n");
    }
    else   //OTA fail
    {
        for (int i = 0; i < 4; i++)
        {
            gpio_write(PIN_BLE_LED, 1);
            sleep_us(250000);
            gpio_write(PIN_BLE_LED, 0);
            sleep_us(250000);
        }
        printf("ota fail=%x\r\n",result);
    }
#endif
}
#endif



/**
 * @brief	Read device paired information(SMP info)
 * @param	none
 * @return	none
 */
void change_ble_stack_smp_inf()
{
	smp_param_save_t tmp[4];
	u8* src  = &smp_param_inf[0].flag;
	u8 *last = &tmp[0].flag;

	/* Read paired info from paired storage addr */
	binding_master_addr_idx = flash_info_load_aaa(CFG_MAST_ADDR, src, LENGHT_USER_BOND_INF);
	if (binding_master_addr_idx < 0)
	{
		/* The device has not been paired, enters the pairing mode */
        set_pair_flag();
		return;
	}

#if 0
	flash_read_page(FLASH_ADR_SMP_PAIRING, 256, last);
	for (u8 i=0;i<8;i++)
	{
		if (memcmp(last,src, 256))
		{
			flash_erase_sector(FLASH_ADR_SMP_PAIRING);
			flash_write_page(FLASH_ADR_SMP_PAIRING, 256, src);
			flash_read_page(FLASH_ADR_SMP_PAIRING, 256, last);
		}
		else
		{
			break;
		}
	}
#else
	for (u8 i=0; i<8; i++)
	{
		/* Read smp info from smp storage addr */
		flash_read_page(FLASH_ADR_SMP_PAIRING, 64, last);

		/* If smp info different from paired info, erase smp info and write paired info to smp storage addr */
		if (memcmp(last, &smp_param_inf[flash_dev_info.mast_id].flag, 64))
		{
			flash_erase_sector(FLASH_ADR_SMP_PAIRING);
			flash_write_page(FLASH_ADR_SMP_PAIRING, 64, &smp_param_inf[flash_dev_info.mast_id].flag);
		}
		else
		{
			break;
		}
	}
#endif
}

/**
 * @brief	callback when host event happen
 * @param	none
 * @return	none
 */
int app_host_event_callback(u32 h, u8 *para, int n)
{
    u8 event = h & 0xFF;

    switch (event)
    {
        case GAP_EVT_SMP_PAIRING_FAIL:
	        {
	            gap_smp_pairingFailEvt_t *p = (gap_smp_pairingFailEvt_t *)para;

	            if (p->reason == PAIRING_FAIL_REASON_UNSPECIFIED_REASON)
	            {
	                bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN); //push terminate cmd into ble TX buffer
	            }
				
	        }
	        break;
		
        case GAP_EVT_SMP_PAIRING_SUCCESS:
	        {
				 gap_smp_pairingSuccessEvt_t *p = (gap_smp_pairingSuccessEvt_t *)para;

				 if (p->bonding_result)
				 {
				 	pair_success=1;
					bls_l2cap_requestConnParamUpdate(DEFAULT_INTERVAL, DEFAULT_INTERVAL, DEFAULT_LATENCY, DEFAULT_TIMEOUT);
			   #if DEBUG_MODE
	                printf("pair success\r\n");
			   #endif
				 }
	        }
	        break;

        case GAP_EVT_SMP_CONN_ENCRYPTION_DONE:
	        {
	            gap_smp_connEncDoneEvt_t *p = (gap_smp_connEncDoneEvt_t *)para;
				
	            if (p->re_connect == SMP_STANDARD_PAIR)  //first paring
	            {
	       			conn_step = SMP_FIRST_CONNECT_DONE_AAA;
			   #if DEBUG_MODE
	                printf("first connected\r\n");
			   #endif
	            }
	            else if (p->re_connect == SMP_FAST_CONNECT)  //auto connect
	            {
	            	my_fifo_reset(&fifo_km);
	                conn_step = SMP_RECONNECT_DONE_AAA;
				#if DEBUG_MODE
	                printf("reconnected\r\n");
				#endif
	            }

	            connect_begin_tick = clock_time() | 1;
	        }
        	break;
		
        default:
            break;
    }

    return 0;
}

/**
 * @brief	Initialization of BLE related configurations
 * @param	none
 * @return	none
 */
void user_init_normal(void)
{
    printf("***** mouse ble user init ******\n");
    ////////////////// BLE stack initialization ////////////////////////////////////
#if BLE_SNIFF_DEBUG
    u8  tbl_mac [6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0xc7};
    extern 	u8	blc_adv_channel[];
    //_attribute_data_retention_	u8		blc_adv_channel[3] = {37, 38, 39};
    blc_adv_channel[0] = 37;
    blc_adv_channel[1] = 38;
    blc_adv_channel[2] = 39;
    memcpy(tbl_mac, (u8 *)&user_cfg.dev_mac, 6);
    tbl_mac[5] = 0xD0 | (flash_dev_info.mast_id + 1);
#else
    u8  tbl_mac [] = {0x12, 0x34, 0x56, 0x78, 0x11, 0x54};
    memcpy(tbl_mac, (u8 *)&user_cfg.dev_mac, 6);

    /* In pairing mode the slave mac +1, to avoid being connected back to the previously paired host */
    if (pair_flag)
        tbl_mac[4] = flash_dev_info.slave_mac_addr[flash_dev_info.mast_id] + 1;
    else
        tbl_mac[4] = flash_dev_info.slave_mac_addr[flash_dev_info.mast_id];

    /* Add BLE channel to MAC */
    tbl_mac[5] = 0xD0 | (flash_dev_info.mast_id + 1);
#endif

    blc_ll_setRandomAddr(tbl_mac);
    ////// Controller Initialization  //////////
    blc_ll_initBasicMCU();				//mandatory
    blc_ll_initStandby_module(tbl_mac);	//mandatory
    blc_ll_initAdvertising_module(); 	//adv module: 		 mandatory for BLE slave,
    blc_ll_initSlaveRole_module();		//slave module: 	 mandatory for BLE slave,
   

	blc_ll_setAclConnMaxOctetsNumber(ACL_CONN_MAX_RX_OCTETS, ACL_CONN_MAX_TX_OCTETS);
	blc_ll_initAclConnTxFifo(app_acl_txfifo, ACL_TX_FIFO_SIZE, ACL_TX_FIFO_NUM);
	blc_ll_initAclConnRxFifo(app_acl_rxfifo, ACL_RX_FIFO_SIZE, ACL_RX_FIFO_NUM);

	u8 check_status = blc_controller_check_appBufferInitialization();
	if (check_status != BLE_SUCCESS){
		/* here user should set some log to know which application buffer incorrect */
		while(1);
	}

    ////// Host Initialization  //////////
    blc_gap_peripheral_init();	//gap initialization
    extern void my_att_init();
    my_att_init();	//gatt initialization
    blc_l2cap_register_handler(blc_l2cap_packet_receive);	//l2cap initialization
	blc_l2cap_initRxDataBuffer(app_l2cap_rx_fifo, L2CAP_RX_BUFF_SIZE);


	/* SMP Initialization */
	/* SMP Initialization may involve flash write/erase(when one sector stores too much information,
	 *   is about to exceed the sector threshold, this sector must be erased, and all useful information
	 *   should re_stored) , so it must be done after battery check */
#if (APP_SECURITY_ENABLE)
	bls_smp_configpairingSecurityInfoStorageAddr(FLASH_ADR_SMP_PAIRING);
	#if CUST_SAVE_MASTER_INFO_ENABLE_AAA
		change_ble_stack_smp_inf();
	#endif
	blc_smp_param_setBondingDeviceMaxNumber(4);  	//default is 4, can not bigger than this value
													//and this func must call before bls_smp_enableParing
	blc_smp_peripheral_init ();
	blc_smp_setSecurityLevel(Unauthenticated_Pairing_with_Encryption);
#else
	blc_smp_setSecurityLevel(No_Security);
#endif

    ///////////////////// USER application initialization ///////////////////
    set_adv_scanRsp_data(); //Set broadcast and scan response content

    //set rf power index, user must set it after every suspend wakeup, cause relative setting will be reset in suspend
    user_set_rf_power(0, 0, 0);
    bls_app_registerEventCallback(BLT_EV_FLAG_SUSPEND_EXIT, &user_set_rf_power);

    //ble event call back
    bls_app_registerEventCallback(BLT_EV_FLAG_CONNECT, &task_connect);
    bls_app_registerEventCallback(BLT_EV_FLAG_TERMINATE, &task_terminate);
#if DIRECT_ADV_TO_UNDIRECT_ENABLE
    bls_app_registerEventCallback(BLT_EV_FLAG_ADV_DURATION_TIMEOUT, &app_switch_to_indirect_adv);
#endif

    // New paring: send security_request immediately after connection complete
    // reConnect:  send security_request 1000mS after connection complete. If master start paring or encryption before 1000mS timeout, slave do not send security_request.
    blc_smp_configSecurityRequestSending(SecReq_NOT_SEND, SecReq_NOT_SEND, 1000); //if not set, default is:  send "security request" immediately after link layer connection established(regardless of new connection or reconnection )

    blc_gap_registerHostEventHandler(app_host_event_callback);
    blc_gap_setEventMask(GAP_EVT_MASK_SMP_PAIRING_FAIL		|  \
                         GAP_EVT_MASK_SMP_PAIRING_SUCCESS	|  \
                         GAP_EVT_MASK_SMP_CONN_ENCRYPTION_DONE);
    
/*******************************************************************************/
    // TODO:Temporary solution, need to be removed in lib next version
    typedef int (*ll_enc_done_callback_t)(u16 connHandle);
    extern void blc_ll_registerConnectionEncryptionDoneCallback(ll_enc_done_callback_t  cb);
    extern int       bls_smp_encryption_done(u16 connHandle);
    blc_ll_registerConnectionEncryptionDoneCallback(bls_smp_encryption_done);
/*******************************************************************************/
    bls_app_registerEventCallback(BLT_EV_FLAG_CONN_PARA_REQ, &task_conn_update_req);
    bls_app_registerEventCallback(BLT_EV_FLAG_CONN_PARA_UPDATE, &task_conn_update_done);
    bls_app_registerEventCallback(BLT_EV_FLAG_GPIO_EARLY_WAKEUP, &task_gpio_eary_wakeup);
    //unit is 10ms
    blc_att_setServerDataPendingTime_upon_ClientCmd(5);

    ///////////////////// Power Management initialization///////////////////
#if (BLE_APP_PM_ENABLE)
	#if (SAVE_RAM_CODE_ENABLE)
		blc_ll_initPowerManagement_module_save_ram();	//pm module:		 optional
	#else
		blc_ll_initPowerManagement_module();			//pm module:		 optional
	#endif
    bls_pm_setSuspendMask(SUSPEND_ADV | SUSPEND_CONN);
    bls_app_registerEventCallback(BLT_EV_FLAG_SUSPEND_ENTER, &ble_remote_set_sleep_wakeup);
#else
    bls_pm_setSuspendMask(SUSPEND_DISABLE);
#endif
    blc_ll_setAdvIntervalCheckEnable(0); //Disable the ADV interval parameter check

#if (BLE_OTA_SERVER_ENABLE)
#if (MODULE_WATCHDOG_ENABLE)
	wd_stop();
#endif
#if (MODULE_32K_WATCHDOG_ENABLE)
	wd_32k_stop();
#endif
    /* OTA module initialization must be called after "blc_ota_setNewFirmwwareStorageAddress"(if used), and before any other OTA API.*/
    blc_ota_initOtaServer_module();
    blc_ota_setOtaProcessTimeout(120); //set OTA whole process timeout
    blc_ota_registerOtaStartCmdCb(app_enter_ota_mode);
    blc_ota_registerOtaResultIndicationCb(app_ota_result);
#endif

#if BLT_SOFTWARE_TIMER_ENABLE
    blt_soft_timer_init();
#endif
    bls_l2cap_setMinimalUpdateReqSendingTime_after_connCreate(1);

    start_tick = clock_time();
}

unsigned char mcu_mode_pair_status(void)
{
	u8 *adr = (u8*)&smp_param_inf[flash_dev_info.mast_id].peer_addr;

	if ( (adr[0] == U8_MAX) && (adr[1] == U8_MAX) && (adr[2] == U8_MAX) && (adr[3] == U8_MAX) )
	{
		return (1);
	}

	if ( (adr[0] == 0) && (adr[1] == 0) && (adr[2] == 0) && (adr[3] == 0) )
	{
		return (1);
	}

	if ( pair_flag )
	{
		return (1);
	}

	return (0);
}


/**
 * @brief	set_adv_type
 * @param	none
 * @return	none
 */
void set_adv_type()
{
    //printf("---set_adv_type.binding_master_addr_idx:0x%04x, pair_flag:%d.mast_id=%d.\n", binding_master_addr_idx, pair_flag,flash_dev_info.mast_id);

	u8 *adr = (u8*)&smp_param_inf[flash_dev_info.mast_id].peer_addr;

    if ((binding_master_addr_idx >= 0) && (pair_flag == 0)) //at least 1 bonding device exist
    {
		//if ((adr[0] == 0) || (adr[0] == U32_MAX))
		if ( ((adr[0] == U8_MAX) && (adr[1] == U8_MAX) && (adr[2] == U8_MAX) && (adr[3] == U8_MAX)) || ((adr[0] == 0) && (adr[1] == 0) && (adr[2] == 0) && (adr[3] == 0)) )
		{ //check whether this mast_id has been bonded
			printf("channel %d not bonded\n", flash_dev_info.mast_id);//not bonded
			set_pair_flag(); //The device has not been paired, enters the pairing mode
			bls_ll_setAdvDuration(0, 0);
	        bls_ll_setAdvParam(MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
	                           ADV_TYPE_CONNECTABLE_UNDIRECTED, USER_OWN_ADDRESS_TYPE,
	                           0,  NULL,
	                           MY_APP_ADV_CHANNEL,
	                           ADV_FP_NONE);
		} 
		else if (smp_param_inf[flash_dev_info.mast_id].peer_addr_type == 0)
		{ //public addr
			printf("channel %d bonded a public addr, direct\n", flash_dev_info.mast_id);
			ll_whiteList_reset(); 	  //clear whitelist
			ll_resolvingList_reset(); //clear resolving list
			if (IS_RESOLVABLE_PRIVATE_ADDR(smp_param_inf[flash_dev_info.mast_id].peer_addr_type, smp_param_inf[flash_dev_info.mast_id].peer_addr))
			{
				ll_resolvingList_add(0, smp_param_inf[flash_dev_info.mast_id].peer_id_addr, smp_param_inf[flash_dev_info.mast_id].peer_irk, NULL);  //no local IRK
				ll_resolvingList_setAddrResolutionEnable(1);
				printf("random addr wL is resolvable priver\n");
        	}
			else
			{
				//if not resolvable random address, add peer address to whitelist
				ll_whiteList_add(smp_param_inf[flash_dev_info.mast_id].peer_addr_type, smp_param_inf[flash_dev_info.mast_id].peer_addr);
				printf("random addr wL is not resolvable priver\n");
			}
			#if	MODULE_DIGITAL_TUBE_ENABLE
				bls_ll_setAdvParam(ADV_INTERVAL_90MS, ADV_INTERVAL_100MS,
								   ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY, USER_OWN_ADDRESS_TYPE,
								   BLE_ADDR_PUBLIC, smp_param_inf[flash_dev_info.mast_id].peer_addr,
								   BLT_ENABLE_ADV_ALL,
								   ADV_FP_ALLOW_SCAN_WL_ALLOW_CONN_WL);
			#else
				bls_ll_setAdvParam(ADV_INTERVAL_3_75MS, ADV_INTERVAL_3_75MS,
								   ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY, USER_OWN_ADDRESS_TYPE,
								   BLE_ADDR_PUBLIC, smp_param_inf[flash_dev_info.mast_id].peer_addr,
								   BLT_ENABLE_ADV_ALL,
								   ADV_FP_ALLOW_SCAN_WL_ALLOW_CONN_WL);
			#endif
            bls_ll_setAdvDuration(2000000, DIRECT_ADV_TO_UNDIRECT_ENABLE);
		} 
		else if (smp_param_inf[flash_dev_info.mast_id].peer_addr_type == 1) 
		{ //random addr
			printf("channel %d bonded a random addr, undirect\n", flash_dev_info.mast_id);
			ll_whiteList_reset(); 	  //clear whitelist
			ll_resolvingList_reset(); //clear resolving list
			if (IS_RESOLVABLE_PRIVATE_ADDR(smp_param_inf[flash_dev_info.mast_id].peer_addr_type, smp_param_inf[flash_dev_info.mast_id].peer_addr))
			{
				ll_resolvingList_add(0, smp_param_inf[flash_dev_info.mast_id].peer_id_addr, smp_param_inf[flash_dev_info.mast_id].peer_irk, NULL);  //no local IRK
				ll_resolvingList_setAddrResolutionEnable(1);
				printf("random addr wL is resolvable priver\n");
        	}
			else
			{
				//if not resolvable random address, add peer address to whitelist
				ll_whiteList_add(smp_param_inf[flash_dev_info.mast_id].peer_addr_type, smp_param_inf[flash_dev_info.mast_id].peer_addr);
				printf("random addr wL is not resolvable priver\n");
			}
		#if Microsoft_POP_Redefine_ENABLE
			tbl_scanData_aaa[2] = 0x04;
		#else
            tbl_advData_aaa[2] = 0x04;
		#endif
            bls_ll_setAdvDuration(0, 0);
			bls_ll_setAdvParam(MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX, \
							ADV_TYPE_CONNECTABLE_UNDIRECTED, USER_OWN_ADDRESS_TYPE, \
							0,  NULL, MY_APP_ADV_CHANNEL, ADV_FP_ALLOW_SCAN_WL_ALLOW_CONN_WL);
        }
    }
    else
    {
		printf("normal pairing adv channel %d\n", flash_dev_info.mast_id);
        bls_ll_setAdvDuration(0, 0);
        bls_ll_setAdvParam(MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
                           ADV_TYPE_CONNECTABLE_UNDIRECTED, USER_OWN_ADDRESS_TYPE,
                           0,  NULL,
                           MY_APP_ADV_CHANNEL,
                           ADV_FP_NONE);
    }
}

/**
 * @brief	BLE Status polling
 * @param	none
 * @return	none
 */
void ble_status_proc_aaa(u8 is_new_key_event)
{
	
#if (MODULE_DIGITAL_TUBE_ENABLE)
	digital_tube_pro();
#endif

    if (ble_status_aaa == T5S_CONNECTED_STATUS_AAA)
    { //connected
	#if (BLE_CONNECT_ENTER_DEEPSLEEP_AFTER_TIME_OUT_ENABLE_AAA && (!BLE_SNIFF_DEBUG))
        if (idle_count >= BLE_CONNECT_TIME_OUT)
        {
        	/* BLE connection timeout (device does not operate for a long time) */
            bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN); //terminate connect
            ble_status_aaa = DEEP_TERMINATE_STATUS_AAA; //BLE status = terminate
        }
	#endif
    }
    else if (ble_status_aaa == POWER_ON_STATUS_AAA)
    { //power on
        bls_ll_setAdvEnable(0); //adv disable
	#if (BLE_SNIFF_DEBUG)
        bls_ll_setAdvDuration(0, 0);
        bls_ll_setAdvParam(MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
                           ADV_TYPE_CONNECTABLE_UNDIRECTED, USER_OWN_ADDRESS_TYPE,
                           0,  NULL,
                           MY_APP_ADV_CHANNEL,
                           ADV_FP_NONE);
	#else
        Switch_Adv_Type = 0;
	
	#if (MODULE_DIGITAL_TUBE_ENABLE)
		digital_tube_pro();
	#endif

        set_adv_type();

	#endif

#if (MODULE_DIGITAL_TUBE_ENABLE)
	digital_tube_pro();
#endif

        set_adv_scanRsp_data(); //Set broadcast and scan response content
        bls_ll_setAdvEnable(1); //adv enable
        adv_begin_tick = clock_time() | 1;
        adv_count = 0;
        ble_status_aaa = LOW_ADV_STATUS_AAA; //BLE status = ADV
    }
    else if (ble_status_aaa == LOW_ADV_STATUS_AAA)
    { //advertising
	#if (BLE_ADV_ENTER_DEEPSLEEP_AFTER_TIME_OUT_ENABLE_AAA&&(!BLE_SNIFF_DEBUG))
        adv_count_poll();

        if (pair_flag)
        {
            if (adv_count >= BLE_ADV_TIMER_OUT)
            {
            	/* BLE pairing timeout: 60s */
                ble_status_aaa = DEEP_SLEEPE_STATUS_AAA;
            }
        }
    #if (BLE_FREE_MATCH_CODE_ENABLE)
        else if ( adv_count >= BLE_RECONNECT_TIME_OUT )
        {
            ble_start_pair();
        }
	#elif (PROJECT_ID == PID_M388) || (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
	    else if (adv_count >= 11)
        {
        	/* BLE reconnect timeout: 10s *///30s now
            ble_status_aaa = DEEP_SLEEPE_STATUS_AAA;
        }
	#elif (PROJECT_ID == PID_M0018) || (PROJECT_ID == PID_535)
	    else if (adv_count >= 31)
        {
        	/* BLE reconnect timeout: 10s *///30s now
            ble_status_aaa = DEEP_SLEEPE_STATUS_AAA;
        }
	#elif (PROJECT_ID == PID_0120)
	    else if (adv_count >= 6)
	    {
	    	 ble_status_aaa = DEEP_SLEEPE_STATUS_AAA;
	    }
    #else
        else if (adv_count >= 60)
        {
        	/* BLE reconnect timeout: 10s *///30s now
            ble_status_aaa = DEEP_SLEEPE_STATUS_AAA;
        }
    #endif

	#endif
	
	#if BLT_APP_LED_ENABLE
        led_ble_Adv_poll(); //LED indicate
	#endif
    }
    else if (ble_status_aaa == BEGIN_CONNECTED_STATUS_AAA)
    { //connect begin
        if (conn_step == BEGIN_CONNECT_AAA)
        {
            if (clock_time_exceed(connect_begin_tick, 40000000))
            {
                ble_status_aaa = OK_CONNECTED_STATUS_AAA; //Generally not executed
            }
        }
		else if (conn_step == SMP_FIRST_CONNECT_DONE_AAA)
		{ //first connect
			if (pair_success)
			{
				ble_status_aaa = OK_CONNECTED_STATUS_AAA;
				/* The SMP encryption is complete, save the pairing information to the flash */
				save_smp_inf();
				clear_pair_flag();
				
            }
		}
        else if (conn_step == SMP_RECONNECT_DONE_AAA)
        { //reconnect
            if (bls_ll_getConnectionInterval() > 6) { //9
                bls_l2cap_requestConnParamUpdate(DEFAULT_INTERVAL, DEFAULT_INTERVAL, DEFAULT_LATENCY, DEFAULT_TIMEOUT);
            }
            ble_status_aaa = OK_CONNECTED_STATUS_AAA;
        }
		
	#if BLT_APP_LED_ENABLE
        led_ble_Adv_poll(); //LED indicate
	#endif
    }
    else if (ble_status_aaa == OK_CONNECTED_STATUS_AAA)
	{ //connect complete
		if (conn_params_tick == 0)
		{
			conn_params_tick = clock_time()|1;
			//bls_l2cap_requestConnParamUpdate(DEFAULT_INTERVAL, DEFAULT_INTERVAL, DEFAULT_LATENCY, DEFAULT_TIMEOUT);
		}
		else
		{
			if (ccc_flag || clock_time_exceed(connect_begin_tick, 3000000))
			{
				//save_smp_inf();
				
				conn_step = CONNECTED_DONE_AAA;
				ble_status_aaa = T5S_CONNECTED_STATUS_AAA;
			
			#if SHOW_MAST_REAL_MAC_DEBUG
				get_master_real_mac();
			#endif
			
			#if BLT_APP_LED_ENABLE
				printf("--->ble connected\n");
		        led_ble_ConnectedStatus(); //LED indicate reconnected
			#endif
			}
		}
	}
    else if (ble_status_aaa == DEEP_SLEEPE_STATUS_AAA)
    { //ready to enter deep sleep
        if ((ui_ota_is_working == 0) && (!blc_ll_isControllerEventPending()))
        {
            printf("!blc_ll_isControllerEventPending()\n");
            enter_deep_aaa(DEEP_SLEEP_ANA_AAA);
        }
    }

#if (MODULE_DIGITAL_TUBE_ENABLE)
	digital_tube_pro();
#endif

}

#if (MODULE_VOLICE_ADJUST_ENABLE)

extern int voice_wheel_confirm(void);

const static u8 kb_buf_up[2] = {0xe9,0x00};
const static u8 kb_buf_down[2] = {0xea,0x00};
const static u8 release_buf[2]={0,0};

u8 ble_voice_flag = 0;

void volice_wheel_push(void)
{
	static unsigned char sc_wheel_release_cnt = 0;
	s8 voice_flag = 0;
	
	voice_flag = voice_wheel_confirm();

	if ( voice_flag ) 
	{
		if ( fun_mode == RF_1M_BLE_MODE && connect_ok )
		{
			if ( 1 == voice_flag ) 
			{
				ble_voice_flag = 1 ;
				blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, HID_CONSUME_REPORT_INPUT_DP_H, kb_buf_up, 2);
			}
			else if ( -1 == voice_flag )
			{
				ble_voice_flag = 1 ;
				blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, HID_CONSUME_REPORT_INPUT_DP_H, kb_buf_down, 2);
			}
			sc_wheel_release_cnt = 1;
		}
	}
	else
	{
		if (  0 == sc_wheel_release_cnt ) return;
		
		if (fun_mode == RF_1M_BLE_MODE && connect_ok )
		{
			blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, HID_CONSUME_REPORT_INPUT_DP_H, release_buf, 2);
		}
		
		if ( sc_wheel_release_cnt )
		{
			if ( ++sc_wheel_release_cnt >= 3 ) {
				sc_wheel_release_cnt = 0;
				ble_voice_flag = 0;
			}
		}
	}
}

#endif

#if (PROJECT_ID == PID_LPV600)

extern  unsigned char gc_lpv600_key_spec_flag;

const static u8 kb_kz_buf[8]       = {0x01,0,VK_Z,0,0,0,0,0};
const static u8 kb_kenter_buf[8]   = {0x00,0,VK_ENTER,0,0,0,0,0};
const static u8 release_kzz_buf[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void mac_kz_kenter_push(void)
{
	static unsigned char sc_kzz_release_cnt = 0;

	if ( gc_lpv600_key_spec_flag )
	{
		if ( fun_mode == RF_1M_BLE_MODE && connect_ok )
		{
			if ( 1 == gc_lpv600_key_spec_flag )
			{
				blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, HID_NORMAL_KB_REPORT_INPUT_DP_H, kb_kenter_buf, 8);
			}
			else if ( 2 == gc_lpv600_key_spec_flag )
			{
				blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, HID_NORMAL_KB_REPORT_INPUT_DP_H, kb_kz_buf, 8);
			}
			sc_kzz_release_cnt = 1;
			gc_lpv600_key_spec_flag = 0;
		}
	}
	else
	{
		if (  0 == sc_kzz_release_cnt ) return;

		if (fun_mode == RF_1M_BLE_MODE && connect_ok )
		{
			blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, HID_NORMAL_KB_REPORT_INPUT_DP_H, release_kzz_buf, 8);
		}

		if ( sc_kzz_release_cnt )
		{
			if ( ++sc_kzz_release_cnt >= 3 ) {
				sc_kzz_release_cnt = 0;
			}
		}
	}
}
#endif

#if (KEY_FEATURE_DESKTOP_ENABLE)
const static u8 kb_buf[8] = {0x08,0,VK_D,0,0,0,0,0};
const static u8 release_buf[8]={0,0,0,0,0,0,0,0};
unsigned char sk_m115_logo_led_on_flag = 0;

void key_homekey_judement(void)
{
	static unsigned char sv_homekey_release_cnt = 0;

#if (PROJECT_ID == PID_DMS006) || (PROJECT_ID == PID_SMT) || (PROJECT_ID == PID_DM2048)
	// do nothing
#else
	static unsigned int sk_logo_time = 0;
	static unsigned char sk_logo_on_flag = 0, sk_logo_off_flag = 0;
	
	if ( 0 == connect_ok ) { sk_logo_on_flag = 0; sk_logo_time = clock_time();}

	if ( (0 == sk_logo_on_flag) && (1 == connect_ok) )
	{
		if ( clock_time_exceed(sk_logo_time, 4*1000*1000) )
		{
			sk_logo_on_flag = 1;
		}
	}
	
	if ( sk_m115_logo_led_on_flag && sk_logo_on_flag)
	{
	#if (LED_M115_LOGO_LED_ENABLE)
		if ( connect_ok )
		{
			gpio_write( LED_M115_LOGO_LED_PIN, 0 );
			gpio_set_output_en(LED_M115_LOGO_LED_PIN, 1);
			sk_logo_off_flag = 0;
		}
	#endif
	}
	
	if ( 0 == sk_m115_logo_led_on_flag || 0 == connect_ok ) 
	{
	#if (LED_M115_LOGO_LED_ENABLE)
		if ( 0 == sk_logo_off_flag )
		{
			sk_logo_off_flag = 1;
			gpio_write( LED_M115_LOGO_LED_PIN, 0 );
			gpio_set_output_en(LED_M115_LOGO_LED_PIN, 0);
		}
	#endif
	}
#endif

	if ( ms_desktop_flag ) 
	{
		if ( fun_mode == RF_1M_BLE_MODE && connect_ok )
		{
			blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, HID_NORMAL_KB_REPORT_INPUT_DP_H, kb_buf, 8);
		}

		if ( fun_mode == RF_2M_2P4G_MODE && connect_ok ) 
		{
			ms_data.btn = MS_BTN_DESKTOP & 0x9F;
		}

		sv_homekey_release_cnt = 1;
	}
	else
	{
		if (  0 == sv_homekey_release_cnt ) return;
		
		if (fun_mode == RF_1M_BLE_MODE && connect_ok )
		{
			blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, HID_NORMAL_KB_REPORT_INPUT_DP_H, release_buf, 8);
		}

		if ( fun_mode == RF_2M_2P4G_MODE && connect_ok )
		{
			ms_data.btn = 0x00;
		}

		if ( sv_homekey_release_cnt )
		{
			if ( ++sv_homekey_release_cnt >= 10 ) {
				sv_homekey_release_cnt = 0;
			}
		}
	}

	if ( 0 == connect_ok ) {
		sv_homekey_release_cnt = 0;
	}
}

#endif

/**
 * @brief	BLE UI loop
 * @param	none
 * @return	none
 */
 
#if (KEY_FEATURE_DESKTOP_ENABLE) && (KEY_FEATURE_HOME_ENABLE)
const static u8 kb_buf[8] = {0x08,0,VK_D,0,0,0,0,0};
const static u8 release_buf[8]={0,0,0,0,0,0,0,0};
unsigned char sk_m115_logo_led_on_flag = 0;

void key_homekey_judement(void)
{
	static unsigned char sv_homekey_release_cnt = 0;
	static unsigned int sk_logo_time = 0;
	static unsigned char sk_logo_on_flag = 0, sk_logo_off_flag = 0;
	
	if ( 0 == connect_ok ) { sk_logo_on_flag = 0; sk_logo_time = clock_time();}

	if ( (0 == sk_logo_on_flag) && (1 == connect_ok) )
	{
		if ( clock_time_exceed(sk_logo_time, 4*1000*1000) )
		{
			sk_logo_on_flag = 1;
		}
	}
	
	if ( sk_m115_logo_led_on_flag && sk_logo_on_flag)
	{
	#if (LED_M115_LOGO_LED_ENABLE)
		if ( connect_ok )
		{
			gpio_write( LED_M115_LOGO_LED_PIN, 0 );
			gpio_set_output_en(LED_M115_LOGO_LED_PIN, 1);
			sk_logo_off_flag = 0;
		}
	#endif
	}
	
	if ( 0 == sk_m115_logo_led_on_flag || 0 == connect_ok ) 
	{
	#if (LED_M115_LOGO_LED_ENABLE)
		if ( 0 == sk_logo_off_flag )
		{
			sk_logo_off_flag = 1;
			gpio_write( LED_M115_LOGO_LED_PIN, 0 );
			gpio_set_output_en(LED_M115_LOGO_LED_PIN, 0);
		}
	#endif
	}
	
	if ( ms_desktop_flag ) 
	{
		if ( fun_mode == RF_1M_BLE_MODE && connect_ok )
		{
			blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, HID_NORMAL_KB_REPORT_INPUT_DP_H, kb_buf, 8);
		}

		if ( fun_mode == RF_2M_2P4G_MODE && connect_ok ) 
		{
			ms_data.btn = MS_BTN_DESKTOP & 0x9F;
		}

		sv_homekey_release_cnt = 1;
	}
	else
	{
		if (  0 == sv_homekey_release_cnt ) return;
		
		if (fun_mode == RF_1M_BLE_MODE && connect_ok )
		{
			blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, HID_NORMAL_KB_REPORT_INPUT_DP_H, release_buf, 8);
		}

		if ( fun_mode == RF_2M_2P4G_MODE && connect_ok )
		{
			ms_data.btn = 0x00;
		}

		if ( sv_homekey_release_cnt )
		{
			if ( ++sv_homekey_release_cnt >= 10 ) {
				sv_homekey_release_cnt = 0;
			}
		}
	}

	if ( 0 == connect_ok ) {
		sv_homekey_release_cnt = 0;
	}
}

#endif

#if (KEY_FEATURE_MIKE_AI_ENABLE) 

const static u8 kb_mike_buf[8] = {0x08,0,VK_H,0,0,0,0,0};
const static u8 kb_ai_buf[8]   = {0x0a,0,KB_F23,0,0,0,0,0};

const static u8 release_buf[8]={0,0,0,0,0,0,0,0};

void key_mike_ai_judement(void)
{
	static unsigned char sv_mike_ai_flag_release = 0;
	
	if ( ms_mike_ai_flag ) 
	{
		if ( fun_mode == RF_1M_BLE_MODE && connect_ok )
		{
			sv_mike_ai_flag_release = 1;
			if ( 1 == ms_mike_ai_flag ) {
				blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, HID_NORMAL_KB_REPORT_INPUT_DP_H, kb_mike_buf, 8);
			}
			else if ( 2 == ms_mike_ai_flag ) {
				printf("ai assitant\n");
				blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, HID_NORMAL_KB_REPORT_INPUT_DP_H, kb_ai_buf, 8);
				#if (PROJECT_ID == PID_AM8697CD) || (PROJECT_ID == PID_AM8697CD_H2)
					ms_mike_ai_flag = 0;
				#endif
			}
		}
	}
	else
	{
		if ( sv_mike_ai_flag_release ) 
		{
			if (fun_mode == RF_1M_BLE_MODE && connect_ok )
			{
				blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, HID_NORMAL_KB_REPORT_INPUT_DP_H, release_buf, 8);
			}
			
			if ( ++sv_mike_ai_flag_release >= 10 ) sv_mike_ai_flag_release = 0;
		}
	}
}
#endif

extern u8 ble_voice_flag;

#if (PROJECT_ID == PID_M710)
	extern unsigned char batty_cap_not_low(void);
	extern void vbat_led_off(void);
#endif

#if (WHEEL_METHOD_2_ENABLE)
extern char wheel_wheel_check(void);
#endif

#if (KEY_FEATURE_BOSSKEY_ENABLE)

extern key_function_special_en gc_key_special_sta;

const static u8 kb_boss_buf[8] = {0x08,0,VK_D,0,0,0,0,0};
const static u8 release_buf[8] = {0,0,0,0,0,0,0,0};

void key_special_func_judement(void)
{
	static unsigned char sv_spec_flag_release = 0;

	if ( gc_key_special_sta )
	{
		if ( fun_mode == RF_1M_BLE_MODE && connect_ok )
		{
			sv_spec_flag_release = 1;
			if ( KEY_SPE_BOSSKEY == gc_key_special_sta )
			{
				printf("boss key send \n");
				blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, HID_NORMAL_KB_REPORT_INPUT_DP_H, kb_boss_buf, 8);
			}
		}
		gc_key_special_sta = KEY_SPE_NONE;
	}
	else
	{
		if ( sv_spec_flag_release )
		{
			if (fun_mode == RF_1M_BLE_MODE && connect_ok )
			{
				blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, HID_NORMAL_KB_REPORT_INPUT_DP_H, release_buf, 8);
			}

			if ( ++sv_spec_flag_release >= 5 ) sv_spec_flag_release = 0;
		}
	}
}

#endif
void ui_loop()
{
    get_ble_data_report_aaa(); //get km data

    loop_cnt++;

    if (has_new_key_event)
    {
        reset_idle_status();
    }

#if (PM_SYS_LOW_POWER_ENABLE)
	if ( connect_ok && has_new_key_event )
	{
		if ( mcu_sleep_status.enter_lower )
		{
			mcu_sleep_status.moved = 1;
		}
	}
#endif

	idle_status_poll();

	ble_status_proc_aaa(has_new_key_event); //BLE Status polling

#if (MODULE_VOLICE_ADJUST_ENABLE)
	 volice_wheel_push();
#endif

#if (PROJECT_ID == PID_LPV600)
	mac_kz_kenter_push();
#endif

#if (WHEEL_LEFT_RIGHT_ENABLE)
	#if (WHEEL_METHOD_2_ENABLE)
		ms_data.wheel_level = wheel_wheel_check();
	#else
		ms_data.wheel_level = voice_wheel_confirm();
	#endif
#endif


    if (connect_ok && (ble_status_aaa >= T5S_CONNECTED_STATUS_AAA))
    {
        ble_notify_data_proc_aaa(); //notify data to master
    }
    else
    {
        ms_data.wheel = 0;
        ms_data.wheel_level = 0;
    }

	#if (BLT_APP_LED_ENABLE)
    	device_led_process();
    	if ( led_batt_dpi_working_on() )
    	{
    		vbat_led_proc();
    		batt_check_time_reload();
    	}
	#endif

	led_ble_conned_succ_dis_next();


#if (KEY_FEATURE_MIKE_AI_ENABLE)
	 key_mike_ai_judement();
#endif

#if (KEY_FEATURE_BOSSKEY_ENABLE)
	key_special_func_judement();
#endif
    //connect_params_proc(); //connect parameter update process
}

/**
 * @brief	BLE power manager
 * @param	none
 * @return	none
 */

void ble_pm_aaa()
{
#if (PM_SYS_LOW_POWER_ENABLE)
	if ( 0 == mcu_sleep_status.enter_lower )
	{
		return;
	}
#endif

#if (PROJECT_ID == PID_535)
	if ( idle_count <= 13 )
	{
		return;
	}
#endif

#if BLE_APP_PM_ENABLE
	if ( led_batt_dpi_working_on() || DEVICE_LED_BUSY || ui_ota_is_working || (loop_cnt < LONG_SUSPEND_TIMER_AAA) || btn_value||(idle_count<3))
    { //Data need notify to master, set latency = 0 and disable wake up source
        bls_pm_setManualLatency(0);
		bls_pm_setWakeupSource(0);

        if (suspend_wake_up_enable)
        {
		#if WHEEL_FUN_ENABLE_AAA
            wheel_set_wakeup_level_suspend(0);
		#endif
		#if BUTTON_FUN_ENABLE_AAA
            btn_set_wakeup_level_suspend(0);
		#endif
		#if (SENSOR_FUN_ENABLE_AAA && SENSOR_MOTION_ENABLE)
            sensor_set_wakeup_level_suspend(0);
		#endif
        }
        suspend_wake_up_enable = 0;
    }
    else
    { //No data need notify, enable wake up source
        loop_cnt = LONG_SUSPEND_TIMER_AAA + 1;
        bls_pm_setWakeupSource(PM_WAKEUP_PAD);
	#if(ADC_VBAT_TO_BTN)
		bls_pm_setManualLatency(13);
	#endif
        if (suspend_wake_up_enable == 0)
        {
		#if WHEEL_FUN_ENABLE_AAA
            wheel_set_wakeup_level_suspend(1);
		#endif
		#if BUTTON_FUN_ENABLE_AAA
            btn_set_wakeup_level_suspend(1);
		#endif
		#if (SENSOR_FUN_ENABLE_AAA && SENSOR_MOTION_ENABLE)
            sensor_set_wakeup_level_suspend(1);
		#endif
        }
        suspend_wake_up_enable = 1;
    }

	#if (PM_DEEPSLEEP_RETENTION_ENABLE)
	    if (suspend_wake_up_enable && (idle_count > 3))
	    {
	        bls_pm_setSuspendMask(SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
	    }
	    else
	    {
	        bls_pm_setSuspendMask(SUSPEND_ADV | SUSPEND_CONN);
	    }
	#else
	    //Enter suspend
	    bls_pm_setSuspendMask(SUSPEND_ADV | SUSPEND_CONN);
	#endif
#endif
}

#if BLT_SOFTWARE_TIMER_ENABLE
int soft_timer_suspend_proc(void)
{
    ui_loop();

    return MOUSE_TIMER_SHORT_T;
}

void add_soft_time()
{
    if (soft_time_flag == 0)
    {
        if (blt_timer.currentNum == 0)
        {
            blt_soft_timer_add(&soft_timer_suspend_proc, MOUSE_TIMER_SHORT_T);
        }
        soft_time_flag = 1;
    }
}

void delet_soft_time()
{
    if (soft_time_flag == 1)
    {
        if (blt_timer.currentNum)
        {
            blt_soft_timer_delete(&soft_timer_suspend_proc);
        }
        soft_time_flag = 0;
    }
}

void check_softe_time()
{
    u16 interval = bls_ll_getConnectionInterval();
    if ((interval > DEFAULT_INTERVAL))
    {
        add_soft_time();
    }
    else
    {
        delet_soft_time();
    }
}
#endif


void main_loop(void)
{
    blc_sdk_main_loop();

    u16 time_interval = 7500;

#if (BLT_SOFTWARE_TIMER_ENABLE)
	blt_soft_timer_process(MAINLOOP_ENTRY);

	check_softe_time();
#else
	u16 interval = bls_ll_getConnectionInterval();

	//soft_time_flag = 0;

	if (interval > 0)
	{
		u8 txFifoNumber = blc_ll_getTxFifoNumber();

		/* When TX FIFO is used more, the sampling interval is lengthened */
		if (txFifoNumber < 4)
		{
			time_interval = interval * 1250 - 1000;
		}
		else if (txFifoNumber < 6)
		{
			time_interval = interval * 625 * 3;
		}
		else
		{
			time_interval = interval * 625 * 5;
		}
	}
	else
	{
		time_interval = 7500;
	}
#endif

#if PM_SYS_LOW_POWER_ENABLE
	time_interval = 7500;
#endif

#if  (BLE_APP_PM_ENABLE==0)
	// soft_time_flag = 1;
	time_interval = 7500;
#endif


	if ( clock_time_exceed(start_tick, time_interval) )
	{
		start_tick = clock_time();
		ui_loop();
	}

    has_new_key_event = 0;
	
    ////////////////////////////////////// PM Process /////////////////////////////////
#if BLE_APP_PM_ENABLE
	ble_pm_aaa();
#else
	bls_pm_setSuspendMask(SUSPEND_DISABLE);
#endif

#if (PM_SYS_LOW_POWER_ENABLE)
	if ( 0 == mcu_sleep_status.enter_lower )
	{
		bls_pm_setSuspendMask(SUSPEND_DISABLE);
	}
#endif

#if (PROJECT_ID == PID_535)
	if ( idle_count <= 13 )
	{
		bls_pm_setSuspendMask(SUSPEND_DISABLE);
	}
#endif
    //blt_pm_proc();
}


