#include "includes.h"



CarInfor     gCarInfor;              //����ID  ��ѹ ���� �¶� ״̬ �Լ����������Ϣ
CarThreshold gCarThreshold;       //������ֵ��������Ϣ
CarPara      gCarPara;            //�������ò���������ֱ̥������������������������Ӳ���汾��
MotorInfor   gMotorInfor;         //��������ص��������Ϣ

uint32 carIdFromServer=0,userIdFromServer=0;            //��¼���Է�������userid��carid
uint8 heartBeatSNFromServer=0;            //�ӷ�������ȡ���������ݵ����к�
uint8 heartBeatSNToServer = 0;						//�����������������������ݵ����к�
uint16 hardwareVersionFromServer=0;
uint16 softwareVersionFromServer=0;
uint16 softwareUpdateDateFromServer=0;
/*
*********************************************************************************
* ��������ServerCmdProcess
* ˵  �������մ�����������ص������
* ��  ����uint8 servercommand ��������
* ����ֵ: ��������+0x80 ��ʾ��Ӧ������ճɹ� FALSE ��ʾʧ��
* ��  ����
*********************************************************************************
*/

uint8 ServerCmdProcess(uint8  *servercommand)
{    
    uint8 state=0;
    gGPRSConnectTimer = 0;
    switch(servercommand[0])
    {     
      case 0x00://������ʱ��������Ӧ
      
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
      case 0x01://����״̬��ѯ
          if(ServerGprsToVCUCmd01(servercommand)) 
          {            
              VcuGprsToServerCmd01();
              return 0x81;
          }
          break; 
          
      case 0x02://�����豸���ؿ���
          if(ServerGprsToVcuCmd02(servercommand)) 
          {
              VcuGprsToServerCmd02();
              return 0x82;
          }
          break; 
      case 0x03://�ֻ��⳵����
          if(ServerGprsToVcuCmd03(servercommand))
          {
              VcuGprsToServerCmd03(servercommand);
              return 0x83;
          }
          break;
        
      case 0x04://�ֻ���������
          if(ServerGprsToVcuCmd04(servercommand))
          {
              VcuGprsToServerCmd04();
              return 0x84;
          }
          break;
      case 0x05://�ֻ���������
          if(ServerGprsToVcuCmd05(servercommand))
          {
              VcuGprsToServerCmd05();
              return 0x85;
          }
          break;        
      case 0x06://��ʼ����������
          if(ServerGprsToVcuCmd06(servercommand))
          {
              VcuGprsToServerCmd06();
              return 0x86;
          }
          break;
      case 0x07://�����������������������
           if(ServerGprsToVcuCmd07(servercommand)) 
           {
              return 0x87;
           }
           break;
      case 0x08://Ͷ���⳵��������
           if(ServerGprsToVcuCmd08(servercommand)) 
           {
              return 0x88;
           }
           break;       
      case 0x09://Ͷ���⳵���������󻹳�����
           if(ServerGprsToVcuCmd09(servercommand)) 
           {
              return 0x89;
           }
           break;
      case 0x0A://�ֻ�����������
          if(ServerGprsToVcuCmd0A(servercommand))
          {
              VcuGprsToServerCmd0A();
              return 0x8A;
          }
          break;
      case 0x0B://�ֻ������⳵����
          if(ServerGprsToVcuCmd0B(servercommand))
          {              
              return 0x8B;
          }
          break;          
      case 0x0C: //��ʼ�������Լ����ò�����ѯ
          if(ServerGprsToVcuCmd0C(servercommand))
          {
              VcuGprsToServerCmd0C();
              return 0x8C;
          }
          break;
      case 0x0D://�豸����״̬��ѯ
          if(ServerGprsToVcuCmd0D(servercommand))
          {
              VcuGprsToServerCmd0D();
              return 0x8D;
          }
          break;      
      case 0x0E://����Ԥ��
          if(ServerGprsToVcuCmd0E(servercommand)) 
          {            
            VcuGprsToServerCmd0E();
            return 0x8E;          
          }
          break;
      case 0x0F://��������Ԥ��
          if(ServerGprsToVcuCmd0F(servercommand))
          {
              VcuGprsToServerCmd0F();                          
                              
              return 0x8F;
          }
          break;            
      case 0x11://ֻ��״̬��ȷʱ �Ž��д���ʱ��������Ӧ
      
          if((gCarInfor.state == CarOrder)||(gCarInfor.state == CarReady)) 
          {            
            if(ServerGprsToVcuCmd11(servercommand))
            {              
                return 0x91;
            }
          }
          break;
      case 0x12: //ͣ����ʱ��������������
          if(ServerGprsToVcuCmd12(servercommand))
          {
              return 0x92;
          }       
          break;           
       case 0x13: //����GPSΧ����������������
          if(ServerGprsToVcuCmd13(servercommand))
          {
              return 0x93;
          }       
          break;
      case 0x14://�ֻ�������������
          if(ServerGprsToVcuCmd14(servercommand))
          {              
              return 0x94;
          }
          break;
      case 0x15://��̨�������ָ��
          if(ServerGprsToVcuCmd15(servercommand))
          {              
              if(VcuGprsToServerCmd15()) 
              {
                WriteCarPara();
                delayms(100);
                WriteBootFlag(GPRS_UPDATE_MODE);
                delayms(100);
                
                //�ر������жϣ�����Զ�̳������ģʽ
                DisableInterrupts;
                while(1);
              }
              return 0x95;
          }
          break;
              
      case 0x31://����ά������
          if(ServerGprsToVcuCmd31(servercommand))
          {
              VcuGprsToServerCmd31(servercommand);
              return 0xB1;
          }
          break;
      case 0x32: //��������ά������
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
* ��������VcuGprsToServerCmd00
* ˵  ��������������ͱ�����������
* ��  ������
* ����ֵ��
* ��  ����
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
  cmd[36] = (uint8)(gCarInfor.tripStopOverTimer/5.0);     //��5��Ϊ��λ���ϴ�    
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
* ��������ServerGprsToVcuCmd00 
* ˵  �����������ظ�VCU�ı�����ʱ������
* ��  ����uint8 *servercommand �յ��������
* ����ֵ�TTRUE ��Ч����  FALSE ��Ч����
* ��  ����
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
* ��������VcuGprsToServerCmd01
* ˵  ����VCU�Է�����������Ϣ��ѯ�������Ӧ
* ��  ������
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
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
* ��������ServerGprsToVCUCmd01
* ˵  ������������VCU������Ϣ��ѯ����
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
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
* ��������VcuGprsToServerCmd02
* ˵  ����VCU�ظ��������豸���ؿ���ָ��
* ��  �����������޻ظ�
* ����ֵ��
* ��  ����
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
* ��������ServerGprsToVcuCmd02
* ˵  ������������VCU�豸���ؽ��п�������
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ���óɹ� FALSE ����ʧ��
* ��  ����
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
* ��������VcuGprsToServerCmd03
* ˵  ����VCU�ظ��������⳵����
* ��  �����������޻ظ�
* ����ֵ��
* ��  ����
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
    
    //userid��carid����ȷ
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
            //��һ���ֻ��⳵���û�����ϵ�ʱ�����Ų���ָ������
            if(gCarThreshold.userLevel==0 && (gCarInfor.Event&TEACH_VOICE_EVENT)==0)
            {               
              gCarInfor.Event |= TEACH_VOICE_EVENT;
              AlarmVoice(FIRST_OP_VOICE);
              
              #if DEBUG_MODE==1
              UART0_SendString("����������ʼ����"); 
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
            UART0_SendString("�ֻ��⳵ʧ��"); 
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
    //�״��⳵�Ȳ����꣬������ʽ����
    if(gCarInfor.state == CarReady) 
    {      
      delayms(13000);
      
      gCarInfor.state = MobileRenting; 
    }
    return ret;     
}


/*
*********************************************************************************
* ��������ServerGprsToVcuCmd03
* ˵  ������������VCU�豸�����ֻ��⳵����
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ���óɹ� FALSE ����ʧ��
* ��  ����
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
    UART0_SendString("�ֻ��⳵����"); 
    #endif
    return TRUE;      
}


/*
*********************************************************************************
* ��������VcuGprsToServerCmd04
* ˵  ����VCU�ظ���������������
* ��  �����������޻ظ�
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
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
          
    //userid��carid����ȷ
    if(carIdFromServer != gCarInfor.carid) 
    {
      cmd[10] = CARID_ERROR;
    } 
    //�綯��δֹͣ������²��ܻ���
    else if(Motorstep_per_sec_count!=0) 
    {                     
       cmd[10] = SPEED_ERROR;
       AlarmVoice(CAR_FULL_STOP_VOICE);
        
    } 
   /* else if((gCarInfor.dipAngle>6)||(gCarInfor.dipAngle<-6)||(gCarInfor.fabAngle>10)||(gCarInfor.fabAngle<-10))  //������Ǵ���6�� ǰ����Ǵ���10�� ���ܻ���   
    {
       cmd[10] = DIPANGLE_ERROR;              
       AlarmVoice(CAR_TILT3_VOICE);
    } */
    //���ֻ��⳵ �»���
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
* ��������ServerGprsToVcuCmd04
* ˵  ������������VCU�豸�����ֻ���������
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ���óɹ� FALSE ����ʧ��
* ��  ����
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
    UART0_SendString("�ֻ���������"); 
    #endif
    return TRUE;      
  
}

/*
*********************************************************************************
* ��������ServerGprsToVcuCmd05
* ˵  ������������VCU���ͳ�����������
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
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
    UART0_SendString("�ֻ���������"); 
    #endif
    return TRUE;
}

/*
*********************************************************************************
* ��������VcuGprsToServerCmd05
* ˵  ����VCU���������ɹ���񵽷�����
* ��  ����uint8 state  ServerGprsToVcuCmd05ʧ�ܻ��߳ɹ���־
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
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
    }//�綯��δֹͣ������²�������
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
        cmd[10]=gCarInfor.state+0x80; //���Ϊʧ��״̬���򷵻ص�ǰ״̬��������ǰ�ĳ���ID���û�ID���ظ�������    
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
* ��������VcuGprsToServerCmd06
* ˵  ���VVCU�ظ���������ʼ��������������
* ��  ����
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
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
* ��������ServerGprsToVcuCmd06
* ˵  ������������VCU�豸�������ò���
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ���óɹ� FALSE ����ʧ��
* ��  ����
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
       //���ò�����ֵ
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
* ��������VcuGprsToServerCmd07
* ˵  ����VCU���ͽ����������󵽷�����
* ��  ����
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
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
* ��������ServerGprsToVcuCmd07
* ˵  ������������ӦVCU���͵Ľ�����������
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
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
        //���������֤
        BCMSecurityAccess(seed,key);
                    
        if(key[0]==servercommand[44] && key[1]==servercommand[45] && key[2]==servercommand[46] && key[3]==servercommand[47]) 
        {
            //���ò�����ֵ
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
                  //��MCU���ֻ��⳵����ά����Ա����¸�λʱ����Ҫ��ԭ����ָ̻�����
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
            //Ͷ�һ���������������£�������ٲ��ӷ�������ȡ
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
            gCarInfor.tripStopOverTimer = servercommand[60];           //������ͣ����ʱʱ���¼
            memcpy((uint8*)&gCarInfor.tripStopTime,&servercommand[61],2);     //�����⳵�ۼƵ�ͣ��ʱ��
            
            if(gCarThreshold.severMode!=servercommand[63]) 
            {              
              gCarThreshold.severMode = servercommand[63];                //����һ��Ŀ��������ֶΣ��ɺ�̨�����л� 
              #if DEBUG_MODE==1
              UART0_SendString("�л�������"); 
              #endif
              GPRS_init_flag = GPRS_RESTART;
              gCarThresholdEepromUpdateFlag=1;
              return FALSE;
            }
            
            if(gCarThreshold.gprsMode != servercommand[64])                //����һ��gprsģʽ�ֶΣ��ɺ�̨�����л� 
            {
              gCarThreshold.gprsMode = servercommand[64];                //����һ��Ŀ��������ֶΣ��ɺ�̨�����л� 
              #if DEBUG_MODE==1
              UART0_SendString("�л�����ģʽ"); 
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
            UART0_SendString("��������ʧ��1"); 
            #endif
            gGPRSConnectTimer+=1+gCarThreshold.maxConnectTimeOut; 
            return FALSE;
        } 
    }
    else
    {
        #if DEBUG_MODE==1
        UART0_SendString("��������ʧ��2"); 
        #endif
        gGPRSConnectTimer+=1+gCarThreshold.maxConnectTimeOut; 
        return FALSE;
    } 
   
}
/*
*********************************************************************************
* ��������VcuGprsToServerCmd08
* ˵  ����VCU����Ͷ���û��⳵�������������
* ��  ����
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
*********************************************************************************
*/
uint8 VcuGprsToServerCmd08()
{
    uint8 cmd[12]= {0};    
    uint8 ret;
    
    cmd[0]=0x08;
    cmd[1]=heartBeatSNToServer;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);    //��ʱ�û�IDΪ���٣���  
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
* ��������ServerGprsToVcuCmd08
* ˵  ������������ӦVCU���͵�Ͷ���û��⳵����
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
*********************************************************************************
*/
uint8 ServerGprsToVcuCmd08(uint8 *servercommand)
{ 
    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
    
     
    if(carIdFromServer==gCarInfor.carid)
    { 
      //Ͷ���⳵����ɹ�
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
* ��������VcuGprsToServerCmd09
* ˵  ����VCU����Ͷ���û������������������
* ��  ����
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
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
* ��������ServerGprsToVcuCmd09
* ˵  ������������ӦVCU���͵�Ͷ���û���������
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
*********************************************************************************
*/

uint8 ServerGprsToVcuCmd09(uint8 *servercommand)
{  

    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
    
    //if(carIdFromServer==gCarInfor.carid && userIdFromServer ==gCarInfor.userid) 
     if(carIdFromServer==gCarInfor.carid)
    {   
        //Ͷ�һ�������ɹ�
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
* ��������ServerGprsToVcuCmd0A
* ˵  ������������VCU���ͳ�����������
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
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
    UART0_SendString("�ֻ���������"); 
    #endif
    return TRUE;
}

/*
*********************************************************************************
* ��������VcuGprsToServerCmd0A
* ˵  ����VCU���ͽ����ɹ���񵽷�����
* ��  ����uint8 state  ServerGprsToVcuCmd0Aʧ�ܻ��߳ɹ���־
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
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
        cmd[10]=gCarInfor.state+0x80; //���Ϊʧ��״̬���򷵻ص�ǰ״̬��������ǰ�ĳ���ID���û�ID���ظ�������    
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
* ��������VcuGprsToServerCmd0B
* ˵  ����VCU�����û������⳵�������������
* ��  ����
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
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
* ��������ServerGprsToVcuCmd0B
* ˵  ������������ӦVCU���͵��û������⳵����
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
*********************************************************************************
*/
uint8 ServerGprsToVcuCmd0B(uint8 *servercommand)
{ 
    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
    
     
    if(carIdFromServer==gCarInfor.carid)
    { 
      //�����⳵����ɹ�
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
* ��������ServerGprsToVcuCmd0C
* ˵  ������������VCU�豸���Ͳ�����ѯ����
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
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
* ��������VcuGprsToServerCmd0C
* ˵  ����VCU���Ͳ������ݵ�������
* ��  ����
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
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
* ��������ServerGprsToVcuCmd0D
* ˵  ������������VCU�豸�����豸����״̬��ѯ����
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
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
* ��������VcuGprsToServerCmd0D
* ˵  ����VCU�����豸����״̬���ݵ�������
* ��  ����
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
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
* ��������ServerGprsToVcuCmd0E
* ˵  ������������VCU���ͳ���Ԥ������
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
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
    UART0_SendString("�ֻ�ԤԼ����"); 
    #endif
    return TRUE;
}

/*
*********************************************************************************
* ��������VcuGprsToServerCmd0E
* ˵  ����VCU����Ԥ���ɹ���񵽷�����
* ��  ����uint8 state  ServerGprsToVcuCmd0Eʧ�ܻ��߳ɹ���־
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
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
    
    
    //carid����ȷ
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
          cmd[10]=gCarInfor.state+0x80; //���Ϊʧ��״̬���򷵻ص�ǰ״̬��������ǰ�ĳ���ID���û�ID���ظ�������    
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
* ��������ServerGprsToVcuCmd0F
* ˵  ������������VCU���ͽ�������Ԥ������
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
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
    UART0_SendString("����ԤԼ����"); 
    #endif   
    return TRUE;   
}

/*
*********************************************************************************
* ��������VcuGprsToServerCmd0F
* ˵  ����VCU���ͽ���Ԥ���ɹ���񵽷�����
* ��  ����
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
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
    
       
    //userid��carid����ȷ
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
        cmd[10]=gCarInfor.state+0x80; //���Ϊʧ��״̬���򷵻ص�ǰ״̬��������ǰ�ĳ���ID���û�ID���ظ�������    
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
* ��������VcuGprsToServerCmd11 
* ˵  ����VCU����������ʹ���������
* ��  ������
* ����ֵ��TRUE ���ͳɹ�  FALSE ����ʧ��
* ��  ����
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
    
    //��ֹ�ڴ��������п��ܷ�����gpsƯ��
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
* ��������ServerGprsToVcuCmd11
* ˵  ������������ӦVCU���͵Ĵ���������
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
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
* ��������VcuGprsToServerCmd12
* ˵  ����VCU�����û�ͣ����ʱ�������������������
* ��  ����
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
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
* ��������ServerGprsToVcuCmd12
* ˵  ������������ӦVCU���͵�ͣ����ʱ����������
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
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
      //������������û���ʱ�ó�������Ҫ�������������û�Ȩ�ޱ��λ
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
* ��������VcuGprsToServerCmd13
* ˵  ����VCU���ͳ���GPSΧ���������������������
* ��  ����
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
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
* ��������ServerGprsToVcuCmd13
* ˵  ������������ӦVCU���ͳ���GPSΧ������������
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
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
* ��������VcuGprsToServerCmd014
* ˵  ����VCU�����û����������������������
* ��  ����
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
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
* ��������ServerGprsToVcuCmd14
* ˵  ������������ӦVCU���͵��û�������������
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
*********************************************************************************
*/

uint8 ServerGprsToVcuCmd14(uint8 *servercommand)
{  

    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
    
    
     if(carIdFromServer==gCarInfor.carid)
    {   
        //������������ɹ�
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
* ��������ServerGprsToVcuCmd15
* ˵  ������������VCU���ͳ��������������
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
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
    UART0_SendString("�����������"); 
    #endif
    return TRUE;
}

/*
*********************************************************************************
* ��������VcuGprsToServerCmd15
* ˵  ����VCU�����Ƿ�������״̬��������
* ��  ����uint8 state  ServerGprsToVcuCmd15ʧ�ܻ��߳ɹ���־
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
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
   //�綯��δֹͣ������²��ܸ���
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
* ��������ServerGprsToVcuCmd31
* ˵  ������������VCU���ͳ���ά������
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
*********************************************************************************
*/

uint8 ServerGprsToVcuCmd31(uint8 *servercommand) 
{    
    memcpy((uint8*)&carIdFromServer,&servercommand[2],4);
    memcpy((uint8*)&userIdFromServer,&servercommand[6],4);
               
    #if DEBUG_MODE==1
    UART0_SendString("����ά������"); 
    #endif           
    return TRUE;      
   
}

/*
*********************************************************************************
* ��������VcuGprsToServerCmd31
* ˵  ����VCU�ظ�����ά��״̬�Ƿ�ɹ����������
* ��  ����
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
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


    //userid��carid����ȷ
    if(carIdFromServer != gCarInfor.carid) 
    {
      cmd[10]=CARID_ERROR;
    }
    else//������Ϊά��״̬
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
* ��������ServerGprsToVcuCmd32
* ˵  ������������VCU���ͽ�������ά������
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
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
    UART0_SendString("����ά������"); 
    #endif        
    return TRUE;      
    
}

/*
*********************************************************************************
* ��������VcuGprsToServerCmd32
* ˵  ����VCU�ظ�����ά��״̬�Ƿ�ɹ����������
* ��  ����
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
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
        gCarThresholdEepromUpdateFlag = 1;   //������̵���Ϣ 
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
* ��������GetCRC
* ˵  �������������е�У��ֵ
* ��  ����uint8* data ��Ҫ����Ļ�����ͷָ��  uint8 len  ����������
* ����ֵ�������������ֽڵ������
* ��  ����
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
* ��������StatisticsEscapeChar
* ˵  ����ͳ����Ҫת����ַ�����
* ��  ����uint8* buf  ��Ҫͳ�Ƶ�����ָ�� ,uint8 length ��Ҫͳ�Ƶ����鳤��
* ����ֵ��ͳ�ƽ��
* ��  ����
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
    //�ټ��ϰ�ͷ�Ͱ�β�������ֽ�
    counter+=2;
    return counter;
}

/*
*********************************************************************************
* ��������VcuConnectServerProcess
* ˵  ����������VCU�����������������
* ��  ����uint8 cmdtype ��������
* ����ֵ��cmdtype �����������ͱ�ʾ������ɣ�FALSE ��ʾ����ʧ��
* ��  ����
*********************************************************************************
*/
uint8 VcuConnectServerProcess(uint8 cmdtype) 
{
    uint8 i,j;
    
    //��������ʱ�����»�ȡ����
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
        UART0_SendString("�������ճ�ʱ1"); 
        
      }
      
      if(i==2) 
      {        
        UART0_SendString("�������ճ�ʱ2"); 
        
      }
      #endif 
      
      if(cmdtype==0x08) 
      {
        continue;
      }
                         
      for(j=0;j<20*(GPRS_RETRY_TIME-i);j++)  //6s  4s 2s
      {
          
          delayms(100);
          
          //���ӹ�����������������ֱ�ӷ���ʧ��
          if(GPRS_init_flag <= GPRS_INITIAL) 
          {            
              return FALSE; 
          }
          if(ServerCommand_flag==PROC_CMD)
          {    
                                                                               
              if((cmdtype+0x80)==ServerCmdProcess(ServerCommand)) 
              {                                                            
                  //���յ��ɹ������ݻظ��������к�����
                  heartBeatSNToServer++;
                  
                  ServerCommand_flag = WAIT_CMD;  
                  return (cmdtype+0x80);
              }                      
              else
              {
                ServerCommand_flag = WAIT_CMD;
                #if DEBUG_MODE==1
                UART0_SendString("��������������"); 
                #endif  
                
                return FALSE;
              }
           }
       }
    }
    #if DEBUG_MODE==1
    UART0_SendString("�������ճ�ʱ3");             
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
* ��������CarStateChange
* ˵  ������������״̬�л�
* ��  ����uint8 deststate  Ŀ��״̬
* ����ֵ�� �����л��Ƿ�ɹ���TRUE��ʾ�л��ɹ� FALSE ��ʾ��ǰ״̬�޷��л�
* ��  ����
*********************************************************************************
*/


uint8 CarStateChange() 
{
  switch(gCarInfor.state)
  {
    case CarReady:          
          
        #if DEBUG_MODE==1
        UART0_SendString("����"); 
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
              UART0_SendString("��������ά��״̬"); 
            } 
            else
            {
              UART0_SendString("��������ά��״̬"); 
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
        UART0_SendString("ԤԼ"); 
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
            UART0_SendString("ԤԼ����ά��״̬"); 
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
            UART0_SendString("��ά��");
         else
            UART0_SendString("�ֻ�ά��");
        #endif
         
        switch(gCarInfor.destState)
        {
          case CarReady:             
            if(gCarInfor.destStateSource == StateFromServer) 
            {
              //��ά���� �����ɺ�̨ͬ��״̬Ϊready
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
              //��ά���£����������˳�ά��
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
        UART0_SendString("Ͷ���⳵");
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
        UART0_SendString("�ֻ��⳵");
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
                                               
            //�г��٣�����ά��
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
            UART0_SendString("�ֻ�����");
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
            UART0_SendString("��������ά��״̬"); 
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






