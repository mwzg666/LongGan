#ifndef _MAIN_H
#define _MAIN_H

#include "STC32G.h"
#include "stdio.h"
#include "intrins.h"
#include "string.h"
#include "stdlib.h"

#ifdef __cplusplus
 extern "C" {
#endif
  
typedef 	unsigned char	uint8_t;
typedef 	unsigned short	uint16_t;
typedef 	unsigned long	uint32_t;
typedef     unsigned int    uint;    
   
typedef 	unsigned char	BOOL;
#define     TRUE    1
#define     FALSE   0

#define     true    1
#define     false   0

typedef 	unsigned char	BYTE;
typedef 	unsigned short	WORD;
typedef 	unsigned long	DWORD;

typedef 	unsigned char	u8;
typedef 	unsigned short	u16;
typedef 	unsigned long	u32;   

#define bool BYTE

#define alt_u8   BYTE
#define alt_u16 WORD
#define alt_u32 DWORD      
     
#define MAIN_Fosc        11059200UL             // 11.0592M     
     
//定义软件版本号
#define VERSION1 2
#define VERSION2 0
#define VERSION3 3
   
#define SENSOR_LONGPOLE  0x2a           //长杆探头
#define POWER_OFF_TIME (2*60*100)
#define LOW_DIBENDI		0   
#define LOW_GAOBENDI	100
#define HIGH_DIBENDI	0   
#define HIGH_GAOBENDI	50   

#define MIB_CST_DOSERATE_THRESHOLD_ALARM	2	
#define MIB_CST_DOSERATE_THRESHOLD_WARNING	2
   
#define DOSERATE_PRE_ALARM_BIT  (1<<0)		//剂量率预警位
#define DOSERATE_ALARM_BIT      (1<<1)			//剂量率报警位
#define DOSE_PRE_ALARM_BIT      (1<<2)			//剂量当量预警位
#define DOSE_ALARM_BIT          (1<<3)			//剂量当量报警位
#define BATTARY_LOW_BIT         (1<<4)			//电量报警位
#define LOW_BACKGROND_BIT       (1<<5)		//低本底报警位
#define HIGH_BACKGROND_BIT      (1<<6)		//高本底报警位

#define GM_HIGH      P4 |= (1<<2)
#define GM_LOW      P4 &= ~(1<<2)
//#define HV_ON     P2OUT |= BIT3
//#define HV_OFF     P2OUT &= (~BIT3)

#if 0
#define BT_PWON     P5OUT = BIT6
#define BT_PWOFF    P5OUT &= (~BIT6)
#define BT_WAKEUP   P5OUT = BIT2
#define BT_SLEEP    P5OUT &= (~BIT2)
#define BT_CONFIG   P5OUT |= BIT3
#define BT_WORK     P5OUT &= (~BIT3)
#endif
   
#define SENSOR_POWER_ON()    
#define SENSOR_POWER_OFF()   
#define LOWCHANNEL_POWER_ON() //MSP430XX_GPIO_WritePin(GPIO_P3, GPIO_PIN_5, GPIO_PIN_RESET)
#define LOWCHANNEL_POWER_OFF() //MSP430XX_GPIO_WritePin(GPIO_P3, GPIO_PIN_5, GPIO_PIN_SET)  

#define BLUE_PWON()     P4 |= (1<<1)
#define BLUE_PWOFF()    P4 &= ~(1<<1)
#define BLUE_SLEEP()    P3 &= ~(1<<3)       //低功耗模式，只能接收数据
#define BLUE_WAKEUP()   P3 |= (1<<3)	    //高电平后延时10ms才能发送数据
#define BLUE_CFG_MODE() P4 |= (1<<0)
#define BLUE_WORK_MODE()    P4 &= ~(1<<0)
   
#pragma pack(2)
//系统配置参数
typedef struct
{
  	float  Hv; // 0x01+高压值
    float  Z1;  //0x02+甄别器阈值1
    float  Ct;  // 0x03+计数时间
    float  Hd;  //0x04+高压误
    float  Z2;  // 0x05+甄别器阈值2
    float  Z3;  //0x06+甄别器阈值3
    float  S1;  // 0x07+探测器修正系数1
    float  S2;  //0x08+探测器修正系数2
    float  Cr;  // 0x09+校准因子
    uint8_t  Hn;  //0x0A+启用核素号
    float  S3;  //0x0B 探测器修正系数3
    float  Z4;  //0x0C 甄别器阈值4

	float PingHuaShiJian;//平滑时间

	float DiYaCanshuA;//低量程通道校准因子A		LP
	float DiYaCanshuB;// 低量程通道校准因子B 	LP
	float DiYaCanshuC;// 低量程通道校准因子C 	LP
	float GaoYaCanshuA;//高量程通道校准因子A 	LP
	float GaoYaCanshuB;// 高量程通道校准因子B 	LP
	float GaoYaCanshuC;// 高量程通道校准因子C 	LP
}SYS_PRAM;
//报警信息
typedef struct
{
  	float  A1;  // 0x01+一级报警阈值(α)
    float  A2;  //0x02+二级报警阈值(α)
    float  Al;  // 0x03+低本底报警阈值(α)
    float  Ah;  //0x04+高本底报警阈值(α)
    float  B1;  // 0x05+一级报警阈值(β)
    float  B2;  //0x06+二级报警阈值(β)
    float  Bl;  // 0x07+低本底报警阈值(β)
    float  Bh;  //0x08+高本底报警阈值(β)
    float  Ac;  // 0x09+alphy核素报警值
    float  Bc;  //0x0A+beta核素报警值
    float  DoseRatePreAlarm;  //0x0B Y剂量率一级报警(uSv/h)
    float  DoseRateAlarm;  //0x0C Y剂量率二级报警(uSv/h)
    float  Y3;  //0x0D Y剂量率三级报警(uSv/h)
    float  DosePreAlarm;  //0x0E 累计剂量预警阀值
    float  DoseAlarm;  //0x0F 累计剂量报警阀值
    uint8_t   Zu;  //0x10 内部累计剂量柱报警
}SYS_ALARM;

//计数结构体    LP
typedef struct
{
	float  Aj;  uint8_t AjSt;   //0x01+正在调整
    float  Hv;  uint8_t HvSt;   //0x02+高压故障
    float  C1;  uint8_t C1St;   //0x03+计数值1+报警状态（1B）（α计数）//C1 低通道剂量率 LP
    float  C2;  uint8_t C2St;   //0x04+计数值2+报警状态（1B）（β计数）//C2 高通道剂量率 LP
    float  C3;  uint8_t C3St;   // 0x05+计数值3+报警状态（1B）(α+β)
    float  P1;  uint8_t P1St;   //0x06+平滑计数值1+报警状态（1B）（α计数）//P1低通道原始计数 LP
    float  P2;  uint8_t P2St;   //0x07+平滑计数值2+报警状态（1B）（α计数）//P2高通道原始计数 LP
    float  P3;  uint8_t P3St;   //0x08+平滑计数值3+报警状态（1B）（α计数）
    float  DoseRate;  uint8_t DRSt;   // 0x09+剂量率+报警状态（1B）uSv/h   LP
    float  Dose;  uint8_t DoSt;   // 0x0A+累计剂量+报警状态（1B）uSv   LP
	float  MaxDoseRate;  uint8_t MRSt;	 //最大剂量率uSv/h   LP 

}STU_DOSERATE;

#pragma pack()

typedef struct
{
	uint16_t SensorType;//探头类型
	uint8_t VerSion1;	//大版本
	uint8_t VerSion2;	//小版本
	uint16_t VerSion3;	//SVN号
	SYS_PRAM s_SysParam;

	SYS_ALARM s_Alarm;

	uint16_t ParaCheckSum;//参数校验和
}LP_PARAM;

typedef struct
{
  	LP_PARAM	stParam;
	uint8_t isSleep;
	uint8_t isCanReadSensor;
	STU_DOSERATE s_DoseMSG;
	uint16_t DoseRatePreAlarmCnt;   //剂量率预警次数，连续2次报警能报警
	uint16_t DoseRateAlarmCnt;      //剂量率报警次数，连续2次报警能报警
	uint32_t NoUartTime;		    //无串口通信时间
	uint32_t LChannelNoCountTime;			//低通道无计数时间
	uint32_t HChannelNoCountTime;			//高通道无计数时间
	uint8_t	 LowChanneloff;					//低通道是否关闭，关闭后就不计报警
	//uint16_t testtime;
}LP_SYSTEM_STTAE;

//电池电量结构体
typedef struct
{
	uint16_t Voltage;		//电池电压值mv   	LP
	uint8_t  batPercent;		//电池百分比%		LP
	uint8_t  Status;        //0：正常，1：电池电压低
}STU_BATTERY;


#ifdef __cplusplus
}
#endif


extern LP_SYSTEM_STTAE SysRunState;


void Error();
void InitParam();
void delay_ms(WORD ms);
void delay_us(BYTE us);
void TimerTask();
void Rs485Hnd();

#ifdef __cplusplus
}
#endif

#endif

