#ifndef    _INCLUDE_H_
#define   _INCLUDE_H_

/*
*********************************************************************************************************
*                                         STANDARD LIBRARIES
*********************************************************************************************************
*/
#pragma MESSAGE DISABLE C1420   //result of function-call is ignored 调用memset\memcpy等函数造成，可忽略
#pragma MESSAGE DISABLE C2705   //possile loss of data 移位时造成的，可忽略
#pragma MESSAGE DISABLE C1858   //partial implicit parameter declaration,隐式函数申明，可忽略
#pragma MESSAGE DISABLE C5900   //result is zero 移位操作引起，可忽略

/***********************程序模式*****************************/  

#define DEBUG_MODE     1        //1表示调试模式  0表示正常模式

/***********************服务器类型*****************************/  
#define  RELEASE_SERVER     0x01
#define  DEBUG_SERVER       0x00
#define  SERVER_MODE    DEBUG_SERVER 

/***********************网络模式*****************************/  
#define  TCP_MODE     0x01
#define  UDP_MODE     0x00
#define  GPRS_MODE    UDP_MODE         

/***********************VCU软硬件版本*****************************/ 
#define  HARDWARE_VERSION        0x0304      //初始硬件版本，高字节表示大版本，低字节表示小版本
#define  SOFTWARE_VERSION        0x0300      //初始软件版本，高字节表示大版本，低字节表示小版本      
#define  SOFTWARE_UPDATE_DATE    0x0917      //初始软件版本发布日期，月和日

#include  <hidef.h>            /* common defines and macros */
#include  "typedefs.h"
#include  "derivative.h"      /* derivative-specific definitions */
#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>          
#include  <math.h>     
/*
*********************************************************************************************************
*                                              APP / BSP
*********************************************************************************************************
*/
#include "init.h"
#include "gprs.h"
#include "usart.h"
#include "gps.h"
#include "tcpudp.h"
#include "adc.h"
#include "timer.h"
#include "pwm.h"                 
#include "app.h"
#include "io.h"
#include "can.h"
#include "flash.h"
#include"voice.h"
#include "Ultrasonic.h"
#include "security.h"
#include "Bluetooth.h"
#include "carctrl.h"
/***********************程序更新模式定义*****************************/  
#define BOOT_MODE         0
#define CAN_UPDATE_MODE   1
#define GPRS_UPDATE_MODE  2
#define APP_MODE          3
/***********************语音播放状态*****************************/  
#define PLAY_START         0
#define PLAY_NOW            1
#define PLAY_END           2


/***********************超声波雷达区域定义*****************************/  
#define  FA_AREA_START       10              //为前方A区域起始
#define  FA_AREA_DOUBLESTART 60              //为前方A区域起始
#define  FA_AREA_END         80              //为前方A区域结束
#define  FB_AREA_START       81              //为前方B区域起始
#define  FB_AREA_END         160             //为前方B区域结束
#define  FC_AREA_START       161             //为前方C区域起始
#define  FC_AREA_END         250             //为前方C区域结束
#define  FD_AREA_START       251             //为前方D区域开始
#define  FD_AREA_END         505             //为前方D区域结束
#define  STATIS_NUM          15               //采集次数


#define  FA_AREA             0
#define  FB_AREA             1
#define  FC_AREA             2
#define  FD_AREA             3


#define  RA_AREA_START       10              //为后方A区域起始
#define  RA_AREA_END         60              //为后方A区域结束
#define  RB_AREA_START       61              //为后方B区域起始
#define  RB_AREA_END         160             //为后方B区域结束
#define  RC_AREA_START       161             //为后方C区域起始
#define  RC_AREA_END         250             //为后方C区域结束
#define  RD_AREA_START       251             //为后方D区域开始
#define  RD_AREA_END         505             //为后方D区域结束

#define  RA_AREA             0
#define  RB_AREA             1
#define  RC_AREA             2
#define  RD_AREA             3


#define  FAREA_REVISE         20             //不同情况下 前向超声探测区域的修正值
#define  RAREA_REVISE         0             //不同情况下 后向超声探测区域的修正值

 //系统工作模式 0:停止运营；1：投币模式（默认）；2：手机模式；3：投币手机混合运营
/***********************系统运营模式*****************************/  
#define  STOP_OPERATION                 0x00
#define  COIN_OPERATION                 0x01
#define  MOBILE_OPERATION               0x02
#define  COIN_AND_MOBILE_OPERATION      0x03

/***********************手机操作时车辆返回的操作结果状态**********************************/
#define  CAR_OP_SUCC                    0x00    //车辆成功
#define  CAR_ERROR                      0x01    //车辆存在错误码
#define  USERID_ERROR                   0x02    //车辆用户ID错误
#define  CARID_ERROR                    0x03    //车辆ID错误
#define  SPEED_ERROR                    0x04    //车辆车速不为0
#define  DIPANGLE_ERROR                 0x05    //车辆倾角不在正常范围
#define  HW_VERSION_ERROR               0x06    //车辆硬件版本错误
#define  SW_VERSION_ERROR               0x07    //车辆软件版本错误

/***********************用户权限**********************************/
#define  MOBILE_TIMEOUT_AUTH            0x01    //用户超时停车权限
#define  OUT_GPS_FENCE_NOTALLOW_AUTH    0x02
#define  OUT_GPS_FENCE_AUTH             0x04    //允许用户超出GPS围栏权限，
#define  OUT_GPS_FENCE_LOWSPEED_AUTH    0x08    //允许用户超出GPS围栏权限，但降速

#define  MOBILE_TIMEOUT_AUTH_VALID      0x10    //用户超时停车权限是否有效标志位，从网络读取到，则表示有效
#define  OUT_GPS_FENCE_AUTH_VALID       0x20    //GPS围栏权限是否有效标志位，从网络读取到，则表示有效

/**************VCU与后台服务器GPRS通信特殊字符********************/
#define  CMD_START          0xFA          //包头
#define  CMD_END            0xFB          //包尾
#define  CMD_ALIAS          0x5C          //转义

/**************VCU接收GPRS/GPS模块命令的三种状态********************/
#define  WAIT_CMD          0x00           //等待命令
#define  RECV_CMD          0x01           //接收命令
#define  PROC_CMD          0x02           //处理命令

/***********************动态可调参数定义***************************/
#define CarID    6           //车辆编号
#define Timer0   0xEA4      //电机控制  5ms     3KHz（Modify by Ora）
#define Timer3  0x1D4C      //AD采样    10ms

#define ELECMAGRELEASE      0x01         //电磁刹车松开
#define ELECMAGHOLD         0x00         //电磁刹车抱住
  
#define Rollover_safe   40    //
#define Rollover_warn   50    // 


#define GPS_TOLERATE_VALUE        20000  //gps围栏的容忍范围约为150米=20000
#define GPS_BUFFER_SIZE           74            //GPS接收buffer
#define GSM_BUFFER_SIZE           120     //GSM接收buffer

#define SERVERCMD_BUFFER_SIZE     128     //SERVER接收buffer
#define GPRS_RETRY_TIME           3       //GPRS重试次数
#define PHONECMD_BUFFER_SIZE      16 //SERVER接收buffer
/***********************系统状态宏定义***************************/
#define CarReady              0x00     //待机状态
#define CarOrder              0x01     //预定状态
#define CarMaintain           0x02     //维修状态
#define CoinRenting           0x20     //投币租车状态
#define MobileRenting         0x21     //手机租车状态
#define CarLock               0x22     //手机锁车状态
#define CarSelfMaintain       0x23     //车辆进入自维护状态
#define BTRenting             0x24     //蓝牙开车状态

/***********************系统目标状态来源***************************/
#define StateFromCar              0x00     //目标状态来源于车辆自身
#define StateFromServer           0x01     //目标状态来源于服务器


/***********************异常状态宏定义***************************/
#define GPRS_DISCONN_EXCEPTION           0x0002     //后台服务器连接断开
#define LOW_BATTERY_EXCEPTION            0x0004     //电池电量低
#define OUT_GPS_FENCE_EXCEPTION          0x0008     //超出GPS栅栏
#define HIGH_TEMPATURE_EXCEPTION         0x0010     //电池温度过高

/***********************事件宏定义***************************/
#define COIN_RENT_START_EVENT            0x0001     //投币租车开始事件
#define COIN_RENT_END_EVENT              0x0002     //投币租车结束事件
#define BT_RENT_START_EVENT              0x0004     //蓝牙租车开始事件
#define HIGH_COLLISION_EVENT             0x0010     //异常碰撞事件
#define TEACH_VOICE_EVENT                0x0020     //培训语音播放时间事件

/***********************AD采样端口定义***************************/
#define ACCEL_PEDAL_CH        0x00        //加速踏板
#define CORNER_SENSOR1_CH     0x01        //转角传感器一
#define CORNER_SENSOR2_CH     0x02        //转角传感器二
#define DIP_Z_AXIS_CH         0x03        //倾角Z轴
#define BATTERY_VOLT_CH       0x04        //电池电压
#define DIP_Y_AXIS_CH         0x05        //倾角Y轴
#define DIP_X_AXIS_CH         0x06        //倾角X轴
#define BATTERY_CURR_CH       0x09        //电池电流
#define BATTERY_TEMP_CH       0x0A        //电池温度
/***********************MCU工作模式***************************/
#define NORMAL_MODE           0x00        //正常模式
#define TEST_MODE             0x01        //测试模式
#define CONFIG_MODE           0x02        //配置模式

/******************电机控制器型号****************/
#define  LiWei              0x01          //骊威控制器
#define  KeChuang           0x02          //邵总控制器
#define  Qian               0x03          //钱老板控制器
 
/******************电机速度控制****************/
#define  FiveKM             4          //5KM
#define  FourKM             0          //4KM
#define  ThreeKM            1          // 3KM
#define  TwoKM              2          //2KM
#define  OneKM              3          //1KM

/***********************下线检测测试标志位*********************/
#define BLUETOOTH_TEST_FLAG   63          //57位以前为管脚占用
#define GPRS_TEST_FLAG        62
#define GPS_TEST_FLAG         61
#define DIPANGLE_TEST_FLAG    60
#define BATTERY_CAL_FLAG      59
#define BATTERY_BASE_FLAG     58
#define EXEEPROM_TEST_FLAG    57

#define MCU_REF_VOLT_FLAG     25
#define MCU_REF_GND_FLAG      24
#define VOICE_OUTPUT2_FLAG    23
#define VOICE_OUTPUT1_FLAG    22
/***********************全局变量定义***************************/

extern uint16 MatrixAD[11];

/**********************定时器中断相关变量*************************/
extern uint16 TimerA_Counter;
extern uint16 Timer0Counter ;
extern uint8 freq_2s;
extern uint8 freq_5s;

extern uint8 freq_15s;
extern uint8 freq_60s;
extern uint8 rent_freq_flag;
extern uint8 standby_freq_flag;

extern uint8  phoneVerifFlag;


extern uint16 Motorstep_per_sec_count ;

extern uint16 gGPRSConnectTimer;
extern uint16 gGprsInitialTimer;
extern uint16 gGPSLocationTimer;

extern uint8  coin_plus_trig;


/*******************************车身状态相关参数定义*********************************/

extern union  devicestate device;

/****************GPRS/GPS相关参数定义***********************/
                                         //GPRS模块上电标志
extern uint8 GSM_MSG_Buf[GSM_BUFFER_SIZE];       //接收到的GSM消息

extern uint8 GSM_buffer_p;

extern uint8 ServerCommand_flag;
extern uint8 ServerCommand[SERVERCMD_BUFFER_SIZE];
extern uint8 GPS_message_flag;

extern uint8 GPS_BUFFER[GPS_BUFFER_SIZE];
extern uint8 GPSRecvIndex;
extern uint8 usart0word;

extern uint8 PhoneCommand_flag;
extern uint8 PhoneCommand[PHONECMD_BUFFER_SIZE];
/*****************AD数据数组*******************************/
extern uint16 ADtemp[11][20];
#endif



