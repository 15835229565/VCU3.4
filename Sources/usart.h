void SCI_Init(void);
void SCI_BR0(uint32 br);
void SCI_BR1(uint32 br);
void SCI_BR2(uint32 br);
void SendByte0(uint8 ch);
void SendByte1(uint8 ch);
void SendByte2(uint8 ch);
void SendBytes(uint8* ch_p,uint8 c_length);
void SendBytesGPRS(uint8* ch_p,uint8 c_length);
void SendBytesToServerByGPRS(uint8* ch_p,uint8 c_length);
void SendBytesToPhoneByBT(uint8* ch_p,uint8 c_length);

void UART1_RxInterruptDisable();
void UART2_RxInterruptDisable();

void UART1_RxInterruptEnable();
void UART2_RxInterruptEnable();
void UART0_SendString(uint8 *text);
void UART1_SendString(uint8 *text);
void UART2_SendString(uint8 *text);
uint8 UART2_GetChar(uint8* data,uint16 timeout);
uint16 UART2_GetData(uint8* buffer,uint16 timeout,uint16 limit);







