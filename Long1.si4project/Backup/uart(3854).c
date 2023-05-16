#include "uart.h"
#include "system.h"
#include <stdio.h>
#include <string.h>

u8  TX1_Cnt;            //发送计数
u8  RX1_Cnt;            //接收计数
bit B_TX1_Busy;         //发送忙标志
u16 Rx1_Timer  = 0;

u8  TX3_Cnt;            //发送计数
u8  RX3_Cnt;            //接收计数
bit B_TX3_Busy;         //发送忙标志
u16 Rx3_Timer  = 0;


u8  RX1_Buffer[MAX_LENGTH]; //接收缓冲
u8  RX3_Buffer[MAX_LENGTH]; //接收缓冲

//========================================================================
// 函数名称: void Uart1_Init(void)
// 函数功能: UART0初始化,9600位波特率/秒,8位字符,1位停止位,无校验.
// 入口参数: @无
// 函数返回: 无
// 当前版本: VER1.0
// 修改日期: 2023.5.5
// 当前作者:
// 其他备注: 波特率9600,时钟源选择为ACLK.误码率可能会大些.
//           用户可选择其他高频时钟源.高频时钟会使波特率的误码率降低.
//========================================================================
void Uart1_Init(void)		//115200bps@11.0592MHz
{  
    SCON = 0x40;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器时钟1T模式
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xE0;			//设置定时初始值
	TH1 = 0xFE;			//设置定时初始值
	ET1 = 0;			//禁止定时器中断
	TR1 = 1;			//定时器1开始计时   
    ES  = 1;            //允许中断
    PS  = 0;            //中断高优先级
    PSH = 0;

    REN = 1;            //允许接收
    
    //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 
    //                 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
    P_SW1 &= 0x3f;
    P_SW1 |= 0x00;  
    
    B_TX1_Busy = 0;
    TX1_Cnt = 0;
    Rx1_Timer  = 0;
    RX1_Cnt = 0;
}


void UART1_ISR (void) interrupt 4
{
    if(RI)
    {
//        if(SysRunState.isSleep == 1)
//    	{
//      		SysRunState.isSleep = 0;
//    		SysRunState.NoUartTime = 0;
//    		//LPM3_EXIT;
//            BLUE_WAKEUP();
//    	}
    
        RI = 0;
        Rx1_Timer = 0;
        RX1_Buffer[RX1_Cnt] = SBUF;
        if(++RX1_Cnt >= MAX_LENGTH)   
        {
            RX1_Cnt = 0;
        }
    }

    if(TI)
    {
        TI = 0;
        B_TX1_Busy = 0;
    }
}

//重定向Printf
char putchar(char c )
{
    SBUF = c;
    while(!TI);
    TI = 0;
    return c;
}
//========================================================================
// 函数名称: void uart_send(u8 *buf, u8 len)
// 函数功能: 串口发送函数
// 入口参数: @*buf：发送的数据；@len：数据长度
// 函数返回: 无
// 当前版本: VER1.0
// 修改日期: 2023.5.5
// 当前作者:
// 其他备注: 
//========================================================================


void Uart3_Init(void)		//9600bps@11.0592MHz
{
	S3CON = 0x10;		//8位数据,可变波特率
	S3CON &= 0xBF;		//串口3选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0xE0;			//设置定时初始值
	T2H = 0xFE;			//设置定时初始值
	AUXR |= 0x10;		//定时器2开始计时

    ES3  = 1;            //允许中断

    S3REN = 1;            //允许接收
    
    //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 
    //                 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4

    S3_S = 0x00;
    
    B_TX3_Busy = 0;
    TX3_Cnt = 0;
    Rx3_Timer  = 0;
    RX3_Cnt = 0;
}

void UART3_ISR (void) interrupt 17
{
    if(S3RI)
    {
        if(SysRunState.isSleep == 1)
    	{
      		SysRunState.isSleep = 0;
    		SysRunState.NoUartTime = 0;
    		//LPM3_EXIT;
            BLUE_WAKEUP();
    	}
    
        S3RI = 0;
        Rx3_Timer = 0;
        RX3_Buffer[RX3_Cnt] = S3BUF;
        if(++RX3_Cnt >= MAX_LENGTH)   
        {
            RX3_Cnt = 0;
        }
    }

    if(S3TI)
    {
        S3TI = 0;
        B_TX3_Busy = 0;
    }
}



void uart_send(u8 *buf, u8 len)
{
    u8 i;
    Uart485_EN(0);
    for (i=0;i<len;i++)     
    {
        S3BUF = buf[i];
        B_TX3_Busy = 1;
        while(B_TX3_Busy);
    }
    Uart485_EN(1);
}

void ClearRs485Buf()
{
    memset(RX3_Buffer,0,MAX_LENGTH);
    RX3_Cnt = 0;
}


