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

unsigned char macro_buff[8] = {0x00};

#if USB_MODE_ENABLE
	extern u8 push_usb_fifo_aaa(u8 type,u8 *buf,u8 len);
#endif

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
		#if USB_MODE_ENABLE
			push_usb_fifo_aaa(MOUSE_DATA_TYPE, &ms_data.btn, sizeof(mouse_data_t));
		#endif
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
			#if USB_MODE_ENABLE
				push_usb_fifo_aaa(MOUSE_DATA_TYPE, &ms_data.btn, sizeof(mouse_data_t));
			#endif
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
		#if USB_MODE_ENABLE
			push_usb_fifo_aaa(MOUSE_DATA_TYPE, &ms_data.btn, sizeof(mouse_data_t));
		#endif
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

unsigned char web_key_macro_max_index_get(unsigned char k)
{
	unsigned char i = 0, n = 0;

	for ( i = 0; i < MACRO_NUM_MAX; i++)
	{
		if ( gc_web_data.macro[k][i].valA || gc_web_data.macro[k][i].valB || gc_web_data.macro[k][i].valC )
		{
			n++;
		}
		else
		{
			break;
		}
	}
	
	return (n);
}

void web_macro_usb_send_detailed_data(macro_elem_t *pbuf)
{
	unsigned char type = (pbuf->valA & 0xF0) >> 4;
	
	switch (type)
	{
		case 1: 
			ms_data.btn |= pbuf->valC;
		#if USB_MODE_ENABLE
			push_usb_fifo_aaa(MOUSE_DATA_TYPE, &ms_data.btn, sizeof(mouse_data_t));
		#endif
			break;
	
		case 2: 
			macro_buff[0] |= pbuf->valC;
		#if USB_MODE_ENABLE
			push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, macro_buff, sizeof(macro_buff));
		#endif
			break;
	
		case 3: 
			macro_buff[2] = pbuf->valC;
		#if USB_MODE_ENABLE
			push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, macro_buff, sizeof(macro_buff));
		#endif
			break;

		case 4: 
			ms_data.x = 0x00;
			if (pbuf->valC >= 0x81 )
			{
				ms_data.x = pbuf->valC | 0xFF00;
			}
			else if (pbuf->valC <= 0x7F)
			{
				ms_data.x = pbuf->valC;
			}
		#if USB_MODE_ENABLE
			push_usb_fifo_aaa(MOUSE_DATA_TYPE, &ms_data.btn, sizeof(mouse_data_t));
		#endif
			break;

			
		case 5: 
			ms_data.y = 0x00;
			if (pbuf->valC >= 0x81 )
			{
				ms_data.y = pbuf->valC | 0xFF00;
			}
			else if (pbuf->valC <= 0x7F)
			{
				ms_data.y = pbuf->valC;
			}
		#if USB_MODE_ENABLE
			push_usb_fifo_aaa(MOUSE_DATA_TYPE, &ms_data.btn, sizeof(mouse_data_t));
		#endif
			break;

		case 6: break;

		case 9:
			ms_data.btn &= ( ~(pbuf->valC) );
		#if USB_MODE_ENABLE
			push_usb_fifo_aaa(MOUSE_DATA_TYPE, &ms_data.btn, sizeof(mouse_data_t));
		#endif
			break;

		case 10: 
			macro_buff[0] &= ( ~(pbuf->valC) );
		#if USB_MODE_ENABLE
			push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, macro_buff, sizeof(macro_buff));
		#endif
			break;
			
		case 11: 
			macro_buff[2] = 0;
		#if USB_MODE_ENABLE
			push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, macro_buff, sizeof(macro_buff));
		#endif
			break;

		case 12: 
			ms_data.x = 0;
		#if USB_MODE_ENABLE
			push_usb_fifo_aaa(MOUSE_DATA_TYPE, &ms_data.btn, sizeof(mouse_data_t));
		#endif
			break;

			
		case 13: 
			ms_data.y = 0;
		#if USB_MODE_ENABLE
			push_usb_fifo_aaa(MOUSE_DATA_TYPE, &ms_data.btn, sizeof(mouse_data_t));
		#endif
			break;

		
		case 14: break;

		default:break;
	}
	
	printf("maro_AA=%d %1x %1x %1x \n", type, pbuf->valA, pbuf->valB, pbuf->valC);
}

unsigned char web_macro_operation(unsigned char i)
{
	if ( KEY_MACRO_PRESSED == web_key_macro_staus(i) )
	{
		return 1;
	}

	if ( gc_web_sta_list.macrocontinuous )
	{
		return 1;
	}

	if ( web_key_macro_count_tab[i] )
	{
		return 1;
	}

	return 0;
}

void web_macro_usb_send(char direct)
{
	unsigned char i = 0, k = 0, m = 0;
	unsigned int optime = 0;

	if ( 0 == gc_web_sta_list.macrokey ) { return; }

	if ( gc_web_sta_list.macrorelease )
	{
		if ( clock_time_exceed(sg_web_key_release_time, 10*1000) )
		{
			macro_buff[0] = 0x00;
			macro_buff[1] = 0x00;
			macro_buff[2] = 0x00;

			if ( gc_web_sta_list.release_count%2 )
			{
			#if USB_MODE_ENABLE
				push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, macro_buff, sizeof(macro_buff));
			#endif
			}
			else
			{
			#if USB_MODE_ENABLE
				push_usb_fifo_aaa(MOUSE_DATA_TYPE, &ms_data.btn, sizeof(mouse_data_t));
			#endif
			}

			sg_web_key_release_time = clock_time();
			gc_web_sta_list.release_count++;
		}

		if ( gc_web_sta_list.release_count >= 10 )
		{
			gc_web_sta_list.macrorelease = 0;
			gc_web_sta_list.macrokey = 0;
			printf("release_succ \n");
		}

		return;
	}

	for ( i = 0; i < KEY_NUM_MAX; i++ )
	{
		if ( web_macro_operation(i) && web_key_macro_time_tab[i] )
		{
			if ( direct || !gc_web_sta_list.macrodelay )
			{
				gc_web_sta_list.macrodelay = 1;
				web_key_macro_time_tab[i]  = clock_time() | 0x01;
				printf("pf111\n");
				web_macro_usb_send_detailed_data( &gc_web_data.macro[i][ web_key_macro_index_tab[i] ] );
			}
			else
			{
			    if ( web_key_macro_index_tab[i] < web_key_macro_max_index_get(i) - 1 )
			    {
					optime = 0x0F & gc_web_data.macro[i][ web_key_macro_index_tab[i] ].valA;
					optime <<= 8;
					optime |= gc_web_data.macro[i][ web_key_macro_index_tab[i] ].valB;

					if ( 0x00 == optime )
					{
						optime = 10;
					}
					
					if ( clock_time_exceed(web_key_macro_time_tab[i], optime*1000) )
					{
						web_key_macro_index_tab[i]++;
						gc_web_sta_list.macrodelay = 0;
					}
			    }
				else
				{
				    web_key_macro_index_tab[i] = 0x00;
					gc_web_sta_list.macrodelay = 0;


					if ( web_key_macro_count_tab[i] )
					{
						web_key_macro_count_tab[i]--;
					}

					if ( 0x00 == web_key_macro_count_tab[i] )
					{
						if ( 0x20 == (0xF0 & gc_web_data.key[i].value) )
						{
							web_key_macro_time_tab[i] = 0x00;
							printf("key_macro_time000\n");
						}
					}
				}
			}
		}

		if ( KEY_MACRO_PRESSED == web_key_macro_press_any(i) )
		{
			gc_web_sta_list.macrocontinuous = 0;

			for ( m = 0; m < KEY_NUM_MAX; m++ )
			{
				if ( 0x40 == (0xF0 & gc_web_data.key[m].value) )
				{
					web_key_macro_time_tab[m] = 0x00;
				}
			}
		}
	
		if ( KEY_MACRO_BOUNCE == web_key_macro_staus(i) )
		{
			web_key_macro_time_tab[i] = 0x00;
		}
			
		if ( 0x00 == web_key_macro_count_tab[i] && 0x00 == web_key_macro_time_tab[i] )
		{
			k++;
		}

		if ( k >= KEY_NUM_MAX )
		{
			gc_web_sta_list.macrorelease = 1;
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
			#if USB_MODE_ENABLE
				push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, sendbuff, sizeof(sendbuff));//push to fifo
			#endif
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
			#if USB_MODE_ENABLE
				push_usb_fifo_aaa(CONSUME_DATA_TYPE, sendbuff, 2);//push to fifo
			#endif
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
				gc_web_sta_list.macrocontinuous = 1;
				gc_web_sta_list.release_type = KEY_RELEASE_MACRO;

				sg_web_key_release_time    = clock_time();
				web_key_macro_time_tab[i]  = clock_time() | 0x01;
				web_key_macro_count_tab[i] = gc_web_data.key[i].func;

				web_macro_usb_send(1);

				// printf("Sendmacro00:%d %d %4x ", i, web_key_macro_count_tab[i], web_key_macro_time_tab[i]);
				// printf("%d %d %d \n", gc_web_sta_list.macrokey, gc_web_sta_list.macrocontinuous, gc_web_sta_list.release_type);
			}
		}
		gc_web_sta_list.trigger = KEY_TRIGGER_NONE;
	}

	if ( KEY_RELEASE_OFFICE == gc_web_sta_list.release_type )
	{
		if (  clock_time_exceed(sg_web_key_release_time, 10*1000) )
		{
			sg_web_key_release_time = clock_time();
		#if USB_MODE_ENABLE
			push_usb_fifo_aaa(NORMAL_KB_DATA_TYPE, sendbuff, sizeof(sendbuff));//push to fifo
		#endif
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
		#if USB_MODE_ENABLE
			push_usb_fifo_aaa(CONSUME_DATA_TYPE, sendbuff, 2);//push to fifo
		#endif
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
	web_macro_usb_send(0);
	
}

void web_function_loop(void)
{
	web_key_function_process();
}


#endif
