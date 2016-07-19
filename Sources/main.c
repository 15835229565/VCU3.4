 /*
 * Ӳ��ƽ̨��9S12G128 ���ܳ�VCU
 * ���ƣ�������
 * ������
 *
 */
#include "includes.h"

/*
*********************************************************************************************************
*                                           main()
* Description: ������ʼ�������ֹ���ʵ��
*
* Arguments  : none
*
* Note(s)    :
*********************************************************** **********************************************
*/
void main(void)
{
    uint16  i,j;
    uint8   secnum=0;
    uint8 btname[18];
    IVBR = 0x7F;                  //���ж�������ʼ��ַ�ĵ�0x7F00
    
    //����������ݳ�ʼ��
    CarDataInit();
    //�豸��ʼ��
    DEV_Init();                                                                              `
            
    FEEDWATCHDOG = ~FEEDWATCHDOG;    
        
    EnableInterrupts;
    //������λ
    #if DEBUG_MODE==0
    BT_Reset(); 
    #endif
    
    //����VCU���ڲ���ģʽ������������ģʽ 
    SetVCUWorkMode();
    
    //���ڲ����Ź�
    _ENABLE_COP_X();
   
    //��ʼ�������������Ż�ӭ����
    WelcomeVoice();  
    
    //���õ�ǰ����ģʽΪ����appģʽ
    if(gCarPara.hardwareVersion>0x300) 
    {      
      WriteBootFlag(APP_MODE);
    }

    for(;;)       
    {              
                                                   
        FEEDWATCHDOG = ~FEEDWATCHDOG;
        _FEED_COP(); /* feeds the dog */
       
        //��ص�ѹ���������¶ȼ��
        BatteryDataProc();
       
        
       //���������Ϣ
        if(gCarParaEepromUpdateFlag==1) 
        {
            FEEDWATCHDOG = ~FEEDWATCHDOG;
            WriteCarPara();
            AlarmVoice(SUCC_VOICE);
            gCarParaEepromUpdateFlag = 0;
        }
       
        if(gCarThresholdEepromUpdateFlag==1) 
        {
            FEEDWATCHDOG = ~FEEDWATCHDOG;
            WriteCarThreshold();
            gCarThresholdEepromUpdateFlag = 0;
        }
        
        #if DEBUG_MODE==0
        //��������ģ������Ϊ����+�ո�
        if(BTnameSetFlag==1) 
        {                       
            memcpy(btname,gCarPara.name,6);
            btname[6]=' ';
            btname[7]='\0';
            if(0!=BT_AT_SET_NAME(btname)) 
            {                
              BTnameSetFlag = 0;
            }                            
        }
        #endif
   
        //����ģʽ��ֱ�ӷ���
        if(VCUWorkMode == CONFIG_MODE) 
        {                    
          GPRSPower = 0;
          continue;
        } 

        
        
        //�ֻ����������������    
        PhoneBTRentingProcess();
        
        //�������ӳ�ʼ��                        
        GPRSConnecting();
        
        //Ͷ�ҿ����������
        CoinRentingProcess();
        
                
        if( VCUWorkMode==TEST_MODE) 
        {
           CommModuleTest();
           continue;
        }        
        //////////////////////////////0504////////////               
        
        //���̨�������������ӳɹ�
        /***************************��Ҫʵʱ�������Ϣ*************************/
        if( GPRS_init_flag==GPRS_CONNECTED)
        {   
            //�ֻ�Զ���⳵������� 
            PhoneGPRSRentingProcess();         
        } 
        
        if(freq_2s==1) 
        {
            //���ͳ�����Ϣ
         // #if DEBUG_MODE==1                            
            SendCarDataByCan();
           // #endif
            freq_2s = 0;
        }
        
        /***********************Ƶ��Ϊ5s����һ�εĳ���***************************/
        if(freq_5s == 1)
        {                         
            //GPSΧ���������
            GPSFenceProcess();       
            freq_5s = 0;
        }        
         
        /***********************Ƶ��Ϊ15s����һ�εĳ���***************************/
        if(freq_15s==1) 
        {                             
             //�쳣�������
             ExceptionProcess();
             //��ȡ�ֻ��ź�ǿ��
             GPRS_GET_RSSI();
             freq_15s = 0;
        }
        /***********************Ƶ��Ϊ60s����һ�εĳ���***************************/
        if(freq_60s == 1)
        {                
            #if DEBUG_MODE==0
            if(phoneVerifFlag==0) 
            {
               BT_Reset();
            }
            #endif
            freq_60s = 0;
        }
    }     
}
