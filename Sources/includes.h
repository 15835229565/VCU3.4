#ifndef    _INCLUDE_H_
#define   _INCLUDE_H_

/*
*********************************************************************************************************
*                                         STANDARD LIBRARIES
*********************************************************************************************************
*/
#pragma MESSAGE DISABLE C1420   //result of function-call is ignored ����memset\memcpy�Ⱥ�����ɣ��ɺ���
#pragma MESSAGE DISABLE C2705   //possile loss of data ��λʱ��ɵģ��ɺ���
#pragma MESSAGE DISABLE C1858   //partial implicit parameter declaration,��ʽ�����������ɺ���
#pragma MESSAGE DISABLE C5900   //result is zero ��λ�������𣬿ɺ���

/***********************����ģʽ*****************************/  

#define DEBUG_MODE     1        //1��ʾ����ģʽ  0��ʾ����ģʽ

/***********************����������*****************************/  
#define  RELEASE_SERVER     0x01
#define  DEBUG_SERVER       0x00
#define  SERVER_MODE    DEBUG_SERVER 

/***********************����ģʽ*****************************/  
#define  TCP_MODE     0x01
#define  UDP_MODE     0x00
#define  GPRS_MODE    UDP_MODE         

/***********************VCU��Ӳ���汾*****************************/ 
#define  HARDWARE_VERSION        0x0304      //��ʼӲ���汾�����ֽڱ�ʾ��汾�����ֽڱ�ʾС�汾
#define  SOFTWARE_VERSION        0x0300      //��ʼ����汾�����ֽڱ�ʾ��汾�����ֽڱ�ʾС�汾      
#define  SOFTWARE_UPDATE_DATE    0x0917      //��ʼ����汾�������ڣ��º���

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
/***********************�������ģʽ����*****************************/  
#define BOOT_MODE         0
#define CAN_UPDATE_MODE   1
#define GPRS_UPDATE_MODE  2
#define APP_MODE          3
/***********************��������״̬*****************************/  
#define PLAY_START         0
#define PLAY_NOW            1
#define PLAY_END           2


/***********************�������״�������*****************************/  
#define  FA_AREA_START       10              //Ϊǰ��A������ʼ
#define  FA_AREA_DOUBLESTART 60              //Ϊǰ��A������ʼ
#define  FA_AREA_END         80              //Ϊǰ��A�������
#define  FB_AREA_START       81              //Ϊǰ��B������ʼ
#define  FB_AREA_END         160             //Ϊǰ��B�������
#define  FC_AREA_START       161             //Ϊǰ��C������ʼ
#define  FC_AREA_END         250             //Ϊǰ��C�������
#define  FD_AREA_START       251             //Ϊǰ��D����ʼ
#define  FD_AREA_END         505             //Ϊǰ��D�������
#define  STATIS_NUM          15               //�ɼ�����


#define  FA_AREA             0
#define  FB_AREA             1
#define  FC_AREA             2
#define  FD_AREA             3


#define  RA_AREA_START       10              //Ϊ��A������ʼ
#define  RA_AREA_END         60              //Ϊ��A�������
#define  RB_AREA_START       61              //Ϊ��B������ʼ
#define  RB_AREA_END         160             //Ϊ��B�������
#define  RC_AREA_START       161             //Ϊ��C������ʼ
#define  RC_AREA_END         250             //Ϊ��C�������
#define  RD_AREA_START       251             //Ϊ��D����ʼ
#define  RD_AREA_END         505             //Ϊ��D�������

#define  RA_AREA             0
#define  RB_AREA             1
#define  RC_AREA             2
#define  RD_AREA             3


#define  FAREA_REVISE         20             //��ͬ����� ǰ����̽�����������ֵ
#define  RAREA_REVISE         0             //��ͬ����� ������̽�����������ֵ

 //ϵͳ����ģʽ 0:ֹͣ��Ӫ��1��Ͷ��ģʽ��Ĭ�ϣ���2���ֻ�ģʽ��3��Ͷ���ֻ������Ӫ
/***********************ϵͳ��Ӫģʽ*****************************/  
#define  STOP_OPERATION                 0x00
#define  COIN_OPERATION                 0x01
#define  MOBILE_OPERATION               0x02
#define  COIN_AND_MOBILE_OPERATION      0x03

/***********************�ֻ�����ʱ�������صĲ������״̬**********************************/
#define  CAR_OP_SUCC                    0x00    //�����ɹ�
#define  CAR_ERROR                      0x01    //�������ڴ�����
#define  USERID_ERROR                   0x02    //�����û�ID����
#define  CARID_ERROR                    0x03    //����ID����
#define  SPEED_ERROR                    0x04    //�������ٲ�Ϊ0
#define  DIPANGLE_ERROR                 0x05    //������ǲ���������Χ
#define  HW_VERSION_ERROR               0x06    //����Ӳ���汾����
#define  SW_VERSION_ERROR               0x07    //��������汾����

/***********************�û�Ȩ��**********************************/
#define  MOBILE_TIMEOUT_AUTH            0x01    //�û���ʱͣ��Ȩ��
#define  OUT_GPS_FENCE_NOTALLOW_AUTH    0x02
#define  OUT_GPS_FENCE_AUTH             0x04    //�����û�����GPSΧ��Ȩ�ޣ�
#define  OUT_GPS_FENCE_LOWSPEED_AUTH    0x08    //�����û�����GPSΧ��Ȩ�ޣ�������

#define  MOBILE_TIMEOUT_AUTH_VALID      0x10    //�û���ʱͣ��Ȩ���Ƿ���Ч��־λ���������ȡ�������ʾ��Ч
#define  OUT_GPS_FENCE_AUTH_VALID       0x20    //GPSΧ��Ȩ���Ƿ���Ч��־λ���������ȡ�������ʾ��Ч

/**************VCU���̨������GPRSͨ�������ַ�********************/
#define  CMD_START          0xFA          //��ͷ
#define  CMD_END            0xFB          //��β
#define  CMD_ALIAS          0x5C          //ת��

/**************VCU����GPRS/GPSģ�����������״̬********************/
#define  WAIT_CMD          0x00           //�ȴ�����
#define  RECV_CMD          0x01           //��������
#define  PROC_CMD          0x02           //��������

/***********************��̬�ɵ���������***************************/
#define CarID    6           //�������
#define Timer0   0xEA4      //�������  5ms     3KHz��Modify by Ora��
#define Timer3  0x1D4C      //AD����    10ms

#define ELECMAGRELEASE      0x01         //���ɲ���ɿ�
#define ELECMAGHOLD         0x00         //���ɲ����ס
  
#define Rollover_safe   40    //
#define Rollover_warn   50    // 


#define GPS_TOLERATE_VALUE        20000  //gpsΧ�������̷�ΧԼΪ150��=20000
#define GPS_BUFFER_SIZE           74            //GPS����buffer
#define GSM_BUFFER_SIZE           120     //GSM����buffer

#define SERVERCMD_BUFFER_SIZE     128     //SERVER����buffer
#define GPRS_RETRY_TIME           3       //GPRS���Դ���
#define PHONECMD_BUFFER_SIZE      16 //SERVER����buffer
/***********************ϵͳ״̬�궨��***************************/
#define CarReady              0x00     //����״̬
#define CarOrder              0x01     //Ԥ��״̬
#define CarMaintain           0x02     //ά��״̬
#define CoinRenting           0x20     //Ͷ���⳵״̬
#define MobileRenting         0x21     //�ֻ��⳵״̬
#define CarLock               0x22     //�ֻ�����״̬
#define CarSelfMaintain       0x23     //����������ά��״̬
#define BTRenting             0x24     //��������״̬

/***********************ϵͳĿ��״̬��Դ***************************/
#define StateFromCar              0x00     //Ŀ��״̬��Դ�ڳ�������
#define StateFromServer           0x01     //Ŀ��״̬��Դ�ڷ�����


/***********************�쳣״̬�궨��***************************/
#define GPRS_DISCONN_EXCEPTION           0x0002     //��̨���������ӶϿ�
#define LOW_BATTERY_EXCEPTION            0x0004     //��ص�����
#define OUT_GPS_FENCE_EXCEPTION          0x0008     //����GPSդ��
#define HIGH_TEMPATURE_EXCEPTION         0x0010     //����¶ȹ���

/***********************�¼��궨��***************************/
#define COIN_RENT_START_EVENT            0x0001     //Ͷ���⳵��ʼ�¼�
#define COIN_RENT_END_EVENT              0x0002     //Ͷ���⳵�����¼�
#define BT_RENT_START_EVENT              0x0004     //�����⳵��ʼ�¼�
#define HIGH_COLLISION_EVENT             0x0010     //�쳣��ײ�¼�
#define TEACH_VOICE_EVENT                0x0020     //��ѵ��������ʱ���¼�

/***********************AD�����˿ڶ���***************************/
#define ACCEL_PEDAL_CH        0x00        //����̤��
#define CORNER_SENSOR1_CH     0x01        //ת�Ǵ�����һ
#define CORNER_SENSOR2_CH     0x02        //ת�Ǵ�������
#define DIP_Z_AXIS_CH         0x03        //���Z��
#define BATTERY_VOLT_CH       0x04        //��ص�ѹ
#define DIP_Y_AXIS_CH         0x05        //���Y��
#define DIP_X_AXIS_CH         0x06        //���X��
#define BATTERY_CURR_CH       0x09        //��ص���
#define BATTERY_TEMP_CH       0x0A        //����¶�
/***********************MCU����ģʽ***************************/
#define NORMAL_MODE           0x00        //����ģʽ
#define TEST_MODE             0x01        //����ģʽ
#define CONFIG_MODE           0x02        //����ģʽ

/******************����������ͺ�****************/
#define  LiWei              0x01          //����������
#define  KeChuang           0x02          //���ܿ�����
#define  Qian               0x03          //Ǯ�ϰ������
 
/******************����ٶȿ���****************/
#define  FiveKM             4          //5KM
#define  FourKM             0          //4KM
#define  ThreeKM            1          // 3KM
#define  TwoKM              2          //2KM
#define  OneKM              3          //1KM

/***********************���߼����Ա�־λ*********************/
#define BLUETOOTH_TEST_FLAG   63          //57λ��ǰΪ�ܽ�ռ��
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
/***********************ȫ�ֱ�������***************************/

extern uint16 MatrixAD[11];

/**********************��ʱ���ж���ر���*************************/
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


/*******************************����״̬��ز�������*********************************/

extern union  devicestate device;

/****************GPRS/GPS��ز�������***********************/
                                         //GPRSģ���ϵ��־
extern uint8 GSM_MSG_Buf[GSM_BUFFER_SIZE];       //���յ���GSM��Ϣ

extern uint8 GSM_buffer_p;

extern uint8 ServerCommand_flag;
extern uint8 ServerCommand[SERVERCMD_BUFFER_SIZE];
extern uint8 GPS_message_flag;

extern uint8 GPS_BUFFER[GPS_BUFFER_SIZE];
extern uint8 GPSRecvIndex;
extern uint8 usart0word;

extern uint8 PhoneCommand_flag;
extern uint8 PhoneCommand[PHONECMD_BUFFER_SIZE];
/*****************AD��������*******************************/
extern uint16 ADtemp[11][20];
#endif



