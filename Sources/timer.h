#ifndef _TIME_H_
#define _TIME_H_
// º¯ÊýÉùÃ÷
void OutputCompare_Init(void);

void RTI_Init(void);
void Init_PT_ICapture(void);
void SpeedSensorCaptureInit();
void TimeoutInit(uint32* timestamp);
uint32 GetTime();
uint8 TimeoutValidate(uint32 timestamp,uint32 timeout) ;
void delayms(uint32 time) ;
void delayus(uint32 time) ;
#endif