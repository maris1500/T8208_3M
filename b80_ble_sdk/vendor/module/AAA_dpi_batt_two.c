#include "AAA_dpi_batt_two.h"

#if BATT_DPI_DISP_TWO_ENABLE
	#include "../8208_dual_mouse/AAA_public_Config.h"
#endif


#if (PROJECT_ID == PID_M388)
	extern void vbatt_lower_clean(void);
#endif

#if BATT_DPI_DISP_TWO_ENABLE

static led_action_t sg_vbat_dpi_tab[VBAT_DPI_NUM];
static unsigned int sg_vabt_dpi_two_loop_time = 0;

extern _attribute_data_retention_user u32 idle_count;

extern unsigned char battery_low_flag_status(void);
extern unsigned char vbatt_lower_connected(void);


void dpi_two_dpi_action(led_act_en act)
{
	if ( act >= LED_DIS_MAX ) return;

	if ( LED_DIS_ON == act )
	{
		if ( RF_2M_2P4G_MODE == mcu_mode_get() )
		{
			gpio_output_config_status(PIN_24G_LED, LED_ON);
		}
		else
		{
			if ( BLE_DEVICE_ID_0 == ble_mode_device_id_get() )
			{
				gpio_output_config_status(PIN_BLE1_LED, LED_OFF);
			}
			else
			{
				gpio_output_config_status(PIN_BLE1_LED, LED_ON);
			}
		}
	}
	else
	{
		if ( RF_2M_2P4G_MODE == mcu_mode_get() )
		{
			gpio_higsesistance_config(PIN_24G_LED);
		}
		else
		{
			gpio_higsesistance_config(PIN_BLE1_LED);
		}
	}
}


void batt_two_batt_action(led_act_en act)
{
	if ( act >= LED_DIS_MAX ) return;

	if ( LED_DIS_ON == act )
	{
		gpio_output_config_status(PIN_BAT_DPI_LED, LED_OFF);
	}
	else
	{
		gpio_higsesistance_config(PIN_BAT_DPI_LED);
	}
}

void vbat_dpi_led_two_insert(vbat_dpi_type_en type, unsigned char times)
{
	if ( type >= VBAT_DPI_CHOOSE_MAX )  return;

	if ( VBAT_DPI_CHOOSE_VBAT == type )
	{
		sg_vbat_dpi_tab[type].offtime = 2000;
		sg_vbat_dpi_tab[type].ontime  = 1000;

		batt_two_batt_action( LED_DIS_ON );
	}
	else if ( VBAT_DPI_CHOOSE_DPI == type )
	{
		sg_vbat_dpi_tab[type].offtime = 500;
		sg_vbat_dpi_tab[type].ontime  = 500;

		dpi_two_dpi_action( LED_DIS_ON );
	}

	sg_vbat_dpi_tab[type].status  = LED_DIS_ON;
	sg_vbat_dpi_tab[type].repeats = times;
	sg_vbat_dpi_tab[type].types   = type;
	sg_vbat_dpi_tab[type].ticks   = clock_time();
}

void vbat_dpi_led_action(vbat_dpi_type_en type, led_act_en act)
{
	if ( VBAT_DPI_CHOOSE_VBAT == type )
	{
		batt_two_batt_action( act );
	}
	else if ( VBAT_DPI_CHOOSE_DPI == type )
	{
		dpi_two_dpi_action( act );
	}
}


void vbat_dpi_led_proc_two(void)
{
	led_action_t *led = &sg_vbat_dpi_tab[0];
	unsigned char i = 0;

	for ( i = 0; i < VBAT_DPI_NUM; i++, led++ )
	{
		if ( 0 == led->repeats )
		{
			continue;
		}

		if ( LED_DIS_ON == led->status )
		{
			if ( clock_time_exceed(led->ticks, led->ontime*1000) )
			{
				vbat_dpi_led_action(led->types, LED_DIS_OFF);
				led->status = LED_DIS_OFF;
				
				if ( led->offtime )
				{
					led->ticks = clock_time();
				#if (PROJECT_ID == PID_M388)
					if ( VBAT_DPI_CHOOSE_VBAT == led->types )
					{
						sg_vabt_dpi_two_loop_time = clock_time();
					}
				#endif
				}
			}
		}
		else
		{
			if ( clock_time_exceed(led->ticks, led->offtime*1000) )
			{
				if ( --led->repeats )
				{
					vbat_dpi_led_action(led->types, LED_DIS_ON);
					led->status = LED_DIS_ON;
					
					led->ticks = clock_time();
				}
			}
		}
	}
}

void batt_dpi_two_loop(void)
{

#if (PROJECT_ID == PID_M388)
	if ( 0 == connect_ok )
	{
		vbatt_lower_clean();
	}
#endif

	if ( clock_time_exceed(sg_vabt_dpi_two_loop_time, 2*1000*1000) )
	{
		sg_vabt_dpi_two_loop_time = clock_time();

		if ( vbatt_lower_connected() && battery_low_flag_status() && (idle_count < 30) )
		{
			vbat_dpi_led_two_insert(VBAT_DPI_CHOOSE_VBAT, 1);
		}
	}

	vbat_dpi_led_proc_two();
}

#endif
