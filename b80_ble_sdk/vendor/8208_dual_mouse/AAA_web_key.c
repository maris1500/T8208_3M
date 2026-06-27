#include "AAA_web_key.h"

#if WEB_KEY_FEATURE_ENABLE
	#include "AAA_Public_KeyValue.h"
	#include "AAA_public_config.h"
	#include "main.h"
#endif

#if WEB_KEY_FEATURE_ENABLE

web_set_status_t gc_web_sta_list;
web_key_fire_t   gc_web_fire_sta;   

static unsigned int sg_web_key_release_time = 0, sg_web_key_fire_time = 0, sg_web_key_macro_time = 0;


extern u8 push_usb_fifo_aaa(u8 type,u8 *buf,u8 len);

void web_key_special_reset(unsigned char index)
{
	web_key_special_tab[index-1] = WEB_KEY_NOMAL;
}

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
	else if ( web_key_media(KEY_LEFT_INDEX) )
	{
		now_value |= KEY_WEB_MEDIA;
		web_key_special_tab[KEY_LEFT_INDEX-1] = WEB_KEY_MEDIA;
	}
	else if ( web_key_fire(KEY_LEFT_INDEX) )
	{
		now_value |= KEY_WEB_FIRE;
		web_key_special_tab[KEY_LEFT_INDEX-1] = WEB_KEY_FIRE;
	}
	else if ( web_key_macro(KEY_LEFT_INDEX) )
	{
		now_value |= KEY_WEB_MACRO;
		web_key_special_tab[KEY_LEFT_INDEX-1] = WEB_KEY_MACRO;
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
	else if ( web_key_media(KEY_RIGHT_INDEX) )
	{
		now_value |= KEY_WEB_MEDIA;
		web_key_special_tab[KEY_RIGHT_INDEX-1] = WEB_KEY_MEDIA;
	}
	else if ( web_key_fire(KEY_RIGHT_INDEX) )
	{
		now_value |= KEY_WEB_FIRE;
		web_key_special_tab[KEY_RIGHT_INDEX-1] = WEB_KEY_FIRE;
	}
	else if ( web_key_macro(KEY_RIGHT_INDEX) )
	{
		now_value |= KEY_WEB_MACRO;
		web_key_special_tab[KEY_RIGHT_INDEX-1] = WEB_KEY_MACRO;
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
	else if ( web_key_media(KEY_MIDDLE_INDEX) )
	{
		now_value |= KEY_WEB_MEDIA;
		web_key_special_tab[KEY_MIDDLE_INDEX-1] = WEB_KEY_MEDIA;
	}
	else if ( web_key_fire(KEY_MIDDLE_INDEX) )
	{
		now_value |= KEY_WEB_FIRE;
		web_key_special_tab[KEY_MIDDLE_INDEX-1] = WEB_KEY_FIRE;
	}
	else if ( web_key_macro(KEY_MIDDLE_INDEX) )
	{
		now_value |= KEY_WEB_MACRO;
		web_key_special_tab[KEY_MIDDLE_INDEX-1] = WEB_KEY_MACRO;
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
	else if ( web_key_media(KEY_K4_INDEX) )
	{
		now_value |= KEY_WEB_MEDIA;
		web_key_special_tab[KEY_K4_INDEX-1] = WEB_KEY_MEDIA;
	}
	else if ( web_key_fire(KEY_K4_INDEX) )
	{
		now_value |= KEY_WEB_FIRE;
		web_key_special_tab[KEY_K4_INDEX-1] = WEB_KEY_FIRE;
	}
	else if ( web_key_macro(KEY_K4_INDEX) )
	{
		now_value |= KEY_WEB_MACRO;
		web_key_special_tab[KEY_K4_INDEX-1] = WEB_KEY_MACRO;
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
	else if ( web_key_media(KEY_K5_INDEX) )
	{
		now_value |= KEY_WEB_MEDIA;
		web_key_special_tab[KEY_K5_INDEX-1] = WEB_KEY_MEDIA;
	}
	else if ( web_key_fire(KEY_K5_INDEX) )
	{
		now_value |= KEY_WEB_FIRE;
		web_key_special_tab[KEY_K5_INDEX-1] = WEB_KEY_FIRE;
	}
	else if ( web_key_macro(KEY_K5_INDEX) )
	{
		now_value |= KEY_WEB_MACRO;
		web_key_special_tab[KEY_K5_INDEX-1] = WEB_KEY_MACRO;
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
	else if ( web_key_media(KEY_DPI_INDEX) )
	{
		now_value |= KEY_WEB_MEDIA;
		web_key_special_tab[KEY_DPI_INDEX-1] = WEB_KEY_MEDIA;
	}
	else if ( web_key_fire(KEY_DPI_INDEX) )
	{
		now_value |= KEY_WEB_FIRE;
		web_key_special_tab[KEY_DPI_INDEX-1] = WEB_KEY_FIRE;
	}
	else if ( web_key_macro(KEY_DPI_INDEX) )
	{
		now_value |= KEY_WEB_MACRO;
		web_key_special_tab[KEY_DPI_INDEX-1] = WEB_KEY_MACRO;
	}
	else
	{
		
	}

	return (now_value);

}


void web_fire_usb_send(unsigned char direct )
{
	if ( 0 == gc_web_sta_list.firekey ) { sg_web_key_fire_time = clock_time(); return; }

	if ( direct || clock_time_exceed(sg_web_key_fire_time, gc_web_fire_sta.interval*1000) )
	{
		ms_data.btn |= 0x01;
		if ( gc_web_fire_sta.times || web_key_fire_pressed() )
		{
			push_usb_fifo_aaa(MOUSE_DATA_TYPE, &ms_data.btn, sizeof(mouse_data_t));

			gc_web_sta_list.release_count = 0;
			sg_web_key_release_time = clock_time();

			if ( gc_web_fire_sta.times )
			{
				gc_web_fire_sta.times--;
			}
			printf("fir00 ");
		}
		sg_web_key_fire_time = clock_time();
	}

	if ( gc_web_fire_sta.times || web_key_fire_pressed() )
	{
		if ( clock_time_exceed(sg_web_key_release_time,  gc_web_fire_sta.interval*600) )
		{
			ms_data.btn &= 0xFE;
			if ( 0 == gc_web_sta_list.release_count  )
			{
				push_usb_fifo_aaa(MOUSE_DATA_TYPE, &ms_data.btn, sizeof(mouse_data_t));
				gc_web_sta_list.release_count = 1;
				printf("11\n");
			}
			sg_web_key_release_time = clock_time();
		}
	}
	else
	{
		if ( clock_time_exceed(sg_web_key_release_time,  10000) )
		{
			ms_data.btn &= 0x00;

			push_usb_fifo_aaa(MOUSE_DATA_TYPE, &ms_data.btn, sizeof(mouse_data_t));
			sg_web_key_release_time = clock_time();
			gc_web_sta_list.release_count ++;
			printf("f22\n");
		}
	}

	if ( gc_web_sta_list.release_count >= 10 )
	{
		gc_web_sta_list.firekey = 0;
		gc_web_fire_sta.times = 0;
		gc_web_sta_list.release_count = 0;
		gc_web_sta_list.release_type = KEY_RELEASE_NONE;
	}

}


void web_macro_usb_send(void)
{
	if ( 0 == gc_web_sta_list.macrokey ) { sg_web_key_macro_time = clock_time(); return; }

	if ( clock_time_exceed(sg_web_key_macro_time, gc_web_fire_sta.interval*1000) )
	{
		sg_web_key_macro_time = clock_time();
	}

	if ( gc_web_fire_sta.times )
	{
		if ( clock_time_exceed(sg_web_key_release_time,  gc_web_fire_sta.interval*600) )
		{
			sg_web_key_release_time = clock_time();
		}
	}
	else
	{
		if ( clock_time_exceed(sg_web_key_release_time,  10000) )
		{
			
		}
	}

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

				gc_web_sta_list.release_type = KEY_RELEASE_OFFICE;
				push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, sendbuff, sizeof(sendbuff));//push to fifo
				
				printf("Sendoffice:%1x %1x \n", sendbuff[0], sendbuff[2]);
			}
		}

		gc_web_sta_list.trigger = KEY_TRIGGER_NONE;
	}
	else if ( KEY_TRIGGER_MEDIA == gc_web_sta_list.trigger )
	{
		for (i = KEY_START_INDEX; i < KEY_MAX_INDEX; i++)
		{
			if ( WEB_KEY_MEDIA == web_key_special_tab[i] )
			{
				sendbuff[0] = gc_web_data.key[i].value;
				sendbuff[1] = gc_web_data.key[i].func;

				gc_web_sta_list.release_type = KEY_RELEASE_MEDIA;
				push_usb_fifo_aaa(CONSUME_DATA_TYPE, sendbuff, 2);//push to fifo
				
				printf("Sendmedia:%1x %1x \n", sendbuff[0], sendbuff[2]);
			}
		}

		gc_web_sta_list.trigger = KEY_TRIGGER_NONE;
	}
	else if ( KEY_TRIGGER_FIRE == gc_web_sta_list.trigger )
	{
		for (i = KEY_START_INDEX; i < KEY_MAX_INDEX; i++)
		{
			if ( WEB_KEY_FIRE == web_key_special_tab[i] )
			{
				gc_web_sta_list.firekey = 1;
				
				gc_web_fire_sta.interval = gc_web_data.key[i].value;
				gc_web_fire_sta.times    = gc_web_data.key[i].func;

				if ( gc_web_fire_sta.interval <= 5 )
				{
					gc_web_fire_sta.interval = 5;
				}

				gc_web_sta_list.release_type = KEY_RELEASE_FIRE;
				sg_web_key_release_time = clock_time();

				web_fire_usb_send(1);
		
				printf("Sendfire:%1x %1x \n", gc_web_fire_sta.interval, gc_web_fire_sta.times);
			}
		}
		gc_web_sta_list.trigger = KEY_TRIGGER_NONE;
	}
	else if (  KEY_TRIGGER_MACRO == gc_web_sta_list.trigger )
	{
		for (i = KEY_START_INDEX; i < KEY_MAX_INDEX; i++)
		{
			if ( WEB_KEY_MACRO == web_key_special_tab[i] )
			{
				gc_web_sta_list.macrokey = 1;

				gc_web_sta_list.release_type = KEY_RELEASE_MACRO;
				sg_web_key_release_time = clock_time();

				web_fire_usb_send(1);
		
				printf("Sendfire:%1x %1x \n", gc_web_fire_sta.interval, gc_web_fire_sta.times);
			}
		}
		gc_web_sta_list.trigger = KEY_TRIGGER_NONE;
	}

	if ( KEY_RELEASE_OFFICE == gc_web_sta_list.release_type )
	{
		if (  clock_time_exceed(sg_web_key_release_time, 10*1000) )
		{
			sg_web_key_release_time = clock_time();
			push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, sendbuff, sizeof(sendbuff));//push to fifo
			if (++gc_web_sta_list.release_count >= 10 )
			{
				gc_web_sta_list.release_count = 0;
				gc_web_sta_list.release_type = KEY_RELEASE_NONE;
			}
		}
	}
	else if ( KEY_RELEASE_MEDIA == gc_web_sta_list.release_type  )
	{
		if (  clock_time_exceed(sg_web_key_release_time, 10*1000) )
		{
			sg_web_key_release_time = clock_time();
			push_usb_fifo_aaa(CONSUME_DATA_TYPE, sendbuff, 2);//push to fifo
			if (++gc_web_sta_list.release_count >= 10 )
			{
				gc_web_sta_list.release_type = KEY_RELEASE_NONE;
				gc_web_sta_list.release_count = 0;
			}
		}
	}
	else if ( KEY_RELEASE_FIRE == gc_web_sta_list.release_type || KEY_RELEASE_MACRO == gc_web_sta_list.release_type )
	{
		
	}
	else
	{
		sg_web_key_release_time = clock_time();
	}

	web_fire_usb_send(0);
	
}

void web_function_loop(void)
{
	web_key_function_process();
}


#endif
