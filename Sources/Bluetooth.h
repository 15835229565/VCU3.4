/*
******************************************************************
*
* 版权所有： 杭州锐行汽车电子技术有限公司
*
******************************************************************
*/

#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

extern uint8  BTnameSetFlag;

uint8 BTSendCommand(uint8* command,uint8*response);
uint8 BT_AT_SET_NAME(uint8* name);
void  BT_Reset();
uint8 BT_AT_SET_MODE();
uint8 HexToAsc(uint8 data); 
uint8 PhoneBTCmdProcess(uint8* phonecmd);

uint8 PhoneBTToVcuCmd29(uint8 *phonecmd);
uint8 VcuBTToPhoneCmd29();
uint8 PhoneBTToVcuCmd30(uint8 *phonecmd);
uint8 VcuBTToPhoneCmd30(uint8 veryflag);
uint8 PhoneBTToVcuCmd31(uint8 *phonecmd);
uint8 VcuBTToPhoneCmd31(uint8 *phonecmd);
uint8 PhoneBTToVcuCmd32(uint8 *phonecmd);
uint8 VcuBTToPhoneCmd32();
uint8 PhoneBTToVcuCmd33(uint8 *phonecmd);
uint8 VcuBTToPhoneCmd33(); 
#endif