#include "includes.h"

uint8 GPRS_init_flag = 0;
uint8 GPS_init_flag = 0;

void GPRSConnecting() 
{
     //�ϵ�����ģ��
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
    
    //��ѯgprs���������������Ѿ����ţ���ֱ�ӽ���gprs��ʼ��
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
    //ģ���ʼ�� ������call ready���ֲ��ҳ�ʼ��ʱ�䳬��35��
    if(GPRS_init_flag==GPRS_INITIAL && gGprsInitialTimer>35) 
    {                  
      
      
      //��ʼ��GPRS                
      if(Init_GPRS())
      { 
        GPRS_init_flag = GPRS_CONNECTTING; 
        GPRS_DVCFG(); 
                                              
      }               
      //��ʼ����ͨ���������ϵ��ʼ�� 
      else 
      {                        
         GPRS_init_flag = GPRS_RESTART; 
         gGPRSConnectTimer = 0;
      }
      //��������GPRS
      if(ReadTestResult(GPRS_TEST_FLAG)) 
      {
        ClearTestResult(GPRS_TEST_FLAG);
      }
      gGprsInitialTimer = 0;                  
    }
                
    //���̨��������������
    if( GPRS_init_flag==GPRS_CONNECTTING)
    {                                 
        gCarThreshold.errCode = gCarThreshold.errCode & (~GPRS_DISCONN_EXCEPTION); 
        if(gCarInfor.state == CarMaintain && gCarInfor.userid==0) 
        {                  
          //����������£��Ҳ���GPSΧ�����棬��ص�����״̬
          if((gCarThreshold.errCode & OUT_GPS_FENCE_EXCEPTION) == 0) 
          {
            gCarInfor.destState = CarReady;
            gCarInfor.destStateSource = StateFromCar;
            CarStateChange(); 
          }
        }                                                                                                               
        //��ʱ������δ��Ӧ����Ͽ�TCP\UDP���ӣ����½��г�ʼ��
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
    
                
    //���δ���ܵ�SYSSTART����������� 
    if((GPRS_init_flag==GPRS_POWERON) && (gGprsInitialTimer>45)) 
    {
      GPRS_init_flag = GPRS_RESTART; 
      gGprsInitialTimer = 0;
    }
    
    
    #if DEBUG_MODE==1
    gCarThreshold.operationMode = COIN_AND_MOBILE_OPERATION;                         
    #endif
        
    
    //�������120�������ϣ���ر�PDP��Ȼ�����³�ʼ��ģ��,���ɹ����ټ�60��
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
    
    //��̨�������������ӳ�ʱ 
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
            //����ͨѶ�쳣                                                                                             
            gCarInfor.userid = 0;
          } */          
        }                                                                                                                       
    }
    
                                            
    
    //��λ��ʱ������GPSģ��
    if(gGPSLocationTimer>30*(1+gCarInfor.SatelliteNum)) 
    {                              
        Restart_GPS();   
        gGPSLocationTimer = 0; 
        GPS_init_flag=GPS_RESTART;
    } 
}



void CoinRentingProcess()  
{
    //Ͷ���û��⳵����            
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
        UART0_SendString("Ͷ���⳵��ʼ"); 
        #endif 
    }

    //Ͷ���û���������
    if((gCarInfor.Event&COIN_RENT_END_EVENT)==COIN_RENT_END_EVENT)  //0429  ndy Ϊ�˽��������ʱ��������̵��˺����Ͷ���û�����
    {
          if(GPRS_init_flag==GPRS_CONNECTED) 
          {                    
            VcuConnectServerProcess(0x09);
          }
          
          TripDataInit();                                         
          
          #if DEBUG_MODE==1
          UART0_SendString("Ͷ���⳵����");
          #endif
          
          gCarInfor.Event &=~COIN_RENT_START_EVENT;
          gCarInfor.Event &=~COIN_RENT_END_EVENT;
          gCarThresholdEepromUpdateFlag = 1;   //������̵���Ϣ
    }
}

void PhoneBTRentingProcess() 
{
   //�����ֻ���������
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
    //4����δ�յ������ظ��������ʾ�������ӶϿ������л�������
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
    
    
    //�������Է������˵�����
    if(ServerCommand_flag==PROC_CMD)
    {
        ServerCmdProcess(ServerCommand);                    
        ServerCommand_flag = WAIT_CMD; 
        rent_freq_flag = 0;
        standby_freq_flag = 0;                  
    }
                                                  
    /***********************С��������ʱ�������ݷ���**************************/
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
    /***********************С������ʱ�������ݷ���**************************/
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
     
          //����GPSΧ���������쳣 
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
  //����¶ȴ�����δ������
  if(gCarInfor.batTemp==0xFF && VCUWorkMode == NORMAL_MODE) 
  {
     AlarmTwoVoice(PAY_ATTENTION_VOICE,TEMP_SENSEOR_FORGET_VOICE);           
  }        
                                       
  //����¶ȳ��꣬�����쳣
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
    //����������ͣ������쳣
   if(gCarInfor.batRest!=0 && (gCarInfor.batRest<=gCarThreshold.batLowLevel)) 
   {
       gCarThreshold.errCode |= LOW_BATTERY_EXCEPTION; 
    } 
    else if(gCarInfor.batRest>(gCarThreshold.batLowLevel+10)) 
    {
       gCarThreshold.errCode &= ~LOW_BATTERY_EXCEPTION;                   
    }     
    
   //����¶ȹ�����������
   if((gCarThreshold.errCode & HIGH_TEMPATURE_EXCEPTION) == HIGH_TEMPATURE_EXCEPTION) 
   {
       if(gCarInfor.speed>1) 
       {
         AlarmVoice(OVERTEMP_BATTERY1_VOICE);
       }
   }
   
   
   //��ص���������������
   if((gCarThreshold.errCode & LOW_BATTERY_EXCEPTION) == LOW_BATTERY_EXCEPTION) 
   {
       if(gCarInfor.speed>1) 
       {
         AlarmVoice(LOW_BATTERY1_VOICE);
       }
   }
   
   
   //�ص�Χ���ڲ�ʱ������߳���
    if((gCarThreshold.errCode & OUT_GPS_FENCE_EXCEPTION) == 0) 
    {
        if(gCarInfor.maxSpeed<gCarInfor.preMaxSpeed) 
        {                        
         gCarInfor.maxSpeed++;         
        }
         //����������µģ���ά��״̬���ص�Χ���ڣ���������״̬
        if(GPRS_init_flag==GPRS_CONNECTED && gCarInfor.state == CarMaintain && gCarInfor.userid==0) 
        {
          gCarInfor.destState = CarReady;
          gCarInfor.destStateSource = StateFromCar;
          CarStateChange();
        }  
    }
    else
    {        
        
        //�ӷ�������ȡGPSΧ��Ȩ��
        if((gCarInfor.userAuthority & OUT_GPS_FENCE_AUTH_VALID)==0) 
        {                                                                                                     
            if(0x93==VcuConnectServerProcess(0x13)) 
            {
               gCarInfor.userAuthority |= OUT_GPS_FENCE_AUTH_VALID;
            }
        }
     
              
        //ά����Ա���������г���GPSΧ����Ȩ�� 
        if(gCarInfor.state == CarMaintain && gCarInfor.userid!=0) 
        {
          ;
        }         
        //��ά����Ա�����������Ȩ�޽����жϣ�������ʾ����GPSΧ��
        else
        { 
        
            if((gCarInfor.userAuthority&OUT_GPS_FENCE_NOTALLOW_AUTH) == OUT_GPS_FENCE_NOTALLOW_AUTH) 
            {
                #if DEBUG_MODE==1
                UART0_SendString("����Χ������ʱͣ��"); 
                #endif 
                //��ά����Ա�⳵���ó�����������gpsΧ�����򻹳���ǿ����Ϊ��ά��״̬                     
                //�ڽ���ά��״̬ǰ�������������� ,���ȴ�10������״̬ת��
                if(gCarInfor.speed>1) 
                {                        
                  AlarmVoice(OUT_FENCE1_VOICE);                                     
                  AlarmVoice(STOPPING_CAR1_VOICE); 
                  delayms(10000);  
                }                                               
                gCarInfor.state = CarMaintain;                                
                gCarInfor.userid = 0;
            } 
            //������Χ�����У���������
            else if((gCarInfor.userAuthority&OUT_GPS_FENCE_AUTH) == OUT_GPS_FENCE_AUTH)
            {
              #if DEBUG_MODE==1
              UART0_SendString("�����Χ����������"); 
              #endif                                  
            } 
            else if((gCarInfor.userAuthority&OUT_GPS_FENCE_LOWSPEED_AUTH) == OUT_GPS_FENCE_LOWSPEED_AUTH) 
            {
              
              #if DEBUG_MODE==1
              UART0_SendString("�����Χ��������"); 
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
      
      
        //�ֻ��⳵ʱ��ͣ����ʱ��������û������г�ʱ�ó���Ȩ�ޣ���VCU��������                                         
      if((gCarInfor.tripStopOverTimer>= (gCarInfor.stopTimeout*60))&&(gCarInfor.state == MobileRenting || gCarInfor.state==CarLock)) 
      {
        //�ӷ�������ȡ��ʱͣ��Ȩ�� 
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
          //�������������ʱ�������򻹳�
          if((gCarInfor.userAuthority & MOBILE_TIMEOUT_AUTH)==0) 
          { 
            gCarInfor.state = CarReady;
            gCarInfor.userid = 0;
                                                                
            //�������ѣ��ó����ó�ʱ���ѱ�����
            AlarmVoice(RENT_OVERTIME1_VOICE); 
            
            #if DEBUG_MODE==1
            UART0_SendString("��ʱ�������"); 
            #endif             
          }
          else
          {
            #if DEBUG_MODE==1
            UART0_SendString("���賬ʱ����"); 
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
        UART0_SendString("�г�ʣ��һ����");      
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
        UART0_SendString("�г̽���");      
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
        
        UART0_SendString("����Ϊ0");      
      }
      #endif                          
      slotTime--;
            
      break;          
                              
     }                                
   
}

