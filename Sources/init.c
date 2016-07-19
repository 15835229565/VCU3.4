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
      
    
      CPMUPROT = 0x26;//ֹͣ����ʱ�����üĴ���
      
      CPMUCLKS_PLLSEL = 1;    
      CPMUOSC_OSCE = 0;  
       
   
   //     �����ڲ�1MHz����PLL��48MHz��bus clockΪ24MHz
     

      CPMUSYNR = 0x00 | 0x17;       
   
      CPMUPOSTDIV = 0x00;        

      while(!CPMUFLG_LOCK);         

      Service_WD();
      CPMULVCTL_LVIE = 0;           //�رյ�ѹ����ж�
      PIEP = 0;                     //�ر�PORT P�ⲿ�ж�
      CPMUCLKS_PLLSEL = 1;          
      IRQCR_IRQEN=0;                //PT2��Ϊ��ͨ�ܽ�ʹ�ã����Թر�IRQ����
     
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
      IRQCR_IRQEN=0;                //PT2��Ϊ��ͨ�ܽ�ʹ�ã����Թر�IRQ����
   
    }
    
}
/*
*********************************************************************************************************
*                                           DEV_Init()
* Description: ���ֳ�ʼ��
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
 
    
    WT588D_BUSY_INEN;                          //GPS���ڲ�����9600
    SCI_BR0(9600);                     //GPRS���ڲ�����115200
    SCI_BR1(9600); 
    
    SCI_BR2(115200);      //ע��˫ģ�������õ���115200������
                                           
    SCI_Init();                                                           

    OutputCompare_Init();                                        //��ʱ����ʼ��
    
    ADC_Init(NULL);                                          //AD��ʼ��
                                                             
    PWMGeneralInit(0,6,6,0xc0);                              //0��Ƶ   01����
    PWMSinglePortInit(7,1,1,1,0);                             //ͨ��7�����SA��Ϊʱ�ӣ���ʼ�ߵ�ƽ�������
    PWMConcatenatePortSetting(7,500,500);            //���PWM Freq=4kHz
    PWMSinglePortInit(5,1,1,1,0);                             //ͨ��5�����SA��Ϊʱ�ӣ���ʼ�ߵ�ƽ�������
    PWMConcatenatePortSetting(5,500,500);            //���PWM Freq=4kHz
    Init_PT_ICapture();
     
    ///////////////////////
    RTI_Init();
   
    device.data = 0;
    

   
    ////�����̵��������س�ʼ��
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
    DirectLEDGREEN= 1; //Ͷ�һ��ֻ��⳵��ָʾ����
    DirectLEDRED= 0; //ϵͳ�ϵ�֮��ָʾ����
    BT_RESET = 0;
    GPRS_init_flag = GPRS_RESTART;  
    GPS_init_flag = GPS_RESTART;          
}

void CarDataInit(void) 
{
    uint8 j;
    //EEPROM_INITIAL();  //???������
    flash_init();
    ReadCarPara();      //��ȡ���ò���
    ReadCarThreshold(); //��ȡ��ֵ��

    gCarInfor.state = CarReady;
    gCarInfor.userid = 0;
    gCarInfor.Event = 0;
    
  //��ʼ����ǰλ��
    gCarInfor.lLatitude1 = 0;
    gCarInfor.lLatitude2 = 0;
    gCarInfor.lLongitude1 = 0;
    gCarInfor.lLongitude2 = 0;
    gCarInfor.maxSpeed = gCarThreshold.maxSpeed;
    gCarInfor.preMaxSpeed = gCarInfor.maxSpeed;
    gCarInfor.stopTimeout = gCarThreshold.stopTimeout;
    
    gCarThreshold.errCode = 0;
    gCarInfor.maxMileage = 10000;
    //��ȡ����
    gCarInfor.seed = 0x12768967; 
    gCarInfor.speedRate = 0;
    gCarInfor.MCUResetFlag=0;
    
    
    TripDataInit();
  
    //0603yzl
    //��ʼ���������������
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

//�г����ݳ�ʼ��
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
    //����ģʽ��
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

   
