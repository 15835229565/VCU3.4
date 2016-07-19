#include "includes.h"



CarInfor     gCarInfor;              //车辆ID  电压 电流 温度 状态 以及故障码等信息
CarThreshold gCarThreshold;       //车辆阈值的配置信息
CarPara      gCarPara;            //车辆配置参数比如轮胎直径，霍尔传感器脉冲数，软硬件版本等
MotorInfor   gMotorInfor;         //保存电机相关的脉冲等信息

uint32 carIdFromServer=0,userIdFromServer=0;            //记录来自服务器的userid和carid
uint8 heartBeatSNFromServer=0;            //从服务器获取的心跳数据的序列号
uint8 heartBeatSNToServer = 0;						//主动发给服务器的心跳数据的序列号
uint16 hardwareVersionFromServer=0;
uint16 softwareVersionFromServer=0;
uint16 softwareUpdateDateFromServer=0;
/*
*********************************************************************************
* 函数名：ServerCmdProcess
* 说  明：接收处理服务器返回的命令包
* 参  数：uint8 servercommand 命令内容
* 返回值: 命令类型+0x80 表示对应命令接收成功 FALSE 表示失败
* 其  他：
*********************************************************************************
*/

uint8 ServerCmdProcess(uint8  *servercommand)
{    
    uint8 state=0;
    gGPRSConnectTimer = 0;
    switch(servercommand[0])
    {     
      case 0x00://被租用时心跳包响应
      
          if((gCarInfor.state == BTRenting) || (gCarInfor.state == CarLock)
              ||(gCarInfor.state == CoinRenting)||(gCarInfor.state == MobileRenting)
              ||(gCarInfor.state == CarMaintain))
          {
            if(ServerGprsToVcuCmd00(servercommand)) 
            {
              return 0x80;
            }
          }
          break;
      case 0x01://车辆状态查询
          if(ServerGprsToVCUCmd01(servercommand)) 
          {            
              VcuGprsToServerCmd01();
              return 0x81;
          }
          break; 
          
      case 0x02://车辆设备开关控制
          if(ServerGprsToVcuCmd02(servercommand)) 
          {
              VcuGprsToServerCmd02();
              return 0x82;
          }
          break; 
      case 0x03://手机租车命令
          if(ServerGprsToVcuCmd03(servercommand))
          {
              VcuGprsToServerCmd03(servercommand);
              return 0x83;
          }
          break;
        
      case 0x04://手机还车命令
          if(ServerGprsToVcuCmd04(servercommand))
          {
              VcuGprsToServerCmd04();
              return 0x84;
          }
          break;
      case 0x05://手机锁车命令
          if(ServerGprsToVcuCmd05(servercommand))
          {
              VcuGprsToServerCmd05();
              return 0x85;
          }
          break;        
      case 0x06://初始化参数设置
          if(ServerGprsToVcuCmd06(servercommand))
          {
              VcuGprsToServerCmd06();
              return 0x86;
          }
          break;
      case 0x07://请求与服务器建立连接命令
           if(ServerGprsToVcuCmd07(servercommand)) 
           {
              return 0x87;
           }
           break;
      case 0x08://投币租车请求命令
           if(ServerGprsToVcuCmd08(servercommand)) 
           {
              return 0x88;
           }
           break;       
      case 0x09://投币租车结束，请求还车命令
           if(ServerGprsToVcuCmd09(servercommand)) 
           {
              return 0x89;
           }
           break;
      case 0x0A://手机解锁车命令
          if(ServerGprsToVcuCmd0A(servercommand))
          {
              VcuGprsToServerCmd0A();
              return 0x8A;
          }
          break;
      case 0x0B://手机蓝牙租车命令
          if(ServerGprsToVcuCmd0B(servercommand))
          {              
              return 0x8B;
          }
          break;          
      case 0x0C: //初始化参数以及配置参数查询
          if(ServerGprsToVcuCmd0C(servercommand))
          {
              VcuGprsToServerCmd0C();
              return 0x8C;
          }
          break;
      case 0x0D://设备开关状态查询
          if(ServerGprsToVcuCmd0D(servercommand))
          {
              VcuGprsToServerCmd0D();
              return 0x8D;
          }
          break;      
      case 0x0E://车辆预定
          if(ServerGprsToVcuCmd0E(servercommand)) 
          {            
            VcuGprsToServerCmd0E();
            return 0x8E;          
          }
          break;
      case 0x0F://结束车辆预定
          if(ServerGprsToVcuCmd0F(servercommand))
          {
              VcuGprsToServerCmd0F();                          
                              
              return 0x8F;
          }
          break;            
      case 0x11://只有状态正确时 才进行待机时心跳包响应
      
          if((gCarInfor.state == CarOrder)||(gCarInfor.state == CarReady)) 
          {            
            if(ServerGprsToVcuCmd11(servercommand))
            {              
                return 0x91;
            }
          }
          break;
      case 0x12: //停车超时，还车请求命令
          if(ServerGprsToVcuCmd12(servercommand))
          {
              return 0x92;
          }       
          break;           
       case 0x13: //超出GPS围栏，还车请求命令
          if(ServerGprsToVcuCmd13(servercommand))
          {
              return 0x93;
          }       
          break;
      case 0x14://手机蓝牙还车命令
          if(ServerGprsToVcuCmd14(servercommand))
          {              
              return 0x94;
          }
          break;
      case 0x15://后台程序更新指令
          if(ServerGprsToVcuCmd15(servercommand))
          {              
              if(VcuGprsToServerCmd15()) 
              {
                WriteCarPara();
                delayms(100);
                WriteBootFlag(GPRS_UPDATE_MODE);
                delayms(100);
                
                //关闭所有中断，进入远程程序更新模式
                DisableInterrupts;
                while(1);
              }
              return 0x95;
          }
          break;
              
      case 0x31://车辆维护命令
          if(ServerGprsToVcuCmd31(servercommand))
          {
              VcuGprsToServerCmd31(servercommand);
              return 0xB1;
          }
          break;
      case 0x32: //结束车辆维护命令
          if(ServerGprsToVcuCmd32(servercommand))
          {
              VcuGprsToServerCmd32();
              return 0xB2;
          }
          break;
      default:
          break;
    }
    return FALSE;
}


/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd00
* 说  明：向服务器发送被租用心跳包
* 参  数：无
* 返回值：
* 其  他：
*********************************************************************************
*/
  
uint8 VcuGprsToServerCmd00() 
{
    uint8 cmd[50]= {0};    
    uint8 ret;
    uint16 time;
   
    cmd[0]=0x00;
    cmd[1]=heartBeatSNToServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);
    memcpy(&cmd[10],(uint8*)&gCarInfor.batVolt,2);
    memcpy(&cmd[12],(uint8*)&gCarInfor.batCurr,2);
    cmd[14] = gCarInfor.batTemp;
    cmd[15] = gCarInfor.state;
    cmd[16] = (uint8)(gCarInfor.speed);
    memcpy(&cmd[17],(uint8*)&gCarInfor.lLatitude1,2);
    memcpy(&cmd[19],(uint8*)&gCarInfor.lLatitude2,4);
    memcpy(&cmd[23],(uint8*)&gCarInfor.lLongitude1,2);
    memcpy(&cmd[25],(uint8*)&gCarInfor.lLongitude2,4);
      
    memcpy(&cmd[29],(uint8*)&gCarInfor.tripMileage,4);    
   
 
    cmd[33] = gCarInfor.tripID; 
    memcpy(&cmd[34],(uint8*)&gCarThreshold.errCode,2);
  cmd[36] = (uint8)(gCarInfor.tripStopOverTimer/5.0);     //以5秒为单位，上传    
    memcpy(&cmd[37],(uint8*)&gCarInfor.tripStopTime,2); 
    
    cmd[39] = gCarInfor.speedRate;
    memcpy(&cmd[40],(uint8*)&gCarThreshold.brakeActiveCounter,4); 
    time = (uint16)(TimerA_Counter/100);
    memcpy(&cmd[44],(uint8*)&time,2);
    
    cmd[46] = gCarInfor.VibrateLevel;
    cmd[47] = gCarInfor.SatelliteNum;
    cmd[48] = gCarInfor.rssi;
    
    
    
    DataEncryption(cmd,49);           
    cmd[49] = GetCRC(cmd,49); 
    
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,50)+50);
   
    if(TRUE == ret) 
    {    
        SendBytesToServerByGPRS(cmd,50);           
    }
    return ret;    
}  



/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd00 
* 说  明：服务器回复VCU的被租用时心跳包
* 参  数：uint8 *servercommand 收到的命令包
* 返回值TRUE 有效命令  FALSE 无效命令
* 其  他：
*********************************************************************************
*/
uint8 ServerGprsToVcuCmd00(uint8 *servercommand) 
{     
      uint32 carid=0,userid=0;
      memcpy((uint8*)&carid,&servercommand[2],4);
      memcpy((uint8*)&userid,&servercommand[6],4);
          
      if(carid==gCarInfor.carid)
      {          
        if(servercommand[10]==1)
        {
            heartBeatSNToServer= servercommand[11]+10;        
        }

             
        gCarInfor.destState = servercommand[12];  
        gCarInfor.destStateSource = StateFromServer;
        if(TRUE==CarStateChange()) 
        {
          gCarInfor.userid = userid;
        }
                     
        gCarInfor.batRest = servercommand[13];
        gCarInfor.VoicePlayValue = servercommand[14];
        gCarInfor.limitSpeed = servercommand[15];
        if(gCarInfor.state == CarMaintain && gCarInfor.userid!=0)
        { 
         ;
        }
        else
        { 
         TripAlarmVoice();    
        }
        return TRUE;              
      } 
      else
      {
        return FALSE;
      }
     
}



/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd01
* 说  明：VCU对服务器进行信息查询命令的响应
* 参  数：无
* 返回值：TRUE 发送成功 FALSE 发送失败
* 其  他：
*********************************************************************************
*/

uint8 VcuGprsToServerCmd01() 
{
    uint8 cmd[42]= {0};    
    uint8 ret;
    
    cmd[0]=0x01;
    cmd[1]=heartBeatSNFromServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);
    
    memcpy(&cmd[10],(uint8*)&gCarInfor.batVolt,2);
    memcpy(&cmd[12],(uint8*)&gCarInfor.batCurr,2);
    cmd[14] = gCarInfor.batTemp;    
    memcpy(&cmd[15],(uint8*)&gCarInfor.lLatitude1,2);
    memcpy(&cmd[17],(uint8*)&gCarInfor.lLatitude2,4);
    memcpy(&cmd[21],(uint8*)&gCarInfor.lLongitude1,2);
    memcpy(&cmd[23],(uint8*)&gCarInfor.lLongitude2,4);
    memcpy(&cmd[27],(uint8*)&gCarInfor.dipAngle,2);
   
    cmd[29] = gCarThreshold.operationMode;
    cmd[30] = gCarInfor.reversState;   
    cmd[31] = (uint8)gCarInfor.speed;
     
    memcpy(&cmd[32],(uint8*)&gCarInfor.tripMileage,4);
    cmd[36] = gCarInfor.tripID;
    
    memcpy(&cmd[37],(uint8*)&gCarThreshold.totalMileage,4);
    DataEncryption(cmd,41);                         
    cmd[41] = GetCRC(cmd,41); 
    
    
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,42)+42);
   
    if(TRUE == ret) 
    {    
        SendBytesToServerByGPRS(cmd,42);           
    }
    return ret;        
}


/*
*********************************************************************************
* 函数名：ServerGprsToVCUCmd01
* 说  明：服务器对VCU进行信息查询命令
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 有效命令 FALSE 无效命令
* 其  他：
*********************************************************************************
*/

uint8 ServerGprsToVCUCmd01(uint8 *servercommand)
{    
    uint32 carid=0,userid=0;
    
    memcpy((uint8*)&carid,&servercommand[2],4);
    memcpy((uint8*)&userid,&servercommand[6],4);
    
    
  //  if(carid==gCarInfor.carid && userid ==gCarInfor.userid) 
    if(carid==gCarInfor.carid)
    {
      if(heartBeatSNFromServer<=servercommand[1]) 
      {
         heartBeatSNFromServer = servercommand[1]; 
      } 
      else 
      {
        return FALSE;
      }
      return TRUE;
    }
    else
    {
      return FALSE;
    }
    
}



/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd02
* 说  明：VCU回复服务器设备开关控制指令
* 参  数：服务器无回复
* 返回值：
* 其  他：
*********************************************************************************
*/
uint8 VcuGprsToServerCmd02() 
{
    uint8 cmd[11]= {0};    
    uint8 ret;
    
    
    cmd[0]=0x02;
    cmd[1]=heartBeatSNFromServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);
    DataEncryption(cmd,10);                             
    cmd[10] = GetCRC(cmd,10); 
    
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,11)+11);
   
    if(TRUE == ret) 
    {    
        SendBytesToServerByGPRS(cmd,11);           
    }
    return ret;           
}

/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd02
* 说  明：服务器对VCU设备开关进行控制命令
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 设置成功 FALSE 设置失败
* 其  他：
*********************************************************************************
*/
uint8 ServerGprsToVcuCmd02(uint8 *servercommand)
{
    
    uint32 carid=0,userid=0;
    static uint8 BrakeRelayStatus=0;
    memcpy((uint8*)&carid,&servercommand[2],4);
    memcpy((uint8*)&userid,&servercommand[6],4);
    
  
     if(carid==gCarInfor.carid)
    {
      if(heartBeatSNFromServer<=servercommand[1]) 
      {
         heartBeatSNFromServer = servercommand[1]; 
      } 
      else 
      {
        return FALSE;
      }
      
      if((servercommand[10]>>7) == 1 && device.state.Headlamp == 0)
      {
          device.state.Headlamp = 1;
      }
      else if((servercommand[10]>>7) == 0 && device.state.Headlamp == 1)
      {
          device.state.Headlamp = 0;
      }

      if(((servercommand[10]&0x40)>>6) == 1 && device.state.Leftlamp == 0)
      {
          device.state.Leftlamp = 1;
      }
      else if(((servercommand[10]&0x40)>>6) == 0 && device.state.Leftlamp == 1)
      {
          device.state.Leftlamp = 0;
      }

      if(((servercommand[10]&0x20)>>5) == 1 && device.state.Rightlamp == 0)
      {
          device.state.Rightlamp = 1;
      }
      else if((servercommand[10]>>5) == 0 && device.state.Rightlamp == 1)
      {
          device.state.Rightlamp = 0;
      }

      if(((servercommand[10]&0x10)>>4) == 1 && device.state.Nightlamp == 0)
      {
          device.state.Nightlamp = 1;
      }
      else if(((servercommand[10]&0x10)>>4) == 0 && device.state.Nightlamp == 1)
      {
          device.state.Nightlamp = 0;
      }

      if(((servercommand[10]&0x08)>>3) == 1 && device.state.Horn == 0)
      {
          HornSwitch = 1;
          device.state.Horn = 1;
      }
      else if(((servercommand[10]&0x08)>>3) == 0 && device.state.Horn == 1)
      {
          HornSwitch = 0;
          device.state.Horn = 0;
      }

      if((servercommand[10]&0x04>>2) == 1 && device.state.Wiper == 0)
      {
          device.state.Wiper = 1;
      }
      else if((servercommand[10]&0x04>>2) == 0 && device.state.Wiper == 1)
      {
          device.state.Wiper = 0;
      }

      if(((servercommand[10]&0x02)>>1) == 1 && device.state.SlotmachineState == 0)
      {
          Slotmachine = 1;
          device.state.SlotmachineState = 1;
      }
      else if(((servercommand[10]&0x02)>>1) == 0 && device.state.SlotmachineState == 1)
      {
          Slotmachine = 0;
          device.state.SlotmachineState = 0;
      }

      if((servercommand[10]&0x01) == 1 && BrakeRelayStatus == 0)
      {
          BrakeRelay = ELECMAGRELEASE;
          BrakeRelayStatus = 1;
      }
      else if((servercommand[10]&0x01) == 0 && BrakeRelayStatus == 1)
      {
          BrakeRelay = ELECMAGHOLD;
          BrakeRelayStatus= 0;
      }
      return TRUE;
   }
   else
      return FALSE;
    
}



/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd03
* 说  明：VCU回复服务器租车命令
* 参  数：服务器无回复
* 返回值：
* 其  他：
*********************************************************************************
*/

uint8 VcuGprsToServerCmd03(uint8* servercommand) 
{
    uint8 cmd[13]= {0};    
    uint8 ret;
    cmd[0]=0x03;
    cmd[1]=heartBeatSNFromServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);  
    
    //userid和carid不正确
    if(carIdFromServer != gCarInfor.carid) 
    {
      cmd[10]=CARID_ERROR;
    }   
    else 
    {
        gCarInfor.destState = MobileRenting; 
        gCarInfor.destStateSource = StateFromServer;        
        if(CarStateChange()==TRUE)
        {
            
            cmd[10] = CAR_OP_SUCC;
            gCarInfor.tripID++;                    
            gCarThreshold.userLevel = servercommand[15];
            //第一次手机租车且用户级别较低时，播放操作指南语音
            if(gCarThreshold.userLevel==0 && (gCarInfor.Event&TEACH_VOICE_EVENT)==0)
            {               
              gCarInfor.Event |= TEACH_VOICE_EVENT;
              AlarmVoice(FIRST_OP_VOICE);
              
              #if DEBUG_MODE==1
              UART0_SendString("提醒语音开始播放"); 
              #endif                            
            } 
            else 
            {                                       
              gCarInfor.userid = userIdFromServer; 
            }
            memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
            memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);
              
            gCarInfor.maxSpeed = servercommand[10]; 
            if(gCarInfor.maxSpeed!=gCarThreshold.maxSpeed)
            {
             gCarThreshold.maxSpeed = gCarInfor.maxSpeed; 
             gCarThresholdEepromUpdateFlag=1;
            }
            gCarInfor.preMaxSpeed = gCarInfor.maxSpeed;
                  
            memcpy((uint8*)&gCarInfor.maxMileage,&servercommand[11],4);   
            
            gCarInfor.stopTimeout = servercommand[16];
            TripDataInit();                    
            SpeedSensorCaptureInit();
                        
        } 
        else
        {        
            cmd[10] = gCarInfor.state+0x80 ;
            #if DEBUG_MODE==1
            UART0_SendString("手机租车失败"); 
            #endif
        }
    }
      
    cmd[11] = gCarInfor.tripID; 
    DataEncryption(cmd,12);              
    cmd[12] = GetCRC(cmd,12); 
    
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,13)+13);
   
    if(TRUE == ret) 
    {    
        SendBytesToServerByGPRS(cmd,13);           
    }
    //首次租车先播放完，才能正式开车
    if(gCarInfor.state == CarReady) 
    {      
      delayms(13000);
      
      gCarInfor.state = MobileRenting; 
    }
    return ret;     
}


/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd03
* 说  明：服务器对VCU设备发送手机租车命令
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 设置成功 FALSE 设置失败
* 其  他：
*********************************************************************************
*/

uint8 ServerGprsToVcuCmd03(uint8* servercommand)
{    
      
    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4); 
        
    heartBeatSNToServer++;
    if(heartBeatSNFromServer<=servercommand[1]) 
    {
       heartBeatSNFromServer = servercommand[1]; 
    } 
    else 
    {
      return FALSE;
    }
    #if DEBUG_MODE==1
    UART0_SendString("手机租车请求"); 
    #endif
    return TRUE;      
}


/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd04
* 说  明：VCU回复服务器还车命令
* 参  数：服务器无回复
* 返回值：TRUE 发送成功 FALSE 发送失败
* 其  他：
*********************************************************************************
*/

uint8 VcuGprsToServerCmd04() 
{
    uint8 cmd[34]= {0};    
    uint8 ret;
    
    
    cmd[0]=0x04;
    cmd[1]=heartBeatSNFromServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);
    
    
    memcpy(&cmd[11],(uint8*)&gCarInfor.tripMileage,4);
    memcpy(&cmd[15],(uint8*)&gCarInfor.tripTime,2);
    cmd[17] = gCarInfor.tripID;
    memcpy(&cmd[18],(uint8*)&gCarInfor.lLatitude1,2);
    memcpy(&cmd[20],(uint8*)&gCarInfor.lLatitude2,4);
    memcpy(&cmd[24],(uint8*)&gCarInfor.lLongitude1,2);
    memcpy(&cmd[26],(uint8*)&gCarInfor.lLongitude2,4);
    memcpy(&cmd[30],(uint8*)&gCarInfor.tripStopTime,2);
    
    cmd[32] = gCarInfor.RadarCollisionCounter;
          
    //userid和carid不正确
    if(carIdFromServer != gCarInfor.carid) 
    {
      cmd[10] = CARID_ERROR;
    } 
    //电动机未停止的情况下不能还车
    else if(Motorstep_per_sec_count!=0) 
    {                     
       cmd[10] = SPEED_ERROR;
       AlarmVoice(CAR_FULL_STOP_VOICE);
        
    } 
   /* else if((gCarInfor.dipAngle>6)||(gCarInfor.dipAngle<-6)||(gCarInfor.fabAngle>10)||(gCarInfor.fabAngle<-10))  //左右倾角大于6度 前后倾角大于10度 不能还车   
    {
       cmd[10] = DIPANGLE_ERROR;              
       AlarmVoice(CAR_TILT3_VOICE);
    } */
    //在手机租车 下还车
    else 
    {
      gCarInfor.destState = CarReady; 
      gCarInfor.destStateSource = StateFromServer;
      if(CarStateChange()==TRUE)
      {
         
          gCarInfor.userid = 0;         
          cmd[10] = CAR_OP_SUCC;      
          
          
                 
          gCarInfor.maxSpeed = gCarThreshold.maxSpeed;
          
          gCarInfor.preMaxSpeed = gCarInfor.maxSpeed;
          gCarThreshold.totalMileage += (uint32)(gCarInfor.tripMileage/1000.0);
          
          //TripDataInit();                    
                                
           
      } 
      else
      {        
          cmd[10] = gCarInfor.state+0x80 ;
      }
      
    }
   
    
    DataEncryption(cmd,33); 
    cmd[33] = GetCRC(cmd,33); 
    
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,34)+34);
   
    if(TRUE == ret) 
    {    
        SendBytesToServerByGPRS(cmd,34);           
    }
    

    return ret;     
}


/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd04
* 说  明：服务器对VCU设备发送手机还车命令
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 设置成功 FALSE 设置失败
* 其  他：
*********************************************************************************
*/

uint8 ServerGprsToVcuCmd04(uint8* servercommand)
{  
  
    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
    
    
   if(carIdFromServer==gCarInfor.carid)
    {
        if(heartBeatSNFromServer<=servercommand[1]) 
        {
           heartBeatSNFromServer = servercommand[1]; 
        } 
        else 
        {
          return FALSE;
        }        
    }
    #if DEBUG_MODE==1
    UART0_SendString("手机还车请求"); 
    #endif
    return TRUE;      
  
}

/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd05
* 说  明：服务器对VCU发送车辆锁定命令
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 有效命令 FALSE 无效命令
* 其  他：
*********************************************************************************
*/
uint8 ServerGprsToVcuCmd05(uint8 *servercommand)
{
    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
    
    if(carIdFromServer==gCarInfor.carid) 
    {        
      heartBeatSNFromServer = servercommand[1]; 
    }
    #if DEBUG_MODE==1
    UART0_SendString("手机锁车请求"); 
    #endif
    return TRUE;
}

/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd05
* 说  明：VCU发送锁车成功与否到服务器
* 参  数：uint8 state  ServerGprsToVcuCmd05失败或者成功标志
* 返回值：TRUE 发送成功 FALSE 发送失败
* 其  他：
*********************************************************************************
*/
uint8 VcuGprsToServerCmd05()
{
    uint8 cmd[12]= {0};    
    uint8 ret;
    
    
    cmd[0]=0x05;
    cmd[1]=heartBeatSNFromServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);  
    
    if(carIdFromServer != gCarInfor.carid) 
    {
      cmd[10]=CARID_ERROR;
    }//电动机未停止的情况下不能锁车
    else if(Motorstep_per_sec_count!=0) 
    {                     
       cmd[10] = SPEED_ERROR;               
    } 
    else 
    {
      gCarInfor.destState = CarLock; 
      gCarInfor.destStateSource = StateFromServer;
      if(CarStateChange()==TRUE)
      {                              
        cmd[10]=CAR_OP_SUCC;
        gCarInfor.userid = userIdFromServer;           
        memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
        memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4); 
        
      }        
      else 
      {
        cmd[10]=gCarInfor.state+0x80; //如果为失败状态，则返回当前状态，并将当前的车辆ID和用户ID返回给服务器    
      }       
    }
     
    
    DataEncryption(cmd,11);
    cmd[11]=GetCRC(cmd,11);
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,12)+12);;
   
    if(TRUE == ret) 
    {    
        SendBytesToServerByGPRS(cmd,12);           
    }    
    return ret;
}


/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd06
* 说  明VCU回复服务器初始化参数设置命令
* 参  数：
* 返回值：TRUE 发送成功 FALSE 发送失败
* 其  他：
*********************************************************************************
*/

uint8 VcuGprsToServerCmd06()
{
    uint8 cmd[11]= {0};    
    uint8 ret;
    
    
    cmd[0]=0x06;
    cmd[1]=heartBeatSNFromServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);
      
    DataEncryption(cmd,10); 
    cmd[10] = GetCRC(cmd,10); 
    
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,11)+11);
   
    if(TRUE == ret) 
    {    
        SendBytesToServerByGPRS(cmd,11);           
    }    
    return ret;
}


/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd06
* 说  明：服务器对VCU设备发送配置参数
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 设置成功 FALSE 设置失败
* 其  他：
*********************************************************************************
*/

uint8 ServerGprsToVcuCmd06(uint8 *servercommand)
{
    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
    
 //   if(carIdFromServer==gCarInfor.carid && userIdFromServer == gCarInfor.userid) 
  if(carIdFromServer==gCarInfor.carid)
    {
      if(heartBeatSNFromServer<=servercommand[1]) 
      {
         heartBeatSNFromServer = servercommand[1]; 
      } 
      else 
      {
          return FALSE;
      }  
       //设置参数阈值
      gCarThreshold.batLowLevel = servercommand[10];
      gCarThreshold.batTempLevel = servercommand[11];
      gCarThreshold.operationMode = servercommand[12];                
      memcpy((uint8*)&gCarThreshold.perCoinMetre,&servercommand[13],2);
      memcpy((uint8*)&gCarThreshold.perCoinTime,&servercommand[15],2);
      
      if(servercommand[17]<3) 
      {        
        gCarThreshold.shortHeartTime = 3;
      } 
      else
      {
        gCarThreshold.shortHeartTime = servercommand[17];
      }
      
      if(servercommand[18]<5) 
      {
        gCarThreshold.longHeartTime = 5; 
      } 
      else
      {
        gCarThreshold.longHeartTime = servercommand[18];
      }
      
      memcpy((uint8*)&gCarThreshold.LTLatitude1,&servercommand[19],2);
      memcpy((uint8*)&gCarThreshold.LTLatitude2,&servercommand[21],4);
      memcpy((uint8*)&gCarThreshold.LTLongitude1,&servercommand[25],2);
      memcpy((uint8*)&gCarThreshold.LTLongitude2,&servercommand[27],4);
      
      memcpy((uint8*)&gCarThreshold.RBLatitude1,&servercommand[31],2);
      memcpy((uint8*)&gCarThreshold.RBLatitude2,&servercommand[33],4);
      memcpy((uint8*)&gCarThreshold.RBLongitude1,&servercommand[37],2);
      memcpy((uint8*)&gCarThreshold.RBLongitude2,&servercommand[39],4);
      gCarThreshold.stopTimeout = servercommand[43];
      gCarInfor.stopTimeout = servercommand[43];
      gCarThreshold.maxCoin = servercommand[44];
      gCarThreshold.maxSpeed = servercommand[45];                       
      gCarThresholdEepromUpdateFlag = 1;
      return TRUE;  
    }
    else
    {
        return FALSE;    
    }
}


/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd07
* 说  明：VCU发送建立连接请求到服务器
* 参  数：
* 返回值：TRUE 发送成功 FALSE 发送失败
* 其  他：
*********************************************************************************
*/
uint8 VcuGprsToServerCmd07() 
{
    uint8 cmd[66]= {0};
    uint8 ret;
    
    memset(cmd,0,66);
    
     
    cmd[0]=0x07;
    cmd[1]=heartBeatSNToServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);
    memcpy(&cmd[10],(uint8*)&gCarInfor.batVolt,2);
    memcpy(&cmd[12],(uint8*)&gCarInfor.batCurr,2);
    cmd[14] = gCarInfor.batTemp;
    memcpy(&cmd[15],(uint8*)&gCarInfor.tripMileage,4);
    memcpy(&cmd[19],(uint8*)&gCarInfor.lLatitude1,2);    
    memcpy(&cmd[21],(uint8*)&gCarInfor.lLatitude2,4);    
    memcpy(&cmd[25],(uint8*)&gCarInfor.lLongitude1,2);
    memcpy(&cmd[27],(uint8*)&gCarInfor.lLongitude2,4);
    memcpy(&cmd[31],(uint8*)&gCarInfor.seed,4);
    memcpy(&cmd[35],(uint8*)&gCarPara.softwareVersion,2);
    memcpy(&cmd[37],(uint8*)&gCarPara.hardwareVersion,2);      
    memcpy(&cmd[39],(uint8*)&gCarInfor.iccid,20);      
    memcpy(&cmd[59],(uint8*)&gCarThreshold.totalMileage,4);      
    memcpy(&cmd[63],(uint8*)&gCarThreshold.GPRS_Noconnect_Coinnum,2);                         
    DataEncryption(cmd,65);
    cmd[65] = GetCRC(cmd,65); 
    
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,66)+66);;
   
    SendBytesToServerByGPRS(cmd,66);   
    
    return ret;    
}

/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd07
* 说  明：服务器响应VCU发送的建立连接命令
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 有效命令 FALSE 无效命令
* 其  他：
*********************************************************************************
*/

uint8 ServerGprsToVcuCmd07(uint8 *servercommand) 
{
    uint8 seed[4],key[4],temp[4]={0};
    uint32 mileage;
    uint16 time;
    memcpy(seed,(uint8*)&gCarInfor.seed,4);
    
    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
    
       
    if(carIdFromServer==gCarInfor.carid)
    {          
        //进行身份验证
        BCMSecurityAccess(seed,key);
                    
        if(key[0]==servercommand[44] && key[1]==servercommand[45] && key[2]==servercommand[46] && key[3]==servercommand[47]) 
        {
            //设置参数阈值
            gCarThreshold.batLowLevel = servercommand[10];
            gCarThreshold.batTempLevel = servercommand[11];
            gCarThreshold.operationMode = servercommand[12];                
            memcpy((uint8*)&gCarThreshold.perCoinMetre,&servercommand[13],2);
            memcpy((uint8*)&gCarThreshold.perCoinTime,&servercommand[15],2);
             
            if(servercommand[17]<3) 
            {        
              gCarThreshold.shortHeartTime = 3;
            } 
            else
            {
              gCarThreshold.shortHeartTime = servercommand[17];
            }
            
            if(servercommand[18]<5) 
            {
              gCarThreshold.longHeartTime = 5; 
            } 
            else
            {
              gCarThreshold.longHeartTime = servercommand[18];
            }
        
        
            memcpy((uint8*)&gCarThreshold.LTLatitude1,&servercommand[19],2);
            memcpy((uint8*)&gCarThreshold.LTLatitude2,&servercommand[21],4);
            memcpy((uint8*)&gCarThreshold.LTLongitude1,&servercommand[25],2);
            memcpy((uint8*)&gCarThreshold.LTLongitude2,&servercommand[27],4);
            
            memcpy((uint8*)&gCarThreshold.RBLatitude1,&servercommand[31],2);
            memcpy((uint8*)&gCarThreshold.RBLatitude2,&servercommand[33],4);
            memcpy((uint8*)&gCarThreshold.RBLongitude1,&servercommand[37],2);
            memcpy((uint8*)&gCarThreshold.RBLongitude2,&servercommand[39],4);
            
            gCarThreshold.stopTimeout = servercommand[43];                                     
            
            memcpy((uint8*)&mileage,&servercommand[48],4);                 
            
            
            
            
            memcpy((uint8*)&time,&servercommand[52],2);
                        
            gCarInfor.tripID = servercommand[54];


            gCarInfor.destState = servercommand[55]; 
            gCarInfor.destStateSource = StateFromServer;             
            if(TRUE==CarStateChange()) 
            {
               gCarInfor.userid= userIdFromServer;  
            }
             gCarInfor.recovTripMileage = 0;
             if((gCarInfor.state==CarLock) || (gCarInfor.state==MobileRenting) || (gCarInfor.state==CarMaintain))
             {                
                
                if(gCarInfor.userid!=0) 
                {                  
                  //当MCU在手机租车或者维护人员情况下复位时，需要将原有里程恢复回来
                  if(gCarInfor.MCUResetFlag==0) 
                  {                    
                    gCarInfor.recovTripMileage = mileage;              
                  } 
                }
             }
            gCarInfor.MCUResetFlag = 1; 
             
            if(servercommand[56]==0)
            {
               gCarThreshold.maxCoin = 20;               
            }
            else
            {
               gCarThreshold.maxCoin = servercommand[56]; 
            }
            
            if((servercommand[57]==0)||(servercommand[57]>25))
            {
               gCarThreshold.maxSpeed = 5;               
            }
            else
            {
               gCarThreshold.maxSpeed = servercommand[57]; 
               
            }
            //投币或者蓝牙开车情况下，最高限速不从服务器读取
            if(gCarInfor.state != CoinRenting && gCarInfor.state!=BTRenting) 
            {              
              gCarInfor.maxSpeed = gCarThreshold.maxSpeed;
              gCarInfor.preMaxSpeed = gCarInfor.maxSpeed;
            }
            memcpy((uint8*)&time,&servercommand[58],2); 
            if(time<=30)
            {
               gCarThreshold.maxConnectTimeOut = 30;               
            }
            else
            {
               gCarThreshold.maxConnectTimeOut = time;               
            }
            gCarInfor.tripStopOverTimer = servercommand[60];           //新增的停车超时时间记录
            memcpy((uint8*)&gCarInfor.tripStopTime,&servercommand[61],2);     //本次租车累计的停车时间
            
            if(gCarThreshold.severMode!=servercommand[63]) 
            {              
              gCarThreshold.severMode = servercommand[63];                //增加一个目标服务器字段，由后台进行切换 
              #if DEBUG_MODE==1
              UART0_SendString("切换服务器"); 
              #endif
              GPRS_init_flag = GPRS_RESTART;
              gCarThresholdEepromUpdateFlag=1;
              return FALSE;
            }
            
            if(gCarThreshold.gprsMode != servercommand[64])                //增加一个gprs模式字段，由后台进行切换 
            {
              gCarThreshold.gprsMode = servercommand[64];                //增加一个目标服务器字段，由后台进行切换 
              #if DEBUG_MODE==1
              UART0_SendString("切换连接模式"); 
              #endif
              GPRS_init_flag = GPRS_RESTART;
              gCarThresholdEepromUpdateFlag=1;
              return FALSE;
            }
            gCarThresholdEepromUpdateFlag=1;
            return TRUE;
        }
        else
        {
            #if DEBUG_MODE==1
            UART0_SendString("建立连接失败1"); 
            #endif
            gGPRSConnectTimer+=1+gCarThreshold.maxConnectTimeOut; 
            return FALSE;
        } 
    }
    else
    {
        #if DEBUG_MODE==1
        UART0_SendString("建立连接失败2"); 
        #endif
        gGPRSConnectTimer+=1+gCarThreshold.maxConnectTimeOut; 
        return FALSE;
    } 
   
}
/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd08
* 说  明：VCU发送投币用户租车请求命令到服务器
* 参  数：
* 返回值：TRUE 发送成功 FALSE 发送失败
* 其  他：
*********************************************************************************
*/
uint8 VcuGprsToServerCmd08()
{
    uint8 cmd[12]= {0};    
    uint8 ret;
    
    cmd[0]=0x08;
    cmd[1]=heartBeatSNToServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);    //此时用户ID为多少？？  
    cmd[10] = gCarInfor.tripID++; 
    DataEncryption(cmd,11);            
    cmd[11] = GetCRC(cmd,11); 
    
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,12)+12);
   
    if(TRUE == ret) 
    {    
        SendBytesToServerByGPRS(cmd,12);   
                 
    }
    return ret;    
}

/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd08
* 说  明：服务器响应VCU发送的投币用户租车请求
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 有效命令 FALSE 无效命令
* 其  他：
*********************************************************************************
*/
uint8 ServerGprsToVcuCmd08(uint8 *servercommand)
{ 
    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
    
     
    if(carIdFromServer==gCarInfor.carid)
    { 
      //投币租车请求成功
      if(servercommand[10]==0) 
      {                  
        return TRUE;        
      } 
      else 
      {          
        return FALSE;
      }
    }
    else
      return FALSE;   
}

/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd09
* 说  明：VCU发送投币用户还车请求命令到服务器
* 参  数：
* 返回值：TRUE 发送成功 FALSE 发送失败
* 其  他：
*********************************************************************************
*/
uint8 VcuGprsToServerCmd09() 
{
    uint8 cmd[31]={0},ret;
    uint32 tripMileage;
    
       
    cmd[0]=0x09;
    cmd[1]=heartBeatSNToServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);       
    
    cmd[10] = gCarInfor.tripCoin;
    
    memcpy(&cmd[11],(uint8*)&gCarInfor.tripMileage,4);
    
    memcpy(&cmd[15],(uint8*)&gCarInfor.tripTime,2);
    cmd[17] = gCarInfor.tripID;
    memcpy(&cmd[18],(uint8*)&gCarInfor.lLatitude1,2);
    memcpy(&cmd[20],(uint8*)&gCarInfor.lLatitude2,4);
    memcpy(&cmd[24],(uint8*)&gCarInfor.lLongitude1,2);
    memcpy(&cmd[26],(uint8*)&gCarInfor.lLongitude2,4);
    
    DataEncryption(cmd,30);
    cmd[30] = GetCRC(cmd,30);
    
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,31)+31);
   
    if(TRUE == ret) 
    {    
        SendBytesToServerByGPRS(cmd,31);                    
    }
    
    gCarThreshold.totalMileage += (uint32)(gCarInfor.tripMileage/1000.0);
    gCarThresholdEepromUpdateFlag=1;
    return ret;     
    
      
}


/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd09
* 说  明：服务器响应VCU发送的投币用户还车请求
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 有效命令 FALSE 无效命令
* 其  他：
*********************************************************************************
*/

uint8 ServerGprsToVcuCmd09(uint8 *servercommand)
{  

    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
    
    //if(carIdFromServer==gCarInfor.carid && userIdFromServer ==gCarInfor.userid) 
     if(carIdFromServer==gCarInfor.carid)
    {   
        //投币还车请求成功
      if(servercommand[10]==0) 
      {                     
         return TRUE;        
      }
      else
      {
         return FALSE;
      }
        
    } 
    else
    {
        return FALSE;
    }
    
}


/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd0A
* 说  明：服务器对VCU发送车辆解锁命令
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 有效命令 FALSE 无效命令
* 其  他：
*********************************************************************************
*/
uint8 ServerGprsToVcuCmd0A(uint8 *servercommand)
{
    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
    
    if(carIdFromServer==gCarInfor.carid) 
    {        
      heartBeatSNFromServer = servercommand[1]; 
    }
    #if DEBUG_MODE==1
    UART0_SendString("手机解锁请求"); 
    #endif
    return TRUE;
}

/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd0A
* 说  明：VCU发送解锁成功与否到服务器
* 参  数：uint8 state  ServerGprsToVcuCmd0A失败或者成功标志
* 返回值：TRUE 发送成功 FALSE 发送失败
* 其  他：
*********************************************************************************
*/
uint8 VcuGprsToServerCmd0A()
{
    uint8 cmd[12]= {0};    
    uint8 ret;
    
    
    cmd[0]=0x0A;
    cmd[1]=heartBeatSNFromServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);  
    
   if(carIdFromServer != gCarInfor.carid) 
   {
      cmd[10]=CARID_ERROR;
   }
   else 
   {
     gCarInfor.destState = MobileRenting;
     gCarInfor.destStateSource = StateFromServer;              
     if(CarStateChange()==TRUE)
     {                              
        cmd[10]=CAR_OP_SUCC;
        gCarInfor.userid = userIdFromServer;           
        memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
        memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);  
     }        
     else 
     {
        cmd[10]=gCarInfor.state+0x80; //如果为失败状态，则返回当前状态，并将当前的车辆ID和用户ID返回给服务器    
     }       
   }
    
    DataEncryption(cmd,11);
    cmd[11]=GetCRC(cmd,11);
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,12)+12);;
   
    if(TRUE == ret) 
    {    
        SendBytesToServerByGPRS(cmd,12);           
    }    
    return ret;
}


/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd0B
* 说  明：VCU发送用户蓝牙租车请求命令到服务器
* 参  数：
* 返回值：TRUE 发送成功 FALSE 发送失败
* 其  他：
*********************************************************************************
*/
uint8 VcuGprsToServerCmd0B()
{
    uint8 cmd[12]= {0};    
    uint8 ret;
    
    cmd[0]=0x0B;
    cmd[1]=heartBeatSNToServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);    
    cmd[10] = gCarInfor.tripID++; 
    DataEncryption(cmd,11);            
    cmd[11] = GetCRC(cmd,11); 
    
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,12)+12);
   
    if(TRUE == ret) 
    {    
        SendBytesToServerByGPRS(cmd,12);   
                 
    }
    return ret;    
}

/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd0B
* 说  明：服务器响应VCU发送的用户蓝牙租车请求
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 有效命令 FALSE 无效命令
* 其  他：
*********************************************************************************
*/
uint8 ServerGprsToVcuCmd0B(uint8 *servercommand)
{ 
    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
    
     
    if(carIdFromServer==gCarInfor.carid)
    { 
      //蓝牙租车请求成功
      if(servercommand[10]==0) 
      {                  
        return TRUE;        
      } 
      else 
      {          
        return FALSE;
      }
    }
    else
      return FALSE;   
}

/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd0C
* 说  明：服务器对VCU设备发送参数查询命令
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 有效命令 FALSE 无效命令
* 其  他：
*********************************************************************************
*/
uint8 ServerGprsToVcuCmd0C(uint8 *servercommand)
{  
    
    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
    
    
    if(carIdFromServer==gCarInfor.carid) 
    {
        if(heartBeatSNFromServer<=servercommand[1]) 
        {
           heartBeatSNFromServer = servercommand[1]; 
        } 
        else 
        {
          return FALSE;
        } 
        return TRUE;
    } 
    else
    {
        return FALSE;
    }
  
}
/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd0C
* 说  明：VCU发送参数数据到服务器
* 参  数：
* 返回值：TRUE 发送成功 FALSE 发送失败
* 其  他：
*********************************************************************************
*/

uint8 VcuGprsToServerCmd0C()
{
    uint8 cmd[58]= {0};    
    uint8 ret;
    
  
     
    cmd[0]=0x0C;
    cmd[1]=heartBeatSNFromServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);  
    
    cmd[10]= gCarPara.hallSensorPara;
    memcpy(&cmd[11],(uint8*)&gCarPara.tireDiameter,2);
    memcpy(&cmd[13],(uint8*)&gCarPara.softwareVersion,2);
    memcpy(&cmd[15],(uint8*)&gCarPara.hardwareVersion,2);
   
    cmd[17] = gCarThreshold.batLowLevel;
    cmd[18] = gCarThreshold.batTempLevel;
    cmd[19] = gCarThreshold.operationMode;                
    memcpy(&cmd[20],(uint8*)&gCarThreshold.perCoinMetre,2);
    memcpy(&cmd[22],(uint8*)&gCarThreshold.perCoinTime,2);
    cmd[24] = gCarThreshold.shortHeartTime;
    cmd[25] = gCarThreshold.longHeartTime;
    
    memcpy(&cmd[26],(uint8*)&gCarThreshold.LTLatitude1,2);
    memcpy(&cmd[28],(uint8*)&gCarThreshold.LTLatitude2,4);
    memcpy(&cmd[32],(uint8*)&gCarThreshold.LTLongitude1,2);
    memcpy(&cmd[34],(uint8*)&gCarThreshold.LTLongitude2,4);
    
    memcpy(&cmd[38],(uint8*)&gCarThreshold.RBLatitude1,2);
    memcpy(&cmd[40],(uint8*)&gCarThreshold.RBLatitude2,4);
    memcpy(&cmd[44],(uint8*)&gCarThreshold.RBLongitude1,2);
    memcpy(&cmd[46],(uint8*)&gCarThreshold.RBLongitude2,4);
    
    cmd[50] = gCarThreshold.stopTimeout;
    cmd[51] = gCarThreshold.maxCoin;
    
    memcpy(&cmd[52],(uint8*)&gCarPara.motorPower,2);
    
    cmd[54] = gCarThreshold.maxSpeed;
    memcpy(&cmd[55],(uint8*)&gCarThreshold.maxConnectTimeOut,2);
    DataEncryption(cmd,57);
    cmd[57] = GetCRC(cmd,57);
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,58)+58);
   
    if(TRUE == ret) 
    {    
        SendBytesToServerByGPRS(cmd,58);           
    }    
    return ret;
}

/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd0D
* 说  明：服务器对VCU设备发送设备开关状态查询命令
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 有效命令 FALSE 无效命令
* 其  他：
*********************************************************************************
*/

uint8 ServerGprsToVcuCmd0D(uint8 *servercommand)
{  
    
    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
    
   
    if(carIdFromServer==gCarInfor.carid)
    {
        if(heartBeatSNFromServer<=servercommand[1]) 
        {
           heartBeatSNFromServer = servercommand[1]; 
        } 
        else 
        {
          return FALSE;
        } 
        return TRUE;
    } 
    else
    {
        return FALSE;
    }
    
}


/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd0D
* 说  明：VCU发送设备开关状态数据到服务器
* 参  数：
* 返回值：TRUE 发送成功 FALSE 发送失败
* 其  他：
*********************************************************************************
*/

uint8 VcuGprsToServerCmd0D()
{
    uint8 cmd[13]= {0};    
    uint8 ret;
    
  
    cmd[0]=0x0D;
    cmd[1]=heartBeatSNFromServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);  
    cmd[10] = device.data;
    cmd[11] = gCarInfor.accPedal;  
    DataEncryption(cmd,12);      
    cmd[12]=GetCRC(cmd,12);
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,13)+13);
   
    if(TRUE == ret) 
    {    
        SendBytesToServerByGPRS(cmd,13);           
    }    
    return ret;
}

/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd0E
* 说  明：服务器对VCU发送车辆预订命令
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 有效命令 FALSE 无效命令
* 其  他：
*********************************************************************************
*/
uint8 ServerGprsToVcuCmd0E(uint8 *servercommand)
{
    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
    
    if(carIdFromServer==gCarInfor.carid) 
    {        
      heartBeatSNFromServer = servercommand[1]; 
    }
    #if DEBUG_MODE==1
    UART0_SendString("手机预约请求"); 
    #endif
    return TRUE;
}

/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd0E
* 说  明：VCU发送预定成功与否到服务器
* 参  数：uint8 state  ServerGprsToVcuCmd0E失败或者成功标志
* 返回值：TRUE 发送成功 FALSE 发送失败
* 其  他：
*********************************************************************************
*/
uint8 VcuGprsToServerCmd0E()
{
    uint8 cmd[12]= {0};    
    uint8 ret;
    
    
    cmd[0]=0x0E;
    cmd[1]=heartBeatSNFromServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);  
    
    
    //carid不正确
    if(carIdFromServer != gCarInfor.carid) 
    {
      cmd[10]=CARID_ERROR;
    }
    else 
    {
       gCarInfor.destState = CarOrder;
       gCarInfor.destStateSource = StateFromServer;             
       if(CarStateChange()==TRUE)
       {              
          cmd[10]=CAR_OP_SUCC;
          gCarInfor.userid = userIdFromServer; 
          gCarInfor.tripTime=0; 
          memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
          memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);  
       }            
       else 
       {
          cmd[10]=gCarInfor.state+0x80; //如果为失败状态，则返回当前状态，并将当前的车辆ID和用户ID返回给服务器    
       }  
    }
    
    DataEncryption(cmd,11);
    cmd[11]=GetCRC(cmd,11);
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,12)+12);;
   
    if(TRUE == ret) 
    {    
        SendBytesToServerByGPRS(cmd,12);           
    }    
    return ret;
}
/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd0F
* 说  明：服务器对VCU发送结束车辆预订命令
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 有效命令 FALSE 无效命令
* 其  他：
*********************************************************************************
*/

uint8 ServerGprsToVcuCmd0F(uint8 *servercommand)
{
    uint32 carid=0,userid=0;
    
    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
    
   
    if(carIdFromServer==gCarInfor.carid)
    {
        if(heartBeatSNFromServer<=servercommand[1]) 
        {
           heartBeatSNFromServer = servercommand[1]; 
        } 
        else 
        {
          return FALSE;
        }  
    }   
    #if DEBUG_MODE==1
    UART0_SendString("结束预约请求"); 
    #endif   
    return TRUE;   
}

/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd0F
* 说  明：VCU发送结束预定成功与否到服务器
* 参  数：
* 返回值：TRUE 发送成功 FALSE 发送失败
* 其  他：
*********************************************************************************
*/
uint8 VcuGprsToServerCmd0F()
{
    uint8 cmd[12]= {0};    
    uint8 ret;
    
    
    cmd[0]=0x0F;
    cmd[1]=heartBeatSNFromServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);  
    
       
    //userid和carid不正确
    if(carIdFromServer != gCarInfor.carid) 
    {
      cmd[10]=CARID_ERROR;
    }     
    else 
    {
     gCarInfor.destState = CarReady;
     gCarInfor.destStateSource = StateFromServer;             
     if(CarStateChange()==TRUE)          
     {
        cmd[10]=CAR_OP_SUCC; 
        gCarInfor.userid = 0;           
     } 
     else 
     {
        cmd[10]=gCarInfor.state+0x80; //如果为失败状态，则返回当前状态，并将当前的车辆ID和用户ID返回给服务器    
     } 
    }
    
    
    DataEncryption(cmd,11);
    cmd[11]=GetCRC(cmd,11);
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,12)+12);
   
    if(TRUE == ret) 
    {    
        SendBytesToServerByGPRS(cmd,12);           
    }    
    return ret;
}

/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd11 
* 说  明：VCU向服务器发送待机心跳包
* 参  数：无
* 返回值：TRUE 发送成功  FALSE 发送失败
* 其  他：
*********************************************************************************
*/

uint8 VcuGprsToServerCmd11() 
{
    uint8 cmd[36]= {0};
    uint8 ret;
    uint16 time;
    
      
    cmd[0]=0x11;
    cmd[1]=heartBeatSNToServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);
    
    memcpy(&cmd[10],(uint8*)&gCarInfor.batVolt,2);
    memcpy(&cmd[12],(uint8*)&gCarInfor.batCurr,2);
    cmd[14] = gCarInfor.batTemp;
    cmd[15] = gCarInfor.state;
    memcpy(&cmd[16],(uint8*)&gCarThreshold.errCode,2);  
    
    //防止在待机过程中可能发生的gps漂移
    memcpy(&cmd[18],(uint8*)&gCarInfor.lLatitude1,2);
    memcpy(&cmd[20],(uint8*)&gCarInfor.lLatitude2,4);
    memcpy(&cmd[24],(uint8*)&gCarInfor.lLongitude1,2);
    memcpy(&cmd[26],(uint8*)&gCarInfor.lLongitude2,4);
    time = (uint16)(TimerA_Counter/100);
    memcpy(&cmd[30],(uint8*)&time,2);
    cmd[32] = gCarInfor.VibrateLevel;
    cmd[33] = gCarInfor.SatelliteNum;
    cmd[34] = gCarInfor.rssi;
    
    
    
    DataEncryption(cmd,35);
    cmd[35] = GetCRC(cmd,35); 
    
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,36)+36);
   
    if(TRUE == ret) 
    {
       SendBytesToServerByGPRS(cmd,36); 
    }
    return ret;
    
}

/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd11
* 说  明：服务器响应VCU发送的待机心跳包
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 有效命令 FALSE 无效命令
* 其  他：
*********************************************************************************
*/

uint8 ServerGprsToVcuCmd11(uint8 *servercommand) 
{   
    
    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
       
   if(carIdFromServer==gCarInfor.carid)
    {                
      gCarInfor.destState = servercommand[10];
      gCarInfor.destStateSource = StateFromServer;             
      if(TRUE==CarStateChange()) 
      {
        gCarInfor.userid = userIdFromServer;
      }
      
      gCarInfor.batRest = servercommand[11];
      
      return TRUE;
    } 
    else
    {
      return FALSE;
    }
    
}



/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd12
* 说  明：VCU发送用户停车超时，还车请求命令到服务器
* 参  数：
* 返回值：TRUE 发送成功 FALSE 发送失败
* 其  他：
*********************************************************************************
*/
uint8 VcuGprsToServerCmd12() 
{
    uint8 cmd[30]={0},ret;
    
   
    
    cmd[0]=0x12;
    cmd[1]=heartBeatSNToServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);

    memcpy(&cmd[10],(uint8*)&gCarInfor.tripMileage,4);
    memcpy(&cmd[14],(uint8*)&gCarInfor.tripTime,2);
    cmd[16] = gCarInfor.tripID;
    memcpy(&cmd[17],(uint8*)&gCarInfor.lLatitude1,2);
    memcpy(&cmd[19],(uint8*)&gCarInfor.lLatitude2,4);
    memcpy(&cmd[23],(uint8*)&gCarInfor.lLongitude1,2);
    memcpy(&cmd[25],(uint8*)&gCarInfor.lLongitude2,4);
    DataEncryption(cmd,29);       
    cmd[29] = GetCRC(cmd,29);
    
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,30)+30);
   
    if(TRUE == ret) 
    {    
        SendBytesToServerByGPRS(cmd,30);                    
    }
    return ret;     
    
      
}


/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd12
* 说  明：服务器响应VCU发送的停车超时，还车请求
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 有效命令 FALSE 无效命令
* 其  他：
*********************************************************************************
*/


uint8 ServerGprsToVcuCmd12(uint8 *servercommand) 
{
    
    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
      
    if(carIdFromServer==gCarInfor.carid)
    {
            
      if(servercommand[10]==0) 
      {           
        gCarInfor.maxSpeed = gCarThreshold.maxSpeed;
        gCarInfor.preMaxSpeed = gCarInfor.maxSpeed; 
        gCarInfor.userAuthority &= ~MOBILE_TIMEOUT_AUTH;
        return TRUE;
      } 
      //服务器允许该用户超时用车，不需要还车，则设置用户权限标记位
      else
      {
        gCarInfor.userAuthority |= MOBILE_TIMEOUT_AUTH;
                    
        return TRUE;
      }
    } 
    else
    {
        return FALSE;
    }
   
}

 /*
*********************************************************************************
* 函数名：VcuGprsToServerCmd13
* 说  明：VCU发送超出GPS围栏，还车请求命令到服务器
* 参  数：
* 返回值：TRUE 发送成功 FALSE 发送失败
* 其  他：
*********************************************************************************
*/
uint8 VcuGprsToServerCmd13() 
{
    uint8 cmd[30]={0},ret;
    cmd[0]=0x13;
    cmd[1]=heartBeatSNToServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);

    memcpy(&cmd[10],(uint8*)&gCarInfor.tripMileage,4);
    memcpy(&cmd[14],(uint8*)&gCarInfor.tripTime,2);
    cmd[16] = gCarInfor.tripID;
    memcpy(&cmd[17],(uint8*)&gCarInfor.lLatitude1,2);
    memcpy(&cmd[19],(uint8*)&gCarInfor.lLatitude2,4);
    memcpy(&cmd[23],(uint8*)&gCarInfor.lLongitude1,2);
    memcpy(&cmd[25],(uint8*)&gCarInfor.lLongitude2,4);
    DataEncryption(cmd,29);       
    cmd[29] = GetCRC(cmd,29);
    
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,30)+30);
   
    if(TRUE == ret) 
    {    
        SendBytesToServerByGPRS(cmd,30);                    
    }
    return ret;     

}

/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd13
* 说  明：服务器响应VCU发送超出GPS围栏，还车请求
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 有效命令 FALSE 无效命令
* 其  他：
*********************************************************************************
*/


uint8 ServerGprsToVcuCmd13(uint8 *servercommand) 
{
    
    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
    
    if(carIdFromServer==gCarInfor.carid)
    {
            
      if(servercommand[10]==0) 
      {   
        gCarInfor.userAuthority |= OUT_GPS_FENCE_NOTALLOW_AUTH;
        return TRUE;
      } 
      else if(servercommand[10]==1) 
      {
        gCarInfor.userAuthority |= OUT_GPS_FENCE_AUTH;
        return TRUE;
      }
      else
      {
        gCarInfor.userAuthority |= OUT_GPS_FENCE_LOWSPEED_AUTH;
        return TRUE;
      }
    } 
    else
    {
        return FALSE;
    }
   
}


/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd014
* 说  明：VCU发送用户蓝牙还车请求命令到服务器
* 参  数：
* 返回值：TRUE 发送成功 FALSE 发送失败
* 其  他：
*********************************************************************************
*/
uint8 VcuGprsToServerCmd14() 
{
    uint8 cmd[31]={0},ret;
    uint32 tripMileage;
    
       
    cmd[0]=0x14;
    cmd[1]=heartBeatSNToServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);       
    
    cmd[10] = gCarInfor.tripCoin;
    
    memcpy(&cmd[11],(uint8*)&gCarInfor.tripMileage,4);
    
    memcpy(&cmd[15],(uint8*)&gCarInfor.tripTime,2);
    cmd[17] = gCarInfor.tripID;
    memcpy(&cmd[18],(uint8*)&gCarInfor.lLatitude1,2);
    memcpy(&cmd[20],(uint8*)&gCarInfor.lLatitude2,4);
    memcpy(&cmd[24],(uint8*)&gCarInfor.lLongitude1,2);
    memcpy(&cmd[26],(uint8*)&gCarInfor.lLongitude2,4);
    
    DataEncryption(cmd,30);
    cmd[30] = GetCRC(cmd,30);
    
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,31)+31);
   
    if(TRUE == ret) 
    {    
        SendBytesToServerByGPRS(cmd,31);                    
    }
     
    gCarThreshold.totalMileage += (uint32)(gCarInfor.tripMileage/1000.0);
    gCarThresholdEepromUpdateFlag=1;
    return ret;     
    
      
}


/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd14
* 说  明：服务器响应VCU发送的用户蓝牙还车请求
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 有效命令 FALSE 无效命令
* 其  他：
*********************************************************************************
*/

uint8 ServerGprsToVcuCmd14(uint8 *servercommand)
{  

    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
    
    
     if(carIdFromServer==gCarInfor.carid)
    {   
        //蓝牙还车请求成功
      if(servercommand[10]==0) 
      {                     
         return TRUE;        
      }
      else
      {
         return FALSE;
      }
        
    } 
    else
    {
        return FALSE;
    }
    
}

/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd15
* 说  明：服务器对VCU发送车辆程序更新命令
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 有效命令 FALSE 无效命令
* 其  他：
*********************************************************************************
*/

uint8 ServerGprsToVcuCmd15(uint8 *servercommand)
{
    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    if(carIdFromServer==gCarInfor.carid) 
    {        
      heartBeatSNFromServer = servercommand[1]; 
    } 
    else
    {
      return FALSE;
    }
        
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
    memcpy((uint8*)&hardwareVersionFromServer,&servercommand[10],2);
    memcpy((uint8*)&softwareVersionFromServer,&servercommand[12],2);
    memcpy((uint8*)&softwareUpdateDateFromServer,&servercommand[14],2);
        
    #if DEBUG_MODE==1
    UART0_SendString("程序更新请求"); 
    #endif
    return TRUE;
}

/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd15
* 说  明：VCU发送是否进入更新状态到服务器
* 参  数：uint8 state  ServerGprsToVcuCmd15失败或者成功标志
* 返回值：TRUE 发送成功 FALSE 发送失败
* 其  他：
*********************************************************************************
*/
uint8 VcuGprsToServerCmd15()
{
    uint8 cmd[18]= {0};    
    uint8 ret;
    
    
    cmd[0]=0x15;
    cmd[1]=heartBeatSNFromServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);  
    
    ret = FALSE;
    if(carIdFromServer != gCarInfor.carid) 
    {
      cmd[10]=CARID_ERROR;
    }
   //电动机未停止的情况下不能更新
    else if(Motorstep_per_sec_count!=0) 
    {                     
       cmd[10] = SPEED_ERROR;               
    } 
    else if(gCarPara.hardwareVersion!=hardwareVersionFromServer)
    {
       cmd[10] = HW_VERSION_ERROR;                
    }    
    else if(gCarInfor.state != CarReady && gCarInfor.state != CarMaintain)
    {
       cmd[10] = gCarInfor.state+0x80;             
    }
    else 
    {     
        cmd[10]=CAR_OP_SUCC; 
        memcpy(&cmd[11],(uint8*)&gCarPara.hardwareVersion,2);
        memcpy(&cmd[13],(uint8*)&gCarPara.softwareVersion,2);
        memcpy(&cmd[15],(uint8*)&gCarPara.softwareUpdateDate,2);        
        gCarPara.softwareVersion = softwareVersionFromServer; 
        gCarPara.softwareUpdateDate = softwareUpdateDateFromServer;
        ret = TRUE;                                         
    }
    
    if(cmd[10] != CAR_OP_SUCC)
    {
        memcpy(&cmd[11],(uint8*)&gCarPara.hardwareVersion,2);
        memcpy(&cmd[13],(uint8*)&gCarPara.softwareVersion,2);
        memcpy(&cmd[15],(uint8*)&gCarPara.softwareUpdateDate,2);  
        ret = FALSE;
    }
    
    DataEncryption(cmd,17);
    cmd[17]=GetCRC(cmd,17);
    GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,18)+18);;
           
    SendBytesToServerByGPRS(cmd,18);                   
    return ret;
}



/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd31
* 说  明：服务器对VCU发送车辆维护命令
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 有效命令 FALSE 无效命令
* 其  他：
*********************************************************************************
*/

uint8 ServerGprsToVcuCmd31(uint8 *servercommand) 
{    
    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
               
    #if DEBUG_MODE==1
    UART0_SendString("进入维护请求"); 
    #endif           
    return TRUE;      
   
}

/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd31
* 说  明：VCU回复进入维护状态是否成功命令到服务器
* 参  数：
* 返回值：TRUE 发送成功 FALSE 发送失败
* 其  他：
*********************************************************************************
*/
uint8 VcuGprsToServerCmd31(uint8 *servercommand) 
{
    uint8 cmd[12]= {0};    
    uint8 ret;
    
    
    cmd[0]=0x31;
    cmd[1]=heartBeatSNFromServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);


    //userid和carid不正确
    if(carIdFromServer != gCarInfor.carid) 
    {
      cmd[10]=CARID_ERROR;
    }
    else//车辆置为维护状态
    {
      gCarInfor.destState = CarMaintain; 
      gCarInfor.destStateSource = StateFromServer;              
      if(CarStateChange()==TRUE) 
      {                    
        gCarInfor.maxSpeed = servercommand[10];
       /* if(gCarInfor.maxSpeed!=gCarThreshold.maxSpeed)
        {
         gCarThreshold.maxSpeed = gCarInfor.maxSpeed; 
         gCarThresholdEepromUpdateFlag=1;
        } */
        gCarInfor.preMaxSpeed = gCarInfor.maxSpeed;
          
        SpeedSensorCaptureInit();
        
        cmd[10]=CAR_OP_SUCC;
        gCarInfor.userid = userIdFromServer;
        memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
        memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);
        TripDataInit();
      } 
      else
      {
        cmd[10]=gCarInfor.state+0x80;
      }
    } 
    
    DataEncryption(cmd,11);      
    cmd[11]=GetCRC(cmd,11);
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,12)+12);

    if(TRUE == ret) 
    {    
      SendBytesToServerByGPRS(cmd,12);           
    }    
    return ret; 
}



/*
*********************************************************************************
* 函数名：ServerGprsToVcuCmd32
* 说  明：服务器对VCU发送结束车辆维护命令
* 参  数：uint8 *servercommand 收到的命令包
* 返回值：TRUE 有效命令 FALSE 无效命令
* 其  他：
*********************************************************************************
*/
uint8 ServerGprsToVcuCmd32(uint8 *servercommand) 
{

    uint32 carid=0,userid=0;
    
    
    if(heartBeatSNFromServer<=servercommand[1]) 
    {
       heartBeatSNFromServer = servercommand[1]; 
    } 
    else 
    {
      return FALSE;
    }
    
    memcpy((uint8*)&carid,&servercommand[2],4);
    memcpy((uint8*)&userid,&servercommand[6],4);
    #if DEBUG_MODE==1
    UART0_SendString("结束维护请求"); 
    #endif        
    return TRUE;      
    
}

/*
*********************************************************************************
* 函数名：VcuGprsToServerCmd32
* 说  明：VCU回复结束维护状态是否成功命令到服务器
* 参  数：
* 返回值：TRUE 发送成功 FALSE 发送失败
* 其  他：
*********************************************************************************
*/
uint8 VcuGprsToServerCmd32() 
{
    uint8 cmd[12]= {0};    
    uint8 ret;
    
    
    cmd[0]=0x32;
    cmd[1]=heartBeatSNFromServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);
    
    if(carIdFromServer != gCarInfor.carid) 
    {
      cmd[10]=CARID_ERROR;
    }    
    else 
    {
      gCarInfor.destState = CarReady;
      gCarInfor.destStateSource = StateFromServer;                     
      if(CarStateChange()==TRUE)
      {                   
        cmd[10]=CAR_OP_SUCC;  
        gCarInfor.userid = 0;
        gCarThreshold.totalMileage += (uint32)(gCarInfor.tripMileage/1000.0);                
        gCarThresholdEepromUpdateFlag = 1;   //保存里程等信息 
        //TripDataInit();                    
      } 
      else 
      { 
        cmd[10] = gCarInfor.state+0x80;
      }
    }
    
    DataEncryption(cmd,11);      
    cmd[11]=GetCRC(cmd,11);
    ret = GPRS_Enable_Send_Data_By_UDP_OR_TCP(StatisticsEscapeChar(cmd,12)+12);
   
    if(TRUE == ret) 
    {    
        SendBytesToServerByGPRS(cmd,12);           
    }    
    return ret; 
}


/*
*********************************************************************************
* 函数名：GetCRC
* 说  明：计算命令中的校验值
* 参  数：uint8* data 需要计算的缓冲区头指针  uint8 len  缓冲区长度
* 返回值：缓冲区各个字节的异或结果
* 其  他：
*********************************************************************************
*/
uint8 GetCRC(uint8* data,uint8 len) 
{
    uint8 crcvalue=0,i; 
            
    for(i=0;i<len;i++) 
    {
        crcvalue= crcvalue ^ data[i];
    }
    return crcvalue;
}

/*
*********************************************************************************
* 函数名：StatisticsEscapeChar
* 说  明：统计需要转义的字符数量
* 参  数：uint8* buf  需要统计的数组指针 ,uint8 length 需要统计的数组长度
* 返回值：统计结果
* 其  他：
*********************************************************************************
*/
uint8 StatisticsEscapeChar(uint8* buf,uint8 length) 
{
    uint8 counter=0,i;
    
    for(i=0;i<length;i++) 
    {
      if((buf[i]==CMD_START) || (buf[i]==CMD_END) || (buf[i]==CMD_ALIAS))
      {
        counter++;
      }
    }
    //再加上包头和包尾的两个字节
    counter+=2;
    return counter;
}

/*
*********************************************************************************
* 函数名：VcuConnectServerProcess
* 说  明：处理由VCU主动发起的命令请求
* 参  数：uint8 cmdtype 命令类型
* 返回值：cmdtype 返回命令类型表示处理完成，FALSE 表示处理失败
* 其  他：
*********************************************************************************
*/
uint8 VcuConnectServerProcess(uint8 cmdtype) 
{
    uint8 i,j;
    
    //建立连接时，重新获取种子
    gCarInfor.seed = MatrixAD[DIP_Z_AXIS_CH]; 
    gCarInfor.seed = gCarInfor.seed<<16;
    gCarInfor.seed +=MatrixAD[DIP_X_AXIS_CH];  
    
    for(i=0;i<GPRS_RETRY_TIME;i++) 
    {   
      switch(cmdtype) 
      {        
        case 0x00:
             VcuGprsToServerCmd00();
             break;
        case 0x07:
             VcuGprsToServerCmd07();
             break;
        case 0x08:
             VcuGprsToServerCmd08();
             break;
        case 0x09:
             VcuGprsToServerCmd09();
             break;
        case 0x0B:
             VcuGprsToServerCmd0B();
             break;
        case 0x11:
             VcuGprsToServerCmd11();
             break;
        case 0x12:
             VcuGprsToServerCmd12();
             break;
        case 0x13:
             VcuGprsToServerCmd13();
             break;
        case 0x14:
             VcuGprsToServerCmd14();             
             break;
        default:
             return FALSE;
      }
      
      #if DEBUG_MODE==1
      if(i==1) 
      {        
        UART0_SendString("心跳接收超时1"); 
        
      }
      
      if(i==2) 
      {        
        UART0_SendString("心跳接收超时2"); 
        
      }
      #endif 
      
      if(cmdtype==0x08) 
      {
        continue;
      }
                         
      for(j=0;j<20*(GPRS_RETRY_TIME-i);j++)  //6s  4s 2s
      {
          
          delayms(100);
          
          //连接过程中网络重启，则直接返回失败
          if(GPRS_init_flag <= GPRS_INITIAL) 
          {            
              return FALSE; 
          }
          if(ServerCommand_flag==PROC_CMD)
          {    
                                                                               
              if((cmdtype+0x80)==ServerCmdProcess(ServerCommand)) 
              {                                                            
                  //接收到成功的数据回复包后，序列号上涨
                  heartBeatSNToServer++;
                  
                  ServerCommand_flag = WAIT_CMD;  
                  return (cmdtype+0x80);
              }                      
              else
              {
                ServerCommand_flag = WAIT_CMD;
                #if DEBUG_MODE==1
                UART0_SendString("服务器请求命令"); 
                #endif  
                
                return FALSE;
              }
           }
       }
    }
    #if DEBUG_MODE==1
    UART0_SendString("心跳接收超时3");             
    #endif 
    
    if(cmdtype!=0x08) 
    { 
      if(gCarThreshold.gprsMode==TCP_MODE) 
      {        
        gGPRSConnectTimer+=1+gCarThreshold.maxConnectTimeOut;
      }
      
      return FALSE;   
    } 
    else 
    {
      return TRUE;
    }
}


uint8 DataEncryption(uint8* data,uint8 len) 
{
    uint8 i;
    
    for(i=0;i<len;i++) 
    {
      data[i]^=0x89;
    }
    return TRUE;
}

/*
*********************************************************************************
* 函数名：CarStateChange
* 说  明：处理车辆的状态切换
* 参  数：uint8 deststate  目标状态
* 返回值： 返回切换是否成功，TRUE表示切换成功 FALSE 表示当前状态无法切换
* 其  他：
*********************************************************************************
*/


uint8 CarStateChange() 
{
  switch(gCarInfor.state)
  {
    case CarReady:          
          
        #if DEBUG_MODE==1
        UART0_SendString("待机"); 
        #endif
                
        switch(gCarInfor.destState)
        {
          case CarReady:                                   
            gCarInfor.state = CarReady;        
            return TRUE;
          
          case CarOrder:            
            gCarInfor.state = CarOrder;             
            return TRUE;
          
          case CarMaintain:
            #if DEBUG_MODE==1
            if(gCarInfor.userid ==0) 
            {
              UART0_SendString("待机进入维护状态"); 
            } 
            else
            {
              UART0_SendString("主动进入维护状态"); 
            }
            #endif 
            gCarInfor.state = CarMaintain;   
            
            return TRUE;
                      
          case CoinRenting:                                  
            if(gCarInfor.destStateSource == StateFromServer) 
            {
              return FALSE; 
            } 
            else
            {              
              gCarInfor.state = CoinRenting;
              return TRUE; 
            }
         
          case MobileRenting:
                        
              gCarInfor.state = MobileRenting;
              return TRUE; 
          case CarLock:
            gCarInfor.state = CarLock;
            return TRUE;
                      
          case BTRenting:
            if(phoneVerifFlag==2) 
            {              
              gCarInfor.state = BTRenting;
              return TRUE;                     
            } 
            else
            {
              return FALSE;
            }
            
        }
    break;
    case CarOrder: 
        #if DEBUG_MODE==1
        UART0_SendString("预约"); 
        #endif          
        
        switch(gCarInfor.destState)
        {
          case CarReady:              
            if(gCarInfor.destStateSource == StateFromServer) 
            {
              gCarInfor.state = CarReady;  
              return TRUE;                 
            }
            else
            {              
              return FALSE; 
            }   
          
          case CarOrder:
            gCarInfor.state = CarOrder;            
            return TRUE;
          
          case CarMaintain:
            
            gCarInfor.state = CarMaintain; 
            #if DEBUG_MODE==1
            UART0_SendString("预约进入维护状态"); 
            #endif      
            return TRUE;
                      
          case CoinRenting:          
            
            return FALSE;
          case MobileRenting:
            
            gCarInfor.state = MobileRenting;                     
            return TRUE; 
          
          case CarLock:            
            return FALSE;
            
          
          case BTRenting:
            gCarInfor.state = BTRenting;
            return TRUE;                                           
        }
    
    
    break;
    case CarMaintain:
         #if DEBUG_MODE==1
         if(gCarInfor.userid ==0)
            UART0_SendString("自维护");
         else
            UART0_SendString("手机维护");
        #endif
         
        switch(gCarInfor.destState)
        {
          case CarReady:             
            if(gCarInfor.destStateSource == StateFromServer) 
            {
              //自维护下 不能由后台同步状态为ready
              if(gCarInfor.userid==0) 
              {
                return FALSE;    
              } 
              else
              {
                gCarInfor.state = CarReady;  
                return TRUE; 
              }
            }
            else
            {
              //自维护下，可以主动退出维护
              if(gCarInfor.userid==0 && gCarThreshold.errCode==0) 
              {                
                gCarInfor.state = CarReady;  
                return TRUE; 
              } 
              else
              {
                return FALSE; 
              }                            
            }
          case CarOrder:
                      
            return FALSE;
          
          case CarMaintain:
            gCarInfor.state = CarMaintain;     
            return TRUE;
                      
          case CoinRenting:          
            
            return FALSE;
          case MobileRenting:
            if(gCarInfor.userid != 0 && gCarThreshold.errCode==0) 
            {                
              gCarInfor.state = MobileRenting;  
              return TRUE; 
            }
            return FALSE; 
          case CarLock:            
            return FALSE;
        
          case BTRenting:
            gCarInfor.state = BTRenting;
            return TRUE;                     
        }                                                         
       
                
    break;
    case CoinRenting:                  
        #if DEBUG_MODE==1         
        UART0_SendString("投币租车");
        #endif
        switch(gCarInfor.destState)
        {
          case CarReady:                                                  
            if(gCarInfor.destStateSource == StateFromServer) 
            {
              return FALSE;                 
            }
            else
            {
              gCarInfor.state = CarReady;  
              return TRUE; 
            }                                             
          case CarOrder:                      
            return FALSE;
          
          case CarMaintain:
                        
            return FALSE;
                      
          case CoinRenting:          
            gCarInfor.state = CoinRenting; 
            return TRUE;
          case MobileRenting:
                               
            return FALSE; 
          
          case CarLock:            
            return FALSE;
            
          
          case BTRenting:
            
            return FALSE;                    
        }
    break;
    case MobileRenting:
        #if DEBUG_MODE==1             
        UART0_SendString("手机租车");
        #endif  
            
        switch(gCarInfor.destState)
        {
          case CarReady: 
            if(gCarInfor.destStateSource == StateFromServer) 
            {
              gCarInfor.state = CarReady;  
              AlarmVoice(PAY_ATTENTION_VOICE);              
              AlarmVoice(DONOT_FORGET_VOICE);
              return TRUE;                 
            }
            else
            {              
              return FALSE; 
            }
           
          case CarOrder:
                      
            return FALSE;
          
          case CarMaintain:
                                               
            //有车速，不进维护
            if(gCarInfor.speed>0) 
            {
              return FALSE;
            }
            else
            {
              gCarInfor.state = CarMaintain;
              return TRUE;   
            }       
          case CoinRenting:          
            
            return FALSE;
          case MobileRenting:
            gCarInfor.state = MobileRenting;                       
            return TRUE; 
          
          case CarLock: 
            gCarInfor.state = CarLock;  
            AlarmVoice(CAR_LOCK_VOICE);                
            return TRUE;
            
          
          case BTRenting:            
            gCarInfor.state = BTRenting;
            return TRUE;                     
        }
    break;
    case CarLock:
        #if DEBUG_MODE==1             
            UART0_SendString("手机锁车");
        #endif
        
        
        switch(gCarInfor.destState)
        {
          case CarReady:
           
            if(gCarInfor.destStateSource == StateFromServer) 
            {
              gCarInfor.state = CarReady;  
              return TRUE;                 
            }
            else
            {              
              return FALSE; 
            }
           
          case CarOrder:
                      
            return FALSE;
          
          case CarMaintain:
            gCarInfor.state = CarMaintain;  
            #if DEBUG_MODE==1
            UART0_SendString("锁车进入维护状态"); 
            #endif               
            return TRUE;
                      
          case CoinRenting:          
            
            return FALSE;
          case MobileRenting:
            gCarInfor.state = MobileRenting; 
            AlarmVoice(CAR_UNLOCK_VOICE);                    
            return TRUE; 
          
          case CarLock: 
            gCarInfor.state = CarLock;                                
            return TRUE;
        
          case BTRenting:            
            gCarInfor.state = BTRenting;
            return TRUE;                 
        }
    break;
    
    case BTRenting:        
        
        switch(gCarInfor.destState)
        {
          case CarReady:
           
            if(gCarInfor.destStateSource == StateFromServer) 
            {               
              return FALSE;                 
            }
            else
            {  
              gCarInfor.state = CarReady;             
              return TRUE; 
            }  
          case CarOrder:
                      
            return FALSE;
          
          case CarMaintain:
            return FALSE;
                      
          case CoinRenting:          
            
            return FALSE;
          case MobileRenting:
                                
            return FALSE; 
          
          case CarLock:                             
            return FALSE;
         
          case BTRenting:                        
            return TRUE;                    
        }
        
    break;
  }   
  return FALSE;
}






