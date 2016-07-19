#include "includes.h"

// 0~10    刹车区
//11~25    起步区
//26~65    加速区
//66~80    保持区
//81~90    起步区
//91~100   刹车区
uint8  pedalAreaBoundPer[5]={10,25,65,85,90}; //给定的踏板区间对应的百分比25    65
uint16 deststep[100]=0;                       //目标脉冲数
uint16 basicDuty[100]=0;                      //基础duty值

uint16 Duty = 500;                            //PWM占空比，值越小速度越快

sint8 uphill_alpha[4]={0,6,10,14};             //上坡坡度
sint8 flat_alpha[5]={2,-2,-6,-10,-14};         //非上坡坡度

uint8 uphill_flag=0;
uint8 Rollover_flag=0;

uint8 getDestMotorSpeed(uint8 maxspeed) 
{
    FP32 destmotorspeed;
    
    if(maxspeed>=0&& maxspeed<=30) 
    {      
      destmotorspeed = 1.631*maxspeed+0.5237;
    }
    
    if(destmotorspeed<1) 
    {
      destmotorspeed = 1;
    }
    
    return (uint8)destmotorspeed; 
}

/*************************************************************/
/*不同电机、电机控制器、VCU版本根据实际标定数据来决定这个函数*/
/*内duty的不同计算公式。                                     */
/*************************************************************/
uint16 getDestDuty(uint8 maxspeed) 
{
    FP32 destduty;
    
    switch(gCarInfor.controllerType) 
    {
      
      case LiWei:
    
      if(maxspeed>0&& maxspeed<=30) 
      {      
        destduty = -6.99*maxspeed + 332.38;      
      }
      else if(maxspeed==0) 
      {
        destduty = 500;
      }
      
      if(destduty<100) 
      {
        destduty = 100;
      }
      break;
      
     case KeChuang:
    
     if(maxspeed>0&& maxspeed<=5) 
      {      
        destduty = -6.99*maxspeed +367.79;// 305.79;       //362.79
        //destduty = -9.85*maxspeed +376.79;
        //destduty = -9.19*(0.76*maxspeed+97.06) +
      } 
      else if(maxspeed>5&& maxspeed<=30) 
      {      
        //destduty = -6.99*maxspeed +362.79;// 305.79;       //362.79
        destduty = -9.85*maxspeed +376.79;
        //destduty = -9.19*(0.76*maxspeed+97.06) +
      } 
      else if(maxspeed==0) 
      {
        if(destduty<=200&&destduty>50) 
        {
          destduty+=10;
        } 
        else if(destduty>200&&destduty<=300) 
        {
          destduty+=8;
        } 
        else if(destduty>300&&destduty<=400) 
        {
          destduty+=5;
        } 
        else if(destduty<=50)
        {
          destduty = 500; 
        }
        else 
        {
          destduty = 500; 
        }
      }
      
      if(destduty<100)    
      {
        destduty = 100;
      }
      break;

    }
    return (uint16)destduty; 
}

void MotorCtrlCurveCalc(uint8 maxspeed)
{

   #if DEBUG_MODE == 0
   
   uint8 j=0;
   static uint8 preMaxSpeed = 0;
   FP32 duty0[2];//在加速区的目标速度和占空比
   FP32 dest[2];   
   
   if(maxspeed==preMaxSpeed)  //判断最大速度是否变化
   {   
    return;                       
   } 
   preMaxSpeed=maxspeed;
     
   for(j=0;j<pedalAreaBoundPer[0];j++)                     //刹车区
   {
       basicDuty[j]=getDestDuty(0);
       deststep[j]=0;
   }
   
   if(maxspeed>2) 
   {    
    duty0[0] = getDestDuty(2);
    dest[0]  = getDestMotorSpeed(2);
   } 
   else
   {
    duty0[0] = getDestDuty(1);
    dest[0]  = getDestMotorSpeed(1);
   }
   
   for(j=pedalAreaBoundPer[0];j<pedalAreaBoundPer[1];j++)  //起步区 10-25
   {
       basicDuty[j] = duty0[0];
       deststep[j]  = dest[0];
   }
   
   
   if(maxspeed>2) 
   {    
       dest[0]  = getDestMotorSpeed(2);
       dest[1]  = getDestMotorSpeed(maxspeed);
     
       duty0[0] = getDestDuty(2);
       duty0[1] = getDestDuty(maxspeed);
   
   } 
   else
   {
      dest[0]  = getDestMotorSpeed(1);
      dest[1]  = getDestMotorSpeed(maxspeed);

      duty0[0] = getDestDuty(1);
      duty0[1] = getDestDuty(maxspeed);
       
   }
   
   for( j=pedalAreaBoundPer[1]; j<pedalAreaBoundPer[2]; j++) //加速区 25-65
   {       
       basicDuty[j]=duty0[0]-(duty0[0]-duty0[1])*(j-pedalAreaBoundPer[1])/(pedalAreaBoundPer[2]-pedalAreaBoundPer[1]);
       deststep[j]=(dest[1]-dest[0])*(j-pedalAreaBoundPer[1])/(pedalAreaBoundPer[2]-pedalAreaBoundPer[1])+dest[0];
       
   }
   
   
   for(j=pedalAreaBoundPer[2];j<pedalAreaBoundPer[3];j++)    //保持区 65-89
   {       
       basicDuty[j]=duty0[1];
       deststep[j]=dest[1];
       
   }
   
   if(maxspeed>2) 
   {    
      duty0[0] = getDestDuty(2);
      dest[0]  = getDestMotorSpeed(2);
   } 
   else
   {
      duty0[0] = getDestDuty(1);
      dest[0]  = getDestMotorSpeed(1);
   }
   for(j=pedalAreaBoundPer[3];j<pedalAreaBoundPer[4];j++)    //起步区 89-90
   {
       basicDuty[j]=duty0[0];
       deststep[j]=dest[0];
   }
   // yzl
   for(j=pedalAreaBoundPer[4];j<100;j++)           //刹车区  90-100
   {
       basicDuty[j]=getDestDuty(0);
       deststep[j]=0;
   }

   #endif 
}

 void MotorInit()
 {   
    FP32 A,B;     //0.04158
    A= (gCarPara.hallSensorPara*gCarPara.gearRatio)*1000;    //减速比改为浮点直接保存
    B= (gCarPara.tireDiameter*3.14);  //8.5--0.02585--1.6085                       //轮胎周长
    
    //31706                                   
    if(A!=0 && B!=0) 
    {                
      gMotorInfor.MetrePerPulse = B/A;                                 //一个脉冲多少米      
    }
    
    Duty = getDestDuty(0); 
} 

uint16 MotorCtrlPID()
{    
    static uint8 feedback_time=0;//反馈时间    
    static uint16 dutytemp; 
    uint8 temp;
              
    if(gCarInfor.accPedal>=pedalAreaBoundPer[0] && gCarInfor.accPedal<pedalAreaBoundPer[1]) 
    {                   
        dutytemp=basicDuty[gCarInfor.accPedal];
    }
    else if( gCarInfor.accPedal>=pedalAreaBoundPer[1] && gCarInfor.accPedal<pedalAreaBoundPer[2] ) 
    {       
         
         if(dutytemp == getDestDuty(0))
         {
            dutytemp = basicDuty[gCarInfor.accPedal];
           // feedback_time = 20;
         }
         
         //目标转速较低情况下，不做差值判断，主要适合倒车时的反馈
         if(deststep[gCarInfor.accPedal]<=3) 
         {            
            if(Motorstep_per_sec_count+1<=deststep[gCarInfor.accPedal]) 
            {              
              if(feedback_time>=20)
              { 
                  if(dutytemp > (basicDuty[gCarInfor.accPedal]-60))
                  {                   
                     dutytemp--;
                  }
                  feedback_time = 0;
              }
              feedback_time++;
            } 
            else if(Motorstep_per_sec_count>deststep[gCarInfor.accPedal]+1) 
            {
              if(feedback_time>=20) 
              { 
                  if(dutytemp < (basicDuty[gCarInfor.accPedal]))
                  {                   
                     dutytemp++;
                  }
                  feedback_time = 0;
              }
              feedback_time++;
            } 
            else
            {
              dutytemp = basicDuty[gCarInfor.accPedal];
            }
         } 
         else if((Motorstep_per_sec_count+3)<deststep[gCarInfor.accPedal])
         {   
              if(feedback_time>=20) //40*5=200ms
              { 
                  if(dutytemp > (basicDuty[gCarInfor.accPedal]-5))
                  {
                     temp = (deststep[gCarInfor.accPedal] - Motorstep_per_sec_count)>>1;
                    // if(temp>1&&temp<10) 
                     if(temp>1&&temp<8) 
                     {                                           
                       dutytemp -= temp;
                     }
                     else if(temp<=1) 
                     {
                       dutytemp--;
                     }
                     else
                     {
                       //dutytemp -=10;
                       dutytemp -=15; //增大爬坡时反馈力度
                     }
                  }
                  feedback_time = 0;
              }
              feedback_time++;
          }
          else if((Motorstep_per_sec_count)>(deststep[gCarInfor.accPedal]+3))
          {
              if(feedback_time>=40) //40*5=200ms
              { 
                  if(dutytemp < (basicDuty[gCarInfor.accPedal]+5))
                  {
                     dutytemp++;
                  }
                  feedback_time = 0;
              }
              feedback_time++;
          } 
          else 
          {                    
            if(dutytemp>basicDuty[gCarInfor.accPedal]+20 || dutytemp<basicDuty[gCarInfor.accPedal]-20) 
            {                          
              dutytemp = basicDuty[gCarInfor.accPedal];
            }
          }
          
    } 
    else if(gCarInfor.accPedal>=pedalAreaBoundPer[2] && gCarInfor.accPedal<pedalAreaBoundPer[3]) 
    {
          if(dutytemp==getDestDuty(0))
          {
            dutytemp = basicDuty[gCarInfor.accPedal];
           // feedback_time = 40;
          }


          //目标转速较低情况下，不做差值判断，主要适合倒车时的反馈
          if(deststep[gCarInfor.accPedal]<=3) 
          {            
            if(Motorstep_per_sec_count+1<=deststep[gCarInfor.accPedal]) 
            {              
              if(feedback_time>=20)
              { 
                  if(dutytemp > (basicDuty[gCarInfor.accPedal]-60))
                  {                   
                     dutytemp--;
                  }
                  feedback_time = 0;
              }
              feedback_time++;
            } 
            else if(Motorstep_per_sec_count>deststep[gCarInfor.accPedal]+1) 
            {
              if(feedback_time>=20) 
              { 
                  if(dutytemp < (basicDuty[gCarInfor.accPedal]))
                  {                   
                     dutytemp++;
                  }
                  feedback_time = 0;
              }
              feedback_time++;
            } 
            else
            {
              dutytemp = basicDuty[gCarInfor.accPedal];
            }
          } 
          else if(Motorstep_per_sec_count+3<deststep[gCarInfor.accPedal])
          {  
              if(feedback_time>=20)
              {                   
               //  temp = (deststep[gCarInfor.accPedal] - Motorstep_per_sec_count)>>1;  
               temp = (deststep[gCarInfor.accPedal] - Motorstep_per_sec_count)>>1;
                 //if(temp>1&&temp<10)  
                 if(temp>1&&temp<8) 
                 {                                           
                   dutytemp -= temp;
                 }
                 else if(temp<=1) 
                 {
                   dutytemp--;
                 }
                 else
                 {
                  // dutytemp -=10;
                   dutytemp -=15;
                 }
                 feedback_time=0;
              } 
              feedback_time++;
          } 
          else if(Motorstep_per_sec_count>deststep[gCarInfor.accPedal]+3) 
          {
              if(feedback_time>=40)
              { 
                  if(dutytemp < (basicDuty[gCarInfor.accPedal]+5)) 
                  {
                     dutytemp++;
                  }
                  feedback_time = 0;
              } 
              feedback_time++;
          }
          else 
          {                        
            if(dutytemp>basicDuty[gCarInfor.accPedal]+20 || dutytemp<basicDuty[gCarInfor.accPedal]-20) 
            {                          
              dutytemp = basicDuty[gCarInfor.accPedal];
            }               
          }
    } 
    else if(gCarInfor.accPedal>=pedalAreaBoundPer[3] && gCarInfor.accPedal<pedalAreaBoundPer[4]) 
    {                       
        //  dutytemp=basicDuty[gCarInfor.accPedal];
    
          if(dutytemp==getDestDuty(0))
          {
            dutytemp = basicDuty[gCarInfor.accPedal];
           // feedback_time = 40;
          }


          //目标转速较低情况下，不做差值判断，主要适合倒车时的反馈
          if(deststep[gCarInfor.accPedal]<=3) 
          {            
            if(Motorstep_per_sec_count+1<=deststep[gCarInfor.accPedal]) 
            {              
              if(feedback_time>=20)
              { 
                  if(dutytemp > (basicDuty[gCarInfor.accPedal]-60))
                  {                   
                     dutytemp--;
                  }
                  feedback_time = 0;
              }
              feedback_time++;
            } 
            else if(Motorstep_per_sec_count>deststep[gCarInfor.accPedal]+1) 
            {
              if(feedback_time>=20) 
              { 
                  if(dutytemp < (basicDuty[gCarInfor.accPedal]))
                  {                   
                     dutytemp++;
                  }
                  feedback_time = 0;
              }
              feedback_time++;
            } 
            else
            {
              dutytemp = basicDuty[gCarInfor.accPedal];
            }
          } 
          else if(Motorstep_per_sec_count+3<deststep[gCarInfor.accPedal])
          {  
              if(feedback_time>=20)
              {                   
               //  temp = (deststep[gCarInfor.accPedal] - Motorstep_per_sec_count)>>1;  
                temp = (deststep[gCarInfor.accPedal] - Motorstep_per_sec_count)>>1;
                 //if(temp>1&&temp<10)  
                 if(temp>1&&temp<8) 
                 {                                           
                   dutytemp -= temp;
                 }
                 else if(temp<=1) 
                 {
                   dutytemp--;
                 }
                 else
                 {
                  // dutytemp -=10;
                   dutytemp -=15;
                 }
                 feedback_time=0;
              } 
              feedback_time++;
          } 
          else if(Motorstep_per_sec_count>deststep[gCarInfor.accPedal]+3) 
          {
              if(feedback_time>=40)
              { 
                  if(dutytemp < (basicDuty[gCarInfor.accPedal]+5)) 
                  {
                     dutytemp++;
                  }
                  feedback_time = 0;
              } 
              feedback_time++;
          }
          else 
          {                        
            if(dutytemp>basicDuty[gCarInfor.accPedal]+20 || dutytemp<basicDuty[gCarInfor.accPedal]-20) 
            {                          
              dutytemp = basicDuty[gCarInfor.accPedal];
            }               
          }
    }
    else 
    {             
           dutytemp = getDestDuty(0); 
    }

    if(dutytemp<getDestDuty(25))    
    {
       dutytemp = getDestDuty(25);                  
    }
    
   /*if(gCarInfor.speed>gCarInfor.maxSpeed-1) 
    {
      HornSwitch = 1; 
    }*/
    return dutytemp;
} 


 
void CarStopMode() 
{
  
   Duty = getDestDuty(0);
   pwm_regesiterset(Duty);
}
                       //侧翻标志
///////////起步程序////////////////////////
void CarStartingMode()
{   
    #if DEBUG_MODE==0                 
     
    if(device.state.ReverseRelayState == 1) ///倒车标志为 1   0504yzl，解决倒车起步过快
    {
       if(((gCarInfor.fabAngle>=uphill_alpha[0])&& (uphill_alpha[3]>=gCarInfor.fabAngle))||((flat_alpha[1]>=gCarInfor.fabAngle)&& (gCarInfor.fabAngle>=flat_alpha[4])))
        {
            uphill_flag=1;
        }
      // Duty=getDestDuty(3);
       Duty=330;
    }
    else if((gCarInfor.fabAngle>=uphill_alpha[0])&& (uphill_alpha[3]>=gCarInfor.fabAngle))
    {
        uphill_flag=1;
        if((gCarInfor.fabAngle>=uphill_alpha[0]) && (uphill_alpha[1]>gCarInfor.fabAngle))  //在0~5度之间
         {
                    //Duty=getDestDuty(8);
                    Duty=getDestDuty(5);
         }
        else if((gCarInfor.fabAngle>uphill_alpha[1])&& (uphill_alpha[2]>gCarInfor.fabAngle)) //在5-9度之间  
         {
                   //Duty=getDestDuty(9);
                   Duty=getDestDuty(6);
         }
        else     //在9-13度之间
        {
                   //Duty=getDestDuty(10);
                   Duty=getDestDuty(7);
        }
    }
    else if((flat_alpha[0]>gCarInfor.fabAngle) && (gCarInfor.fabAngle>=flat_alpha[4]))   
    {
        //if(gCarInfor.maxSpeed>=10)
        if(gCarInfor.maxSpeed>=5)
        {
            if((flat_alpha[0]>=gCarInfor.fabAngle)&& (gCarInfor.fabAngle>flat_alpha[1]))  //设置-1~2度区间是平地区间   
            {              
               // Duty=getDestDuty(5); 
                Duty=getDestDuty(5);          
            }
            else if ((flat_alpha[1]>=gCarInfor.fabAngle)&& (gCarInfor.fabAngle>=flat_alpha[4]))//-2~-14度之间为下坡区间
            {
                 uphill_flag=1;
                 if((flat_alpha[1]>=gCarInfor.fabAngle) && (gCarInfor.fabAngle>flat_alpha[2]))//-2~-6度之间  
                 {                  
                    Duty=getDestDuty(5); 
                 }
                 else  if ((flat_alpha[2]>=gCarInfor.fabAngle) && (gCarInfor.fabAngle>flat_alpha[3]))//-6~-10度之间
                 {                  
                    Duty=getDestDuty(3); 
                 }
                 else //-10~-14度之间
                 {
                    Duty=getDestDuty(2);     
                 }
            }
         } 
         else 
         {          
            Duty=getDestDuty(gCarInfor.maxSpeed); 
         }
    }
    else  if ((gCarInfor.fabAngle>uphill_alpha[3])||(flat_alpha[4]>gCarInfor.fabAngle))   //在13-90度之间
    {
         Duty = getDestDuty(0);
    }

    pwm_regesiterset(Duty); 
    #endif
}     
////////////////////正常行驶程序/////////////////

void CarRunningMode(void)    
{   
           
    uint8 i=0,step; 
    static uint8 brake_counter = 0;
    static uint8 brake_counter1 = 0;
    static uint8 brake_flag=0; 
        
    #if DEBUG_MODE==0
    /*if((-5)>=gCarInfor.fabAngle&&brake_flag==0)
    {
      //if(gCarInfor.maxSpeed>10&&(gCarInfor.speed>12))
     if(gCarInfor.maxSpeed>10&&(Motorstep_per_sec_count>20))// Motorstep_per_sec_count 
       { 
        
         //if((-3)>=gCarInfor.fabAngle)
         if(brake_counter<=70&&brake_flag==0) 
         {
            Duty=500;        
            pwm_regesiterset(Duty); 
            brake_counter +=1;
           // AlarmVoice(NUMBER_1_VOICE);
         } 
         else
         {
           brake_counter=0;
          // AlarmVoice(NUMBER_2_VOICE);
           brake_flag=1;
         }
       }  
        //else if(gCarInfor.maxSpeed<=10&&(gCarInfor.speed>=gCarInfor.maxSpeed))
         else if(gCarInfor.maxSpeed<=10&&(Motorstep_per_sec_count>=17))
       {
            
          if(brake_counter<=50&&brake_flag==0) 
           {
              Duty=500;        
              pwm_regesiterset(Duty); 
              brake_counter +=1;
             // AlarmVoice(NUMBER_9_VOICE);
           } 
           else
           {
             brake_counter=0;
           //  AlarmVoice(NUMBER_10_VOICE);
             brake_flag=1;
           }
        } 
    
     }
  // else if(((-5)<gCarInfor.fabAngle&&(-3)>=gCarInfor.fabAngle)&&(gCarInfor.speed>gCarInfor.maxSpeed+2)&&brake_flag==0)
    else if(((-5)<gCarInfor.fabAngle&&(-3)>=gCarInfor.fabAngle)&&(Motorstep_per_sec_count>=(uint8)(1.62*gCarInfor.maxSpeed+1.6))&&brake_flag==0)
   {
   //if((-3)>=gCarInfor.fabAngle)
     if(brake_counter<=70&&brake_flag==0) 
     {
        Duty=500;        
        pwm_regesiterset(Duty); 
        brake_counter +=1;
       // AlarmVoice(NUMBER_5_VOICE);
     } 
     else
     {
       brake_counter=0;
       //AlarmVoice(NUMBER_6_VOICE);
       brake_flag=1;
     }
     
  } 
  else if(((-3)<gCarInfor.fabAngle&&(-2)>=gCarInfor.fabAngle)&&(Motorstep_per_sec_count>=(uint8)(1.62*gCarInfor.maxSpeed+2.6))&&brake_flag==0)
  {
   //if((-3)>=gCarInfor.fabAngle)
     if(brake_counter<=60&&brake_flag==0) 
     {
        Duty=500;        
        pwm_regesiterset(Duty); 
        brake_counter +=1;
       // AlarmVoice(NUMBER_7_VOICE);
     } 
     else
     {
       brake_counter=0;
      // AlarmVoice(NUMBER_8_VOICE);
       brake_flag=1;
     }
     
  }
  
   else
  { 
    
     if(brake_flag==1) 
     {
      brake_counter1++;
      if(brake_counter1>150) 
      {
        brake_flag=0;
        //AlarmVoice(NUMBER_3_VOICE);
        brake_counter1=0;
      }
     } 
     else 
     {
       brake_flag=0;
       brake_counter=0;
     }*/
    //最高车速较慢情况下，直接给duty值来进行控制
    if(gCarInfor.maxSpeed<=3)
    {
         Duty = getDestDuty(gCarInfor.maxSpeed);        
    }
    else 
    {
        if(device.state.ReverseRelayState == 1) ///倒车标志为 1
        {                        
            //根据最大速度变化，计算基础duty和脉冲数
           // MotorCtrlCurveCalc(1);
            //倒车情况下，雷达检测到A区域或者踏板位置为非正常行驶区域，则停车
            if(gRearBarriArea == RA_AREA)
            {               
                Duty = getDestDuty(0);        
            }
            else
            {                                                              
               // Duty = MotorCtrlPID(); 
                Duty =330;                       
            }            
        }
        else 
        {
                       
            //根据最大速度变化，计算基础duty和脉冲数
            MotorCtrlCurveCalc(gCarInfor.maxSpeed);
              
            if(gFrontBarriArea == FA_AREA)
            {
                Duty = getDestDuty(0);        
            }
            else if(gFrontBarriArea == FB_AREA)
            {                                
                if(gCarInfor.speed<=3) 
                {
                    Duty = getDestDuty(2);        
                } 
                else if((gCarInfor.speed<=10)&&(gCarInfor.speed>3))  
                {
                    Duty = getDestDuty(2);        
                }
                else if((gCarInfor.speed>10)&&(gCarInfor.speed<=15)) 
                {
                    Duty = getDestDuty(3);        
                }
                else
                {
                    Duty = getDestDuty(4);        
                }                         
                
            }
            else if(gFrontBarriArea == FC_AREA)
            {
                
                if(gCarInfor.speed<=3) 
                {
                     Duty = getDestDuty(2);    
                } 
                else if((gCarInfor.speed<=5)&&(gCarInfor.speed>3)) 
                {
                     Duty = getDestDuty(2);    
                }
                 else if((gCarInfor.speed<=10)&&(gCarInfor.speed>5)) 
                {
                     Duty = getDestDuty(3);    
                }
                else if((gCarInfor.speed>10)&&(gCarInfor.speed<=15)) 
                {
                     Duty = getDestDuty(4);    
                }
                else
                {
                     Duty = getDestDuty(5);    
                }
                
            } 
            else
            {        
                Duty = MotorCtrlPID();
            }
          }               
    }      
    pwm_regesiterset(Duty);  
  //}
    #endif
} 
void CarRunningCtrl() 
{                 
    static uint16 start_counter = 0;
    //////////////////////////////////////起步类型判断 //////////////////////////////////////
    //   放置的位置有待确定:1.在判断外面  2.在判断里面
     //当电机未工作时，设置起步标志位
    if(Motorstep_per_sec_count==0) 
    {          
        //设置起步标志位   
        
        if(gCarInfor.accPedal < pedalAreaBoundPer[0]>>1)  //维持12*5ms=60ms即可启动
        {                
          if(start_counter<1000) 
          {            
            start_counter +=16;
          }
        } 
        else if(gCarInfor.accPedal < pedalAreaBoundPer[0])  //维持25*5ms=75ms即可启动
        {                
          if(start_counter<1000) 
          {            
            start_counter +=8;
          }
        } 
        else if(gCarInfor.accPedal < pedalAreaBoundPer[1]) //维持50*5ms=250ms即可启动
        {
          if(start_counter<1000) 
          {            
            start_counter +=4;
          }
        }
    }
    
     
    if(gCarInfor.accPedal>pedalAreaBoundPer[0] && gCarInfor.accPedal<pedalAreaBoundPer[4] ) 
    {                  
       if((gCarInfor.availableTime >= gCarInfor.tripTime && gCarInfor.state == CoinRenting)||(gCarInfor.state == BTRenting)||(gCarInfor.state == MobileRenting && gCarInfor.userid!=0)||((gCarInfor.state == CarMaintain)&&(gCarInfor.userid!=0)))
       { 
          //当电动车起步条件满足时：电磁刹未打开、电机无输出、起步计数
          if(((gRearBarriArea != FA_AREA && device.state.ReverseRelayState == 1)||(gFrontBarriArea != FA_AREA && device.state.ReverseRelayState != 1))&& (start_counter>=200)&&(Motorstep_per_sec_count==0))   //当电机一旦转动起来，将不会进入此程序，
          {
              CarStartingMode(); 
          }
          else if (Motorstep_per_sec_count!=0) 
          {                             
              CarRunningMode(); //正常行驶程序               
              start_counter = 0;                                                           
          } 
          else 
          {                                     
              ;//保持原有duty输出
          }
       } 
       else
       {         
          CarStopMode();          
          start_counter = 0;
       }
    }
    else
    {         
      if(gCarInfor.accPedal>=pedalAreaBoundPer[4]) 
      {
         start_counter = 0;
      }
      CarStopMode();
    }
     
}

#if DEBUG_MODE==0 


//电机有输出情况下，电磁刹控制程序每5毫秒执行一次
void MotorElecMagnetControl() 
{

  static uint8 uphill_time=0;
  static uint8 braketimer=0;
  
  //停车状态   
  if(gCarInfor.speed==0 || Motorstep_per_sec_count==0)
  {            
    //非起步状态
    if(Duty>=getDestDuty(0)) 
    { 
      braketimer++;     
      if(braketimer>50) //无车速，无输出情况下，250ms后锁电磁刹
      {        
        BrakeRelay = ELECMAGHOLD;        
      }
      
      ///////////////20160223修改停车时蜂鸣器控制///////
    //当超声波探测到A区域有物体，且车速低于5公里每小时则启动电磁刹  
     if(device.state.ReverseRelayState != 1)    
      {      
        if((gFrontBarriArea == FA_AREA) ) 
        {    
          if(gCarInfor.state == BTRenting || gCarInfor.state == MobileRenting || gCarInfor.state == CoinRenting || (gCarInfor.state == CarMaintain && gCarInfor.userid!=0)) 
          {
            HornSwitch = 1;
          }
        } 
        else
        {
          HornSwitch = 0;
        }
      }
    //倒车状态下
       else    
      {
        if(gRearBarriArea == RA_AREA) 
        {     
          if(gCarInfor.state == BTRenting || gCarInfor.state == MobileRenting || gCarInfor.state == CoinRenting || (gCarInfor.state == CarMaintain && gCarInfor.userid!=0)) 
          {
            HornSwitch = 1;
          }
        } 
        else
        {              
          HornSwitch = 0;  
        }   
      }    
  ////////////////////////////////////////
    }  //起步状态下
    else     
    {
      braketimer = 0;
      if(gFrontBarriArea == FA_AREA || gRearBarriArea == FA_AREA) 
      {
         if(device.state.ReverseRelayState != 1)    
        {      
          if((gFrontBarriArea == FA_AREA) ) 
          {    
            if(gCarInfor.state == BTRenting || gCarInfor.state == MobileRenting || gCarInfor.state == CoinRenting || (gCarInfor.state == CarMaintain && gCarInfor.userid!=0)) 
            {
              HornSwitch = 1;
              BrakeRelay = ELECMAGHOLD;   
            }
          } 
          else
          {
            HornSwitch = 0;
             BrakeRelay = ELECMAGRELEASE;  
          }
        }
      //倒车状态下
         else    
        {
          if(gRearBarriArea == RA_AREA) 
          {     
            if(gCarInfor.state == BTRenting || gCarInfor.state == MobileRenting || gCarInfor.state == CoinRenting || (gCarInfor.state == CarMaintain && gCarInfor.userid!=0)) 
            {
              HornSwitch = 1;
              BrakeRelay = ELECMAGHOLD;  
            }
          } 
          else
          {              
            HornSwitch = 0;  
           BrakeRelay = ELECMAGRELEASE;
          }   
        }   
        /*if(gCarInfor.state == BTRenting || gCarInfor.state == MobileRenting || gCarInfor.state == CoinRenting || (gCarInfor.state == CarMaintain && gCarInfor.userid!=0)) 
        {
          HornSwitch = 1;
        }*/ 
       // BrakeRelay = ELECMAGHOLD;        
      } 
      else
      {    
         HornSwitch = 0;       
         if(uphill_flag==1 ) 
         {
            if(uphill_time<60)//坡起执行时间是0.3S;
            { 
              uphill_time++;              
            } 
            else 
            { 
              BrakeRelay = ELECMAGRELEASE;
              uphill_flag=0;
              uphill_time=0;   
            }       
         } 
         else
         {
              BrakeRelay = ELECMAGRELEASE;  
         }
      }
      
    }
  }//行驶状态 
  else
  { 
    braketimer = 0;
    //当超声波探测到A区域有物体，且车速低于5公里每小时则启动电磁刹  
    if(device.state.ReverseRelayState != 1)    
    {      
      if((gFrontBarriArea == FA_AREA) && (gCarInfor.speed>=0&&gCarInfor.speed<=5)) 
      {    
        if(gCarInfor.state == BTRenting || gCarInfor.state == MobileRenting || gCarInfor.state == CoinRenting || (gCarInfor.state == CarMaintain && gCarInfor.userid!=0)) 
        {
          HornSwitch = 1;
        }
        BrakeRelay = ELECMAGHOLD;         
      } 
      else
      {
        HornSwitch = 0;
        if(Duty<getDestDuty(0)) 
        {          
          BrakeRelay = ELECMAGRELEASE;
        }
      }
    }
    //倒车状态下
    else    
    {
      if(gRearBarriArea == RA_AREA) 
      {     
        if(gCarInfor.state == BTRenting || gCarInfor.state == MobileRenting || gCarInfor.state == CoinRenting || (gCarInfor.state == CarMaintain && gCarInfor.userid!=0)) 
        {
          HornSwitch = 1;
        }
        BrakeRelay = ELECMAGHOLD ;
      } 
      else
      {              
        HornSwitch = 0;  
        if(Duty<getDestDuty(0)) 
        {          
          BrakeRelay = ELECMAGRELEASE;
        }
      }   
    }    
  } 
        
}

//根据踏板传感器位置判断在刹车情况下，车速的变化情况
//发送到后台供服务器进行分析
//该函数100ms执行一次
void BrakePerforCheck() 
{
  static uint8 brakeworktimer=0;
  static uint16 prePedalPos=0;
  static uint16 preSpeed=0;
  FP32 temp;
  
  if(prePedalPos==0) 
  {
    prePedalPos = gCarInfor.accelPedalPos;       
  } 
  else
  {
    //判断车辆开始刹车
    if((prePedalPos<gCarPara.maxAccelPedal-5) && (gCarInfor.accelPedalPos>=(gCarPara.maxAccelPedal-10)))
    {
       preSpeed = Motorstep_per_sec_count;
       brakeworktimer = 0;  
       gCarThreshold.brakeActiveCounter++;   //计算刹车次数     
    }
    
    //判断踏板处于刹车区,同时起始速度需要高于5公里每小时
    if(gCarInfor.accelPedalPos>=(gCarPara.maxAccelPedal-60)) 
    {
      brakeworktimer++; 
    } 
    else
    {
      brakeworktimer = 0;
      preSpeed = 0;
    }
    
    //时间到达3秒或者车速降为0都计算变化率
    if((brakeworktimer>0) ||(Motorstep_per_sec_count==0 && preSpeed!=0)) 
    {
      temp = (FP32)(preSpeed - Motorstep_per_sec_count)/brakeworktimer;
      temp = temp*100;
      gCarInfor.speedRate = (uint8)temp;
      //AlarmVoice(NUMBER_1_VOICE) ; 
      if(gCarInfor.speedRate<=120&&gCarInfor.speedRate>=90) 
      {
         CalibraMaxpedal();
      }
      brakeworktimer = 0;  
      preSpeed = 0;   
    } 
    //如果大于10秒 则不使用本次数据
    if(brakeworktimer>100)
    {
      brakeworktimer = 0;  
      preSpeed = 0;
    }
    
  }
  
  prePedalPos = gCarInfor.accelPedalPos;    
}
#endif


uint16 PadelMaxtemp[6];
void CalibraMaxpedal() 
{
      static uint16 RecordMaxpadel=0;
      //static uint16 PadelMaxtemp[10];
      static uint8 ADIndex=0;
     
      //AlarmVoice(NUMBER_3_VOICE);
      if((gCarInfor.accelPedalPos<gCarPara.maxAccelPedal+40)&&(gCarInfor.accelPedalPos>gCarPara.maxAccelPedal-40))
      {
        //AlarmVoice(NUMBER_2_VOICE) ;
        PadelMaxtemp[ADIndex]= gCarInfor.accelPedalPos; 
      }
      
      if(ADIndex>=5) 
      {
        ADIndex= 0;
      }
      else
      {
        ADIndex++;
      }
       RecordMaxpadel= SignalFilter(&PadelMaxtemp,6);

       if(RecordMaxpadel>=600&&RecordMaxpadel<=850) 
       {
         if((gCarPara.maxAccelPedal<=RecordMaxpadel+30)&&(gCarPara.maxAccelPedal>=RecordMaxpadel-30)) 
         {
             gCarPara.maxAccelPedal= RecordMaxpadel;
             gCarParaEepromUpdateFlag = 1;
         } 
         else
         {
             gCarPara.maxAccelPedal= RecordMaxpadel;
         }
        
       }
}