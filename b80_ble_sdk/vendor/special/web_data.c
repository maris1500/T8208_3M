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

void web_data_process(unsigned char *buf)
{
	if ( 0x06 == buf[0] && 0x01 == buf[1] && 0x01 == buf[3] )
	{
		web_key_handle( &gc_web_data.key[0], &buf[5] );
	}
}
