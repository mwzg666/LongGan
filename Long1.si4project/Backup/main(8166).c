#include "Sensor.h"
#include "mcp4725.h"
#include "system.h"
#include "main.h"
#include "CMD.h"
#include "uart.h"
#include "i2c.h"
#include "flash.h"
#include "CalcDoseRate.h"
#include "DoseRate.h"

LP_SYSTEM_STTAE SysRunState={0};

extern void Adc_Init();
extern uint16_t DeviceGetBatVal(void);
extern void DeviceGetBatAlarm(STU_BATTERY *bat);
extern STU_BATTERY s_Bat;

//unsigned int ADC16Result = 0; 

//========================================================================
// ��������: void InitParam()
// ��������: ��ʼ����������
// ��ڲ���: @��
// ��������: ��
// ��ǰ�汾: VER1.0
// �޸�����: 2023.5.5
// ��ǰ����:
// ������ע: 
//========================================================================

void InitParam(LP_PARAM *param)
{
	memset((void*)param,0,sizeof(LP_PARAM));
	param->SensorType = SENSOR_LONGPOLE;//̽ͷ����
	param->s_SysParam.Hv = 800;      //��ѹֵ
	param->s_SysParam.Z1 = 2500-600;//�������ֵ1
	param->s_SysParam.Ct = 1000;    //����ʱ��
	param->s_SysParam.Hd = 3;        //��ѹ���
	param->s_SysParam.Z2 = 1100;    //�������ֵ2
	param->s_SysParam.Hn = 0x5a;

	param->s_SysParam.DiYaCanshuA = 0.63;//������ͨ��У׼����
	param->s_SysParam.DiYaCanshuB = 0.00019;//������ͨ��У׼����
	param->s_SysParam.DiYaCanshuC = 0.83; //1;//������ͨ��У׼����
	param->s_SysParam.GaoYaCanshuA = 33.6;//������ͨ��У׼����
	param->s_SysParam.GaoYaCanshuB = 0.000023;//������ͨ��У׼����
	param->s_SysParam.GaoYaCanshuC = 0.83; // ;//������ͨ��У׼����
	
	param->s_Alarm.DosePreAlarm = 300;//300uSv
	param->s_Alarm.DoseAlarm = 400;//400uSv
	param->s_Alarm.DoseRatePreAlarm = 300;//300uSv/h
	param->s_Alarm.DoseRateAlarm = 400;//400uSv/h

	WritePara();
}

void DevInit(void)
{
    BLUE_PWOFF();
  	GM_LOW;
	BLUE_CFG_MODE();
	BLUE_WAKEUP();
}
void sysSleep(void)
{
  	SysRunState.isSleep = 1;
	//LPM3;
}

void DevSleep(void)
{
  	SENSOR_POWER_OFF();//��ʱ�Ȳ��ظ�ѹ
	BLUE_CFG_MODE();
	delay_ms(2500);
	uart_send("AT+CLEARADDR\r\n",14);//���Զ��������ַ
	delay_ms(500);
	BLUE_WORK_MODE();
	BLUE_SLEEP();
	T3R = 0;
	sysSleep();
	SensorMeasureBegin();//��ʼ���� 
	T3R = 1;
	SENSOR_POWER_ON();
}


void BtInit()
{
    BLUE_WORK_MODE();
    BLUE_SLEEP();
    BLUE_PWOFF();
    delay_ms(500);
    
    BLUE_PWON();
    BLUE_WAKEUP();
    BLUE_CFG_MODE();

            
	uart_send("AT+ROLE=0\r\n",11);//��ģʽ
	delay_ms(500);
	uart_send("AT+LOWPOWER=1\r\n",15);//����͹���
	delay_ms(500);
	uart_send("AT+TXPOWER=7\r\n",14);//���书��0~7
	delay_ms(500);
    
	uart_send("AT+CLEARADDR\r\n",14);//���Զ��������ַ
	delay_ms(500);
	uart_send("AT+AUTH=1\r\n",11);//���ü�Ȩ
	delay_ms(500);
	uart_send("AT+BIND=1\r\n",11);//���ð󶨵�ַ
	delay_ms(500);
    
	/*MSP430XX_UART_Send("AT+RADDR?\r\n",11);//
	delay_ms(500);
	MSP430XX_UART_Send("AT+BLECONNPARAM?\r\n",18);
	delay_ms(1000);
	MSP430XX_UART_Send("AT+DFU\r\n",8);
	delay_ms(1000);*/

    
    BLUE_WORK_MODE();
}


//========================================================================
// ��������: void delay_ms(WORD ms)  
// ��������: ������ʱ����
// ��ڲ���: @WORD ms����ʱ���ٺ���
// ��������: ��
// ��ǰ�汾: VER1.0
// �޸�����: 2023.5.5
// ��ǰ����:
// ������ע: 
//========================================================================
void delay_ms(WORD ms)              
{
    WORD t = 1000;
    while(ms--)
    {
        for (t=0;t<1000;t++) ;
    }
}


void delay_us(BYTE us)
{
    while(us--)
    {
        ;
    }
}

//========================================================================
// ��������: void IoInit()
// ��������: ��Ƭ��I/O�ڳ�ʼ��
// ��ڲ���: @��
// ��������: ��
// ��ǰ�汾: VER1.0
// �޸�����: 2023.5.5
// ��ǰ����:
// ������ע: 
//========================================================================
void IoInit()
{
    EAXFR = 1;
    WTST = 0;                       //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���

    P0M1 = 0x00;   P0M0 = 0x00;     //����Ϊ׼˫���
    P1M1 = 0x00;   P1M0 = 0x00;     //����Ϊ׼˫���
    P2M1 = 0x02;   P2M0 = 0x00;     //����Ϊ׼˫���
    P3M1 = 0x00;   P3M0 = 0x00;     //P3.3����Ϊ�������
    P4M1 = 0x00;   P4M0 = 0x00;     //����Ϊ׼˫���
    P5M1 = 0x00;   P5M0 = 0x00;     //����Ϊ׼˫���
    P6M1 = 0x00;   P6M0 = 0x00;     //����Ϊ׼˫���
    P7M1 = 0x00;   P7M0 = 0x00;     //����Ϊ׼˫���
}

//========================================================================
// ��������: void TimerTask()
// ��������: ��ʱ����ͨ����ʱ��0��ʱ10ms�������������
// ��ڲ���: @��
// ��������: ��
// ��ǰ�汾: VER1.0
// �޸�����: 2023.5.5
// ��ǰ����:
// ������ע: 
//========================================================================
void TimerTask()
{
    u16 delta = 0;
    static u16 Time1s = 0;
    if(Timer0Cnt)
    {
        delta = Timer0Cnt * 10;
        Timer0Cnt = 0;
        if(RX1_Cnt>0)
        {
            Rx1_Timer += delta;
        }
        if(RX3_Cnt>0)
        {
            Rx3_Timer += delta;
        }
        Time1s += delta;
        if(Time1s >= 1000)                      //100*10=1000ms
        {
            Time1s = 0;
            SysRunState.isCanReadSensor = 1;

            PluseCnt = 100;//((u32)LowOverFlowFlag*65536)<<8 | (u32)(T3H*256+T3L) ;
            T3R = 0; 
            T3H = 0;
            T3L = 0;
            T3R = 1;            

            PlusePortCnt = 0;
            LowOverFlowFlag = 0;
        }
    }
    if(SysRunState.NoUartTime > POWER_OFF_TIME)
    {
        DevSleep();
    }
    else
    {
        sysSleep();
    }
}

//========================================================================
// ��������: void Rs485Hnd()
// ��������: ͨ��RS485����λ������
// ��ڲ���: @��
// ��������: ��
// ��ǰ�汾: VER1.0
// �޸�����: 2023.5.5
// ��ǰ����:
// ������ע: 
//========================================================================
void Rs485Hnd()
{
    if(Rx3_Timer > 20)                  //���ڳ�ʱ20ms
    {
        Rx3_Timer = 0;
        DataPro(RX3_Buffer,RX3_Cnt);
        ClearRs485Buf();
    }
}


void Error()
{
    while(1)
    {
        
        //RUN_LED(1);
        delay_ms(200);
        //RUN_LED(0);
        delay_ms(200);
    }
}


int main(void)
{    
    SysInit();

    IoInit();
    //I2C_Init();
    Adc_Init();
	DevInit();
    
	delay_ms(2000);
        
	Uart1_Init();
    Uart3_Init();
    ClearRs485Buf();
    SensorInit();    
	Timer0_Init();
	Timer3_Init();
	
	GetPara(&SysRunState.stParam);
    
	EA = 1;
    
	//�����ȼ��һ�ε�ص���
    DeviceGetBatVal();

    BtInit();
    DeviceGetBatVal();

	BLUE_SLEEP();
	DeviceGetBatAlarm(&s_Bat);//�����ȼ��һ�ε�ص���
	SensorMeasureBegin();//��ʼ���� 
	InitArr();
    
    MCP4725_OutVol(MCP4725_S1_ADDR,2500-(WORD)SysRunState.stParam.s_SysParam.Z1);
	
    while(1)
    {              
        if(SysRunState.isCanReadSensor == 1)
        {
            CaptureSensorPluseCounter(); //����ǰ�������
            SensorMeasureBegin();         //��ʼ���� 
            SysRunState.isCanReadSensor = 0;
        }

        if((SysRunState.NoUartTime >= 90*100)&&(SysRunState.NoUartTime < 93*100))
        {
            //��ʱ����ͨ�����ݳ��Ը�λ����ģ��
            BtInit();
            //revFlag = 0;
            RX1_Cnt = 0;
            SensorMeasureBegin();//��ʼ���� 
            SysRunState.NoUartTime = 93*100;
        }

        Rs485Hnd();
        TimerTask();
        
    }
}




