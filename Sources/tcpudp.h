#ifndef    _TCPUDP_H_
#define   _TCPUDP_H_


typedef struct //�˴����ݲ�������EEPROM��
{
    uint32 carid;                  //����ID ΪVIN��ĺ�6λ
    uint32 userid;                 //�û�ID
    uint16 batVolt;               //������ص�ѹ
    uint16 batCurr;               //������ص���
    uint8  batTemp;              //��������¶�
    uint8  state;                    //����״̬
    uint8  destState;               //Ŀ�공��״̬
    uint8  destStateSource;               //Ŀ�공��״̬��Դ 0Ϊ��������״̬ת����Ҫ 1��ʾ������ת����Ҫ
    uint8  maxSpeed;                //��ǰ�û��ܹ�����������ٶ�
    uint8  preMaxSpeed;                //��ǰ�û��ܹ�����������ٶ�
    FP32   speed;                   //�����ٶ� (����ÿСʱ)
    uint16 lLatitude1;             //������γ��
    uint32 lLatitude2;
    uint16 lLongitude1;
    uint32 lLongitude2;
    uint32 tripMileage;          //��ǰ������̣��ף�
    uint32 recovTripMileage;          //��ǰ�ӷ������ָ��ĳ�����̣��ף�
    uint8  tripID;                     //��ǰ�г�ID
    uint32 maxMileage;        //������û��ܹ���ʻ��������
    uint32 seed;                     //����� 
    uint8  reversState;           //����״̬��ǰ��״̬
    sint16 dipAngle;              //���
    sint16 fabAngle;              //ǰ�����
    sint16 updownAngle;
    uint8  accPedal;              //����̤��λ�� �ٷֱ�
    uint8  tripCoin;               //�����Ͷ��������Ͷ���ܶԪ )
    uint16 tripTime;              //������ۼ�ʱ�䣨�룩
    uint16 tripStopTime;              //������ۼ�ͣ��ʱ�䣨�룩
    uint8  batRest;                //ʣ�����
    FP32   batVoltStand;           //��ص�ѹ��׼ֵ
    FP32   batCurrStand;           //��ص�����׼ֵ
    uint16  tripStopOverTimer;      //��ǰ��ʱͣ��ʱ�� ��λ 5��
    uint8  stopTimeout;            //�⳵�����£�ͣ����ʱʱ�䣬��λΪ����    
    uint8  BTTimeout;              //����������ʱʱ��
    FP32   batzeroVoltVolt;             //��ص�ѹΪ��ʱ�Ļ�׼��ѹ
    uint8  iccid[20];
    uint16 userAuthority;          //�û�Ȩ��
    uint8  SatelliteNum;           //���Ǹ���
    FP32   preLatitude;            //ǰһ�γɹ���λ��λ��
    FP32   preLongitude;
    uint16 Event;                  //�¼���־λ
    uint8  speedRate;              //ɲ������£��ٶȱ仯��
    uint8  MCUResetFlag;           //��Ƭ����λ��־
    uint8  RadarCollisionCounter;  //��������״��⵽��ײ�Ĵ��� 
    uint8  VibrateLevel;           //��ǿ��    
    uint8  VoiceQueue[8];          //�������У�����Ҫ���ŵ���������ö����У������ڽ��в���
    uint8  VoiceQueueStart;        //��������ͷλ��
    uint8  VoiceQueueEnd;          //��������β��λ��
    uint8  VoicePlayFlag;          //����״̬
    uint8  VoicePlayValue;         //��̨��Ҫ�������ŵ���������
    uint8  limitSpeed;             //��̨���͵ĳ������������
    
    uint16 availableTime;                    //�ɹ���ʻ����ʱ�䣨���ӣ�
    uint32 availableMetre;                   //�ɹ���ʻ���ܾ��루�ף�
    uint8  controllerType;         //�������������
    uint8  rssi;                   //gprs�ź�ǿ��
    uint16 accelPedalPos;          //����̤��ֵ��ADֵ
}CarInfor;

//���� ��ֵ�Լ�����
typedef struct //�˴����ݱ�����EEPROM��
{
    uint8  batLowLevel;           //�͵���������ֵ0~100
    uint8  batTempLevel;           //����¶ȳ��꾯����ֵ0~255
    uint8  operationMode;         //������Ӫģʽ 0:ֹͣ��Ӫ��1��Ͷ��ģʽ��Ĭ�ϣ���2���ֻ�ģʽ��3��Ͷ���ֻ������Ӫ
    uint16 perCoinMetre;          // ÿӲ�Ҷ�����
    uint16 perCoinTime;           // ÿӲ�ҿ�ռ��С�����ʱ��  
    uint8  shortHeartTime;        //����������
    uint8  longHeartTime;         //����������
    uint8  maxSpeed;              //Ĭ���û��ܹ���������߳���    
    uint8  userLevel;             //�û��ȼ�
    uint16 LTLatitude1;           //�������ϽǾ�γ��
    uint32 LTLatitude2;
    uint16 LTLongitude1;
    uint32 LTLongitude2;
    uint16 RBLatitude1;             //�������½Ǿ�γ��
    uint32 RBLatitude2;
    uint16 RBLongitude1;
    uint32 RBLongitude2;
    uint8  stopTimeout;             //�⳵�����£�ͣ����ʱʱ�䣬��λΪ����
    uint16 errCode;                 //����������
    uint32 totalMileage;            //����� (km)
    uint16 GPRS_Noconnect_Coinnum;  //ԭ�����ڼ�¼���һ��λ�ã����ڸ�Ϊ����Ͽ�ʱͶ�Ҹ���
    uint32 brakeActiveCounter;      //ԭ�����ڼ�¼���һ��λ�ã����ڸ�Ϊɲ��ʹ�ô���
    uint16 NC2;                     //ԭ�����ڼ�¼���һ��λ�ã������ã�Ϊ�˱���flash�洢����һ���ԣ���ɾ����λ��
    uint32 NC3;                     //ԭ�����ڼ�¼���һ��λ�ã������ã�Ϊ�˱���flash�洢����һ���ԣ���ɾ����λ��
    uint8  maxCoin;                 //���Ͷ����
    uint16 maxConnectTimeOut;       //�������ӶϿ���ʱʱ�� 
    uint8  gprsInitFlag;            //gprsģ���Ƿ��Ѿ���ʼ�����λ 
    uint8  severMode;               //������ʽ���������ǲ��Է�������־λ
    uint8  gprsMode;                //TCPģʽ����UDPģʽ  
}CarThreshold;


//���� Ӳ����ز���//�˴����ݱ�����EEPROM��
typedef struct
{
    uint16 motorPower;             //�������
    uint8  hallSensorPara;         //������������תһȦ��������������
    uint16 tireDiameter;           //��ֱ̥��
    FP32  gearRatio;              //���Ӽ�������ٱ�    
    uint16 softwareVersion;         //����汾
    uint16 hardwareVersion;         //Ӳ���汾     
    FP32 mcuRefVoltStand;         //MCU�ο���ѹ��׼ֵ
    FP32 batzeroCurrVolt;             //��ص���Ϊ��ʱ�Ļ�׼��ѹ
    FP32 batVoltReviseCoef;           //��ص�ѹ����ϵ��
    FP32 batCurrReviseCoef;           //��ص�������ϵ��    
    uint16  XAccelBase;           //X���׼ֵ
    uint16  YAccelBase;           //Y���׼ֵ
    uint16  ZAccelBase;           //Z���׼ֵ    
    uint16 minAccelPedal;        //̤����͵�λ��
    uint16 maxAccelPedal;        //̤����ߵ�λ�� 
    uint16 midAccelPedal;        //̤���ɲ����ʼ��
    uint32 testResult1;          //���Խ��1
    uint32 testResult2;          //���Խ��2
    uint16 softwareUpdateDate;      //����������� 
    uint8  VIN[7];                  //����VIN��
    uint8  VCUID[7];                //VCU����
    uint8  name[6];                 //���� 
    
}CarPara;


typedef struct
{
    FP32  MetrePerPulse;             //�������������
    uint32 tripPulseCounter;         //�����г̵����������������
    uint8  pulseCaptureMode;         //���β�׽��ʽ1�����ز�׽ 0�½��ز�׽ 
    uint16 pulseData[16];             //�ɼ����Ĳ�������
    uint8  pulseDataIndex;           //��������λ��
    uint16 preTCNT1;
    uint16 preTCNT2;
    uint8  motorStopFlag;            //���ֹͣ��־λ
    
}MotorInfor;

extern uint32 userIdFromServer; 
extern CarInfor  gCarInfor;              
extern CarThreshold gCarThreshold;       
extern CarPara      gCarPara;
extern MotorInfor   gMotorInfor;   

uint8 ServerCmdProcess(uint8  *servercommand);

uint8 VcuGprsToServerCmd00();
uint8 ServerGprsToVcuCmd00(uint8 *servercommand);

uint8 VcuGprsToServerCmd01();
uint8 ServerGprsToVCUCmd01(uint8 *servercommand); 

uint8 VcuGprsToServerCmd02();
uint8 ServerGprsToVcuCmd02(uint8 *servercommand);

uint8 VcuGprsToServerCmd03(uint8* servercommand); 
uint8 ServerGprsToVcuCmd03(uint8* servercommand);

uint8 VcuGprsToServerCmd04();
uint8 ServerGprsToVcuCmd04(uint8* servercommand);

uint8 VcuGprsToServerCmd05();
uint8 ServerGprsToVcuCmd05(uint8* servercommand);

uint8 VcuGprsToServerCmd06();
uint8 ServerGprsToVcuCmd06(uint8 *servercommand);

uint8 VcuGprsToServerCmd07();
uint8 ServerGprsToVcuCmd07(uint8 *servercommand);

uint8 VcuGprsToServerCmd08();
uint8 ServerGprsToVcuCmd08(uint8 *servercommand);

uint8 VcuGprsToServerCmd09();
uint8 ServerGprsToVcuCmd09(uint8 *servercommand);

uint8 VcuGprsToServerCmd0A();
uint8 ServerGprsToVcuCmd0A(uint8* servercommand);

uint8 VcuGprsToServerCmd0B();
uint8 ServerGprsToVcuCmd0B(uint8* servercommand);


uint8 VcuGprsToServerCmd0C();
uint8 ServerGprsToVcuCmd0C(uint8 *servercommand);

uint8 VcuGprsToServerCmd0D();
uint8 ServerGprsToVcuCmd0D(uint8 *servercommand);


uint8 VcuGprsToServerCmd0E();
uint8 ServerGprsToVcuCmd0E(uint8 *servercommand);

uint8 VcuGprsToServerCmd0F();
uint8 ServerGprsToVcuCmd0F(uint8 *servercommand);

uint8 VcuGprsToServerCmd11();
uint8 ServerGprsToVcuCmd11(uint8* servercommand);

uint8 VcuGprsToServerCmd12();
uint8 ServerGprsToVcuCmd12(uint8 *servercommand);

uint8 VcuGprsToServerCmd13();
uint8 ServerGprsToVcuCmd13(uint8 *servercommand);

uint8 VcuGprsToServerCmd14();
uint8 ServerGprsToVcuCmd14(uint8* servercommand);

uint8 VcuGprsToServerCmd15();
uint8 ServerGprsToVcuCmd15(uint8* servercommand);


uint8 VcuGprsToServerCmd31(uint8 *servercommand);
uint8 ServerGprsToVcuCmd31(uint8 *servercommand);

uint8 VcuGprsToServerCmd32();
uint8 ServerGprsToVcuCmd32(uint8 *servercommand);

uint8 VcuConnectServerProcess(uint8 cmdtype);
uint8 DataEncryption(uint8* data,uint8 len);
uint8 GetCRC(uint8* data,uint8 len);
uint8 StatisticsEscapeChar(uint8* buf,uint8 length);
uint8 CarStateChange();
#endif