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
    TC0 = TCNT+Timer0;      //10ms�������    
    TC3 = TCNT+Timer3;  //10ms   AD����     
    TTOV_TOV0=1;
    TTOV_TOV3=1;
    
    TIE_C0I= 1;      /* Enable channel 0 interrupt, handled by function TIM0ISR. */    
    TIE_C3I= 1;      /* Enable channel 0 interrupt, handled by function TIM0ISR. */
   
    TSCR1_TSWAI = 1;    /* Disables the timer module while in wait mode.    */
    TSCR1_TSFRZ = 1;    /* Disables the timer counter while in freeze mode. */
    TSCR2_PR    = 0x5;  /* Set prescaler to divide by 32                   */
    TSCR2_TCRE  = 1;
    TSCR1_TEN   = 1;    /* Timer Enable.                                    */
    
   
    //�ж����ڣ�0x4926*128/24MHz = 100ms

}

/*
*********************************************************************************************************
*                                           оƬ��ʼ��
* Description: RTI_Init����RTIʵʱ�ж�����1ms��ʱ�����
*
* Arguments  : none
*
* Note(s)    :
*********************************************************************************************************
*/

void RTI_Init(void)
{
    CPMUCLKS_RTIOSCSEL = 0;    //ʹ���ⲿ����
    CPMURTI = 0xEF;            //�趨100ms�Ķ�ʱ�ж� 11000100����8λ��ʾ���ƣ�0:2���ƣ�1:10����;
    CPMUINT_RTIE = 0;         //��RTI���ж�
}

/*
*********************************************************************************************************
*                                           
* Description: SpeedSensorCaptureInit  ���³�ʼ�����ٴ�������׽��ʽ
*
* Arguments  : none
*
* Note(s)    :
*********************************************************************************************************
*/
void SpeedSensorCaptureInit() 
{
   gMotorInfor.tripPulseCounter = 0;
   TCTL3_EDG4A = 1;                         //�������벶׽��ʽ�Ĳ�׽��ʽ
   TCTL3_EDG4B = 0;
   gMotorInfor.pulseCaptureMode  = 1;
   memset(gMotorInfor.pulseData,0,16);
   gMotorInfor.pulseDataIndex = 0;
   gMotorInfor.preTCNT1 = 0;
   gMotorInfor.preTCNT2 = 0;
}

/*
*********************************************************************************************************
*                                           оƬ��ʼ��
* Description: Init_PT_ICapture������Ͷ���ź����������ж�
*
* Arguments  : none
*
* Note(s)    :
*********************************************************************************************************
*/
void Init_PT_ICapture(void)
{
    TIOS_IOS4 = 0;                               //�趨ͨ��0Ϊ���벶׽������ʽ
    TIOS_IOS5 = 0;

    TFLG1 = 0xFF;                                //����жϱ�־λ
    TCTL3_EDG4A = 1;                         //�������벶׽��ʽ�Ĳ�׽��ʽ
    TCTL3_EDG4B = 0;
    gMotorInfor.pulseCaptureMode  = 1;
    TCTL3_EDG5A = 1;                        //�������벶׽��ʽ�Ĳ�׽��ʽ
    TCTL3_EDG5B = 0;
    TIE_C4I = 1;                                  //��ʱ���ж�ʹ��
    TIE_C5I = 1;                                  //��ʱ���ж�ʹ��
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


