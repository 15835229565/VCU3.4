#include "includes.h"


void TimeoutInit(uint32* timestamp)
{
    *timestamp = GetTime();
}
uint32 GetTime()
{
    return Timer0Counter;
}

uint8 TimeoutValidate(uint32 timestamp,uint32 timeout)
{
    uint32 current = GetTime();
    FEEDWATCHDOG = ~FEEDWATCHDOG;
    if(timestamp<=current)
    {
        if((current-timestamp)>=timeout)
        {
            return TRUE;
        }
    }
    else
    {
        if((0xFFFFFFFF-timestamp+current)>=timeout)
        {
            return TRUE;
        }
    }
    return FALSE;
}
void OutputCompare_Init(void)
{
    TSCR1_TEN = 0;      /* Disable Timer module before adjusting registers.    */    
    TIOS_IOS0 = 1;      /* Set Channel 0 as output compare.                    */
    TIOS_IOS3 = 1;    
    TSCR2_TOI = 1;
    TCTL2_OM3 = 0;      /* Set channel 0 to toggle when a Timer match occurs.  */
    TCTL2_OL3 = 1;      /* Set channel 0 to toggle when a Timer match occurs.  */
    TCTL2_OM0 = 0;      /* Set channel 0 to toggle when a Timer match occurs.  */
    TCTL2_OL0 = 0;      /* Set channel 0 to toggle when a Timer match occurs.  */    
    TC0 = TCNT+Timer0;      //10ms电机控制    
    TC3 = TCNT+Timer3;  //10ms   AD采样     
    TTOV_TOV0=1;
    TTOV_TOV3=1;
    
    TIE_C0I= 1;      /* Enable channel 0 interrupt, handled by function TIM0ISR. */    
    TIE_C3I= 1;      /* Enable channel 0 interrupt, handled by function TIM0ISR. */
   
    TSCR1_TSWAI = 1;    /* Disables the timer module while in wait mode.    */
    TSCR1_TSFRZ = 1;    /* Disables the timer counter while in freeze mode. */
    TSCR2_PR    = 0x5;  /* Set prescaler to divide by 32                   */
    TSCR2_TCRE  = 1;
    TSCR1_TEN   = 1;    /* Timer Enable.                                    */
    
   
    //中断周期：0x4926*128/24MHz = 100ms

}

/*
*********************************************************************************************************
*                                           芯片初始化
* Description: RTI_Init；用RTI实时中断设置1ms的时间节拍
*
* Arguments  : none
*
* Note(s)    :
*********************************************************************************************************
*/

void RTI_Init(void)
{
    CPMUCLKS_RTIOSCSEL = 0;    //使用外部晶振
    CPMURTI = 0xEF;            //设定100ms的定时中断 11000100，第8位表示进制，0:2进制，1:10进制;
    CPMUINT_RTIE = 0;         //打开RTI的中断
}

/*
*********************************************************************************************************
*                                           
* Description: SpeedSensorCaptureInit  重新初始化车速传感器捕捉方式
*
* Arguments  : none
*
* Note(s)    :
*********************************************************************************************************
*/
void SpeedSensorCaptureInit() 
{
   gMotorInfor.tripPulseCounter = 0;
   TCTL3_EDG4A = 1;                         //设置输入捕捉方式的捕捉方式
   TCTL3_EDG4B = 0;
   gMotorInfor.pulseCaptureMode  = 1;
   memset(gMotorInfor.pulseData,0,16);
   gMotorInfor.pulseDataIndex = 0;
   gMotorInfor.preTCNT1 = 0;
   gMotorInfor.preTCNT2 = 0;
}

/*
*********************************************************************************************************
*                                           芯片初始化
* Description: Init_PT_ICapture；设置投币信号与脉冲检测中断
*
* Arguments  : none
*
* Note(s)    :
*********************************************************************************************************
*/
void Init_PT_ICapture(void)
{
    TIOS_IOS4 = 0;                               //设定通道0为输入捕捉工作方式
    TIOS_IOS5 = 0;

    TFLG1 = 0xFF;                                //清除中断标志位
    TCTL3_EDG4A = 1;                         //设置输入捕捉方式的捕捉方式
    TCTL3_EDG4B = 0;
    gMotorInfor.pulseCaptureMode  = 1;
    TCTL3_EDG5A = 1;                        //设置输入捕捉方式的捕捉方式
    TCTL3_EDG5B = 0;
    TIE_C4I = 1;                                  //定时器中断使能
    TIE_C5I = 1;                                  //定时器中断使能
}

void delayms(uint32 time)
{
    uint32 i,j;
    for(i=0; i<time; i++)
    {
        FEEDWATCHDOG = ~FEEDWATCHDOG;
        delayus(1000);
        _FEED_COP();  /* feeds the dog */
    }
}

void delayus(uint32 time)
{
    uint32 i,j;
    for(i=0; i<(time/6); i++)
    {
      FEEDWATCHDOG = ~FEEDWATCHDOG;
    }
}


