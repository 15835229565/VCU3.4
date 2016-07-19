#include  "includes.h"

uint8 GPRS_ERROR=0;



uint8 AT_Contains(uint8 *str,uint8 *find)
{
    uint8 * start = find;  
    uint8 i=0; 
    
    while (i<GSM_buffer_p) 
    {
        if (str[i] == *find)
        {
            find++;
        }
        else
        {
            find = start; 
        }
        
        if (*(find + 1) == '\0')
        {
            return TRUE;
        }
        i++;
    }  
    return FALSE;
}
      

uint8 AT_Transmit(uint8* command)
{
    uint8 len; 
  
    GSM_buffer_p = 0;   
    UART1_SendString(command);
   
    return TRUE ;
}
uint8 CheckATResp(uint8*response,uint16 waittime) 
{
    uint8 len=0,i;
    uint8  temp;
    
      
    temp=(uint16)(waittime/100);
    for(i=0;i<temp;i++)
    { 
      
      delayms(100);    
      
      if(TRUE==AT_Contains(GSM_MSG_Buf,response)) 
      {               
        return TRUE; 
      }
      
    
      if(TRUE==AT_Contains(GSM_MSG_Buf,"ERROR")) 
      {
        continue;
      }
         
    }   
    return FALSE;
}


/*
*********************************************************************************
* 函数名：Init_GPRS
* 说  明：初始化GPRS模块
* 参  数：无
* 返回值：
* 其  他GPRS模块从上电到CALL Ready出现需要26秒钟。然后到TCP连接建立需要10秒钟，
*         得出的结论是尽量不要使用重新上电的方式来进行模块的重启，尽量使用软复位的
*         得方式来重启模块 
*********************************************************************************
*/
uint8  Init_GPRS(void)
{
    uint8 i,j;
    
    //初始化重试0次
    for(i=0;i<1;i++) 
    {             
      AT_Transmit("at^sics=0,conType,GPRS0\r\n"); 
      if(FALSE==CheckATResp("OK",5000)) 
      {
        continue;
      }
          
      
      AT_Transmit("at^sics=0,passwd,gprs\r\n"); 
      if(FALSE==CheckATResp("OK",5000)) 
      {
        continue;
      }
      
      AT_Transmit("at^sics=0,user,cm\r\n"); 
      if(FALSE==CheckATResp("OK",5000)) 
      {
        continue;
      }
      
      AT_Transmit("at^sics=0,apn,cmnet\r\n"); 
      if(FALSE==CheckATResp("OK",5000)) 
      {
        continue;
      }
      
      AT_Transmit("at^siss=1,srvType,socket\r\n"); 
      if(FALSE==CheckATResp("OK",5000)) 
      {
        continue;
      }
      
      AT_Transmit("at^siss=1,conId,0\r\n"); 
      if(FALSE==CheckATResp("OK",5000)) 
      {
        continue;
      }
      
      GPRS_Get_SIM_ICCID(gCarInfor.iccid,20);
      GPRS_IOMODE();
      #if DEBUG_MODE ==0
      if(gCarThreshold.gprsMode==TCP_MODE) 
      {      
        if(gCarThreshold.severMode ==DEBUG_SERVER) 
        {           
           AT_Transmit("at^siss=1,address,\"socktcp://demo.roboy.com.cn:9124\"\r\n");
        } 
        else 
        {
           AT_Transmit("at^siss=1,address,\"socktcp://carserver.roboy.com.cn:9124\"\r\n");           
        }
      } 
      else 
      {
        if(gCarThreshold.severMode ==DEBUG_SERVER) 
        {
           AT_Transmit("at^siss=1,address,\"sockudp://demo.roboy.com.cn:9123\"\r\n");
        } 
        else 
        {
           AT_Transmit("at^siss=1,address,\"sockudp://carserver.roboy.com.cn:9123\"\r\n");
        }
      }
      #endif
        
     // AT_Transmit("at^siss=1,address,\"sockudp://dev.roboy.com.cn:9123\"\r\n");
      #if DEBUG_MODE ==1
      AT_Transmit("at^siss=1,address,\"sockudp://carserver.roboy.com.cn:9123\"\r\n");     
      #endif
      if(TRUE==CheckATResp("OK",5000)) 
      {
         AT_Transmit("AT^SISO=1\r\n"); 
         if(TRUE==CheckATResp("^SISW:",20000)) 
         {
            return TRUE;
         }
      }
          
    }
    return FALSE;

}
/*
*********************************************************************************
* 函数名：Close_GPRS
* 说  明：关闭GPRS模块
* 参  数：无
* 返回值：
* 其  他：
*********************************************************************************
*/
uint8  Close_GPRS(void)
{
    uint8 ret1,ret2;
    AT_Transmit("AT^SISC=1\r\n");
    ret1 =CheckATResp("OK",5000);
    
    if(ret1==TRUE)
        return TRUE;
    else
        return FALSE;
}

/*
*********************************************************************************
* 函数名：GPRS_Get_SIM_ICCID
* 说  明：获得SIM卡的ICCID
* 参  数：无
* 返回值：
* 其  他：
*********************************************************************************
*/
uint8 GPRS_Get_SIM_ICCID(uint8*iccid,uint8 len)
{
    uint8 i,j;
    AT_Transmit("AT^ICCID?\r\n");
   
    if(TRUE==CheckATResp("OK",5000)) 
    {
      for(i=0;i<GSM_buffer_p-1;i++) 
      {
        if(GSM_MSG_Buf[i]==':' && GSM_MSG_Buf[i+1]==' ' ) 
        {
          if(i+20<GSM_buffer_p) 
          {
            memcpy(iccid,&GSM_MSG_Buf[i+2],len);
            return TRUE;
          }
        }
      }
    }
    return FALSE;
}
/*
*********************************************************************************
* 函数名：GPRS_Open_UDP_OR_TCP_Connection
* 说  明：开启UDP或TCP连接
* 参  数：无
* 返回值：
* 其  他：
*********************************************************************************
*/
uint8 GPRS_Open_UDP_OR_TCP_Connection(void)
{
      
      AT_Transmit("at^siss=1,srvType,socket\r\n"); 
      if(FALSE==CheckATResp("OK",5000)) 
      {
        ;
      }
      
      AT_Transmit("at^siss=1,conId,0\r\n"); 
      if(FALSE==CheckATResp("OK",5000)) 
      {
        ;
      }
      
      #if DEBUG_MODE ==0
      if(gCarThreshold.gprsMode==TCP_MODE) 
      {      
        if(gCarThreshold.severMode ==DEBUG_SERVER) 
        {           
           AT_Transmit("at^siss=1,address,\"socktcp://demo.roboy.com.cn:9124\"\r\n");
        } 
        else 
        {
           AT_Transmit("at^siss=1,address,\"socktcp://carserver.roboy.com.cn:9124\"\r\n");           
        }
      } 
      else 
      {
        if(gCarThreshold.severMode ==DEBUG_SERVER) 
        {
           AT_Transmit("at^siss=1,address,\"sockudp://demo.roboy.com.cn:9123\"\r\n");
        } 
        else 
        {
           AT_Transmit("at^siss=1,address,\"sockudp://carserver.roboy.com.cn:9123\"\r\n");
        }
      }
      #endif
    
    #if DEBUG_MODE ==1
    AT_Transmit("at^siss=1,address,\"sockudp://carserver.roboy.com.cn:9123\"\r\n");     
    #endif
            
    if(TRUE==CheckATResp("OK",5000)) 
    {
       AT_Transmit("AT^SISO=1\r\n"); 
       if(TRUE==CheckATResp("^SISW:",20000)) 
       {
         
          
          return TRUE;
       }
    }
    return FALSE; 
   
}
/*
*********************************************************************************
* 函数名：GPRS_Enable_Send_Data_By_UDP_OR_TCP
* 说  明：使能UDP或TCP连接数据发送
* 参  数：无
* 返回值：
* 其  他：
*********************************************************************************
*/
uint8 GPRS_Enable_Send_Data_By_UDP_OR_TCP(uint8 len)
{
    uint8 ATbuf[16]="AT^SISW=1,";
    
    //转为字符
    if(len>0&&len<10) 
    {
      ATbuf[10]=len+'0';
      ATbuf[11]='\r';
      ATbuf[12]='\n';
      ATbuf[13]=0;
    }
    else if(len>=10&&len<100)
    {
      ATbuf[10]=len/10+'0';
      ATbuf[11]=len%10+'0';
      ATbuf[12]='\r';
      ATbuf[13]='\n';
      ATbuf[14]=0;
    }
    else if(len>=100)
    {
      ATbuf[10]=len/100+'0';
      ATbuf[11]=(len/10)%10+'0';
      ATbuf[12]=len%10+'0';
      ATbuf[13]='\r';
      ATbuf[14]='\n';
      ATbuf[15]=0;
    }
    AT_Transmit(ATbuf);

    return CheckATResp("^SISW",5000); 
    
}



void EmptyBuffer()
{
    memset(GSM_MSG_Buf,0,GSM_BUFFER_SIZE);
    GSM_buffer_p = 0;
}

uint8 GPRS_SERVICE_ATTACH(void)
{
    uint8 data0[11],data1[11];
    
    memcpy(data0,"+CGATT: 0",9);
    data0[9]=0x0D;
    data0[10]=0x00;
    
    memcpy(data1,"+CGATT: 1",9);
    data1[9]=0x0D;
    data1[10]=0x00;

    AT_Transmit("AT+CGATT?\r\n");
    if(TRUE==CheckATResp(data0,2000)) 
    {
      return FALSE;
    }
    
    if(TRUE==CheckATResp(data1,2000)) 
    {
      return TRUE;
    }
    
    return FALSE; 
}

uint8 GPRS_CREG(void)
{
    uint8 data0[11],data1[11];
    
    memcpy(data0,"+CREG: 0",9);
    data0[9]=0x0D;
    data0[10]=0x00;
    
    memcpy(data1,"+CREG: 1",9);
    data1[9]=0x0D;
    data1[10]=0x00;

    AT_Transmit("AT+CREG?\r\n");
    if(TRUE==CheckATResp(data0,2000)) 
    {
      return FALSE;
    }
    
    if(TRUE==CheckATResp(data1,2000)) 
    {
      return TRUE;
    }
    
    return FALSE; 
}

uint8 GPRS_GET_RSSI(void)
{    
    uint8 i;
    AT_Transmit("AT+CSQ\r\n"); 
    if(TRUE==CheckATResp("OK",5000)) 
    {
      for(i=0;i<GSM_buffer_p-1;i++) 
      {
        if(GSM_MSG_Buf[i]==':' && GSM_MSG_Buf[i+1]==' ' ) 
        {
          if(i+3<GSM_buffer_p) 
          {
             if(GSM_MSG_Buf[i+3]>='0'&& GSM_MSG_Buf[i+3]<='9' && GSM_MSG_Buf[i+2]>='0'&& GSM_MSG_Buf[i+2]<='9') 
             {              
              gCarInfor.rssi = 10*(GSM_MSG_Buf[i+2]-'0') + (GSM_MSG_Buf[i+3]-'0');
              EmptyBuffer();
              return TRUE;
             } 
             else if(GSM_MSG_Buf[i+3]==',' && GSM_MSG_Buf[i+2]>='0'&& GSM_MSG_Buf[i+2]<='9') 
             {
              gCarInfor.rssi =(GSM_MSG_Buf[i+2]-'0');
              EmptyBuffer();
              return TRUE;
             }
             EmptyBuffer();   
             return FALSE;
              
          }
        }
      }
    } 
    EmptyBuffer();
}

uint8 GPRS_IOMODE(void)
{   
    AT_Transmit("AT^IOMODE=0,1\r\n");
    if(TRUE==CheckATResp("OK",2000)) 
    {
      return TRUE;
    }
    
    return FALSE; 
}

uint8 GPRS_MODULE_POWEROFF(void)
{
    delayms(5000);
    
    AT_Transmit("AT^SMSO\r\n");
    if(FALSE==CheckATResp("MS OFF",5000)) 
    {
      return TRUE;
    }
    
    return FALSE;  
}

uint8 GPRS_DVCFG(void)
{    
    
    AT_Transmit("AT^DVCFG=1\r\n");
    if(FALSE==CheckATResp("OK",5000)) 
    {
      return TRUE;
    }
    
    return FALSE;  
}

void MG301_PowerOn()
{
  if(GSM_buffer_p>=5&&FALSE!=AT_Contains(GSM_MSG_Buf,"CLOSED")) 
  {              
      gGPRSConnectTimer+=1+gCarThreshold.maxConnectTimeOut;
      GSM_buffer_p = 0;
  }

  if(GSM_buffer_p>=5&&FALSE!=AT_Contains(GSM_MSG_Buf,"SYSST")) 
  {
      GPRS_init_flag = GPRS_INITIAL; 
      #if DEBUG_MODE ==0     
      AT_Transmit("ATE0\r\n");
      #endif             
      GSM_buffer_p = 0;
  }
  
  if(GSM_buffer_p>=5&&FALSE!=AT_Contains(GSM_MSG_Buf,"SHUTD")) 
  {
      GPRS_init_flag = GPRS_RESTART;
      GSM_buffer_p = 0;
  }
}




