/********************************************************************************************************
 * @file     AAA_24G_RF.h
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

#ifndef _APP_RF_LINK_LAYER_24_H_
#define _APP_RF_LINK_LAYER_24_H_

#include "types.h"


extern u8	device_channel;
extern u32  ll_chn_mask;
extern u8	get_next_channel_with_mask(u32 mask, u8 chn);
extern void irq_device_tx(void);
extern void irq_device_rx(void);
extern void ll_device_init(void);

typedef void (*task_when_rf_func)(void);
extern  task_when_rf_func p_task_when_rf;

extern void rf_drv_private_2m_init();
extern void set_pair_access_code(u32 code);
extern void set_data_access_code(u32 code);


extern void rf_set_access_code1(u32 code);
extern u32  rf_get_access_code1();
extern void rf_irq_src_allclr();
extern void irq_handle_private_2m();
extern int  rf_trx_state_set(RF_StatusTypeDef rf_status, signed char rf_channel);
extern void rf_acc_len_set(unsigned char len);
extern void rf_receiving_pipe_enble(u8 channel_mask);
extern void rf_stx_to_rx(u8 *p,u32 rx_timeout_us);



#endif

