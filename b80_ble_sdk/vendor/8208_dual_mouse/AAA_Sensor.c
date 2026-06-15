/********************************************************************************************************
 * @file     AAA_Sensor.c
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

#if (PROJECT_ID == PID_DMS006)
	unsigned char gc_dms006_cpi_flag = 0;
#endif

#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_HM668)
	unsigned char gc_digital_dpi_flag = 0;
	unsigned short int gc_digital_dpi_delay = 0;
#endif

#if TUBE_DPI_DISP_ENABLE
	unsigned char gc_digital_dpi_flag = 0;
	unsigned short int gc_digital_dpi_delay = 0;
#endif

#if (PROJECT_ID == PID_M388)
	#include "../module/AAA_dpi_batt_two.h"
#endif

#if (PROJECT_ID == PID_4027) || (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
	unsigned char gl_cpi_change_flag;
#endif

#if SENSOR_FUN_ENABLE_AAA

#define sif_spi_clk_low    gpio_write(PIN_SIF_SCL, 0)
#define sif_spi_clk_high   gpio_write(PIN_SIF_SCL, 1)

#define sif_spi_sda_low    gpio_write(PIN_SIF_SDA, 0)
#define sif_spi_sda_high   gpio_write(PIN_SIF_SDA, 1)
#define get_spi_sda_vaule  gpio_read(PIN_SIF_SDA)

#define sif_spi_sda_output_enable	gpio_set_output_en(PIN_SIF_SDA, 1)
#define sif_spi_sda_output_disable	gpio_set_output_en(PIN_SIF_SDA, 0)

#define sif_spi_sda_input_enable    gpio_set_input_en(PIN_SIF_SDA, 1)
#define sif_spi_sda_input_disable   gpio_set_input_en(PIN_SIF_SDA, 0)

#if (SENSOR_MISO_ENABLE)
#define sif_spi_sda_read_input_enable    gpio_set_input_en(PIN_SIF_SDA_READ, 1)
#define sif_spi_sda_read_input_disable   gpio_set_input_en(PIN_SIF_SDA_READ, 0)

#define sif_spi_sda_read_output_enable     gpio_set_output_en(PIN_SIF_SDA_READ, 1)
#define sif_spi_sda_read_output_disable    gpio_set_output_en(PIN_SIF_SDA_READ, 0)

#define get_spi_sda_read_vaule  gpio_read(PIN_SIF_SDA_READ)
#endif

#if SENSOR_CS_ENABLE
    #define sif_spi_cs_high	gpio_write(PIN_SENSOR_CS,1)
    #define sif_spi_cs_low	gpio_write(PIN_SENSOR_CS,0)
    #define CS_DELAY		sleep_us(5)
#else
    #define sif_spi_cs_high
    #define sif_spi_cs_low
    #define CS_DELAY
#endif

#define		DLY_200NS    CLOCK_DLY_20_CYC  //sleep_us(5)//asm("tnop");asm("tnop")
#define		DLY_100NS    CLOCK_DLY_10_CYC  //sleep_us(5)//asm("tnop")

#define		SENSOR_RECOVER_FAIL		1

////////////////////////////////////////////////////////////////////////////
// serial interface function
////////////////////////////////////////////////////////////////////////////


//_attribute_data_retention_user u16	check_spi_counter;
//_attribute_data_retention_user u8	check_resync_counter;
_attribute_data_retention_user u8	product_id1 = 0;
_attribute_data_retention_user u8	product_id2 = 0;
_attribute_data_retention_user u8	product_id3 = 0;
//u8  reg_0d;
_attribute_data_retention_user u8	sensor_type = 0xff;
//_attribute_data_retention_user u8   mouse_cust_fct3065xy = 0;
//_attribute_data_retention_user u8   dbg_sensor_cpi = 0;


#if (PROJECT_ID == PID_LXL256)
unsigned char gl_cpi_change_flag = 0;
#endif

#if LED_RGB_DPI_ACTION_ENABLE
    extern void battery_low_flag_clear(void);
#endif

u8 dpi_max_level = 3; //max DPI value
u8 dpi_3605_limit_level_flag = 0;

/**
 * @brief	set sensor wake up from suspend
 * @param	enable 0:disable wake up , 1:enable wake up
 * @return	none
 */
void sensor_set_wakeup_level_suspend(u8 enable)
{
#if SENSOR_MOTION_ENABLE
   cpu_set_gpio_wakeup(PIN_SIF_MOTION, 0, enable); //low wakeup suspend
#endif
}

/**
 * @brief	set sensor wake up from deep sleep
 * @param	enable 0:disable wake up , 1:enable wake up
 * @return	none
 */
void sensor_set_wakeup_level_deepsleep(u8 enable)
{
#if SENSOR_MOTION_ENABLE
    cpu_set_gpio_wakeup(PIN_SIF_MOTION, 0, enable); //low wakeup suspend
#endif
}

/**
 * @brief	initial sensor pin
 * @param	none
 * @return	none
 */
void sif_init(void)
{
#if (PROJECT_ID == PID_Q15)
	sleep_ms(100);
#endif

#if SENSOR_CS_ENABLE
	gpio_set_func(PIN_SENSOR_CS,AS_GPIO);
	gpio_set_input_en(PIN_SENSOR_CS,0);
	gpio_set_output_en(PIN_SENSOR_CS,1);
	
	gpio_write(PIN_SENSOR_CS, 1);
#endif

#if (PROJECT_ID == PID_QXX9)
	gpio_input_config_status(PIN_SIF_MOTION, PM_PIN_PULLUP_1M);
#endif

#if (PROJECT_ID == PID_Q15)
	gpio_write(PIN_SENSOR_CS, 0);
	sleep_ms(5);
	gpio_write(PIN_SENSOR_CS, 1);
#endif

}

/**
 * @brief	reset sensor pin when MCU power down
 * @param	none
 * @return	none
 */
void sensor_gpio_powerDownConfig()
{
    gpio_setup_up_down_resistor(PIN_SIF_SCL, PM_PIN_PULLUP_1M);

    if (gpio_read(PIN_SIF_SDA))
    {
        gpio_setup_up_down_resistor(PIN_SIF_SDA, PM_PIN_PULLUP_1M);
    }
    else
    {
        gpio_setup_up_down_resistor(PIN_SIF_SDA, PM_PIN_PULLDOWN_100K);
    }
}

/**
 * @brief	send a byte
 * @param	data
 * @return	none
 */
_attribute_ram_code_sec_ void sif_SendByte(u8 data)
{
    u8 i = 0;
    u8 buf = data;

    for (i = 0; i < 8; i++)
    {
        sif_spi_clk_low;
        gpio_write(PIN_SIF_SDA, buf & 0x80);
        DLY_100NS;

        sif_spi_clk_high;
        buf <<= 1;
        DLY_100NS;
    }
}

/**
 * @brief	read a byte
 * @param	none
 * @return	read result
 */
_attribute_ram_code_sec_ u8 sif_ReadByte()
{
    u8 i = 0;
    u8 dat = 0;

    for (i = 0; i < 8; i++)
    {
        sif_spi_clk_low;
        dat <<= 1;
        DLY_100NS;

        sif_spi_clk_high;
        DLY_100NS;
    #if (SENSOR_MISO_ENABLE)
        if ( get_spi_sda_read_vaule )
    #else
        if (get_spi_sda_vaule)
    #endif
        {
            dat |= 1;
        }
    }

    return dat;
}

/**
 * @brief	read Register value
 * @param	cAddr: Register address
 * @return	Register value
 */
_attribute_ram_code_sec_ u8 I2C_PAN3204LL_ReadRegister(u8 cAddr)
{
    u8 dat = 0;

    //u8 r = irq_disable();
    
 #if (SENSOR_MISO_ENABLE)
    // gpio_write(PIN_SIF_SDA, 1);
    // sif_spi_sda_read_output_disable;
    // sif_spi_sda_read_input_enable;
    gpio_output_config(PIN_SIF_SDA);  gpio_write(PIN_SIF_SDA, 1);
    gpio_input_config_status(PIN_SIF_SDA_READ, PM_PIN_PULLUP_10K);
    CS_DELAY;
#else
    sif_spi_sda_output_enable;
    sif_spi_sda_input_disable;
#endif

    sif_spi_cs_low;

    CS_DELAY;

    sif_SendByte(cAddr & 0x7f);

#if (SENSOR_MISO_ENABLE)
	// do nothing
	gpio_input_config_status(PIN_SIF_SDA_READ, PM_PIN_PULLUP_10K);
    gpio_input_config_status(PIN_SIF_SDA, PM_PIN_PULLUP_10K);
    
	WaitUs(5);
#else
    sif_spi_sda_output_disable;// delay 5us
    sif_spi_sda_input_enable;  // delay 15us
#endif

    //WaitUs(5);       //this delay is necessary

    dat = sif_ReadByte();
	
    CS_DELAY;
    sif_spi_cs_high;

    //irq_restore(r);

    return dat;
}

/**
 * @brief	write Register data
 * @param	cAddr: Register address
 * @param	cData: write data
 * @return	none
 */
_attribute_ram_code_sec_  void I2C_PAN3204LL_WriteRegister(u8 cAddr, u8 cData)
{
   // u8 r = irq_disable();
    gpio_set_output_en(PIN_SIF_SDA, 1);
    gpio_set_input_en(PIN_SIF_SDA, 0);
    sif_spi_cs_low;
	
    CS_DELAY;
	
    sif_SendByte((cAddr | 0x80));
	
    WaitUs(2);
    
    sif_SendByte(cData);

    CS_DELAY;

    sif_spi_cs_high;

    gpio_set_output_en(PIN_SIF_SDA, 0);
    gpio_set_input_en(PIN_SIF_SDA, 1);

   // irq_restore(r)
}


//----------------------------------------------------------------------
// FUNCTION NAME: Sensor3204LL_Optimization_Setting
//
// DESCRIPTION:
//      Do the optimization of the sensor PAN3204LL to consume less current.
//      Also, if need, set a better tracking performance on Critical surfaces.
//
//----------------------------------------------------------------------
void DownloadConfigTable(const unsigned char *ptbl, unsigned int len)
{
    unsigned int i, addr;

    for (i = 0; i < len; i += 2)
    {
        addr = ptbl[0];
        I2C_PAN3204LL_WriteRegister(addr, ptbl[1]);
        ptbl += 2;
    }
}

const unsigned char Config_3205_TJ3T[] = {
	0x09, 0x5A,
	0x0D, 0x0A,
	0x1B, 0x35,
    0x1D, 0xDB,

	0x28, 0xB4,
	0x29, 0x46,
	0x2A, 0x96,
	0x2B, 0x8C,
	0x2C, 0x6E,
	0x2D, 0x64,
	0x38, 0x5F,
	0x39, 0x0F,
	0x3A, 0x32,
	0x3B, 0x47,
	0x42, 0x10,

	0x43, 0x09,
	0x54, 0x2E,
	0x55, 0xF2,
	0x61, 0xF4,
	0x63, 0x70,
	0x75, 0x52,
	0x76, 0x41,
	0x77, 0xED,
	0x78, 0x23,
	0x79, 0x46,
	0x7A, 0xE5,
	0x7C, 0x48,

    0x7D, 0x80,
	0x7E, 0x77,

	0x7F, 0x01,
	0x0B, 0x00,

	0x7F, 0x00,
	0x09, 0x00,
};

const unsigned char Config_VT108[] = {		//KA8
	0x09,  0x5a,
	0x0d,  0x12,
	0x0e,  0xc5,
	0x09,  0x00,
};

const unsigned char Config_3212[] = {
	0x09,  0x5a,
	0x06,  0xa0,
	0x26,  0x34,
#if SENSOR_DATA_LENGTH_12_BIT_ENABLE
	0x19,  0x04, //new
#endif
	0x09,  0x00,
};

const unsigned char config_paw3805ek_cjv1[] = 
{
	0x09,  0x5a,
	0x06,  0xa0,
	0x26,  0x34,
#if SENSOR_DATA_LENGTH_12_BIT_ENABLE
	0x19,  0x04, //new
#endif
	0x51,  0x03,
	0x09,  0x00,
};

const unsigned char config_paw3220db_tjdl[] =
{
	0x09,  0x5a,
	0x26,  0x34,
	0x4b,  0x00,
	0x5c,  0xd4,
	0x0d,  0x1a,
	0x0e,  0x1c,
	0x7f,  0x01,
	0x42,  0x4f,
	0x43,  0x93,
	0x44,  0x48,
	0x45,  0xf2,
	0x47,  0x4f,
	0x48,  0x93,
	0x49,  0x48,
	0x4a,  0xf3,
	0x64,  0x66,
	0x79,  0x08,
	0x7f,  0x00,
#if SENSOR_DATA_LENGTH_12_BIT_ENABLE
	0x19,  0x04, //new
#endif

#if (PROJECT_ID == PID_QXX9)
	0x0A, 0x77,
	0x0B, 0x1F,
	0x0C, 0x7F,
#endif
	
	0x09,  0x00,
};

const unsigned char config_sg8670[] =
{
#if 0
	0x7f,  0x26,
	0x09,  0x5a,
	
	0x28,  0x04,
	0x29,  0xb0,
	0x5b,  0x00,
	0x4e,  0x1a,
	0x4f,  0x00,
#if SENSOR_DATA_LENGTH_12_BIT_ENABLE
	// 0x0f,  0x04,
#endif
	0x7e,  0x01,
	0x40,  0xff,
	0x41,  0xff,
	0x42,  0xff,
	0x43,  0xff,
	0x47,  0xaf,
	0x4a,  0x40,
	0x4c,  0x03,
	0x4d,  0x00,
	0x4e,  0x02,
	0x7e,  0x00,
	
	0x7f,  0x00,
	0x09,  0x00,
#else
	0x09,  0x5a,
#if SENSOR_DATA_LENGTH_12_BIT_ENABLE
	 0x0f,  0x04,
#endif
	0x09,  0x00,
#endif
};

const u8 sg8925_dpi[4][2] =
{
	{0x04, 0xB0}, // 1200
	{0x09, 0x60}, // 2400
	{0x0c, 0x80}, // 3200
	{0x13, 0x88}, // 5000
};

void sg8925_CPI_set(u8 dpi_val1, u8 dpi_val2)
{
	for (u8 i = 0; i < 16; i++)
	{
		u8	reg_1 = 0, reg_2 = 0;

		I2C_PAN3204LL_WriteRegister(0x09,0x5A);
		I2C_PAN3204LL_WriteRegister(0x7f,0xa7);
		I2C_PAN3204LL_WriteRegister(0x67,0x13);

		I2C_PAN3204LL_WriteRegister(0x0d, dpi_val1);
		I2C_PAN3204LL_WriteRegister(0x0e, dpi_val2);

		reg_1 = I2C_PAN3204LL_ReadRegister(0x0d);
		reg_2 = I2C_PAN3204LL_ReadRegister(0x0e);

		I2C_PAN3204LL_WriteRegister(0x09,0x00);
		I2C_PAN3204LL_WriteRegister(0x7f,0x00);

		if ( reg_1 == dpi_val1 && reg_2 == dpi_val2 )
		{
			printf(" sg89250 dpi set sucess \n");
			return;
		}
	}
}

const u8 s205_dpi_tbl[] =
{
#if (PROJECT_ID == PID_TB903)
	25, // cpi_1200 25*50 = 1250
	33, // cpi_1600 33*50 = 1650
	49, // cpi_2400 49*50 = 2450
	65, // cpi_3200 65*50 = 3250
	97, // cpi_4800 97*50 = 4850
	128, //cpi_6400 128*50 = 6400
#else
    40, 	// cpi_2000
    60,		// cpi_3000 
    100,	// cpi_5000
    128,	// 6400
#endif
};

// office mode
const unsigned char Config_s205_tab[] = {
	0x09, 0xc3,
#if (1)
	0x64, 0x80,
	0x65, 0x03,
	0x66, 0x83,
	0x50, 0xe9,
	0x54, 0x04,
	0x73, 0x53,
	0x75, 0x08,
	0x0a, 0x73,
	0x1a, 0x30,
	0x0b, 0x30,
	0x6d, 0xd0,
	0x6e, 0xf0,
	0x7c, 0x70,
	0x7b, 0x10,
	0x24, 0x4f,
	0x2f, 0x01,
	0x74, 0x20,
	0x77, 0x30,
	0x2f, 0x00,
	0x44, 0x09,
	0x43, 0xc4,
#endif

#if SENSOR_DATA_LENGTH_12_BIT_ENABLE
	0x19, 0x04,
	0x22, 0x01,
#endif

	0x09, 0x00,
};

const unsigned char config_s101ul_tab[] =
{
	0x09, 0xC3,
	0x19, 0x00,
	0x22, 0x01,
	0x26, 0x34,

#if 0
	0x64, 0x80,
	0x65, 0x03,
	0x66, 0x83,
	0x50, 0xE9,
	0x54, 0x04,
	0x73, 0x53, 
	0x0A, 0x73,
	0x1A, 0x30,
	0x67, 0x18,
	0x6D, 0xD0,
	0x6E, 0xF0,
	0x7C, 0x70,
	0x7B, 0x10,
	0x24, 0x4F,
	0x25, 0x24,
	0x2F, 0x01,
	0x74, 0x20,
	0x77, 0x30,
	0x2F, 0x00,
	0x44, 0x0B,
	0x43, 0xB8,
#endif

	0x09, 0x00,
};

const unsigned char config_3311_part_A[] =
{
	0x7F, 0x00,
	0x55, 0x00,
	0x4D, 0x50,
	0x4E, 0x46,
	0x4F, 0x46,

	0x77, 0x24,
	0x7F, 0x05,
	0x44, 0xA8,
	0x4A, 0x14,
	0x53, 0x0C,

	0x5B, 0xEA,
	0x61, 0x13,
	0x62, 0x07,
	0x64, 0xD8,
	0x6D, 0x86,

	0x7D, 0x84,
	0x7E, 0x00,
	0x7F, 0x06,
	0x60, 0x70,
	0x61, 0x00,

	0x7E, 0x40,
	0x7F, 0x07,
	0x42, 0x16,
	0x7F, 0x09,
	0x40, 0x03,

	0x7F, 0x0A,
	0x49, 0x00,
	0x4A, 0x23,
	0x4C, 0x28,
	0x4F, 0x02,

	0x7F, 0x0C,
	0x40, 0xA0,
	0x41, 0x70,
	0x42, 0x20,
	0x43, 0xC5,

	0x44, 0x44,
	0x45, 0x04,
	0x4C, 0x60,
	0x54, 0x00,
	0x55, 0x40,

	0x59, 0x93,
	0x7F, 0x0D,
	0x4F, 0x02,
	0x4E, 0x6B,
	0x7F, 0x14,

	0x4A, 0x67,
	0x62, 0x1C,
	0x63, 0x1C,
	0x6D, 0x82,
	0x6F, 0xD8,

	0x73, 0x83,
	0x74, 0x00,
	0x7A, 0x16,
	0x7F, 0x10,
	0x48, 0x0F,

	0x49, 0x88,
	0x4C, 0x1D,
	0x4F, 0x08,
	0x51, 0x6F,
	0x52, 0x90,

	0x54, 0x64,
	0x55, 0xF0,
	0x5C, 0x40,
	0x61, 0xEE,
	0x62, 0xE5,

	0x7F, 0x00,
	0x5B, 0x40,
	0x61, 0xAD,
	0x51, 0xEA,
	0x19, 0x9F,
};

void config_3311_part_B(void)
{
	unsigned int i = 0;
	unsigned char reg = 0x00;

	for ( i = 0; i < 1000; i++ )
	{
		reg = I2C_PAN3204LL_ReadRegister(0x20);
		if ( 0x0F == reg)
		{
			i = 65535;
		}

		sleep_ms(1);
	}
	printf("->reg:%1x %d \n", reg, i);
}

const unsigned char config_3311_part_C[] =
{
	0x19, 0x10,
	0x40, 0x00,
	0x61, 0xD5,
	0x7F, 0x00
};

const u8 s101ul_dpi_tbl[] =
{
#if (PROJECT_ID == PID_660)
	#if UL1001_DPI_2_ENABLE
		31,  //1550 1400 cpi_1200    1000
		41,  //2050 1800 cpi_1600    1400
		61,  //3050 2800 cpi_2400    2000
		80,  //4000 3600 cpi_3200    2800
		96,  //          cpi_4800    4000
	#else
		24,  // cpi_1200    1000
		32,  // cpi_1600    1400
		48,  // cpi_2400    2000
		64,	 // cpi_3200    2800
		96,  // cpi_4800    4000
	#endif
#elif (PROJECT_ID == PID_DMS06) || (PROJECT_ID == PID_DMS157)
	31,  //1550 1400 cpi_1200    1000
	41,  //2050 1800 cpi_1600    1400
	61,  //3050 2800 cpi_2400    2000
	80,  //4000 3600 cpi_3200    2800
	96,  //          cpi_4800    4000
#else
	8,  //  400
	16,  // 800
	32,  // 1600
	64,  // 3200
	96,  // 4800
#endif
};

void s101ul_CPI_set(u8 dpi_val)
{
	u8	i = 0, reg_1 = 0, reg_2 = 0;
	for (i = 0; i < 16; i++)
	{
		I2C_PAN3204LL_WriteRegister(0x09,0x5A);
		
		I2C_PAN3204LL_WriteRegister(0x0D, dpi_val);
		I2C_PAN3204LL_WriteRegister(0x0E, dpi_val);
		
		reg_1 = I2C_PAN3204LL_ReadRegister(0x0D);
		reg_2 = I2C_PAN3204LL_ReadRegister(0x0E);

		I2C_PAN3204LL_WriteRegister(0x09,0x00);

		if ( reg_1 == dpi_val && reg_2 == dpi_val )
		{
			printf(" s101ul dpi set sucess \n");
			return;
		}
	}
}


const u8 paw3311_dpi_tbl[] =
{
	0x12,  // 800
	0x1C,  // 1200
	0x25,  // 1600
	0x38,  // 2400
	0x4B,  // 3200
	0x8D   // 240000 = 1200 * 2
};

void paw3311_CPI_set(u8 dpi_val)
{
	u8	i = 0, reg_1 = 0;
	for (i = 0; i < 16; i++)
	{
		if ( dpi_val < 10000 )
		{
			I2C_PAN3204LL_WriteRegister(0x4D,0x50);
		}
		else
		{
			I2C_PAN3204LL_WriteRegister(0x4D,0xD0);
		}

		I2C_PAN3204LL_WriteRegister(0x4E, dpi_val);

		reg_1 = I2C_PAN3204LL_ReadRegister(0x4E);

		if ( reg_1 == dpi_val )
		{
			printf(" paw3311 dpi set sucess \n");
			return;
		}
	}
}

const unsigned char config_sg8925[] =
{
	0x06,  0x80,
	0x7f,  0xa7,
	0x90,  0x5a,
//	0x09,  0x5a,

	0x05,  0x00, // xx enable sleep mode
	0x20,  0xf0,
	0x21,  0x10,
	0x2b,  0x01,

	0x2d,  0x64,
	0x57,  0x08,
	0x1b,  0x5d, //xx

	0x67,  0x13,

#if SENSOR_DATA_LENGTH_12_BIT_ENABLE
	0x19,  0x04, //new
#endif

	0x0d,  0x20,
	0x0e,  0x20,

	0x7f,  0x00,
	0x09,  0x00,
};

const unsigned char Config_s205_game_tab[] = {
	0x09, 0xc3,
#if (1)
	0x64, 0x80,
	0x65, 0x03,
	0x66, 0x83,
	0x50, 0xe9,
	0x54, 0x04,
	0x73, 0xd3, //
	0x75, 0x48, //
	0x0a, 0x17,
	0x1a, 0x30,
	0x0b, 0x00, //
	0x6d, 0xd0,
	0x6e, 0xf0,
	0x7c, 0x70,
	0x7b, 0x10,
	0x24, 0x4f,
	0x25, 0x24,
	0x2f, 0x01,
	0x74, 0x20,
	0x77, 0x30,
	0x2f, 0x00,
	0x44, 0x09,
	0x43, 0xc4,
#endif

#if SENSOR_DATA_LENGTH_12_BIT_ENABLE
	0x19, 0x04,
	0x22, 0x01,
#endif

	0x09, 0x00,
};
const unsigned char config_paw3104db_txdt[] = 
{
	0x09,  0x5a,
	0x06,  0xa0,
	0x05,  0xb8,
#if SENSOR_DATA_LENGTH_12_BIT_ENABLE
	0x19,  0x04, //new
#endif
	0x09,  0x00,
};

const unsigned char Config_YS8009[] = {
	0x06,  0x03,
	0x09,  0x5a,
	0x0b,  0x90,
	0x09,  0x69,
	0x0d,  0x48,
	0x0e,  0x9f,
	0x0f,  0xba,
	//0x16,  0xbd,
	0x17,  0x08,
	//0x22,  0x11,
	0x09,  0x00,
};


void sensor_resync()
{
	gpio_write(PIN_SIF_SCL, 1);
    WaitUs(3);
    gpio_write(PIN_SIF_SCL, 0);

    if ((sensor_type == SENSOR_3065) || (sensor_type == SENSOR_3065XY))
    {
        WaitUs(10000);				//FCT3065-XY SCL Low 10ms, then high 512ms
        gpio_write(PIN_SIF_SCL, 1);
        WaitUs(40000);
    }
    else
    {
		WaitUs(5);
		gpio_write(PIN_SIF_SCL, 1);
		WaitUs(30000);
    }
}

/**
 * @brief	sensor check id
 * @param	none
 * @return	none
 */
u8 sensor_check_id()
{
	u8 reg0 = 0;
	reg0 = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_PRODUCT_ID1);

	if ( reg0 == 0x43 || reg0 == 0x4E || reg0 == 0x50)
	{
		return 1; 
	}
	
	if ( (reg0 != 0x30) && (reg0 != 0x31) && (reg0 != 0x58) && (reg0 != 0x18) && (reg0 != 0x50) )
	{
		//unknown sensor, resync
		sensor_resync();
		return 0;
	}

	return 1; //valid sensor
}

/**
 * @brief	optical sensor resync
 * @param	retry: retry times
 * @return	result 0:failed, 1:success
 */
int OPTSensor_resync(u32 retry)
{
	for (u16 i=0; i<retry; i++)
	{
	#if (MODULE_WATCHDOG_ENABLE)
        wd_clear();
	#endif
	
	#if (MODULE_32K_WATCHDOG_ENABLE)
		wd_32k_clear();
	#endif
	
		if (sensor_check_id())
		{
			return 1; //resync success
		}
	}

	return 0; //resync fail
}

/**
 * @brief	Identify the type of the sensor
 * @param	none
 * @return	sensor type
 */
static int sensor_type_identify()
{
	int sensor_type = SENSOR_3205_TJNT;

	if (product_id1 == 0x18)
	{
		return SENSOR_OM18;
	}
	else if ((product_id1 == 0x58) && (product_id2 == 0x59))
	{
		return SENSOR_KA8G2;
	}
	else if (product_id1 == 0x30)
	{
		if (product_id2 == 0x02)
		{
			if ( 0xC0 == I2C_PAN3204LL_ReadRegister(0x7a) )
			{
				return ( SENSOR_SG8925 );
			}

			unsigned char reg_5f = 0x00;
			
			reg_5f = I2C_PAN3204LL_ReadRegister(0x49);
			if ( 0xb2 == reg_5f )
			{
				return ( SENSOR_S205 );
			}

			if ( 0xC1 == reg_5f )
			{
				return SENSOR_S101UL;
			}
	
			reg_5f = I2C_PAN3204LL_ReadRegister(0x5f);
			reg_5f = (reg_5f&0x03);

			if ( 3 == reg_5f )
				return SENSOR_PAW3212;
			else
				return SENSOR_PAW3220DB_TJDL;
		//#if (PROJECT_ID == PID_M98)
		//	return SENSOR_PAW3220DB_TJDL;
		//#else
		//	return SENSOR_PAW3212;
		//#endif
		}
		else if ((product_id2&0xf0) == 0x50)
		{
			if (product_id2 == 0x54)
			{
				return SENSOR_VT108;
			}
			else if (product_id2 == 0x5A && product_id3 == 0xC2)
			{
				return SENSOR_SG8670;
			}
			else
			{
				return SENSOR_M8650A;
			}
		}
		else if ((product_id2&0xf0) == 0xd0)//
		{
			if (product_id2 == 0xd8)
			{
				if (I2C_PAN3204LL_ReadRegister(0x1B) == 0x1D)
	            {
	                return SENSOR_YS8009;
	            }
				
			}
			else if (product_id2 == 0xd1)
			{
				if (I2C_PAN3204LL_ReadRegister(0x49) == 0xa1)
	            {
	                return SENSOR_S201B;
	            }				
			}			

			return SENSOR_3205_TJ3T;
		}
	}
	else if (product_id1 == 0x31)
	{
		if (product_id2 == 0x00)
		{
			return SENSOR_3065;
		}
		else if (product_id2 == 0x70)
		{
			 return SENSOR_3065XY;
		}
		else if( product_id2 == 0x61)
		{
			 return(SENSOR_PAW3805EK_CJV1);
		}
		else if( product_id2 == 0x20 || product_id2 == 0x50)
		{
			return(SENSOR_PAW3104DB_TXDT);
		}
	}
	else if ( product_id1 == 0x43 )
	{
		printf("-->product_id1==0x43 \n");
	}
	else if ( product_id1 == 0x50 )
	{
		unsigned char reg_5f = 0x00;

		reg_5f = I2C_PAN3204LL_ReadRegister(0x5f);

		if ( 0xAF == reg_5f )
		{
			return ( SENSOR_3311 );
		}
	}
	
	return  sensor_type; //return default type
}

/**
 * @brief	initial om18
 * @param	none
 * @return	none
 */
void om18_Optimization_Setting()
{
	u8 var1,var2,var3;
	I2C_PAN3204LL_WriteRegister(0x09,0x5a);

	I2C_PAN3204LL_WriteRegister(0x62,0x63);
	I2C_PAN3204LL_WriteRegister(0x6e,0x63);
	I2C_PAN3204LL_WriteRegister(0x6f,0x63);
	I2C_PAN3204LL_WriteRegister(0x70,0x63);
	I2C_PAN3204LL_WriteRegister(0x71,0x63);
	I2C_PAN3204LL_WriteRegister(0x56,0x08);
	var1=I2C_PAN3204LL_ReadRegister(0x43)&0x0f;
	var2=I2C_PAN3204LL_ReadRegister(0x45)&0x7f;
	var3=I2C_PAN3204LL_ReadRegister(0x46)&0x7f;

	I2C_PAN3204LL_WriteRegister(0x43,var1);
	I2C_PAN3204LL_WriteRegister(0x45,var2);
	I2C_PAN3204LL_WriteRegister(0x46,var3);
	I2C_PAN3204LL_WriteRegister(0x56,0);
#if SENSOR_DATA_LENGTH_12_BIT_ENABLE
	I2C_PAN3204LL_WriteRegister(0x19,0x14); //14
#else
	I2C_PAN3204LL_WriteRegister(0x19,0x10);
#endif
	I2C_PAN3204LL_WriteRegister(0x09,0x00);
}

/**
 * @brief	initial KA8G2
 * @param	none
 * @return	none
 */
void KA8G2_Optimization_Setting()
{
	I2C_PAN3204LL_WriteRegister(0x09,0xA5);
	I2C_PAN3204LL_WriteRegister(0x46,0x34); // ???? spi ??????
	I2C_PAN3204LL_WriteRegister(0x19,0x00);
	I2C_PAN3204LL_WriteRegister(0x60,0x07);
	I2C_PAN3204LL_WriteRegister(0x69,0x04);
	I2C_PAN3204LL_WriteRegister(0x7D,0x20);
	I2C_PAN3204LL_WriteRegister(0x7E,0x00);
	I2C_PAN3204LL_WriteRegister(0x09,0x00);
	I2C_PAN3204LL_WriteRegister(0x0D,0x1B);
	I2C_PAN3204LL_WriteRegister(0x0E,0x1B);

	I2C_PAN3204LL_WriteRegister(0x6E,0x1c);  //disable 
	//I2C_PAN3204LL_WriteRegister(0x6E,0x14);// eanble inter Internal resistance

	I2C_PAN3204LL_WriteRegister(0x09,0x00);
}

/**
 * @brief	initial optical sensor
 * @param	none
 * @return	none
 * @note 	called when power on, or mouse waked up from deep-sleep
 */
unsigned int OPTSensor_Init(unsigned int poweron)
{
	// Do the full chip reset.
	u8 reg6=0;
	
	sif_init(); //initial sensor pin
	if (OPTSensor_resync(33) == 0)	{printf("------>OPTSensor_Init failed \n");return 0;} //check sensor id
	Sensor3204_Wakeup(sensor_type);

#if (PROJECT_ID == PID_Q15)
	if ( sensor_type == SENSOR_3311 )
	{
		unsigned char reg_r1 = 0x00, reg_r2 = 0x00;

		I2C_PAN3204LL_WriteRegister(0x7F, 0x00);
		I2C_PAN3204LL_WriteRegister(0x3A, 0x5A);
		sleep_ms(60);

		I2C_PAN3204LL_WriteRegister(0x40, 0x80);
		I2C_PAN3204LL_WriteRegister(0x55, 0x01);
		sleep_ms(2);

		I2C_PAN3204LL_WriteRegister(0x7F, 0x0E);
		I2C_PAN3204LL_WriteRegister(0x43, 0x1D);

		reg_r1 = I2C_PAN3204LL_ReadRegister(0x46);
		I2C_PAN3204LL_WriteRegister(0x43, 0x1E);
		reg_r2 = I2C_PAN3204LL_ReadRegister(0x46);

		I2C_PAN3204LL_WriteRegister(0x7F, 0x14);
		I2C_PAN3204LL_WriteRegister(0x6A, reg_r1);
		I2C_PAN3204LL_WriteRegister(0x6C, reg_r2);

	}
	else
#endif
	{
		reg6 = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_CONFIGURATION)|0x80;//read sensor status
		I2C_PAN3204LL_WriteRegister(REG_PAN3204LL_CONFIGURATION, reg6); //reset sensor
		WaitUs(25000); //wait reset finish

		//get sensor id
		product_id1 = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_PRODUCT_ID1);    //power-on must re-read product_id1, or it would make mistake
		product_id2 = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_PRODUCT_ID2);
		// product_id3 = I2C_PAN3204LL_ReadRegister(0x60);				//
		product_id3 = I2C_PAN3204LL_ReadRegister(0x7A);

		sensor_type = sensor_type_identify(); //get sensor type
		printf("---sensor_type=%d, id1 = 0x%1x, id2 = 0x%1x\n",sensor_type, product_id1, product_id2);
	}


	// if (poweron)
	{
		if (sensor_type == SENSOR_3205_TJNT)
		{
			//DownloadConfigTable(Config_3205,sizeof(Config_3205));
		}
		else if (sensor_type == SENSOR_3205_TJ3T)
		{
			DownloadConfigTable(Config_3205_TJ3T,sizeof(Config_3205_TJ3T));
		}
		else if (sensor_type == SENSOR_3065)
		{
			// DownloadConfigTable(Config_AN3205,sizeof(Config_AN3205));
			// DownloadConfigTable(Config_3205,sizeof(Config_3205));
			// DownloadConfigTable(Config_3205_TJ3T,sizeof(Config_3205_TJ3T));
		}
		else if (sensor_type == SENSOR_VT108)
		{
			DownloadConfigTable(Config_VT108,sizeof(Config_VT108));
		}
		else if (sensor_type == SENSOR_M8650A)
		{
			//DownloadConfigTable(Config_VT108,sizeof(Config_VT108));
			I2C_PAN3204LL_WriteRegister(REG_PAN3204LL_CONFIGURATION, reg6&0xf7);
		}
		else if (sensor_type == SENSOR_OM18)
		{
			om18_Optimization_Setting();
		}
		else if (sensor_type == SENSOR_YS8009)
		{
			DownloadConfigTable(Config_YS8009, sizeof(Config_YS8009));
		}
		else if (sensor_type == SENSOR_KA8G2)
		{
			KA8G2_Optimization_Setting();
		}
		else if (sensor_type == SENSOR_PAW3212 )
		{
			DownloadConfigTable(Config_3212, sizeof(Config_3212));
		}
		else if (sensor_type == SENSOR_S201B)
		{
			//SensorYS8008_Optimization_Setting();
			I2C_PAN3204LL_WriteRegister(0x09,0xC3);
			I2C_PAN3204LL_WriteRegister(0x22,0x01);

		#if (PROJECT_ID == PID_XT27)
			I2C_PAN3204LL_WriteRegister(0x05, 0xB8);
			I2C_PAN3204LL_WriteRegister(0x0B, 0xF5);
		#endif

		#if (PROJECT_ID == PID_285)
			I2C_PAN3204LL_WriteRegister(0x5F,0x03);
			unsigned char temp0 = I2C_PAN3204LL_ReadRegister(0x22);
			unsigned char temp1 = I2C_PAN3204LL_ReadRegister(0x5F);
			printf("---> temp = %1x, %1x\n", temp0, temp1);
		#endif
			I2C_PAN3204LL_WriteRegister(0x09,0x00);
		}
		else if (sensor_type == SENSOR_PAW3805EK_CJV1)
		{
			DownloadConfigTable(config_paw3805ek_cjv1, sizeof(config_paw3805ek_cjv1));
		}
		else if (sensor_type == SENSOR_PAW3104DB_TXDT)
		{
			DownloadConfigTable(config_paw3104db_txdt, sizeof(config_paw3104db_txdt));
		}
		else if (sensor_type == SENSOR_PAW3220DB_TJDL)
		{
			DownloadConfigTable(config_paw3220db_tjdl, sizeof(config_paw3220db_tjdl));
		}
		else if ( sensor_type == SENSOR_SG8670 )
		{
			printf("sg8670 init \n");
			I2C_PAN3204LL_WriteRegister(REG_PAN3204LL_CONFIGURATION, reg6&0xf7);
			DownloadConfigTable(config_sg8670, sizeof(config_sg8670));
		}
		else if ( sensor_type == SENSOR_S205 )
		{
			// DownloadConfigTable(Config_s205_tab, sizeof(Config_s205_tab));
			DownloadConfigTable(Config_s205_game_tab, sizeof(Config_s205_game_tab));
		}
		else if ( sensor_type == SENSOR_SG8925 )
		{
			 DownloadConfigTable(config_sg8925, sizeof(config_sg8925));
		}
		else if ( sensor_type == SENSOR_S101UL )
		{
			DownloadConfigTable(config_s101ul_tab, sizeof(config_s101ul_tab));
		}
		else if ( sensor_type == SENSOR_3311 )
		{
			DownloadConfigTable(config_3311_part_A, sizeof(config_3311_part_A));
			config_3311_part_B();

		//	I2C_PAN3204LL_WriteRegister(REG_PAN3204LL_CONFIGURATION, 0x00); //reset sensor
		//	sleep_ms(10);
		//	I2C_PAN3204LL_WriteRegister(0x19, 0x10);
		I2C_PAN3204LL_WriteRegister(0x7F, 0x00);
		I2C_PAN3204LL_WriteRegister(0x3A, 0x5A);  // load configuration
		sleep_ms(50);							  // ����ȣ�

			I2C_PAN3204LL_ReadRegister(0x02);
			I2C_PAN3204LL_ReadRegister(0x03);
			I2C_PAN3204LL_ReadRegister(0x04);
			I2C_PAN3204LL_ReadRegister(0x05);
			I2C_PAN3204LL_ReadRegister(0x06);

			DownloadConfigTable(config_3311_part_C, sizeof(config_3311_part_C));


			I2C_PAN3204LL_ReadRegister(0x02);
			I2C_PAN3204LL_ReadRegister(0x03);
			I2C_PAN3204LL_ReadRegister(0x04);
			I2C_PAN3204LL_ReadRegister(0x05);
			I2C_PAN3204LL_ReadRegister(0x06);
		}
	}
	
	return sensor_type;
}


/**
 *
 @brief	get sensor motion data
 * @param	no_overflow
 * @return	get result 0:failed 1:success
 */
unsigned int OPTSensor_motion_report( u32 no_overflow )
{
	unsigned int optical_status = 0;
	unsigned short int reg_x = 0, reg_y = 0, reg_X_Y_H = 0;
	unsigned short int x_low = 0, y_low = 0;

	//sensor ph5205 no overflow read

	if ((sensor_type == SENSOR_PAW3212) || (sensor_type == SENSOR_OM18) || (sensor_type == SENSOR_KA8G2) || (sensor_type == SENSOR_PAW3805EK_CJV1) 
		|| (sensor_type == SENSOR_PAW3104DB_TXDT) || (sensor_type == SENSOR_PAW3220DB_TJDL) || (sensor_type == SENSOR_SG8670) )
	{
		no_overflow = 1;
	}

	if ( sensor_type == SENSOR_S205 || sensor_type == SENSOR_SG8925 || sensor_type == SENSOR_S101UL || sensor_type == SENSOR_3311)
	{
		no_overflow = 1;
	}

	ms_data.x = 0;
	ms_data.y = 0;

#if 1
	if (OPTSensor_resync(33)==0)
	{
		return 0;
	}
#endif

#if SENSOR_MOTION_ENABLE
    if (gpio_read(PIN_SIF_MOTION))
    { //if motion PIN is high level, means sensor have no data, return directly
     //   return 0;
    }
#endif

	optical_status = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_MOTION_STATUS); //get sensor status

	if ( (optical_status & MOTION_STATUS_MOT)  || (no_overflow && ((optical_status & MOTION_STATUS_DXOVF) || (optical_status & MOTION_STATUS_DYOVF))))
	{ //sensor data valid
	#if SENSOR_DATA_LENGTH_12_BIT_ENABLE
		if( sensor_type == SENSOR_PAW3805EK_CJV1 )
		{
			x_low = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_DELTA_X);
			y_low = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_DELTA_Y);
		
			reg_x = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_DELTA_X_H) << 8;  //
			reg_y = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_DELTA_Y_H) << 8;  //

			reg_x |= x_low;
			reg_y |= y_low;

			ms_data.x = reg_x;
			ms_data.y = reg_y;
		}
		else if ((sensor_type == SENSOR_OM18) || (sensor_type == SENSOR_PAW3212) || (sensor_type == SENSOR_PAW3104DB_TXDT) || (sensor_type == SENSOR_PAW3220DB_TJDL) || ( sensor_type == SENSOR_SG8670 ) ) //
		{
			/* get 12bit length sensor data */
			reg_X_Y_H = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_DELTA_XY_H);
			reg_x = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_DELTA_X)|((reg_X_Y_H&0xF0)<<4);  // DIRECTION_CLOCK_12 X
			reg_y = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_DELTA_Y)|((reg_X_Y_H&0x0F)<<8);  // DIRECTION_CLOCK_12 Y
			//reg_z = I2C_PAN3204LL_ReadRegister(0x06);
			
			//printf("optical_status = %d, reg_x = %d, reg_y = %d, reg_z = 0x%1x\n", optical_status, reg_x, reg_y, reg_z);
		#if 0
			if (reg_x==0x800)
				reg_x = 0x801;
			if (reg_y==0x800)
				reg_y = 0x801;
			
			if ( !no_overflow ){			
				if (optical_status & MOTION_STATUS_DXOVF)
					reg_x = ((signed char)reg_x>=0)? 0x801:0x7ff;   //x overflow, current<0,0x7f, current>0, 0xff

				if (optical_status & MOTION_STATUS_DYOVF)
					reg_y = ((signed char)reg_y>=0)? 0x801:0x7ff;   //y overflow, current<0,0x7f, current>0, 0xff
			}
		#endif

			if (reg_x & 0x0800)
			{
				ms_data.x = 0xf000 | reg_x;
			}
			else
			{
				ms_data.x = reg_x;
			}

			if (reg_y & 0x0800)
			{
				ms_data.y = 0xf000 | reg_y;
			}
			else
			{
				ms_data.y = reg_y;
			}
		}
		else if(sensor_type == SENSOR_S201B || sensor_type == SENSOR_S205 || sensor_type == SENSOR_S101UL )// 16 bit sensor
		{
			reg_x = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_DELTA_X); // DIRECTION_CLOCK_12 X
			reg_y = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_DELTA_Y);  // DIRECTION_CLOCK_12 Y

			reg_x |= I2C_PAN3204LL_ReadRegister(0x20) << 8;
			reg_y |= I2C_PAN3204LL_ReadRegister(0x21) << 8;

            ms_data.x = reg_x;
            ms_data.y = reg_y;
		}
		else if ( sensor_type == SENSOR_3311 )
		{
			reg_x = I2C_PAN3204LL_ReadRegister(0x03); // DIRECTION_CLOCK_12 X
			reg_x |= I2C_PAN3204LL_ReadRegister(0x04) << 8;
			reg_y = I2C_PAN3204LL_ReadRegister(0x05);  // DIRECTION_CLOCK_12 Y
			reg_y |= I2C_PAN3204LL_ReadRegister(0x06) << 8;

            ms_data.x = reg_x;
            ms_data.y = reg_y;

            if (reg_x || reg_y )
             printf("xy=%2x, %2x \n", reg_x, reg_y);
		}
		else
	#endif
		{
			/* get 8bit length sensor data */
			reg_x = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_DELTA_X);  // DIRECTION_CLOCK_12 X
			reg_y = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_DELTA_Y);  // DIRECTION_CLOCK_12 Y

			if (reg_x == 0x80)
				reg_x = 0x81;

			if (reg_y == 0x80)
				reg_y = 0x81;

			if ( !no_overflow )
			{
				if (optical_status & MOTION_STATUS_DXOVF)
					reg_x = ((signed char)reg_x>=0)? 0x81:0x7f;   //x overflow, current<0,0x7f, current>0, 0xff

				if (optical_status & MOTION_STATUS_DYOVF)
					reg_y = ((signed char)reg_y>=0)? 0x81:0x7f;   //y overflow, current<0,0x7f, current>0, 0xff
			}

			if (reg_x & 0x0080)
			{
				ms_data.x = 0xff00 | reg_x;
			}
			else
			{
				ms_data.x = reg_x;
			}

			if (reg_y & 0x0080)
			{
				ms_data.y = 0xff00 | reg_y;
			}
			else
			{
				ms_data.y = reg_y;
			}
		}

		return 1;
	}
	else{  //sensor data invalid
		return 0;
	}
}

/**
 * @brief	set optical sensor to shutdown mode
 * @param	none
 * @return	none
 * #note 	Must clear SLP_ENH bit in the OPERATION_MODE register before using Power-Down mode.
 */
void OPTSensor_Shutdown(void)
{
	u8 reg06 = 0;
	
	for (u8 i=0; i<8; i++)
	{  
		/* set sensor to shutdown */
		reg06 = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_CONFIGURATION);
		I2C_PAN3204LL_WriteRegister(REG_PAN3204LL_CONFIGURATION,reg06|0x08);

		sleep_us(1000); //wait set
		reg06 = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_CONFIGURATION); //read back set result
		
		if (reg06 & 0x08)
		{ //set success, break while
			break;
		}
		else
		{ //set fail, resync
			OPTSensor_resync(1);
		}
	}
	
	sensor_gpio_powerDownConfig(); //reset sensor pin when MCU power down
}

const unsigned char fct3065_xy_dpi_tbl[] = {
	2, // 1200,  1300
	3, // 1600,
	5, // 2400,
	5, // 3200,
};

void sensor3065_xy_cpi_set(u8 dpi_val)
{
	unsigned char temp = 0x00;
	u8	reg_1 = 0, reg_2 = 0, i = 0;

	for (i = 0; i < 16; i++)
	{
		if ( dpi_val <= 1 )
		{
			temp = fct3065_xy_dpi_tbl[dpi_val];

		#if DPI_3065XY_FOUR_LEVEL_ENABLE
			dpi_3605_limit_level_flag = 0;
		#endif

			reg_2 = (I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_CONFIGURATION) & 0xf8) | (temp & 0x07);
			I2C_PAN3204LL_WriteRegister(REG_PAN3204LL_CONFIGURATION, reg_2);
			reg_1 = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_CONFIGURATION);

			if (reg_1 == reg_2)
			{
				return;
			}
		}
		else if ( dpi_val == 2 || dpi_val == 3 )
		{
			I2C_PAN3204LL_WriteRegister(REG_PAN3204LL_WRITE_PROTECT, 0x5A);
			I2C_PAN3204LL_WriteRegister(REG_PAN3204LL_CONFIGURATION, 0x04);

			// temp = (I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_CONFIGURATION) & 0xf8) | 0x04;
			// I2C_PAN3204LL_WriteRegister(REG_PAN3204LL_CONFIGURATION, temp);

			temp = I2C_PAN3204LL_ReadRegister(0x10);

			if ( 0x22 == temp )
			{
				if ( dpi_val == 2 )
				{
					I2C_PAN3204LL_WriteRegister(0x69, 0x60);
					I2C_PAN3204LL_WriteRegister(0x6A, 0x60);

					reg_1 = I2C_PAN3204LL_ReadRegister(0x69);
					reg_2 = I2C_PAN3204LL_ReadRegister(0x6A);

					if ( reg_1 == 0x60 && reg_2 == 0x60 )
					{
						I2C_PAN3204LL_WriteRegister(REG_PAN3204LL_WRITE_PROTECT, 0x00);
						printf("--> set cpi 2400 ok 00 \n");
						return;
					}
				}
				else if ( dpi_val == 3 )
				{
					I2C_PAN3204LL_WriteRegister(0x69, 0x7F);
					I2C_PAN3204LL_WriteRegister(0x6A, 0x7F);

					reg_1 = I2C_PAN3204LL_ReadRegister(0x69);
					reg_2 = I2C_PAN3204LL_ReadRegister(0x6A);

					if ( reg_1 == 0x7F && reg_2 == 0x7F )
					{
						I2C_PAN3204LL_WriteRegister(REG_PAN3204LL_WRITE_PROTECT, 0x00);
						printf("--> set cpi 3200 ok 00 \n");
						return;
					}
				}
			}
			else if ( 0x26 == temp)
			{
				if ( dpi_val == 2 )
				{
					I2C_PAN3204LL_WriteRegister(0x75, 0x3F);
					I2C_PAN3204LL_WriteRegister(0x76, 0x3F);

					reg_1 = I2C_PAN3204LL_ReadRegister(0x75);
					reg_2 = I2C_PAN3204LL_ReadRegister(0x76);

					if ( reg_1 == 0x3F && reg_2 == 0x3F )
					{
						I2C_PAN3204LL_WriteRegister(REG_PAN3204LL_WRITE_PROTECT, 0x00);
						printf("--> set cpi 2400 ok 11 \n");
						return;
					}
				}
				else if ( dpi_val == 3 )
				{
					I2C_PAN3204LL_WriteRegister(0x75, 0x5A);
					I2C_PAN3204LL_WriteRegister(0x76, 0x5A);

					reg_1 = I2C_PAN3204LL_ReadRegister(0x75);
					reg_2 = I2C_PAN3204LL_ReadRegister(0x76);

					if ( reg_1 == 0x5A && reg_2 == 0x5A )
					{
						printf("--> set cpi 3200 ok 11 \n");
						I2C_PAN3204LL_WriteRegister(REG_PAN3204LL_WRITE_PROTECT, 0x00);
						return;
					}
				}
			}
			else
			{
				#if DPI_3065XY_FOUR_LEVEL_ENABLE
					dpi_value = 2;
					dpi_max_level = 3;
					dpi_3605_limit_level_flag = 1;

					temp = fct3065_xy_dpi_tbl[1];

					reg_2 = (I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_CONFIGURATION) & 0xf8) | (temp & 0x07);
					I2C_PAN3204LL_WriteRegister(REG_PAN3204LL_CONFIGURATION, reg_2);
					reg_1 = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_CONFIGURATION);

					if (reg_1 == reg_2)
					{
						return;
					}
				#endif
			}

			I2C_PAN3204LL_WriteRegister(REG_PAN3204LL_WRITE_PROTECT, 0x00);
		}
	}
}

const u8 TJNT_3205_cpi_tbl[] =
{
	//0, 600,
	1,//800
	//2, //1000
	3,//1300
	4,//1600
};
/**
 * @brief	set cpi to sensor
 * @param	dpi_val
 * @return	none
 */
void sensor3204_cpi_set(u8 dpi_val)
{
	for (u8 i = 0; i < 32; i++)
	{
		u8	reg_1 = 0, reg_2 = 0;
		reg_2 = (I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_CONFIGURATION) & 0xf8) | (dpi_val & 0x07);
		I2C_PAN3204LL_WriteRegister(REG_PAN3204LL_CONFIGURATION, reg_2);
		reg_1 = I2C_PAN3204LL_ReadRegister(REG_PAN3204LL_CONFIGURATION);
		if (reg_1 == reg_2)
		{
			return;
		}
	}
}

const u8 s201x_dpi_tbl[] =
{
#if (PROJECT_ID == PID_DMS06) || (PROJECT_ID == PID_DMS03A) || (PROJECT_ID == PID_601) || (PROJECT_ID == PID_SQ220) || (PROJECT_ID == PID_V106)
    0x8A, 			// cpi_1200
    0x8B, 			// cpi_1600
    0x83, 			// cpi_2400
    0x84,			// cpi_3200
#elif (PROJECT_ID == PID_TX108CD)
	0x89, 			// cpi_1000
    0x8A, 			// cpi_1200
    0x8B, 			// cpi_1600
    0x83, 			// cpi_2400
#elif (PROJECT_ID == PID_XT27)
	0x88, 			// cpi_800
    0x8A, 			// cpi_1200
    0x8B, 			// cpi_1600
    0x83, 			// cpi_2400
#elif (PROJECT_ID == PID_8693) || (PROJECT_ID == PID_G929)
	0x88, 			// cpi_800
	0x8A,           // cpi_1200
	0x8B, 			// cpi_1600
	0x84,			// cpi_3200
#else
    0x8A, 			// cpi_1200
    0x8B, 			// cpi_1600
    0x83, 			// cpi_2400
    0x84,			// cpi_3200 
#endif
};

void S201B_CPI_set(u8 dpi)
{
	for (u8 i = 0; i < 16; i++)
	{
		u8	reg_1 = 0;
		I2C_PAN3204LL_WriteRegister(0x09, 0xC3);
		I2C_PAN3204LL_WriteRegister(REG_S201x_CPI, s201x_dpi_tbl[dpi]);
		reg_1 = I2C_PAN3204LL_ReadRegister(REG_S201x_CPI);
		I2C_PAN3204LL_WriteRegister(0x09, 0x00);
		if(reg_1 == s201x_dpi_tbl[dpi])
		{
			printf("cpi r23 = %1x \n", reg_1);
			return;
		}
		else
		{
			printf("cpi err: %1x, %1x \n", reg_1, s201x_dpi_tbl[dpi]);
		}
	}
}

const u8 SENSOR_VT108_dpi_tbl[]=
{
	//0,//400, 1,//500, 2,//600,
	3,//800
	//4,//1000
	5,//1200
	6,//1600
};
const u8 SENSOR_3065_dpi_tbl[] =
{
#if (PROJECT_ID == PID_AM8697CD) || (PROJECT_ID == PID_AM8697CD_H2)
	2, // 1000
	3, // 1400
	4, // 1600
#else
	//0, 600,
	1,//800
	//2, //1000
	3,//1300
	4,//1600
#endif
};

const u8 SENSOR_S201B_dpi_tbl[]=
{
#if (PROJECT_ID == PID_296B)
	1, // 800
	3, // 1200
	4  // 1600
#elif (PROJECT_ID == PID_M5230)
	1,  // 800
	3,  // 1200
	4,  // 1600
	4,  // 2400
	4,  // 3200
#elif (PROJECT_ID == PID_TX109)
	3,  // 1200
	4,  // 1600
	4,  // 2000  1600 * 5 / 4
	4,  // 2400  1600 * 3 / 2
	4,  // 4000  1600 * 5 / 2
#elif (PROJECT_ID == PID_G61_272)
	3,  // 1200
	4,  // 1600 * 3 / 2 = 2400
	4,  // 1600 *2 = 3200
#elif (PROJECT_ID == PID_FDB_M300) || (PROJECT_ID == PID_TB129)
	1,  // 800
	4,  // 1600
	4,  // 2400  1600 * 3 / 2
	4,  // 3200  1600 * 2
#else
	//0, 600,
	1,//800
	//2, //1000
	3,//1300
	4,//1600
#endif
};

const u8 SENSOR_3065XY_dpi_tbl[]=
{
#if (PROJECT_ID == PID_DMS06)

#else
	0, // 800
	//1, //1000
	2, //1300
	3, //1600
#endif
};

const u8 M8650A_dpi_tbl[]=
{
	0, //800
	//1, //1000
	2, //1200
	3, //1600
};
	
const u8 KA8G2_dpi_tbl[] =
{
#if (PROJECT_ID == PID_LXL256)
	14,  // 800
	27,  // 1600
	40,  // 2400
	66,  // 4000
#elif (PROJECT_ID == PID_M0018)
	14, // 800
	27, // 1600
	54, // 3200
#elif (PROJECT_ID == PID_2187)
	7, // 400
	14, // 800
	20, // 1200
	40, // 2400
	40, // 4200 = 2400 * 7 / 4
#else
	14, //step 60
	//17, //1000
	//40, //2400
	20,//1200
	27,//1620
	//67, //4000
#endif
};

/**
 * @brief	KA8G2_CPI_set
 * @param	dpi_val
 * @return	none
 */
void KA8G2_CPI_set(u8 dpi_val)
{
	for (u8 i = 0; i < 16; i++)
	{
		u8	reg_1 = 0, reg_2 = 0;
		I2C_PAN3204LL_WriteRegister(0x09,0xA5);
		I2C_PAN3204LL_WriteRegister(0x0d, dpi_val);
		I2C_PAN3204LL_WriteRegister(0x0e, dpi_val);
		reg_1 = I2C_PAN3204LL_ReadRegister(0x0d);
		reg_2 = I2C_PAN3204LL_ReadRegister(0x0e);
		I2C_PAN3204LL_WriteRegister(0x09,0x00);
		if ((reg_1 == reg_2) && (reg_2 == dpi_val))
		{
		    return;
		}
	}
}

void s205_CPI_set(u8 dpi_val)
{
	for (u8 i = 0; i < 16; i++)
	{
		u8	reg_1 = 0, reg_2 = 0;
		
		I2C_PAN3204LL_WriteRegister(0x09,0x5A);

		I2C_PAN3204LL_WriteRegister(0x0d, dpi_val);
		I2C_PAN3204LL_WriteRegister(0x0e, dpi_val);
		
		reg_1 = I2C_PAN3204LL_ReadRegister(0x0d);
		reg_2 = I2C_PAN3204LL_ReadRegister(0x0e);
		
		I2C_PAN3204LL_WriteRegister(0x09,0x00);
	
		if ( reg_1 == dpi_val && reg_2 == dpi_val )
		{
			return;
		}
	}
}

const u8 paw3212_dpi_tbl[] =
{
#if (PROJECT_ID == PID_WM2015)
	32, 			// cpi_1200 = 32*38 = 1216
	42, 			// cpi_1600 = 42*38 = 1596
	63, 			// cpi_2400 = 63*38 = 2394
#elif (PROJECT_ID == PID_MS2360)
	22, 			// cpi_1200 = 32*38 = 1216
	43, 			// cpi_1600 = 42*38 = 1596
	63, 			// cpi_2400 = 63*38 = 2394
	63				// cpi_3200 = 63*38 * 4 /3 = 3200
#elif (PROJECT_ID == PID_E368_CHARGE)
	22, 			// cpi_800 = 22*38 = 836
	43, 			// cpi_1600 = 42*38 = 1596
	63, 			// cpi_2400 = 63*38 = 2394
	63				// cpi_3200 = 63*38 * 4 /3 = 3200
#elif (PROJECT_ID == PID_CM2057B)
	22, 			// cpi_800 =  22*38 = 836, step = 38
	32, 			// cpi_1200 = 32*38 = 1216
	43, 			// cpi_1600 = 42*38 = 1634
	63, 			// cpi_2400 = 63*38 = 2394
	63,				// cpi_3200 = 63*38 * 4 / 3 = 3192
	63,				// cpi_4800 = 63*38 * 2 = 4788
#elif (PROJECT_ID == PID_M388)
	#if BLuetoothMouse1_DPI_ENABLE
		22, 			// cpi_800 =  22*38 = 836, step = 38
		32, 			// cpi_1200 = 32*38 = 1216
		63,				// cpi_2400 = 63*38 = 2400
		63, 			// cpi_3200 = 63*38 * 4 / 3  = 3200
		63,				// cpi_4800 = 63*38 * 5 / 3  = 4000
	#else
		22, 			// cpi_800 =  22*38 = 836, step = 38
		43, 			// cpi_1600 = 42*38 = 1634
		63,				// cpi_2400 = 63*38 = 2400
		63, 			// cpi_3200 = 63*38* 4 / 3  = 3200
		63,				// cpi_4800 = 63*38 * 2     = 4800
	#endif
#elif (PROJECT_ID == PID_M740)
	42,				// 42 * 38 = 1596 = 1600
	63,				// 63 * 38 = 2394 = 2400
	63,				// 63 * 38 * 4 / 3 = 3200
	63,				// 63 * 38 *5 / 3  = 4000
#elif (PROJECT_ID == PID_8693) || (PROJECT_ID == PID_G929)
	21,  // 800
	42,  // 1600
	63,  // 2400
	63,  // 4000 2400 * 5 / 3
#elif (PROJECT_ID == PID_0120)
	11,				// cpi_400 =  11*38 = 418,
	22, 			// cpi_800 =  22*38 = 836, step = 38
	32, 			// cpi_1200 = 32*38 = 1216
	63, 			// cpi_2400 = 63*38 = 2394
	63,				// cpi_4200 = 63*38 * 7 / 4 = 4200
#else
	22, 			// cpi_800 =  22*38 = 836, step = 38
	32, 			// cpi_1200 = 32*38 = 1216
	42, 			// cpi_1600 = 42*38 = 1596
	63, 			// cpi_2400 = 63*38 = 2394
	63,
	// cpi_3600 Counterfeiting
#endif
};

/**
 * @brief	paw3212_CPI_set
 * @param	dpi_val
 * @return	none
 */
void paw3212_CPI_set(u8 dpi_val)
{
	for (u8 i = 0; i < 16; i++)
	{
		u8	reg_1 = 0, reg_2 = 0;
		I2C_PAN3204LL_WriteRegister(0x09,0x5A);
		I2C_PAN3204LL_WriteRegister(0x0d, dpi_val);
		I2C_PAN3204LL_WriteRegister(0x0e, dpi_val);
		reg_1 = I2C_PAN3204LL_ReadRegister(0x0d);
		reg_2 = I2C_PAN3204LL_ReadRegister(0x0e);
		I2C_PAN3204LL_WriteRegister(0x09,0x00);
			
		if ((reg_1 == dpi_val) && (reg_2 == dpi_val))
		{
		    return;
		}
	}
}

void paw3220_CPI_set(u8 dpi_val_x, u8 dpi_val_y)
{
	for (u8 i = 0; i < 16; i++)
	{
		u8	reg_1 = 0, reg_2 = 0;
		I2C_PAN3204LL_WriteRegister(0x09,0x5A);
		I2C_PAN3204LL_WriteRegister(0x0d, dpi_val_x);
		I2C_PAN3204LL_WriteRegister(0x0e, dpi_val_y);
		reg_1 = I2C_PAN3204LL_ReadRegister(0x0d);
		reg_2 = I2C_PAN3204LL_ReadRegister(0x0e);
		I2C_PAN3204LL_WriteRegister(0x09,0x00);
			
		if ( (reg_1 == dpi_val_x) && (reg_2 == dpi_val_y) )
		{
			return;
		}
	}
}

void sg8670_CPI_set(u8 dpi_val)
{
	if ( dpi_val >= 10 )
	{
		for (u8 i = 0; i < 16; i++)
		{
			u8	reg_1 = 0, reg_2 = 0;
			I2C_PAN3204LL_WriteRegister(0x7F,0x26);
			I2C_PAN3204LL_WriteRegister(0x09,0x5A);

			I2C_PAN3204LL_WriteRegister(0x7E, 0x00);

			I2C_PAN3204LL_WriteRegister(0x6E, (dpi_val|0x80) );
			I2C_PAN3204LL_WriteRegister(0x6F, (dpi_val&0x7F) );

			reg_1 = I2C_PAN3204LL_ReadRegister(0x6E) & 0x7F;
			reg_2 = I2C_PAN3204LL_ReadRegister(0x6F) & 0x7F;

			I2C_PAN3204LL_WriteRegister(0x7F,0x00);
			I2C_PAN3204LL_WriteRegister(0x09,0x00);

			if ( reg_1 == dpi_val && reg_2 == dpi_val)
			{
				printf(" sg8670 dpi set sucess \n");
				return;
			}
		}
	}
	else
	{
		for (u8 i = 0; i < 16; i++)
		{
			u8	reg_1 = 0;
			I2C_PAN3204LL_WriteRegister(0x09,0x5A);
			I2C_PAN3204LL_WriteRegister(0x06, dpi_val);
			reg_1 = I2C_PAN3204LL_ReadRegister(0x06);
			I2C_PAN3204LL_WriteRegister(0x09,0x00);

			if ( reg_1 == dpi_val)
			{
				printf(" sg8670 dpi set sucess \n");
				return;
			}
		}
	}
}

const u8 om18_dpi_tbl[] =
{
	// 22, 			// cpi_800 =  22*38 = 836, step = 38
	32, 			// cpi_1200 = 32*38 = 1216
	42, 			// cpi_1600 = 42*38 = 1596
	63, 			// cpi_2400 = 63*38 = 2394
	// cpi_3600 Counterfeiting
};

const u8 paw3805ek_cjv1_dpi_tab[5][2] =
{
	{0x1F, 0x22},  //800
	{0x2E, 0x32},  //1200
	{0x3C, 0x43},  //1600
	{0x5B, 0x64},  //2400
	{0x5B, 0x64},  //3200
};

const u8 paw3104db_tsdt_dpi_tbl[] =
{
	21, 			// cpi_800 =  21*38 = 798
	42, 			// cpi_1600 = 42*38 = 1596
	84, 			// cpi_3200 = 84*38 = 3192
	101, 			// 106*38 = 4028
	101,			// 
	101				// 
};

const u8 paw3220db_tjdl_dpi_tbl[6][2] =
{
#if (PROJECT_ID == PID_QXX9)
	{20, 22}, // cpi 800
	{40, 43}, // cpi 1600
	{60, 65}, // cpi 2400
	{80, 87}, // cpi 3200
	{100, 108}, // cpi 4000
	{100, 108}, // cpi 4800 = 4000 * 6 / 5
#elif ((PROJECT_ID == PID_MS2360))
	{20, 22}, // cpi 800
	{40, 43}, // cpi 1600
	{60, 65}, // cpi 2400
	{80, 87}, // cpi 3200
#elif (PROJECT_ID == PID_BM2055)
	{20, 22}, // cpi 800
	{30, 33}, // cpi 1200
	{40, 43}, // cpi 1600
	{60, 65}, // cpi 2400
	{80, 87}, // cpi 3200
	{100, 108}, // cpi 4800 = 4000 * 6 / 5
#else
	{25, 27}, // 
	{50, 54},
	{75, 81}, 
	{100, 108}, 
#endif
};

const u8 sg8670_dpi_tbl[5] =
{
#if (PROJECT_ID == PID_XL2)
	0x05,  // 1200
	0x06,  // 1600
	0x07,  // 2400 
	0x07,  // 2400 
	0x07,  // 2400 
#elif (PROJECT_ID == PID_MS13)
	53, // 1200 53 * 23 = 1219
	79, // 1800 79 * 23 = 1817
	105, // 2400 105 * 23 = 2415
	105, // 3200 105 * 23 * 4 / 3
#elif (PROJECT_ID == PID_BG523) || (PROJECT_ID == PID_M388)
	#if BLuetoothMouse1_DPI_ENABLE
		35,  // 800
		53,  // 1200
		105, // 2400
		105, // 3200 = 2400 * 4 / 3
		105, // 4000 = 2400 * 5 / 3
	#else
		53, // 1200
		105, // 2400
		105, // 3200 = 2400 * 4 / 3
		105, // 4000 = 2400 * 5 / 3
	#endif
#elif (PROJECT_ID == PID_4027)
	35, // 800
	70, // 1600
	105, // 2400
	105, // 4000 = 2400 * 5 / 3
	105  // 8000 = 2400 * 10 / 3
#elif (PROJECT_ID == PID_2187)
	18, // 400
	35, // 800
	53, // 1200
	105, // 2400
	105, // 4200 = 2400 * 7 / 4
#elif (PROJECT_ID == PID_TB903)
	0x05,  // 1200
	0x06,  // 1600
	0x07,  // 2400

	0x06,  // 3200 = 1600 * 2
	0x07,  // 4800 = 2400 * 2

	0x07,  // 6400 = 2400 * 8 / 3
#else
	0x05,  // 1200  real
	0x06,  // 1600  real
	0x06,  // 2000  1600*4 / 5
	0x07,  // 2400  real
	0x07,  // 4000  2400*4 / 5
#endif
};

void paw3805ek_cjv1_dpi_set(u8 dpi_x, u8 dpi_y)
{
	u8	reg_1 = 0, reg_2 = 0;

	for (u8 i = 0; i < 16; i++)
	{
		I2C_PAN3204LL_WriteRegister(0x09,0x5a);

		I2C_PAN3204LL_WriteRegister(0x0D, dpi_x);
		I2C_PAN3204LL_WriteRegister(0x0E, dpi_y);

		reg_1 = I2C_PAN3204LL_ReadRegister(0x0D);
		reg_2 = I2C_PAN3204LL_ReadRegister(0x0E);

		I2C_PAN3204LL_WriteRegister(0x09,0x00);
		if ((reg_1 == dpi_x) && (reg_2 == dpi_y))
		{
			return;
		}
	}
}

/**
 * @brief	om18_CPI_set
 * @param	dpi_val
 * @return	none
 */
void om18_CPI_set(u8 dpi_val)
{
	for (u8 i = 0; i < 16; i++)
	{
		u8	reg_1 = 0, reg_2 = 0;

		I2C_PAN3204LL_WriteRegister(0x09,0x5a);

		I2C_PAN3204LL_WriteRegister(0x10, dpi_val);
		I2C_PAN3204LL_WriteRegister(0x11, dpi_val);
		reg_1 = I2C_PAN3204LL_ReadRegister(0x10);
		reg_2 = I2C_PAN3204LL_ReadRegister(0x11);
		I2C_PAN3204LL_WriteRegister(0x09,0x00);
		if ((reg_1 == reg_2) && (reg_2 == dpi_val))
		{
			return;
		}
	}
}


// _attribute_data_retention_user  u8 xy_multiple_flag = 0;
/**
 * @brief	sensor_CPI_set
 * @param	cpi_ctrl
 * @return	none
 */
void sensor_dpi_set(u8 cpi_ctrl)
{
	if (sensor_type == SENSOR_3205_TJ3T)
	{
		sensor3204_cpi_set(cpi_ctrl+2);
	}
	else if (sensor_type == SENSOR_3205_TJNT)
	{
		//sensor3204_cpi_set(cpi_ctrl+2);
		sensor3204_cpi_set(TJNT_3205_cpi_tbl[cpi_ctrl]);
	}
	else if (sensor_type == SENSOR_3065)
	{
		//sensor3204_cpi_set(cpi_ctrl+2);
		sensor3204_cpi_set(SENSOR_3065_dpi_tbl[cpi_ctrl]);
	}
	else if (sensor_type == SENSOR_3065XY)
	{
		//sensor3204_cpi_set(cpi_ctrl+1);
	#if (PROJECT_ID == PID_DMS06) || DPI_3065XY_FOUR_LEVEL_ENABLE
		sensor3065_xy_cpi_set( cpi_ctrl );
	#else
		sensor3204_cpi_set(SENSOR_3065XY_dpi_tbl[cpi_ctrl]);
	#endif
	}
	else if (sensor_type == SENSOR_VT108)
	{
		//sensor3204_cpi_set(cpi_ctrl+4);
		sensor3204_cpi_set(SENSOR_VT108_dpi_tbl[cpi_ctrl]);
	}
	else if (sensor_type == SENSOR_M8650A)
	{
		//sensor3204_cpi_set(cpi_ctrl+1);
		sensor3204_cpi_set(M8650A_dpi_tbl[cpi_ctrl]+4);
	}
	else if (sensor_type == SENSOR_OM18)
	{
		om18_CPI_set(om18_dpi_tbl[cpi_ctrl]);
	}
	else if (sensor_type == SENSOR_YS8009)
	{
		sensor3204_cpi_set(cpi_ctrl+2);
	}
	else if (sensor_type == SENSOR_KA8G2)
	{
		KA8G2_CPI_set(KA8G2_dpi_tbl[cpi_ctrl]);
	}
	else if (sensor_type == SENSOR_PAW3212)
	{
		paw3212_CPI_set(paw3212_dpi_tbl[cpi_ctrl]);
	}
	else if (sensor_type == SENSOR_S201B)
	{
		S201B_CPI_set( cpi_ctrl );
	}
	else if (sensor_type == SENSOR_PAW3805EK_CJV1)
	{
		paw3805ek_cjv1_dpi_set(paw3805ek_cjv1_dpi_tab[cpi_ctrl][0], paw3805ek_cjv1_dpi_tab[cpi_ctrl][1]);
	}
	else if (sensor_type == SENSOR_PAW3104DB_TXDT)
	{
		paw3212_CPI_set(paw3104db_tsdt_dpi_tbl[cpi_ctrl]);
	}
	else if (sensor_type == SENSOR_PAW3220DB_TJDL)
	{
		paw3220_CPI_set(paw3220db_tjdl_dpi_tbl[cpi_ctrl][0], paw3220db_tjdl_dpi_tbl[cpi_ctrl][1]);
	}
	else if ( sensor_type == SENSOR_SG8670 )
	{
		sg8670_CPI_set(sg8670_dpi_tbl[cpi_ctrl]);
	}
	else if ( sensor_type == SENSOR_S205 )
	{
		s205_CPI_set( s205_dpi_tbl[cpi_ctrl] );
	}
	else if ( sensor_type == SENSOR_SG8925 )
	{
		sg8925_CPI_set(sg8925_dpi[cpi_ctrl][0], sg8925_dpi[cpi_ctrl][1]);
	}
	else if (sensor_type==SENSOR_S101UL)
	{
		s101ul_CPI_set( s101ul_dpi_tbl[cpi_ctrl] );
	}
	else if (sensor_type == SENSOR_3311)
	{
		paw3311_CPI_set( paw3311_dpi_tbl[cpi_ctrl] );
	}
}

void sensor_dpi_default(void)
{
	if ( 1 == dpi_value)
	{
		set_current_cpi_value_handle();
	}

	dpi_value = 1;
//	analog_write(USED_DPI_DEEP_ANA_REG, dpi_value);
	sensor_dpi_set(dpi_value);

#if LED_DPI_VBAT_DIFF_ENABLE
	 sc_bat_dpi_flag = LED_DIFF_DPI_DEFAULT;
#endif

#if BLT_APP_LED_ENABLE && LED_DPI_INDICATE_ENABLE
	dpi_led_show( dpi_value + 1 );
#endif

}


#if (PROJECT_ID == PID_M710)
   extern void led_dpi_insert( unsigned char times);
#endif


#if	LED_RGB_DPI_ENABLE
   extern void led_reg_dpi_display(void);
#endif

   extern void ext_mcu_dpi_time_reset(void);

/**

 * @brief	switch dpi_value process
 * @param	none
 * @return	none
 */
void btn_dpi_set()
{
	dpi_value ++;

	if (dpi_value >= dpi_max_level)
	{
		dpi_value = 0;
	}

	printf("dpi_value = %d \n", dpi_value);

#if DPI_SAVE_FLASH
	set_current_cpi_value_handle(); //save dpi_value to flash
#endif

//	analog_write(USED_DPI_DEEP_ANA_REG, dpi_value); //save dpi_value to analog register

    sensor_dpi_set(dpi_value); //set dpi_value to sensor

#if (PROJECT_ID == PID_660) || (PROJECT_ID == PID_HM668)
	gc_digital_dpi_flag = 1;
	gc_digital_dpi_delay = 0;
#endif

#if (TUBE_DPI_DISP_ENABLE)
	gc_digital_dpi_flag = 1;
	gc_digital_dpi_delay = 0;
#endif

#if (PROJECT_ID == PID_4027) || (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
	gl_cpi_change_flag = 1;
	ext_mcu_dpi_time_reset();
#endif

#if (PROJECT_ID == PID_DMS157)
	#if	LED_RGB_DPI_ENABLE
		led_reg_dpi_display();
	#endif
#endif

#if (LED_DPI_INDICATE_ENABLE)

    vbat_dpi_struct_clean();

#if LED_RGB_DPI_ACTION_ENABLE
    battery_low_flag_clear();
#endif

#if LED_DPI_VBAT_DIFF_ENABLE
	 sc_bat_dpi_flag = LED_DIFF_DPI_DISPLAY;
#endif

#if (PROJECT_ID == PID_M388)
	vbat_dpi_led_two_insert(VBAT_DPI_CHOOSE_DPI, (dpi_value + 1) );
#elif (PROJECT_ID == PID_0120)
	rgb_code_dpi_set( RGB_CODE_STATUS_DPI_START, RGB_CODE_ACT_SET);
#elif (PROJECT_ID == PID_MS631) || (PROJECT_ID == PID_MS358B)
	// do nothing
#else
    dpi_led_show( dpi_value + 1 );
#endif

#endif

}

/**
 * @brief	sensor wake up
 * @param	none
 * @return	none
 */
int Sensor3204_Wakeup(u32 sensor)
{
#if (PROJECT_ID == PID_Q15)
	gpio_write(PIN_SENSOR_CS, 0);
	sleep_ms(1);

	product_id1 = I2C_PAN3204LL_ReadRegister(0x00);
	product_id2 = I2C_PAN3204LL_ReadRegister(0x5F);

	printf("sensor_id =  %1x %1x \n", product_id1, product_id2);

	if ( 0x50 == product_id1 && 0xAF == product_id2 )
	{
		sensor_type = SENSOR_3311;
		printf("sensor = PAW3311 \n");
	}
#endif

    return 1;
}

/**
 * @brief	modify sensor data by sensor_direct
 * @param	sensor_dir
 * @return	none
 */
void check_sensor_dircet(u8 sensor_dir)
{
    s16 temp;

    if (sensor_dir == SENSOR_DIRECTION_CLOCK_3)
    {
        temp = ms_data.y;
        ms_data.y = ms_data.x;
        ms_data.x = temp;
    }
    else if (sensor_dir == SENSOR_DIRECTION_CLOCK_6)
    {
        ms_data.x = -ms_data.x;
    }
    else if (sensor_dir == SENSOR_DIRECTION_CLOCK_9)
    {
        temp = ms_data.y;
        ms_data.y = -ms_data.x;
        ms_data.x = -temp;
    }
    else if (sensor_dir == SENSOR_DIRECTION_CLOCK_12)
    {
    #if (PROJECT_ID == PID_8659A)
        
    #else
        ms_data.y = -ms_data.y;
    #endif
    }
}

_attribute_data_retention_user int sm_sum_x=0;
_attribute_data_retention_user int sm_pre_x=0; 
_attribute_data_retention_user int sm_sum_y=0;
_attribute_data_retention_user int sm_pre_y=0;

/**
 * @brief	smoother sensor data
 * @param	none
 * @return	none
 */
void iir_smoother()
{
    sm_sum_x = sm_sum_x - sm_pre_x + ms_data.x;
    sm_pre_x = sm_sum_x / 2;
    ms_data.x = sm_pre_x;

    sm_sum_y = sm_sum_y - sm_pre_y + ms_data.y;
    sm_pre_y = sm_sum_y / 2;
    ms_data.y = sm_pre_y;
}

/**
 * @brief	clear smoother data
 * @param	none
 * @return	none
 */
static inline void iir_smoother_clear(void)
{
    sm_sum_x = 0;
    sm_pre_x = 0;

    sm_sum_y = 0;
    sm_pre_y = 0;
}

#define sm_dyn_pth1  6
#define sm_dyn_pth2  4

/**
 * @brief	adaptive smoother sensor data
 * @param	none
 * @return	none
 */
u8 adaptive_smoother()
{
    _attribute_data_retention_user static u8 asm_flg = 0;
    _attribute_data_retention_user static u32 sm_last_smoother_tick = 0;

    //auto clear asm sum when no data for a long time
    if (asm_flg && clock_time_exceed(sm_last_smoother_tick, 100000))
    {
        asm_flg = 0;
        iir_smoother_clear();
    }

    if (!asm_flg)
    {
        if ((abs(ms_data.x) > sm_dyn_pth1) || (abs(ms_data.y) > sm_dyn_pth1))
        {
            asm_flg = 1;
            iir_smoother();
        }
        else
        {
            asm_flg = 0;
            iir_smoother_clear();
        }
    }
    else
    {
        if ((abs(ms_data.x) < sm_dyn_pth2) && (abs(ms_data.y) < sm_dyn_pth2))
        {
            asm_flg = 0;
            iir_smoother_clear();
        }
        else
        {
            asm_flg = 1;
            iir_smoother();
        }
    }

    if (asm_flg)
    {
        sm_last_smoother_tick = clock_time();
    }

    return asm_flg;
}
#endif

