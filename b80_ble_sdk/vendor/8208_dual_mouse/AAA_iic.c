#include "AAA_iic.h"

#if (IIC_DRIVE_ENABLE) || MODULE_OLED_ENABLE

#include "AAA_oled_font.h"

u8 OLED_GRAM[OLED_X_MAX][OLED_P_MAX];
u8 x_offset;

#if (PROJECT_ID == PID_4027)
	extern unsigned char gl_cpi_change_flag;
#endif

static unsigned int sg_oled_loop_cnt = 0;

#if (CHARGE_CHECK_ENABLE)
	extern void charge_uncharge_check(void);
#endif

#if USB_MODE_ENABLE || USB_5V_CHECK_ENABLE
	extern void charge_plugin_check(void);
#endif

extern unsigned char mouse_wheel_moved(void);

void OLED_ColorTurn(u8 i)
{
	if ( i == 0 )
	{
		OLED_WR_Byte(0xA6,OLED_CMD);
	}
	else if ( i == 1 )
	{
		OLED_WR_Byte(0xA7,OLED_CMD);
	}
}


void OLED_DisplayTurn(u8 i)
{
	if ( i == 0 )
	{
		x_offset=8;
		OLED_WR_Byte(0xD3,OLED_CMD); /*set display offset*/ 
		OLED_WR_Byte(0x12,OLED_CMD); 
		OLED_WR_Byte(0xC8,OLED_CMD); //
		OLED_WR_Byte(0xA0,OLED_CMD);
	}
	else if(i==1)
	{
		x_offset=0;
		OLED_WR_Byte(0xD3,OLED_CMD); /*set display offset*/ 
		OLED_WR_Byte(0x2e,OLED_CMD); 
		OLED_WR_Byte(0xC0,OLED_CMD); //·
		OLED_WR_Byte(0xA1,OLED_CMD);
	}
}

void IIC_delay(void)
{
	sleep_us(10);
}

void I2C_Start(void)
{
	OLED_SDA_Set();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SDA_Clr();
	IIC_delay();
	OLED_SCL_Clr();
	IIC_delay();
}


void I2C_Stop(void)
{
	OLED_SDA_Clr();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SDA_Set();
}


void I2C_WaitAck(void)
{
	OLED_SDA_Set();
	IIC_delay();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SCL_Clr();
	IIC_delay();
}


void Send_Byte(u8 dat)
{
	u8 i = 0;
	
	for (i = 0; i < 8; i++)
	{
		if( dat & 0x80 )
		{
			OLED_SDA_Set();
		}
		else
		{
			OLED_SDA_Clr();
		}
		IIC_delay();
		OLED_SCL_Set();
		IIC_delay();
		OLED_SCL_Clr();
		dat <<= 1;
	}
}


void OLED_WR_Byte(u8 dat,u8 mode)
{
	I2C_Start();
	Send_Byte(0x78);
	I2C_WaitAck();
	
	if( mode )
	{
		Send_Byte(0x40);
	}
	else
	{
		Send_Byte(0x00);
	}
	I2C_WaitAck();
	Send_Byte(dat);
	I2C_WaitAck();
	I2C_Stop();
}


void OLED_DisPlay_On(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);
	OLED_WR_Byte(0x14,OLED_CMD);
	OLED_WR_Byte(0xAF,OLED_CMD);
}


void OLED_DisPlay_Off(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);
	OLED_WR_Byte(0x10,OLED_CMD);
	OLED_WR_Byte(0xAE,OLED_CMD);
}


void OLED_Refresh(void)
{
	u8 i = 0, n = 0;
	
	for ( i = 0; i < OLED_P_MAX; i++)
	{
		OLED_WR_Byte(0xb0+i,OLED_CMD); 
		OLED_WR_Byte(0x00+x_offset,OLED_CMD); 
		OLED_WR_Byte(0x10,OLED_CMD);
		I2C_Start();
		Send_Byte(0x78);
		I2C_WaitAck();
		Send_Byte(0x40);
		I2C_WaitAck();
		
		for ( n = 0; n < OLED_X_MAX; n++)
		{
			Send_Byte(OLED_GRAM[n][i]);
			I2C_WaitAck();
		}
		I2C_Stop();
	}
}

void oled_buff_clear(void)
{
	u8 i = 0, n = 0;
	
	for ( i = 0; i < OLED_P_MAX; i++)
	{
		for ( n = 0; n < OLED_X_MAX; n++)
		{
			OLED_GRAM[n][i] = 0;
		}
	}
}

void OLED_Clear(void)
{
	u8 i = 0, n = 0;
	
	for ( i = 0; i < OLED_P_MAX; i++)
	{
		for ( n = 0; n < OLED_X_MAX; n++)
		{
			OLED_GRAM[n][i] = 0;
		}
	}
	OLED_Refresh();
}


void OLED_DrawPoint(u8 x, u8 y, u8 t)
{
	u8 i = 0, m = 0, n = 0;
	
	i = y/8;
	m = y%8;
	n = 1<<m;
	
	if(t)
	{
		OLED_GRAM[x][i] |= n;
	}
	else
	{
		OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
		OLED_GRAM[x][i] |= n;
		OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
	}
}


void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode)
{
	u16 t = 0; 
	s16 xerr = 0, yerr = 0, delta_x = 0, delta_y = 0, distance = 0;
	s16 incx = 0, incy = 0, uRow = 0, uCol = 0;
	
	delta_x = x2 - x1; 
	delta_y = y2 - y1;
	
	uRow = x1;
	uCol = y1;
	
	if( delta_x > 0 ) {
		incx = 1; 
	}
	else if ( delta_x == 0 ) {
		incx = 0;
	}
	else {
		incx = -1;
		delta_x = -delta_x;
	}
	
	if( delta_y > 0 ) {
		incy = 1;
	}
	else if ( delta_y == 0 ) {
		incy = 0;
	}
	else { 
		incy = -1;
		delta_y =- delta_x;
	}
	
	if ( delta_x > delta_y ) {
		distance = delta_x;
	}
	else {
		distance = delta_y;
	}
	
	for ( t = 0; t < distance + 1; t++ )
	{
		OLED_DrawPoint(uRow, uCol, mode);
		
		xerr += delta_x;
		yerr += delta_y;
		
		if ( xerr > distance )
		{
			xerr -= distance;
			uRow += incx;
		}
		
		if ( yerr > distance )
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}

void oled_draw_rectangle(u8 x1, u8 y1, u8 x2, u8 y2)
{
	OLED_DrawLine(x1, y1, x2, y1, 1);
	OLED_DrawLine(x1, y1, x1, y2, 1);

	OLED_DrawLine(x1, y2, x2, y2, 1);
	OLED_DrawLine(x2, y1+1, x2, y2-1, 1);
}


void OLED_DrawCircle(u8 x,u8 y,u8 r)
{
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        OLED_DrawPoint(x + a, y - b,1);
        OLED_DrawPoint(x - a, y - b,1);
        OLED_DrawPoint(x - a, y + b,1);
        OLED_DrawPoint(x + a, y + b,1);
 
        OLED_DrawPoint(x + b, y + a,1);
        OLED_DrawPoint(x + b, y - a,1);
        OLED_DrawPoint(x - b, y - a,1);
        OLED_DrawPoint(x - b, y + a,1);
        
        a++;
        num = (a * a + b * b) - r*r;
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}



void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode)
{
	u8 i,m,temp,size2,chr1;
	u8 x0 = x, y0 = y;
	
	if( size1 == 8 )
		size2=6;
	else 
		size2=(size1/8+((size1%8)?1:0))*(size1/2); 
	
	chr1=chr-' '; 
	for( i = 0; i < size2; i++)
	{
		if( size1 == OLED_FONT_SIZE_WITH_8 )
		{
			temp=asc2_0806[chr1][i];
		} 
		else if( size1 == OLED_FONT_SIZE_WITH_12 )
		{
			temp=asc2_1206[chr1][i];
		} 
		else if( size1 == OLED_FONT_SIZE_WITH_16 )
		{
			temp=asc2_1608[chr1][i];
		}
		else if( size1 == OLED_FONT_SIZE_WITH_24 )
		{
			temp=asc2_2412[chr1][i];
		}
		else 
		{
			return;
		}
		
		for( m=0; m<8; m++)
		{
			if( temp&0x01 )
				OLED_DrawPoint(x,y,mode);
			else
				OLED_DrawPoint(x,y,!mode);
			
			temp>>=1;
			y++;
		}
		
		x++;
		if( (size1!=8) && ((x-x0)==size1/2) )
		{
			x=x0;
			y0=y0+8;
		}
		y=y0;
  }
}



void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 mode)
{
	while( (*chr>=' ') && (*chr<='~') )
	{
		OLED_ShowChar(x,y,*chr,size1,mode);
		if(size1==8)
			x += 6;
		else
			x += size1/2;
		chr++;
	}
}

//m^n
u32 OLED_Pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)
	{
		result*=m;
	}
	return result;
}


void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode)
{
	u8 t,temp,m=0;
	if(size1==8)m=2;
	for(t=0;t<len;t++)
	{
		temp=(num/OLED_Pow(10,len-t-1))%10;
		if(temp==0)
		{
			OLED_ShowChar(x+(size1/2+m)*t,y,'0',size1,mode);
		}
		else 
		{
			OLED_ShowChar(x+(size1/2+m)*t,y,temp+'0',size1,mode);
		}
	}
}

#if (OLED_INDICATE_CHINESE)
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode)
{
	
}

void OLED_ScrollDisplay(u8 num,u8 space,u8 mode)
{
	u8 i,n,t=0,m=0,r;
	
	while(1)
	{
		if( m == 0 )
		{
			OLED_ShowChinese(120,6,t,16,mode); 
			t++;
		}
		
		if(t==num)
		{
			for(r=0;r<16*space;r++)
			{
				for(i=1;i<136;i++)
				{
					for(n=0;n<4;n++)
					{
						OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
					}
				}
				OLED_Refresh();
			}
			t=0;
		}
		
		m++;
		if(m==16) { m = 0; }
		
		for( i = 1; i < 136; i++) 
		{
			for( n = 0; n < 4; n++)
			{
				OLED_GRAM[i-1][n] = OLED_GRAM[i][n];
			}
		}
		OLED_Refresh();
	}
}


void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 BMP[],u8 mode)
{
	u16 j=0;
	u8 i,n,temp,m;
	u8 x0=x,y0=y;
	sizey=sizey/8+((sizey%8)?1:0);
	for(n=0;n<sizey;n++)
	{
		for(i=0;i<sizex;i++)
		{
			temp=BMP[j];
			j++;
			
			for(m=0;m<8;m++)
			{
				if(temp&0x01)OLED_DrawPoint(x,y,mode);
				else OLED_DrawPoint(x,y,!mode);
				temp>>=1;
				y++;
			}
			
			x++;
			if((x-x0)==sizex)
			{
				x=x0;
				y0=y0+8;
			}
			y=y0;
		}
	}
}

#endif

void OLED_Init(void)
{
	gpio_output_config(OLED_POWER_PIN);  gpio_write(OLED_POWER_PIN, 1);
	gpio_output_config(OLED_SCL_PIN);    gpio_write(OLED_SCL_PIN,   1);
	gpio_output_config(OLED_SDA_PIN);    gpio_write(OLED_SDA_PIN,   1);
	gpio_output_config(OLED_RES_PIN);    gpio_write(OLED_RES_PIN,   1);

	sleep_ms(50);
	wd_32k_clear();

	OLED_RES_Clr();
	sleep_ms(10);

	OLED_RES_Set();
	sleep_ms(10);
	wd_32k_clear();

	OLED_WR_Byte(0xAE,OLED_CMD); /*display off*/
	OLED_WR_Byte(0xA8,OLED_CMD); /*multiplex ratio*/ 
	OLED_WR_Byte(0x1b,OLED_CMD); /*duty = 1/28*/ 
	OLED_WR_Byte(0xad,OLED_CMD); /*External or Internal IREF Selection */ 
	OLED_WR_Byte(0x40,OLED_CMD);
	OLED_WR_Byte(0xD3,OLED_CMD); /*set display offset*/ 
	OLED_WR_Byte(0x12,OLED_CMD); 
	OLED_WR_Byte(0xA0,OLED_CMD); /*set segment remap*/ 
	OLED_WR_Byte(0xC8,OLED_CMD); /*Com scan direction*/ 
	OLED_WR_Byte(0xA6,OLED_CMD); /*normal / reverse*/ 
	OLED_WR_Byte(0x40,OLED_CMD); /*set display start line*/ 
	OLED_WR_Byte(0xA4,OLED_CMD); 
	OLED_WR_Byte(0x81,OLED_CMD); /*contract control*/ 
	OLED_WR_Byte(0xF0,OLED_CMD); /*128*/ 
	OLED_WR_Byte(0xD9,OLED_CMD); /*set pre-charge period*/ 
	OLED_WR_Byte(0x22,OLED_CMD); 
	OLED_WR_Byte(0xD5,OLED_CMD); /*set osc division*/ 
	OLED_WR_Byte(0x41,OLED_CMD); 
	OLED_WR_Byte(0xDA,OLED_CMD); /*set COM pins*/ 
	OLED_WR_Byte(0x10,OLED_CMD); 
	OLED_WR_Byte(0x20,OLED_CMD); /*Set Memory Addressing Mode */ 
	OLED_WR_Byte(0x02,OLED_CMD); /*Page Addressing Mode*/ 
	OLED_WR_Byte(0xdb,OLED_CMD); /*set vcomh*/ 
	OLED_WR_Byte(0x30,OLED_CMD); 
	OLED_WR_Byte(0x8d,OLED_CMD); /*set charge pump enable*/ 
	OLED_WR_Byte(0x52,OLED_CMD); 
	OLED_WR_Byte(0x08,OLED_CMD); /*set lower column address*/ 
	OLED_WR_Byte(0x10,OLED_CMD); /*set higher column address*/

	OLED_Clear();
	OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
	wd_32k_clear();
	sleep_ms(100);

	OLED_ColorTurn(0);
	OLED_DisplayTurn(1);
	
	sg_oled_loop_cnt = clock_time()|1;
	
	wd_32k_clear();
}

void oled_close(void)
{
	oled_buff_clear();
	OLED_Refresh(); 
	IIC_POWER_OUT(0);
}

void oled_wakeup_and_sleep(unsigned char mode)
{
	if ( 0 == mode )
	{
		IIC_POWER_OUT(0);
		OLED_SCL_Clr();
		OLED_SDA_Clr();
		OLED_RES_Clr();
	}
	else 
	{
		OLED_Init();
	}
}

#if (OLED_DISPLAY_METH_1)

	#define  RECT_X1  2
	#define  RECT_Y1  0

	#define  RECT_X2  27
	#define  RECT_Y2  10

	#define  RECT_LEN   4
	#define  RECT_HIGH  10

#elif OLED_DISPLAY_METH_2

	#define RECT_X1  2
	#define RECT_Y1  0

	#define RECT_X2  25
	#define RECT_Y2  10

	#define RECT_LEN   3
	#define RECT_HIGH  10

	#define BATT_LEVEL_0 10
	#define BATT_LEVEL_1 20
	#define BATT_LEVEL_2 40
	#define BATT_LEVEL_3 60
	#define BATT_LEVEL_4 80
	#define BATT_LEVEL_5 95

	#define BATTERY_CAP_LEVEL 5

#else

	#define  RECT_X1  90
	#define  RECT_Y1  2
	#define  RECT_X2  110
	#define  RECT_Y2  12

	#define  RECT_LEN   4
	#define  RECT_HIGH  10

#endif

#define  OLED_BAT_CHARGE_NONE   0
#define  OLED_BAT_CHARGE_FULL   1
#define  OLED_BAT_CHARGE_ING    2
#define  OLED_BAT_LOWER         3

void oled_batt_display(void)
{
#if (OLED_DISPLAY_METH_1)
	unsigned char i = 0, x1 = 0, y1 = 0;
	static unsigned char sv_cap_index = 0;

	if ( bat_flag.charge_ing || bat_flag.charge_ful )
	{
		oled_draw_rectangle(RECT_X1,   RECT_Y1,   RECT_X2,   RECT_Y2);
		oled_draw_rectangle(RECT_X1+1,   RECT_Y1+1,   RECT_X2-1,   RECT_Y2-1);
		
		OLED_DrawLine(RECT_X2 + 2, RECT_Y1 + 3, RECT_X2 + 2, RECT_Y2 - 3, 1);
		OLED_DrawLine(RECT_X2 + 2, RECT_Y1 + 3, RECT_X2 + 2, RECT_Y2 - 3, 1);
	}
	else
	{
		oled_draw_rectangle(RECT_X1,  RECT_Y1, RECT_X2,  RECT_Y2);
		oled_draw_rectangle(RECT_X1+1,  RECT_Y1+1, RECT_X2-1,  RECT_Y2-1);
		
		OLED_DrawLine(RECT_X2 + 2,   RECT_Y1 + 3, RECT_X2 + 2, RECT_Y2 - 3, 1);
		OLED_DrawLine(RECT_X2 + 3,   RECT_Y1 + 3, RECT_X2 + 3, RECT_Y2 - 3, 1);
	}
	
	if ( bat_flag.charge_ing )
	{
		if ( ++sv_cap_index >= 5 )
		{
			sv_cap_index = 0;
			return;
		}

		for( i = 1; i <= sv_cap_index; i++)
		{
			x1 = RECT_X1 + i*RECT_LEN + i;
			y1 = RECT_Y1;
			OLED_DrawLine(x1 + 0, y1, x1 + 0, y1 + RECT_HIGH, 1);
			OLED_DrawLine(x1 + 1, y1, x1 + 1, y1 + RECT_HIGH, 1);
		}
		printf("--> line act\n");
		return;
	}

	if ( bat_flag.low_batt )
	{
		if ( 0 == sv_cap_index )
		{
			x1 = RECT_X1 + 1*RECT_LEN + 1;
			x1 = x1;
			y1 = RECT_Y1;
			OLED_DrawLine(x1 + 0, y1, x1 + 0, y1 + RECT_HIGH, 1);
			OLED_DrawLine(x1 + 1, y1, x1 + 1, y1 + RECT_HIGH, 1);
		}
		sv_cap_index = (sv_cap_index)?(0):(1);
		return;
	}

	x1 = RECT_X1 + 1*RECT_LEN + 1; 
	y1 = RECT_Y1;
	OLED_DrawLine(x1 + 0, y1, x1 + 0, y1 + RECT_HIGH, 1);
	OLED_DrawLine(x1 + 1, y1, x1 + 1, y1 + RECT_HIGH, 1);
	
	x1 = RECT_X1 + 2*RECT_LEN + 2; 
	y1 = RECT_Y1;
	if ( bat_mv.percent > 25 )
	{
		OLED_DrawLine(x1 + 0, y1, x1 + 0, y1 + RECT_HIGH, 1);
		OLED_DrawLine(x1 + 1, y1, x1 + 1, y1 + RECT_HIGH, 1);
	}
	
	x1 = RECT_X1 + 3*RECT_LEN + 3;
	y1 = RECT_Y1;
	if ( bat_mv.percent > 50 )
	{
		OLED_DrawLine(x1 + 0, y1, x1 + 0, y1 + RECT_HIGH, 1);
		OLED_DrawLine(x1 + 1, y1, x1 + 1, y1 + RECT_HIGH, 1);
	}
	
	x1 = RECT_X1 + 4*RECT_LEN + 4;
	y1 = RECT_Y1;
	if ( bat_mv.percent > 75 )
	{
		OLED_DrawLine(x1 + 0, y1, x1 + 0, y1 + RECT_HIGH, 1);
		OLED_DrawLine(x1 + 1, y1, x1 + 1, y1 + RECT_HIGH, 1);
	}

#elif OLED_DISPLAY_METH_2

	unsigned char i = 0, x1 = 0, y1 = 0;
	static unsigned char sv_cap_index = 0, sv_led_sta = 0;

	if ( bat_flag.charge_ing )
	{
		oled_draw_rectangle(RECT_X1,   RECT_Y1,   RECT_X2,   RECT_Y2);

		OLED_DrawLine(RECT_X2 + 2,    RECT_Y1 + 3, RECT_X2 + 2,  RECT_Y2 - 3, 1);
	}
	else
	{
		if ( bat_flag.low_batt )
		{
			if (  0 == sv_led_sta )
			{
				oled_draw_rectangle(RECT_X1,   RECT_Y1,   RECT_X2,   RECT_Y2);

				OLED_DrawLine(RECT_X2 + 2,    RECT_Y1 + 3,  RECT_X2 + 2, RECT_Y2 - 3, 1);
				OLED_DrawLine(RECT_X2 + 3,	  RECT_Y1 + 4,  RECT_X2 + 3, RECT_Y2 - 4, 1);
			}
		}
		else
		{
			oled_draw_rectangle(RECT_X1,	RECT_Y1,   RECT_X2,   RECT_Y2);

			OLED_DrawLine(RECT_X2 + 2,	RECT_Y1 + 3, RECT_X2 + 2, RECT_Y2 - 3, 1);
			OLED_DrawLine(RECT_X2 + 3,	RECT_Y1 + 4, RECT_X2 + 3, RECT_Y2 - 4, 1);
		}
	}

	if ( bat_flag.charge_ing )
	{
		if ( ++sv_cap_index > BATTERY_CAP_LEVEL )
		{
			sv_cap_index = 0;
			return;
		}

		for( i = 1; i <= sv_cap_index; i++)
		{
			x1 = RECT_X1 + i*RECT_LEN + i - 1;
			y1 = RECT_Y1;

			OLED_DrawLine(x1 + 0, y1 + 2, x1 + 0, y1 + RECT_HIGH - 1, 1);
			OLED_DrawLine(x1 + 1, y1 + 2, x1 + 1, y1 + RECT_HIGH - 1, 1);
		}
		return;
	}

	if ( bat_flag.low_batt )
	{
		if ( 0 == sv_led_sta )
		{
			x1 = RECT_X1 + 2;
			y1 = RECT_Y1;
			OLED_DrawLine(x1 + 0, y1 + 2, x1 + 0, y1 + RECT_HIGH - 1, 1);
		}
		sv_led_sta = (sv_led_sta)?(0):(1);


		return;
	}

	if ( bat_mv.percent < BATT_LEVEL_1 )
	{
		x1 = RECT_X1 + 2;
		y1 = RECT_Y1;

		OLED_DrawLine(x1 + 0, y1 + 2, x1 + 0, y1 + RECT_HIGH - 1, 1);
		OLED_DrawLine(x1 + 1, y1 + 2, x1 + 1, y1 + RECT_HIGH - 1, 1);
	}

	if ( bat_mv.percent >= BATT_LEVEL_1 )
	{
		x1 = RECT_X1 + 1*RECT_LEN;
		y1 = RECT_Y1;

		OLED_DrawLine(x1 + 0, y1 + 2, x1 + 0, y1 + RECT_HIGH - 1, 1);
		OLED_DrawLine(x1 + 1, y1 + 2, x1 + 1, y1 + RECT_HIGH - 1, 1);
	}

	if ( bat_mv.percent >= BATT_LEVEL_2 )
	{
		x1 = RECT_X1 + 2*RECT_LEN + 1;
		y1 = RECT_Y1;

		OLED_DrawLine(x1 + 0, y1 + 2, x1 + 0, y1 + RECT_HIGH - 1, 1);
		OLED_DrawLine(x1 + 1, y1 + 2, x1 + 1, y1 + RECT_HIGH - 1, 1);
	}

	if ( bat_mv.percent >= BATT_LEVEL_3 )
	{
		x1 = RECT_X1 + 3*RECT_LEN + 2;
		y1 = RECT_Y1;

		OLED_DrawLine(x1 + 0, y1 + 2, x1 + 1, y1 + RECT_HIGH - 1, 1);
		OLED_DrawLine(x1 + 1, y1 + 2, x1 + 1, y1 + RECT_HIGH - 1, 1);
	}

	if ( bat_mv.percent >= BATT_LEVEL_4 )
	{
		x1 = RECT_X1 + 4*RECT_LEN + 3;
		y1 = RECT_Y1;

		OLED_DrawLine(x1 + 0, y1 + 2, x1 + 1, y1 + RECT_HIGH - 1, 1);
		OLED_DrawLine(x1 + 1, y1 + 2, x1 + 1, y1 + RECT_HIGH - 1, 1);
	}

	if ( bat_mv.percent >= BATT_LEVEL_5 )
	{
		x1 = RECT_X1 + 5*RECT_LEN + 4;
		y1 = RECT_Y1;

		OLED_DrawLine(x1 + 0, y1 + 2, x1 + 1, y1 + RECT_HIGH - 1, 1);
		OLED_DrawLine(x1 + 1, y1 + 2, x1 + 1, y1 + RECT_HIGH - 1, 1);
	}

#else

	unsigned char i = 0, x1 = 0, y1 = 0;
	static unsigned char sv_cap_index = 0;

	if ( bat_flag.charge_ing || bat_flag.charge_ful )
	{
		oled_draw_rectangle(RECT_X1,   RECT_Y1,   RECT_X2,   RECT_Y2);
		OLED_DrawLine(RECT_X2 + 2, RECT_Y1 + 3, RECT_X2 + 2, RECT_Y2 - 3, 1);
	}
	else
	{
		oled_draw_rectangle(RECT_X1 - 20,   RECT_Y1,   RECT_X2 - 20,   RECT_Y2);
		OLED_DrawLine(RECT_X2  - 20 + 2,    RECT_Y1 + 3, RECT_X2  - 20 + 2, RECT_Y2 - 3, 1);
	}
	
	if ( bat_flag.charge_ing )
	{
		if ( ++sv_cap_index >= 4 )
		{
			sv_cap_index = 0;
			return;
		}

		for( i = 1; i <= sv_cap_index; i++)
		{
			x1 = RECT_X1 + i*RECT_LEN + i;
			y1 = RECT_Y1;
			OLED_DrawLine(x1 + 0, y1, x1 + 0, y1 + RECT_HIGH, 1);
		}
		return;
	}

	if ( bat_flag.low_batt )
	{
		if ( 0 == sv_cap_index )
		{
			x1 = RECT_X1 + 1*RECT_LEN + 1;
			x1 = x1 - 20;
			y1 = RECT_Y1;
			OLED_DrawLine(x1 + 0, y1, x1 + 0, y1 + RECT_HIGH, 1);
		}
		sv_cap_index = (sv_cap_index)?(0):(1);
		return;
	}

	x1 = RECT_X1 + 1*RECT_LEN + 1; 
	y1 = RECT_Y1;
	if ( 0 == bat_flag.charge_ful ) {
		x1 = x1 - 20;
	}
	OLED_DrawLine(x1 + 0, y1, x1 + 0, y1 + RECT_HIGH, 1);

	x1 = RECT_X1 + 2*RECT_LEN + 2; 
	y1 = RECT_Y1;
	if ( 0 == bat_flag.charge_ful ) {
		x1 = x1 - 20;
	}
	if ( bat_mv.percent > 66 )
		OLED_DrawLine(x1 + 0, y1, x1 + 0, y1 + RECT_HIGH, 1);

	x1 = RECT_X1 + 3*RECT_LEN + 3;
	y1 = RECT_Y1;
	if ( 0 == bat_flag.charge_ful ) {
		x1 = x1 - 20;
	}
	if ( bat_mv.percent > 99 )
		OLED_DrawLine(x1 + 0, y1, x1 + 0, y1 + RECT_HIGH, 1);

	if ( bat_flag.charge_ful ) return;
	
	unsigned char cap_tab[6] = {0x00};
	x1 = 94;
	if (  bat_mv.percent < 10 ) {
		cap_tab[i++] = bat_mv.percent  + '0';
	}
	else if ( bat_mv.percent < 100 ) 
	{
		cap_tab[i++] = bat_mv.percent / 10 + '0';
		cap_tab[i++] = bat_mv.percent % 10 + '0';
	}
	else 
	{
		cap_tab[i++] = 1 + '0';
		cap_tab[i++] = 0 + '0';
		cap_tab[i++] = 0 + '0';
	}
	cap_tab[i++] = '%';
	cap_tab[i] = 0;
	
	OLED_ShowString(x1, 2, cap_tab, OLED_FONT_SIZE_WITH_12,1);
#endif
}

void oled_mode_display(void)
{
#if (OLED_DISPLAY_METH_1)
	#define MODE_X   2
	#define MODE_Y   14

	#define DPI_X    44
	#define DPI_Y    2
	
	if ( RF_2M_2P4G_MODE == mcu_mode_get() ) 
	{
		OLED_ShowString(MODE_X,    MODE_Y, (u8 *)"2.", OLED_FONT_SIZE, 1);
		OLED_ShowString(MODE_X+13, MODE_Y, (u8 *)"4G", OLED_FONT_SIZE, 1);
	}
	else if ( RF_1M_BLE_MODE == mcu_mode_get() )
	{
		if ( BLE_DEVICE_ID_0 == ble_mode_device_id_get() )  {
			OLED_ShowString(MODE_X, MODE_Y, (u8 *)"BT1", OLED_FONT_SIZE, 1);
		}
		else if ( BLE_DEVICE_ID_1 == ble_mode_device_id_get() ) {
			OLED_ShowString(MODE_X, MODE_Y, (u8 *)"BT2", OLED_FONT_SIZE, 1);
		}
	}

	if ( 0 == dpi_value )
	{
		OLED_ShowString(DPI_X, DPI_Y, (u8 *)"800", OLED_FONT_SIZE_WITH_24, 1);
	}
	else if ( 1 == dpi_value )
	{
		OLED_ShowString(DPI_X, DPI_Y, (u8 *)"1200", OLED_FONT_SIZE_WITH_24, 1);
	}
	else if ( 2 == dpi_value )
	{
		OLED_ShowString(DPI_X, DPI_Y, (u8 *)"1600", OLED_FONT_SIZE_WITH_24, 1);
	}
	else if ( 3 == dpi_value )
	{
		OLED_ShowString(DPI_X, DPI_Y, (u8 *)"2400", OLED_FONT_SIZE_WITH_24, 1);
	}
#elif (OLED_DISPLAY_METH_2)

	#define MODE_X   59
	#define MODE_Y   14

	#define DPI_X   0
	#define DPI_Y   12

	#define REPORT_X  58
	#define RRPORT_Y  0

	unsigned char dpi_tab[9] = {0x00}, k = 0;
	static unsigned char sg_oled_dis_sta = 0;

	if ( 0 == dpi_value )
	{
		dpi_tab[k++] = '8'; dpi_tab[k++] = '0'; dpi_tab[k++] = '0';// 800
	}
	else if ( 1 == dpi_value )
	{
		dpi_tab[k++] = '1'; dpi_tab[k++] = '6'; dpi_tab[k++] = '0'; dpi_tab[k++] = '0';// 1200
	}
	else if ( 2 == dpi_value )
	{
		dpi_tab[k++] = '2'; dpi_tab[k++] = '4'; dpi_tab[k++] = '0'; dpi_tab[k++] = '0';// 1600
	}
	else if ( 3 == dpi_value )
	{
		dpi_tab[k++] = '4'; dpi_tab[k++] = '0'; dpi_tab[k++] = '0'; dpi_tab[k++] = '0';// 4000
	}
	else if ( 4 == dpi_value )
	{
		dpi_tab[k++] = '8'; dpi_tab[k++] = '0'; dpi_tab[k++] = '0'; dpi_tab[k++] = '0';// 4000
	}

	dpi_tab[k++] = 'd'; dpi_tab[k++] = 'p'; dpi_tab[k++] = 'i'; dpi_tab[k] = 0;

	if ( RF_2M_2P4G_MODE == mcu_mode_get() )
	{
		if ( 0 == connect_ok )
		{
			if ( 0 == sg_oled_dis_sta )
			{
				OLED_ShowString(MODE_X,    MODE_Y, (u8 *)"2.", OLED_FONT_SIZE, 1);
				OLED_ShowString(MODE_X+13, MODE_Y, (u8 *)"4G", OLED_FONT_SIZE, 1);

			#if LED_G24_ENABLE
				gpio_write(PIN_24G_LED, LED_ON);
			#endif

			}
			else
			{
				OLED_ShowString(MODE_X,    MODE_Y, (u8 *)"  ", OLED_FONT_SIZE, 1);
				OLED_ShowString(MODE_X+13, MODE_Y, (u8 *)"  ", OLED_FONT_SIZE, 1);

			#if LED_G24_ENABLE
				gpio_write(PIN_24G_LED, LED_OFF);
			#endif

			}
			sg_oled_dis_sta = (sg_oled_dis_sta)?(0):(1);
		}
		else
		{
			OLED_ShowString(MODE_X,    MODE_Y, (u8 *)"2.", OLED_FONT_SIZE, 1);
			OLED_ShowString(MODE_X+13, MODE_Y, (u8 *)"4G", OLED_FONT_SIZE, 1);

			#if LED_G24_ENABLE
				gpio_write(PIN_24G_LED, LED_OFF);
			#endif
		}
	}
	else if ( RF_1M_BLE_MODE == mcu_mode_get() )
	{

		if ( 0 == connect_ok )
		{
			if ( BLE_DEVICE_ID_0 == ble_mode_device_id_get() )
			{
				if ( 0 == sg_oled_dis_sta )
				{

				#if LED_BT1_ENABLE
					gpio_write(PIN_BLE1_LED, LED_ON);
				#endif

					OLED_ShowString(MODE_X, MODE_Y, (u8 *)"BT1", OLED_FONT_SIZE, 1);
				}
				else
				{

				#if LED_BT1_ENABLE
					gpio_write(PIN_BLE1_LED, LED_OFF);
				#endif

					OLED_ShowString(MODE_X, MODE_Y, (u8 *)"   ", OLED_FONT_SIZE, 1);
				}
			}

			sg_oled_dis_sta = (sg_oled_dis_sta)?(0):(1);
		}
		else
		{
			if ( BLE_DEVICE_ID_0 == ble_mode_device_id_get() )
			{
				OLED_ShowString(MODE_X, MODE_Y, (u8 *)"BT1", OLED_FONT_SIZE, 1);
			}

			#if LED_BT1_ENABLE
				gpio_write(PIN_BLE1_LED, LED_OFF);
			#endif
		}
	}

	OLED_ShowString(DPI_X,     DPI_Y,    (u8 *)dpi_tab,  OLED_FONT_SIZE, 1);

#if (PROJECT_ID == PID_4027)
	// do nothing
#else
	OLED_ShowString(REPORT_X,  RRPORT_Y, (u8 *)"250Hz",  OLED_FONT_SIZE, 1);
#endif

#else

	#define MODE_X   1
	#define MODE_Y   0

	#define DPI_X   1
	#define DPI_Y   14
	
	unsigned char dpi_tab[9] = {0x00};

	dpi_tab[0] = 'D'; dpi_tab[1] = 'P'; 
	dpi_tab[2] = 'I'; dpi_tab[3] = ':';

	if ( 0 == dpi_value )
	{
		dpi_tab[4] = '8'; dpi_tab[5] = '0'; dpi_tab[6] = '0'; dpi_tab[7] = 0;// 800
	}
	else if ( 1 == dpi_value )
	{
		dpi_tab[4] = '1'; dpi_tab[5] = '2'; dpi_tab[6] = '0'; dpi_tab[7] = '0'; dpi_tab[8] = 0;// 1200
	}
	else if ( 2 == dpi_value )
	{
		dpi_tab[4] = '1'; dpi_tab[5] = '6'; dpi_tab[6] = '0'; dpi_tab[7] = '0'; dpi_tab[8] = 0;// 1200
	}
	
	if ( RF_2M_2P4G_MODE == mcu_mode_get() ) 
	{
		OLED_ShowString(MODE_X,    MODE_Y, (u8 *)"2.", OLED_FONT_SIZE, 1);
		OLED_ShowString(MODE_X+13, MODE_Y, (u8 *)"4G", OLED_FONT_SIZE, 1);
	}
	else if ( RF_1M_BLE_MODE == mcu_mode_get() )
	{
		if ( BLE_DEVICE_ID_0 == ble_mode_device_id_get() )  {
			OLED_ShowString(MODE_X, MODE_Y, (u8 *)"BT1", OLED_FONT_SIZE, 1);
		}
		else if ( BLE_DEVICE_ID_1 == ble_mode_device_id_get() ) {
			OLED_ShowString(MODE_X, MODE_Y, (u8 *)"BT2", OLED_FONT_SIZE, 1);
		}
	}
	OLED_ShowString(DPI_X, DPI_Y, (u8 *)dpi_tab, OLED_FONT_SIZE, 1);
#endif
}

void oled_charge_display(void)
{
#if (OLED_DISPLAY_METH_1)
	unsigned char i = 0;
	#define OLED_CHARGE_X  32
	#define OLED_CHARGE_Y  5

	if ( 0 == bat_flag.charge_ful && 0 == bat_flag.charge_ing ) return;
	
	for ( i = 0; i < 4; i++ )
	{
		OLED_DrawPoint(OLED_CHARGE_X + i, OLED_CHARGE_Y - i, 1);
		OLED_DrawPoint(OLED_CHARGE_X + 1 + i, OLED_CHARGE_Y - i, 1);
	}

	for ( i = 0; i < 4; i++ )
	{
		OLED_DrawPoint(OLED_CHARGE_X + i, OLED_CHARGE_Y, 1);
		OLED_DrawPoint(OLED_CHARGE_X + i, OLED_CHARGE_Y+1, 1);
	}
	
	for ( i = 0; i < 4; i++ )
	{
		OLED_DrawPoint(OLED_CHARGE_X + i, OLED_CHARGE_Y + 3 - i, 1);
		OLED_DrawPoint(OLED_CHARGE_X + 1 + i, OLED_CHARGE_Y + 3 - i, 1);
	}
#elif OLED_DISPLAY_METH_2

	unsigned char i = 0;
	#define OLED_CHARGE_X  31
	#define OLED_CHARGE_Y  5

	if ( bat_flag.charge_ing )
	{
		for ( i = 0; i < 4; i++ )
			OLED_DrawPoint(OLED_CHARGE_X + i, OLED_CHARGE_Y - i, 1);

		for ( i = 0; i < 4; i++ )
			OLED_DrawPoint(OLED_CHARGE_X + i, OLED_CHARGE_Y, 1);

		for ( i = 0; i < 4; i++ )
			OLED_DrawPoint(OLED_CHARGE_X + i, OLED_CHARGE_Y + 3 - i, 1);
	}

#else

	unsigned char i = 0;
	#define OLED_CHARGE_X  116
	#define OLED_CHARGE_Y  7

	if ( 0 == bat_flag.charge_ful && 0 == bat_flag.charge_ing ) return;
	
	for ( i = 0; i < 4; i++ )
		OLED_DrawPoint(OLED_CHARGE_X + i, OLED_CHARGE_Y - i, 1);

	for ( i = 0; i < 4; i++ )
		OLED_DrawPoint(OLED_CHARGE_X + i, OLED_CHARGE_Y, 1);

	for ( i = 0; i < 4; i++ )
		OLED_DrawPoint(OLED_CHARGE_X + i, OLED_CHARGE_Y + 3 - i, 1);
#endif
}

void oled_display(void)
{
	#define OLED_TIME_POWEROU 150*1000
	#define OLED_TIME_PAIR 230*1000
	#define OLED_TIME_RECN 800*1000
	#define OLED_TIME_CONN 500*1000
	#define OLED_TIME_SHUT 230*1000

	unsigned int SG_OLED_TIME_LOOP = OLED_TIME_CONN;
	static unsigned char sv_oled_power_on_first = 0, sv_oled_screen_onoff_flag = 0;

	if ( 0 == connect_ok )
	{
		if ( pair_flag )
		{
			SG_OLED_TIME_LOOP = OLED_TIME_PAIR;
		}
		else
		{
			SG_OLED_TIME_LOOP = OLED_TIME_RECN;
		}
	}
	else
	{
		pair_flag = 0;
		SG_OLED_TIME_LOOP = OLED_TIME_CONN;
	}

	
	if ( gl_cpi_change_flag || clock_time_exceed(sg_oled_loop_cnt, SG_OLED_TIME_LOOP) )
	{
		sg_oled_loop_cnt = clock_time();
		sv_oled_power_on_first = 1;
		gl_cpi_change_flag = 0;

		#if (CHARGE_CHECK_ENABLE)
			charge_uncharge_check();
		#endif

		#if USB_MODE_ENABLE || USB_5V_CHECK_ENABLE
			charge_plugin_check();
		#endif

		if (connect_ok && idle_count > OLED_DISPLAY_TIME )
		{
			sv_oled_screen_onoff_flag = 1;
			oled_wakeup_and_sleep( 0 );
			sg_oled_loop_cnt = clock_time();
			return;
		}

		if ( sv_oled_screen_onoff_flag )
		{
			sv_oled_screen_onoff_flag = 0;
			oled_wakeup_and_sleep( 1 );
		}
		
		if ( 0 == mouse_wheel_moved() )
		{
			oled_buff_clear();
			oled_mode_display();
			oled_batt_display();
			oled_charge_display();
			OLED_Refresh(); 
		}
	}
}

#endif

