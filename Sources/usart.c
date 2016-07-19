#include  "includes.h"

//串口初始化
void SCI_Init()
{
    SCI0CR1 = 0x00;  /* 8 Data Bits, 1 Start Bit, 1 Stop Bit, No Parity */
    SCI0CR2 = 0x2C;  /* 使能接收中断;使能 Tx,Rx */

    SCI1CR1 = 0x00;  /* 8 Data Bits, 1 Start Bit, 1 Stop Bit, No Parity */
    SCI1CR2 = 0x2C;  /* 使能接收中断;使能 Tx,Rx */

    
    SCI2CR1 = 0x00;  /* 8 Data Bits, 1 Start Bit, 1 Stop Bit, No Parity */
    SCI2CR2 = 0x2C;  /* 使能接收中断;使能 Tx,Rx */
    PERM_PERM2 = 1;  //串口输入管脚上拉
 
    /* SCIASR1, SCIACR1, SCIACR2, SCISR1, SCISR2, SCIDRH & SCIDRL left at default values */
}
/*
*********************************************************************************
* 函数名： UART1_RxInterruptDisable
* 说  明：串口1中断禁止
* 参  数：无
* 返回值：
* 其  他：
*********************************************************************************
*/
void UART1_RxInterruptDisable()
{
    SCI1CR2_RIE =0;
}

void UART2_RxInterruptDisable()
{
    SCI2CR2_RIE =0;
}

void UART1_SendString(uint8 *text )
{
    while (*text  !=  '\0')
    {
        SendByte1 ( *text++);
    }
}
void UART2_SendString(uint8 *text )
{
    while (*text  !=  '\0')
    {
        SendByte2 ( *text++);
    }
}

void UART0_SendString(uint8 *text )
{
    while (*text  !=  '\0')
    {
        SendByte0 ( *text++);
    }
    SendByte0('\r');
    SendByte0('\n');
}

uint8 UART2_GetChar(uint8* data,uint16 timeout)
{
    uint32 timestamp,i=0;
    
    TimeoutInit(&timestamp);
    
    while(TRUE)
    {
        if (SCI2SR1_RDRF ==1)
        {
            *data=SCI2DRL;
            SCI2SR1_RDRF= 0;
           
            return TRUE;
        }
        if (TimeoutValidate(timestamp,timeout) == TRUE)
        {
            return FALSE;
        }
        
    }
}

uint16 UART2_GetData(uint8* buffer,uint16 timeout,uint16 limit)
{
    uint16 index= 0;
    while(UART2_GetChar(&buffer[index],timeout) ==TRUE && index<=limit)
    {
        index++;
        FEEDWATCHDOG = ~FEEDWATCHDOG;
    }
    return index;
}
/*
*********************************************************************************
* 函数名： UART1_RxInterruptEnable
* 说  明：串口1中断使能
* 参  数：无
* 返回值：
* 其  他：
*********************************************************************************
*/
void UART1_RxInterruptEnable()
{
    SCI1CR2_RIE = 1;
}
void UART2_RxInterruptEnable()
{
    SCI2CR2_RIE = 1;
}

//串口波特率设置
void SCI_BR0(uint32 br)
{
    uint16 brPrescaler;
    brPrescaler = (uint16)(24000000 / (16 * br));

    /* Set the Baud Rate */
    SCI0BDH = (uint8)((brPrescaler>>8));
    SCI0BDL = (uint8)(brPrescaler);
}

void SCI_BR1(uint32 br)
{
    uint16 brPrescaler;
    brPrescaler = (uint16)(24000000 / (16 * br));

    /* Set the Baud Rate */
    SCI1BDH = (uint8)((brPrescaler>>8));
    SCI1BDL = (uint8)(brPrescaler);
}

void SCI_BR2(uint32 br)
{
    uint16 brPrescaler;
    brPrescaler = (uint16)(24000000 / (16 * br));

    /* Set the Baud Rate */
    SCI2BDH = (uint8)((brPrescaler>>8));
    SCI2BDL = (uint8)(brPrescaler);
}

//串口发送字节
void SendByte0(uint8 ch)    
{
    /* check SCI transmit data register is empty */
    while(SCI0SR1_TDRE == 0);
    SCI0DRL = ch;
}
//串口发送字节
void SendByte1(uint8 ch)
{
    /* check SCI transmit data register is empty */
    while(SCI1SR1_TDRE == 0);
    SCI1DRL = ch;
    
    //调试模式用       
    #if DEBUG_MODE==1
    SendByte0(ch);
    #endif
}
void SendByte2(uint8 ch)
{
    /* check SCI transmit data register is empty */
    while(SCI2SR1_TDRE == 0);
    SCI2DRL = ch;
    //调试模式用       
    #if DEBUG_MODE==1 
    //SendByte0(ch);
    #endif
}

void SendBytes(uint8* ch_p,uint8 c_length)
{
    uint16 i;
    for(i=0; i<c_length; i++)
    {
        while (SCI0SR1_TDRE == 0); // Interrupt Flag 2 & UART1 Interrupt Flag Register
        SCI0DRL = (ch_p[i]);            // USART 1 Transmit Buffer
    }
}

//将数据进行转义打包后通过蓝牙发送到手机
void SendBytesToPhoneByBT(uint8* ch_p,uint8 c_length)
{
    uint16 i,j;
   
    //发送包头
    SendByte2(CMD_START);
    for(i=0; i<c_length; i++)
    {                                     
        //加入转义字符
        if((ch_p[i]==CMD_START) || (ch_p[i]==CMD_END) || (ch_p[i]==CMD_ALIAS)) 
        {          
          SendByte2(CMD_ALIAS);          
        }
        SendByte2(ch_p[i]);
    }
    //发送包尾
    SendByte2(CMD_END);
}

//将数据进行转义打包后通过GPRS发送到服务器端
void SendBytesToServerByGPRS(uint8* ch_p,uint8 c_length)
{
    uint16 i,j;
   
    //发送包头
    SendByte1(CMD_START);
    for(i=0; i<c_length; i++)
    {                                     
        //加入转义字符
        if((ch_p[i]==CMD_START) || (ch_p[i]==CMD_END) || (ch_p[i]==CMD_ALIAS)) 
        {          
          SendByte1(CMD_ALIAS);          
        }
        SendByte1(ch_p[i]);
    }
    //发送包尾
    SendByte1(CMD_END);
}

void SendBytesGPRS(uint8* ch_p,uint8 c_length)
{
    uint16 i;
        
    for(i=0; i<c_length; i++)
    {       
        SendByte1(ch_p[i]);
    }
}

