// ��������
#ifndef    _ADC_H_
#define   _ADC_H_
#define    ADCPRECISION_08        (0x1F)  // 8λAD�ɼ�
#define    ADCPRECISION_10         (0x30)   // 10λAD�ɼ�
#define    ADCollectMode_IPT         (0x1F)   // AD �ж�
#define    ADCollectMode_Con         (0x00)   // �����ɼ�
#define    ADCollect_ClockCycles        (0xE1)   // f_clk=fBUS/(2*(PRS+1))
typedef struct ADC_Tile                           //ADC������ƽṹ��
{
    uint8                   ADCPrecision;       //AD��������
    uint8                   ADCollectMode;   // AD����ģʽ����
    uint8                   ADClockCycles;      //AD����Ƶ������
} ADC_Type,*ADC_Ref;
void ADC_Init(ADC_Ref ADC_Ptr);                             //AD ��ʼ������
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

