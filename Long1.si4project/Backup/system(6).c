#include "main.h"
#include "uart.h"
#include "system.h"

u16  Timer0Cnt = 0;
u32  PluseCnt = 0;
u8   LowOverFlowFlag = 0;
u8   PlusePortCnt = 0;

void SysInit()
{
    HIRCCR = 0x80;           // 启动内部高速IRC
    while(!(HIRCCR & 1));
    CLKSEL = 0; 
}


//========================================================================
// 函数名称: void Timer0_Init(void)	
// 函数功能: 在时钟源为11.0592MHz下TIMER0初始化为10ms定时
// 入口参数: @无
// 函数返回: 无
// 当前版本: VER1.0
// 修改日期: 2023.5.5
// 当前作者:
// 其他备注: 
//========================================================================
void Timer0_Init(void)		//10毫秒@11.0592MHz
{
	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0x00;				//设置定时初始值
	TH0 = 0xDC;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;				//使能定时器0中断
    PT0  = 1;
    PT0H = 1;
}

//========================================================================
// 函数名称: void Timer3_Init(void)
// 函数功能: TIMER3初始化为下降沿计数模式
// 入口参数: @无
// 函数返回: 无
// 当前版本: VER1.0
// 修改日期: 2023.5.5
// 当前作者:
// 其他备注: 
//========================================================================
void Timer3_Init(void)
{
    T3L = 0x00;
    T3H = 0x00;
    T4T3M  = 0x0c;
    IE2 = 0x20;             //Timer3 interrupt enable
}

//========================================================================
// 函数名称: void Timer1_Init(void)	
// 函数功能: 在时钟源为11.0592MHz下TIMER1初始化为10ms定时
// 入口参数: @无
// 函数返回: 无
// 当前版本: VER1.0
// 修改日期: 2023.5.5
// 当前作者:
// 其他备注: 
//========================================================================
void Timer1_Init(void)		//10毫秒@11.0592MHz
{
	AUXR &= 0xBF;			//定时器时钟12T模式
	TMOD &= 0x0F;			//设置定时器模式
	TL1 = 0x00;				//设置定时初始值
	TH1 = 0xDC;				//设置定时初始值
	TF1 = 0;				//清除TF1标志
	TR1 = 1;				//定时器1开始计时
	ET1 = 1;				//使能定时器1中断
}
 
//========================================================================
// 函数名称: u32 GetCounter(void)
// 函数功能: 获取TIMER3的脉冲计数
// 入口参数: @无
// 函数返回: 脉冲计数的值
// 当前版本: VER1.0
// 修改日期: 2023.5.5
// 当前作者:
// 其他备注: 
//========================================================================
u32 GetCounter(void)
{
  	u32 Return;
	Return = PluseCnt;
	PluseCnt = 0;
	return Return;
}

void Timer0_Isr (void) interrupt 1   //Timer0中断
{
    Timer0Cnt ++;
    SysRunState.NoUartTime++;
    PlusePortCnt++;
}

void Timer3_Isr() interrupt 19      //Timer3中断
{
    LowOverFlowFlag++;
   
}




