#include "includes.h"

uint8 GPRS_init_flag = 0;
uint8 GPS_init_flag = 0;

void GPRSConnecting() 
{
     //断电重启模块
    if(GPRS_init_flag == GPRS_RESTART )
    {
        GPRSPower = 0; 
        GPRSReset = 1;
        delayms(50);
        GPRSReset = 0;       
        gGprsInitialTimer = 0; 
        GPRS_init_flag = GPRS_POWEROFF;                              
    }
    
    if(gGprsInitialTimer>3 && GPRS_init_flag == GPRS_POWEROFF)     
    {                          
        
        GPRSPower = 1; 
        
      GPRS_init_flag = GPRS_POWERON;  
    }
              
    if(gGprsInitialTimer>6 && GPRS_init_flag == GPRS_POWERON)     
    {
        GPRSPower = 0; 
    }
    
    //查询gprs服务附着情况，如果已经附着，则直接进行gprs初始化
    if(GPRS_init_flag==GPRS_INITIAL) 
    {              
      if(TRUE==GPRS_SERVICE_ATTACH()) 
      {
         gGprsInitialTimer=36;
      }
        else
        {
           delayms(1000);
        }
    }
    //模块初始化 必须在call ready出现并且初始化时间超过35秒
    if(GPRS_init_flag==GPRS_INITIAL && gGprsInitialTimer>35) 
    {                  
      
      
      //初始化GPRS                
      if(Init_GPRS())
      { 
        GPRS_init_flag = GPRS_CONNECTTING; 
        GPRS_DVCFG(); 
                                              
      }               
      //初始化不通过则重新上电初始化 
      else 
      {                        
         GPRS_init_flag = GPRS_RESTART; 
         gGPRSConnectTimer = 0;
      }
      //主动测试GPRS
      if(ReadTestResult(GPRS_TEST_FLAG)) 
      {
        ClearTestResult(GPRS_TEST_FLAG);
      }
      gGprsInitialTimer = 0;                  
    }
                
    //与后台服务器建立连接
    if( GPRS_init_flag==GPRS_CONNECTTING)
    {                                 
        gCarThreshold.errCode = gCarThreshold.errCode & (~GPRS_DISCONN_EXCEPTION); 
        if(gCarInfor.state == CarMaintain && gCarInfor.userid==0) 
        {                  
          //有网络情况下，且不在GPS围栏外面，则回到待机状态
          if((gCarThreshold.errCode & OUT_GPS_FENCE_EXCEPTION) == 0) 
          {
            gCarInfor.destState = CarReady;
            gCarInfor.destStateSource = StateFromCar;
            CarStateChange(); 
          }
        }                                                                                                               
        //超时服务器未响应，则断开TCP\UDP连接，重新进行初始化
        if(FALSE==VcuConnectServerProcess(0x07)) 
        {                                    
          ;                                                   
        } 
        else 
        {                                     
          GPRS_init_flag=GPRS_CONNECTED;
          gGPRSConnectTimer = 0;                  
        
          gGPSLocationTimer = 0;  
          standby_freq_flag = 0;                                    
        }                                
    }
    
                
    //如果未接受到SYSSTART，则程序重启 
    if((GPRS_init_flag==GPRS_POWERON) && (gGprsInitialTimer>45)) 
    {
      GPRS_init_flag = GPRS_RESTART; 
      gGprsInitialTimer = 0;
    }
    
    
    #if DEBUG_MODE==1
    gCarThreshold.operationMode = COIN_AND_MOBILE_OPERATION;                         
    #endif
        
    
    //如果超过120秒连不上，则关闭PDP，然后重新初始化模块,不成功则再加60秒
    if(gGPRSConnectTimer<180 && gGPRSConnectTimer>120 && GPRS_init_flag>GPRS_INITIAL) 
    { 
        Close_GPRS();                   
        if(Init_GPRS())
        { 
          GPRS_init_flag = GPRS_CONNECTTING;                  
        } 
        else
        {     
          gGPRSConnectTimer += 60;                
        }
    }
    
    //后台服务器网络连接超时 
    if(gGPRSConnectTimer>gCarThreshold.maxConnectTimeOut && GPRS_init_flag>GPRS_INITIAL) 
    {           
        GPRS_init_flag = GPRS_RESTART; 
        gCarThreshold.errCode = gCarThreshold.errCode | GPRS_DISCONN_EXCEPTION;
                                        
        if(gCarInfor.state == CarMaintain && gCarInfor.userid!=0) 
        {
          ;
        } 
        else
        {;/*   
                                  
          gCarInfor.destState = CarMaintain;
          gCarInfor.destStateSource = StateFromCar;
          
          if(TRUE==CarStateChange()) 
          {                                         
            //网络通讯异常                                                                                             
            gCarInfor.userid = 0;
          } */          
        }                                                                                                                       
    }
    
                                            
    
    //定位超时则重启GPS模块
    if(gGPSLocationTimer>30*(1+gCarInfor.SatelliteNum)) 
    {                              
        Restart_GPS();   
        gGPSLocationTimer = 0; 
        GPS_init_flag=GPS_RESTART;
    } 
}



void CoinRentingProcess()  
{
    //投币用户租车命令            
    if((gCarInfor.Event&COIN_RENT_START_EVENT)==COIN_RENT_START_EVENT)
    {                    
        if(GPRS_init_flag==GPRS_CONNECTED) 
        {                    
          VcuConnectServerProcess(0x08);
        }               
        SpeedSensorCaptureInit();                
        gCarInfor.Event &=~COIN_RENT_START_EVENT;
        gCarInfor.Event &=~COIN_RENT_END_EVENT;
        #if DEBUG_MODE==1
        UART0_SendString("投币租车开始"); 
        #endif 
    }

    //投币用户还车命令
    if((gCarInfor.Event&COIN_RENT_END_EVENT)==COIN_RENT_END_EVENT)  //0429  ndy 为了解决当可用时间或可用里程到了后清空投币用户数据
    {
          if(GPRS_init_flag==GPRS_CONNECTED) 
          {                    
            VcuConnectServerProcess(0x09);
          }
          
          TripDataInit();                                         
          
          #if DEBUG_MODE==1
          UART0_SendString("投币租车结束");
          #endif
          
          gCarInfor.Event &=~COIN_RENT_START_EVENT;
          gCarInfor.Event &=~COIN_RENT_END_EVENT;
          gCarThresholdEepromUpdateFlag = 1;   //保存里程等信息
    }
}

void PhoneBTRentingProcess() 
{
   //处理手机蓝牙命令
    #if DEBUG_MODE==0
    if(PhoneCommand_flag==PROC_CMD)
    {
        PhoneBTCmdProcess(PhoneCommand);                    
        PhoneCommand_flag = WAIT_CMD;                   
    }
    #endif
    
    if((gCarInfor.Event&BT_RENT_START_EVENT)==BT_RENT_START_EVENT) 
    {          
        AlarmVoice(BT_STARTUP_CAR_VOICE);
        AlarmVoice(SUCC_VOICE); 
        gCarInfor.Event&=~BT_RENT_START_EVENT;
    }
    //4分钟未收到心跳回复包，则表示蓝牙连接断开，进行还车操作
    if((gCarInfor.BTTimeout>240) && (phoneVerifFlag==2) && (gCarInfor.state == CarMaintain))
    {
        gCarInfor.destState = CarReady; 
        gCarInfor.destStateSource = StateFromCar;                           
        CarStateChange();              
        AlarmVoice(BT_TEST_VOICE);
        AlarmVoice(FAILED_VOICE); 
        phoneVerifFlag = 0;
        gCarInfor.BTTimeout = 0;
    }  
}

void PhoneGPRSRentingProcess() 
{
    
    
    //处理来自服务器端的命令
    if(ServerCommand_flag==PROC_CMD)
    {
        ServerCmdProcess(ServerCommand);                    
        ServerCommand_flag = WAIT_CMD; 
        rent_freq_flag = 0;
        standby_freq_flag = 0;                  
    }
                                                  
    /***********************小车被租用时心跳数据发送**************************/
    if(rent_freq_flag == 1)
    {
        if((gCarInfor.state == BTRenting) || (gCarInfor.state == CarLock)||(gCarInfor.state == CoinRenting)||(gCarInfor.state == MobileRenting)
                ||(gCarInfor.state == CarMaintain))
        {
            VcuConnectServerProcess(0x00);  
            gCarInfor.VibrateLevel = 0;                      
        }
        rent_freq_flag = 0;
    }
    /***********************小车待机时心跳数据发送**************************/
    if(standby_freq_flag == 1)
    {
        if((gCarInfor.state == CarOrder)||(gCarInfor.state == CarReady))
        {
             VcuConnectServerProcess(0x11); 
             gCarInfor.VibrateLevel=0;                        
        }
        standby_freq_flag = 0;
        
    }
    
    

   

   
}

void GPSFenceProcess() 
{
    if(GPS_init_flag == GPS_SUCC)
    {                                
     
          //超出GPS围栏，设置异常 
          if(GPSfence())
          {                               
            gCarThreshold.errCode = gCarThreshold.errCode | OUT_GPS_FENCE_EXCEPTION;                                                                                                                            
          }
          else
          {   
            gCarThreshold.errCode = gCarThreshold.errCode &( ~OUT_GPS_FENCE_EXCEPTION);                                                                                                    
          }    
      }
                       
    
}

void ExceptionProcess()
{
  #if DEBUG_MODE == 0   
  //电池温度传感器未插提醒
  if(gCarInfor.batTemp==0xFF && VCUWorkMode == NORMAL_MODE) 
  {
     AlarmTwoVoice(PAY_ATTENTION_VOICE,TEMP_SENSEOR_FORGET_VOICE);           
  }        
                                       
  //电池温度超标，设置异常
  if(gCarInfor.batTemp!=0xFF) 
  {                  
    if(gCarInfor.batTemp>=gCarThreshold.batTempLevel)
    {
       gCarThreshold.errCode |= HIGH_TEMPATURE_EXCEPTION;                       
    } 
    else if(gCarInfor.batTemp+10<gCarThreshold.batTempLevel) 
    {
       gCarThreshold.errCode &= ~HIGH_TEMPATURE_EXCEPTION;
       
    } 
  }
    //电池容量过低，设置异常
   if(gCarInfor.batRest!=0 && (gCarInfor.batRest<=gCarThreshold.batLowLevel)) 
   {
       gCarThreshold.errCode |= LOW_BATTERY_EXCEPTION; 
    } 
    else if(gCarInfor.batRest>(gCarThreshold.batLowLevel+10)) 
    {
       gCarThreshold.errCode &= ~LOW_BATTERY_EXCEPTION;                   
    }     
    
   //电池温度过高语音提醒
   if((gCarThreshold.errCode & HIGH_TEMPATURE_EXCEPTION) == HIGH_TEMPATURE_EXCEPTION) 
   {
       if(gCarInfor.speed>1) 
       {
         AlarmVoice(OVERTEMP_BATTERY1_VOICE);
       }
   }
   
   
   //电池电量过低语音提醒
   if((gCarThreshold.errCode & LOW_BATTERY_EXCEPTION) == LOW_BATTERY_EXCEPTION) 
   {
       if(gCarInfor.speed>1) 
       {
         AlarmVoice(LOW_BATTERY1_VOICE);
       }
   }
   
   
   //回到围栏内部时，逐步提高车速
    if((gCarThreshold.errCode & OUT_GPS_FENCE_EXCEPTION) == 0) 
    {
        if(gCarInfor.maxSpeed<gCarInfor.preMaxSpeed) 
        {                        
         gCarInfor.maxSpeed++;         
        }
         //有网络情况下的，自维护状态，回到围栏内，则进入待机状态
        if(GPRS_init_flag==GPRS_CONNECTED && gCarInfor.state == CarMaintain && gCarInfor.userid==0) 
        {
          gCarInfor.destState = CarReady;
          gCarInfor.destStateSource = StateFromCar;
          CarStateChange();
        }  
    }
    else
    {        
        
        //从服务器读取GPS围栏权限
        if((gCarInfor.userAuthority & OUT_GPS_FENCE_AUTH_VALID)==0) 
        {                                                                                                     
            if(0x93==VcuConnectServerProcess(0x13)) 
            {
               gCarInfor.userAuthority |= OUT_GPS_FENCE_AUTH_VALID;
            }
        }
     
              
        //维护人员开车，具有超出GPS围栏的权限 
        if(gCarInfor.state == CarMaintain && gCarInfor.userid!=0) 
        {
          ;
        }         
        //非维护人员开车，则根据权限进行判断，语音提示超出GPS围栏
        else
        { 
        
            if((gCarInfor.userAuthority&OUT_GPS_FENCE_NOTALLOW_AUTH) == OUT_GPS_FENCE_NOTALLOW_AUTH) 
            {
                #if DEBUG_MODE==1
                UART0_SendString("超出围栏，及时停车"); 
                #endif 
                //非维护人员租车，该车辆不允许超过gps围栏，则还车，强制置为自维护状态                     
                //在进入维护状态前，进行语音提醒 ,并等待10秒后进行状态转换
                if(gCarInfor.speed>1) 
                {                        
                  AlarmVoice(OUT_FENCE1_VOICE);                                     
                  AlarmVoice(STOPPING_CAR1_VOICE); 
                  delayms(10000);  
                }                                               
                gCarInfor.state = CarMaintain;                                
                gCarInfor.userid = 0;
            } 
            //允许超出围栏运行，不发语音
            else if((gCarInfor.userAuthority&OUT_GPS_FENCE_AUTH) == OUT_GPS_FENCE_AUTH)
            {
              #if DEBUG_MODE==1
              UART0_SendString("允许出围栏，不降速"); 
              #endif                                  
            } 
            else if((gCarInfor.userAuthority&OUT_GPS_FENCE_LOWSPEED_AUTH) == OUT_GPS_FENCE_LOWSPEED_AUTH) 
            {
              
              #if DEBUG_MODE==1
              UART0_SendString("允许出围栏，降速"); 
              #endif 
              
              
              if(gCarInfor.speed>1)
              {                 
                 AlarmVoice(OUT_FENCE2_VOICE);                    
              }
              
              
              if(gCarInfor.maxSpeed>=4) 
              {
                 gCarInfor.maxSpeed--;
              }
              else
              {
                 gCarInfor.maxSpeed = 3;
              }               
            }
         }                         
      } 
      
      
        //手机租车时，停车超时，如果该用户不具有超时用车的权限，则VCU还车请求                                         
      if((gCarInfor.tripStopOverTimer>= (gCarInfor.stopTimeout*60))&&(gCarInfor.state == MobileRenting || gCarInfor.state==CarLock)) 
      {
        //从服务器读取超时停车权限 
        if((gCarInfor.userAuthority & MOBILE_TIMEOUT_AUTH_VALID)==0)
        {
            if(0x92==VcuConnectServerProcess(0x12)) 
            {
               gCarInfor.userAuthority |= MOBILE_TIMEOUT_AUTH_VALID;
            }
        }        
            
        
        if((gCarInfor.userAuthority & MOBILE_TIMEOUT_AUTH_VALID)== MOBILE_TIMEOUT_AUTH_VALID) 
        {           
          gCarInfor.tripStopOverTimer = 0; 
          //如果服务器允许超时还车，则还车
          if((gCarInfor.userAuthority & MOBILE_TIMEOUT_AUTH)==0) 
          { 
            gCarInfor.state = CarReady;
            gCarInfor.userid = 0;
                                                                
            //语音提醒，该车租用超时，已被还车
            AlarmVoice(RENT_OVERTIME1_VOICE); 
            
            #if DEBUG_MODE==1
            UART0_SendString("超时还车完成"); 
            #endif             
          }
          else
          {
            #if DEBUG_MODE==1
            UART0_SendString("无需超时还车"); 
            #endif                                                       
          }             
        }
      }     
   #endif
}

 
void SendCarDataByCan() 
{
 
  uint16 temp;
  TxMsg.type = FRAME_STD_DATA;
  TxMsg.datalen = 8; 
  
  /*TxMsg.id=0x01;
 
  TxMsg.data[0] = (uint8)(gCarInfor.carid>>24);    
  TxMsg.data[1] = (uint8)(gCarInfor.carid>>16);    
  TxMsg.data[2] = (uint8)(gCarInfor.carid>>8);    
  TxMsg.data[3] = (uint8)(gCarInfor.carid);
  
  TxMsg.data[4] = (uint8)(gCarInfor.userid>>24);    
  TxMsg.data[5] = (uint8)(gCarInfor.userid>>16);    
  TxMsg.data[6] = (uint8)(gCarInfor.userid>>8);    
  TxMsg.data[7] = (uint8)(gCarInfor.userid);                    
  if(FALSE==Can_Send(TxMsg)) 
  {
    return;
  }
  
  TxMsg.id=0x02;
 
  TxMsg.data[0] = (uint8)(gCarInfor.batVolt>>8);    
  TxMsg.data[1] = (uint8)(gCarInfor.batVolt);    
  TxMsg.data[2] = (uint8)(gCarInfor.batCurr>>8);    
  TxMsg.data[3] = (uint8)(gCarInfor.batCurr);
  
  TxMsg.data[4] = (uint8)(gCarInfor.batTemp);    
  TxMsg.data[5] = (uint8)(gCarInfor.state);    
  TxMsg.data[6] = (uint8)(gCarInfor.destState);    
  TxMsg.data[7] = (uint8)(gCarInfor.destStateSource);                    
  Can_Send(TxMsg); */
  
  TxMsg.id=0x03;

  TxMsg.data[0] = (uint8)(gCarInfor.batVolt>>8);
  TxMsg.data[1] = (uint8)(gCarInfor.batVolt);    
  TxMsg.data[2] = (uint8)(gCarThreshold.maxSpeed);    
  TxMsg.data[3] = (uint8)(gCarInfor.speed);
  
  //TxMsg.data[4] = (uint8)(gCarInfor.tripMileage>>24);    
  //TxMsg.data[5] = (uint8)(gCarInfor.tripMileage>>16);    
  TxMsg.data[4] = (uint8)(Duty>>8);    
  TxMsg.data[5] = (uint8)(Duty);   
  TxMsg.data[6] = (uint8)(gCarInfor.tripMileage>>8);    
  TxMsg.data[7] = (uint8)(gFrontBarriArea);                    
  Can_Send(TxMsg); 
  
  /*TxMsg.id=0x04;
  temp = (uint16)(gCarInfor.preLatitude*100);
  TxMsg.data[0] = (uint8)(temp>>8);    
  TxMsg.data[1] = (uint8)(temp);  
  
  temp = (uint16)(gCarInfor.preLongitude*100);  
  TxMsg.data[2] = (uint8)(temp>>8);    
  TxMsg.data[3] = (uint8)(temp);  
  
  
  TxMsg.data[4] = (uint8)(gCarInfor.recovTripMileage>>24);    
  TxMsg.data[5] = (uint8)(gCarInfor.recovTripMileage>>16);    
  TxMsg.data[6] = (uint8)(gCarInfor.recovTripMileage>>8);    
  TxMsg.data[7] = (uint8)(gCarInfor.recovTripMileage);                           
  Can_Send(TxMsg); 
  
  TxMsg.id=0x05;
 
  TxMsg.data[0] = (uint8)(gCarInfor.seed>>24);    
  TxMsg.data[1] = (uint8)(gCarInfor.seed>>16);    
  TxMsg.data[2] = (uint8)(gCarInfor.seed>>8);    
  TxMsg.data[3] = (uint8)(gCarInfor.seed);
  
  TxMsg.data[4] = (uint8)(gCarInfor.maxMileage>>24);    
  TxMsg.data[5] = (uint8)(gCarInfor.maxMileage>>16);    
  TxMsg.data[6] = (uint8)(gCarInfor.maxMileage>>8);    
  TxMsg.data[7] = (uint8)(gCarInfor.maxMileage);                    
  Can_Send(TxMsg); 
  
  TxMsg.id=0x06;
 
  TxMsg.data[0] = (uint8)(gCarInfor.dipAngle>>8);    
  TxMsg.data[1] = (uint8)(gCarInfor.dipAngle);    
  TxMsg.data[2] = (uint8)(gCarInfor.fabAngle>>8);    
  TxMsg.data[3] = (uint8)(gCarInfor.fabAngle);
  
  TxMsg.data[4] = (uint8)(gCarInfor.reversState);    
  TxMsg.data[5] = (uint8)(gCarInfor.accPedal);    
  TxMsg.data[6] = (uint8)(gCarInfor.accelPedalPos>>8);    
  TxMsg.data[7] = (uint8)(gCarInfor.accelPedalPos);                    
  Can_Send(TxMsg); 
  
  TxMsg.id=0x07;
 
  TxMsg.data[0] = (uint8)(gCarInfor.tripCoin);    
  TxMsg.data[1] = (uint8)(gCarInfor.batRest);     
  TxMsg.data[2] = (uint8)(gCarInfor.tripStopTime>>8);    
  TxMsg.data[3] = (uint8)(gCarInfor.tripStopTime);
  
  TxMsg.data[4] = (uint8)(gCarInfor.tripStopOverTimer);    
  TxMsg.data[5] = (uint8)(gCarInfor.stopTimeout);    
  TxMsg.data[6] = (uint8)(gCarInfor.tripTime>>8);    
  TxMsg.data[7] = (uint8)(gCarInfor.tripTime);                           
  Can_Send(TxMsg);
  
  TxMsg.id=0x08;
 
  TxMsg.data[0] = (uint8)(gCarInfor.BTTimeout);    
  TxMsg.data[1] = (uint8)(gCarInfor.speedRate);    
  TxMsg.data[2] = (uint8)(gCarInfor.userAuthority>>8);    
  TxMsg.data[3] = (uint8)(gCarInfor.userAuthority);
  
  TxMsg.data[4] = (uint8)(gCarInfor.RadarCollisionCounter);    
  TxMsg.data[5] = (uint8)(gCarInfor.VibrateLevel);    
  TxMsg.data[6] = (uint8)(gCarInfor.Event>>8);    
  TxMsg.data[7] = (uint8)(gCarInfor.Event);                           
  Can_Send(TxMsg);
  
  TxMsg.id=0x09;     
  temp = (uint16)(gCarInfor.batVoltStand*100); 
  TxMsg.data[0] = (uint8)(temp>>8);    
  TxMsg.data[1] = (uint8)(temp);
  temp = (uint16)(gCarInfor.batCurrStand*100);    
  TxMsg.data[2] = (uint8)(temp>>8);    
  TxMsg.data[3] = (uint8)(temp);
  
  temp = (uint16)(gCarInfor.batzeroVoltVolt*100);    
  TxMsg.data[4] = (uint8)(temp>>8);    
  TxMsg.data[5] = (uint8)(temp);    
  TxMsg.data[6] = (uint8)(gCarInfor.controllerType);    
  TxMsg.data[7] = (uint8)(gCarInfor.rssi);                           
  Can_Send(TxMsg);      
    
  TxMsg.id=0x0A;
 
  TxMsg.data[0] = (uint8)(gCarInfor.availableMetre>>24);    
  TxMsg.data[1] = (uint8)(gCarInfor.availableMetre>>16);    
  TxMsg.data[2] = (uint8)(gCarInfor.availableMetre>>8);    
  TxMsg.data[3] = (uint8)(gCarInfor.availableMetre);
  
  TxMsg.data[4] = (uint8)(gCarInfor.availableTime>>8);    
  TxMsg.data[5] = (uint8)(gCarInfor.availableTime);    
  TxMsg.data[6] = (uint8)(gCarInfor.VoicePlayFlag);    
  TxMsg.data[7] = (uint8)(0);                           
  Can_Send(TxMsg);
  
  TxMsg.id=0x0B;
 
  TxMsg.data[0] = (uint8)(gCarThreshold.totalMileage>>24);    
  TxMsg.data[1] = (uint8)(gCarThreshold.totalMileage>>16);    
  TxMsg.data[2] = (uint8)(gCarThreshold.totalMileage>>8);    
  TxMsg.data[3] = (uint8)(gCarThreshold.totalMileage);
  
  TxMsg.data[4] = (uint8)(Duty>>8);    
  TxMsg.data[5] = (uint8)(Duty);    
  TxMsg.data[6] = (uint8)(gCarThreshold.gprsMode);    
  TxMsg.data[7] = (uint8)(gCarThreshold.severMode);                           
  Can_Send(TxMsg);
  
  TxMsg.id=0x0C;
  temp = (uint16)(gCarPara.mcuRefVoltStand*100); 
  TxMsg.data[0] = (uint8)(temp>>8);    
  TxMsg.data[1] = (uint8)(temp);
  
  temp = (uint16)(gCarPara.batzeroCurrVolt*100);     
  TxMsg.data[2] = (uint8)(temp>>8);    
  TxMsg.data[3] = (uint8)(temp);
  
  temp = (uint16)(gCarPara.batVoltReviseCoef*1000); 
  TxMsg.data[4] = (uint8)(temp>>8);    
  TxMsg.data[5] = (uint8)(temp);
  
  temp = (uint16)(gCarPara.batCurrReviseCoef*1000);     
  TxMsg.data[6] = (uint8)(temp>>8);    
  TxMsg.data[7] = (uint8)(temp);                           
  Can_Send(TxMsg);
  
  TxMsg.id=0x0D;
  
  TxMsg.data[0] = (uint8)(gCarPara.XAccelBase>>8);    
  TxMsg.data[1] = (uint8)(gCarPara.XAccelBase);    
  TxMsg.data[2] = (uint8)(gCarPara.YAccelBase>>8);    
  TxMsg.data[3] = (uint8)(gCarPara.YAccelBase);
  
  TxMsg.data[4] = (uint8)(gCarPara.ZAccelBase>>8);    
  TxMsg.data[5] = (uint8)(gCarPara.ZAccelBase);    
  TxMsg.data[6] = (uint8)(gCarPara.minAccelPedal>>8);    
  TxMsg.data[7] = (uint8)(gCarPara.minAccelPedal); 
   
  Can_Send(TxMsg);
  
  TxMsg.id=0x0E;
  
  TxMsg.data[0] = (uint8)(gCarPara.maxAccelPedal>>8);    
  TxMsg.data[1] = (uint8)(gCarPara.maxAccelPedal);    
  TxMsg.data[2] = (uint8)(gCarPara.midAccelPedal>>8);    
  TxMsg.data[3] = (uint8)(gCarPara.midAccelPedal);
  
  TxMsg.data[4] = (uint8)(gCarPara.hardwareVersion>>8);    
  TxMsg.data[5] = (uint8)(gCarPara.hardwareVersion);    
  TxMsg.data[6] = (uint8)(gCarPara.softwareVersion>>8);    
  TxMsg.data[7] = (uint8)(gCarPara.softwareVersion); 
   
  Can_Send(TxMsg);    
  
  TxMsg.id=0x0F;
  
  TxMsg.data[0] = (uint8)(gCarPara.testResult1>>24);    
  TxMsg.data[1] = (uint8)(gCarPara.testResult1>>16);    
  TxMsg.data[2] = (uint8)(gCarPara.testResult1>>8);    
  TxMsg.data[3] = (uint8)(gCarPara.testResult1);
  
  TxMsg.data[0] = (uint8)(gCarPara.testResult2>>24);    
  TxMsg.data[1] = (uint8)(gCarPara.testResult2>>16);    
  TxMsg.data[2] = (uint8)(gCarPara.testResult2>>8);    
  TxMsg.data[3] = (uint8)(gCarPara.testResult2);
  Can_Send(TxMsg);     */
 
   
}


void TripAlarmVoice() 
{

  static uint8 preVoicePlayValue=0;
  static uint8 remindCounter=0,slotTime=0;
  static uint8 repeatNum=0;
    
  if(preVoicePlayValue != gCarInfor.VoicePlayValue)
  {    
    switch(gCarInfor.VoicePlayValue) 
    {
      case 1:               
         remindCounter = 1; 
         break;
      case 2:
         remindCounter = 3;
         break;
      case 3:
         remindCounter = 6;
         break;
      case 5:
         remindCounter = 6;
         break;
      case 7:
         remindCounter = 6;
         break;      
      case 9:
         remindCounter = 2;
         break;
      case 10:
         remindCounter = 20;
         break;
    }                     
    slotTime = 0;
    repeatNum = remindCounter;          
  }
  preVoicePlayValue = gCarInfor.VoicePlayValue;
  switch(gCarInfor.VoicePlayValue)
  {
      
    case 0:
    remindCounter=0;
    slotTime = 0;
    gCarInfor.maxSpeed = gCarThreshold.maxSpeed;
    gCarInfor.preMaxSpeed = gCarInfor.maxSpeed;
    break;
    case 1:
      if(remindCounter>0) 
      {              
        AlarmVoice(PAY_ATTENTION_VOICE);
        AlarmVoice(TRIP_TEN_MINUTE_VOICE);
        AlarmVoice(REMIND_RECHARGE_VOICE);
        remindCounter=0;
      }
      gCarInfor.maxSpeed = gCarThreshold.maxSpeed;
      gCarInfor.preMaxSpeed = gCarInfor.maxSpeed;
      
      break;
    case 2:
      if(remindCounter>0) 
      { 
        AlarmVoice(PAY_ATTENTION_VOICE);
        AlarmVoice(TRIP_FIVE_MINUTE_VOICE);
        AlarmVoice(REMIND_RECHARGE_VOICE);
        remindCounter--;
      }
      gCarInfor.maxSpeed = gCarThreshold.maxSpeed;
      gCarInfor.preMaxSpeed = gCarInfor.maxSpeed;
      break;
    case 3:                        
      if(slotTime==0) 
      { 
        if(remindCounter>0) 
        {                
          AlarmVoice(PAY_ATTENTION_VOICE);
          AlarmVoice(ACCOUNT_ZERO_VOICE);
          AlarmVoice(REMIND_RECHARGE_VOICE);
          remindCounter--;                 
        } 
        else if(remindCounter==0)  
        {
          AlarmVoice(PAY_ATTENTION_VOICE);
          AlarmVoice(ACCOUNT_ZERO_VOICE);
          AlarmVoice(REMIND_RECHARGE_VOICE);
        } 
        slotTime = repeatNum-remindCounter;                          
      }                           
      slotTime--;
      
      
      break; 
      
    case 5:
      if(slotTime==0) 
      { 
        if(remindCounter>0) 
        {                
          AlarmVoice(PAY_ATTENTION_VOICE);
          AlarmVoice(LACK_MONEY1_VOICE);
          AlarmVoice(SPEED_LIMIT_VOICE);
          AlarmVoice(REMIND_RECHARGE_VOICE);
          
          remindCounter--;    
          slotTime = repeatNum-remindCounter;                                       
        } 
        else if(remindCounter==0)  
        {
          AlarmVoice(PAY_ATTENTION_VOICE);
          AlarmVoice(LACK_MONEY1_VOICE);
          AlarmVoice(SPEED_LIMIT_VOICE);
          AlarmVoice(REMIND_RECHARGE_VOICE);                
          slotTime = repeatNum;                                        
        } 
        
      }  
      
      if(gCarInfor.maxSpeed>gCarInfor.limitSpeed) 
      {         
        gCarInfor.maxSpeed--;
        gCarInfor.preMaxSpeed = gCarInfor.maxSpeed;
      }                           
      slotTime--;
            
            
      break;  
    case 7:
      
      if(slotTime==0) 
      { 
        if(remindCounter>0) 
        {                
          AlarmVoice(PAY_ATTENTION_VOICE);
          AlarmVoice(LACK_MONEY1_VOICE);
          AlarmVoice(CAR_TIYAN_VOICE);
          remindCounter--;                 
        } 
        else if(remindCounter==0)  
        {
          AlarmVoice(PAY_ATTENTION_VOICE);
          AlarmVoice(LACK_MONEY1_VOICE);
          AlarmVoice(CAR_TIYAN_VOICE);
        } 
        slotTime = repeatNum-remindCounter;                          
      }                           
      slotTime--;
      
      
      break;     
    case 9:
      if(slotTime==0) 
      { 
        if(remindCounter>0) 
        {                
          AlarmVoice(PAY_ATTENTION_VOICE);
          AlarmVoice(REMAIN_TIME);
          AlarmVoice(CAR_TIYAN_VOICE);
          
          remindCounter--;    
          slotTime = repeatNum-remindCounter;                                       
        }                
        #if DEBUG_MODE==1
        UART0_SendString("行程剩余一分钟");      
        #endif              
      }                              
      slotTime--;
      
      break;        
    case 10:
      if(slotTime==0 && gCarInfor.maxSpeed>0) 
      {         
        AlarmVoice(PAY_ATTENTION_VOICE);
        AlarmVoice(LACK_MONEY1_VOICE);          
        AlarmVoice(STOPPING_CAR1_VOICE);                
        slotTime = 1;  
        #if DEBUG_MODE==1
        UART0_SendString("行程结束");      
        #endif                                                        
      }  
      
      if(gCarInfor.maxSpeed>gCarInfor.limitSpeed) 
      {         
        gCarInfor.maxSpeed--;
        gCarInfor.preMaxSpeed = gCarInfor.maxSpeed;
      }   
      
      #if DEBUG_MODE==1
      if(gCarInfor.maxSpeed==0) 
      {
        
        UART0_SendString("限速为0");      
      }
      #endif                          
      slotTime--;
            
      break;          
                              
     }                                
   
}

