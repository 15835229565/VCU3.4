#include "includes.h"

// 0~10    ɲ����
//11~25    ����
//26~65    ������
//66~80    ������
//81~90    ����
//91~100   ɲ����
uint8  pedalAreaBoundPer[5]={10,25,65,85,90}; //������̤�������Ӧ�İٷֱ�25    65
uint16 deststep[100]=0;                       //Ŀ��������
uint16 basicDuty[100]=0;                      //����dutyֵ

uint16 Duty = 500;                            //PWMռ�ձȣ�ֵԽС�ٶ�Խ��

sint8 uphill_alpha[4]={0,6,10,14};             //�����¶�
sint8 flat_alpha[5]={2,-2,-6,-10,-14};         //�������¶�

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
/*��ͬ����������������VCU�汾����ʵ�ʱ궨�����������������*/
/*��duty�Ĳ�ͬ���㹫ʽ��                                     */
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
   FP32 duty0[2];//�ڼ�������Ŀ���ٶȺ�ռ�ձ�
   FP32 dest[2];   
   
   if(maxspeed==preMaxSpeed)  //�ж�����ٶ��Ƿ�仯
   {   
    return;                       
   } 
   preMaxSpeed=maxspeed;
     
   for(j=0;j<pedalAreaBoundPer[0];j++)                     //ɲ����
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
   
   for(j=pedalAreaBoundPer[0];j<pedalAreaBoundPer[1];j++)  //���� 10-25
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
   
   for( j=pedalAreaBoundPer[1]; j<pedalAreaBoundPer[2]; j++) //������ 25-65
   {       
       basicDuty[j]=duty0[0]-(duty0[0]-duty0[1])*(j-pedalAreaBoundPer[1])/(pedalAreaBoundPer[2]-pedalAreaBoundPer[1]);
       deststep[j]=(dest[1]-dest[0])*(j-pedalAreaBoundPer[1])/(pedalAreaBoundPer[2]-pedalAreaBoundPer[1])+dest[0];
       
   }
   
   
   for(j=pedalAreaBoundPer[2];j<pedalAreaBoundPer[3];j++)    //������ 65-89
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
   for(j=pedalAreaBoundPer[3];j<pedalAreaBoundPer[4];j++)    //���� 89-90
   {
       basicDuty[j]=duty0[0];
       deststep[j]=dest[0];
   }
   // yzl
   for(j=pedalAreaBoundPer[4];j<100;j++)           //ɲ����  90-100
   {
       basicDuty[j]=getDestDuty(0);
       deststep[j]=0;
   }

   #endif 
}

 void MotorInit()
 {   
    FP32 A,B;     //0.04158
    A= (gCarPara.hallSensorPara*gCarPara.gearRatio)*1000;    //���ٱȸ�Ϊ����ֱ�ӱ���
    B= (gCarPara.tireDiameter*3.14);  //8.5--0.02585--1.6085                       //��̥�ܳ�
    
    //31706                                   
    if(A!=0 && B!=0) 
    {                
      gMotorInfor.MetrePerPulse = B/A;                                 //һ�����������      
    }
    
    Duty = getDestDuty(0); 
} 

uint16 MotorCtrlPID()
{    
    static uint8 feedback_time=0;//����ʱ��    
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
         
         //Ŀ��ת�ٽϵ�����£�������ֵ�жϣ���Ҫ�ʺϵ���ʱ�ķ���
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
                       dutytemp -=15; //��������ʱ��������
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


          //Ŀ��ת�ٽϵ�����£�������ֵ�жϣ���Ҫ�ʺϵ���ʱ�ķ���
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


          //Ŀ��ת�ٽϵ�����£�������ֵ�жϣ���Ҫ�ʺϵ���ʱ�ķ���
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
                       //�෭��־
///////////�𲽳���////////////////////////
void CarStartingMode()
{   
    #if DEBUG_MODE==0                 
     
    if(device.state.ReverseRelayState == 1) ///������־Ϊ 1   0504yzl����������𲽹���
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
        if((gCarInfor.fabAngle>=uphill_alpha[0]) && (uphill_alpha[1]>gCarInfor.fabAngle))  //��0~5��֮��
         {
                    //Duty=getDestDuty(8);
                    Duty=getDestDuty(5);
         }
        else if((gCarInfor.fabAngle>uphill_alpha[1])&& (uphill_alpha[2]>gCarInfor.fabAngle)) //��5-9��֮��  
         {
                   //Duty=getDestDuty(9);
                   Duty=getDestDuty(6);
         }
        else     //��9-13��֮��
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
            if((flat_alpha[0]>=gCarInfor.fabAngle)&& (gCarInfor.fabAngle>flat_alpha[1]))  //����-1~2��������ƽ������   
            {              
               // Duty=getDestDuty(5); 
                Duty=getDestDuty(5);          
            }
            else if ((flat_alpha[1]>=gCarInfor.fabAngle)&& (gCarInfor.fabAngle>=flat_alpha[4]))//-2~-14��֮��Ϊ��������
            {
                 uphill_flag=1;
                 if((flat_alpha[1]>=gCarInfor.fabAngle) && (gCarInfor.fabAngle>flat_alpha[2]))//-2~-6��֮��  
                 {                  
                    Duty=getDestDuty(5); 
                 }
                 else  if ((flat_alpha[2]>=gCarInfor.fabAngle) && (gCarInfor.fabAngle>flat_alpha[3]))//-6~-10��֮��
                 {                  
                    Duty=getDestDuty(3); 
                 }
                 else //-10~-14��֮��
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
    else  if ((gCarInfor.fabAngle>uphill_alpha[3])||(flat_alpha[4]>gCarInfor.fabAngle))   //��13-90��֮��
    {
         Duty = getDestDuty(0);
    }

    pwm_regesiterset(Duty); 
    #endif
}     
////////////////////������ʻ����/////////////////

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
    //��߳��ٽ�������£�ֱ�Ӹ�dutyֵ�����п���
    if(gCarInfor.maxSpeed<=3)
    {
         Duty = getDestDuty(gCarInfor.maxSpeed);        
    }
    else 
    {
        if(device.state.ReverseRelayState == 1) ///������־Ϊ 1
        {                        
            //��������ٶȱ仯���������duty��������
           // MotorCtrlCurveCalc(1);
            //��������£��״��⵽A�������̤��λ��Ϊ��������ʻ������ͣ��
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
                       
            //��������ٶȱ仯���������duty��������
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
    //////////////////////////////////////�������ж� //////////////////////////////////////
    //   ���õ�λ���д�ȷ��:1.���ж�����  2.���ж�����
     //�����δ����ʱ�������𲽱�־λ
    if(Motorstep_per_sec_count==0) 
    {          
        //�����𲽱�־λ   
        
        if(gCarInfor.accPedal < pedalAreaBoundPer[0]>>1)  //ά��12*5ms=60ms��������
        {                
          if(start_counter<1000) 
          {            
            start_counter +=16;
          }
        } 
        else if(gCarInfor.accPedal < pedalAreaBoundPer[0])  //ά��25*5ms=75ms��������
        {                
          if(start_counter<1000) 
          {            
            start_counter +=8;
          }
        } 
        else if(gCarInfor.accPedal < pedalAreaBoundPer[1]) //ά��50*5ms=250ms��������
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
          //���綯������������ʱ�����ɲδ�򿪡������������𲽼���
          if(((gRearBarriArea != FA_AREA && device.state.ReverseRelayState == 1)||(gFrontBarriArea != FA_AREA && device.state.ReverseRelayState != 1))&& (start_counter>=200)&&(Motorstep_per_sec_count==0))   //�����һ��ת�����������������˳���
          {
              CarStartingMode(); 
          }
          else if (Motorstep_per_sec_count!=0) 
          {                             
              CarRunningMode(); //������ʻ����               
              start_counter = 0;                                                           
          } 
          else 
          {                                     
              ;//����ԭ��duty���
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


//������������£����ɲ���Ƴ���ÿ5����ִ��һ��
void MotorElecMagnetControl() 
{

  static uint8 uphill_time=0;
  static uint8 braketimer=0;
  
  //ͣ��״̬   
  if(gCarInfor.speed==0 || Motorstep_per_sec_count==0)
  {            
    //����״̬
    if(Duty>=getDestDuty(0)) 
    { 
      braketimer++;     
      if(braketimer>50) //�޳��٣����������£�250ms�������ɲ
      {        
        BrakeRelay = ELECMAGHOLD;        
      }
      
      ///////////////20160223�޸�ͣ��ʱ����������///////
    //��������̽�⵽A���������壬�ҳ��ٵ���5����ÿСʱ���������ɲ  
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
    //����״̬��
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
    }  //��״̬��
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
      //����״̬��
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
            if(uphill_time<60)//����ִ��ʱ����0.3S;
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
  }//��ʻ״̬ 
  else
  { 
    braketimer = 0;
    //��������̽�⵽A���������壬�ҳ��ٵ���5����ÿСʱ���������ɲ  
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
    //����״̬��
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

//����̤�崫����λ���ж���ɲ������£����ٵı仯���
//���͵���̨�����������з���
//�ú���100msִ��һ��
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
    //�жϳ�����ʼɲ��
    if((prePedalPos<gCarPara.maxAccelPedal-5) && (gCarInfor.accelPedalPos>=(gCarPara.maxAccelPedal-10)))
    {
       preSpeed = Motorstep_per_sec_count;
       brakeworktimer = 0;  
       gCarThreshold.brakeActiveCounter++;   //����ɲ������     
    }
    
    //�ж�̤�崦��ɲ����,ͬʱ��ʼ�ٶ���Ҫ����5����ÿСʱ
    if(gCarInfor.accelPedalPos>=(gCarPara.maxAccelPedal-60)) 
    {
      brakeworktimer++; 
    } 
    else
    {
      brakeworktimer = 0;
      preSpeed = 0;
    }
    
    //ʱ�䵽��3����߳��ٽ�Ϊ0������仯��
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
    //�������10�� ��ʹ�ñ�������
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