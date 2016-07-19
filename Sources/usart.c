#include  "includes.h"

//���ڳ�ʼ��
void SCI_Init()
{
    SCI0CR1 = 0x00;  /* 8 Data Bits, 1 Start Bit, 1 Stop Bit, No Parity */
    SCI0CR2 = 0x2C;  /* ʹ�ܽ����ж�;ʹ�� Tx,Rx */

    SCI1CR1 = 0x00;  /* 8 Data Bits, 1 Start Bit, 1 Stop Bit, No Parity */
    SCI1CR2 = 0x2C;  /* ʹ�ܽ����ж�;ʹ�� Tx,Rx */

    
    SCI2CR1 = 0x00;  /* 8 Data Bits, 1 Start Bit, 1 Stop Bit, No Parity */
    SCI2CR2 = 0x2C;  /* ʹ�ܽ����ж�;ʹ�� Tx,Rx */
    PERM_PERM2 = 1;  //��������ܽ�����
 
    /* SCIASR1, SCIACR1, SCIACR2, SCISR1, SCISR2, SCIDRH & SCIDRL left at default values */
}
/*
*********************************************************************************
* �������� UART1_RxInterruptDisable
* ˵  ��������1�жϽ�ֹ
* ��  ������
* ����ֵ��
* ��  ����
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
* �������� UART1_RxInterruptEnable
* ˵  ��������1�ж�ʹ��
* ��  ������
* ����ֵ��
* ��  ����
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

//���ڲ���������
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

//���ڷ����ֽ�
void SendByte0(uint8 ch)    
{
    /* check SCI transmit data register is empty */
    while(SCI0SR1_TDRE == 0);
    SCI0DRL = ch;
}
//���ڷ����ֽ�
void SendByte1(uint8 ch)
{
    /* check SCI transmit data register is empty */
    while(SCI1SR1_TDRE == 0);
    SCI1DRL = ch;
    
    //����ģʽ��       
    #if DEBUG_MODE==1
    SendByte0(ch);
    #endif
}
void SendByte2(uint8 ch)
{
    /* check SCI transmit data register is empty */
    while(SCI2SR1_TDRE == 0);
    SCI2DRL = ch;
    //����ģʽ��       
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

//�����ݽ���ת������ͨ���������͵��ֻ�
void SendBytesToPhoneByBT(uint8* ch_p,uint8 c_length)
{
    uint16 i,j;
   
    //���Ͱ�ͷ
    SendByte2(CMD_START);
    for(i=0; i<c_length; i++)
    {                                     
        //����ת���ַ�
        if((ch_p[i]==CMD_START) || (ch_p[i]==CMD_END) || (ch_p[i]==CMD_ALIAS)) 
        {          
          SendByte2(CMD_ALIAS);          
        }
        SendByte2(ch_p[i]);
    }
    //���Ͱ�β
    SendByte2(CMD_END);
}

//�����ݽ���ת������ͨ��GPRS���͵���������
void SendBytesToServerByGPRS(uint8* ch_p,uint8 c_length)
{
    uint16 i,j;
   
    //���Ͱ�ͷ
    SendByte1(CMD_START);
    for(i=0; i<c_length; i++)
    {                                     
        //����ת���ַ�
        if((ch_p[i]==CMD_START) || (ch_p[i]==CMD_END) || (ch_p[i]==CMD_ALIAS)) 
        {          
          SendByte1(CMD_ALIAS);          
        }
        SendByte1(ch_p[i]);
    }
    //���Ͱ�β
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

