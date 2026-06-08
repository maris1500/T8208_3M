#ifndef AAA_IIC_H_
#define AAA_IIC_H_

#include "AAA_app_config_Telink_QFN32.h"

#if (IIC_DRIVE_ENABLE) || MODULE_OLED_ENABLE

#include "AAA_public_config.h"
#include "timer.h"

#define IIC_POWER_OUT(x)   gpio_write(OLED_POWER_PIN, x)

#define OLED_SCL_Clr()     gpio_write(OLED_SCL_PIN, 0)
#define OLED_SCL_Set()     gpio_write(OLED_SCL_PIN,  1)

#define OLED_SDA_Clr()     gpio_write(OLED_SDA_PIN, 0)
#define OLED_SDA_Set()     gpio_write(OLED_SDA_PIN, 1)

#define OLED_RES_Clr()     gpio_write(OLED_RES_PIN, 0)
#define OLED_RES_Set()     gpio_write(OLED_RES_PIN, 1)


#define OLED_CMD   0
#define OLED_DATA  1

#if (PROJECT_ID == PID_4027)

	#define OLED_DISPLAY_METH_2	 1
	#define OLED_X_MAX  128
	#define OLED_P_MAX  4
	#define OLED_Y_MAX  32

#else

	#define OLED_X_MAX  120
	#define OLED_P_MAX  4
	#define OLED_Y_MAX  32

#endif

#define OLED_INDICATE_CHINESE   0

void OLED_ClearPoint(u8 x,u8 y);
void OLED_ColorTurn(u8 i);
void OLED_DisplayTurn(u8 i);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_WaitAck(void);
void Send_Byte(u8 dat);
void OLED_WR_Byte(u8 dat,u8 mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode);
void OLED_DrawCircle(u8 x,u8 y,u8 r);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode);
void OLED_ShowChar6x8(u8 x,u8 y,u8 chr,u8 mode);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode);

#if (OLED_INDICATE_CHINESE)
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode);
void OLED_ScrollDisplay(u8 num,u8 space,u8 mode);
void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 BMP[],u8 mode);
#endif

void OLED_Init(void);
void oled_display(void);

#endif

#endif /* AAA_IIC_H_*/
