/********************************************************************************************************
 * @file     emi.c
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
 * @date     May. 12, 2018
 *
 * @par      Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
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
/*
 * emi.c
 *
 *  Created on: 2018-5-31
 *      Author: Administrator
 */
#include "AAA_public_config.h"

#if EMI_TEST_FUN_ENABLE_AAA

enum {
	EMI_cmd_carrier = 0,
	EMI_cmd_cd		= 1,
	EMI_cmd_rx		= 2,
	EMI_cmd_tx		= 3,
};

_attribute_data_retention_user u8 mode = 0;
_attribute_data_retention_user u8 channel = 0;
_attribute_data_retention_user u8 run_flag = 0;
u8 enter_emi_flag = 0;

/**
 * @brief	push carrier to fifo
 * @param	none
 * @return	none
 */
void emi_cd_push_fifo()
{
	if (enter_emi_flag && (mode == EMI_cmd_cd))
	{
		rf_continue_mode_run();
	}
}

/**
 * @brief	Switch EMI mode or channel by button
 * @param	none
 * @return	none
 */
_attribute_ram_code_ u8 emi_has_change()
{
    static u32 tick;
    u16 now_value = 0;

    if (clock_time_exceed(tick, 10000))
    {
        tick = clock_time();

	#if BUTTON_FUN_ENABLE_AAA
        now_value = btn_scan(); //get button value
	#endif

        if (last_btn_value != now_value)
        {
            if (last_btn_value == KEY_BTN_LEFT)
            {
                channel = (channel + 1) % 0x03;
                run_flag = 1;
            }
            else if (last_btn_value == KEY_BTN_MIDDLE)
            {
                mode = (mode + 1) % 0x04;
                run_flag = 1;
            }
        }

        last_btn_value = now_value;
    }

    return run_flag;
}

void emicarrieronly(RF_ModeTypeDef rf_mode, RF_PowerTypeDef pwr, signed char rf_chn)
{
    rf_emi_single_tone(pwr, rf_chn);
	
    while (!emi_has_change())
    {

    }
	
    rf_emi_stop();
}

#if 0
void emi_con_prbs9(RF_ModeTypeDef rf_mode, RF_PowerTypeDef pwr, signed char rf_chn)
{
    rf_emi_tx_continue_setup(rf_mode, pwr, rf_chn, 0);

    while (!emi_has_change())
    {
        rf_continue_mode_run();
    }
    rf_emi_stop();
}

void emi_con_tx0f(RF_ModeTypeDef rf_mode, RF_PowerTypeDef pwr, signed char rf_chn)
{
    rf_emi_tx_continue_setup(rf_mode, pwr, rf_chn, 1);

    while (!emi_has_change())
    {
        rf_continue_mode_run();
    }
    rf_emi_stop();
}
#endif

void emi_carrier_data(RF_ModeTypeDef rf_mode, RF_PowerTypeDef pwr, signed char rf_chn, u8 data_type)
{
    rf_emi_tx_continue_setup(rf_mode, pwr, rf_chn, data_type);

    while (!emi_has_change())
    {
        rf_continue_mode_run();
    }
	
    rf_emi_stop();
}


#if 0
void emitxprbs9(RF_ModeTypeDef rf_mode, RF_PowerTypeDef pwr, signed char rf_chn)
{
    rf_emi_tx_brust_setup(rf_mode, pwr, rf_chn, 0);
    while (!emi_has_change())
    {
        rf_emi_tx_brust_loop(rf_mode, 0);
    }
    rf_emi_stop();
}

void emitx0f(RF_ModeTypeDef rf_mode, RF_PowerTypeDef pwr, signed char rf_chn)
{
    rf_emi_tx_brust_setup(rf_mode, pwr, rf_chn, 1);
    while (!emi_has_change())
    {
        rf_emi_tx_brust_loop(rf_mode, 1);
    }
    rf_emi_stop();
}
#endif

void emi_tx_mode(RF_ModeTypeDef rf_mode, RF_PowerTypeDef pwr, signed char rf_chn,u8 data_type)
{
    rf_emi_tx_burst_setup(rf_mode, pwr, rf_chn, data_type);
	
    while (!emi_has_change())
    {
        rf_emi_tx_burst_loop(rf_mode, data_type);
    }
	
    rf_emi_stop();
}

void emirx(RF_ModeTypeDef rf_mode, RF_PowerTypeDef pwr, signed char rf_chn)
{
    rf_emi_rx(rf_mode, rf_chn);
	
    while (!emi_has_change())
    {
        rf_emi_rx_loop();
#if 0
        if (rf_emi_get_rxpkt_cnt() != read_reg32(0x84000c))
        {
            write_reg8(RF_RSSI, rf_emi_get_rssi_avg());
            write_reg32(RF_REC_NUMB, rf_emi_get_rxpkt_cnt());
        }
#endif
    }
	
    rf_emi_stop();
}

/**
 * @brief	emi_init
 * @param	none
 * @return	none
 */
void emi_init()
{
    write_reg32(0x408,0x29417671 );//access code  0xf8118ac9
	//write_reg8(0x13c,0x10); // print buffer size set

	//write_reg8(0x40005,tx_cnt);//tx_cnt
	//write_reg8(0x40006,run);//run
	//write_reg8(0x40007,cmd_now);//cmd
	//write_reg8(0x40008,power_level);//power
	//write_reg8(0x40009,chn);//chn
	//write_reg8(0x4000a,mode);//mode
	//write_reg8(0x4000b,hop);//hop
	//write_reg8(0x40004,0);
	//write_reg32(0x4000c,0);
	//gpio_shutdown(GPIO_ALL);//for pm

	analog_write(0x33,0xff);
}

/**
 * @brief	EMI test loop
 * @param	none
 * @return	none
 */
void emi_process()
{
    u8 tbl_chn[3] = {05, 30, 70};
    u16 now_value = 0;
	int rf_mode = RF_MODE_PRIVATE_2M;

	printf("---emi_process.\r\n");

	if (fun_mode == RF_1M_BLE_MODE)
	{	
		u16 retry_cnt = 500;

		bls_ll_setAdvEnable(0);
		
		while ((BLS_LINK_STATE_CONN == blc_ll_getCurrentState()) &&  ((--retry_cnt) > 0))
		{
			bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN);
			
			sleep_ms(20);
		}
	}
	
    while (1) //waite for btn release
    {
        now_value = btn_scan();
		
        if (now_value == 0)
        {
            break;
        }
    }

	printf("---EMI key release.\n");

#if (BLT_APP_LED_ENABLE) && (LED_G24_ENABLE)
    gpio_set_output_en(PIN_24G_LED, 1);
    gpio_write(PIN_24G_LED, LED_OFF);
#endif

    enter_emi_flag = 1;

    /* RF initial, IQR disable */
    emi_init();
    rf_mode_init();
	rf_set_pri_2M_mode();
    run_flag = 1;
	irq_disable();
	
	/* EMI test loop */
    while (1)
    {
        if (run_flag)
        {
        	printf("---mode=%d,channel=%d.\n",mode,channel);
			
            run_flag = 0;

		#if (BLT_APP_LED_ENABLE) && (LED_G24_ENABLE)
            for (u8 i=0; i<(mode+1); i++)
            {
                sleep_us(150000 * (channel + 1));
                gpio_write(PIN_24G_LED, LED_ON);
                sleep_us(150000 * (channel + 1));
                gpio_write(PIN_24G_LED, LED_OFF);
            }
		#endif

            if (mode == 0) //carri
            {
                emicarrieronly(rf_mode, user_cfg.emi_tx_power, tbl_chn[channel]);
            }
            else if (mode == 1) //cd
            {
                //data type: 2 : tx55  , 1: tx0f
                emi_carrier_data(rf_mode, user_cfg.emi_tx_power, tbl_chn[channel], 2);
            }
            else if (mode == 2) //rx
            {
                emirx(rf_mode, user_cfg.emi_tx_power, tbl_chn[channel]);
            }
            else if (mode == 3) //tx
            {
                emi_tx_mode(rf_mode, user_cfg.emi_tx_power, tbl_chn[channel], 1);
            }
        }
    }
}

/**
 * @brief	EMI key check
 * @param	check_ms
 * @return	none
 */
void emi_key_check_aaa(u16 check_ms)
{
	static u32 key_press_hold_tick = 0;

	if (btn_value == COMB_BTN_EMI)
	{
		//my_fifo_reset(&fifo_km); //Clear EMI key

		if (clock_time_exceed(key_press_hold_tick,check_ms*1000))
		{
			power_on_tick = 0;
			
			enter_emi_flag = 1; //set enter_emi_flag

		#if SENSOR_FUN_ENABLE_AAA
			OPTSensor_Shutdown();
		#endif
		
		#if (MODULE_WATCHDOG_ENABLE)
			wd_stop();
		#endif

		#if (MODULE_32K_WATCHDOG_ENABLE)
			wd_32k_stop();
		#endif
		
			emi_process();  //EMI test loop
		}
	}
	else
	{
		key_press_hold_tick = clock_time();
	}
}

#endif
