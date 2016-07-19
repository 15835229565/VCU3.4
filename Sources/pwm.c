#include "includes.h"

//***************************************************************
//��������PWMEnable
//����  ��ʹ�ܵ���PWM�˿�
//����������1�� byte����  channel����PWMͨ����
//����ֵ����
//***************************************************************
void PWMEnable(byte channel)
{
    if(channel>7)  channel=7;
    PWME|=(1<<channel);
}
//***************************************************************
//�������PPWMDisable
//����  ����ֹ����PWM�˿�
//����������1�� byte����  channel����PWMͨ����
//����ֵ����
//***************************************************************
void PWMDisable(byte channel)
{
    if(channel>7) channel=7;
    PWME&=~(1<<channel);
}

//***************************************************************
//��������PWMGeneralInit
//����  ��PWM�˿�ͨ�üĴ�������
//����������4�� byte����
//����1��   prclock[* pckb2 pckb1 pckb0 * pcka2 pcka1 pcka0]
//          ����ʱ��Ԥ��Ƶ��* pckb2 pckb1 pckb0
//                          * pcka2 pcka1 pcka0    clock A/B
//                              0     0     0      busclock
//                              0     0     1      busclock/2
//                              0     1     0      busclock/4
//                              0     1     1      busclock/8
//                              1     0     0      busclock/16
//                              1     0     1      busclock/32
//                              1     1     0      busclock/64
//                              1     1     1      busclock/128
//����2��   scla ʱ��SA��Ƶ����
//          Clock SA=Clock A/(2*scla)
//
//����3��   sclb ʱ��SB��Ƶ����
//          Clock SB=Clock B/(2*sclb)
//
//����4��   control[con67 con45 con23 con01 pswai pfrz]
//          ΪPWM�������ƼĴ��� con67 con45 con23 con01
//                               1 Ϊ����ͨ������
//                               0 Ϊ��������ͨ��
//                              pswai
//                               1 �ȴ�ģʽֹͣʱ������
//                               0 �ȴ�ģʽ����ʱ������
//                              pfrz
//                               1 ����ģʽֹͣPWMʱ������
//                               0 ����ģʽ����PWMʱ������
//����ֵ����
//****************************************************************
void PWMGeneralInit(byte prclock,byte scla,byte sclb,byte control)
{
    //��ֹ����PWM
    PWME=0x00;
    //����Ԥ��Ƶ����
    PWMPRCLK=prclock;
    //SA scale
    PWMSCLA=scla;
    //SB scale
    PWMSCLB=sclb;
    //��������
    PWMCTL=control;
    //���뷽ʽ����
}
//***************************************************************
//��������PWMSinglePortInit
//����  ��PWM�����˿ڼĴ�������
//����������5�� byte����
//����1��   channel ����ǰ���õ�PWMͨ����
//
//����2��   clkab ʱ��ѡ�����
//����3��   clock ʱ��ѡ�����                   Port 0145   Port2367
//                               clkab   clock   CLK Select CLK Select
//                                 0       0      Clock A    Clock B
//                                 0       1      Clock SA   Clock SB
//                                 1       0      Clock B    Clock A
//                                 1       1      Clock SB   Clock SA
//
//����4��   polarityΪPWM����
//                            0 �����ʼ�͵�ƽ�����ڼ�����ʼʱ���
//                            1 �����ʼ�ߵ�ƽ�����ڼ�����ʼʱ���
//
//����5��   alignΪPWM���뷽ʽ
//                            0 PWM��������
//                            1 PWM������Ķ���
//����ֵ����
//****************************************************************
void PWMSinglePortInit(byte channel,byte clkab,byte clock,byte polarity,byte align)
{
    if(channel>7) channel=7;
    //��ֹ��ͨ��PWM
    PWMDisable(channel);

    //PWMʱ��Դ 0  B/SB  1 A/SA
    if(clkab==0) PWMCLKAB&=~(1<<channel);
    else         PWMCLKAB|=1<<channel;
    //PWMʱ��Դ 0ΪA/B   1ΪSA/SB
    if(clock==0) PWMCLK&=~(1<<channel);
    else         PWMCLK|=1<<channel;
    //���뷽ʽ
    if(align==0) PWMCAE&=~(1<<channel);
    else         PWMCAE|=1<<channel;
    //����
    if(polarity==0)  PWMPOL&=~(1<<channel);
    else             PWMPOL|=1<<channel;

}
//***************************************************************
//��������PWMSinglePortSetting
//����  ��PWM�����˿������������
//����������3�� byte����
//����1��   channel ����ǰ���õ�PWMͨ����
//
//����2��   period �������ò���
//          Left Mode:
//                    PWM Period=CLK Period * period
//          Center Mode:
//                    PWM Period=CLK Period * period * 2
//
//����3��   duty   ռ�ձ����ò���
//          polarity=0��ʼ�͵�ƽ��
//                    Duty Cycle=(period-duty)/period*100%
//          polarity=1��ʼ�ߵ�ƽ
//                    Duty Cycle=duty/period*100%
//����ֵ����
//****************************************************************
void PWMSinglePortSetting(byte channel,byte period,byte duty)
{
    if(channel>7) channel=7;
    //��ֹ��ͨ��PWM
    PWMDisable(channel);
    switch(channel)
    {
    case 0:
        //�������ڼĴ���
        PWMPER0=period;
        //����ռ�ձȼĴ���
        PWMDTY0=duty;
        break;
    case 1:
        //�������ڼĴ���
        PWMPER1=period;
        //����ռ�ձȼĴ���
        PWMDTY1=duty;
        break;
    case 2:
        //�������ڼĴ���
        PWMPER2=period;
        //����ռ�ձȼĴ���
        PWMDTY2=duty;
        break;
    case 3:
        //�������ڼĴ���
        PWMPER3=period;
        //����ռ�ձȼĴ���
        PWMDTY3=duty;
        break;
    case 4:
        //�������ڼĴ���
        PWMPER4=period;
        //����ռ�ձȼĴ���
        PWMDTY4=duty;
        break;
    case 5:
        //�������ڼĴ���
        PWMPER5=period;
        //����ռ�ձȼĴ���
        PWMDTY5=duty;
        break;
    case 6:
        //�������ڼĴ���
        PWMPER6=period;
        //����ռ�ձȼĴ���
        PWMDTY6=duty;
        break;
    case 7:
        //�������ڼĴ���
        PWMPER7=period;
        //����ռ�ձȼĴ���
        PWMDTY7=duty;
        break;
    default:
        break;
    }
    PWMEnable(channel);
}


//***************************************************************
//��������PWMConcatenatePortSetting
//����  ��PWM�����˿������������
//����������3�� 1��byte���� 2��word����
//����1��   channel ����ǰ���õ�PWMͨ����
//
//����2��   period �������ò���
//          Left Mode:
//                    PWM Period=CLK Period * period
//          Center Mode:
//                    PWM Period=CLK Period * period * 2
//
//����3��   duty   ռ�ձ����ò���
//          polarity=0��ʼ�͵�ƽ��
//                    Duty Cycle=(period-duty)/period*100%
//          polarity=1��ʼ�ߵ�ƽ
//                    Duty Cycle=duty/period*100%
//����ֵ����
//****************************************************************
void PWMConcatenatePortSetting(byte channel,word period,word duty)
{
    if(channel>7) channel=7;
    switch(channel)
    {
    case 0:
    case 1:
        PWMDisable(0); //��ֹ��ͨ��PWM
        PWMDisable(1); //��ֹ��ͨ��PWM
        //�������ڼĴ���
        PWMPER01=period;
        //����ռ�ձȼĴ���
        PWMDTY01=duty;
        PWMEnable(0);
        PWMEnable(1);
        break;
    case 2:
    case 3:
        PWMDisable(2); //��ֹ��ͨ��PWM
        PWMDisable(3); //��ֹ��ͨ��PWM
        //�������ڼĴ���
        PWMPER23=period;
        //����ռ�ձȼĴ���
        PWMDTY23=duty;
        PWMEnable(2);
        PWMEnable(3);
        break;
    case 4:
    case 5:
        PWMDisable(4); //��ֹ��ͨ��PWM
        PWMDisable(5); //��ֹ��ͨ��PWM
        //�������ڼĴ���
        PWMPER45=period;
        //����ռ�ձȼĴ���
        PWMDTY45=duty;
        PWMEnable(4);
        PWMEnable(5);
        break;
    case 6:
    case 7:
        PWMDisable(6); //��ֹ��ͨ��PWM
        PWMDisable(7); //��ֹ��ͨ��PWM
        //�������ڼĴ���
        PWMPER67=period;
        //����ռ�ձȼĴ���
        PWMDTY67=duty;
        PWMEnable(6);
        PWMEnable(7);
        break;
    default:
        break;
    }
}
/*
*********************************************************************************************************
*                                                pwm_regesiterset()
*
* Description : PWM���Ĵ�������
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/
void pwm_regesiterset(word duty)
{                                                 
    PWME&=~(1<<7);
    PWMPER67 = 500;
    PWMDTY67 = duty;
    PWME|=(1<<7);
}

void epspwm_regesiterset(word epsduty)
{
    PWME&=~(1<<5);
    PWMPER45 = 500;
    PWMDTY45 = epsduty;
    PWME|=(1<<5);
}

/*
*********************************************************************************************************
*                                                MotorPWMCalc()
*
* Description : PWM��ռ�ձ�����
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

