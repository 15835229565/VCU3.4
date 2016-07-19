#ifndef _VOICE_H_
#define _VOICE_H_

/************************************日常常用语音*******************************/

#define  PAY_ATTENTION_VOICE           0      //请注意
#define  WELCOME1_VOICE                1      //欢迎使用萝卜智能车
#define  WELCOME2_VOICE                2      //萝卜小车欢迎您的使用
#define  SELFTEST_FAILED1_VOICE        3      //车辆自检失败
#define  SELFTEST_FAILED2_VOICE        4      //我生病了，无法为您服务
#define  SORRY1_VOICE                  5      //对不起
#define  SORRY2_VOICE                  6      //抱歉抱歉
#define  STOP_IMMEDIATE1_VOICE         7      //请及时停车
#define  STOP_IMMEDIATE2_VOICE         8      //快停车快停车
#define  NETWORK_FAILED1_VOICE         9      //网络通讯异常
#define  NETWORK_FAILED2_VOICE         10      //网络好烂啊，我找不到主人了
#define  GPS_FAILED1_VOICE             11      //位置定位异常
#define  GPS_FAILED2_VOICE             12      //这是什么地方，我找不到方向了
#define  BACKUP1_VOICE                 13       //倒车
#define  BACKUP2_VOICE                 14       //各单位请注意我要倒车啦
#define  OVERSPEED1_VOICE              15       //您已超速
#define  OVERSPEED2_VOICE              16       //您开的太快了
#define  LACK_MONEY1_VOICE             17       //您的账户可用余额不足
#define  LACK_MONEY2_VOICE             18       //工钱用完了，我要罢工了
#define  LOW_BATTERY1_VOICE            19      //电池电量过低
#define  LOW_BATTERY2_VOICE            20      //电池没电了，我跑不动了
#define  OUT_FENCE1_VOICE              21      //行驶区域超出限定范围
#define  OUT_FENCE2_VOICE              22      //这边不让开，快回去吧
#define  COIN_OVER_MILEAGE1_VOICE      23      //投币行程已到
#define  COIN_OVER_MILEAGE2_VOICE      24      //这点钱，我只能送您到这了
#define  STOPPING_CAR1_VOICE           25      //即将停车
#define  STOPPING_CAR2_VOICE           26      //我要停车了
#define  CONTROL_SPEED1_VOICE          27      //请控制车速
#define  CONTROL_SPEED2_VOICE          28      //开慢点
#define  MOBILE_RENT1_VOICE            29      //请使用手机进行租车
#define  MOBILE_RENT2_VOICE            30      //快点下载手机客户端，感受租车新体验
#define  CAR_IN_MAINTAIN1_VOICE        31      //目前车辆进入维护状态，无法使用
#define  CAR_IN_MAINTAIN2_VOICE        32      //对不起，我要检查身体，无法为您服务
#define  COLLISION1_VOICE              33      //检测到严重碰撞
#define  COLLISION2_VOICE              34      //撞的我好痛啊
#define  STOP_STOP_VOICE               35      //停车
#define  OVERTEMP_BATTERY1_VOICE       36      //电池温度过高
#define  OVERTEMP_BATTERY2_VOICE       37      //我的电池好热啊，快点停车
#define  RENT_OVERTIME1_VOICE          38      //该车租用超时，已被还车。
#define  RENT_OVERTIME2_VOICE          39      //我等你等的太久，已还车了。
#define  STOP_CHARGE1_VOICE            40      //目前已停止计费
#define  STOP_CHARGE2_VOICE            41      //现在不收你工钱了
#define  CAR_TILT1_VOICE               42      //车停太斜了，无法开车
#define  CAR_TILT2_VOICE               43      //站都站不稳，没法走了



/************************************标定以及配置语音*******************************/

#define    CVST                        44
#define  DIP_CAL_VOICE                 CVST+0      //进入车辆倾角标定模式，请确保车已处于水平位置
#define  DIP_CAL_SUCC_VOICE            CVST+1     //车辆倾角标定成功
#define  ACCEL_PEDAL_CAL_VOICE         CVST+2     //进入踏板标定模式
#define  ACCEL_PEDAL_BOTTOM_VOICE      CVST+3     //请将踏板踩到底
#define  ACCEL_PEDAL_TOP_VOICE         CVST+4     //请完全松开踏板
#define  ACCEL_PEDAL_CAL_SUCC_VOICE    CVST+5     //车辆踏板标定成功
#define  BATTERY_CAL_VOICE             CVST+6     //进入车辆电压电流标定模式
#define  BATTERY_CAL_SUCC_VOICE        CVST+7     //车辆电压电流标定成功
#define  WRITE_PARA_VOICE              CVST+8     //正在进行车辆参数读取
#define  READ_PARA_VOICE               CVST+9     //正在进行车辆参数写入
#define  UPDATE_SW_SUCC_VOICE          CVST+10    //程序更新成功
#define  UPDATE_SW_FAILED_VOICE        CVST+11    //程序更新失败
#define  START_UPDATE_SW_VOICE         CVST+12    //程序更新中，请勿插拔控制器
#define  ENTER_SW_UPDATE_MODE_VOICE    CVST+13    //注意，进入程序更新模式
#define  ACCEL_SENSOE_VOICE            CVST+14      //加速度传感器
#define  RESET_VOICE                   CVST+15      //重新启动
#define  BRAKE_START_VOICE             CVST+16      //请将踏板踩到刹车起始点
#define  BATTERY_CAL_FAILED_VOICE      CVST+17      //车辆电压电流标定失败


/************************************下线检测语音*******************************/
#define    TVST                        CVST+18
#define  ENTER_TEST_MODE_VOICE         TVST+0     //进入测试模式
#define  TEST_FAILED_VOICE             TVST+1      //测试失败
#define  TEST_SUCC_VOICE               TVST+2      //测试成功
#define  CAL_FAILED_VOICE              TVST+3      //标定失败
#define  CAL_SUCC_VOICE                TVST+4      //标定成功
#define  BATTERY_VOLT_VOICE            TVST+5      //电池电压
#define  BATTERY_CURR_VOICE            TVST+6      //电池电流
#define  BATTERY_TEMP_VOICE            TVST+7      //电池温度
#define  UART_FAILED_VOICE             TVST+8      //串口

#define  NUMBER_0_VOICE                TVST+9      //零
#define  NUMBER_1_VOICE                TVST+10      //一
#define  NUMBER_2_VOICE                TVST+11      //二
#define  NUMBER_3_VOICE                TVST+12      //三
#define  NUMBER_4_VOICE                TVST+13     //四
#define  NUMBER_5_VOICE                TVST+14      //五
#define  NUMBER_6_VOICE                TVST+15      //六
#define  NUMBER_7_VOICE                TVST+16      //七
#define  NUMBER_8_VOICE                TVST+17      //八
#define  NUMBER_9_VOICE                TVST+18      //九
#define  NUMBER_10_VOICE               TVST+19      //十
#define  PIN_VOICE                     TVST+20      //号管脚

#define  BT_TEST_VOICE                 TVST+21      //蓝牙
#define  CAN_TEST_VOICE                TVST+22      //can总线
#define  GPRS_TEST_VOICE               TVST+23      //通讯模块
#define  GPS_TEST_VOICE                TVST+24      //定位模块
#define  ALL_TEST_SUCC_VOICE           TVST+25      //功能测试完成，未发现问题


#define  CAR_CHECK_ABNORM_VOICE        TVST+26      //车辆存在故障
#define  CAR_MOTOR_ABNORM_VOICE        TVST+27      //电机故障
#define  CAR_PEDAL_ABNORM_VOICE        TVST+28      //踏板故障
#define  CAR_RADAR_ABNORM_VOICE        TVST+29      //雷达故障


/************************************实车测试备用语音*******************************/

#define  DVST                          TVST+30
#define  SUDDEN_TURN_VOICE             DVST        //急转弯
#define  UP_SLOPE_VOICE                DVST+1      //上坡中
#define  DOWN_SLOPE_VOICE              DVST+2      //下坡中
#define  SPEED_UP_VOICE                DVST+3      //急加速
#define  SPEED_DOWN_VOICE              DVST+4      //急刹车

#define  CAR_MAINTAIN_VOICE            DVST+5      //维护
#define  CAR_MOBILE_RENT_VOICE         DVST+6      //手机租车
#define  CAR_COIN_RENT_VOICE           DVST+7      //投币租车
#define  CAR_READY_VOICE               DVST+8      //待机
#define  CAR_ORDER_VOICE               DVST+9      //预定
#define  STATE_VOICE                   DVST+10      //状态
#define  BT_STARTUP_CAR_VOICE          DVST+11      //蓝牙开车
#define  SUCC_VOICE                    DVST+12      //成功
#define  FAILED_VOICE                  DVST+13      //失败

#define  CAL_ZERO_FAILED_VOICE         DVST+14      //调零失败 
#define  CAL_ZERO_SUCC_VOICE           DVST+15      //调零成功
#define  CAL_ZERO_START_VOICE          DVST+16      //调零开始
#define  TEST_START_VOICE              DVST+17      //测试开始
#define  FIRST_OP_VOICE                DVST+18      //欢迎使用萝卜车，开车请用力踩住踏板后慢慢松开至一半的位置。
                                                    //停车请踩住踏板，车辆完全停止后快速松开踏板。倒车请按右手边
                                                    //的绿色按钮。详情请参看操作指南.
#define  CAR_TILT3_VOICE               DVST+19      //车停太斜了，无法还车
#define  CAR_TILT4_VOICE               DVST+20      //站都站不稳，没法还车                                                                                                  
#define  EEPROM_WRITE_VOICE            DVST+21      //写入成功
#define  REMAIN_TIME                   DVST+22      //您的使用时间还剩一分钟
#define  REMAIN_MILEAGE                DVST+23      //您还能开100米
#define  MUSIC_VOICE                   DVST+24      //音乐
#define  TEMP_SENSEOR_FORGET_VOICE     DVST+25      //电池温度传感器未插
#define  CAR_LOCK_VOICE                DVST+26      //锁车成功
#define  CAR_UNLOCK_VOICE              DVST+27      //开车成功
#define  CAR_TILT5_VOICE               DVST+28      //坡太陡了，我爬不上去
#define  CAR_FULL_STOP_VOICE           DVST+29      //请在车辆停稳后，再还车
#define  CAR_TIYAN_VOICE               DVST+30      //请靠边停车
#define  CAR_WANGQIAN_VOICE            DVST+31      //不能往前开了，再开我就回不去了!

#define  DEBUG_SERVER_VOICE            DVST+32      //正在连接测试服务器
#define  RELEASE_SERVER_VOICE          DVST+33      //正在连接正式服务器
#define  EXTERN_EEPROM_VOICE           DVST+34      //外部存储器
#define  TURN_LEFT_VOICE               DVST+35      //左转
#define  TURN_RIGHT_VOICE              DVST+36      //右转
#define  CAN_UPDATESW_VOICE            DVST+37      //请用看总线进行程序更新
#define  CAR_RETURN_VOICE              DVST+38      //还车成功
#define  DONOT_FORGET_VOICE            DVST+39      //请勿遗留个人物品在车上
#define  NO_SIMCARD_VOICE              DVST+40      //未检测到通信卡
#define  PRODUCTION_MODE_VOICE         DVST+41      //进入生产模式
#define  GPRS_UPDATESW_SUCC_VOICE      DVST+42      //远程程序更新成功
#define  GPRS_UPDATESW_FAIL_VOICE      DVST+43      //远程程序更新失败
#define  REBOOT_OLDAPP_VOICE           DVST+44      //启动旧版本应用程序
#define  REMIND_RECHARGE_VOICE         DVST+45      //请再次充值
#define  TRIP_TEN_MINUTE_VOICE         DVST+46      //您的租用行程还剩余十分钟
#define  TRIP_FIVE_MINUTE_VOICE        DVST+47      //您的租用行程还剩余五分钟
#define  TRIP_REMAIN_VOICE             DVST+48      //您的租用行程还剩余
#define  MINUTE_VOICE                  DVST+49      //分钟
#define  ACCOUNT_ZERO_VOICE            DVST+50      //您的账户可用余额为零
#define  START_DEDUCT_DEPOIST_VOICE    DVST+51      //开始扣除账户押金
#define  RETURN_CAR_VOICE              DVST+52      //请及时归还车辆至还车点
#define  CAR_STOP_TIME_VOICE           DVST+53      //分钟后车辆将停止运行
#define  SPEED_LIMIT_VOICE             DVST+54      //进入限速模式
#define  OVERTIME_DEDUCT_VOICE         DVST+55      //超时将扣除账户押金
#define  DEDUCT_DEPOIST_VOICE          DVST+56      //扣除押金
#define  YUAN_VOICE                    DVST+57      //元
#define  TWENTY_VOICE                  DVST+58      //二十
#define  THIRTY_VOICE                  DVST+59      //三十
#define  FORTY_VOICE                   DVST+60      //四十
#define  FIFTY_RECHARGE_VOICE          DVST+61      //五十
#define  ONE_HUNDRED_VOICE             DVST+62      //一百
#define  HUNDRED_VOICE                 DVST+63      //百
#define  THOUSAND_VOICE                DVST+64      //千
#define  STOP_VOICE                    0xFE         //停止语音播放
/**********************************************************************************/

void WT588D3LSend(uint8 data);
void HighestVolume() ;
void MiddleVolume() ;
void LowestVolume();
void  MakeSound(uint8 num) ;
uint8 WT588BUSY() ;
uint8 AlarmVoice(uint8 id);
uint8 AlarmTwoVoice(uint8 id1,uint8 id2);
void WT588BusyValidVolt();
void VoiceQueueInit();
uint8 VoiceQueueAdd(uint8 vnum1,uint8 vnum2);
uint8 VoiceQueueGet();
uint8 VoiceQueueSub();
uint8 SearchVoiceQueue(uint8 vnum);
void VoiceProcess();
void WelcomeVoice();
#endif



