/*
******************************************************************
*
*
*
******************************************************************
*/
#include "includes.h"
/*******************************CAN参数定义*********************************/
CAN_frame TxMsg;
CAN_frame RxMsg;
CAN_frame CarCanFrame;
CAN_frame CarParaFromPC;


static void MSCAN0_ConfigPort()
{

    CAN_PxOUT &=~(CAN_RXCAN_PIN+CAN_TXCAN_PIN);
    CAN_PxDIR &= ~(CAN_RXCAN_PIN+CAN_TXCAN_PIN);
    CAN_PxDIR |= CAN_TXCAN_PIN;
    

}
uint8 cancheck = 0;


uint8 canflag = 0;
uint8 VCUWorkMode=NORMAL_MODE;
uint8 canchecksum(CAN_frame temp);


void Can_Init(uint8 baudrate)
{
    MSCAN0_ConfigPort();
    CANCTL1_CANE = 1 ;
    CANCTL0_INITRQ = 1 ;   // initial request, start to initial
    while (CANCTL1_INITAK != 1 ) ;
   
    CANCTL1_LISTEN = 0 ;   // not listen mode
    CANCTL1_LOOPB =0;
    
    if(gCarPara.hardwareVersion==0x300) 
    {
      
      CANCTL1_CLKSRC = 1 ;   // 0-外部晶振  1-bus clock=24
      if (baudrate == CAN_BAUDRATE_500K)
      {
          CANBTR0_BRP = 5 ;     // prescale = 6 -5    
      }
      else
      {
          CANBTR0_BRP = 11 ;
      }
    } 
    else if(gCarPara.hardwareVersion>=0x301) 
    {
      CANCTL1_CLKSRC = 0 ;   // crystal clock, 16MHz
      if (baudrate == CAN_BAUDRATE_500K)
      {
          CANBTR0_BRP = 3 ;     // prescale = 4 -3    2-1
      }
      else
      {
          CANBTR0_BRP = 7 ;
      } 
    }
    //CANBTR0_BRP = 1 ;     // prescale = 4 -3    2-1
    //CANBTR0_BRP = 3 ;
    CANBTR1_SAMP = 0 ;     // once per bit
    CANBTR1_TSEG_10 = 4 ;  // 5tq
    CANBTR1_TSEG_20 = 1 ;  // 2tq
    //CANBTR1_TSEG_10 = 6 ;     // 7tq
    //CANBTR1_TSEG_20 = 7 ;     // 8tq
    // bit rate = ftq / num-of-tq = fclk / prescale / (syncSeg + tSeg1 + tSeg2)
    // = 16MHz / 2 / (1 + 5 + 2) = 1000Kbps

    CANIDAC_IDAM = 0 ;     /* never choose 3(close filter), or no message will be received!!!
								   two 32 bit acceptance filters*/

    //set AMR
    CANIDMR0 = 0xff ;
    CANIDMR1 = 0xff ;
    CANIDMR2 = 0xff ;
    CANIDMR3 = 0xff ;


    CANCTL0_INITRQ = 0 ;   // stop initialization, start can
    while (CANCTL1_INITAK != 0 ) ;
    while (CANCTL0_SYNCH != 1) ;

    CANRIER = 0 ;				//disable Receiver Interrupt
    CANTIER = 0 ;				//disable Transmitter Interrupt

    CANRFLG = 0xC3;
    CANRIER_RXFIE = 1 ;  //enable receiver interrupt
   // CANTFLG_TXE = 1;      //使能发送数据
   // CANTFLG  = 1;
   // CANTIER = 1;
    //	CANTFLG  = 1;
    //	CANTIER = 1;
    CarCanFrame.id=0;
    CarCanFrame.datalen=0;

}

uint8 Can_Send(CAN_frame canframe)
{

    dword tempID,idr32;
    byte temp;
    uint32 timestamp;
               
    TimeoutInit(&timestamp); 
    //等待250ms，若无法发送则can发送失败
    while (0 == CANTFLG) //	no available tx register buffer
    {
        if(TimeoutValidate(timestamp,25)==TRUE)
        {
            return FALSE;
        }
    }
    tempID = canframe.id ;
    switch(canframe.type)
    {
    case FRAME_STD_DATA :
        idr32 = (tempID << 21) ;
        break ;
    case FRAME_STD_REMOTE :
        idr32 = (tempID << 21) | 0x00100000 ;
        break ;
    case FRAME_EXT_DATA :
        idr32 = ((tempID << 1) & 0x0007fffe) | ((tempID << 3) & 0xffe00000) | 0x00180000 ;
        break ;
    case FRAME_EXT_REMOTE :
        idr32 = ((tempID << 1) & 0x0007fffe) | ((tempID << 3) & 0xffe00000) | 0x00180001 ;
        break ;
    default :
        return FALSE ;
    }


    CANTBSEL = CANTFLG ;          // choose the available TX Buffer. see <<S12MSCANV2.pdf>> page 31																														 //获得缓冲区
    // if you display CAN1TBSEL here , you will always see just one "1" is set


    CANTXIDR0 = idr32 >> 24 ;
    CANTXIDR1 = idr32 << 8 >> 24 ;
    CANTXIDR2 = idr32 << 16 >> 24 ;
    CANTXIDR3 = idr32 << 24 >> 24 ;
    CANTXDSR0 = canframe.data[0] ;
    CANTXDSR1 = canframe.data[1] ;
    CANTXDSR2 = canframe.data[2] ;
    CANTXDSR3 = canframe.data[3] ;
    CANTXDSR4 = canframe.data[4] ;
    CANTXDSR5 = canframe.data[5] ;
    CANTXDSR6 = canframe.data[6] ;
    CANTXDSR7 = canframe.data[7] ;

    CANTXDLR = canframe.datalen ;
    CANTXTBPR = 0x1 ;						// NOT USED
    /* setting the tx registers */

    temp = CANTBSEL;
    temp = temp^(temp -1); //choose the lowest bit
    CANTFLG = temp ;		  // clear TXE in order to send data in TX Buffer. see <<S12MSCANV2.pdf>> page 28

    return TRUE ;
}

/***CAN接受函数*****************************************************/
void CAN_receive(void)         //接受8个数
{
    uint8 i;
    uint32 tempID ;
   
// RxMsg.id = (CANRXIDR0<<8)+CANRXIDR1;
    tempID = *(uint32 *)&((CANRXIDR0)) ; 
    RxMsg.datalen = CANRXDLR;
    
    if (CANRXIDR1_IDE)		 // ext frame
		{
				if (CANRXIDR3_RTR) 
				{		  
						RxMsg.type= FRAME_EXT_REMOTE;
				}
				else 
				{				  
						RxMsg.type = FRAME_EXT_DATA;
				}
				RxMsg.id = ((tempID >> 1) & 0x03ffff) | ((tempID >> 21) << 18) ;
		}
		else								  // std frame
		{
				if (CANRXIDR1_SRR) 
				{
				    RxMsg.type=FRAME_STD_REMOTE;
				}
				else 
				{				  
						RxMsg.type=FRAME_STD_DATA;
				}
				RxMsg.id = (tempID >> 21);
		}

    for(i=0; i<RxMsg.datalen; i++)
    {
        RxMsg.data[i] = *(&(CANRXDSR0) + i);      
    }
    
    if(RxMsg.id == 0x01||RxMsg.id == 0x03) 
    {      
      CarParaFromPC.id = RxMsg.id;
      CarParaFromPC.datalen = RxMsg.datalen;
      memcpy(CarParaFromPC.data,RxMsg.data,8);
      CarParaFromPC.datalen = 0;
      #if DEBUG_MODE == 0
      CarParaFromPCProc();
      #endif      
    }
}

byte CANMark(void)   //超声波处理程序
{              
      if(RxMsg.id == 573)
      { 
          EffectiveObstaclesDataCompute(RxMsg.data);
         // RadarAnticollision(RxMsg.data);      
       }
       
     return canflag;
}

#if  DEBUG_MODE == 0
void CarParaFromPCProc() 
{
    CAN_frame canframe;
    static uint8 checkstate=0;
    uint8 seed[4]={0},key[4]={0},allzero[8]={0},i;
    uint16 davalue=0;
    
    uint8  sendflag=0,baseflag=0;
    FP32   volttemp;
    static uint8 voiceflag=0;   
    canframe.id = 0x004;
    canframe.type = FRAME_STD_DATA;
    canframe.datalen = 8;
    memset(canframe.data,0,8); 
    memset(allzero,0,8); 
    canframe.data[0] = CarParaFromPC.data[0];
    
    memcpy(seed,(uint8*)&gCarInfor.seed,4);
    if(checkstate !=2 && CarParaFromPC.data[0]!=0x01 && CarParaFromPC.data[0]!=0x81) 
    {
      return;
    }  
    switch(CarParaFromPC.data[0]) 
    {
    
      //////////验证过程///////////////////////////////////////////////////////////////////
      case 0x01:
            checkstate = 1;     
            canframe.id = 0x002;
            memcpy(&canframe.data[1],seed,4);    
            sendflag=1;       
            break;
      case 0x81:
            canframe.id = 0x002;
            if(checkstate==1) 
            {              
              //进行身份验证
              if(seed[0]!=0 || seed[1]!=0 || seed[2]!=0 || seed[3]!=0) 
              {              
                BCMSecurityAccess(seed,key);
                
                //验证通过
                if(CarParaFromPC.data[1]==key[0]&&CarParaFromPC.data[2]==key[1]&&CarParaFromPC.data[3]==key[2]&&CarParaFromPC.data[4]==key[3]) 
                {
                   checkstate=2;
                   
                   if(CarParaFromPC.data[5]==1) 
                   {
                      //进入配置模式
                      //AlarmVoice(PAY_ATTENTION_VOICE);
                      AlarmVoice(WRITE_PARA_VOICE);
                      VCUWorkMode = CONFIG_MODE;
                      
                   }
                   else if(CarParaFromPC.data[5]==2)
                   {
                      //进入程序更新模式
                     /* WriteBootFlag(BOOT_MODE);
                      TSCR1_TEN = 0;
                      CANRIER_RXFIE=0;
                      SCI2CR2_RIE = 0;
                      SCI1CR2_RIE = 0;
                      SCI0CR2_RIE = 0;
                      CPMUCOP = 0;
                      DisableInterrupts;
                      __asm 
                      {
                         jmp 0xC800
                      }*/
                   } 
                   else if(CarParaFromPC.data[5]==3) 
                   {
                      //进入测试模式                      
                      VCUWorkMode = TEST_MODE;                      
                      ClearNCTestResult();
                      //AlarmVoice(PAY_ATTENTION_VOICE);
                      AlarmVoice(ENTER_TEST_MODE_VOICE);
                      gCarParaEepromUpdateFlag = 1;
                        
                       
                   }
                   canframe.data[1]=1;
                }
                else
                {
                    canframe.data[1]=0;
                    checkstate = 0;
                }
              }
            } 
            else
            {
                canframe.data[1]=0;
                checkstate = 0;
            }
            sendflag=1;
            break;
            
      //////////装车参数配置过程///////////////////////////////////////////////////////////////////
      case 0x02:
            if(MemEqual(&CarParaFromPC.data[1],allzero,7)==FALSE) 
            {              
              memcpy(gCarPara.VIN,&CarParaFromPC.data[1],7);
              memcpy(&canframe.data[1],&CarParaFromPC.data[1],7);  
              
              //取VIN码的后三个字节作为车辆的唯一标示ID    
              memcpy((uint8*)&gCarInfor.carid,&gCarPara.VIN[4],3);
              gCarInfor.carid = gCarInfor.carid>>8;
              gCarParaEepromUpdateFlag = 1;                          
            } 
            sendflag=1;
            break;    
      case 0x82:
           // ReadCarPara(); 
            memcpy(&canframe.data[1],gCarPara.VIN,7);
            sendflag=1;
            break;
      case 0x03:
            if(MemEqual(&CarParaFromPC.data[1],allzero,7)==FALSE) 
            {
              memcpy(gCarPara.VCUID,&CarParaFromPC.data[1],7);
              memcpy(&canframe.data[1],&CarParaFromPC.data[1],7);  
              gCarParaEepromUpdateFlag = 1;             
            }
            sendflag=1;
            break;
      case 0x83:
            //ReadCarPara(); 
            memcpy(&canframe.data[1],gCarPara.VCUID,7);
            sendflag=1;
            break;
      case 0x04:
            if(MemEqual(&CarParaFromPC.data[1],allzero,2)==FALSE) 
            {
              memcpy((uint8*)&gCarPara.tireDiameter,&CarParaFromPC.data[1],2);
              gCarParaEepromUpdateFlag = 1;
            }
            
            if(CarParaFromPC.data[3]!=0) 
            {
              gCarPara.hallSensorPara = CarParaFromPC.data[3];
              gCarParaEepromUpdateFlag = 1;
            }
            
            if(MemEqual(&CarParaFromPC.data[4],allzero,2)==FALSE) 
            {
              memcpy((uint8*)&gCarPara.motorPower,&CarParaFromPC.data[4],2);
              gCarParaEepromUpdateFlag = 1;
            }
            
            if(MemEqual(&CarParaFromPC.data[6],allzero,2)==FALSE) 
            {
              memcpy((uint8*)&davalue,&CarParaFromPC.data[6],2);
              gCarPara.gearRatio = (FP32)davalue/100.0;
              gCarParaEepromUpdateFlag = 1;
            }
            sendflag=1;
            break;
      case 0x84:
           // ReadCarPara(); 
            memcpy(&canframe.data[1],(uint8*)&gCarPara.tireDiameter,2);
            canframe.data[3] = gCarPara.hallSensorPara;
            memcpy(&canframe.data[4],(uint8*)&gCarPara.motorPower,2);
            davalue = (uint16)(gCarPara.gearRatio*100);
            memcpy(&canframe.data[6],(uint8*)&davalue,2);
            sendflag=1;            
            break;
      case 0x05:
            if(MemEqual(&CarParaFromPC.data[1],allzero,6)==FALSE) 
            {
              memcpy((uint8*)&gCarPara.name,&CarParaFromPC.data[1],6);
              BTnameSetFlag = 1;              
              gCarParaEepromUpdateFlag = 1;
            }
            sendflag=1;           
            break;
      case 0x85:
           // ReadCarPara(); 
            memcpy(&canframe.data[1],(uint8*)&gCarPara.name,6); 
            sendflag=1;
            break;
      case 0x08: 
            AlarmVoice(DIP_CAL_VOICE);                       
            gCarPara.XAccelBase = MatrixAD[DIP_X_AXIS_CH];
            gCarPara.YAccelBase = MatrixAD[DIP_Y_AXIS_CH];
            gCarPara.ZAccelBase = MatrixAD[DIP_Z_AXIS_CH];
            
            memcpy(&canframe.data[1],(uint8*)&gCarPara.XAccelBase,2);
            memcpy(&canframe.data[3],(uint8*)&gCarPara.YAccelBase,2);
            memcpy(&canframe.data[5],(uint8*)&gCarPara.ZAccelBase,2);
            gCarParaEepromUpdateFlag = 1;
            sendflag=1;
            break;
      case 0x88:             
            //ReadCarPara(); 
            memcpy(&canframe.data[1],(uint8*)&gCarPara.XAccelBase,2);
            memcpy(&canframe.data[3],(uint8*)&gCarPara.YAccelBase,2);
            memcpy(&canframe.data[5],(uint8*)&gCarPara.ZAccelBase,2);
            sendflag=1;
            break;
      case 0x09:
            canframe.data[1] = CarParaFromPC.data[1];
            //标定油门踏板最高点
            if(CarParaFromPC.data[1]==1)
            {
               if(voiceflag==0) 
               {                
                  AlarmVoice(ACCEL_PEDAL_TOP_VOICE);
                  voiceflag = 1;
               }
               gCarPara.minAccelPedal=MatrixAD[ACCEL_PEDAL_CH];
               memcpy(&canframe.data[2],(uint8*)&gCarPara.minAccelPedal,2);
            }
            //标定油门踏板最低点
            else if(CarParaFromPC.data[1]==2)
            {
               if(voiceflag==0) 
               { 
                 AlarmVoice(ACCEL_PEDAL_BOTTOM_VOICE);
                 voiceflag = 1;
               }
               gCarPara.maxAccelPedal=MatrixAD[ACCEL_PEDAL_CH];
               memcpy(&canframe.data[4],(uint8*)&gCarPara.maxAccelPedal,2);
            }
            //标定油门踏板刹车位置
           /* 0603，暂时注销
            else if(CarParaFromPC.data[1]==3)
            {
               if(voiceflag==0) 
               { 
                 AlarmVoice(BRAKE_START_VOICE);
                 voiceflag = 1;
               }
               gCarPara.midAccelPedal=MatrixAD[ACCEL_PEDAL_CH];
               memcpy(&canframe.data[6],(uint8*)&gCarPara.midAccelPedal,2);
            }
             */
                        
            sendflag=1;
            break;
      case 0x89:   
            voiceflag = 0;   
            //标定油门踏板最高点
            if(CarParaFromPC.data[1]==1)
            {
               gCarPara.minAccelPedal=MatrixAD[ACCEL_PEDAL_CH];
               AlarmVoice(CAL_SUCC_VOICE);               
               memcpy(&canframe.data[2],(uint8*)&gCarPara.minAccelPedal,2);
               gCarParaEepromUpdateFlag = 1;
            }
            //标定油门踏板最低点
            else if(CarParaFromPC.data[1]==2)
            {
               gCarPara.maxAccelPedal=MatrixAD[ACCEL_PEDAL_CH];
               AlarmVoice(CAL_SUCC_VOICE);               
               memcpy(&canframe.data[4],(uint8*)&gCarPara.maxAccelPedal,2);
               gCarParaEepromUpdateFlag = 1;
            }
            //标定油门踏板刹车位置
            /*
            else if(CarParaFromPC.data[1]==3)
            {
               gCarPara.midAccelPedal=MatrixAD[ACCEL_PEDAL_CH];
               AlarmVoice(CAL_SUCC_VOICE);               
               memcpy(&canframe.data[6],(uint8*)&gCarPara.midAccelPedal,2);
               gCarParaEepromUpdateFlag = 1;
            } 
            */ 
            else if(CarParaFromPC.data[1]==0) 
            {
               //ReadCarPara(); 
               gCarPara.minAccelPedal=MatrixAD[ACCEL_PEDAL_CH];
               gCarPara.maxAccelPedal=MatrixAD[ACCEL_PEDAL_CH];
              // gCarPara.midAccelPedal=MatrixAD[ACCEL_PEDAL_CH];
               memcpy(&canframe.data[2],(uint8*)&gCarPara.minAccelPedal,2);
               memcpy(&canframe.data[4],(uint8*)&gCarPara.maxAccelPedal,2);
              // memcpy(&canframe.data[6],(uint8*)&gCarPara.midAccelPedal,2);
            }
            
            sendflag=1;
            break;       
      //////////VCU下线检测参数配置过程///////////////////////////////////////////////////////////////////
      /////软件版本，硬件版本以及生产日期配置
      case 0x06:
            if(MemEqual(&CarParaFromPC.data[1],allzero,6)==FALSE) 
            {
              memcpy((uint8*)&gCarPara.softwareVersion,&CarParaFromPC.data[1],2);
              
              memcpy((uint8*)&gCarPara.hardwareVersion,&CarParaFromPC.data[3],2);
              
              memcpy((uint8*)&gCarPara.softwareUpdateDate,&CarParaFromPC.data[5],2);
              //gCarPara.softwareUpdateDate = SOFTWARE_UPDATE_DATE;
              gCarParaEepromUpdateFlag = 1;
            }
            sendflag=1;
            break;
      case 0x86:            
            memcpy(&canframe.data[1],(uint8*)&gCarPara.softwareVersion,2);
            memcpy(&canframe.data[3],(uint8*)&gCarPara.hardwareVersion,2);
           // gCarPara.softwareUpdateDate = SOFTWARE_UPDATE_DATE;
            memcpy(&canframe.data[5],(uint8*)&gCarPara.softwareUpdateDate,2);
            sendflag=1;
            break;
      case 0x07:
                        
            baseflag=0;
            canframe.data[1] = 0;
            if(ReadTestResult(BATTERY_BASE_FLAG)==0) 
            {
              
              if(MemEqual(&CarParaFromPC.data[1],allzero,2)==FALSE) 
              {
                memcpy((uint8*)&davalue,&CarParaFromPC.data[1],2);
                
                gCarInfor.batVoltStand = (FP32)davalue/1000.0;
                if(gCarInfor.batVoltStand>=10.0)//超过12v才能进行标定 
                {                   
                   gCarPara.batVoltReviseCoef=(gCarInfor.batVoltStand-gCarInfor.batzeroVoltVolt)/(FP32)MatrixAD[BATTERY_VOLT_CH]; 
                   gCarParaEepromUpdateFlag = 1;                  
                } 
                else 
                {
                  baseflag=1;
                }
              }
              
              if(MemEqual(&CarParaFromPC.data[3],allzero,2)==FALSE) 
              {
                memcpy((uint8*)&davalue,&CarParaFromPC.data[3],2);
                gCarInfor.batCurrStand = (FP32)davalue/1000.0;
                
                if(gCarInfor.batCurrStand>=10.0)//超过9A才能进行标定 
                {
                  volttemp =  (FP32)(MatrixAD[BATTERY_CURR_CH])/(FP32)1024;
                  volttemp *= gCarPara.mcuRefVoltStand;
                  volttemp -= gCarPara.batzeroCurrVolt;                  
                  volttemp = volttemp/gCarInfor.batCurrStand;
                  
                  gCarPara.batCurrReviseCoef= volttemp;
                  gCarParaEepromUpdateFlag = 1;
                }
                else 
                {
                  baseflag=1;
                }
              }
              
              if(baseflag==0) 
              {
                canframe.data[1] = 1;
                ClearTestResult(BATTERY_CAL_FLAG);
                AlarmVoice(CAL_SUCC_VOICE);
              } 
              else
              {
                canframe.data[1] = 0;
                SetTestResult(BATTERY_CAL_FLAG);
                AlarmVoice(CAL_FAILED_VOICE);
              }
                            
            }
            else
            {
              AlarmVoice(CAL_ZERO_FAILED_VOICE);
            }
            sendflag=1;
            break;
      case 0x87:
            ReadCarPara(); 
            
            davalue =(uint16)(gCarPara.batVoltReviseCoef*100000); 
            memcpy(&canframe.data[1],(uint8*)&davalue,2);
            
            davalue =(uint16)(gCarPara.batCurrReviseCoef*100000); 
            memcpy(&canframe.data[3],(uint8*)&davalue,2);
            
            davalue =(uint16)(gCarPara.mcuRefVoltStand*1000);
            memcpy(&canframe.data[5],(uint8*)&davalue,2);
            sendflag=1;            
            break;
      case 0x10:
            //标定模式下，关闭GPRS模块
             
            GPRSReset = 1;
            GPRS_init_flag=GPRS_CONNECTED;      
            canframe.data[0]=CarParaFromPC.data[0];  
            //获取MCU参考电压       
            memcpy((uint8*)&davalue,&CarParaFromPC.data[1],2);
            
            if(davalue>4900 && davalue<5300) 
            {
               gCarPara.mcuRefVoltStand = (FP32)davalue/1000.0; 
               if(ReadTestResult(MCU_REF_VOLT_FLAG)==1) 
               {                
                  ClearTestResult(MCU_REF_VOLT_FLAG);
               }
               
               if(ReadTestResult(MCU_REF_GND_FLAG)==1) 
               {                
                  ClearTestResult(MCU_REF_GND_FLAG);
               }
            } 
            else
            {
               if(ReadTestResult(MCU_REF_VOLT_FLAG)==0) 
               {
                  SetTestResult(MCU_REF_VOLT_FLAG);
               }
               
               if(ReadTestResult(MCU_REF_GND_FLAG)==0) 
               {
                  SetTestResult(MCU_REF_GND_FLAG);
               }
               AlarmVoice(CAL_ZERO_FAILED_VOICE);
            }
            
          
            volttemp = (FP32)MatrixAD[BATTERY_VOLT_CH]/1024.0;
            volttemp = volttemp*gCarPara.mcuRefVoltStand;
            davalue = (uint16)(volttemp*1000);
            memcpy(&canframe.data[1],(uint16*)&davalue,2);
            
            volttemp = (FP32)MatrixAD[BATTERY_CURR_CH]/1024.0;
            volttemp = volttemp*gCarPara.mcuRefVoltStand;
            davalue = (uint16)(volttemp*1000);
            memcpy(&canframe.data[3],(uint16*)&davalue,2); 
            sendflag=1;                                                      
            break;  
        case 0x90:
            canframe.data[0]=CarParaFromPC.data[0];         
            baseflag=1;
            volttemp = (FP32)MatrixAD[BATTERY_VOLT_CH]/1024;
            volttemp = volttemp*(gCarPara.mcuRefVoltStand);
            
            if(volttemp>1.0) 
            {
               //电压调零失败
               SetTestResult(BATTERY_BASE_FLAG);
               baseflag = 0;
            } 
            else
            {
              gCarInfor.batzeroVoltVolt = volttemp;
            }
            
            volttemp = (FP32)MatrixAD[BATTERY_CURR_CH]/1024;
            volttemp = volttemp*(gCarPara.mcuRefVoltStand);              
            if(volttemp>2.590 || volttemp<2.0)   //2.55 或2.45
            {
               //电流调零失败
               SetTestResult(BATTERY_BASE_FLAG);
               baseflag = 0;
            } 
            else
            { 
                gCarPara.batzeroCurrVolt = volttemp;
            }
            
            //两个调零成功
            if(baseflag==1) 
            {
               ClearTestResult(BATTERY_BASE_FLAG);
               gCarParaEepromUpdateFlag = 1;
               canframe.data[1]=1;
               AlarmVoice(CAL_ZERO_SUCC_VOICE);
            } 
            else
            {
               canframe.data[1]=0;
               AlarmVoice(CAL_ZERO_FAILED_VOICE);
            }
           
            sendflag=1; 
            break;
                 
      //////////VCU下线检测管脚测试过程///////////////////////////////////////////////////////////////////
      case 0x0A:
           
            canframe.data[0]=CarParaFromPC.data[0];
            canframe.data[1]=CarParaFromPC.data[1];
            canframe.data[2]=CarParaFromPC.data[2];
            canframe.data[3]=CarParaFromPC.data[3];
            
            //表示VCU输出
            if(CarParaFromPC.data[1]==1)
            {
              
              switch(CarParaFromPC.data[2]) 
              {
                case 15:
                  //输出高电平
                  if(CarParaFromPC.data[3]==1) 
                  {
                    NightlampSwitch = 1;
                    
                    
                  }
                  //不输出
                  else if(CarParaFromPC.data[3]==0) 
                  {
                    NightlampSwitch = 0;
                  }
                  break;
                case 16:
                  //输出高电平
                  if(CarParaFromPC.data[3]==1) 
                  {
                    LeftlampSwitch = 1;
                  }
                  //不输出
                  else if(CarParaFromPC.data[3]==0) 
                  {
                    LeftlampSwitch = 0;
                    
                  }
                  break;
                case 17:
                  //输出高电平
                  if(CarParaFromPC.data[3]==1) 
                  {
                    RightlampSwitch = 1;
                  }
                  //不输出
                  else if(CarParaFromPC.data[3]==0) 
                  {
                    RightlampSwitch = 0;
                  }
                  break;
                case 18:
                  //输出高电平
                  if(CarParaFromPC.data[3]==1) 
                  {
                    ReverselampSwitch = 1;
                  }
                  //不输出
                  else if(CarParaFromPC.data[3]==0) 
                  {
                    ReverselampSwitch = 0;
                  }
                  break;
                case 19:
                
                  //输出高电平
                  if(CarParaFromPC.data[3]==1) 
                  {
                    HornSwitch = 1;
                    
                  }
                  //不输出
                  else if(CarParaFromPC.data[3]==0) 
                  {
                    HornSwitch = 0 ;
                  }
                  break;
                case 20: 
                  //输出高电平
                  if(CarParaFromPC.data[3]==1) 
                  {
                    ;//MOS_CONTROL = 1;
                  }
                  //不输出
                  else if(CarParaFromPC.data[3]==0) 
                  {
                    ;//MOS_CONTROL = 0;
                  }
                  break;
                case 21: 
                  //输出高电平
                  if(CarParaFromPC.data[3]==1) 
                  {
                    Slotmachine = 1;
                  }
                  //不输出
                  else if(CarParaFromPC.data[3]==0) 
                  {
                    Slotmachine = 0;
                   
                  }
                  
                  break;
                case 22: 
                  //语音测试
                  if(CarParaFromPC.data[3]==1) 
                  {
                     AlarmVoice(TEST_SUCC_VOICE); 
                     ClearTestResult(VOICE_OUTPUT2_FLAG);
                     ClearTestResult(VOICE_OUTPUT1_FLAG);
                  }
                  //测试失败
                  else if(CarParaFromPC.data[3]==0) 
                  {
                     AlarmVoice(TEST_FAILED_VOICE);
                     SetTestResult(VOICE_OUTPUT2_FLAG);
                     SetTestResult(VOICE_OUTPUT1_FLAG);                   
                  }
                  
                  break; 
                  
               case 26: 
                  //输出高电平
                  if(CarParaFromPC.data[3]==1) 
                  {
                    //PT0AD_PT0AD4 = 1;
                    DirectLEDRED = 1;
 
                  }
                  //输出低电平
                  else if(CarParaFromPC.data[3]==0) 
                  {
                    //PT0AD_PT0AD4 = 0;
                    DirectLEDRED = 0;
                  }
                  break;
               case 27: 
                  //输出高电平
                  if(CarParaFromPC.data[3]==1) 
                  {
                    //PT0AD_PT0AD5 = 1;
                    DirectLEDGREEN = 1;
                    
                  }
                  //输出低电平
                  else if(CarParaFromPC.data[3]==0) 
                  {
                    //PT0AD_PT0AD5 = 0;
                    DirectLEDGREEN = 0;
                  }
                  break;
               case 43: 
                  //输出低电平
                  if(CarParaFromPC.data[3]==1) 
                  {
                    HeadlampRelay = 0;
                    CarParaFromPC.data[3]=0;
                  }
                  //不输出
                  else if(CarParaFromPC.data[3]==0) 
                  {
                    HeadlampRelay = 1;
                    CarParaFromPC.data[3]=1;
                  }
                  break;
               case 44: 
                  //输出低电平
                  if(CarParaFromPC.data[3]==1) 
                  {
                    DaocheRelay = 0;
                    CarParaFromPC.data[3]=0;
                  }
                  //不输出
                  else if(CarParaFromPC.data[3]==0) 
                  {
                    DaocheRelay = 1;
                    CarParaFromPC.data[3]=1;
                  }
                  break;
               case 45: 
                  //输出低电平
                  if(CarParaFromPC.data[3]==1) 
                  {
                    YuanguangRelay = 0;
                    CarParaFromPC.data[3]=0;
                  }
                  //不输出
                  else if(CarParaFromPC.data[3]==0) 
                  {
                    YuanguangRelay = 1;
                    CarParaFromPC.data[3]=1;
                  }
                  break;
               case 46: 
                  //输出低电平
                  if(CarParaFromPC.data[3]==1) 
                  {
                    BrakeRelay = 0;
                    CarParaFromPC.data[3]=0;
                  }
                  //不输出
                  else if(CarParaFromPC.data[3]==0) 
                  {
                    BrakeRelay = 1;
                    CarParaFromPC.data[3]=1;
                  }
                  break;
               case 47: 
                  //输出低电平
                  if(CarParaFromPC.data[3]==1) 
                  {
                    GPS_RESET = 0;
                    CarParaFromPC.data[3]=0;
                  }
                  //不输出
                  else if(CarParaFromPC.data[3]==0) 
                  {
                    GPS_RESET = 1;
                    CarParaFromPC.data[3]=1;
                  }
                  break;
              }
            }
            //表示VCU输入
            else if(CarParaFromPC.data[1]==0)
            {
              switch(CarParaFromPC.data[2]) 
              { 
                case 10:
                  if(gMotorInfor.tripPulseCounter>6) 
                  {
                     canframe.data[4]=1;
                     gMotorInfor.tripPulseCounter= 0;
                  } 
                  else
                  {
                     canframe.data[4]=0; 
                  }
                   break;
                case 14:
                  //检测到投币信号
                  if(coin_plus_trig ==true) 
                  {
                     coin_plus_trig = false;
                     canframe.data[4]=1;
                  } 
                  else
                  {
                     canframe.data[4]=0;
                  }
                  break;
                case 48:                  
                  if(Read74HC151(3)==1 && CarParaFromPC.data[4]==1)
                  {
                    canframe.data[4]=1;
                  } 
                  else if(Read74HC151(3)==0 && CarParaFromPC.data[4]==0) 
                  {
                    canframe.data[4]=1;
                  } 
                  else
                  {
                    canframe.data[4]=0;
                  }
                  break;
                case 49:
                  if(Read74HC151(2)==1 && CarParaFromPC.data[4]==1)
                  {
                    canframe.data[4]=1;
                  } 
                  else if(Read74HC151(2)==0 && CarParaFromPC.data[4]==0) 
                  {
                    canframe.data[4]=1;
                  } 
                  else
                  {
                    canframe.data[4]=0;
                  }
                  break;
                case 50:
                  if(Read74HC151(1)==1 && CarParaFromPC.data[4]==1)
                  {
                    canframe.data[4]=1;
                  } 
                  else if(Read74HC151(1)==0 && CarParaFromPC.data[4]==0) 
                  {
                    canframe.data[4]=1;
                  } 
                  else
                  {
                    canframe.data[4]=0;
                  }
                  break;
                case 51:
                  if(Read74HC151(0)==1 && CarParaFromPC.data[4]==1)
                  {
                    canframe.data[4]=1;
                  } 
                  else if(Read74HC151(0)==0 && CarParaFromPC.data[4]==0) 
                  {
                    canframe.data[4]=1;
                  } 
                  else
                  {
                    canframe.data[4]=0;
                  }               
                  break;
                case 52:
                  if(Read74HC151(7)==1 && CarParaFromPC.data[4]==1)
                  {
                    canframe.data[4]=1;
                  } 
                  else if(Read74HC151(7)==0 && CarParaFromPC.data[4]==0) 
                  {
                    canframe.data[4]=1;
                  } 
                  else
                  {
                    canframe.data[4]=0;
                  }
                  break;
                case 53:
                  if(Read74HC151(6)==1 && CarParaFromPC.data[4]==1)
                  {
                    canframe.data[4]=1;
                  } 
                  else if(Read74HC151(6)==0 && CarParaFromPC.data[4]==0) 
                  {
                    canframe.data[4]=1;
                  } 
                  else
                  {
                    canframe.data[4]=0;
                  }
                  break;
                case 54:
                  if(Read74HC151(5)==1 && CarParaFromPC.data[4]==1)
                  {
                    canframe.data[4]=1;
                  } 
                  else if(Read74HC151(5)==0 && CarParaFromPC.data[4]==0) 
                  {
                    canframe.data[4]=1;
                  } 
                  else
                  {
                    canframe.data[4]=0;
                  }
                  break;
                case 55:
                  if(Read74HC151(4)==1 && CarParaFromPC.data[4]==1)
                  {
                    canframe.data[4]=1;
                  } 
                  else if(Read74HC151(4)==0 && CarParaFromPC.data[4]==0) 
                  {
                    canframe.data[4]=1;
                  } 
                  else
                  {
                    canframe.data[4]=0;
                  }               
                  break;
              }   
            }
            sendflag=1;
            break;
      case 0x8A:
              
            //测试失败
            if(CarParaFromPC.data[2]==0) 
            {
              SetTestResult(CarParaFromPC.data[1]);              
            }
            //测试成功
            else
            {
               ClearTestResult(CarParaFromPC.data[1]);
            }
            sendflag=1;
            break;
      case 0x0B:
            canframe.data[0]=CarParaFromPC.data[0];                        
           
            //表示VCU DA 输出
            if(CarParaFromPC.data[1]==1)
            {
              canframe.data[1]=CarParaFromPC.data[1];
              davalue = CarParaFromPC.data[3];
              davalue = davalue<<8;
              davalue += CarParaFromPC.data[4];
              
              switch(CarParaFromPC.data[2]) 
              {
                case 28:   
                  canframe.data[2]=CarParaFromPC.data[2];
                  if(davalue==350) //3.50v 对应PWM=70
                  {                    
                    
                    epspwm_regesiterset(100);
                   
                    canframe.data[3]=CarParaFromPC.data[3];
                    canframe.data[4]=CarParaFromPC.data[4];
                  }
                  
                  if(davalue==175) //1.75v 对应PWM=280 
                  {
                     
                    epspwm_regesiterset(300);
                    
                    canframe.data[3]=CarParaFromPC.data[3];
                    canframe.data[4]=CarParaFromPC.data[4];
                  }
                  
                  break;
                case 38:
                  canframe.data[2]=CarParaFromPC.data[2];
                  if(davalue==350) //3.50v 对应PWM=70
                  {  
                                 
                    pwm_regesiterset(100);
                    
                    canframe.data[3]=CarParaFromPC.data[3];
                    canframe.data[4]=CarParaFromPC.data[4];
                  }
                  
                  if(davalue==175) //1.75v 对应PWM=280 
                  {
                        
                    pwm_regesiterset(300);
                   
                    canframe.data[3]=CarParaFromPC.data[3];
                    canframe.data[4]=CarParaFromPC.data[4];
                  }
                  break;
              }
            }
            //表示VCU AD 输入 和转速输入
            else if(CarParaFromPC.data[1]==0)
            {
              canframe.data[1]=CarParaFromPC.data[1];
              davalue = CarParaFromPC.data[5];
              davalue = davalue<<8;
              davalue += CarParaFromPC.data[6];
              
              davalue = (uint16)(1024*((FP32)davalue/500.0));
              switch(CarParaFromPC.data[2]) 
              {
                 canframe.data[2]=CarParaFromPC.data[2];
                //转速输入
                case 10: 
                  if(gMotorInfor.tripPulseCounter>=davalue&&gMotorInfor.tripPulseCounter<=(davalue+1)) 
                  {
                    davalue = gMotorInfor.tripPulseCounter;
                    memcpy(&canframe.data[5],(uint8*)&davalue,2);
                  }                   
                  break;
                //以下为AD
                case 11:
                  if(MatrixAD[ACCEL_PEDAL_CH]>=davalue-50 && MatrixAD[ACCEL_PEDAL_CH]<=davalue+50) 
                  {
                    davalue = MatrixAD[ACCEL_PEDAL_CH];
                    davalue = (uint16)(((FP32)davalue/1024)*500);
                    memcpy(&canframe.data[5],(uint8*)&davalue,2);
                  }
                  break;
                case 12:
                  if(MatrixAD[CORNER_SENSOR1_CH]>=davalue-50 && MatrixAD[CORNER_SENSOR1_CH]<=davalue+50) 
                  {
                    davalue = MatrixAD[CORNER_SENSOR1_CH];
                    davalue = (uint16)(((FP32)davalue/1024)*500);
                    memcpy(&canframe.data[5],(uint8*)&davalue,2);
                  }
                  break;
                case 13:
                  if(MatrixAD[CORNER_SENSOR2_CH]>=davalue-50 && MatrixAD[CORNER_SENSOR2_CH]<=davalue+50) 
                  {
                    davalue = MatrixAD[CORNER_SENSOR2_CH];
                    davalue = (uint16)(((FP32)davalue/1024)*500);
                    memcpy(&canframe.data[5],(uint8*)&davalue,2);
                  }
                  break;
                case 56:
                  if(MatrixAD[BATTERY_TEMP_CH]>=davalue-50 && MatrixAD[BATTERY_TEMP_CH]<=davalue+50) 
                  {
                    davalue = MatrixAD[BATTERY_TEMP_CH];
                    davalue = (uint16)(((FP32)davalue/1024)*500);
                    memcpy(&canframe.data[5],(uint8*)&davalue,2);
                  }                  
                  break;                  
              }
            }
            sendflag=1;
            break;
      case 0x8B:
           
            //测试失败
            if(CarParaFromPC.data[2]==0) 
            {
              SetTestResult(CarParaFromPC.data[1]);            
            }
            //测试成功
            else
            {
               ClearTestResult(CarParaFromPC.data[1]);
            }            
            gMotorInfor.tripPulseCounter = 0;
            sendflag=1;
            break;
      case 0x0C:
      
            canframe.data[0]=CarParaFromPC.data[0];                                   
            //表示VCU 串口 输出
            if(CarParaFromPC.data[1]==1)
            {
              canframe.data[1] = 1;
              canframe.data[2] = 0x3C;             
              SendByte0(canframe.data[2]);              
              
            }            
            //表示VCU 串口 输入
            else if(CarParaFromPC.data[1]==0)
            {
              canframe.data[1] = 0;
              if(usart0word=='A') 
              {
                  canframe.data[2]=1;
              } 
              else
              {
                  canframe.data[2]=0;
              }
            }
            sendflag=1;
            break;
      case 0x8C:
            //表示VCU 串口 测试失败
            if(CarParaFromPC.data[2]==0)
            {
              SetTestResult(CarParaFromPC.data[1]);            
            } 
            //测试成功
            else
            {
              ClearTestResult(CarParaFromPC.data[1]);
            }
            sendflag=1;
            break;
      case 0x0D:
            
            canframe.data[0]=CarParaFromPC.data[0]; 
            canframe.data[1]=CarParaFromPC.data[1];                                   
            //表示VCU 需要测试外部存储器
            if(CarParaFromPC.data[1]==0)
            {                
                if(0==ReadTestResult(EXEEPROM_TEST_FLAG)) 
                {                  
                  canframe.data[2]=1;
                } 
                else
                {
                  canframe.data[2]=0;
                }                            
            }    
            //表示VCU 需要测试蓝牙
            if(CarParaFromPC.data[1]==1)
            {                
                if(0==ReadTestResult(BLUETOOTH_TEST_FLAG)) 
                {                  
                  canframe.data[2]=1;
                } 
                else
                {
                  canframe.data[2]=0;
                }                            
            }            
            //表示VCU 需要测试GPRS
            else if(CarParaFromPC.data[1]==2)
            {
                if(ReadTestResult(GPRS_TEST_FLAG)==0)
                {                
                  canframe.data[2]=1;
                  canframe.data[3]=GPRS_init_flag;//gprs模块，该模块表示当前gprs状态 0-重启 1-上电 2-表示成功 
                } 
                else 
                {
                  SetTestResult(GPRS_TEST_FLAG);
                  canframe.data[2]=0;
                  canframe.data[3]=GPRS_init_flag;//gprs模块，该模块表示当前gprs状态 0-重启 1-上电 2-表示成功                  
                }
            }
            //表示VCU 需要测试GPS
            else if(CarParaFromPC.data[1]==3)
            {                                              
                if(ReadTestResult(GPS_TEST_FLAG)==0) 
                {                  
                  canframe.data[2]=1;
                  canframe.data[3]=GPS_init_flag;  //0-等待 1接收 2处理   
                } 
                else
                { 
                  SetTestResult(GPS_TEST_FLAG);                 
                  canframe.data[2]=0; 
                  canframe.data[3]=GPS_init_flag;  //0-等待 1接收 2处理               
                }
            } 
            else if(CarParaFromPC.data[1]==4)  //重启蓝牙
            {                
                SetTestResult(BLUETOOTH_TEST_FLAG);
                BT_Reset();
                AlarmVoice(BT_TEST_VOICE);
                //delayms(1000);                
                AlarmVoice(RESET_VOICE);
            }
            else if(CarParaFromPC.data[1]==5)  //重启GPRS GPS
            {
                GPRS_init_flag = GPRS_RESTART;
                SetTestResult(GPRS_TEST_FLAG);
                
                AlarmVoice(GPRS_TEST_VOICE);
                          
                AlarmVoice(RESET_VOICE);
                
                GPS_init_flag = GPS_RESTART;
                
                SetTestResult(GPS_TEST_FLAG);
                AlarmVoice(GPS_TEST_VOICE);                                
                AlarmVoice(RESET_VOICE);
                Restart_GPS();
            }
            else if(CarParaFromPC.data[1]==6)  //重启GPS
            {
              ;
            }
            
            sendflag=1;
            break;
      case 0x8D:            
            sendflag=1;
            break;
      case 0x0E:
      
            canframe.data[0]=CarParaFromPC.data[0];                                   
            
            memcpy(&canframe.data[1],(uint8*)&MatrixAD[DIP_X_AXIS_CH],2);
            memcpy(&canframe.data[3],(uint8*)&MatrixAD[DIP_Y_AXIS_CH],2);
            memcpy(&canframe.data[5],(uint8*)&MatrixAD[DIP_Z_AXIS_CH],2);
            sendflag=1;
            break;
      case 0x8E:
            canframe.data[0]=CarParaFromPC.data[0];    
            //测试成功
            if(CarParaFromPC.data[1]==1) 
            {
              ClearTestResult(DIPANGLE_TEST_FLAG);   
              AlarmVoice(ACCEL_SENSOE_VOICE);
              //delayms(2000);                                
              AlarmVoice(TEST_SUCC_VOICE);           
            }
            //测试失败
            else
            {
              SetTestResult(DIPANGLE_TEST_FLAG);
              AlarmVoice(ACCEL_SENSOE_VOICE);
              //delayms(2000);  
              AlarmVoice(TEST_FAILED_VOICE);     
            }
            sendflag=1;
            break;
       case 0x0F:
                  
            canframe.data[0]=CarParaFromPC.data[0];                                   
            if(CarParaFromPC.data[1]==0) 
            {              
              memcpy(&canframe.data[1],(uint8*)&gCarPara.testResult1,4);
            }
            
            if(CarParaFromPC.data[1]==1) 
            {              
              memcpy(&canframe.data[1],(uint8*)&gCarPara.testResult2,4);
            }
            
            ClearNCTestResult();
            if(gCarPara.testResult1==0 && gCarPara.testResult2==0 ) 
            {
               AlarmVoice(ALL_TEST_SUCC_VOICE);
            } 
            else
            {
               AlarmVoice(CAR_CHECK_ABNORM_VOICE);
            }
            sendflag=1;
            break;
      case 0x11:
                  
            canframe.data[0]=CarParaFromPC.data[0];                                   
            
            memcpy(&canframe.data[1],(uint8*)&gCarInfor.batVolt,2);
            memcpy(&canframe.data[3],(uint8*)&gCarInfor.batCurr,2);
            memcpy(&canframe.data[5],(uint8*)&gCarInfor.batTemp,1);
            
            sendflag=1;
            break;
      case 0x12:
                  
            canframe.data[0]=CarParaFromPC.data[0];
           
            memcpy((uint8*)&davalue,&canframe.data[1],2); 
                                               
            gCarPara.batVoltReviseCoef = (FP32)davalue/100000.0; 
            
            memcpy((uint8*)&davalue,&canframe.data[3],2); 
            
            gCarPara.batCurrReviseCoef = (FP32)davalue/100000.0; 
           
          
            memcpy((uint8*)&davalue,&canframe.data[5],2);  
            gCarPara.mcuRefVoltStand = (FP32)davalue/1000.0; 
            
            gCarParaEepromUpdateFlag = 1;
            sendflag=1;
            break;
      case 0x13:
                  
            canframe.data[0]=CarParaFromPC.data[0];                                   
            
            gCarPara.testResult1 = 0;
            gCarPara.testResult2 = 0;
            
            gCarParaEepromUpdateFlag = 1;
            sendflag=1;
            break;
        
    }
    if(sendflag!=0)
    {
      Can_Send(canframe);
    }
    FEEDWATCHDOG = ~FEEDWATCHDOG;
}
 #endif

void CommModuleTest() 
{
    uint8 j,i;
    uint8 testdata0[8],testdata1[8];
            
    //主动测试蓝牙
    #if DEBUG_MODE==0
    if(ReadTestResult(BLUETOOTH_TEST_FLAG)) 
    {
       if(BT_AT_SET_NAME("ROBOY001 ")!=0) 
       {
          AlarmVoice(BT_TEST_VOICE);      
          AlarmVoice(TEST_SUCC_VOICE); 
          ClearTestResult(BLUETOOTH_TEST_FLAG);
       } 
       else
       {
          AlarmVoice(BT_TEST_VOICE);
          AlarmVoice(TEST_FAILED_VOICE);  
       }
    }
        
    //主动测试外部eeprom
    if(gCarPara.hardwareVersion>=0x301) 
    {      
      if(ReadTestResult(EXEEPROM_TEST_FLAG)) 
      {
        for(i=0;i<8;i++)
        {
          testdata0[i]=i+1;
          testdata1[i]=0;
        }
        
        SMBSend(0x1200, testdata0, 8);
        SMBReceive(0x1200,testdata1,8);
        if(TRUE==MemEqual(testdata0,testdata1,8))
        {
          AlarmVoice(EXTERN_EEPROM_VOICE);      
          AlarmVoice(TEST_SUCC_VOICE); 
          ClearTestResult(EXEEPROM_TEST_FLAG);
        }
        else
        {
          AlarmVoice(EXTERN_EEPROM_VOICE);        
            AlarmVoice(TEST_FAILED_VOICE);  
         }
      }        
    }
    
        
    //测试gprs  
    if(ReadTestResult(GPRS_TEST_FLAG)!=0) 
    { 
      //测试成功                
      if( GPRS_init_flag>=GPRS_CONNECTTING)
      {
         ClearTestResult(GPRS_TEST_FLAG);
         AlarmVoice(GPRS_TEST_VOICE);         
         AlarmVoice(TEST_SUCC_VOICE);        
           
      } 
      else if(GPRS_init_flag == GPRS_RESTART)
      {                
          ;                                                                                               
            }
          }   
          
    //测试GPS
    if(ReadTestResult(GPS_TEST_FLAG)!=0 && GPS_init_flag == GPS_SUCC) 
    {      
              ClearTestResult(GPS_TEST_FLAG);
              AlarmVoice(GPS_TEST_VOICE);
      AlarmVoice(TEST_SUCC_VOICE);                                                                  ;     
    } 
    #endif         
}
/*
*********************************************************************************
* 函数名：SetTestResult
* 说  明：根据每个管脚的测试结果设置测试位，高8位为VCU内部测试使用
* 参  数int8 pinno 测试对应的管脚号
* 返回值: 
* 其  他：
*********************************************************************************
*/
void SetTestResult(uint8 pinno)
{
  uint32 i=1;
  uint32 temp;
  
  
  
  if(pinno>=0&&pinno<=31) 
  {
    temp=i<<pinno;
    gCarPara.testResult1 |= temp;
    gCarParaEepromUpdateFlag = 1;
  }
  
  if(pinno>=32&&pinno<=63) 
  {
    temp=i<<(pinno-32);
    gCarPara.testResult2 |= temp;
    gCarParaEepromUpdateFlag = 1;
  }
  
}

/*
*********************************************************************************
* 函数名：ClearTestResult
* 说  明：根据每个管脚的测试结果清除测试位，高8位为VCU内部测试使用
* 参  数int8 pinno 测试对应的管脚号
* 返回值: 
* 其  他：
*********************************************************************************
*/

void ClearTestResult(uint8 pinno)
{
  uint32 i=1;
  uint32 temp;
  
  
  if(pinno>=0&&pinno<=31) 
  {
    temp=i<<pinno;
    gCarPara.testResult1 &= ~temp;
    gCarParaEepromUpdateFlag = 1;
  }
  
  if(pinno>=32&&pinno<=63) 
  {
    temp=i<<(pinno-32);
    gCarPara.testResult2 &= ~temp;
    gCarParaEepromUpdateFlag = 1;
  }
  
}

/*
*********************************************************************************
* 函数名：ReadTestResult
* 说  明：读取某个测试位的测试结果
* 参  数int8 pinno 测试对应的管脚号
* 返回值: 1 表示改为测试未通过 0表示测试通过 
* 其  他：
*********************************************************************************
*/
uint8 ReadTestResult(uint8 pinno)
{
  uint32 i=1;
  uint32 temp;
    
  
  if(pinno>=0&&pinno<=31) 
  {
    temp=i<<pinno;
    if((gCarPara.testResult1 & temp)!=0) 
    { 
      return 1;
    } 
    else
    { 
      return 0;
    }
  }
  
  if(pinno>=32&&pinno<=63) 
  {
    temp=i<<(pinno-32);
    if((gCarPara.testResult2 & temp)!=0) 
    { 
      return 1;
    } 
    else
    { 
      return 0;
    }
  }
}

 //以下部分为保留管脚，不需要测试，因此设置为0
 void ClearNCTestResult() 
 {
    uint8 i;
    for(i=0;i<10;i++) 
    {              
      ClearTestResult(i);
    }
    
    for(i=29;i<38;i++) 
    {              
      ClearTestResult(i);
    }
    
    //备用mos管         
      ClearTestResult(20);
    //去除外部串口测试
    ClearTestResult(39);
    ClearTestResult(40);
    //去除can接口测试
    ClearTestResult(41);
    ClearTestResult(42);
    //去除备用继电器测试
    ClearTestResult(47);
    
    
    if(gCarPara.hardwareVersion==0x300) 
    {
      ClearTestResult(57);
    }
  
 }
            
#pragma CODE_SEG __NEAR_SEG NON_BANKED
void interrupt  Canrx_Process_INT(void)
{
    _FEED_COP();
    CAN_receive();
    FEEDWATCHDOG = ~FEEDWATCHDOG;
    CANMark();
    
    CANRFLG_RXF = 1;
}

void interrupt CAN_Tx_interrupt(void)       //发送中断函数
{
   
    CANTFLG = 1;

}
#pragma CODE_SEG DEFAULT









