#include "uart.h"
#include "system.h"
#include <stdio.h>
#include <string.h>

u8  TX1_Cnt;            //���ͼ���
u8  RX1_Cnt;            //���ռ���
bit B_TX1_Busy;         //����æ��־
u16 Rx1_Timer  = 0;

u8  TX3_Cnt;            //���ͼ���
u8  RX3_Cnt;            //���ռ���
bit B_TX3_Busy;         //����æ��־
u16 Rx3_Timer  = 0;


u8  RX1_Buffer[MAX_LENGTH]; //���ջ���
u8  RX3_Buffer[MAX_LENGTH]; //���ջ���

//========================================================================
// ��������: void Uart1_Init(void)
// ��������: UART0��ʼ��,9600λ������/��,8λ�ַ�,1λֹͣλ,��У��.
// ��ڲ���: @��
// ��������: ��
// ��ǰ�汾: VER1.0
// �޸�����: 2023.5.5
// ��ǰ����:
// ������ע: ������9600,ʱ��Դѡ��ΪACLK.�����ʿ��ܻ��Щ.
//           �û���ѡ��������Ƶʱ��Դ.��Ƶʱ�ӻ�ʹ�����ʵ������ʽ���.
//========================================================================
void Uart1_Init(void)		//115200bps@11.0592MHz
{  
    SCON = 0x40;		//8λ����,�ɱ䲨����
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0xE0;			//���ö�ʱ��ʼֵ
	TH1 = 0xFE;			//���ö�ʱ��ʼֵ
	ET1 = 0;			//��ֹ��ʱ���ж�
	TR1 = 1;			//��ʱ��1��ʼ��ʱ   
    ES  = 1;            //�����ж�
    PS  = 0;            //�жϸ����ȼ�
    PSH = 0;

    REN = 1;            //�������
    
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

//�ض���Printf
char putchar(char c )
{
    SBUF = c;
    while(!TI);
    TI = 0;
    return c;
}
//========================================================================
// ��������: void uart_send(u8 *buf, u8 len)
// ��������: ���ڷ��ͺ���
// ��ڲ���: @*buf�����͵����ݣ�@len�����ݳ���
// ��������: ��
// ��ǰ�汾: VER1.0
// �޸�����: 2023.5.5
// ��ǰ����:
// ������ע: 
//========================================================================


void Uart3_Init(void)		//9600bps@11.0592MHz
{
	S3CON = 0x10;		//8λ����,�ɱ䲨����
	S3CON &= 0xBF;		//����3ѡ��ʱ��2Ϊ�����ʷ�����
	AUXR |= 0x04;		//��ʱ��ʱ��1Tģʽ
	T2L = 0xE0;			//���ö�ʱ��ʼֵ
	T2H = 0xFE;			//���ö�ʱ��ʼֵ
	AUXR |= 0x10;		//��ʱ��2��ʼ��ʱ

    ES3  = 1;            //�����ж�

    S3REN = 1;            //�������
    
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


