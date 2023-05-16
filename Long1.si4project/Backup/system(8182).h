#ifndef SYSTEM_H
#define SYSTEM_H

#ifdef __cplusplus
extern  "C" {
#endif
   
//===============================变量定义=============================




//===============================结构体定义===========================




//===============================变量声明=============================
extern u16 Timer0Cnt;
extern u32 PluseCnt;
extern u8  LowOverFlowFlag;    
extern u8  PlusePortCnt;

    
//===============================函数声明=============================   
void SysInit();
void Timer0_Init(void);
void Timer3_Init(void);
void SetVcoreUp (unsigned int level);
void Timer1_Init(void);
u32 GetCounter(void);
    
#ifdef __cplusplus
}
#endif

#endif
