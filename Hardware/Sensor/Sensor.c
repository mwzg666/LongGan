#include "Sensor.h"
#include "CalcDoseRate.h"
#include "CalcCPS.h"
#include "DoseRate.h"
#include "system.h"



#if 0
#define USE_LOW_USV 8000		//8mSv
#define USE_HIGH_USV 10000		//10mSv
#endif
//yao 切换临界切换值↓
#define USE_LOW_USV    2000		//2mSv
#define USE_HIGH_USV   1000		//1mSv
//↑切换临界切换值
#define LOW_SEG 0 //低量程段状态
#define HIG_SEG 1 //高量程段状态
static u8 gDoseSeg = LOW_SEG;//当前处在的段

extern LP_SYSTEM_STTAE SysRunState;

u32 Low_CPS = 0;
u32 High_CPS = 0;

static float LowSmothCPS,HighSmothCPS;
static float LowNOSmothCPS,HighNOSmothCPS;
static float LowSumCPS,HighSumCPS;

static float SmothCPS_B;
static float NOSmothCPS_B;
static float SumCPS_B;

//static float* pSmothCPS = NULL;
//static float Calc_Pa,Calc_Pb,Calc_Pc;
//static U8 useL_H;//使用低通道还是高通道

u32 InSenserCnt = 0;    //  姣忔椂闂存璁℃暟
float OldDr = 0.0;
float NewDr;
float RtCps,NewCps;

void SensorInit(void)
{
    memset((void*)&SysRunState.s_DoseMSG,0,sizeof(STU_DOSERATE));
}

void SensorMeasureBegin(void)
{ 
	Low_CPS = 0;
	High_CPS = 0;
	GetCounter();
}


void CaptureSensorPluseCounter(void)
{
	/**************测试用的****************************************/
	//Low_CPS = 10;
	//High_CPS = 10;
	/*****************************************************/
	
	//FilterLow(Low_CPS);
	//FilterHigh(High_CPS);
	//LowSumCPS += Low_CPS;
	LowSumCPS = GetCounter();
	//HighSumCPS += High_CPS;
	HighSumCPS = GetHightCounter();
	if((LowSumCPS == 0)&&(SysRunState.LowChanneloff == 0))
	{
	  	SysRunState.LChannelNoCountTime++;
	}
	else
	{
	  	SysRunState.LChannelNoCountTime = 0;
	}
	if(HighSumCPS == 0)
	{
	  	SysRunState.HChannelNoCountTime++;
	}
	else
	{
	  	SysRunState.HChannelNoCountTime = 0;
	}        

    switch(gDoseSeg)
    {
        case LOW_SEG:
        {
            //SysRunState.s_DoseMSG.C2 = SysRunState.s_DoseMSG.C1;//高通道剂量率清零
            //LOWCHANNEL_POWER_ON();
            //P1IE |= BIT1;//打开低量程定制中断
            SysRunState.LowChanneloff = 0;//低通道打开
            LowSmothCPS = CalcLow(
                                SysRunState.stParam.s_SysParam.DiYaCanshuA, 
                                SysRunState.stParam.s_SysParam.DiYaCanshuB, 
                                SysRunState.stParam.s_SysParam.DiYaCanshuC,
                                LowSumCPS, 
                                SysRunState.s_DoseMSG.DoseRate,
                                &SysRunState.s_DoseMSG.C1);

            SmothCPS_B = CalcLow(SysRunState.stParam.s_SysParam.DiYaCanshuA, 
                                SysRunState.stParam.s_SysParam.DiYaCanshuB, 
                                SysRunState.stParam.s_SysParam.DiYaCanshuC,
                                LowSumCPS, 
                                SysRunState.s_DoseMSG.DoseRate,
                                &SysRunState.s_DoseMSG.C3);                              
            if (LowSmothCPS != -1)
            {
                SysRunState.s_DoseMSG.DoseRate = SysRunState.s_DoseMSG.C1;
            }
            if (SmothCPS_B != -1)
            {
                SysRunState.s_DoseMSG.DoseRate = SysRunState.s_DoseMSG.C3;
            }
            //else
            //{
            //      SysRunState.s_DoseMSG.DoseRate = SysRunState.s_DoseMSG.C2;
            //}

            if(SysRunState.s_DoseMSG.DoseRate >= USE_LOW_USV)//
            {
                gDoseSeg = HIG_SEG;
                //GM_HIGH;
                ClearCounter();
            }

            #if 0
            else
            {
                gDoseSeg = LOW_SEG;
                GM_LOW;
            }
            #endif
            }
            break;
                  
                  
        case HIG_SEG:
        {
            //SysRunState.s_DoseMSG.C1 = SysRunState.s_DoseMSG.C2;//低通道剂量率清零
            SysRunState.LowChanneloff = 1;//低通道关闭
            //P1IE &= ~BIT1;//关闭低量程定制中断
            HighSmothCPS = CalcHigh(
                            SysRunState.stParam.s_SysParam.GaoYaCanshuA, 
                            SysRunState.stParam.s_SysParam.GaoYaCanshuB, 
                            SysRunState.stParam.s_SysParam.GaoYaCanshuC,
                            LowSumCPS, 
                            SysRunState.s_DoseMSG.DoseRate,
                            &SysRunState.s_DoseMSG.C2);
            if (HighSmothCPS != -1)
            {
                SysRunState.s_DoseMSG.DoseRate = SysRunState.s_DoseMSG.C2;
            }
            //else
            //{
            //    SysRunState.s_DoseMSG.DoseRate = SysRunState.s_DoseMSG.C1;
            //}
            
            if(SysRunState.s_DoseMSG.DoseRate < USE_HIGH_USV)
            {
              gDoseSeg = LOW_SEG;
              GM_LOW;
              ClearCounter();
            }

            #if 0
            else
            {
              gDoseSeg = HIG_SEG;
              GM_HIGH;
            }
            #endif
         }
        break; 
       
        default: gDoseSeg = LOW_SEG;break;
    }

	LowNOSmothCPS = LowSumCPS;
	HighNOSmothCPS = HighSumCPS;	
    NOSmothCPS_B = LowSumCPS;
		
	HighSumCPS = 0;
	LowSumCPS = 0;
	
	/*if(SysRunState.s_DoseMSG.C1 > 1)
	{
		//剂量率大于1，停止
		LowSumCPS = 0;
	}*/
	
	SysRunState.s_DoseMSG.P1 = LowNOSmothCPS;
	SysRunState.s_DoseMSG.P2 = HighNOSmothCPS;
	SysRunState.s_DoseMSG.P3 = NOSmothCPS_B;

	/*if(SysRunState.testtime>0)
	{
	  	SysRunState.s_DoseMSG.DoseRate = 999.9;
	}*/
	SysRunState.s_DoseMSG.Dose += SysRunState.s_DoseMSG.DoseRate/3600.0f;
    SysRunState.s_DoseMSG.Dose_B += SysRunState.s_DoseMSG.DoseRate/3600.0f;
	//SysRunState.s_DoseMSG.Dose = LowNOSmothCPS;
	
	if(SysRunState.s_DoseMSG.DoseRate>SysRunState.s_DoseMSG.MaxDoseRate)
	{
		SysRunState.s_DoseMSG.MaxDoseRate = SysRunState.s_DoseMSG.DoseRate;
	}
    
    if(SysRunState.s_DoseMSG.DoseRate>SysRunState.s_DoseMSG.MaxDoseRate_B)
	{
		SysRunState.s_DoseMSG.MaxDoseRate_B = SysRunState.s_DoseMSG.DoseRate;
	}
	CalcAlarmState(&SysRunState);           

}

float Get_Low_Counter(void)
{
	return LowNOSmothCPS;
}

float Get_High_Counter(void)
{
	return HighNOSmothCPS;
}

float Get_Low_Smooth_Counter(void)
{
	return LowSmothCPS;
}

float Get_High_Smooth_Counter(void)
{
	return HighSmothCPS;
}

u16 CalcAlarmState(LP_SYSTEM_STTAE *me)
{
#if 0
  	/* 剂量当量报警检查 */	
	if ((me->s_DoseMSG.Dose >= me->stParam.s_Alarm.DoseAlarm)&&(me->stParam.s_Alarm.DoseAlarm > 0)) 
	{ 
		me->s_DoseMSG.DoSt = 2;
    } 
	/* 剂量当量预警检查 */	
	else if((me->s_DoseMSG.Dose >= me->stParam.s_Alarm.DosePreAlarm)&&(me->stParam.s_Alarm.DosePreAlarm > 0)) 
	{ 
		me->s_DoseMSG.DoSt = 1;
    } 
#endif
	
	//U16 alarmState = me->Alarmstate&BATTARY_LOW_BIT;
  	if(me->s_DoseMSG.DoseRate >= 9999999)//10Sv以上则是过载报警
	{
	  	me->s_DoseMSG.DoseRate = 9999999;
		me->s_DoseMSG.DRSt = 5;
		return true;
	}
	
	/* 剂量当量率报警检查 */	
	if ((me->s_DoseMSG.DoseRate >= me->stParam.s_Alarm.DoseRateAlarm)&&(me->stParam.s_Alarm.DoseRateAlarm > 0)) 
	{ 
		if((++me->DoseRateAlarmCnt) >= MIB_CST_DOSERATE_THRESHOLD_ALARM) {//连续两次报警则认为报警
			me->s_DoseMSG.DRSt = 2;
			return true;
		}
    } else {
		me->DoseRateAlarmCnt= 0x0;
		me->s_DoseMSG.DRSt = 0;
	}
	
	/* 剂量当量率预警检查 */	
	if ((me->s_DoseMSG.DoseRate >= me->stParam.s_Alarm.DoseRatePreAlarm)&&(me->s_DoseMSG.DoseRate < me->stParam.s_Alarm.DoseRateAlarm))
	{ 
		if((++me->DoseRatePreAlarmCnt) >= MIB_CST_DOSERATE_THRESHOLD_WARNING) {//连续两次报警则认为报警
			me->s_DoseMSG.DRSt = 1;
			return true;
		}
    } else {
		me->DoseRatePreAlarmCnt= 0x0;
		me->s_DoseMSG.DRSt = 0;
	}
	
	//if((SysRunState.LChannelNoCountTime>60)&&(SysRunState.HChannelNoCountTime>1200))//低通道1分钟无数据，高通道10分钟无数据则报双探测器异常
	//{
	//  	me->s_DoseMSG.DRSt = 8;
	//}
	//else 
    if(SysRunState.LChannelNoCountTime>60)//低通道1分钟无数据,探测器异常
	{
	  	me->s_DoseMSG.DRSt = 6;
	}
	//else if(SysRunState.HChannelNoCountTime>1200)//高通道20分钟无数据,探测器异常
	//{
	  	//me->s_DoseMSG.DRSt = 7;
	//}
	else
	{
	  	me->s_DoseMSG.DRSt = 0;
	}
	return true;
}



// Port 1 interrupt service routine
//#pragma vector=PORT1_VECTOR
void common_Port1_isr() interrupt 38
{
    if(P1INTF & (1<<1))
 	{
	  	// 传感器 脉冲 LOW channel
		Low_CPS ++;
		P1INTF &= ~(1<<1);                           // P1.1 IFG cleared
	}
}

