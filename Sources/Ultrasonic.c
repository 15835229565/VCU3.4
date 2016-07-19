//#include "Ultrasonic.h"
#include "includes.h"


//每50ms调用一次，采集最近STATIS_NUM次雷达数据做统计

uint16 FrontLeftRadar[STATIS_NUM];
uint16 FrontRightRadar[STATIS_NUM];
uint16 RearLeftRadar[STATIS_NUM];
uint16 RearRightRadar[STATIS_NUM];
uint8 ChangeData[8];
uint8  gFrontBarriArea=FD_AREA;
uint8  gRearBarriArea=RD_AREA;
uint8  radardataindex=0;
  

void EffectiveObstaclesDataCompute(uint8 *data) 
{
   uint16 Anticollisiondata0,Anticollisiondata1,Anticollisiondata2,Anticollisiondata3;
   uint16 FrontCrossRange=0xffff;
   static uint8 FrontCrossRangeFlag=0,FrontCrossRangeNum=0,CrossInA_Araeflag=0;
     
   Anticollisiondata1=(uint16)(data[2]<<8);
   Anticollisiondata1+=data[3];
   
   Anticollisiondata2=(uint16)(data[4]<<8);
   Anticollisiondata2+=data[5];
   
   Anticollisiondata0=(uint16)(data[0]<<8);
   Anticollisiondata0+=data[1];
   
   Anticollisiondata3=(uint16)(data[6]<<8);
   Anticollisiondata3+=data[7];
   if(device.state.ReverseRelayState == 0)
   {
     if((Anticollisiondata1>=FA_AREA_DOUBLESTART && Anticollisiondata1<FD_AREA_END)&&(Anticollisiondata2>=FA_AREA_DOUBLESTART && Anticollisiondata2<FD_AREA_END))
     {
       AlarmVoice(NUMBER_1_VOICE);
       if(Anticollisiondata1>Anticollisiondata2)
       {
         FrontCrossRange=(Anticollisiondata1+Anticollisiondata2)*(Anticollisiondata1-Anticollisiondata2)/80;
       } 
       else if(Anticollisiondata1<Anticollisiondata2)
       {
         FrontCrossRange=(Anticollisiondata1+Anticollisiondata2)*(Anticollisiondata2-Anticollisiondata1)/80;
       }
        else 
       {
         FrontCrossRange=40;
       } 
     }
      else if((Anticollisiondata1>=FA_AREA_START && Anticollisiondata1<FA_AREA_DOUBLESTART)||(Anticollisiondata2>=FA_AREA_START && Anticollisiondata2<FA_AREA_DOUBLESTART))
     {          
        AlarmVoice(NUMBER_2_VOICE);
        
     
        ChangeData[0] = RxMsg.data[0];
        ChangeData[1] = RxMsg.data[1];    
        ChangeData[2] = RxMsg.data[2];    
        ChangeData[3] = RxMsg.data[3];

        ChangeData[4] = RxMsg.data[4];    
        ChangeData[5] = RxMsg.data[5];   
        ChangeData[6] = RxMsg.data[6];    
        ChangeData[7] = RxMsg.data[7]; 
        RadarAnticollision(ChangeData); 
        CrossInA_Araeflag=1;
     } 
     else
     {
        AlarmVoice(NUMBER_9_VOICE);
        gFrontBarriArea = FD_AREA;
        FrontCrossRange=0xffff;
     }
     if(FrontCrossRange<=40) 
     { 
        TxMsg.id=0x10;
         AlarmVoice(NUMBER_8_VOICE);
        ChangeData[0] = RxMsg.data[0];
        ChangeData[1] = RxMsg.data[1];    
        ChangeData[2] = RxMsg.data[2];    
        ChangeData[3] = RxMsg.data[3];

        ChangeData[4] = RxMsg.data[4];    
        ChangeData[5] = RxMsg.data[5];   
        ChangeData[6] = RxMsg.data[6];    
        ChangeData[7] = RxMsg.data[7]; 
        Can_Send(RxMsg);                   
        FrontAnticollosionManage(ChangeData);
        
        FrontCrossRangeFlag=1; 
        FrontCrossRangeNum=0;
        
     } 
     else if(FrontCrossRangeFlag==1&&FrontCrossRangeNum<=3) 
     {
        ChangeData[0] = RxMsg.data[0];
        ChangeData[1] = RxMsg.data[1];    
        ChangeData[2] = RxMsg.data[2];    
        ChangeData[3] = RxMsg.data[3];

        ChangeData[4] = RxMsg.data[4];    
        ChangeData[5] = RxMsg.data[5];   
        ChangeData[6] = RxMsg.data[6];    
        ChangeData[7] = RxMsg.data[7];                    
        FrontAnticollosionManage(ChangeData);
        FrontCrossRangeNum++;
        AlarmVoice(NUMBER_3_VOICE);
     }
     else
     {
       if(CrossInA_Araeflag==1) 
       {
          CrossInA_Araeflag=0;
       } 
       else
       {
          gFrontBarriArea = FD_AREA; 
           
            FrontCrossRangeFlag=0;
       }
       }
   }
    else
   {
        ChangeData[0]= RxMsg.data[0];
        ChangeData[1] = RxMsg.data[1];    
        ChangeData[2] = RxMsg.data[2];    
        ChangeData[3] = RxMsg.data[3];

        ChangeData[4] = RxMsg.data[4];    
        ChangeData[5] = RxMsg.data[5];   
        ChangeData[6] = RxMsg.data[6];    
        ChangeData[7] = RxMsg.data[7];                    
        RadarAnticollision(ChangeData); 
   }
}
void FrontAnticollosionManage(uint8 *data) 
{ 
     uint16 FrontLeftManage=0,FrontRightManage=0;
     FrontLeftManage=(uint16)(data[2]<<8);
     FrontLeftManage+=data[3];
     FrontRightManage=(uint16)(data[4]<<8);
     FrontRightManage+=data[5]; 
     if((FrontLeftManage>=FA_AREA_DOUBLESTART && FrontLeftManage<=FA_AREA_END)||(FrontRightManage>=FA_AREA_DOUBLESTART && FrontRightManage<=FA_AREA_END)) 
    {
      gFrontBarriArea = FA_AREA; 
      AlarmVoice(NUMBER_4_VOICE);
    } 
     if((FrontLeftManage>=FB_AREA_START && FrontLeftManage<=FB_AREA_END)||(FrontRightManage>=FB_AREA_START && FrontRightManage<=FB_AREA_END)) 
    {
        gFrontBarriArea = FB_AREA;
        AlarmVoice(NUMBER_5_VOICE); 
    } 
    
     if((FrontLeftManage>=FC_AREA_START && FrontLeftManage<=FC_AREA_END)||(FrontRightManage>=FC_AREA_START && FrontRightManage<=FC_AREA_END)) 
    {
       gFrontBarriArea = FC_AREA; 
       AlarmVoice(NUMBER_6_VOICE);
       
    } 
    
     if((FrontLeftManage>=FD_AREA_START && FrontLeftManage<=FD_AREA_END)||(FrontRightManage>=FD_AREA_START && FrontRightManage<=FD_AREA_END)) 
    {
      gFrontBarriArea = FD_AREA; 
      AlarmVoice(NUMBER_7_VOICE);
    }
}
void RadarAnticollision(uint8 *data) 
{   
     #if DEBUG_MODE==0 
     uint16 i,frontmax=0,frontmaxarea=0,rearmax=0,rearmaxarea=0,buf[4],frontmin=0,rearmin=0;
     uint8 frontstatis[4]={0,0,0,0};
     uint8 rearstatis[4]={0,0,0,0};
     uint8 frnc=0,flnc=0,rrnc=0,rlnc=0;
     static uint8 preFrontBarriArea=0;
     static uint8 voiceflag=0,voiceflag1=0,voiceflag2=0;
     sint8 mfarea_revise,mrarea_revise;
          
     FrontLeftRadar[radardataindex]=(uint16)(data[2]<<8);
     FrontLeftRadar[radardataindex]+=data[3];
     
     FrontRightRadar[radardataindex]=(uint16)(data[4]<<8);
     FrontRightRadar[radardataindex]+=data[5];
     
     RearLeftRadar[radardataindex]=(uint16)(data[0]<<8);
     RearLeftRadar[radardataindex]+=data[1];
     
     RearRightRadar[radardataindex]=(uint16)(data[6]<<8);
     RearRightRadar[radardataindex]+=data[7];

    // EffectiveObstaclesDataCompute(FrontLeftRadar,FrontRightRadar,radardataindex);
     radardataindex++;
    if(radardataindex>=STATIS_NUM) 
     {
        radardataindex=0;
        voiceflag = 1;
        voiceflag1 = 1;
        voiceflag2 = 1;
     } 
     
     //当车辆静止时，将探测区域整体后移，容易起步
     
     if(gCarInfor.speed==0) 
     {
        mfarea_revise = -1*FAREA_REVISE;
        mrarea_revise = -1*RAREA_REVISE;
     } 
     else 
     {
        mfarea_revise = 0;
        mrarea_revise = 0;
     }
     //超声波数据预处理 去除下雨等引入的干扰
     
     //去除雨水导致的雷达探测距离忽远忽近的问题
     //主要算法为统计最近几个点之间的距离变化之和来实现
     //当变化率和大于300时，则不采用这组数据
     frontmax = RadarAbsChangeRateCompute(FrontLeftRadar,STATIS_NUM,radardataindex);
     
    
     if(frontmax>5000) 
     {
        flnc = 1;
        if(voiceflag2==1) 
        {          
         //AlarmVoice(CONTROL_SPEED2_VOICE);
          voiceflag2 = 0;
        }
     } 
     else
     {
        flnc = 0;
     }  
     frontmax = RadarAbsChangeRateCompute(FrontRightRadar,STATIS_NUM,radardataindex);
     
     if(frontmax>5000) 
     {
        frnc = 1;
        if(voiceflag2==1) 
        {          
          //AlarmVoice(CONTROL_SPEED2_VOICE);
          voiceflag2 = 0;
        }
     } 
     else
     {
        frnc = 0;
     }
     
     frontmax = RadarAbsChangeRateCompute(RearLeftRadar,STATIS_NUM,radardataindex);
     if(frontmax>5000) 
     {
        rlnc = 1;
        if(voiceflag2==1) 
        {          
          //AlarmVoice(CONTROL_SPEED2_VOICE);
          voiceflag2 = 0;
        }
     } 
     else
     {
        rlnc = 0;
     }
     frontmax = RadarAbsChangeRateCompute(RearRightRadar,STATIS_NUM,radardataindex);
     if(frontmax>5000) 
     {
        rrnc = 1;
        if(voiceflag2==1) 
        {          
          //AlarmVoice(CONTROL_SPEED2_VOICE);
          voiceflag2 = 0;
        }
     } 
     else
     {
        rrnc = 0;
     }
      
     
     //这部分算法主要是弥补上述算法的不足，比如当变化率总和满足要求
     //但是数据中只是在505中只跳变了一次到20，然后再跳跃回来
     
     frontmax = 0;
     frontmin = 0;
     
     
     for(i=0;i<STATIS_NUM;i++) 
     {
        if(FrontLeftRadar[i]>=500) 
        {
           frontmax++;
        }
        
        if(FrontLeftRadar[i]<=FA_AREA_END) 
        {
           frontmin++;
        }
     }
     
     if((frontmax+frontmin)>=STATIS_NUM*0.75 && (frontmax>=(STATIS_NUM>>2))&&(frontmin<=(STATIS_NUM>>1))&&(frontmin>0)) 
     {
        flnc = 1;
        if(voiceflag1==1) 
        {          
          //AlarmVoice(PAY_ATTENTION_VOICE);
          voiceflag1 = 0;
        }
     } 
     else
     {
        //flnc = 0;             
     }
     
     frontmax = 0;
     frontmin = 0;
     for(i=0;i<STATIS_NUM;i++) 
     {
        if(FrontRightRadar[i]>=500) 
        {
           frontmax++;
        }
        
        if(FrontRightRadar[i]<=FA_AREA_END) 
        {
           frontmin++;
        }
     }
     //只有20和505 说明是错误数据
     if((frontmax+frontmin)>=STATIS_NUM*0.75 && (frontmax>=(STATIS_NUM>>2))&&(frontmin<=(STATIS_NUM>>1))&&(frontmin>0)) 
     {
        frnc = 1;
       
        if(voiceflag1==1) 
        {          
          //AlarmVoice(PAY_ATTENTION_VOICE);
          voiceflag1 = 0;
        }
     } 
     else
     {
        //frnc = 0;             
     }
     
     
     frontmax = 0;
     frontmin = 0;
     for(i=0;i<STATIS_NUM;i++) 
     {
        if(RearLeftRadar[i]>=500) 
        {
           frontmax++;
        }
        
        if(RearLeftRadar[i]<=RA_AREA_END) 
        {
           frontmin++;
        }
     }
     
     if((frontmax+frontmin)>=STATIS_NUM*0.75 && (frontmax>=(STATIS_NUM>>2))&&(frontmin<=(STATIS_NUM>>1))&&(frontmin>0)) 
     {
        rlnc = 1;
        if(voiceflag1==1) 
        {          
          //AlarmVoice(PAY_ATTENTION_VOICE);
          voiceflag1 = 0;
        }
     } 
     else
     {
        //rlnc = 0;             
     }                    
     
     
     frontmax = 0;
     frontmin = 0;
     for(i=0;i<STATIS_NUM;i++) 
     {
        if(RearRightRadar[i]>=500) 
        {
           frontmax++;
        }
        
        if(RearRightRadar[i]<=RA_AREA_END) 
        {
           frontmin++;
        }
     }
     
     if((frontmax+frontmin)>=STATIS_NUM*0.75 && (frontmax>=(STATIS_NUM>>2))&&(frontmin<=(STATIS_NUM>>1))&&(frontmin>0)) 
     {
        rrnc = 1;
        if(voiceflag1==1) 
        {          
          //AlarmVoice(PAY_ATTENTION_VOICE);
          voiceflag1 = 0;
        }
     } 
     else
     {
        rrnc = 0;             
     }

     
     
     for(i=0;i<STATIS_NUM;i++) 
     {
        //统计前向左侧雷达数据
        if(flnc==0) 
        {          
          if(FrontLeftRadar[i]>=FA_AREA_START && FrontLeftRadar[i]<=(FA_AREA_END+mfarea_revise)) 
          {
            frontstatis[FA_AREA]++;
          } 
          if(FrontLeftRadar[i]>=(FB_AREA_START+mfarea_revise) && FrontLeftRadar[i]<=(FB_AREA_END+mfarea_revise)) 
          {
            frontstatis[FB_AREA]++;
          } 
          
          if(FrontLeftRadar[i]>=(FC_AREA_START+mfarea_revise) && FrontLeftRadar[i]<=(FC_AREA_END+mfarea_revise)) 
          {
            frontstatis[FC_AREA]++;
          } 
          
          if(FrontLeftRadar[i]>=(FD_AREA_START+mfarea_revise) && FrontLeftRadar[i]<=(FD_AREA_END+mfarea_revise)) 
          {
            frontstatis[FD_AREA]++;
          }
        }
        //统计前向右侧雷达数据
        
        if(frnc==0) 
        {
          if(FrontRightRadar[i]>=FA_AREA_START && FrontRightRadar[i]<=(FA_AREA_END+mfarea_revise)) 
          {
            frontstatis[FA_AREA]++;
          } 
          if(FrontRightRadar[i]>=(FB_AREA_START+mfarea_revise) && FrontRightRadar[i]<=(FB_AREA_END+mfarea_revise)) 
          {
            frontstatis[FB_AREA]++;
          } 
          
          if(FrontRightRadar[i]>=(FC_AREA_START+mfarea_revise) && FrontRightRadar[i]<=(FC_AREA_END+mfarea_revise)) 
          {
            frontstatis[FC_AREA]++;
          } 
          
          if(FrontRightRadar[i]>=(FD_AREA_START+mfarea_revise) && FrontRightRadar[i]<=(FD_AREA_END+mfarea_revise)) 
          {
            frontstatis[FD_AREA]++;
          }
        }
        
        //统计后向左侧雷达数据
        if(rlnc==0) 
        {
          if(RearLeftRadar[i]>=(RA_AREA_START) && RearLeftRadar[i]<=(RA_AREA_END+mrarea_revise)) 
          {
            rearstatis[RA_AREA]++;
          } 
          if(RearLeftRadar[i]>=(RB_AREA_START+mrarea_revise) && RearLeftRadar[i]<=(RB_AREA_END+mrarea_revise)) 
          {
            rearstatis[RB_AREA]++;
          } 
          
          if(RearLeftRadar[i]>=(RC_AREA_START+mrarea_revise) && RearLeftRadar[i]<=(RC_AREA_END+mrarea_revise)) 
          {
            rearstatis[RC_AREA]++;
          } 
          
          if(RearLeftRadar[i]>=(RD_AREA_START+mrarea_revise) && RearLeftRadar[i]<=(RD_AREA_END+mrarea_revise)) 
          {
            rearstatis[RD_AREA]++;
          } 
        }
        //统计后向右侧雷达数据
        
        if(rrnc==0) 
        {
          if(RearRightRadar[i]>=RA_AREA_START && RearRightRadar[i]<=(RA_AREA_END+mrarea_revise)) 
          {
            rearstatis[RA_AREA]++;
          } 
          if(RearRightRadar[i]>=(RB_AREA_START+mrarea_revise) && RearRightRadar[i]<=(RB_AREA_END+mrarea_revise)) 
          {
            rearstatis[RB_AREA]++;
          } 
          
          if(RearRightRadar[i]>=(RC_AREA_START+mrarea_revise) && RearRightRadar[i]<=(RC_AREA_END+mrarea_revise)) 
          {
            rearstatis[RC_AREA]++;
          } 
          
          if(RearRightRadar[i]>=(RD_AREA_START+mrarea_revise) && RearRightRadar[i]<=(RD_AREA_END+mrarea_revise)) 
          {
            rearstatis[RD_AREA]++;
          } 
        } 
     } 
     
    
     frontmax = 1;
     frontmaxarea = FD_AREA;
     rearmax = 1;
     rearmaxarea = RD_AREA;
          
     
     //统计A B C三个区域的值
     for(i=0;i<3;i++) 
     {
       if(frontstatis[i]>frontmax) 
       {
          frontmax = frontstatis[i];
          frontmaxarea = i;
       }
       
       if(rearstatis[i]>rearmax) 
       {
          rearmax = rearstatis[i];
          rearmaxarea = i;
       }
     }
     preFrontBarriArea = gFrontBarriArea;
     
     if(frontstatis[FD_AREA]>(STATIS_NUM*2-1)) 
     {
       gFrontBarriArea = FD_AREA;
     } 
     else
     {      
      gFrontBarriArea = frontmaxarea;
     }
     
     
     
     //当B和D区域转换时，先经过一个C区域，否则容易有顿挫感
     if(preFrontBarriArea==FB_AREA && gFrontBarriArea==FD_AREA && gCarInfor.speed<=8) 
     {
       gFrontBarriArea = FC_AREA; 
     }
     
     if(preFrontBarriArea==FD_AREA && gFrontBarriArea==FB_AREA && gCarInfor.speed>8) 
     {
       gFrontBarriArea = FC_AREA; 
     }
     
     //当车速大于10公里时，超声波探头不进行判断
     if(gCarInfor.speed>10) 
     {
       gFrontBarriArea = FD_AREA; 
     }
     
     if(preFrontBarriArea!=FA_AREA && gFrontBarriArea == FA_AREA) 
     {
        gCarInfor.RadarCollisionCounter++;
     }
     
     
     
     if(rearstatis[RD_AREA]>(STATIS_NUM*2-1)) 
     {
       gRearBarriArea = RD_AREA;
     } 
     else
     {      
       gRearBarriArea = rearmaxarea;      
     }
     
     
      //蓝牙租车条件下，前置超声波不启用
      if(gCarInfor.state == BTRenting)
      {                                  
         gFrontBarriArea = FD_AREA; 
      }
     
     #endif          
}


//计算雷达数组中相邻两个点之间的距离变化率
uint16  RadarAbsChangeRateCompute(uint16* data,uint8 datalen,uint8 startindex) 
{
    uint16 changeratesum=0;
    uint8 i;
    uint8 m,n;
    
    
    for(i=0;i<datalen-1;i++) 
    {
      m = (startindex+i)%datalen;
      n = (startindex+i+1)%datalen;
      if(data[m]>=data[n])
      {
         changeratesum +=(data[m] - data[n]);
      }
      else
      {
         changeratesum +=(data[n] - data[m]);
      }
       
    }
    
    return changeratesum;
}

