 /*
 * 硬件平台：9S12G128 智能车VCU
 * 名称：主程序
 * 描述：
 *
 */
#include "includes.h"

/*
*********************************************************************************************************
*                                           main()
* Description: 包括初始化即各种功能实现
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
    IVBR = 0x7F;                  //将中断向量起始地址改到0x7F00
    
    //车辆相关数据初始化
    CarDataInit();
    //设备初始化
    DEV_Init();                                                                              `
            
    FEEDWATCHDOG = ~FEEDWATCHDOG;    
        
    EnableInterrupts;
    //蓝牙复位
    #if DEBUG_MODE==0
    BT_Reset(); 
    #endif
    
    //设置VCU处于测试模式还是正常工作模式 
    SetVCUWorkMode();
    
    //打开内部看门狗
    _ENABLE_COP_X();
   
    //初始化语音，并播放欢迎语音
    WelcomeVoice();  
    
    //设置当前启动模式为正常app模式
    if(gCarPara.hardwareVersion>0x300) 
    {      
      WriteBootFlag(APP_MODE);
    }

    for(;;)       
    {              
                                                   
        FEEDWATCHDOG = ~FEEDWATCHDOG;
        _FEED_COP(); /* feeds the dog */
       
        //电池电压，电流，温度检测
        BatteryDataProc();
       
        
       //保存参数信息
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
        //设置蓝牙模块名称为车牌+空格
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
   
        //配置模式下直接返回
        if(VCUWorkMode == CONFIG_MODE) 
        {                    
          GPRSPower = 0;
          continue;
        } 

        
        
        //手机蓝牙开车处理过程    
        PhoneBTRentingProcess();
        
        //网络连接初始化                        
        GPRSConnecting();
        
        //投币开车处理过程
        CoinRentingProcess();
        
                
        if( VCUWorkMode==TEST_MODE) 
        {
           CommModuleTest();
           continue;
        }        
        //////////////////////////////0504////////////               
        
        //与后台服务器建立连接成功
        /***************************需要实时处理的消息*************************/
        if( GPRS_init_flag==GPRS_CONNECTED)
        {   
            //手机远程租车处理过程 
            PhoneGPRSRentingProcess();         
        } 
        
        if(freq_2s==1) 
        {
            //发送车辆信息
         // #if DEBUG_MODE==1                            
            SendCarDataByCan();
           // #endif
            freq_2s = 0;
        }
        
        /***********************频率为5s处理一次的程序***************************/
        if(freq_5s == 1)
        {                         
            //GPS围栏处理过程
            GPSFenceProcess();       
            freq_5s = 0;
        }        
         
        /***********************频率为15s处理一次的程序***************************/
        if(freq_15s==1) 
        {                             
             //异常处理过程
             ExceptionProcess();
             //获取手机信号强度
             GPRS_GET_RSSI();
             freq_15s = 0;
        }
        /***********************频率为60s处理一次的程序***************************/
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
