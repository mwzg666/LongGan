#ifndef UART_H
#define UART_H
#include "main.h"

//===============================变量定义=============================
#define Uart_BaudRate       9600
#define Baudrate            38400UL
#define MAX_LENGTH          200

#define SYSTEM_FREQ         25000000

#define UART_MAX_BAUDRATE   115200    //定义串口最大和最小波特率
#define UART_MIN_BAUDRATE   1200

#define UART_USB_BAUDRATE   9600      //配置串口波特率


//===============================结构体定义===========================



//===============================变量声明=============================
extern u8  TX1_Cnt;                 //发送计数
extern u8  RX1_Cnt;                 //接收计数
extern bit B_TX1_Busy;              //发送忙标志
extern u16 Rx1_Timer;
extern u8  RX1_Buffer[MAX_LENGTH];  //接收缓冲

extern u8  TX3_Cnt;                 //发送计数
extern u8  RX3_Cnt;                 //接收计数
extern bit B_TX3_Busy;              //发送忙标志
extern u16 Rx3_Timer;
extern u8  RX3_Buffer[MAX_LENGTH];  //接收缓冲


//===============================函数声明=============================
void Uart1_Init(void);
void InitUart();
void Uart3_Init(void);
void uart_send(u8 *buf, u8 len);
void ClearRs485Buf();

#endif