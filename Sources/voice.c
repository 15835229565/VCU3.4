#include  "includes.h"

uint8 VoiceBusyLevel=0;//0表示低有效 1表示高有效

void VoiceQueueInit() 
{
   memset(gCarInfor.VoiceQueue,0xFF,8);
   gCarInfor.VoiceQueueStart = 0;
   gCarInfor.VoiceQueueEnd = 0;
   gCarInfor.VoicePlayFlag = PLAY_END;
}

uint8 VoiceQueueAdd(uint8 vnum1,uint8 vnum2)
{
  if(vnum1!=0xFF && vnum2!=0xFF) 
  {    
    //如果队列中已经存在该语音，则不添加
    if(SearchVoiceQueue(vnum1)||SearchVoiceQueue(vnum2)) 
    {
      return 0;
    }
    if(gCarInfor.VoiceQueue[gCarInfor.VoiceQueueEnd%8]==0xFF && gCarInfor.VoiceQueue[(gCarInfor.VoiceQueueEnd+1)%8]==0xFF) 
    {
      gCarInfor.VoiceQueue[gCarInfor.VoiceQueueEnd%8] = vnum1;
      gCarInfor.VoiceQueue[(gCarInfor.VoiceQueueEnd+1)%8] = vnum2;
      gCarInfor.VoiceQueueEnd = (gCarInfor.VoiceQueueEnd+2)%8;
      return 1;
    } 
    else
    {
      return 0;
    }
  }
  else if(vnum1!=0xFF && vnum2==0xFF) 
  {
    if(SearchVoiceQueue(vnum1)) 
    {
      return 0;
    }
    
    if(gCarInfor.VoiceQueue[gCarInfor.VoiceQueueEnd%8]==0xFF) 
    {
      gCarInfor.VoiceQueue[gCarInfor.VoiceQueueEnd%8] = vnum1;
      gCarInfor.VoiceQueueEnd = (gCarInfor.VoiceQueueEnd+1)%8;
      return 1;
    } 
    else
    {
      return 0;
    }
  } 
  else if(vnum1==0xFF && vnum2!=0xFF) 
  {
    if(SearchVoiceQueue(vnum2)) 
    {
      return 0;
    }
    if(gCarInfor.VoiceQueue[gCarInfor.VoiceQueueEnd%8]==0xFF) 
    {
      gCarInfor.VoiceQueue[gCarInfor.VoiceQueueEnd%8] = vnum2;
      gCarInfor.VoiceQueueEnd = (gCarInfor.VoiceQueueEnd+1)%8;
      return 1;
    } 
    else
    {
      return 0;
    }
  } 
  else 
  {
    return 0;
  }
}
uint8 VoiceQueueGet()
{
  return gCarInfor.VoiceQueue[gCarInfor.VoiceQueueStart]; 
}

uint8 VoiceQueueSub()
{
  if(gCarInfor.VoiceQueue[gCarInfor.VoiceQueueStart]!=0xFF) 
  {
     gCarInfor.VoiceQueue[gCarInfor.VoiceQueueStart]=0xFF;
     gCarInfor.VoiceQueueStart = (gCarInfor.VoiceQueueStart+1)%8;
     return 1;
  }
  
  return 0;
}

uint8 SearchVoiceQueue(uint8 vnum)
{
   uint8 i;
   
   if(vnum==0)
   {
      return 0;
   }
   for(i=0;i<8;i++)
   {
     if(vnum==gCarInfor.VoiceQueue[i]) 
     {
        return 1;
     }
   }
   return 0;
}

void WT588D3LSend(uint8 data)
{
    uint8 i;
    WT588D_CS_PxOUT&=~WT588D_CS_PIN;
    delayms(6);//delayms(5);   //1010修改
    for(i=0; i<8; i++)
    {
        WT588D_CLK_PxOUT &=~WT588D_CLK_PIN;
        if((data&0x01)==0)
        {
            WT588D_DATA_PxOUT &=~WT588D_DATA_PIN;
        }
        else
        {
            WT588D_DATA_PxOUT |=WT588D_DATA_PIN;
        }
        delayus(350);
        WT588D_CLK_PxOUT |=WT588D_CLK_PIN;
        delayus(350);
        data=data>>1;
    }
    delayms(30);  //delayms(20);   //1010修改
    WT588D_DATA_PxOUT |=WT588D_DATA_PIN;
    WT588D_CS_PxOUT |=  WT588D_CS_PIN;
    WT588D_CLK_PxOUT |=WT588D_CLK_PIN ;
    //delayms(100);  
}

void HighestVolume()
{       
    AlarmVoice(0xE7);
}

void MiddleVolume()
{
    
    AlarmVoice(0xE4);
}

void LowestVolume()
{    
    AlarmVoice(0xE1);
}


void  MakeSound(uint8 num)
{

    WT588D3LSend(num);

}

uint8 WT588BUSY()
{      
    uint8 i;
    uint8 lowcounter,highcounter;
    lowcounter = 0;
    highcounter = 0;
     for(i=0;i<10;i++) 
     {
       delayus(100);
       if(WT588D_BUSY_PIN==0)
       {
        lowcounter++;
       } 
       else
       {
        highcounter++;
       }
       FEEDWATCHDOG = ~FEEDWATCHDOG;    
     }   
    
    //低电平表示忙状态    
    if(VoiceBusyLevel==0) 
    {               
              
       if(lowcounter>3)        
       {
         return 1;
       } 
       else
       {
         return 0;
       }
    } 
    //高电平表示忙状态    
    else    
    {      
       if(highcounter>3)
       {
         return 1;
       } 
       else
       {
         return 0;
       }
    } 
    
}

void WT588BusyValidVolt()
{
    
    uint8 i,levelL;
    
    levelL = 0;    
   
    for(i=0;i<8;i++) 
    {
      if(WT588D_BUSY_PIN==0) 
      {
         levelL++;
      } 
     
      delayus(100);
    }
    
    if(levelL>4) 
    {
       VoiceBusyLevel = 1;
    } 
    else
    {
       VoiceBusyLevel = 0;
    }    
}


uint8 AlarmVoice(uint8 id)
{
    return VoiceQueueAdd(id,0xFF);   
}

uint8 AlarmTwoVoice(uint8 id1,uint8 id2)
{
    return VoiceQueueAdd(id1,id2);   
}


void VoiceProcess() 
{
    static uint8 preReversState=0,preLeftlampState=0,preRightlampState=0; 
    static uint8 reversCounter=0,leftLampCounter=0,rightLampCounter=0; 

    if(device.state.Rightlamp == 1)
    {
        
        if(rightLampCounter==0) 
        {              
          AlarmTwoVoice(TURN_RIGHT_VOICE,PAY_ATTENTION_VOICE);
        }
        
        if(rightLampCounter<30) 
        {              
          rightLampCounter++;
        } 
        else
        {
          rightLampCounter = 0;
        }
        
        preRightlampState = 1;  
    } 
    else
    {
        if(VoiceQueueGet()==TURN_RIGHT_VOICE)
        {
          VoiceQueueSub();
          VoiceQueueSub();
        }
        if(preRightlampState>0)
        {          
          WT588D3LSend(STOP_VOICE);
          preRightlampState--;
        }
        rightLampCounter = 0;
    }
    
    if(device.state.Leftlamp == 1) 
    {                        
        if(leftLampCounter==0) 
        {              
          AlarmTwoVoice(TURN_LEFT_VOICE,PAY_ATTENTION_VOICE);
        }
        
        if(leftLampCounter<30) 
        {              
          leftLampCounter++;
        } 
        else
        {
          leftLampCounter = 0;
        }

        preLeftlampState = 1;
        
    } 
    else
    {
        if(VoiceQueueGet()==TURN_LEFT_VOICE)
        {
          VoiceQueueSub();
          VoiceQueueSub();
        }
        if(preLeftlampState>0)
        {
          
          WT588D3LSend(STOP_VOICE);
          preLeftlampState--;
        }
        leftLampCounter = 0;
    }
    
    if(device.state.ReverseRelayState == 1) 
    {
        if(reversCounter==0) 
        {              
          AlarmTwoVoice(BACKUP1_VOICE,PAY_ATTENTION_VOICE); 
        }
        
        if(reversCounter<30) 
        {              
          reversCounter++;
        } 
        else
        {
          reversCounter = 0;
        }
        
        preReversState = 1;
        
    } 
    else
    {
        if(preReversState>0)
        {
          if(VoiceQueueGet()==BACKUP1_VOICE)
          {
            VoiceQueueSub();
            VoiceQueueSub();
          }
          WT588D3LSend(STOP_VOICE);
          
          preReversState--;
        }
        reversCounter = 0;
    }
    
    
    switch(gCarInfor.VoicePlayFlag)
    {
      case PLAY_START:
      if(WT588BUSY()==1)
      {
        gCarInfor.VoicePlayFlag = PLAY_NOW; 
        VoiceQueueSub();
      }
      else
      {                        
        gCarInfor.VoicePlayFlag = PLAY_END; 
      }
      
      break;
      case PLAY_NOW:
      if(WT588BUSY()==0)
      {
        gCarInfor.VoicePlayFlag = PLAY_END; 
      } 
     
      break;
      case PLAY_END:
       if(VoiceQueueGet()!=0xFF)
       {
         
         if(device.state.Leftlamp==0 )
         {
           if(VoiceQueueGet()==TURN_LEFT_VOICE) 
           {
              VoiceQueueSub();
              VoiceQueueSub();
              break;
           }
           
           
         }
         
         if(device.state.Rightlamp==0)
         {
           if(VoiceQueueGet()==TURN_RIGHT_VOICE) 
           {
              VoiceQueueSub();
              VoiceQueueSub();
              break;
           }
           
           
         }             
         if(device.state.ReverseRelayState==0)
         {
           if(VoiceQueueGet()==BACKUP1_VOICE) 
           {
              VoiceQueueSub();
              VoiceQueueSub();
              break;
           }
           
           
         }
         
         //为培训用户语音处理一下
          if((gCarInfor.Event & TEACH_VOICE_EVENT) == TEACH_VOICE_EVENT) 
          {
            
            if(FIRST_OP_VOICE==VoiceQueueGet()) 
            {
               gCarInfor.userid = 0;                                                         
            } 
            else 
            {                                      
               gCarInfor.userid = userIdFromServer;
               gCarInfor.Event &= ~TEACH_VOICE_EVENT;                   
            }
          }
         
         MakeSound(VoiceQueueGet());             
         if(VoiceQueueGet()>0xDF) 
         {
           VoiceQueueSub();
           gCarInfor.VoicePlayFlag = PLAY_END; 
         } 
         else
         {              
          gCarInfor.VoicePlayFlag = PLAY_START; 
         }
         
       }
      break;
    }
}

void WelcomeVoice() 
{
  
  VoiceQueueInit();
  
  //让语音芯片停止工作，以便进行busy管脚有效性的判断
  MakeSound(STOP_VOICE);   
  //判断busy管脚是高有效还是低有效
  WT588BusyValidVolt();
  
  //根据模式设置音量
  if(VCUWorkMode == NORMAL_MODE)
    MiddleVolume();
  else
    HighestVolume();
 
  //连接测试服务器时，进行语音提醒
  if(VCUWorkMode == NORMAL_MODE) 
  {
    if(gCarThreshold.severMode==DEBUG_SERVER) 
    {
      if(gCarPara.hardwareVersion==0x300) 
      {
        AlarmVoice(ENTER_TEST_MODE_VOICE);
      } 
      else if(gCarPara.hardwareVersion>=0x301) 
      {
        AlarmVoice(DEBUG_SERVER_VOICE);
      }
    }
    else if(gCarThreshold.severMode==RELEASE_SERVER) 
    {        
      if(gCarPara.hardwareVersion>=0x301) 
      {
        AlarmVoice(RELEASE_SERVER_VOICE);
      }
    }
  } 
  
  if(VCUWorkMode == TEST_MODE)
  { 
    AlarmVoice(PAY_ATTENTION_VOICE);
    FEEDWATCHDOG = ~FEEDWATCHDOG;
    AlarmVoice(ENTER_TEST_MODE_VOICE);
  }
}


