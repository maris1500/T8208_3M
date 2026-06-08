/********************************************************************************************************
 * @file     AAA_24G_RF.c
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

#if G24_MODE_ENABLE

#define  RF_TX_TO_RX_AUTO_MODE_ENABLE  0    //tx to tx auto mode

#define  RF_FAST_TX_SETTING_ENABLE  0    //fast tx enable

#define  RF_FAST_RX_SETTING_ENABLE  0	//fast rx enable

#define  RX_FRAME_SIZE     64

_attribute_data_retention_user volatile unsigned char rx_packet[RX_FRAME_SIZE]  __attribute__((aligned(4)));//rx_packet 64 bytes with 4 aligned
//_attribute_data_retention_user volatile unsigned char ack_payload[RX_FRAME_SIZE];

_attribute_data_retention_user volatile unsigned char rf_state = 0;//rf_state parameter
_attribute_data_retention_user volatile unsigned int rf_rx_timeout_us = D24G_PAIR_TIMER_OUT;


//u32 ll_chn_mask = LL_CHANNEL_SEARCH_FLAG;

_attribute_data_retention_user u8 device_channel = 0;//device frequency channel 

_attribute_data_retention_user volatile unsigned char device_ack_received = 0;//device ack received paremeter


//_attribute_data_retention_user int		device_sync = 0;

//-------------------- rf init------------------
#define FRE_OFFSET 	0
//#define FRE_STEP 	5

const unsigned char rf_chn[5] =
{
    FRE_OFFSET + 5, FRE_OFFSET + 17, FRE_OFFSET + 30,//5,17,30
    FRE_OFFSET + 45, FRE_OFFSET + 60,//45,60
};

#if (RF_FAST_TX_SETTING_ENABLE||RF_FAST_RX_SETTING_ENABLE)//fast tx or rx enable

volatile u32 tick_rf_fast_setting=0;
volatile int rf_fast_setting_flag=0;//0 init 1:time out  2:enable fast set
#if RF_FAST_RX_SETTING_ENABLE

/**
 * @brief       This function init rf_rx fast settle
 * @return      
 * @note        
 */
_attribute_ram_code_sec_ void rf_rx_fast_settle_init()
{
	write_reg8(0x1284,(read_reg8(0x1284)&0x87)|(0x6<<3));
	//write_reg8(0x1284,(read_reg8(0x1284))|0x37);
	//rx timing sequence
	write_reg8(0x12b0,0x00);
	write_reg8(0x12b1,read_reg8(0x12b1)&0xfe);
	write_reg8(0x12b2,0x10);
	write_reg8(0x12b3,read_reg8(0x12b3)&0xfe);
	write_reg8(0x12b4,0x10);
	write_reg8(0x12b5,read_reg8(0x12b5)&0xfe);
	write_reg8(0x12b6,0x32);
	write_reg8(0x12b7,read_reg8(0x12b7)&0xfe);
	write_reg8(0x12b8,0x58);
	write_reg8(0x12b9,read_reg8(0x12b9)&0xfe);
	write_reg8(0x12ba,0x58 );
	write_reg8(0x12bb,read_reg8(0x12bb)&0xfe);
}


/**
 * @brief       This function set_rx_fast_settle en
 * @return      
 * @note        
 */
_attribute_ram_code_sec_ void rf_rx_fast_settle_en(void)
{
	write_reg8(0x1229,read_reg8(0x1229)|0x01);
}

/**
 * @brief       This function set fast_settle dis
 * @return      
 * @note        
 */
_attribute_ram_code_sec_ void rf_rx_fast_settle_dis(void)
{
	write_reg8(0x1229,read_reg8(0x1229)&0xfe);
}
#endif

#if RF_FAST_TX_SETTING_ENABLE //if rf tx setting enable

/**
 * @brief       This function init rf_tx fast settle
 * @return      
 * @note        
 */
_attribute_ram_code_sec_ void rf_tx_fast_settle_init()
{
	write_reg8(0x1284,(read_reg8(0x1284)&0xf8)|0x02);

	//write_reg8(0x1284,(read_reg8(0x1284))|0x02);

	write_reg8(0x12a4,0x00);
	write_reg8(0x12a5,read_reg8(0x12a5)&0xfe);
	write_reg8(0x12a6,0x10);
	write_reg8(0x12a7,read_reg8(0x12a7)&0xfe);
	write_reg8(0x12a8,0x62);
	write_reg8(0x12a9,read_reg8(0x12a9)&0xfe);
	write_reg8(0x12aa,0x64);
	write_reg8(0x12ab,read_reg8(0x12ab)&0xfe);
	write_reg8(0x12ac,0x6a);
	write_reg8(0x12ad,read_reg8(0x12ad)&0xfe);
	write_reg8(0x12ae,0x6a);
	write_reg8(0x12af,read_reg8(0x12af)&0xfe);
	write_reg8(0x12bc,0x62);
	write_reg8(0x12bd,read_reg8(0x12bd)&0xfe);
}


/**
 * @brief       This function set rf_tx fast settle en
 * @return      
 * @note        
 */
_attribute_ram_code_sec_ void rf_tx_fast_settle_en(void)
{
	write_reg8(0x1229,read_reg8(0x1229)|0x02);
}

/**
 * @brief       This function set rf_tx fast settle dis
 * @return      
 * @note        
 */
_attribute_ram_code_sec_ void rf_tx_fast_settle_dis(void)
{
	write_reg8(0x1229,read_reg8(0x1229)&0xfd);
}
#endif//end if rf tx fast setting enable
#endif//end if rf tx or rx fast setting enable


/**
 * @brief       This function set to init rf drv private 2m
 * @return      
 * @note        
 */
_attribute_ram_code_sec_ void rf_drv_private_2m_init()
{
	rf_mode_init();//init rf mode
	rf_set_pri_2M_mode();//set pri 2m mode
	//// add end

	write_reg16(0xf06, 0);//rx wait time
	write_reg8(0x0f0c, 0x54); //rx settle time: Default 150us, minimum 85us(0x54+1)

	write_reg16(0xf0e, 0);//tx wait time
	write_reg8(0x0f04, 0x70);//tx settle time: Default 150us, minimum 113us(0x70+1)

	write_reg8(0xf10, 0);// wait time on NAK

#if (RF_FAST_TX_SETTING_ENABLE||RF_FAST_RX_SETTING_ENABLE)	//if rf fast tx or rx enable
	rf_fast_setting_flag=0;//fast setting flag set 0
	tick_rf_fast_setting=clock_time()|1;//update rf fast setting time
	#if RF_FAST_RX_SETTING_ENABLE	//must fist  call rx fast settle init
		rf_rx_fast_settle_init();//init rx fast settle init first
		rf_rx_fast_settle_dis();//set rf fast settle dis
	#endif

	#if RF_FAST_TX_SETTING_ENABLE	
		rf_tx_fast_settle_init();//init tx fast settle init first
		rf_tx_fast_settle_dis();//set tx fast settle dis
	#endif
#endif

	reg_dma_chn_en |= FLD_DMA_CHN_RF_RX  | FLD_DMA_CHN_RF_TX;//enalbe dma rf rx and dma rf tx 
	rf_rx_buffer_set((u8 *)rx_packet, RX_FRAME_SIZE, 0);//set rx packet

	write_reg8(0x405, (read_reg8(0x405)&0xf8)|5); //access_byte_num[2:0]

#if TB903_D8373_ENABLE
	write_reg8(0x420, 35);
#else

		write_reg8(0x420, 38);
	#if PAIRED_WITH_8366_DONGLE
		write_reg8(0x402, 0x48);//preamble length
	#else
		write_reg8(0x402, 0x44);//preamble length
	#endif

#endif


	rf_set_tx_rx_off();//reset tx rx mode
#if MCU_CORE_B80B
	rf_rx_acc_code_enable(0x01);//set rf acc_code
	rf_tx_acc_code_select(0);//set rx acc_code chan
#endif
	irq_disable();//irq disable
	irq_clr_src();//clear irq src
	rf_irq_src_allclr();//clear all rf irq src
	irq_enable_type(FLD_IRQ_ZB_RT_EN); //enable RF irq
	rf_irq_disable(FLD_RF_IRQ_ALL);//disable all rf irq
	rf_irq_enable(FLD_RF_IRQ_TX | FLD_RF_IRQ_RX | FLD_RF_IRQ_RX_TIMEOUT|FLD_RF_IRQ_FIRST_TIMEOUT); //enable tx,rx,rx timeout,first time out irq
	irq_enable();//irq enable
}


/**
 * @brief       This function set rf srx time and time out
 * @param[in]   rx_timeout	- rx time out time
 * @param[in]   tick	- 
 * @return      
 * @note        
 */
_attribute_ram_code_sec_  void my_rf_start_srx  (unsigned int tick,unsigned int rx_timeout)
{
    write_reg8 (0x800f00, 0x80);						// disable srx
	write_reg32 (0x800f28, rx_timeout-1);				// first timeout
	write_reg32(0x800f18, tick);
    write_reg8(0x800f16, read_reg8(0x800f16) | 0x04);	// Enable cmd_schedule mode
	write_reg16 (0x800f00, 0x3f86);						// srx
}


/**
 * @brief       This function set single tx mode tx packet addr and tick
 * @param[in]   addr	- packet addr
 * @param[in]   tick	- 
 * @return      
 * @note        
 */
void my_rf_start_stx  (void* addr, unsigned int tick)
{
	write_reg32(0x800f18, tick);
    write_reg8(0x800f16, read_reg8(0x800f16) | 0x04);	// Enable cmd_schedule mode
	write_reg8 (0x800f00, 0x85);						// single TX
	write_reg16 (0x800c0c, (unsigned short)((unsigned int)addr));//set packet addr
}


/**
 * @brief       This function  set single tx to rx mode packet addr and rf rx timeout
 * @param[in]   addr	- the addr of packet need send
 * @param[in]   tick	- 
 * @param[in]   timeout_us	- rf rx time out
 * @return      
 * @note        
 */
void rf_start_stxtorx(void *addr, unsigned int tick, unsigned int timeout_us)
{
    write_reg16(0x800f0a, timeout_us - 1);
    write_reg32(0x800f18, tick);
    write_reg8(0x800f16, read_reg8(0x800f16) | 0x04);	 // Enable cmd_schedule mode
    write_reg16(0x800c0c, (unsigned short)((unsigned int)addr));
    write_reg8(0x800f00, 0x87);	   // single tx2rx
}


/**
 * @brief       This function set pair access code
 * @param[in]   code	- access code
 * @return      
 * @note        
 */
void set_pair_access_code(u32 code)
{
    write_reg32(0x800408, ((code & 0xffffff00) | 0x71));
    write_reg8(0x80040c, code);
}


/**
 * @brief       This function set data access code
 * @param[in]   code	- access code
 * @return      
 * @note        
 */
void set_data_access_code(u32 code)
{
#if MORE_PIPE_ENABLE
    write_reg32(0x800408, ((code & 0xffffff00) | 0x77)); //mouse pipe1
    //write_reg32(0x800408, ((code & 0xffffff00) | 0x03)); //keyboard pipe2
#else
    write_reg32(0x800408, ((code & 0xffffff00) | 0x77));
#endif
    write_reg8(0x80040c, code);
}


/**
 * @brief       This function clr all rf irq src
 * @return      
 * @note        
 */
void rf_irq_src_allclr()
{
    reg_rf_irq_status = 0xffff;
}


/**
 * @brief       This function read payload data
 * @param[in]   payload	- the buff to store rx packet
 * @return      
 * @note        
 */
u8  Read_Payload(u8 *payload)
{
    u8 *rx_pkt;
    u8 ret, i;

    rx_pkt = (u8 *) rx_packet ;//pointer rx_packet

    rf_rx_buffer_set((u8 *)rx_packet, RX_FRAME_SIZE, 0);//set rx buff

    ret = rx_pkt[4] & 0x3f; //get length of rx paylaod

    for (i = 0; i < (ret + 5); i++)
    {
        payload[i] = rx_pkt[i];
    }
    return 1;
}


/**
 * @brief       This function deal 2m private irq
 * @return      
 * @note        
 */
_attribute_ram_code_sec_ void irq_handle_private_2m()
{
	unsigned short rf_irq_src = rf_irq_src_get();//get irq src

	if(rf_irq_src)
	{
		if (rf_irq_src & FLD_RF_IRQ_RX)//if irq src is rf rx
		{
			//PIN_DEBUG_RF_STATE_CHANGE_TOGGLE;//debug rf state change
			PIN_DEBUG_RF_RX_LEVEL(0);
			rf_irq_clr_src(FLD_RF_IRQ_RX);//clear rf rx src
			rf_state = RF_RX_END_STATUS;//change rf state to rf_rx_end
		}

		if (rf_irq_src & FLD_RF_IRQ_TX)//if irq src is rf tx
		{
			//PIN_DEBUG_RF_STATE_CHANGE_TOGGLE;//debug rf state change
			PIN_DEBUG_RF_TX_LEVEL(0);
			rf_irq_clr_src(FLD_RF_IRQ_TX);//clear rf tx src
			rf_state = RF_RX_START_STATUS;//change rf state to rf_rx_start

		#if (RF_TX_TO_RX_AUTO_MODE_ENABLE==0)			
			my_rf_start_srx(clock_time(), rf_rx_timeout_us);//single rx time out set
		#endif
		}
	#if (RF_TX_TO_RX_AUTO_MODE_ENABLE==1)//if tx to rx auto mode enable
		if (rf_irq_src & FLD_RF_IRQ_RX_TIMEOUT)//if irq src is rf rx time out
		{
			PIN_DEBUG_RF_RX_TIMEOUT_LEVEL(0);//debug rf_rx time out	
			rf_irq_clr_src(FLD_RF_IRQ_RX_TIMEOUT);//clear irq rx timeout src
			rf_state = RF_RX_TIMEOUT_STATUS;//change to rf state to rf_rx_timeout status
		}
	#else
		if (rf_irq_src & FLD_RF_IRQ_FIRST_TIMEOUT)
		{
			PIN_DEBUG_RF_RX_TIMEOUT_LEVEL(0);//debug rf_rx time out	
			rf_irq_clr_src(FLD_RF_IRQ_FIRST_TIMEOUT);//clear irq first timeout src
			rf_state = RF_RX_TIMEOUT_STATUS;//change rf_state to rf_rx_timeout status
		}
	#endif
		irq_clr_src();//clear irq all src
	}
}


/**
 * @brief       This function get next channel
 * @param[in]   chn	- 
 * @param[in]   mask	- 
 * @return      
 * @note        
 */
_attribute_ram_code_sec_  u8 get_next_channel_with_mask(u32 mask, u8 chn)
{
	PIN_DEBUG_RF_CHN_NEXT_TOGGLE;
#if (HOPING_FREQ==0)
   	return (chn+1)%5;//get next chn
#else
	int chn_high = (mask >> 4) & 0x0f;
	if (mask & LL_CHANNEL_SEARCH_FLAG) {
		return LL_NEXT_CHANNEL (chn);
	}
	else if (chn_high != chn) {
		return chn_high;
	}
	else {
		return mask & 0x0f;
	}
#endif
}


/**
 * @brief       This function check rf fasting setting time
 * @return      
 * @note        
 */
void check_rf_fast_setting_time()
{
#if (RF_FAST_TX_SETTING_ENABLE||RF_FAST_RX_SETTING_ENABLE)
	if(clock_time_exceed(tick_rf_fast_setting, 2000000))
	{
		tick_rf_fast_setting=clock_time();
		rf_fast_setting_flag=1;
	}
#endif
}


/**
 * @brief       This function check rf fasting setting flag
 * @return      
 * @note        
 */
_attribute_ram_code_sec_ void check_rf_fast_setting_flag()
{
#if (RF_FAST_TX_SETTING_ENABLE||RF_FAST_RX_SETTING_ENABLE)
	if(rf_fast_setting_flag==1)//timeout
	{
		rf_fast_setting_flag=0;//defualt
	}
	else
	{
		rf_fast_setting_flag=2;//enable rf fast setting flag
	}
#endif
}


/**
 * @brief       This function set rf_tx_rx setting
 * @return      
 * @note        
 */
_attribute_ram_code_sec_ void rf_set_tx_rx_setting()
{
#if (RF_FAST_TX_SETTING_ENABLE||RF_FAST_RX_SETTING_ENABLE)
	if(rf_fast_setting_flag<2)
	{
	#if RF_FAST_TX_SETTING_ENABLE
		write_reg8(0x0f04, 0x70);//tx settle time: Default 150us, minimum 113us(0x70+1)
		rf_tx_fast_settle_dis();
	#endif
	
	#if RF_FAST_RX_SETTING_ENABLE
		write_reg8(0x0f0c, 0x54); //rx settle time: Default 150us, minimum 85us(0x54+1)
		rf_rx_fast_settle_dis();
	#endif
	}
	else //if(rf_ca_cnt==1)
	{
	#if RF_FAST_TX_SETTING_ENABLE
		write_reg8(0x0f04, 70);//tx settle time: Default 150us, minimum 113us(0x70+1)
		rf_tx_fast_settle_en();
	#endif

	#if RF_FAST_RX_SETTING_ENABLE
		write_reg8(0x0f0c, 54); //rx settle time: Default 150us, minimum 85us(0x54+1)
		rf_rx_fast_settle_en();
	#endif
	}
		
#endif
}


/**
 * @brief       This function set single tx to rx 
 * @param[in]   p	- packet pointer
 * @param[in]   rx_timeout_us	- rf rx timeout
 * @return      
 * @note        
 */
_attribute_ram_code_sec_ void rf_stx_to_rx(u8 *p,u32 rx_timeout_us)
{
#if (RF_FAST_TX_SETTING_ENABLE||RF_FAST_RX_SETTING_ENABLE)
	rf_set_tx_rx_setting();
#endif
	PIN_DEBUG_RF_TX_LEVEL(1);
	PIN_DEBUG_RF_RX_LEVEL(1);
	PIN_DEBUG_RF_RX_TIMEOUT_LEVEL(1);
	PIN_DEBUG_RF_RX_CRC_OK_LEVEL(1);
	PIN_DEBUG_RF_RX_DATA_OK_LEVEL(1);

#if RF_TX_TO_RX_AUTO_MODE_ENABLE
	rf_trx_state_set(RF_MODE_AUTO, rf_chn[device_channel]);
	rf_start_stxtorx(p, ClockTime(), rx_timeout_us);
#else
	rf_set_channel(rf_chn[device_channel], 0);//set RF's channel
	my_rf_start_stx(p, ClockTime());//rf single tx start
#endif
}


/**
 * @brief       This function deal with rf rx packet
 * @param[in]   p_rf_data	- rf rx packet
 * @return      
 * @note        
 */
extern u8  rf_rx_process(rf_packet_t *p_pkt);
_attribute_ram_code_sec_ void irq_device_rx(void)
{
	//Read_Payload();
	if(RF_TPLL_PACKET_CRC_OK(rx_packet)&&RF_TPLL_PACKET_LENGTH_OK(rx_packet))
	{
		PIN_DEBUG_RF_RX_CRC_OK_LEVEL(0);

		//rf_packet_t *p = (rf_packet_t *)(rx_packet);
		if (rf_rx_process((rf_packet_t *)rx_packet) /*&& ll_chn_tick != p->tick*/)
		{
			PIN_DEBUG_RF_RX_DATA_OK_LEVEL(0);
			
			// ll_chn_tick = p->tick;	

			// device_sync = 1;
			
			device_ack_received = 1;
		#if (HOPING_FREQ==1)
			device_channel = get_next_channel_with_mask(ll_chn_mask, device_channel);
		#endif
			//ll_chn_rx_tick = clock_time();

		#if TEST_LOST_RATE
			user_rx_right_data_cnt++;
		#endif
		}
	}

	//rf_rx_buffer_set((u8 *)rx_packet, RX_FRAME_SIZE, 0);
	rx_packet[0] = 1;
}
#endif
