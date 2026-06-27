#include "web_data.h"

#if (WEB_HID_ENABLE)
	#include "../8208_dual_mouse/AAA_public_config.h"
	#include "../8208_dual_mouse/main.h"
#endif

char web_key_nomal(unsigned char index)
{
	unsigned char type = gc_web_data.key[index-1].type;

	if ( WEB_KEY_NOMAL == type )
	{
		return (1);
	}

	return (0);

}

char web_key_dpi(unsigned char index)
{
	unsigned char type = gc_web_data.key[index-1].type;

	if ( WEB_KEY_DPI == type || WEB_KEY_DPI_LOCK == type )
	{
		return (1);
	}

	return (0);

}

char web_key_office(unsigned char index)
{
	unsigned char type = gc_web_data.key[index-1].type;

	if ( WEB_KEY_OFFICE == type )
	{
		return (1);
	}

	return (0);

}

char web_key_media(unsigned char index)
{
	unsigned char type = gc_web_data.key[index-1].type;

	if ( WEB_KEY_MEDIA == type )
	{
		return (1);
	}

	return (0);

}

char web_key_fire(unsigned char index)
{
	unsigned char type = gc_web_data.key[index-1].type;

	if ( WEB_KEY_FIRE == type )
	{
		return (1);
	}

	return (0);

}

char web_key_macro(unsigned char index)
{
	unsigned char type = gc_web_data.key[index-1].type;

	if ( WEB_KEY_MACRO == type )
	{
		return (1);
	}

	return (0);

}

void web_key_handle(web_key_t *key, unsigned char *buf)
{
	unsigned char i = 0, k = 0, ret = 0;

	for (i = WEB_KEY_START; i < WEB_KEY_END; i++)
	{
		if ( key[i].type != buf[k + 0] || key[i].value != buf[k + 1] || key[i].func != buf[k + 2] )
		{
			key[i].type  = buf[k + 0];
			key[i].value = buf[k + 1];
			key[i].func  = buf[k + 2];

			ret = 1;
		}
		k += 3;
	}

	if ( ret )
	{
		printf("rev_succ_and_store \n");
	}
}

void web_report_rate(unsigned char *buf)
{
	if ( gc_web_data.sta.rate_cur != *buf )
	{
		gc_web_data.sta.rate_cur = *buf;
		printf("rate_cur=%d \n", gc_web_data.sta.rate_cur);
	}
}

void web_sleep_time(unsigned char *buf)
{
	if ( gc_web_data.sta.active_time != *buf )
	{
		gc_web_data.sta.active_time = *buf;
	}
	printf("active_time=%d \n", gc_web_data.sta.active_time);
}

void web_light_mode(unsigned char *buf)
{
	if ( gc_web_data.sta.light_mode != *buf )
	{
		gc_web_data.sta.light_mode = *buf;
		printf("light_mode=%d \n", gc_web_data.sta.light_mode );
	}

	if ( gc_web_data.sta.lightness != *(buf+1) )
	{
		gc_web_data.sta.lightness = *(buf+1);
		printf("lightness=%d \n", gc_web_data.sta.lightness );
	}
}

void web_sensor_wakeup_modify(unsigned char *buf)
{
	if ( gc_web_data.sta.sensor_wake_up != *(buf+1) )
	{
		gc_web_data.sta.sensor_wake_up = *(buf+1);
		printf("sensor_wake_up=%d \n", gc_web_data.sta.sensor_wake_up );
	}
}

void web_dpi_level(unsigned char *buf)
{
	const unsigned char level_max[7] = {0x01, 0x03, 0x07, 0x0F, 0x1f, 0x3f, 0x7f};
	unsigned char i = 0;

	if ( gc_web_data.dpi.level_cur != *buf )
	{
		 gc_web_data.dpi.level_cur = *buf;
		 printf("level_cur=%d \n", gc_web_data.dpi.level_cur );
	}

	for ( i = 0; i < 8; i++ )
	{
		if ( level_max[i] == *(buf+1) )
		{
			break;
		}
	}

	if ( gc_web_data.dpi.level_max != (i+1)  )
	{
		 gc_web_data.dpi.level_max = (i+1);
		 printf("level_max=%d \n", gc_web_data.dpi.level_max );
	}
}

void web_factory_set(void)
{
	if ( 0 == gc_web_data.sta.factory_up )
	{
		gc_web_data.sta.factory_up = 1;
		printf("factory_up \n" );
	}
}

void web_macro_receive(unsigned char *buf)
{
	unsigned char leng  = *(buf) - 1;
	unsigned char index = *(buf+1);
	unsigned char i = 0;

	buf++;
	buf++;

	for ( i = 0; (i < leng)&&(i < MACRO_NUM_MAX); i++ )
	{
		gc_web_data.macro[index-1][i].valA = *buf++ ;
		gc_web_data.macro[index-1][i].valB = *buf++ ;
		gc_web_data.macro[index-1][i].valC = *buf++ ;
	}
}

void web_data_process(unsigned char *buf)
{
	if ( 0x06 == buf[0] && 0x01 == buf[1] && 0x01 == buf[3] )
	{
		web_key_handle( &gc_web_data.key[0], &buf[5] );
	}
	else if ( 0x06 == buf[0] && 0x02 == buf[1] && 0x02 == buf[3] )
	{
		web_dpi_level( &buf[5] );
	}
	else if ( 0x06 == buf[0] && 0x03 == buf[1] && 0x01 == buf[3] )
	{

	}
	else if ( 0x06 == buf[0] && 0x04 == buf[1] && 0x01 == buf[3] )
	{
		web_report_rate( &buf[5] );
	}
	else if ( 0x06 == buf[0] && 0x07 == buf[1] && 0x01 == buf[3] )
	{
		web_sleep_time( &buf[5] );
	}
	else if ( 0x06 == buf[0] && 0x06 == buf[1] && 0x01 == buf[3] )
	{
		web_light_mode( &buf[6] );
	}
	else if ( 0x06 == buf[0] && 0x08 == buf[1] && 0x01 == buf[3] )
	{
		web_sensor_wakeup_modify( &buf[5] );
	}
	else if ( 0x06 == buf[0] && 0x0f == buf[1] && 0x01 == buf[3] )
	{
		web_factory_set();
	}
	else if ( 0x06 == buf[0] && 0x10 == buf[1] && 0x01 == buf[3] )
	{
		web_macro_receive( &buf[5] );
	}
}
