#include "AAA_web_key.h"

#if WEB_KEY_FEATURE_ENABLE
	#include "AAA_public_config.h"
	#include "main.h"
#endif

#if WEB_KEY_FEATURE_ENABLE

web_set_status_t gc_web_sta_list;

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


void web_key_function_process(void)
{
	if ( KEY_TRIGGER_OFFICE == gc_web_sta_list.trigger )
	{
		
	}
}


#endif
