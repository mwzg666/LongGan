#ifndef SENSOR_H
#define SENSOR_H

#include "main.h"

//===============================变量定义=============================




//===============================结构体定义===========================




//===============================变量声明=============================



//===============================函数声明=============================
void SensorInit(void);
void SensorMeasureBegin(void);
void CaptureSensorPluseCounter(void);

float Get_Low_Smooth_Counter(void);
float Get_High_Smooth_Counter(void);
float Get_Low_Counter(void);
float Get_High_Counter(void);
void Get_Bendi_Value(char time);
float Netro_Calibration(u32 time,u32 refdata,u8 channel);
u16 CalcAlarmState(LP_SYSTEM_STTAE *me);
#endif