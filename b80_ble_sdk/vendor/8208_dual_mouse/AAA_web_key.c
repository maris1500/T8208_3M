#include "AAA_web_key.h"

#if WEB_KEY_FEATURE_ENABLE
	#include "AAA_Public_KeyValue.h"
	#include "AAA_public_config.h"
	#include "main.h"
#endif

#if WEB_KEY_FEATURE_ENABLE

const unsigned char sg_assitant_keycode[8][2] =
{
	{0x01, KB_LCtrl},
	{0x02, KB_LShift},
	{0x04, KB_LAlt},
	{0x08, KB_LWin},

	{0x10, KB_RCtrl},
	{0x20, KB_RShift},
	{0x40, KB_RAlt},
	{0x80, KB_RWin},
};


web_set_status_t gc_web_sta_list;

static unsigned int sg_web_key_release_time = 0;

unsigned short int web_key_left_function(void)
{
	unsigned short int now_value = 0x00;

	web_key_special_tab[KEY_LEFT_INDEX-1] = WEB_KEY_NOMAL;
	
	if ( web_key_nomal(KEY_LEFT_INDEX) )
	{
		now_value |= gc_web_data.key[KEY_LEFT_INDEX - 1].value ;
	}
	else if ( web_key_dpi(KEY_LEFT_INDEX) )
	{
		now_value |= KEY_BTN_CPI;
		web_key_special_tab[KEY_LEFT_INDEX-1] = WEB_KEY_DPI;
	}
	else if ( web_key_office(KEY_LEFT_INDEX) )
	{
		now_value |= KEY_WEB_OFFICE;
		web_key_special_tab[KEY_LEFT_INDEX-1] = WEB_KEY_OFFICE;
	}
	else
	{
		
	}

	return (now_value);
}


unsigned short int web_key_right_function(void)
{
	unsigned short int now_value = 0x00;

	web_key_special_tab[KEY_RIGHT_INDEX-1] = WEB_KEY_NOMAL;
	
	if ( web_key_nomal(KEY_RIGHT_INDEX) )
	{
		now_value |= gc_web_data.key[KEY_RIGHT_INDEX - 1].value ;
	}
	else if ( web_key_dpi(KEY_RIGHT_INDEX) )
	{
		now_value |= KEY_BTN_CPI;
		web_key_special_tab[KEY_RIGHT_INDEX-1] = WEB_KEY_DPI;
	}
	else if ( web_key_office(KEY_RIGHT_INDEX) )
	{
		now_value |= KEY_WEB_OFFICE;
		web_key_special_tab[KEY_RIGHT_INDEX-1] = WEB_KEY_OFFICE;
	}
	else
	{
	
	}

	return (now_value);

}


unsigned short int web_key_middle_function(void)
{
	unsigned short int now_value = 0x00;
	
	web_key_special_tab[KEY_MIDDLE_INDEX-1] = WEB_KEY_NOMAL;
	
	if ( web_key_nomal(KEY_MIDDLE_INDEX) )
	{
		now_value |= gc_web_data.key[KEY_MIDDLE_INDEX - 1].value ;
	}
	else if ( web_key_dpi(KEY_MIDDLE_INDEX) )
	{
		now_value |= KEY_BTN_CPI;
		web_key_special_tab[KEY_MIDDLE_INDEX-1] = WEB_KEY_DPI;
	}
	else if ( web_key_office(KEY_MIDDLE_INDEX) )
	{
		now_value |= KEY_WEB_OFFICE;
		web_key_special_tab[KEY_MIDDLE_INDEX-1] = WEB_KEY_OFFICE;
	}
	else
	{
	
	}

	return (now_value);
}

unsigned short int web_key_k4_function(void)
{
	unsigned short int now_value = 0x00;

	web_key_special_tab[KEY_K4_INDEX-1] = WEB_KEY_NOMAL;
	
	if ( web_key_nomal(KEY_K4_INDEX) )
	{
		now_value |= gc_web_data.key[KEY_K4_INDEX - 1].value ;
	}
	else if ( web_key_dpi(KEY_K4_INDEX) )
	{
		now_value |= KEY_BTN_CPI;
		web_key_special_tab[KEY_K4_INDEX-1] = WEB_KEY_DPI;
	}
	else if ( web_key_office(KEY_K4_INDEX) )
	{
		now_value |= KEY_WEB_OFFICE;
		web_key_special_tab[KEY_K4_INDEX-1] = WEB_KEY_OFFICE;
	}
	else
	{
	
	}

	return (now_value);
}

unsigned short int web_key_k5_function(void)
{
	unsigned short int now_value = 0x00;
	
	web_key_special_tab[KEY_K5_INDEX-1] = WEB_KEY_NOMAL;
	
	if ( web_key_nomal(KEY_K5_INDEX) )
	{
		now_value |= gc_web_data.key[KEY_K5_INDEX - 1].value ;
	}
	else if ( web_key_dpi(KEY_K5_INDEX) )
	{
		now_value |= KEY_BTN_CPI;
		web_key_special_tab[KEY_K5_INDEX-1] = WEB_KEY_DPI;
	}
	else if ( web_key_office(KEY_K5_INDEX) )
	{
		now_value |= KEY_WEB_OFFICE;
		web_key_special_tab[KEY_K5_INDEX-1] = WEB_KEY_OFFICE;
	}
	else
	{
	
	}

	return (now_value);

}

unsigned short int web_key_dpi_function(void)
{
	unsigned short int now_value = 0x00;

	web_key_special_tab[KEY_DPI_INDEX-1] = WEB_KEY_NOMAL;
	
	if ( web_key_nomal(KEY_DPI_INDEX) )
	{
		now_value |= gc_web_data.key[KEY_DPI_INDEX - 1].value ;
	}
	else if ( web_key_dpi(KEY_DPI_INDEX) )
	{
		now_value |= KEY_BTN_CPI;
		web_key_special_tab[KEY_DPI_INDEX-1] = WEB_KEY_DPI;
	}
	else if ( web_key_office(KEY_DPI_INDEX) )
	{
		now_value |= KEY_WEB_OFFICE;
		web_key_special_tab[KEY_DPI_INDEX-1] = WEB_KEY_OFFICE;
	}
	else
	{
	
	}

	return (now_value);

}

void web_key_office_data_load(void)
{
	
}

void web_key_function_process(void)
{
	unsigned char i = 0;
	unsigned char sendbuff[8] = {0x00};

	if ( KEY_TRIGGER_OFFICE == gc_web_sta_list.trigger )
	{
		for (i = KEY_START_INDEX; i < KEY_MAX_INDEX; i++)
		{
			if ( WEB_KEY_OFFICE == web_key_special_tab[i] )
			{
				sendbuff[0] = gc_web_data.key[i].value;
				sendbuff[2] = gc_web_data.key[i].func;

				gc_web_sta_list.release = 1;
				push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, sendbuff, sizeof(sendbuff));//push to fifo
				
				printf("Sendoffice:%1x %1x \n", sendbuff[0], sendbuff[2]);
			}
		}

		gc_web_sta_list.trigger = KEY_TRIGGER_NONE;
	}


	if ( gc_web_sta_list.release )
	{
		if (  clock_time_exceed(sg_web_key_release_time, 50*1000) )
		{
			sg_web_key_release_time = clock_time();
			push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, sendbuff, sizeof(sendbuff));//push to fifo
			if (++gc_web_sta_list.release >= 5 )
			{
				gc_web_sta_list.release = 0;
			}
		}
	}
	else
	{
		sg_web_key_release_time = clock_time();
	}
}

void web_function_loop(void)
{
	web_key_function_process();
}


#endif
