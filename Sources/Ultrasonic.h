#ifndef    _ULTRA_H_
#define   _ULTRA_H_

#define START_SPEED 180
#define FAST_SPEED 1
#define WARING_DISTANCE 300
#define WARING1_DISTANCE 450
#define STOPPING_DISTANCE 150
#define WURENSTOPPING_DISTANCE 50

extern uint8  gFrontBarriArea;
extern uint8  gRearBarriArea;
//extern uint16 Anticollisiondata0,Anticollisiondata1,Anticollisiondata2,Anticollisiondata3;
 void EffectiveObstaclesDataCompute(uint8 *data); 
void RadarAnticollision(uint8 *data);
void FrontAnticollosionManage(uint8 *data);
uint16  RadarAbsChangeRateCompute(uint16* data,uint8 datalen,uint8 startindex);
#endif
