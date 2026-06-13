/********************************************************************************************************
 * @file     AAA_Sensor.h
 *
 * @brief    This is the header file for KMD SDK
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

#ifndef MOUSE_SENSOR_H_
#define MOUSE_SENSOR_H_

#define SENSOR_DATA_LENGTH_12_BIT_ENABLE	1 //0:8Bit  1:12bit

#define SENSOR_3204			0x00
#define SENSOR_3204LL		0x01
#define SENSOR_3205_TJ3T	0x02
#define SENSOR_3205_TJNT	0x03
#define SENSOR_3065			0x04  //Stop production
#define SENSOR_3065XY		0x05
#define SENSOR_VT108		0x06
#define SENSOR_M8650A		0x07
#define SENSOR_OM18			0x08
#define SENSOR_YS8009		0x09
#define SENSOR_KA8G2		0x0a
#define SENSOR_PAW3212	    0x0b
#define SENSOR_S201B	    0x0C
#define SENSOR_A3000		0x0D

#define SENSOR_PAW3805EK_CJV1   0x0E
#define SENSOR_PAW3104DB_TXDT   0x0F
#define SENSOR_PAW3220DB_TJDL   0x10
#define SENSOR_SG8670           0x11
#define SENSOR_S205             0x12
#define SENSOR_SG8925           0x13
#define SENSOR_S101UL           0x14

#define SENSOR_3311     0x15
#define SENSOR_SG8960   0x16

unsigned int OPTSensor_motion_report(u32 no_overflow);
void sensor_dpi_set(u8 dpi);
void sensor_dpi_default(void);

extern  u8 dpi_max_level;
void check_sensor_dircet(u8 sensor_dir);

extern int (*pf_sensor_motion_report)(signed char *pBuf, u32 no_overflow);

u32 mouse_sensor_sleep_wakeup(u8 *p_sensor, u8 *sleep, u32 wakeup);

#endif /* MOUSE_SENSOR_H_ */

