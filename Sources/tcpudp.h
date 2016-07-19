#ifndef    _TCPUDP_H_
#define   _TCPUDP_H_


typedef struct //此处数据不保存在EEPROM中
{
    uint32 carid;                  //车辆ID 为VIN码的后6位
    uint32 userid;                 //用户ID
    uint16 batVolt;               //车辆电池电压
    uint16 batCurr;               //车辆电池电流
    uint8  batTemp;              //车辆电池温度
    uint8  state;                    //车辆状态
    uint8  destState;               //目标车辆状态
    uint8  destStateSource;               //目标车辆状态来源 0为车辆自身状态转换需要 1表示服务器转换需要
    uint8  maxSpeed;                //当前用户能够操作的最高速度
    uint8  preMaxSpeed;                //当前用户能够操作的最高速度
    FP32   speed;                   //车辆速度 (公里每小时)
    uint16 lLatitude1;             //车辆经纬度
    uint32 lLatitude2;
    uint16 lLongitude1;
    uint32 lLongitude2;
    uint32 tripMileage;          //当前车辆里程（米）
    uint32 recovTripMileage;          //当前从服务器恢复的车辆里程（米）
    uint8  tripID;                     //当前行程ID
    uint32 maxMileage;        //本租次用户能够行驶的最大距离
    uint32 seed;                     //随机码 
    uint8  reversState;           //倒车状态或前进状态
    sint16 dipAngle;              //倾角
    sint16 fabAngle;              //前后倾角
    sint16 updownAngle;
    uint8  accPedal;              //加速踏板位置 百分比
    uint8  tripCoin;               //本租次投币数量即投币总额（元 )
    uint16 tripTime;              //本租次累计时间（秒）
    uint16 tripStopTime;              //本租次累计停车时间（秒）
    uint8  batRest;                //剩余电量
    FP32   batVoltStand;           //电池电压标准值
    FP32   batCurrStand;           //电池电流标准值
    uint16  tripStopOverTimer;      //当前超时停车时间 单位 5秒
    uint8  stopTimeout;            //租车条件下，停车超时时间，单位为分钟    
    uint8  BTTimeout;              //蓝牙心跳超时时间
    FP32   batzeroVoltVolt;             //电池电压为零时的基准电压
    uint8  iccid[20];
    uint16 userAuthority;          //用户权限
    uint8  SatelliteNum;           //卫星个数
    FP32   preLatitude;            //前一次成功定位的位置
    FP32   preLongitude;
    uint16 Event;                  //事件标志位
    uint8  speedRate;              //刹车情况下，速度变化率
    uint8  MCUResetFlag;           //单片机复位标志
    uint8  RadarCollisionCounter;  //本租次内雷达检测到碰撞的次数 
    uint8  VibrateLevel;           //振动强度    
    uint8  VoiceQueue[8];          //语音队列，将需要播放的语音放入该队列中，并定期进行播放
    uint8  VoiceQueueStart;        //语音队列头位置
    uint8  VoiceQueueEnd;          //语音队列尾巴位置
    uint8  VoicePlayFlag;          //语音状态
    uint8  VoicePlayValue;         //后台需要车辆播放的语音内容
    uint8  limitSpeed;             //后台发送的车辆的最低限速
    
    uint16 availableTime;                    //可供行驶的总时间（分钟）
    uint32 availableMetre;                   //可供行驶的总距离（米）
    uint8  controllerType;         //电机控制器类型
    uint8  rssi;                   //gprs信号强度
    uint16 accelPedalPos;          //加速踏板值，AD值
}CarInfor;

//车辆 阈值以及参数
typedef struct //此处数据保存在EEPROM中
{
    uint8  batLowLevel;           //低电量警告阈值0~100
    uint8  batTempLevel;           //电池温度超标警告阈值0~255
    uint8  operationMode;         //车辆运营模式 0:停止运营；1：投币模式（默认）；2：手机模式；3：投币手机混合运营
    uint16 perCoinMetre;          // 每硬币多少米
    uint16 perCoinTime;           // 每硬币可占用小车最多时间  
    uint8  shortHeartTime;        //短心跳秒数
    uint8  longHeartTime;         //长心跳秒数
    uint8  maxSpeed;              //默认用户能够操作的最高车速    
    uint8  userLevel;             //用户等级
    uint16 LTLatitude1;           //车辆左上角经纬度
    uint32 LTLatitude2;
    uint16 LTLongitude1;
    uint32 LTLongitude2;
    uint16 RBLatitude1;             //车辆右下角经纬度
    uint32 RBLatitude2;
    uint16 RBLongitude1;
    uint32 RBLongitude2;
    uint8  stopTimeout;             //租车条件下，停车超时时间，单位为分钟
    uint16 errCode;                 //车辆错误码
    uint32 totalMileage;            //总里程 (km)
    uint16 GPRS_Noconnect_Coinnum;  //原来用于记录最后一次位置，现在改为网络断开时投币个数
    uint32 brakeActiveCounter;      //原来用于记录最后一次位置，现在改为刹车使用次数
    uint16 NC2;                     //原来用于记录最后一次位置，现弃用，为了保持flash存储区的一致性，不删除该位置
    uint32 NC3;                     //原来用于记录最后一次位置，现弃用，为了保持flash存储区的一致性，不删除该位置
    uint8  maxCoin;                 //最多投币数
    uint16 maxConnectTimeOut;       //网络连接断开超时时间 
    uint8  gprsInitFlag;            //gprs模块是否已经初始化标记位 
    uint8  severMode;               //连接正式服务器还是测试服务器标志位
    uint8  gprsMode;                //TCP模式还是UDP模式  
}CarThreshold;


//车辆 硬件相关参数//此处数据保存在EEPROM中
typedef struct
{
    uint16 motorPower;             //电机功率
    uint8  hallSensorPara;         //霍尔传感器旋转一圈产生的脉冲数量
    uint16 tireDiameter;           //轮胎直径
    FP32  gearRatio;              //增加减速箱的速比    
    uint16 softwareVersion;         //软件版本
    uint16 hardwareVersion;         //硬件版本     
    FP32 mcuRefVoltStand;         //MCU参考电压标准值
    FP32 batzeroCurrVolt;             //电池电流为零时的基准电压
    FP32 batVoltReviseCoef;           //电池电压修正系数
    FP32 batCurrReviseCoef;           //电池电流修正系数    
    uint16  XAccelBase;           //X轴基准值
    uint16  YAccelBase;           //Y轴基准值
    uint16  ZAccelBase;           //Z轴基准值    
    uint16 minAccelPedal;        //踏板最低点位置
    uint16 maxAccelPedal;        //踏板最高点位置 
    uint16 midAccelPedal;        //踏板的刹车起始点
    uint32 testResult1;          //测试结果1
    uint32 testResult2;          //测试结果2
    uint16 softwareUpdateDate;      //程序更新日期 
    uint8  VIN[7];                  //车辆VIN码
    uint8  VCUID[7];                //VCU编码
    uint8  name[6];                 //车牌 
    
}CarPara;


typedef struct
{
    FP32  MetrePerPulse;             //单个脉冲的米数
    uint32 tripPulseCounter;         //本次行程电机产生的脉冲数量
    uint8  pulseCaptureMode;         //波形捕捉方式1上升沿捕捉 0下降沿捕捉 
    uint16 pulseData[16];             //采集到的波形数据
    uint8  pulseDataIndex;           //波形数据位置
    uint16 preTCNT1;
    uint16 preTCNT2;
    uint8  motorStopFlag;            //电机停止标志位
    
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