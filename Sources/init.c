#include"includes.h"


void Service_WD(void)
{
    CPMUARMCOP = 0x55;
    CPMUARMCOP = 0xAA;    
}

void SetBusClock_24MHz(void)
{
    
    if(gCarPara.hardwareVersion==0x300) 
    {
      
    
      CPMUPROT = 0x26;//停止保护时钟配置寄存器
      
      CPMUCLKS_PLLSEL = 1;    
      CPMUOSC_OSCE = 0;  
       
   
   //     采用内部1MHz晶振，PLL到48MHz，bus clock为24MHz
     

      CPMUSYNR = 0x00 | 0x17;       
   
      CPMUPOSTDIV = 0x00;        

      while(!CPMUFLG_LOCK);         

      Service_WD();
      CPMULVCTL_LVIE = 0;           //关闭低压检测中断
      PIEP = 0;                     //关闭PORT P外部中断
      CPMUCLKS_PLLSEL = 1;          
      IRQCR_IRQEN=0;                //PT2作为普通管脚使用，所以关闭IRQ功能
     
      CPMUPROT_PROT= ~(0x26);
    } 
    else if(gCarPara.hardwareVersion>=0x301) 
    {
      CPMUCLKS_PLLSEL = 0;  
      CPMUOSC_OSCPINS_EN = 1;

      CPMUOSC_OSCE = 1; /* enable ext osc */
      /*
      Initialise the system clock from a 16 MHz Crystal,
      24 MHz Bus CLK (48 MHz VCO, 48 MHz PLL)
      */

      CPMUSYNR = 0x00 | 0x05;       /* VCOFRQ[7:6], SYNDIV[5:0] */

      CPMUREFDIV = 0x20 | 0x03;     /* REFFRQ[7:6], REFDIV[3:0] */

      CPMUPOSTDIV = 0x00;           /* POSTDIV = 0 FPLL = FVCO  */

      while(!CPMUFLG_LOCK);         /* wait for VCO to stabilize*/

      Service_WD();

      CPMUCLKS_PLLSEL = 1;          /* Switch clk to use PLL    */
      IRQCR_IRQEN=0;                //PT2作为普通管脚使用，所以关闭IRQ功能
   
    }
    
}
/*
*********************************************************************************************************
*                                           DEV_Init()
* Description: 各种初始化
*
* Arguments  : none
*
* Note(s)    :
*********************************************************************************************************
*/
void DEV_Init(void)
{

    //#if DEBUG_MODE==1
    SetBusClock_24MHz();
   // #endif
   
   
   
    Can_Init(CAN_BAUDRATE_500K);
    
    SPI2CR1_SPE = 0;
    SPI1CR1_SPE = 0;
    GPIO_init(NULL) ;
 
    
    WT588D_BUSY_INEN;                          //GPS串口波特率9600
    SCI_BR0(9600);                     //GPRS串口波特率115200
    SCI_BR1(9600); 
    
    SCI_BR2(115200);      //注意双模蓝牙采用的是115200波特率
                                           
    SCI_Init();                                                           

    OutputCompare_Init();                                        //定时器初始化
    
    ADC_Init(NULL);                                          //AD初始化
                                                             
    PWMGeneralInit(0,6,6,0xc0);                              //0分频   01级联
    PWMSinglePortInit(7,1,1,1,0);                             //通道7输出，SA作为时钟，起始高电平，左对齐
    PWMConcatenatePortSetting(7,500,500);            //输出PWM Freq=4kHz
    PWMSinglePortInit(5,1,1,1,0);                             //通道5输出，SA作为时钟，起始高电平，左对齐
    PWMConcatenatePortSetting(5,500,500);            //输出PWM Freq=4kHz
    Init_PT_ICapture();
     
    ///////////////////////
    RTI_Init();
   
    device.data = 0;
    

   
    ////各个继电器、开关初始化
    NightlampSwitch = 0;
    HeadlampRelay = 0;
    LeftlampSwitch = 0;
    RightlampSwitch = 0;
    DaocheRelay = 0;
    ReverselampSwitch = 0;
    YuanguangRelay = 0;
    HornSwitch = 0;
    BrakeRelay = ELECMAGHOLD;
    GPRSPower = 0;
    GPRSReset = 0;
    DirectLEDGREEN= 1; //投币或手机租车后指示灯亮
    DirectLEDRED= 0; //系统上电之后指示灯亮
    BT_RESET = 0;
    GPRS_init_flag = GPRS_RESTART;  
    GPS_init_flag = GPS_RESTART;          
}

void CarDataInit(void) 
{
    uint8 j;
    //EEPROM_INITIAL();  //???测试用
    flash_init();
    ReadCarPara();      //读取配置参数
    ReadCarThreshold(); //读取阈值等

    gCarInfor.state = CarReady;
    gCarInfor.userid = 0;
    gCarInfor.Event = 0;
    
  //初始化当前位置
    gCarInfor.lLatitude1 = 0;
    gCarInfor.lLatitude2 = 0;
    gCarInfor.lLongitude1 = 0;
    gCarInfor.lLongitude2 = 0;
    gCarInfor.maxSpeed = gCarThreshold.maxSpeed;
    gCarInfor.preMaxSpeed = gCarInfor.maxSpeed;
    gCarInfor.stopTimeout = gCarThreshold.stopTimeout;
    
    gCarThreshold.errCode = 0;
    gCarInfor.maxMileage = 10000;
    //获取种子
    gCarInfor.seed = 0x12768967; 
    gCarInfor.speedRate = 0;
    gCarInfor.MCUResetFlag=0;
    
    
    TripDataInit();
  
    //0603yzl
    //初始化电机控制器类型
    switch(gCarPara.motorPower) 
    {
        case 1: 
        {
          gCarInfor.controllerType=LiWei;
          break;
        }
        case 2: 
        {
          gCarInfor.controllerType=KeChuang;
          break;
        }
        case 3: 
        {
          gCarInfor.controllerType=Qian;
          break;
        }                                
        default : 
        {          
            gCarInfor.controllerType=KeChuang;
        }
    }
    MotorInit();     
}

//行程数据初始化
void TripDataInit() 
{
   gCarInfor.tripCoin = 0;
   gCarInfor.tripMileage = 0;
   gCarInfor.tripStopOverTimer = 0;
   gCarInfor.tripStopTime = 0;
   gCarInfor.tripTime = 0;
   gCarInfor.recovTripMileage = 0;
   gCarInfor.userAuthority = 0;   
   gCarInfor.RadarCollisionCounter = 0; 
   gCarInfor.availableMetre = 0;
   gCarInfor.availableTime = 0; 
}

void SetVCUWorkMode() 
{
  if(gCarPara.testResult1!=0|| gCarPara.testResult2!=0) 
  {                  
    //调试模式用
    #if DEBUG_MODE==1     
    VCUWorkMode = NORMAL_MODE;
    #endif
          
    #if DEBUG_MODE==0    
    VCUWorkMode = NORMAL_MODE; 
    #endif    
    
  } 
  else
  {
    VCUWorkMode = NORMAL_MODE;
  }        
}

   
