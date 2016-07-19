
extern uint16 deststep[100];
extern uint16 basicDuty[100]; 
extern uint8 pedalAreaBoundPer[5]; 

extern uint16 Duty;
extern uint8 Rollover_flag;                       //≤‡∑≠±Í÷æ

uint8  getDestMotorSpeed(uint8 maxspeed);
uint16 getDestDuty(uint8 maxspeed);

void CarStartingMode();
void CarRunningMode(void);
void CarStopMode();
void CarRunningCtrl();
void CalibraMaxpedal();
uint16 MotorCtrlPID();
void MotorCtrlCurveCalc(uint8 maxspeed);
void MotorInit();
void MotorElecMagnetControl();
void BrakePerforCheck();