// 函数声明
#ifndef    _ADC_H_
#define   _ADC_H_
#define    ADCPRECISION_08        (0x1F)  // 8位AD采集
#define    ADCPRECISION_10         (0x30)   // 10位AD采集
#define    ADCollectMode_IPT         (0x1F)   // AD 中断
#define    ADCollectMode_Con         (0x00)   // 连续采集
#define    ADCollect_ClockCycles        (0xE1)   // f_clk=fBUS/(2*(PRS+1))
typedef struct ADC_Tile                           //ADC输出控制结构体
{
    uint8                   ADCPrecision;       //AD精度设置
    uint8                   ADCollectMode;   // AD采样模式设置
    uint8                   ADClockCycles;      //AD采样频率设置
} ADC_Type,*ADC_Ref;
void ADC_Init(ADC_Ref ADC_Ptr);                             //AD 初始化函数
void ADC_IPT_Enable() ;
void ADC_IPT_DisEnable() ;
uint16 ADC_GetValue(byte ch);
uint16 SignalFilter(uint16* signaldata,uint8 length);
void BatteryDataProc();
void SlopeAngleCalc();
void VibrateLevelCalc();
void SteeringAngleCalc();
void PedalPosCalc();
#endif

