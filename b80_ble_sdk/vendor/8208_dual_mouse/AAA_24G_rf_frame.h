/********************************************************************************************************
 * @file     AAA_24G_rf_frame.h
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

/*
 * led_rf_frame.h
 *
 *  Created on: Jan 13, 2014
 *      Author: xuzhen
 */

#ifndef _APP_24G_RF_FRAME_H_
#define _APP_24G_RF_FRAME_H_

// #define MOUSE_DATA_LEN_AAA  6



enum
{
    PIPE_PARING			= 0x00,
    PIPE_MOUSE			= 0x01,
    PIPE_KEYBOARD		= 0x02,
    //PIPE_AUDIO			= 0x03,
    //PIPE_TOUCH			= 0x04,
    //PIPE_RC				= 0x05,
    PIPE_ALL   = 0X3F,
};


enum
{
    EMPTY_CMD = 0x00,
    EMPTY_ACK_CMD = 1,
    
    PAIR_CMD = 2,
    PAIR_ACK_CMD = 3,

	MOUSE_CMD = 4,
	MOUSE_ACK_CMD = 5,

	KB_CMD = 6,
	KB_ACK_CMD = 7,

	RECONNECT_CMD = 8,
	RECONNECT_ACK_CMD = 9,

#if Dongle_Lark_A1_Flash_Enable
	AUTO_PAIR_CMD = 0x40,
#endif
};

typedef struct
{
    u8 btn;
#if (MOUSE_DATA_LEN_AAA==6)
    s16 x;
    s16 y;
#elif (MOUSE_DATA_LEN_AAA==4)
    s8 x;
    s8 y;
#endif
    s8 wheel;
    s8 wheel_level;
} mouse_data_t;


typedef struct
{
	u32 did;

	u8 key[12];
}AES_KEY;

typedef struct
{
    u32 dma_len;

    u8  rf_len;
    u8  dat[59];
} rf_packet_t;


typedef struct
{	
	u8  cmd; //data type
	u16 fw_version; //frimware version
	u32 did;
	u8  key[12];
} pair_data_t;

typedef struct
{	
	u8 cmd; //data type
	u8 seq_no;
	u8 pno_no;
	
	u8 tick_0;
	u8 tick_1;
	u8 chn;
	u8 host_led_status;
	
	u32 gid;
	u32 did;
	u8 key[12];
} pair_ack_data_t;


typedef struct
{
	u8	cmd; //bit7=0: no aes  =1: aes
	u8	seq_no; //frame count
	u8	pn_no;
	
	u32 did; //device id

	u8  km_dat[6]; //data or km

	u8  rsv1[3]; //for aes  16 bytes
	u16 crc16;
} km_3_c_1_data_t;

typedef struct
{
	
	u8 cmd;//data type
	u8 seq_no;
	u8 pno_no;
	
	u8 tick_0;
	u8 tick_1;
	u8 chn;
	u8 host_led_status;
	
} km_ack_data_t;


#endif /* LED_RF_FRAME_H_ */

