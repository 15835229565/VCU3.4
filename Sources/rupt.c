#include "includes.h"
#define ABS(x,y)   (x)<(y)?((y)-(x)):((x)-(y))


/**********************串口中断相关变量*************************/

uint8 usart0word;
uint8 usart1word;
uint8 usart2word;
uint8 ServerCommand_flag = WAIT_CMD;
uint8 PhoneCommand_flag = WAIT_CMD;

uint8 ServerCommand[SERVERCMD_BUFFER_SIZE];
uint8 PhoneCommand[PHONECMD_BUFFER_SIZE];

uint8 GSM_MSG_Buf[GSM_BUFFER_SIZE];      //接收到的GSM消息
uint8 GSM_buffer_p = 0;
uint8 GPRSRecvIndex = 0;                   //单次接收到的GPRS数据长度

uint8 GPS_message_flag = WAIT_CMD;
uint8 GPS_BUFFER[GPS_BUFFER_SIZE]=0;
uint8 GPSRecvIndex = 0;                   //单次接收到的GPS定位信息




/**********************定时器中断相关变量*************************/

uint16 gGPRSConnectTimer = 0;  //gprs网络连接定时器
uint16 gGprsInitialTimer = 0;  //gprs网络初始化定时器
uint16 gGPSLocationTimer = 0;  //GPS定位定时器
uint16 TimerOverflowCounter=0;

/**********************定时器0*************************/

uint8  coin_plus_trig = false;                        //投币脉冲检测


/**********************定时器1*************************/
uint16 TimerA_Counter = 0;
uint16 Timer0Counter=0;//基本定时器计数

uint8 freq_2s = 0;
uint8 freq_5s = 0;
uint8 freq_15s = 0;
uint8 freq_60s = 0;

uint8 rent_freq_flag = 0;
uint8 standby_freq_flag = 0;
/**********************定时器4*************************/


uint16 Motorstep_per_sec = 0;                                   //计算每秒脉冲数  
uint16 Motorstep_per_sec_count =0;


/**********************定时器6*************************/
uint16  MatrixAD[11] = 0;
                              
#pragma CODE_SEG __NEAR_SEG NON_BANKED


void interrupt  SCI2_ISR(void) 
{      
    
    static uint8 aliasFlag1=0,rxIndex1=0;
    uint8 i;    
   
    SCI2SR1_RDRF = 0;
    usart2word = SCI2DRL;
    
     //处理来自服务器的数据包    
    if (aliasFlag1)
    {
        PhoneCommand[rxIndex1++] = usart2word;
        aliasFlag1 = 0;
    }
    else
    {
        switch (usart2word)
        {
         case CMD_START:                                        
                rxIndex1 = 0; 
                PhoneCommand_flag = RECV_CMD;                       
                break;
         case CMD_END:
                if(GetCRC(PhoneCommand,rxIndex1-1)==PhoneCommand[rxIndex1-1]) 
                {                      
                  
                  PhoneCommand_flag = PROC_CMD;
                  //调试模式用       
                  #if DEBUG_MODE==1 
                  for(i=0;i<rxIndex1;i++) 
                  {                    
                    SendByte0(PhoneCommand[i]);
                  }
                  SendByte0(0x0D);
                  SendByte0(0x0A);
                  #endif
                } 
                else 
                {
                  PhoneCommand_flag = WAIT_CMD;
                                   
                  
                  rxIndex1 = 0;
                }
                break;
         case CMD_ALIAS:
                aliasFlag1 = 1;
                break;
         default:
                PhoneCommand[rxIndex1++] = usart2word;
                break;
        }
    }
    
    if(rxIndex1 >= PHONECMD_BUFFER_SIZE) 
    {
       rxIndex1 = 0;
       PhoneCommand_flag = WAIT_CMD;
    } 
   
}
/*
*********************************************************************************************************
*                                                SCI1_ISR()
*
* Description : GPRS接口中断，串口接收处理数据
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/
void interrupt  SCI1_ISR(void)
{

    static uint8 aliasFlag=0,rxIndex=0,error=0;    
  
    SCI1SR1_RDRF=0;    
    usart1word = SCI1DRL;
    if(GSM_buffer_p >= GSM_BUFFER_SIZE)
    {
        GSM_buffer_p=0;
    }
    //调试模式用
    #if DEBUG_MODE==1 
    SendByte0(usart1word);
    #endif
    GSM_MSG_Buf[GSM_buffer_p]=usart1word;
    
    

     //处理来自服务器的数据包 
    if(ServerCommand_flag != PROC_CMD) 
    {          
      if (aliasFlag)
      {
          ServerCommand[rxIndex++] = usart1word;
          aliasFlag = 0;
      }
      else
      {
          switch (usart1word)
          {
           case CMD_START:                                        
                  rxIndex = 0; 
                  ServerCommand_flag = RECV_CMD;                       
                  break;
           case CMD_END:
                  if(GetCRC(ServerCommand,rxIndex-1)==ServerCommand[rxIndex-1]) 
                  {                      
                    DataEncryption(ServerCommand,rxIndex-1);
                    ServerCommand_flag = PROC_CMD;
                  } 
                  else 
                  {
                    ServerCommand_flag = WAIT_CMD;
                    rxIndex = 0;
                  }
                  break;
           case CMD_ALIAS:
                  aliasFlag = 1;
                  break;
           default:
                  ServerCommand[rxIndex++] = usart1word;
                  break;
          }
      }
    
    
      if(rxIndex >= SERVERCMD_BUFFER_SIZE) 
      {
         rxIndex = 0;
         ServerCommand_flag = WAIT_CMD;
      }
    }
    
    if(ServerCommand_flag!= RECV_CMD) 
    {      
      MG301_PowerOn();
    }

    GSM_buffer_p++;
    
}

/*
*********************************************************************************************************
*                                                SCI0_ISR()
*
* Description : 调试接口，电池管理
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/
void interrupt  SCI0_ISR(void)
{
    
    uint8 i,j;
    uint16 msg_p = 0;
    
    uint8 temp[8];
    uint16 lLatitude1,lLongitude1;
    uint32 lLatitude2,lLongitude2; 
    FP32  lat1,lon1,lat2,lon2;
    static FP32 latsave=0,lonsave=0;
    
    SCI0SR1_RDRF = 0;
    usart0word = SCI0DRL; 
    if(GPSRecvIndex>=GPS_BUFFER_SIZE||usart0word=='$')
    {
        GPSRecvIndex=0;
    }
    GPS_BUFFER[GPSRecvIndex++]=usart0word;   
    if(GPSRecvIndex>=49)
    {
      
      //获取卫星个数信息
      if(GPS_BUFFER[1]=='G' && GPS_BUFFER[2]=='P' && GPS_BUFFER[3]=='G' && GPS_BUFFER[4]=='S' && GPS_BUFFER[5]=='V'&& GPS_BUFFER[6]==',') 
      {
        j = 0;
        for(i=7;i<=40;i++) 
        {
          if(GPS_BUFFER[i]==',') 
          {
            j++;
            if(j==2) 
            {
              break;
            }
          }
        }   
        if(j==2 && GPS_BUFFER[i+1]>='0' && GPS_BUFFER[i+1]<='9' && GPS_BUFFER[i+2]>='0' && GPS_BUFFER[i+2]<='9')        
        {
         gCarInfor.SatelliteNum = 10*(GPS_BUFFER[i+1]-'0') + (GPS_BUFFER[i+2]-'0');
        }    
        GPSRecvIndex=0;
      } 
      //获取定位信息
      else if(GPS_BUFFER[1]=='G' && GPS_BUFFER[2]=='P' && GPS_BUFFER[3]=='G' && GPS_BUFFER[4]=='L' && GPS_BUFFER[5]=='L'&& GPS_BUFFER[6]==',') 
      {
         
         j = 0;
         //AlarmTwoVoice(NUMBER_2_VOICE);
         for(i=7;i<=50;i++) 
         {
            if(GPS_BUFFER[i]==',') 
            {
              j++;
              if(j==5) 
              {
                break;
              }
            }
         }
       //  AlarmTwoVoice(BT_TEST_VOICE);
         if(j==5 && GPS_BUFFER[i+1]=='A') 
         {
             
          
              msg_p=7;
              
              lLatitude1 = (GPS_BUFFER[msg_p]-48)*10+(GPS_BUFFER[msg_p+1]-48);
              
              temp[0] =  GPS_BUFFER[msg_p+2]-48;
              temp[1] =  GPS_BUFFER[msg_p+3]-48;
              temp[2] =  GPS_BUFFER[msg_p+5]-48;
              temp[3] =  GPS_BUFFER[msg_p+6]-48;
              temp[4] =  GPS_BUFFER[msg_p+7]-48;
              temp[5] =  GPS_BUFFER[msg_p+8]-48;
              temp[6] =  GPS_BUFFER[msg_p+9]-48;
              temp[7] = 0;
              
              lLatitude2 = 0;
              for(i=0; i<8; i++)
              {
                  lLatitude2 = lLatitude2*10 + temp[i];
              }
              
              msg_p = msg_p+13;
              lLongitude1 = (GPS_BUFFER[msg_p]-48)*100+(GPS_BUFFER[msg_p+1]-48)*10+(GPS_BUFFER[msg_p+2]-48);
    
             
              temp[0] =  GPS_BUFFER[msg_p+3]-48;
              temp[1] =  GPS_BUFFER[msg_p+4]-48;
              temp[2] =  GPS_BUFFER[msg_p+6]-48;
              temp[3] =  GPS_BUFFER[msg_p+7]-48;
              temp[4] =  GPS_BUFFER[msg_p+8]-48;
              temp[5] =  GPS_BUFFER[msg_p+9]-48;
              temp[6] =  GPS_BUFFER[msg_p+10]-48;
              temp[7] = 0;
              lLongitude2 = 0;
              for(i=0; i<8; i++)
              {
                  lLongitude2 = lLongitude2*10 + temp[i];
              } 
              
              gCarInfor.lLatitude1 = lLatitude1;
              gCarInfor.lLatitude2 = lLatitude2;
              gCarInfor.lLongitude1 = lLongitude1;
              gCarInfor.lLongitude2 = lLongitude2;
              GPSRecvIndex=0;
              GPS_init_flag = GPS_SUCC; 
              gGPSLocationTimer = 0;   
                                                                  
         } 
         else
         {
              GPS_init_flag = GPS_RESTART;
              GPSRecvIndex=0; 
         }
      }
    }
   
}

#pragma CODE_SEG DEFAULT

#pragma CODE_SEG __NEAR_SEG NON_BANKED

void interrupt   ECT_TimerOverflow_ISR(void)
{
    TimerOverflowCounter++;   
    TFLG2 = TFLG2_TOF_MASK; //clear?timer?overflow?interrupt?flag??Main Timer Interrupt Flag
    
}

/*
*********************************************************************************************************
*                                               RTI_ISR()
*
* Description : 作为系统时钟，目前处理行驶中的可用时间 ，中断执行频率100ms
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/
void interrupt  RTI_ISR(void) 
{
       CPMUFLG_RTIF = 1; 
}


/*
*********************************************************************************************************
*                                               TIM0_ISR()
*
* Description : 电机控制，定时器定时处理，中断执行频率5ms
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/
void interrupt  TIM0_ISR(void)
{
    TC0 = TCNT+Timer0;
    TFLG1 = TFLG1_C0F_MASK; //these flags are set when an IOC occurs. (modify by Ora) 
    EnableInterrupts;

    if( VCUWorkMode==TEST_MODE) 
    {      
       return;
    }
    CoinSignalProcess();
    
    #if DEBUG_MODE==0   
    //电磁刹控制程序 每5ms判断一次超声波雷达
    if(VCUWorkMode==NORMAL_MODE) 
    {          
      MotorElecMagnetControl();
    }
    
    
    //车辆运行控制
    CarRunningCtrl();
   
    #endif    

    TFLG1 = TFLG1_C0F_MASK; 
}
/*
*********************************************************************************************************
*                                               TIM4_ISR()
*
* Description : 电机脉冲输入捕获，用于检测脉冲个数，判断行驶里程，
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/
uint16 TCNTdiff1=0;
uint16 TCNTdiff2=0;

void interrupt TIM4_ISR(void)
{

     
     static uint16 currTCNT1=0;
     static uint16 currTCNT2=0;
     uint32 mileageSum=0;
     uint8 i;
     uint16 max,min,num;
     
     TFLG1_C4F = 1;  //清中断标志位
   
     if(gMotorInfor.pulseCaptureMode==1)//当前为上升沿捕捉 TCNTdiff1为下降沿到上升沿的脉冲宽度 
     {  
        gMotorInfor.preTCNT2= TCNT;    
        TCTL3_EDG4A = 0;                         //设置下降沿捕捉
        TCTL3_EDG4B = 1;
        gMotorInfor.pulseCaptureMode = 0;
        
        
        if(gMotorInfor.preTCNT1==0) 
        {
          return;
        }
        currTCNT1= TCNT;
        if(currTCNT1>=gMotorInfor.preTCNT1) 
         {      
            TCNTdiff1 = currTCNT1 - gMotorInfor.preTCNT1+TimerOverflowCounter*65535;
         } 
         else
         {
            TCNTdiff1 = TimerOverflowCounter*65535 - gMotorInfor.preTCNT1+ currTCNT1;            
         }
         TimerOverflowCounter = 0;
         
         
         gMotorInfor.pulseData[gMotorInfor.pulseDataIndex++]=TCNTdiff1;
        
          
          if(TCNTdiff1>1500) 
          {            
            
             Motorstep_per_sec++;   //记录脉冲数  ndy为了判断车辆是否已经停止实时性要求较高不可用于计算里程
          //位置更换0611
          }
         
     } 
     else   //下降沿到来时，记录TCNT  TCNTdiff2为上升沿到下降沿的脉冲宽度 
     {
        
        gMotorInfor.preTCNT1 = TCNT;
        TCTL3_EDG4A = 1;                         //设置上升沿捕捉
        TCTL3_EDG4B = 0;
        gMotorInfor.pulseCaptureMode  = 1;
        
        if(gMotorInfor.preTCNT2==0) 
        {
          return;
        }
        currTCNT2 = TCNT;
        if(currTCNT2>=gMotorInfor.preTCNT2) 
         {      
            TCNTdiff2 = currTCNT2 - gMotorInfor.preTCNT2+TimerOverflowCounter*65535;
         } 
         else
         {
            TCNTdiff2 = TimerOverflowCounter*65535 - gMotorInfor.preTCNT2+ currTCNT2;            
         }     
         TimerOverflowCounter = 0; 
                         
         gMotorInfor.pulseData[gMotorInfor.pulseDataIndex++]=TCNTdiff2;
         if(TCNTdiff2>1500) 
         {                        
             Motorstep_per_sec++;   //记录脉冲数  ndy为了判断车辆是否已经停止实时性要求较高不可用于计算里程
          //位置更换0611
         }
     }
     
     
     
     
     if(gMotorInfor.pulseDataIndex==16)
     {
       //获取当前数组的最大值
       max = gMotorInfor.pulseData[0];
       min = max; 
       for(i=0;i<16;i++) 
       {
         if(gMotorInfor.pulseData[i]>max) 
         {
           max = gMotorInfor.pulseData[i];
         } 
         if(gMotorInfor.pulseData[i]<min) 
         {
           min = gMotorInfor.pulseData[i];
         }      
       }
       //获取当前数组中距离最大值和最小值中间值较近(大于四分之一)数的个数
       //如果最大值与最小值只差500，说明数组内波形基本一致
       if(max<(min+500)) 
       {
        num = 16;
       } 
       else
       {        
         num=0;
         for(i=0;i<16;i++) 
         {          
           if(gMotorInfor.pulseData[i]>(max>>2)) 
           {
             num++;
           }
         }
       } 
       
       gMotorInfor.tripPulseCounter += (num>>1);
       //Motorstep_per_sec+=(num>>1);   //记录脉冲数 
       
       gMotorInfor.pulseDataIndex = 0; 
     }
     
    if( VCUWorkMode==TEST_MODE) 
    {      
       return;
    }
    
     
    if((gCarInfor.state == CarMaintain && gCarInfor.userid!=0)||gCarInfor.state == CoinRenting || gCarInfor.state == MobileRenting || (gCarInfor.state == BTRenting)) 
    {
      gCarInfor.tripMileage = gCarInfor.recovTripMileage+(uint32)(gMotorInfor.tripPulseCounter*gMotorInfor.MetrePerPulse);                    
    }      
    

    

}

#pragma CODE_SEG __NEAR_SEG NON_BANKED
/*
*********************************************************************************************************
*                                               TIM5_ISR()
*
* Description : 投币信号检测，输入捕获
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/
void interrupt TIM5_ISR(void)
{
     static uint16 timer0=0,timer1=0;
     static uint8 ToubiCaptureMode=1;
     
     TFLG1_C5F = 1;  //清中断标志位
     if(ToubiCaptureMode==1)//当前为上升沿捕捉 TCNTdiff1为下降沿到上升沿的脉冲宽度 
     {  
        TCTL3_EDG5A = 0;                         //设置下降沿捕捉
        TCTL3_EDG5B = 1;
        ToubiCaptureMode = 0;
        timer0 =  Timer0Counter;
     } 
     else   //下降沿到来时，记录TCNT  TCNTdiff2为上升沿到下降沿的脉冲宽度 
     {
        TCTL3_EDG5A = 1;                         //设置上升沿捕捉
        TCTL3_EDG5B = 0;
        ToubiCaptureMode = 1;
        timer1 = Timer0Counter; 
        coin_plus_trig = false;
        if(timer1>timer0) 
        {
          if((timer1-timer0)>2 && (timer1-timer0)<8) 
          {
              coin_plus_trig = true; 
          }
        } 
        else
        {
          if((65535-timer0+timer1)>2 && (65535-timer0+timer1)<8) 
          {
              coin_plus_trig = true;  
          }
        }
     }
    
     if(VCUWorkMode == TEST_MODE) 
     {
        coin_plus_trig = true;       //测试模式下不检测脉冲宽度 
     }
     #if DEBUG_MODE==1
     coin_plus_trig = true;       
     #endif
}


/*
*********************************************************************************************************
*                                               TIM3_ISR()
*
* Description : AD信号定时采集，中断执行频率10ms
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/


uint16 ADtemp[11][20];

void interrupt TIM3_ISR(void)
{

    static uint8 ADIndex1=0,ADIndex2=0,ADIndex3=0;
    uint16 i;
     
    
    
    TFLG1 = TFLG1_C3F_MASK; 
    EnableInterrupts;
   
    Timer0Counter++; 
    TimerA_Counter++;
    
    TC3 = TCNT+Timer3;
    
   if(TimerA_Counter%10 == 0)
    {            
        Motorstep_per_sec_count = Motorstep_per_sec;  
        Motorstep_per_sec = 0;
         
          
        #if DEBUG_MODE==0  
        //刹车性能评价
        BrakePerforCheck();
        #endif 
        
        //语音处理
        VoiceProcess();
    }
    
    if(TimerA_Counter%25 == 0)
    {
       

        //计算倾斜角度
        SlopeAngleCalc();  
       
       // SendCarDataByCan(); 
         
        BodyEquipControl();               
    }

    if(TimerA_Counter%50 == 0)
    {                  
        CarStatusIndicate();  
         TripInforCalc();  
        //计算倾斜角度
       // SlopeAngleCalc();            
    }
    
    if(TimerA_Counter%100 == 0) 
    {                                             
        gGPRSConnectTimer++;
        gGprsInitialTimer++;
        
        gCarInfor.BTTimeout++;                        
        
        //TripInforCalc();
         
    }
    
    if(TimerA_Counter%200 == 0) 
    {                                             
        freq_2s = 1;
    }
    
    
    if(TimerA_Counter%500 == 0)           //5秒钟计时
    {
        freq_5s = 1;
        
    }
    if(TimerA_Counter%1500 == 0)          //15秒钟计时
    {
        freq_15s = 1;           
    }
    
    if(TimerA_Counter%6000 == 0)          //60秒钟计时
    {
        freq_60s = 1;
    }

    if((gCarThreshold.shortHeartTime>0)&&(TimerA_Counter%(gCarThreshold.shortHeartTime*100) == 0))
    {
        rent_freq_flag = 1;
    }
    if((gCarThreshold.longHeartTime>0)&&(TimerA_Counter%(gCarThreshold.longHeartTime*100) == 0)) 
    {            
        standby_freq_flag = 1;             
    }
    
      
    
   
    if(TimerA_Counter%5==0) //50ms采集一次电池电压，电流 温度
    {      
      ADtemp[BATTERY_VOLT_CH][ADIndex1] = ADC_GetValue(BATTERY_VOLT_CH);     //电池电压
      ADtemp[BATTERY_CURR_CH][ADIndex1] = ADC_GetValue(BATTERY_CURR_CH);     //电池电流
      ADtemp[BATTERY_TEMP_CH][ADIndex1] = ADC_GetValue(BATTERY_TEMP_CH);     //电池温度    
      
      if(ADIndex1>=19) 
      {
        ADIndex1 = 0;
      }
      else
      {
        ADIndex1 ++;
      }
    }
    ADtemp[ACCEL_PEDAL_CH][ADIndex3] = ADC_GetValue(ACCEL_PEDAL_CH);       //踏板信号
    ADtemp[DIP_X_AXIS_CH][ADIndex2] = ADC_GetValue(DIP_X_AXIS_CH);         //倾角X轴 
    ADtemp[DIP_Y_AXIS_CH][ADIndex2] =  ADC_GetValue(DIP_Y_AXIS_CH);     //Y轴0.66         
    ADtemp[DIP_Z_AXIS_CH][ADIndex2] = ADC_GetValue(DIP_Z_AXIS_CH);         //倾角z轴
    ADtemp[CORNER_SENSOR1_CH][ADIndex2] = ADC_GetValue(CORNER_SENSOR1_CH);     //角度传感器一
    ADtemp[CORNER_SENSOR2_CH][ADIndex2] = ADC_GetValue(CORNER_SENSOR2_CH);     //角度传感器二    
    
    if(ADIndex2>=9) 
    {
      ADIndex2 = 0;
    }
    else
    {
      ADIndex2 ++;
    }
    
    if(ADIndex3>=9) 
    {
      ADIndex3 = 0;
    }
    else
    {
      ADIndex3 ++;
    }
    //振动强度计算
    VibrateLevelCalc();
    
    
    
    
    MatrixAD[DIP_X_AXIS_CH]   = SignalFilter(ADtemp[DIP_X_AXIS_CH],10);    
    MatrixAD[DIP_Y_AXIS_CH]   = SignalFilter(ADtemp[DIP_Y_AXIS_CH],10);       
    MatrixAD[ACCEL_PEDAL_CH]  = SignalFilter(ADtemp[ACCEL_PEDAL_CH],10);    
    MatrixAD[DIP_Z_AXIS_CH] = SignalFilter(ADtemp[DIP_Z_AXIS_CH],10);
    MatrixAD[BATTERY_TEMP_CH] = SignalFilter(ADtemp[BATTERY_TEMP_CH],20); 
    MatrixAD[BATTERY_CURR_CH] = SignalFilter(ADtemp[BATTERY_CURR_CH],20);
    MatrixAD[BATTERY_VOLT_CH] = SignalFilter(ADtemp[BATTERY_VOLT_CH],20);
    MatrixAD[CORNER_SENSOR1_CH] = SignalFilter(ADtemp[CORNER_SENSOR1_CH],20);   
    MatrixAD[CORNER_SENSOR2_CH] = SignalFilter(ADtemp[CORNER_SENSOR2_CH],20);   
    
    
    //踏板位置计算        
    PedalPosCalc();
    
    
    //转向角度计算
    SteeringAngleCalc(); 
    
    
}

#pragma CODE_SEG DEFAULT
