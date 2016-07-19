#ifndef _VOICE_H_
#define _VOICE_H_

/************************************�ճ���������*******************************/

#define  PAY_ATTENTION_VOICE           0      //��ע��
#define  WELCOME1_VOICE                1      //��ӭʹ���ܲ����ܳ�
#define  WELCOME2_VOICE                2      //�ܲ�С����ӭ����ʹ��
#define  SELFTEST_FAILED1_VOICE        3      //�����Լ�ʧ��
#define  SELFTEST_FAILED2_VOICE        4      //�������ˣ��޷�Ϊ������
#define  SORRY1_VOICE                  5      //�Բ���
#define  SORRY2_VOICE                  6      //��Ǹ��Ǹ
#define  STOP_IMMEDIATE1_VOICE         7      //�뼰ʱͣ��
#define  STOP_IMMEDIATE2_VOICE         8      //��ͣ����ͣ��
#define  NETWORK_FAILED1_VOICE         9      //����ͨѶ�쳣
#define  NETWORK_FAILED2_VOICE         10      //������ð������Ҳ���������
#define  GPS_FAILED1_VOICE             11      //λ�ö�λ�쳣
#define  GPS_FAILED2_VOICE             12      //����ʲô�ط������Ҳ���������
#define  BACKUP1_VOICE                 13       //����
#define  BACKUP2_VOICE                 14       //����λ��ע����Ҫ������
#define  OVERSPEED1_VOICE              15       //���ѳ���
#define  OVERSPEED2_VOICE              16       //������̫����
#define  LACK_MONEY1_VOICE             17       //�����˻���������
#define  LACK_MONEY2_VOICE             18       //��Ǯ�����ˣ���Ҫ�չ���
#define  LOW_BATTERY1_VOICE            19      //��ص�������
#define  LOW_BATTERY2_VOICE            20      //���û���ˣ����ܲ�����
#define  OUT_FENCE1_VOICE              21      //��ʻ���򳬳��޶���Χ
#define  OUT_FENCE2_VOICE              22      //��߲��ÿ������ȥ��
#define  COIN_OVER_MILEAGE1_VOICE      23      //Ͷ���г��ѵ�
#define  COIN_OVER_MILEAGE2_VOICE      24      //���Ǯ����ֻ������������
#define  STOPPING_CAR1_VOICE           25      //����ͣ��
#define  STOPPING_CAR2_VOICE           26      //��Ҫͣ����
#define  CONTROL_SPEED1_VOICE          27      //����Ƴ���
#define  CONTROL_SPEED2_VOICE          28      //������
#define  MOBILE_RENT1_VOICE            29      //��ʹ���ֻ������⳵
#define  MOBILE_RENT2_VOICE            30      //��������ֻ��ͻ��ˣ������⳵������
#define  CAR_IN_MAINTAIN1_VOICE        31      //Ŀǰ��������ά��״̬���޷�ʹ��
#define  CAR_IN_MAINTAIN2_VOICE        32      //�Բ�����Ҫ������壬�޷�Ϊ������
#define  COLLISION1_VOICE              33      //��⵽������ײ
#define  COLLISION2_VOICE              34      //ײ���Һ�ʹ��
#define  STOP_STOP_VOICE               35      //ͣ��
#define  OVERTEMP_BATTERY1_VOICE       36      //����¶ȹ���
#define  OVERTEMP_BATTERY2_VOICE       37      //�ҵĵ�غ��Ȱ������ͣ��
#define  RENT_OVERTIME1_VOICE          38      //�ó����ó�ʱ���ѱ�������
#define  RENT_OVERTIME2_VOICE          39      //�ҵ���ȵ�̫�ã��ѻ����ˡ�
#define  STOP_CHARGE1_VOICE            40      //Ŀǰ��ֹͣ�Ʒ�
#define  STOP_CHARGE2_VOICE            41      //���ڲ����㹤Ǯ��
#define  CAR_TILT1_VOICE               42      //��̫ͣб�ˣ��޷�����
#define  CAR_TILT2_VOICE               43      //վ��վ���ȣ�û������



/************************************�궨�Լ���������*******************************/

#define    CVST                        44
#define  DIP_CAL_VOICE                 CVST+0      //���복����Ǳ궨ģʽ����ȷ�����Ѵ���ˮƽλ��
#define  DIP_CAL_SUCC_VOICE            CVST+1     //������Ǳ궨�ɹ�
#define  ACCEL_PEDAL_CAL_VOICE         CVST+2     //����̤��궨ģʽ
#define  ACCEL_PEDAL_BOTTOM_VOICE      CVST+3     //�뽫̤��ȵ���
#define  ACCEL_PEDAL_TOP_VOICE         CVST+4     //����ȫ�ɿ�̤��
#define  ACCEL_PEDAL_CAL_SUCC_VOICE    CVST+5     //����̤��궨�ɹ�
#define  BATTERY_CAL_VOICE             CVST+6     //���복����ѹ�����궨ģʽ
#define  BATTERY_CAL_SUCC_VOICE        CVST+7     //������ѹ�����궨�ɹ�
#define  WRITE_PARA_VOICE              CVST+8     //���ڽ��г���������ȡ
#define  READ_PARA_VOICE               CVST+9     //���ڽ��г�������д��
#define  UPDATE_SW_SUCC_VOICE          CVST+10    //������³ɹ�
#define  UPDATE_SW_FAILED_VOICE        CVST+11    //�������ʧ��
#define  START_UPDATE_SW_VOICE         CVST+12    //��������У������ο�����
#define  ENTER_SW_UPDATE_MODE_VOICE    CVST+13    //ע�⣬����������ģʽ
#define  ACCEL_SENSOE_VOICE            CVST+14      //���ٶȴ�����
#define  RESET_VOICE                   CVST+15      //��������
#define  BRAKE_START_VOICE             CVST+16      //�뽫̤��ȵ�ɲ����ʼ��
#define  BATTERY_CAL_FAILED_VOICE      CVST+17      //������ѹ�����궨ʧ��


/************************************���߼������*******************************/
#define    TVST                        CVST+18
#define  ENTER_TEST_MODE_VOICE         TVST+0     //�������ģʽ
#define  TEST_FAILED_VOICE             TVST+1      //����ʧ��
#define  TEST_SUCC_VOICE               TVST+2      //���Գɹ�
#define  CAL_FAILED_VOICE              TVST+3      //�궨ʧ��
#define  CAL_SUCC_VOICE                TVST+4      //�궨�ɹ�
#define  BATTERY_VOLT_VOICE            TVST+5      //��ص�ѹ
#define  BATTERY_CURR_VOICE            TVST+6      //��ص���
#define  BATTERY_TEMP_VOICE            TVST+7      //����¶�
#define  UART_FAILED_VOICE             TVST+8      //����

#define  NUMBER_0_VOICE                TVST+9      //��
#define  NUMBER_1_VOICE                TVST+10      //һ
#define  NUMBER_2_VOICE                TVST+11      //��
#define  NUMBER_3_VOICE                TVST+12      //��
#define  NUMBER_4_VOICE                TVST+13     //��
#define  NUMBER_5_VOICE                TVST+14      //��
#define  NUMBER_6_VOICE                TVST+15      //��
#define  NUMBER_7_VOICE                TVST+16      //��
#define  NUMBER_8_VOICE                TVST+17      //��
#define  NUMBER_9_VOICE                TVST+18      //��
#define  NUMBER_10_VOICE               TVST+19      //ʮ
#define  PIN_VOICE                     TVST+20      //�Źܽ�

#define  BT_TEST_VOICE                 TVST+21      //����
#define  CAN_TEST_VOICE                TVST+22      //can����
#define  GPRS_TEST_VOICE               TVST+23      //ͨѶģ��
#define  GPS_TEST_VOICE                TVST+24      //��λģ��
#define  ALL_TEST_SUCC_VOICE           TVST+25      //���ܲ�����ɣ�δ��������


#define  CAR_CHECK_ABNORM_VOICE        TVST+26      //�������ڹ���
#define  CAR_MOTOR_ABNORM_VOICE        TVST+27      //�������
#define  CAR_PEDAL_ABNORM_VOICE        TVST+28      //̤�����
#define  CAR_RADAR_ABNORM_VOICE        TVST+29      //�״����


/************************************ʵ�����Ա�������*******************************/

#define  DVST                          TVST+30
#define  SUDDEN_TURN_VOICE             DVST        //��ת��
#define  UP_SLOPE_VOICE                DVST+1      //������
#define  DOWN_SLOPE_VOICE              DVST+2      //������
#define  SPEED_UP_VOICE                DVST+3      //������
#define  SPEED_DOWN_VOICE              DVST+4      //��ɲ��

#define  CAR_MAINTAIN_VOICE            DVST+5      //ά��
#define  CAR_MOBILE_RENT_VOICE         DVST+6      //�ֻ��⳵
#define  CAR_COIN_RENT_VOICE           DVST+7      //Ͷ���⳵
#define  CAR_READY_VOICE               DVST+8      //����
#define  CAR_ORDER_VOICE               DVST+9      //Ԥ��
#define  STATE_VOICE                   DVST+10      //״̬
#define  BT_STARTUP_CAR_VOICE          DVST+11      //��������
#define  SUCC_VOICE                    DVST+12      //�ɹ�
#define  FAILED_VOICE                  DVST+13      //ʧ��

#define  CAL_ZERO_FAILED_VOICE         DVST+14      //����ʧ�� 
#define  CAL_ZERO_SUCC_VOICE           DVST+15      //����ɹ�
#define  CAL_ZERO_START_VOICE          DVST+16      //���㿪ʼ
#define  TEST_START_VOICE              DVST+17      //���Կ�ʼ
#define  FIRST_OP_VOICE                DVST+18      //��ӭʹ���ܲ�����������������ס̤��������ɿ���һ���λ�á�
                                                    //ͣ�����ס̤�壬������ȫֹͣ������ɿ�̤�塣�����밴���ֱ�
                                                    //����ɫ��ť��������ο�����ָ��.
#define  CAR_TILT3_VOICE               DVST+19      //��̫ͣб�ˣ��޷�����
#define  CAR_TILT4_VOICE               DVST+20      //վ��վ���ȣ�û������                                                                                                  
#define  EEPROM_WRITE_VOICE            DVST+21      //д��ɹ�
#define  REMAIN_TIME                   DVST+22      //����ʹ��ʱ�仹ʣһ����
#define  REMAIN_MILEAGE                DVST+23      //�����ܿ�100��
#define  MUSIC_VOICE                   DVST+24      //����
#define  TEMP_SENSEOR_FORGET_VOICE     DVST+25      //����¶ȴ�����δ��
#define  CAR_LOCK_VOICE                DVST+26      //�����ɹ�
#define  CAR_UNLOCK_VOICE              DVST+27      //�����ɹ�
#define  CAR_TILT5_VOICE               DVST+28      //��̫���ˣ���������ȥ
#define  CAR_FULL_STOP_VOICE           DVST+29      //���ڳ���ͣ�Ⱥ��ٻ���
#define  CAR_TIYAN_VOICE               DVST+30      //�뿿��ͣ��
#define  CAR_WANGQIAN_VOICE            DVST+31      //������ǰ���ˣ��ٿ��Ҿͻز�ȥ��!

#define  DEBUG_SERVER_VOICE            DVST+32      //�������Ӳ��Է�����
#define  RELEASE_SERVER_VOICE          DVST+33      //����������ʽ������
#define  EXTERN_EEPROM_VOICE           DVST+34      //�ⲿ�洢��
#define  TURN_LEFT_VOICE               DVST+35      //��ת
#define  TURN_RIGHT_VOICE              DVST+36      //��ת
#define  CAN_UPDATESW_VOICE            DVST+37      //���ÿ����߽��г������
#define  CAR_RETURN_VOICE              DVST+38      //�����ɹ�
#define  DONOT_FORGET_VOICE            DVST+39      //��������������Ʒ�ڳ���
#define  NO_SIMCARD_VOICE              DVST+40      //δ��⵽ͨ�ſ�
#define  PRODUCTION_MODE_VOICE         DVST+41      //��������ģʽ
#define  GPRS_UPDATESW_SUCC_VOICE      DVST+42      //Զ�̳�����³ɹ�
#define  GPRS_UPDATESW_FAIL_VOICE      DVST+43      //Զ�̳������ʧ��
#define  REBOOT_OLDAPP_VOICE           DVST+44      //�����ɰ汾Ӧ�ó���
#define  REMIND_RECHARGE_VOICE         DVST+45      //���ٴγ�ֵ
#define  TRIP_TEN_MINUTE_VOICE         DVST+46      //���������г̻�ʣ��ʮ����
#define  TRIP_FIVE_MINUTE_VOICE        DVST+47      //���������г̻�ʣ�������
#define  TRIP_REMAIN_VOICE             DVST+48      //���������г̻�ʣ��
#define  MINUTE_VOICE                  DVST+49      //����
#define  ACCOUNT_ZERO_VOICE            DVST+50      //�����˻��������Ϊ��
#define  START_DEDUCT_DEPOIST_VOICE    DVST+51      //��ʼ�۳��˻�Ѻ��
#define  RETURN_CAR_VOICE              DVST+52      //�뼰ʱ�黹������������
#define  CAR_STOP_TIME_VOICE           DVST+53      //���Ӻ�����ֹͣ����
#define  SPEED_LIMIT_VOICE             DVST+54      //��������ģʽ
#define  OVERTIME_DEDUCT_VOICE         DVST+55      //��ʱ���۳��˻�Ѻ��
#define  DEDUCT_DEPOIST_VOICE          DVST+56      //�۳�Ѻ��
#define  YUAN_VOICE                    DVST+57      //Ԫ
#define  TWENTY_VOICE                  DVST+58      //��ʮ
#define  THIRTY_VOICE                  DVST+59      //��ʮ
#define  FORTY_VOICE                   DVST+60      //��ʮ
#define  FIFTY_RECHARGE_VOICE          DVST+61      //��ʮ
#define  ONE_HUNDRED_VOICE             DVST+62      //һ��
#define  HUNDRED_VOICE                 DVST+63      //��
#define  THOUSAND_VOICE                DVST+64      //ǧ
#define  STOP_VOICE                    0xFE         //ֹͣ��������
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



