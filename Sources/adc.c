#include  "includes.h"




// AD初始化
ADC_Type ADC_Cfg=
{
    ADCPRECISION_10,ADCollectMode_Con,ADCollect_ClockCycles
};                     

//***************************************************************
//函数名：ADC_Init
//功能  ：   AD采集初始化
//函数参数：1结构体类型
//返回值：无
//***************************************************************
void ADC_Init(ADC_Ref ADC_Ptr)
{
    if(ADC_Ptr == NULL)
        ADC_Ptr = &ADC_Cfg;

    ATDCTL1 =ADC_Ptr->ADCPrecision;  /* 10-Bit resolution ,discharge before sampling. */
    ATDCTL2 =ADC_Ptr->ADCollectMode;
    ATDCTL3 = 0x88;  /* Right Justified Data, Single conversion sequence */
    ATDCTL4 = ADC_Ptr->ADClockCycles;  /* 6 MHz, Notice: 12MHz Max ATD Clock, Fatdlk = FBUS/(2*(PRS+1)) */
    /* 26 ATD Clock cycles sample time */
    
}
//***************************************************************
//函数名DC_IPT_Enable
//功能  ：使能AD中断
//函数参数：无
//返回值：无
//***************************************************************
void ADC_IPT_Enable()
{
    ATDCTL2_ASCIE =1;
}
//***************************************************************
//函数名DC_IPT_DisEnable
//功能  ：关闭AD中断
//函数参数：无
//返回值：无
//***************************************************************
void ADC_IPT_DisEnable()
{
    ATDCTL2_ASCIE =0;
}
// AD初始化
//void ADC_Init(void)
//{
//  ATDCTL1 = 0x3F;  /* 10-Bit resolution ,discharge before sampling. */

//  ATDCTL3 = 0x88;  /* Right Justified Data, Single conversion sequence */

//  ATDCTL4 = 0xE1;  /* 6 MHz, Notice: 12MHz Max ATD Clock, Fatdlk = FBUS/(2*(PRS+1)) */
/* 26 ATD Clock cycles sample time */

//}

// ADC通道采集
uint16 ADC_GetValue(byte ch)
{
    ATDCTL5 = 0x0F & ch;  /* Start Continuous Conversions on ch */
    while (!ATDSTAT0_SCF);  /* wait for conversion sequence to complete   */
    return ATDDR0;
}

/*
*********************************************************************************
* 函数名：SignalFilter
* 说  明：针对AD采样到的信号进行滤波处理
* 参  数：uint16* signaldata 信号数据数组,uint8 length 数据长度
* 返回值: 处理结果
* 其  他：
*********************************************************************************
*/

uint16 SignalFilter(uint16* signaldata,uint8 length) 
{
   uint8 i,j;
   uint16 min,max,sum;
   
   min=signaldata[0];
   max=signaldata[0];
   for(i=0;i<length;i++) 
   {
      if(min>signaldata[i]) 
      {
        min = signaldata[i];
      }
      if(max<signaldata[i]) 
      {
        max = signaldata[i];
      }
   }
   
   if(max>(min+500)) 
   {
    
     j=0;
     sum=0;
     for(i=0;i<length;i++) 
     {
        if(min==signaldata[i]) 
        {
          continue;
        }
        if(max==signaldata[i]) 
        {
          continue;
        }
        j++;
        sum+=signaldata[i];
     }
   } 
   else 
   {
     sum=0;
     j=length;
     for(i=0;i<length;i++) 
     {        
        sum+=signaldata[i];
     }
   }
   if(sum==0) 
   {
      return (uint16)min;
   } 
   else
   {
      return (uint16)((FP32)sum/j);
   }
}


void BatteryDataProc() 
{
    FP32 temp;
    FP32 Battery_temp;                                        
    
                                        
   //计算电压
    
    temp = MatrixAD[BATTERY_VOLT_CH]*gCarPara.batVoltReviseCoef;   
    gCarInfor.batVolt =(uint16)(temp*1000);
    
    //计算电流
    
    if(MatrixAD[BATTERY_CURR_CH]!=0 && gCarPara.batCurrReviseCoef>0) 
    {            
      temp = gCarPara.mcuRefVoltStand*((FP32)MatrixAD[BATTERY_CURR_CH]/1024);
    
       //得到电压差
      if(temp>(FP32)gCarPara.batzeroCurrVolt) 
      {        
        temp =  temp-(FP32)gCarPara.batzeroCurrVolt;        
        temp =  temp/(FP32)gCarPara.batCurrReviseCoef;        
        gCarInfor.batCurr = (uint16)(temp*1000);
      } 
      else            
      {
        gCarInfor.batCurr = 0;
      }
    }
    
    //计算温度    
    
    Battery_temp=MatrixAD[BATTERY_TEMP_CH]*10/(1024-MatrixAD[BATTERY_TEMP_CH]);
    Battery_temp=-26.614*log(Battery_temp)+91.278;
    
    if(Battery_temp<-50) 
    {      
      gCarInfor.batTemp=0xFF;
    } 
    else
    {
      if(Battery_temp>=0) 
      {        
        gCarInfor.batTemp=(uint8)Battery_temp;
      } 
      else 
      {
        gCarInfor.batTemp=0;
      }
    }  
    
    
    #if DEBUG_MODE== 1
    gCarInfor.batTemp = 25;
    gCarInfor.batVolt = 53000;
    gCarInfor.batCurr = 5600;
    //gCarInfor.speed = 10;
    #endif
}

void SlopeAngleCalc() 
{
    FP32 temp,temp1,temp2;
    FP32 U0,U1,U;
    static sint8  dip[4],fab[4],updown[4];
    static uint8 index=0;
    uint8 i;
    
   
    U0=(gCarPara.XAccelBase*1.0/1024)*(gCarPara.mcuRefVoltStand);   //计算加速度计的基准电压
    U1=(MatrixAD[DIP_X_AXIS_CH]*1.0/1024)*(gCarPara.mcuRefVoltStand); //计算当前电压
    U=(U0-U1)*1.25;          //求出压差
    temp1=asin(U)*57.3;   //根据电压变化与角度正弦值成正比的关系求出坡度
     
   
    U0=(gCarPara.YAccelBase*1.0/1024)*(gCarPara.mcuRefVoltStand);   //计算加速度计的基准电压
    U1=(MatrixAD[DIP_Y_AXIS_CH]*1.0/1024)*(gCarPara.mcuRefVoltStand); //计算当前电压
    U=(U0-U1)*1.25;//求出压差
    temp=asin(U)*57.3;   //根据电压变化与角度正弦值成正比的关系求出坡度
     
    
    U0=(gCarPara.ZAccelBase*1.0/1024)*(gCarPara.mcuRefVoltStand);   //计算加速度计的基准电压
    U1=(MatrixAD[DIP_Z_AXIS_CH]*1.0/1024)*(gCarPara.mcuRefVoltStand); //计算当前电压
    U=(U0-U1)*1.25;//求出压差
    temp2=asin(U)*57.3;   //根据电压变化与角度正弦值成正比的关系求出坡度     
    
       
       
    #if DEBUG_MODE==1     
    gCarInfor.dipAngle=0;
    gCarInfor.fabAngle=0;
    gCarInfor.updownAngle=0;
    #endif

    #if DEBUG_MODE==0     

    dip[index]= (sint8)(temp);
        
    
    //fab[index]= (sint8)(temp1);  
    gCarInfor.fabAngle = (sint8)(temp1);
    updown[index]= (sint8)(temp2); 
    
    if(index<3) 
    {
      index++;
    } 
    else
    {
      index = 0;
    }
    
    gCarInfor.dipAngle = dip[0];
    //gCarInfor.fabAngle = fab[0]; 
    gCarInfor.updownAngle = updown[0];  
    
    //取小角度为最终角度值
    for(i=0;i<4;i++)
    {
     
      
   /*   if(abs(gCarInfor.fabAngle) > abs(fab[i])) 
      {
         gCarInfor.fabAngle = fab[i];
      }
        */
       if(abs(gCarInfor.dipAngle) > abs(dip[i])) 
      {
         gCarInfor.dipAngle = dip[i];
      }
      
      if(abs(gCarInfor.updownAngle) > abs(updown[i])) 
      {
         gCarInfor.updownAngle = updown[i];
      } 
      
    }
        
    #endif    
}

void VibrateLevelCalc()
{
    uint8 xlevel[4],ylevel[4],zlevel[4],level=0,i;
    uint8 X_LEVEL1,X_LEVEL2,X_LEVEL3;
    uint8 Y_LEVEL1,Y_LEVEL2,Y_LEVEL3;
    uint8 Z_LEVEL1,Z_LEVEL2,Z_LEVEL3;
    
    
    if(gCarInfor.speed!=0) 
    {      
      X_LEVEL1 = 10;          //该数值相对于标定时的值作为判断依据
      X_LEVEL2 = 30;              
      X_LEVEL3 = 100; 
      
      Y_LEVEL1 = 40;          //该数值相对于标定时的值作为判断依据
      Y_LEVEL2 = 80;              
      Y_LEVEL3 = 160; 
      
      Z_LEVEL1 = 40;          //该数值相对于标定时的值作为判断依据
      Z_LEVEL2 = 80;              
      Z_LEVEL3 = 160;    
    } 
    else
    {
      X_LEVEL1 = 10;          //该数值相对于标定时的值作为判断依据
      X_LEVEL2 = 20;              
      X_LEVEL3 = 40; 
      
      Y_LEVEL1 = 20;          //该数值相对于标定时的值作为判断依据
      Y_LEVEL2 = 40;              
      Y_LEVEL3 = 60; 
      
      Z_LEVEL1 = 20;          //该数值相对于标定时的值作为判断依据
      Z_LEVEL2 = 40;              
      Z_LEVEL3 = 60;   
    }


    memset(xlevel,0,4);
    memset(ylevel,0,4);
    memset(zlevel,0,4);
    //统计20次即20*50=1000ms 内的振动等级
    for(i=0;i<20;i++)
    {
      if(ADtemp[DIP_X_AXIS_CH][i]>(gCarPara.XAccelBase-X_LEVEL1)) 
      {
          xlevel[0]++;
      } 
      else if(ADtemp[DIP_X_AXIS_CH][i]>(gCarPara.XAccelBase-X_LEVEL2)) 
      {
          xlevel[1]++;
      } 
      else if(ADtemp[DIP_X_AXIS_CH][i]>(gCarPara.XAccelBase-X_LEVEL3)) 
      {
          xlevel[2]++;
      } 
      else
      {   
          xlevel[3]++;
      }
      
      if(ADtemp[DIP_Y_AXIS_CH][i]>(gCarPara.YAccelBase-Y_LEVEL1)) 
      {
          ylevel[0]++;
      } 
      else if(ADtemp[DIP_Y_AXIS_CH][i]>(gCarPara.YAccelBase-Y_LEVEL2))
      {
          ylevel[1]++;
      } 
      else if(ADtemp[DIP_Y_AXIS_CH][i]>(gCarPara.YAccelBase-Y_LEVEL3))
      {
          ylevel[2]++;
      } 
      else
      {   
          ylevel[3]++;
      }
      
      
      if(ADtemp[DIP_Z_AXIS_CH][i]>(gCarPara.ZAccelBase-Z_LEVEL1)) 
      {
          zlevel[0]++;
      } 
      else if(ADtemp[DIP_Z_AXIS_CH][i]>(gCarPara.ZAccelBase-Z_LEVEL2))
      {
          zlevel[1]++;
      } 
      else if(ADtemp[DIP_Z_AXIS_CH][i]>(gCarPara.ZAccelBase-Z_LEVEL3))
      {
          zlevel[2]++;
      } 
      else
      {   
          zlevel[3]++;
      }
    }

    //计算等级的总和
    for(i=0;i<4;i++)
    {
       xlevel[i] = xlevel[i]+ylevel[i]+zlevel[i];
       
       if(xlevel[i]>(6-i)) 
       {
         level = i;
       }
    }


    if(gCarInfor.VibrateLevel==0 || level>gCarInfor.VibrateLevel) 
    {
     gCarInfor.VibrateLevel=level;
    }
}

void SteeringAngleCalc() 
{
    uint8 y_loopangle=0;
    uint16 y_angle=0;
    uint16 y_tempangle =0;
    uint16 y_Maxtempad3 = 0;
    uint16 y_Mintempad3 = 1024;

    ////////////////////////Y 轴AD平均值计算///////////////////////////         
    if(y_loopangle>=7)
    {
      y_angle = y_angle-y_Maxtempad3-y_Mintempad3;
      y_angle/= (y_loopangle-2);
      y_tempangle =y_angle;
      y_Maxtempad3 = 0;
      y_Mintempad3 = 1024;
      y_loopangle = 0;
      y_angle = 0;
    } 
    else
    {
      y_loopangle++;
      y_angle += MatrixAD[DIP_Y_AXIS_CH];           
      if(MatrixAD[DIP_Y_AXIS_CH]>y_Maxtempad3)
        y_Maxtempad3= MatrixAD[DIP_Y_AXIS_CH];
      if(MatrixAD[DIP_Y_AXIS_CH]<y_Mintempad3)
        y_Mintempad3= MatrixAD[DIP_Y_AXIS_CH];
    }            
}

void PedalPosCalc() 
{
  gCarInfor.accelPedalPos = MatrixAD[ACCEL_PEDAL_CH];
   
  if(gCarPara.maxAccelPedal>gCarPara.minAccelPedal+300) 
  {      
    if(gCarInfor.accelPedalPos>=gCarPara.minAccelPedal) 
    {      
      if(gCarInfor.accelPedalPos<=gCarPara.maxAccelPedal) 
      {              
        gCarInfor.accPedal = (uint8)((FP32)(gCarPara.maxAccelPedal - gCarInfor.accelPedalPos)*100 /(FP32)(gCarPara.maxAccelPedal-gCarPara.minAccelPedal));
      }
      else
      {
        gCarInfor.accPedal = 0;
      }
    } 
    else
    {
      gCarInfor.accPedal = 99;
    }
  } 
  else if(gCarPara.minAccelPedal>gCarPara.maxAccelPedal+300) 
  {      
    if(gCarInfor.accelPedalPos<=gCarPara.minAccelPedal) 
    {      
      if(gCarInfor.accelPedalPos>=gCarPara.maxAccelPedal) 
      {              
        gCarInfor.accPedal = (uint8)((FP32)(gCarInfor.accelPedalPos - gCarPara.maxAccelPedal)*100 /(FP32)(gCarPara.minAccelPedal-gCarPara.maxAccelPedal));
      }
      else
      {
        gCarInfor.accPedal = 0;
      }
    } 
    else
    {
      gCarInfor.accPedal = 99;
    }
  }  
  else
  {
    gCarInfor.accPedal = 99;
  }
  
}

