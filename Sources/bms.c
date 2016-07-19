#include "includes.h"
uint16 TotalElectricity = 0x9A48;
uint16 CurrentElectricity = 0x9A48;
byte LowElectricityflag = 0;    //
byte BMSflag = 0;
extern byte BMS_End_flag;

byte BMS[20] = {0x7E, 0x32, 0x31 , 0x30 , 0x30 ,
                0x34, 0x36, 0x34 , 0x32 , 0x45 ,
                0x30, 0x30, 0x32 , 0x30 , 0x31 ,
                0x46, 0x44, 0x33 , 0x35 , 0x0D
               };
void GetBMSinfo(void)
{
    //BMS  ����
    if(BMSflag ==0)
    {
        SendBytes(BMS,20);
        SendBytes(BMS,20);
        BMSflag = 1;
    }
    else
    {
        if(BMS_End_flag)
        {
            CurrentElectricity  = (GSM_Receive_Buf[131]-48)*4096
                                  + (GSM_Receive_Buf[132]-48)*256
                                  + (GSM_Receive_Buf[133]-48)*16
                                  + (GSM_Receive_Buf[134]-48);
            if((CurrentElectricity <0x1F18)
                    && (CurrentElectricity > 0xF8C)
                    && (LowElectricityflag != 1)) //��ص��������ܵ���25%������20%
            {
                LowElectricityflag = 1;
                DirectLED1 = 1;
            }
            else  if(CurrentElectricity < 0xF8C)   //��ص��������ܵ���20%
            {
                LowElectricityflag = 2;
                gCarThreshold.errCode |= HIGH_TEMPATURE_EXCEPTION;  
                    
                gCarThreshold.operationMode = STOP_OPERATION;
                gCarThresholdEepromUpdateFlag = 1;            
            }
            else
            {
                LowElectricityflag = 0;
            }
            BMS_End_flag = 0;
            memset(GSM_Receive_Buf, 0, 150);
        }
        BMSflag = 0;
    }
}
