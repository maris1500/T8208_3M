#include "AAA_mcu_ext.h"

#if (MODULE_MCU_EXTERNAL_ENABLE)
#include "../8208_dual_mouse/AAA_public_config.h"
#include "../8208_dual_mouse/main.h"
#endif

#if (MODULE_MCU_EXTERNAL_ENABLE)

#define IO_1_TIME 900
#define IO_0_TIME 300

extern unsigned char gl_cpi_change_flag;

static unsigned int sg_mcu_send_time = 0;
static unsigned int sg_mcu_conn_time = 0;

static unsigned char sg_mcu_send_conn_times = 0;

ext_mcu_status_t   ext_mcu_io;

ext_mcu_lvd_logo_t ext_mcu_lvd_logo = {
.lvd_send_times = 3,
};


extern unsigned char mcu_mode_pair_status(void);

void ext_mcu_send_char(unsigned char cha)
{
	unsigned char i = 0;

	for (i = 0; i < 8; i++)
	{
		if ( cha & 0x80 )
		{
			 EXT_IO_OUTPUT(EXT_IO_1);	sleep_us(IO_1_TIME);
			 EXT_IO_OUTPUT(EXT_IO_0);	sleep_us(IO_0_TIME);
		}
		else
		{
			 EXT_IO_OUTPUT(EXT_IO_1);	sleep_us(IO_0_TIME);
			 EXT_IO_OUTPUT(EXT_IO_0);	sleep_us(IO_1_TIME);
		}
		cha <<= 1;
	}

	EXT_IO_OUTPUT(EXT_IO_1);	sleep_us(300);
	EXT_IO_OUTPUT(EXT_IO_0);	sleep_us(3000);

}

void ext_mcu_conn_status(void)
{
	if ( EXT_LED_PAIR == ext_mcu_io.conn )
	{
		sg_mcu_send_conn_times = 0;
	
		if ( RF_2M_2P4G_MODE == mcu_mode_get() )
		{
			ext_mcu_send_char( G24_CODE_PAIR_ING );
			printf(" g24 pairing : 0x%1x\n", G24_CODE_PAIR_ING);
		}
		else
		{
			if ( BLE_DEVICE_ID_0 == ble_mode_device_id_get() )
			{
				ext_mcu_send_char( BLE1_CODE_PAIR_ING );
				printf(" ble pairing : 0x%1x\n", BLE1_CODE_PAIR_ING);
			}
			else if ( BLE_DEVICE_ID_1 == ble_mode_device_id_get() )
			{
			#if LED_BT2_ENABLE
				ext_mcu_send_char( BLE2_CODE_PAIR_ING );
			#endif
			}
		}
	}

	if ( EXT_LED_RECONN == ext_mcu_io.conn )
	{
		sg_mcu_send_conn_times = 0;

		if ( RF_2M_2P4G_MODE == mcu_mode_get() )
		{
			ext_mcu_send_char( G24_CODE_CONN_ING );
			printf(" g24 reconing : 0x%1x\n", G24_CODE_CONN_ING);
		}
		else
		{
			if ( BLE_DEVICE_ID_0 == ble_mode_device_id_get() )
			{
				ext_mcu_send_char( BLE1_CODE_CONN_ING );
				printf(" ble reconing : 0x%1x\n", BLE1_CODE_CONN_ING);
			}
			else if ( BLE_DEVICE_ID_1 == ble_mode_device_id_get() )
			{
			#if LED_BT2_ENABLE
				ext_mcu_send_char( BLE2_CODE_CONN_ING );
			#endif
			}
		}
	}

	if ( EXT_LED_CONN_SUCC == ext_mcu_io.conn && sg_mcu_send_conn_times < 3)
	{
		if ( RF_2M_2P4G_MODE == mcu_mode_get() )
		{
			ext_mcu_send_char( G24_CODE_CONN_SUCC );
			printf(" g24 conn succ: 0x%1x\n", G24_CODE_CONN_SUCC);
		}
		else 
		{
			if ( BLE_DEVICE_ID_0 == ble_mode_device_id_get() )
			{
				ext_mcu_send_char( BLE1_CODE_CONN_SUCC );
				printf(" ble conn succ: 0x%1x\n", BLE1_CODE_CONN_SUCC);
			}
			else if ( BLE_DEVICE_ID_1 == ble_mode_device_id_get() )
			{
			#if LED_BT2_ENABLE
				ext_mcu_send_char( BLE2_CODE_CONN_SUCC );
			#endif
			}
		}
		
		sg_mcu_send_conn_times++;
	}
}

void ext_mcu_lvd_send(void)
{
	if ( 0 == connect_ok ) return;
	
#if (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
	if ( idle_count >= 10 )
	{
		ext_mcu_lvd_logo.lvd_send_times = 0;
		if ( ext_mcu_lvd_logo.logo_send_times <3 )
		{
			ext_mcu_lvd_logo.logo_send_times++;
			ext_mcu_send_char( LVD_CODE_OFF );
		}
		return;
	}
#endif

	if ( ext_mcu_lvd_logo.lvd_send_times < 3 )
	{
		if ( 1 == ext_mcu_lvd_logo.lvd_act )
		{
			ext_mcu_send_char( LVD_CODE_ON );
			printf("lvd on : 0x%1x\n", LVD_CODE_ON);
		}
		else
		{
			ext_mcu_send_char( LVD_CODE_OFF );
			printf("lvd off : 0x%1x\n", LVD_CODE_OFF);
		}

		if( ++ext_mcu_lvd_logo.lvd_send_times >= 3 )
		{
			ext_mcu_lvd_logo.lvd_send_times = 3;
			ext_mcu_lvd_logo.lvd_act = 0;
		}
	}

#if (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
	ext_mcu_lvd_logo.logo_send_times = 0;
#endif
}

#if (PROJECT_ID == PID_HJY505V2)
void ext_mcu_logo_send(void)
{
	static unsigned int sv_logo_delay_time = 0;
	static unsigned char sv_logo_start_flag = 0;
	
	if ( 0 == connect_ok ) 
	{ 
		sv_logo_delay_time = clock_time();
		sv_logo_start_flag = 0;
		return; 
	}

	if ( 0 == sv_logo_start_flag )
	{
		if ( !clock_time_exceed(sv_logo_delay_time, 4000*1000) )
		{
			return;
		}
		sv_logo_start_flag = 1;
	}

	if ( (2 == sv_logo_start_flag || 3 == sv_logo_start_flag) && (ext_mcu_lvd_logo.logo_send_times < 3) )
	{
		if ( ext_mcu_lvd_logo.logo_act )
		{
			ext_mcu_send_char( LOGO_CODE_ON );
			printf("logo on : 0x%1x, %d\n", LOGO_CODE_ON, idle_count);
		}
		else
		{
			ext_mcu_send_char( LOGO_CODE_OFF );
			printf("logo off : 0x%1x, %d\n", LOGO_CODE_OFF, idle_count);
		}

		if ( ++ext_mcu_lvd_logo.logo_send_times >= 3)
		{
			ext_mcu_lvd_logo.logo_send_times = 3;
		}
	}
	
	if ( (idle_count < LED_RGB_RUN_TIME) &&  (1== sv_logo_start_flag || 3 == sv_logo_start_flag) )
	{
		ext_mcu_lvd_logo.logo_act = 1;
		ext_mcu_lvd_logo.logo_send_times = 0;
		sv_logo_start_flag = 2;
		printf("--->1111 %d\n", idle_count);
	}

	if ( (idle_count >= LED_RGB_RUN_TIME) &&  (2 == sv_logo_start_flag) )
	{
		ext_mcu_lvd_logo.logo_act = 0;
		ext_mcu_lvd_logo.logo_send_times = 0;
		sv_logo_start_flag = 3;
		printf("--->222 %d\n", idle_count);
	}
}
#endif


void ext_mcu_dpi_send(void)
{
	static unsigned char sv_dpi_send_times = 0;
	
	if ( 0 == connect_ok ) { gl_cpi_change_flag = 0; return; }
	
	if ( 0 == gl_cpi_change_flag ) { sv_dpi_send_times = 0; return; }

	if ( sv_dpi_send_times < 3)
	{
		ext_mcu_send_char( LOGO_DPI_ON + dpi_value );
		printf("dpi on : 0x%1x, %d\n", (LOGO_DPI_ON + dpi_value), (dpi_value+1) );
	}

	if ( ++sv_dpi_send_times >= 3 )
	{
		sv_dpi_send_times = 3;
		gl_cpi_change_flag = 0;
	}
}

void ext_mcu_dpi_time_reset(void)
{
	sg_mcu_send_time = clock_time();
}

void ext_mcu_pro(void)
{
	if ( clock_time_exceed(sg_mcu_conn_time, 100*1000) )
	{
		sg_mcu_conn_time = clock_time();
		ext_mcu_conn_status();
	}
	
	if ( clock_time_exceed(sg_mcu_send_time, 250*1000) )
	{
		sg_mcu_send_time = clock_time();
		ext_mcu_lvd_send();
		ext_mcu_dpi_send();
	}
}

void ext_mcu_prepare(void)
{
	unsigned char i = 0;

	if ( mcu_mode_pair_status() )
	{
		ext_mcu_io.conn = EXT_LED_PAIR;
	}
	else
	{
		ext_mcu_io.conn = EXT_LED_RECONN;
	}

	for (i = 0; i < 3; i++)
	{
		if ( EXT_LED_PAIR == ext_mcu_io.conn )
		{
			if ( RF_2M_2P4G_MODE == mcu_mode_get() )
			{
				ext_mcu_send_char( G24_CODE_PAIR_ING );
			}
			else
			{
				if ( BLE_DEVICE_ID_0 == ble_mode_device_id_get() )
				{
					ext_mcu_send_char( BLE1_CODE_PAIR_ING );
				}
			}
		}
		else if ( EXT_LED_RECONN == ext_mcu_io.conn )
		{
			if ( RF_2M_2P4G_MODE == mcu_mode_get() )
			{
				ext_mcu_send_char( G24_CODE_CONN_ING );
			}
			else
			{
				if ( BLE_DEVICE_ID_0 == ble_mode_device_id_get() )
				{
					ext_mcu_send_char( BLE1_CODE_CONN_ING );
				}
			}
		}

		sleep_ms(50);
	}
}

void ext_mcu_enter_sleep(void)
{
	unsigned char i = 0;
	for ( i = 0; i < 3; i++)
	{
		ext_mcu_send_char( LED_EVT_ALL_OFF );
		sleep_ms(50);
	}

}

#endif
