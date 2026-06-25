#include "AAA_web_key.h"

#if WEB_KEY_FEATURE_ENABLE
	#include "AAA_Public_KeyValue.h"
	#include "AAA_public_config.h"
	#include "main.h"
#endif

#if WEB_KEY_FEATURE_ENABLE

web_set_status_t gc_web_sta_list;
web_key_fire_t   gc_web_fire_sta;   

static unsigned int sg_web_key_release_time = 0, sg_web_key_fire_time = 0;


extern u8 push_usb_fifo_aaa(u8 type,u8 *buf,u8 len);


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
	else 
	{
		
	}


	return (now_value);

}

void web_fire_usb_send(unsigned char direct )
{
	mouse_data_t mdata = {0};

	if ( 0 == gc_web_sta_list.firekey ) { sg_web_key_fire_time = clock_time(); return; }

	if ( direct || clock_time_exceed(sg_web_key_fire_time, gc_web_fire_sta.interval*1000) )
	{
		mdata.btn = 0x01;
		if ( gc_web_fire_sta.times )
		{
			push_usb_fifo_aaa(MOUSE_DATA_TYPE, &mdata.btn, sizeof(mouse_data_t));
			usb_aut_report_time_reset();
			gc_web_fire_sta.times--;
			printf("fir00\n");
		}
		sg_web_key_fire_time = clock_time();
	}

	if ( gc_web_fire_sta.interval <= 100 )
	{
		 if ( clock_time_exceed(sg_web_key_release_time,  (1000 * gc_web_fire_sta.interval / 2) ) )
		 {
		 	mdata.btn = 0x00;
		 	push_usb_fifo_aaa(MOUSE_DATA_TYPE, &mdata.btn, sizeof(mouse_data_t));
		 	sg_web_key_release_time = clock_time();
			usb_aut_report_time_reset();
			if ( 0 == gc_web_fire_sta.times )
			{
				gc_web_sta_list.release_count ++;
				printf("fir11\n");
			}
		 }
	}
	else
	{
		 if ( clock_time_exceed(sg_web_key_release_time,  100*1000) )
		 {
		 	sg_web_key_release_time = clock_time();
			mdata.btn = 0x00;
			push_usb_fifo_aaa(MOUSE_DATA_TYPE, &ms_data.btn, sizeof(mouse_data_t));
			usb_aut_report_time_reset();
			printf("firaa\n");
			if ( 0 == gc_web_fire_sta.times )
			{
				gc_web_sta_list.release_count ++;
				printf("fir22\n");
			}
		 }
	}

	if ( gc_web_sta_list.release_count >= 10 )
	{
		gc_web_sta_list.firekey = 0;
		gc_web_fire_sta.times = 0;
		gc_web_sta_list.release_count = 0;
		printf("fir33\n");
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

				gc_web_sta_list.release_type = 1;
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

				gc_web_sta_list.release_type = 2;
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

				gc_web_sta_list.release_type = 3;
				sg_web_key_release_time = clock_time();

				web_fire_usb_send(1);
		
				printf("Sendfire:%1x %1x \n", gc_web_fire_sta.interval, gc_web_fire_sta.times);
			}
		}
		gc_web_sta_list.trigger = KEY_TRIGGER_NONE;
	}

	if ( 1 == gc_web_sta_list.release_type )
	{
		if (  clock_time_exceed(sg_web_key_release_time, 10*1000) )
		{
			sg_web_key_release_time = clock_time();
			push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, sendbuff, sizeof(sendbuff));//push to fifo
			if (++gc_web_sta_list.release_count >= 10 )
			{
				gc_web_sta_list.release_count = 0;
				gc_web_sta_list.release_type = 0;
			}
		}
	}
	else if ( 2 == gc_web_sta_list.release_type  )
	{
		if (  clock_time_exceed(sg_web_key_release_time, 10*1000) )
		{
			sg_web_key_release_time = clock_time();
			push_usb_fifo_aaa(CONSUME_DATA_TYPE, sendbuff, 2);//push to fifo
			if (++gc_web_sta_list.release_count >= 10 )
			{
				gc_web_sta_list.release_type = 0;
				gc_web_sta_list.release_count = 0;
			}
		}
	}
	else if ( 3 == gc_web_sta_list.release_type )
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
