#ifndef    _IO_H_
#define   _IO_H_



#define     SET_GPIO_PIN(x)     x=1
#define     CLR_GPIO_PIN(x)     x=0

#define     GPIO_PORT_T                1
#define     GPIO_PORT_S                2
#define     GPIO_PORT_P                3
#define     GPIO_PORT_J                4
#define     GPIO_PORT_AD0              5
#define     GPIO_PORT_AD8              6
#define     GPIO_PIN_0_MASK     0x01u                                            
#define     GPIO_PIN_1_MASK     0x02u
#define     GPIO_PIN_2_MASK     0x04u
#define     GPIO_PIN_3_MASK     0x08u
#define     GPIO_PIN_4_MASK     0x10u
#define     GPIO_PIN_5_MASK     0x20u
#define     GPIO_PIN_6_MASK     0x40u
#define     GPIO_PIN_7_MASK     0x80u
#define     GPIO_HIGH_LEVEL     0x01u
#define     GPIO_LOW_LEVEL      0x00u


/***********************输出口定义***************************/
#define Slotmachine              PTJ_PTJ0    //投币器电源
#define HeadlampRelay            PTJ_PTJ1    //大灯继电器
#define DaocheRelay              PTJ_PTJ2    //倒车信号继电器
#define YuanguangRelay           PTJ_PTJ3    //远光灯继电器
#define BrakeRelay               PTJ_PTJ4    //电磁刹继电器
#define GPS_RESET                PTJ_PTJ5    //GPS模块复位

#define GPRSPower                PTJ_PTJ7    //gprs模块上电信号

#define NightlampSwitch          PTP_PTP0       //夜灯控制
#define LeftlampSwitch           PTP_PTP1     	//左转灯控制
#define RightlampSwitch          PTP_PTP2       //右转灯控制
#define ReverselampSwitch        PTP_PTP3       //刹车灯控制
#define zuche1                   PTP_PTP5
#define zuche2                   PTP_PTP6
#define BT_RESET                 PTT_PTT1       //蓝牙模块复位控制
#define HornSwitch               PTT_PTT2       //蜂鸣器开关

#define FEEDWATCHDOG             PTT_PTT6      //外部看门狗喂狗信号

 
#define EX_EE_SCL_OUTEN         { DDRJ_DDRJ6 = 1;}
#define EX_EE_SCL_INEN          { DDRJ_DDRJ6 = 0;}
#define EX_EE_SCL_OUT           PTJ_PTJ6   //外部EEPROM SCL管脚输出寄存器
#define EX_EE_SCL_IN            PTJ_PTJ6  //外部EEPROM SCL管脚输入寄存器


#define EX_EE_SDA_OUTEN         {DDRS |= GPIO_PIN_7_MASK;}
#define EX_EE_SDA_INEN          {DDRS &= ~GPIO_PIN_7_MASK;}
#define EX_EE_SDA_OUT           PTS_PTS7   //外部EEPROM SDA管脚输出寄存器
#define EX_EE_SDA_IN            PTS_PTS7  //外部EEPROM SDA管脚输入寄存器


#define GPRSReset                PTT_PTT7         //gprs模块复位信号


#define DirectLEDGREEN        PT0AD_PT0AD5        // 指示灯绿灯，投币或手机租车状态指示灯  5 
#define DirectLEDRED          PT0AD_PT0AD4        // 指示灯红灯，系统上电指示灯         4

                        
#define select81A               PTS_PTS4
#define select81B               PTS_PTS5
#define select81C               PTS_PTS6               

//语音芯片相关管脚定义//

#define WT588D_BUSY_INEN        {DDR1AD &=~GPIO_PIN_7_MASK; ATDDIENL |=GPIO_PIN_7_MASK;}
#define WT588D_BUSY_PIN         PTI1AD_PTI1AD7
 
                      
#define WT588D_DATA_PxDIR      DDR0AD
#define WT588D_DATA_PxOUT      PT0AD
#define WT588D_DATA_PIN        0x08       //PAD11

#define WT588D_CS_PxDIR        DDR0AD
#define WT588D_CS_PxOUT        PT0AD
#define WT588D_CS_PIN          0x80       //PAD15

#define WT588D_CLK_PxDIR       DDR0AD
#define WT588D_CLK_PxOUT       PT0AD
#define WT588D_CLK_PIN         0x40       //PAD14

/***********************输入口定义***************************/
#define Select81Output         PTI0AD_PTI0AD0

/******************CAN总线相关管脚定义*********************/
#define CAN_PxDIR             DDRM
#define CAN_PxOUT             PTM
#define CAN_RXCAN_PIN         0x01
#define CAN_TXCAN_PIN         0x02

#define CAN_S_PxDIR           DDRS
#define CAN_S_PxOUT           PTS
#define CAN_S_PIN             0x20


typedef struct GPIO_Port_Tile
{
    uint8                   PortIndex;//   PORT T\S\P\J\AD0\AD8端口索引
    uint8                   Direction;//   PP0-7输入输出控制
    uint8                   PullEnable;  //使能PP0-7下拉或上拉电阻     用于输入
    uint8                   PolaritySelect; //选择下拉电阻同时上升沿有效
    uint8                   InputEnable; // AD输入模式
    uint8                   AD0Set;
} GPIO_Port_Type,*GPIO_Port_Ref;
typedef struct GPIO_Cfg_Tile
{
    uint8   PortCount;
    GPIO_Port_Ref   GPIO_Port_Cfg;
} GPIO_Cfg_Type,*GPIO_Cfg_Ref;

struct DEVICESTATE
{
    uint8 Headlamp:1;
    uint8 Leftlamp:1;
    uint8 Rightlamp:1;
    uint8 Nightlamp:1;
    uint8 Horn:1;
    uint8 Wiper:1;
    uint8 SlotmachineState:1;
    uint8 ReverseRelayState:1;
};
union  devicestate
{
    struct DEVICESTATE state;
    uint8 data;
};

extern uint8 SwitchChangeFlag;                                 
uint8 Read74HC151(uint8 Dx);
void GPIO_init(GPIO_Cfg_Ref  GPIO_Ptr);
void CoinSignalProcess();
void IO_Control(void);
void BodyEquipControl();
void CarStatusIndicate();
void TripInforCalc();
#endif