
uint8  Init_GPRS(void);


#define GPRS_RESTART    0
#define GPRS_POWEROFF   1
#define GPRS_POWERON    2
#define GPRS_INITIAL    3
#define GPRS_CONNECTTING 4
#define GPRS_CONNECTED    5

#define GPS_RESTART    0
#define GPS_SUCC       1



extern uint8 GPRS_ERROR;

uint8 AT_Contains(uint8 *str,uint8 *find);
uint8 AT_Transmit(uint8* command);
uint8 CheckATResp(uint8*response,uint16 waittime);//���ڲ�ͬ��ATָ�ģ�鷵��ʱ�䲻ͬ��������������������в�ָͬ�ͬ����ʱ
void EmptyBuffer(void);

uint8 GPRS_Open_UDP_OR_TCP_Connection(void);
uint8 GPRS_Enable_Send_Data_By_UDP_OR_TCP(uint8 len);

uint8 GPRS_Get_SIM_ICCID(uint8*iccid,uint8 len);
uint8 GPRS_SERVICE_ATTACH(void);

void MG301_PowerOn();

uint8 GPRS_CREG(void);
uint8 GPRS_GET_RSSI(void);
uint8 GPRS_IOMODE(void);
uint8 GPRS_MODULE_POWEROFF(void);
uint8 GPRS_DVCFG(void);



