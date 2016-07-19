#include  "includes.h"

/*******************************车身状态相关参数定义*********************************/
union  devicestate device;



Bool SwitchChange[8]={0};
uint8 SwitchChangeFlag=0;                                  

GPIO_Port_Type GPIO_Port_Cfg[]=
{
    {
        GPIO_PORT_T,
        GPIO_PIN_7_MASK|GPIO_PIN_6_MASK|GPIO_PIN_3_MASK|GPIO_PIN_2_MASK|GPIO_PIN_1_MASK,
        GPIO_PIN_7_MASK|GPIO_PIN_1_MASK,
        0,0,0
    },
    {
        GPIO_PORT_S,
            GPIO_PIN_4_MASK|GPIO_PIN_5_MASK|GPIO_PIN_6_MASK|GPIO_PIN_7_MASK,
    //    GPIO_PIN_4_MASK|GPIO_PIN_5_MASK|GPIO_PIN_6_MASK|GPIO_PIN_7_MASK,
        0,0,0,0
    } ,
    {
        GPIO_PORT_P,
        GPIO_PIN_0_MASK|GPIO_PIN_1_MASK|GPIO_PIN_2_MASK| \
        GPIO_PIN_3_MASK|GPIO_PIN_4_MASK|GPIO_PIN_5_MASK|GPIO_PIN_6_MASK,
        0,0,0,0
    } ,
    {
        GPIO_PORT_J,
        GPIO_PIN_0_MASK|GPIO_PIN_1_MASK|GPIO_PIN_2_MASK|GPIO_PIN_3_MASK \
        |GPIO_PIN_4_MASK|GPIO_PIN_5_MASK|GPIO_PIN_7_MASK,
        0,0,0,0
    }  ,
    {
        GPIO_PORT_AD0,
        GPIO_PIN_7_MASK,
        GPIO_PIN_7_MASK,
        GPIO_PIN_7_MASK,
        0,
        0
    // GPIO_PIN_7_MASK,0,0,0,
    }  ,
    
    {
        GPIO_PORT_AD8,
        GPIO_PIN_4_MASK|GPIO_PIN_5_MASK|GPIO_PIN_3_MASK|GPIO_PIN_6_MASK|GPIO_PIN_7_MASK,
        GPIO_PIN_0_MASK,
        GPIO_PIN_0_MASK,
        GPIO_PIN_0_MASK|GPIO_PIN_4_MASK|GPIO_PIN_5_MASK|GPIO_PIN_3_MASK,
        GPIO_PIN_4_MASK|GPIO_PIN_6_MASK|GPIO_PIN_7_MASK,
    },
    
};

/********************************************/
/**********Port 配置列表**********/
/********************************************/
GPIO_Cfg_Type  GPIO_Cfg=
{
    6,
    GPIO_Port_Cfg,
};

void GPIO_init(GPIO_Cfg_Ref  GPIO_Ptr)
{
    uint8 i=0;
   
    if(NULL == GPIO_Ptr)
        GPIO_Ptr = &GPIO_Cfg;

    while(i<GPIO_Ptr->PortCount)
    {
        switch(GPIO_Ptr->GPIO_Port_Cfg[i].PortIndex)
        {
        case 1:
        {
            DDRT= GPIO_Ptr->GPIO_Port_Cfg[i].Direction;
            PERT = GPIO_Ptr->GPIO_Port_Cfg[i].PullEnable;
            PPST =  GPIO_Ptr->GPIO_Port_Cfg[i].PolaritySelect;
            break;
        }
        case 2:
        {
            DDRS= GPIO_Ptr->GPIO_Port_Cfg[i].Direction;
            PERS = GPIO_Ptr->GPIO_Port_Cfg[i].PullEnable;
            PPSS =  GPIO_Ptr->GPIO_Port_Cfg[i].PolaritySelect;
            break;
        }
        case 3:
        {
            DDRP= GPIO_Ptr->GPIO_Port_Cfg[i].Direction;
            PERP = GPIO_Ptr->GPIO_Port_Cfg[i].PullEnable;
            PPSP =  GPIO_Ptr->GPIO_Port_Cfg[i].PolaritySelect;
            break;
        }
        case 4:
        {
            DDRJ= GPIO_Ptr->GPIO_Port_Cfg[i].Direction;
            PERJ = GPIO_Ptr->GPIO_Port_Cfg[i].PullEnable;
            PPSJ = GPIO_Ptr->GPIO_Port_Cfg[i].PolaritySelect;
            break;
        }
        case 5:
        {
            DDR1AD= GPIO_Ptr->GPIO_Port_Cfg[i].Direction;
            PER1AD = GPIO_Ptr->GPIO_Port_Cfg[i].PullEnable;
            PPS1AD = GPIO_Ptr->GPIO_Port_Cfg[i].PolaritySelect;
            ATDDIENL=GPIO_Ptr->GPIO_Port_Cfg[i].InputEnable;
            break;
        }
        case 6:
        {
            DDR0AD= GPIO_Ptr->GPIO_Port_Cfg[i].Direction;
            PER0AD = GPIO_Ptr->GPIO_Port_Cfg[i].PullEnable;
            PPS0AD =  GPIO_Ptr->GPIO_Port_Cfg[i].PolaritySelect;
            ATDDIENH=GPIO_Ptr->GPIO_Port_Cfg[i].InputEnable;
           // PT0AD= GPIO_Ptr->GPIO_Port_Cfg[i].AD0Set;
            break;
        }
        default :
            break;
        }
        i++;
    }
}

void IO_Control()
{
    static uint8 switchON[6]={0,0,0,0,0,0};
    uint8 i=0;
    
          
    if(Read74HC151(0))//夜灯开关
    {
      if(switchON[0]<5)
      {
        switchON[0]++;
      }
    }
    else
    {
      if(switchON[0]>0)
      {
        switchON[0]--;
      }
    }
    
    
    
    
    if(switchON[0]>=4)
    {
        NightlampSwitch = 1;
        device.state.Nightlamp = 1;
    }
    else if(switchON[0]<=1)
    {
        NightlampSwitch = 0;
        device.state.Nightlamp = 0;
    }
    
    
    
    if(Read74HC151(1))//大灯开关
    {
      if(switchON[1]<5)
      {
        switchON[1]++;
      }
      
    }
    else
    {
      if(switchON[1]>0)
      {
        switchON[1]--;
      }
    }

    if(switchON[1]>=4)//大灯开关
    {
        HeadlampRelay = 1;
        device.state.Headlamp = 1;
    }
    else if(switchON[1]<=1)
    {
        HeadlampRelay = 0;
        device.state.Headlamp = 0;
    }
    
    if(Read74HC151(2))
    {
      if(switchON[2]<2)
      {
        switchON[2]++;
      }
      
    }
    else
    {
      if(switchON[2]>0)
      {
        switchON[2]--;
      }
    }
    

   if(switchON[2]>=2)//左转向灯开关    
    {
        
        LeftlampSwitch = 1;    
        device.state.Leftlamp = 1;
    }
    else if(switchON[2]<=1)
    {
        LeftlampSwitch = 0;
        device.state.Leftlamp = 0;
        
    }

    if(Read74HC151(3))
    {
      if(switchON[3]<2)
      {
        switchON[3]++;
      }
    }
    else
    {
      if(switchON[3]>0)
      {
        switchON[3]--;
      }
    }
    if(switchON[3]>=2)//右转向灯开关
    {
        
        RightlampSwitch = 1;
        device.state.Rightlamp= 1;
    }
    else if(switchON[3]<=1)
    {
        RightlampSwitch = 0;
        device.state.Rightlamp= 0;        
    }    

    if(Read74HC151(4))
    {
      if(switchON[4]<2)
      {
        switchON[4]++;
      }
    }
    else
    {
      if(switchON[4]>0)
      {
        switchON[4]--;
      }
    }

    if(switchON[4]>=2)//倒车按钮
    {      
        DaocheRelay = 1;
        gCarInfor.reversState = 1;
        device.state.ReverseRelayState = 1;
        
    }
    else if(switchON[4]<=1)
    {       
        DaocheRelay = 0;
        gCarInfor.reversState = 0;
        device.state.ReverseRelayState = 0;
        
    } 
   
    if(Read74HC151(5))
    {
      if(switchON[5]<5)
      {
        switchON[5]++;
      }
      
    }
    else
    {
      if(switchON[5]>0)
      {
        switchON[5]--;
      }
      
    }
    
     
    if(switchON[5]>=4)//远光灯
    {
        YuanguangRelay = 1;
        device.state.Wiper = 1;
        
    }
    else if(switchON[5]<=1)
    {
        YuanguangRelay = 0;
        device.state.Wiper = 0;
    } 
}

// 74HC151是8选1数据选择器.它系一个8线输入,单线输出的多路模拟开关.
uint8 Read74HC151(uint8 Dx)
{
    switch(Dx)
    {
    case 7:
        select81A = 1; // Set 74HC151 to read D7
        select81B = 1;
        select81C = 1;
        break;
    case 6:
        select81C = 1; // Set 74HC151 to read D6
        select81B = 1;
        select81A = 0;
        break;
    case 5:
        select81A = 1; // Set 74HC151 to read D5
        select81B = 0;
        select81C = 1;
        break;
    case 4:
        select81C = 1; // Set 74HC151 to read D4
        select81B = 0;
        select81A = 0;
        break;
    case 3:
        select81C = 0; // Set 74HC151 to read D3
        select81B = 1;
        select81A = 1;
        break;
    case 2:
        select81A = 0; // Set 74HC151 to read D2
        select81B = 1;
        select81C = 0;
        break;
    case 1:
        select81C = 0; // Set 74HC151 to read D1
        select81B = 0;
        select81A = 1;
        break;
    case 0:
        select81A = 0; // Set 74HC151 to read D0
        select81B = 0;
        select81C = 0;
        break;
    default:
        return 2;
    }
    return((Select81Output)?1:0);
}

void CoinSignalProcess() 
{
    //投币器电源控制，车辆状态处于待机或投币，
    //且币数不大于最大币数，且未被预约时，打开投币器电源
    
    if((gCarThreshold.operationMode == COIN_OPERATION || gCarThreshold.operationMode == COIN_AND_MOBILE_OPERATION ) && (GPRS_init_flag!=GPRS_CONNECTED)&&(gCarInfor.tripCoin<gCarThreshold.maxCoin)&&(gCarInfor.state==CarReady || gCarInfor.state==CoinRenting )) 
    {
        Slotmachine = 1;
        device.state.SlotmachineState = 1;
    }
    else//其余情况关闭投币器电源
    {
        device.state.SlotmachineState = 0;
        Slotmachine = 0;
    }

   
    
    
    //投币脉冲检测
    if(true == coin_plus_trig && device.state.SlotmachineState == 1)
    {                                                        
        gCarInfor.availableTime += (gCarThreshold.perCoinTime*60);        //投入的硬币总共可供使用小车的时间 秒为单位
        gCarInfor.availableMetre += gCarThreshold.perCoinMetre;	    //投入的硬币总共可供行驶的里程           
        coin_plus_trig = false;
        gCarInfor.maxSpeed=5; 
        gCarInfor.preMaxSpeed=gCarInfor.maxSpeed;           
        gCarInfor.tripCoin++;
       
        gCarThreshold.GPRS_Noconnect_Coinnum++;
        gCarThresholdEepromUpdateFlag=1;
    }
    
    //投币状态转换
    if(gCarInfor.availableTime!=0 && gCarInfor.availableMetre!=0&&gCarInfor.state==CarReady) 
     {            
        gCarInfor.destState = CoinRenting; 
        gCarInfor.destStateSource = StateFromCar;
        if(CarStateChange()==TRUE) 
        {
          gCarInfor.Event |= COIN_RENT_START_EVENT;
          gCarInfor.userid = 0;
        //  TripDataInit();
        }
     }
     
             
    if((gCarInfor.state==CoinRenting)&&((gCarInfor.availableMetre < gCarInfor.tripMileage) ||(gCarInfor.availableTime <gCarInfor.tripTime)))            
        //if((gCarInfor.state==CoinRenting)&&(gCarInfor.availableMetre < gCarInfor.tripMileage))            

    {                    
        gCarInfor.destState = CarReady;
        gCarInfor.destStateSource = StateFromCar; 
        if(CarStateChange()==TRUE) 
        {
          gCarInfor.Event |= COIN_RENT_END_EVENT;
          gCarInfor.userid = 0;            
        }
    } 
}

void BodyEquipControl() 
{
  
  
  if((VCUWorkMode==NORMAL_MODE)&&((gCarInfor.state == CarMaintain)||(gCarInfor.state == CoinRenting)||(gCarInfor.state == MobileRenting)|| (gCarInfor.state == BTRenting))) 
  {                      
      IO_Control();//车身器件控制     
  }
  else
  {   
      if(VCUWorkMode==NORMAL_MODE) 
      {
        
        NightlampSwitch = 0;
        device.state.Nightlamp = 0;
        HeadlampRelay = 0;
        device.state.Headlamp = 0;
        LeftlampSwitch = 0;
        device.state.Leftlamp = 0;
        RightlampSwitch = 0;
        device.state.Rightlamp= 0;
        YuanguangRelay = 0;
        device.state.Wiper = 0;
        HornSwitch = 0;
        DaocheRelay = 0;
        gCarInfor.reversState = 0;
        device.state.ReverseRelayState = 0; 
      } 
  }
  

  //刹车灯控制
  if(VCUWorkMode==NORMAL_MODE) 
  {    
    if(gCarInfor.accPedal<pedalAreaBoundPer[0]) 
    {     
       ReverselampSwitch = 1;          
    }
    else
    {
      ReverselampSwitch = 0;       
    }         
  }        
}

void CarStatusIndicate() 
{
  if(VCUWorkMode == NORMAL_MODE) 
  {    
    if(gCarInfor.state==CarMaintain && gCarInfor.userid ==0) 
    {
       //红灯 绿灯同时闪烁表示进入自维护状态
       if(DirectLEDRED==0) 
       {
          DirectLEDRED = 1;
          DirectLEDGREEN = 1;
       } 
       else
       {
          DirectLEDRED = 0;
          DirectLEDGREEN = 0;
       }
    }
    else 
    {
      
        //无网络，红灯闪
        if(GPRS_init_flag < GPRS_CONNECTED) 
        {
          DirectLEDRED  =~DirectLEDRED;
        }
        else
        {            
          DirectLEDRED  = 0;            
        }
        
        
        //租车状态下，绿灯亮
        if(gCarInfor.state==BTRenting || gCarInfor.state==CarOrder || gCarInfor.state==CoinRenting||gCarInfor.state==CarLock||(gCarInfor.state==MobileRenting&&gCarInfor.userid!=0)||(gCarInfor.state==CarMaintain && gCarInfor.userid!=0)) 
        {
          DirectLEDGREEN =0; 
        }
        else//非租车状态下
        {
          //无gps信号，绿灯闪
          if(GPS_init_flag < GPS_SUCC) 
          {
            DirectLEDGREEN =~DirectLEDGREEN;
          }
          else
          {
            DirectLEDGREEN = 1;
          }
        }
    }
  }
}


void TripInforCalc() 
{
  static uint32 preTripMileage=0;
  static uint8 tripMileageChange[2];
  static uint8 tripMileageChangeIndex=0;
  uint16 tripMileChangeSum;
  uint8 i;
  static uint8 j=0;
  
  
    
  if(gCarInfor.state == CoinRenting || gCarInfor.state == MobileRenting || (gCarInfor.state == BTRenting)) 
  {
    j++;
    if(j>=4)
    {
      gCarInfor.tripTime++;
      j=0;
    }
  }
  //每1秒计算一次里程增量，放到数组中                
  if(preTripMileage==0) 
  {
    preTripMileage = gCarInfor.tripMileage;
    gCarInfor.speed = 0;
    memset(tripMileageChange,0,2); 
    tripMileageChangeIndex = 0;          
  } 
  else                     
  {
    if(gCarInfor.tripMileage>=preTripMileage) 
    {                                    
      tripMileageChange[tripMileageChangeIndex++]= gCarInfor.tripMileage - preTripMileage;
      if(tripMileageChangeIndex==2) 
      {
        tripMileageChangeIndex = 0;
      }
    }                
    preTripMileage = gCarInfor.tripMileage;          
  }

  tripMileChangeSum = 0;
  //每1秒计算一次车速
  for(i=0;i<2;i++)
  {
     tripMileChangeSum+=tripMileageChange[i];
  }

  gCarInfor.speed = (FP32)tripMileChangeSum*0.72*5; 

  //租车条件下进行停车超时计时
  if(gCarInfor.speed < 0.1 && ((gCarInfor.state == BTRenting) || gCarInfor.state == CoinRenting || gCarInfor.state == MobileRenting || gCarInfor.state == CarLock))
  {                                                                                       
      gCarInfor.tripStopOverTimer+=1;                
      gCarInfor.tripStopTime+=1;                   //记录租车时停车时间      
  }
  else
  {
      gCarInfor.tripStopOverTimer=0;                 
  }
 
        
   #if DEBUG_MODE==1 
  //   gCarInfor.speed = 10;
   #endif
}









