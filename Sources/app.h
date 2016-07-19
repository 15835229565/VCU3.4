#ifndef _APP_H_
#define _APP_H_

extern uint8 GPRS_init_flag;
extern uint8 GPS_init_flag;

void GPRSConnecting();
void CoinRentingProcess();
void PhoneBTRentingProcess();
void PhoneGPRSRentingProcess();
void GPSFenceProcess();
void ExceptionProcess();  
void SendCarDataByCan();

#endif