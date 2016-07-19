
/*
******************************************************************
*
* ��Ȩ���У� ���������������Ӽ������޹�˾
*
******************************************************************
*/
#include  "includes.h"


uint8 CmdRxBuffer[64];
uint32 carIdFromPhone=0,userIdFromPhone=0;            //��¼�����ֻ���userid��carid
uint8  phoneVerifFlag=0;
uint8  BTnameSetFlag=0;

#if DEBUG_MODE==0

void BT_Reset() 
{  
  BT_RESET = 1;
  delayms(300);
  BT_RESET = 0;
}



uint8 BT_AT_SET_NAME(uint8* name) 
{
    uint8 i,namelength=0;
    //���������豸����
    uint8 bldata[64];    
    memcpy(bldata,"AT+QA",5);
    
    for(i=0;i<16;i++) 
    {
        if(name[i]!=0)
        {          
            bldata[i+7]=name[i];
            namelength++;
        } 
        else 
        {
          break;
        }
    }
    bldata[5] = HexToAsc((namelength/10));
    bldata[6] = HexToAsc((namelength%10));
    bldata[i+7]='\r';
    bldata[i+8]='\n';
    bldata[i+9]='\0';   
      
    delayms(300);      
    if (BTSendCommand(bldata,"QA") != 0)
    {     
        BT_Reset();   
        return 1;
    }  
    
    return 0;
}

uint8 HexToAsc(uint8 data) 
{  
    if(data>=0&&data<=9) 
    {
      return data+'0';
    } 
    else if(data>=10&&data<=15)  
    {
      return ((data-10)+'A');
    } 
    else 
    {
      return 0;
    }
    
}


static uint8 strcontains(uint8 *str,uint8 *find)
{
    uint8 * start = find;        
    while ( *str !='\0') 
    {
		    if (*str == *find)
		    {
		        find++;
		    }
		    else
		    {
		        find = start; 
		    }
		    
		    if (*(find + 1) == '\0')
		    {
		        return 1;
		    }
		    str++;
    }	
    return 0;
}
      



uint8 BT_AT_SET_MODE() 
{
    uint8 i,namelength=0;
    //���������豸����
    uint8 bldata[16];    
    memcpy(bldata,"AT+QH040680",11);
    
    bldata[11]='\r';
    bldata[12]='\n';
    bldata[13]='\0';   
              
    if (BTSendCommand(bldata,"QH") != 0)
    {              
        return 1;
    }  
    
    return 0;
}





uint8 PhoneBTCmdProcess(uint8* phonecmd) 
{
    
    switch(phonecmd[0])
    {
      
      case 0x29://��ȡ��֤������
          if(PhoneBTToVcuCmd29(phonecmd))
          {
              VcuBTToPhoneCmd29();
              phoneVerifFlag = 1;
              #if DEBUG_MODE == 1
              UART0_SendString("������֤"); 
              #endif    
              
              return 0xA9;
          }
          break;
      case 0x30: //������������
          if(PhoneBTToVcuCmd30(phonecmd) && phoneVerifFlag == 1)
          {
              VcuBTToPhoneCmd30(1);
              phoneVerifFlag = 2;
              gCarInfor.BTTimeout=0;
              #if DEBUG_MODE == 1
              UART0_SendString("�������ӿ�ʼ"); 
              #endif 
              
              return 0xB2;
          } 
          else
          {
              
              VcuBTToPhoneCmd30(0);
              phoneVerifFlag=0;
              return FALSE;
          }
          break;
      
      case 0x31://����ά������
          if(PhoneBTToVcuCmd31(phonecmd)&&phoneVerifFlag==2)
          {
              
              VcuBTToPhoneCmd31(phonecmd);
              if(GPRS_init_flag==GPRS_CONNECTED) 
              {                    
                VcuConnectServerProcess(0x0B);
              }               
              SpeedSensorCaptureInit();
              
              gCarInfor.Event |= BT_RENT_START_EVENT;
              
              #if DEBUG_MODE == 1
              UART0_SendString("������ʼά��"); 
              #endif             
              return 0xB1;
          }
          break;
      case 0x32: //��������ά������
          if(PhoneBTToVcuCmd32(phonecmd)&&phoneVerifFlag==2)
          {
              
             
              VcuBTToPhoneCmd32();
              phoneVerifFlag = 0;
              if(GPRS_init_flag==GPRS_CONNECTED) 
              {                    
                VcuConnectServerProcess(0x14);
              } 
              #if DEBUG_MODE == 1
              UART0_SendString("��������ά��");
              #endif
              return 0xB2;
          }
          break;
           
      case 0x33: //��ȡ�����ظ�����
          if(PhoneBTToVcuCmd33(phonecmd)&&phoneVerifFlag==2) 
          {            
            VcuBTToPhoneCmd33();
            return 0xB3;
          }
          break;      
           
      default:
          break; 
    }
    return FALSE;    
}


/*
*********************************************************************************
* ��������PhoneBTToVcuCmd29
* ˵  �����ֻ���VCU���ͻ�ȡ��֤������
* ��  ����uint8 *phonecmd �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
*********************************************************************************
*/

uint8 PhoneBTToVcuCmd29(uint8 *phonecmd) 
{    
    memcpy((uint8*)&carIdFromPhone,&phonecmd[2],4);
    memcpy((uint8*)&userIdFromPhone,&phonecmd[6],4);
              
    return TRUE;         
}

/*
*********************************************************************************
* ��������VcuBTToPhoneCmd29
* ˵  ����VCU�ظ���ȡ��֤������ֻ�
* ��  ����
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
*********************************************************************************
*/

uint8 VcuBTToPhoneCmd29() 
{
    uint8 cmd[15]= {0};    
    uint8 ret;
  
    cmd[0]=0x29;
    cmd[1]=0;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);
    memcpy(&cmd[10],(uint8*)&gCarInfor.seed,4);

    //DataEncryption(cmd,11);      
    cmd[14]=GetCRC(cmd,14);
     
    SendBytesToPhoneByBT(cmd,15);           
        
    return ret; 
}

/*
*********************************************************************************
* ��������PhoneBTToVcuCmd30
* ˵  �����ֻ���VCU���ͻ�ȡ��֤������
* ��  ����uint8 *phonecmd �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
*********************************************************************************
*/

uint8 PhoneBTToVcuCmd30(uint8 *phonecmd) 
{    
    
    uint8 seed[4],key1[4],key2[4];
    memcpy((uint8*)&carIdFromPhone,&phonecmd[2],4);
    memcpy((uint8*)&userIdFromPhone,&phonecmd[6],4);
    memcpy(key2,&phonecmd[10],4);
    
    memcpy(seed,(uint8*)&gCarInfor.seed,4);
    BCMSecurityAccess(seed,key1);
    if(key1[0]==key2[0] && key1[1]==key2[1] && key1[2]==key2[2] && key1[3]==key2[3]) 
    {
      return TRUE;
    }
    return FALSE;         
}

/*
*********************************************************************************
* ��������VcuBTToPhoneCmd30
* ˵  ����VCU�ظ���ȡ��֤������ֻ�
* ��  ����
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
*********************************************************************************
*/

uint8 VcuBTToPhoneCmd30(uint8 veryflag) 
{
    uint8 cmd[20]= {0};    
    uint8 ret;
    
    
    cmd[0]=0x30;
    cmd[1]=0;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);
    cmd[10] = veryflag;        
    memcpy(&cmd[11],(uint8*)&gCarThreshold.errCode,2);
    memcpy(&cmd[13],(uint8*)&gCarInfor.batVolt,2);
    memcpy(&cmd[15],(uint8*)&gCarInfor.batCurr,2);
    memcpy(&cmd[17],(uint8*)&gCarInfor.batTemp,1);
    //DataEncryption(cmd,11);      
    cmd[18]=GetCRC(cmd,18);
     
    SendBytesToPhoneByBT(cmd,19);           
        
    return ret; 
}



/*
*********************************************************************************
* ��������PhoneBTToVcuCmd31
* ˵  �����ֻ���VCU���ͳ���ά������
* ��  ����uint8 *phonecmd �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
*********************************************************************************
*/

uint8 PhoneBTToVcuCmd31(uint8 *phonecmd) 
{    
    memcpy((uint8*)&carIdFromPhone,&phonecmd[2],4);
    memcpy((uint8*)&userIdFromPhone,&phonecmd[6],4);
              
    return TRUE;         
}

/*
*********************************************************************************
* ��������VcuBTToPhoneCmd31
* ˵  ����VCU�ظ�����ά��״̬�Ƿ�ɹ�����ֻ�
* ��  ����
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
*********************************************************************************
*/

uint8 VcuBTToPhoneCmd31(uint8 *phonecmd) 
{
    uint8 cmd[12]= {0};    
    uint8 ret;
    
    
    cmd[0]=0x31;
    cmd[1]=0;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);


   //����������Ϊά��״̬
    
    gCarInfor.destState = BTRenting; 
    gCarInfor.destStateSource = StateFromCar;
    if(TRUE==CarStateChange()) 
    {
      
      gCarInfor.maxSpeed = phonecmd[10];             
      gCarInfor.preMaxSpeed = gCarInfor.maxSpeed;            
        
      cmd[10]=CAR_OP_SUCC;
      gCarInfor.userid = userIdFromPhone; 
      memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);
    } 
    else
    {
      cmd[10]=gCarInfor.state+0x80;
    }   
         
    cmd[11]=GetCRC(cmd,11);
     
    SendBytesToPhoneByBT(cmd,12);           
        
    return ret; 
}

/*
*********************************************************************************
* ��������PhoneBTToVcuCmd32
* ˵  �����ֻ���VCU���ͽ�������ά������
* ��  ����uint8 *servercommand �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
*********************************************************************************
*/
uint8 PhoneBTToVcuCmd32(uint8 *phonecmd) 
{

    memcpy((uint8*)&carIdFromPhone,&phonecmd[2],4);
    memcpy((uint8*)&userIdFromPhone,&phonecmd[6],4);                  
           
    return TRUE;      
    
}

/*
*********************************************************************************
* ��������VcuGprsToServerCmd32
* ˵  ����VCU�ظ�����ά��״̬�Ƿ�ɹ����������
* ��  ����
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
*********************************************************************************
*/
uint8 VcuBTToPhoneCmd32() 
{
    uint8 cmd[12]= {0};    
    uint8 ret;
    
    
    cmd[0]=0x32;
    cmd[1]=0;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);
    
  
    gCarInfor.destState = CarReady; 
    gCarInfor.destStateSource = StateFromCar;     
    if(CarStateChange()==TRUE)
    {                          
      cmd[10]=CAR_OP_SUCC;   
      gCarInfor.userid = 0;  
    } 
    else 
    { 
      cmd[10] = gCarInfor.state+0x80;
    }
    
         
    cmd[11]=GetCRC(cmd,11);    
    SendBytesToPhoneByBT(cmd,12);               
    return ret; 
}



/*
*********************************************************************************
* ��������PhoneBTToVcuCmd33
* ˵  �����ֻ��ظ�VCU���͵���������
* ��  ����uint8 *phonecmd �յ��������
* ����ֵ��TRUE ��Ч���� FALSE ��Ч����
* ��  ����
*********************************************************************************
*/

uint8 PhoneBTToVcuCmd33(uint8 *phonecmd) 
{    
       
    memcpy((uint8*)&carIdFromPhone,&phonecmd[2],4);
    memcpy((uint8*)&userIdFromPhone,&phonecmd[6],4);
    if(gCarInfor.userid==userIdFromPhone && gCarInfor.carid == carIdFromPhone) 
    {       
      gCarInfor.BTTimeout = 0;
    }
    return TRUE;         
}

/*
*********************************************************************************
* ��������VcuBTToPhoneCmd33
* ˵  ����VCU�����������ֻ�
* ��  ����
* ����ֵ��TRUE ���ͳɹ� FALSE ����ʧ��
* ��  ����
*********************************************************************************
*/

uint8 VcuBTToPhoneCmd33() 
{
    uint8 cmd[26]= {0};    
    uint8 ret;
    
    
    cmd[0]=0x33;
    cmd[1]=0;
    memcpy(&cmd[2],(uint8*)&gCarInfor.carid,4);
    memcpy(&cmd[6],(uint8*)&gCarInfor.userid,4);
    cmd[10] = (uint8)gCarInfor.speed;        
    memcpy(&cmd[11],(uint8*)&gCarInfor.tripMileage,4);
    memcpy(&cmd[15],(uint8*)&gCarThreshold.totalMileage,4);
    memcpy(&cmd[19],(uint8*)&gCarInfor.batVolt,2);
    memcpy(&cmd[21],(uint8*)&gCarInfor.batCurr,2);
    memcpy(&cmd[23],(uint8*)&gCarInfor.batTemp,1);
    //DataEncryption(cmd,11);      
    cmd[24]=GetCRC(cmd,24);
     
    SendBytesToPhoneByBT(cmd,25);           
        
    return ret; 
}

uint8 BTSendCommand(uint8* command,uint8*response)
{
    uint8 len;
 
    UART2_RxInterruptDisable();
  	UART2_SendString(command);
	  len=UART2_GetData(CmdRxBuffer,400,16);
	  UART2_RxInterruptEnable();
	  if (len > 0)
	  {
        return strcontains(CmdRxBuffer,response);
	  }	   
	  return 0;
}

#endif








