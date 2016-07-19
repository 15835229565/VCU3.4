/*
******************************************************************
*
*
*
******************************************************************
*/
#ifndef _CAN_H_
#define _CAN_H_
#define FRAME_STD_DATA      0x01
#define FRAME_STD_REMOTE    0x02
#define FRAME_EXT_DATA      0x03
#define FRAME_EXT_REMOTE    0x04
#define CAN_BAUDRATE_500K   1
#define CAN_BAUDRATE_250K   2

typedef struct can_frame
{
    uint32 id ;
    uint8 type ;
    uint8 datalen ;
    uint8 data[8] ;
} CAN_frame;

extern CAN_frame RxMsg;
extern CAN_frame TxMsg;
extern uint8 VCUWorkMode;
extern CAN_frame CarCanFrame;
extern CAN_frame CarParaFromPC;
void Can_Init(uint8 baudrate);
static void MSCAN0_ConfigPort(void);
void CarParaFromPCProc();
uint8 Can_Send(CAN_frame canframe);
void CAN_receive(void);

void SetTestResult(uint8 pinno);
void ClearTestResult(uint8 pinno);
uint8 ReadTestResult(uint8 pinno);
 void ClearNCTestResult();
void CommModuleTest(); 
#endif