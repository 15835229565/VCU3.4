#include "includes.h"

//***************************************************************
//函数名：PWMEnable
//功能  ：使能单个PWM端口
//函数参数：1个 byte类型  channel代表PWM通道号
//返回值：无
//***************************************************************
void PWMEnable(byte channel)
{
    if(channel>7)  channel=7;
    PWME|=(1<<channel);
}
//***************************************************************
//函数名PWMDisable
//功能  ：禁止单个PWM端口
//函数参数：1个 byte类型  channel代表PWM通道号
//返回值：无
//***************************************************************
void PWMDisable(byte channel)
{
    if(channel>7) channel=7;
    PWME&=~(1<<channel);
}

//***************************************************************
//函数名：PWMGeneralInit
//功能  ：PWM端口通用寄存器配置
//函数参数：4个 byte类型
//参数1：   prclock[* pckb2 pckb1 pckb0 * pcka2 pcka1 pcka0]
//          代表时钟预分频：* pckb2 pckb1 pckb0
//                          * pcka2 pcka1 pcka0    clock A/B
//                              0     0     0      busclock
//                              0     0     1      busclock/2
//                              0     1     0      busclock/4
//                              0     1     1      busclock/8
//                              1     0     0      busclock/16
//                              1     0     1      busclock/32
//                              1     1     0      busclock/64
//                              1     1     1      busclock/128
//参数2：   scla 时钟SA分频参数
//          Clock SA=Clock A/(2*scla)
//
//参数3：   sclb 时钟SB分频参数
//          Clock SB=Clock B/(2*sclb)
//
//参数4：   control[con67 con45 con23 con01 pswai pfrz]
//          为PWM级联控制寄存器 con67 con45 con23 con01
//                               1 为两个通道级联
//                               0 为两个单独通道
//                              pswai
//                               1 等待模式停止时钟输入
//                               0 等待模式允许时钟输入
//                              pfrz
//                               1 冻结模式停止PWM时钟输入
//                               0 冻结模式允许PWM时钟输入
//返回值：无
//****************************************************************
void PWMGeneralInit(byte prclock,byte scla,byte sclb,byte control)
{
    //禁止所有PWM
    PWME=0x00;
    //设置预分频参数
    PWMPRCLK=prclock;
    //SA scale
    PWMSCLA=scla;
    //SB scale
    PWMSCLB=sclb;
    //级联配置
    PWMCTL=control;
    //对齐方式设置
}
//***************************************************************
//函数名：PWMSinglePortInit
//功能  ：PWM单独端口寄存器配置
//函数参数：5个 byte类型
//参数1：   channel 代表当前配置的PWM通道号
//
//参数2：   clkab 时钟选择参数
//参数3：   clock 时钟选择参数                   Port 0145   Port2367
//                               clkab   clock   CLK Select CLK Select
//                                 0       0      Clock A    Clock B
//                                 0       1      Clock SA   Clock SB
//                                 1       0      Clock B    Clock A
//                                 1       1      Clock SB   Clock SA
//
//参数4：   polarity为PWM极性
//                            0 输出起始低电平，周期计数开始时变高
//                            1 输出起始高电平，周期计数开始时变低
//
//参数5：   align为PWM对齐方式
//                            0 PWM输出左对齐
//                            1 PWM输出中心对齐
//返回值：无
//****************************************************************
void PWMSinglePortInit(byte channel,byte clkab,byte clock,byte polarity,byte align)
{
    if(channel>7) channel=7;
    //禁止该通道PWM
    PWMDisable(channel);

    //PWM时钟源 0  B/SB  1 A/SA
    if(clkab==0) PWMCLKAB&=~(1<<channel);
    else         PWMCLKAB|=1<<channel;
    //PWM时钟源 0为A/B   1为SA/SB
    if(clock==0) PWMCLK&=~(1<<channel);
    else         PWMCLK|=1<<channel;
    //对齐方式
    if(align==0) PWMCAE&=~(1<<channel);
    else         PWMCAE|=1<<channel;
    //极性
    if(polarity==0)  PWMPOL&=~(1<<channel);
    else             PWMPOL|=1<<channel;

}
//***************************************************************
//函数名：PWMSinglePortSetting
//功能  ：PWM单独端口输出参数配置
//函数参数：3个 byte类型
//参数1：   channel 代表当前配置的PWM通道号
//
//参数2：   period 周期配置参数
//          Left Mode:
//                    PWM Period=CLK Period * period
//          Center Mode:
//                    PWM Period=CLK Period * period * 2
//
//参数3：   duty   占空比配置参数
//          polarity=0起始低电平：
//                    Duty Cycle=(period-duty)/period*100%
//          polarity=1起始高电平
//                    Duty Cycle=duty/period*100%
//返回值：无
//****************************************************************
void PWMSinglePortSetting(byte channel,byte period,byte duty)
{
    if(channel>7) channel=7;
    //禁止该通道PWM
    PWMDisable(channel);
    switch(channel)
    {
    case 0:
        //设置周期寄存器
        PWMPER0=period;
        //设置占空比寄存器
        PWMDTY0=duty;
        break;
    case 1:
        //设置周期寄存器
        PWMPER1=period;
        //设置占空比寄存器
        PWMDTY1=duty;
        break;
    case 2:
        //设置周期寄存器
        PWMPER2=period;
        //设置占空比寄存器
        PWMDTY2=duty;
        break;
    case 3:
        //设置周期寄存器
        PWMPER3=period;
        //设置占空比寄存器
        PWMDTY3=duty;
        break;
    case 4:
        //设置周期寄存器
        PWMPER4=period;
        //设置占空比寄存器
        PWMDTY4=duty;
        break;
    case 5:
        //设置周期寄存器
        PWMPER5=period;
        //设置占空比寄存器
        PWMDTY5=duty;
        break;
    case 6:
        //设置周期寄存器
        PWMPER6=period;
        //设置占空比寄存器
        PWMDTY6=duty;
        break;
    case 7:
        //设置周期寄存器
        PWMPER7=period;
        //设置占空比寄存器
        PWMDTY7=duty;
        break;
    default:
        break;
    }
    PWMEnable(channel);
}


//***************************************************************
//函数名：PWMConcatenatePortSetting
//功能  ：PWM级联端口输出参数配置
//函数参数：3个 1个byte类型 2个word类型
//参数1：   channel 代表当前配置的PWM通道号
//
//参数2：   period 周期配置参数
//          Left Mode:
//                    PWM Period=CLK Period * period
//          Center Mode:
//                    PWM Period=CLK Period * period * 2
//
//参数3：   duty   占空比配置参数
//          polarity=0起始低电平：
//                    Duty Cycle=(period-duty)/period*100%
//          polarity=1起始高电平
//                    Duty Cycle=duty/period*100%
//返回值：无
//****************************************************************
void PWMConcatenatePortSetting(byte channel,word period,word duty)
{
    if(channel>7) channel=7;
    switch(channel)
    {
    case 0:
    case 1:
        PWMDisable(0); //禁止该通道PWM
        PWMDisable(1); //禁止该通道PWM
        //设置周期寄存器
        PWMPER01=period;
        //设置占空比寄存器
        PWMDTY01=duty;
        PWMEnable(0);
        PWMEnable(1);
        break;
    case 2:
    case 3:
        PWMDisable(2); //禁止该通道PWM
        PWMDisable(3); //禁止该通道PWM
        //设置周期寄存器
        PWMPER23=period;
        //设置占空比寄存器
        PWMDTY23=duty;
        PWMEnable(2);
        PWMEnable(3);
        break;
    case 4:
    case 5:
        PWMDisable(4); //禁止该通道PWM
        PWMDisable(5); //禁止该通道PWM
        //设置周期寄存器
        PWMPER45=period;
        //设置占空比寄存器
        PWMDTY45=duty;
        PWMEnable(4);
        PWMEnable(5);
        break;
    case 6:
    case 7:
        PWMDisable(6); //禁止该通道PWM
        PWMDisable(7); //禁止该通道PWM
        //设置周期寄存器
        PWMPER67=period;
        //设置占空比寄存器
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
* Description : PWM波寄存器设置
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
* Description : PWM波占空比设置
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

