#include "includes.h"
#include <mc9s12g128.h>

uint8 gCarParaEepromUpdateFlag=0;   //车辆参数EEPROM更新标志位 1表示需要更新 0表示不用更新
uint8 gCarThresholdEepromUpdateFlag=0;   //车辆阈值EEPROM更新标志位 1表示需要更新 0表示不用更新



uint8 routine_ppage = 0xEu;

/*
 * Function:flash_init
 * Param<void>:
 * Return<void>:
 * REQ IDs:TPMS_SW_BASE_0015,TPMS_SW_BASE_0154
 * Discription: 3?ê??ˉFlashê±?ó
 * Note:
*/
#pragma CODE_SEG RAM_CODE
/*
 * Function:fcmd_start_and_wait         
 * Param<void>:
 * Return<void>:
 * REQ IDs:TPMS_SW_BASE_0155,TPMS_SW_BASE_0156
 * Discription:
 * Note:
*/
void fcmd_start_and_wait(void)
{
    FSTAT = FLASH_CMD_START; /*Step5 start command*/
    while (!FSTAT_CCIF) /*Step6 wait cmd complete or an error occur*/
    {
    }
}
#pragma CODE_SEG DEFAULT

void flash_init(void)
{
    routine_ppage = PPAGE;
    FCNFG = 0x00;
    if (0 == FCLKDIV_FDIVLD)
    {
        FCLKDIV_FDIV =  0x03; /*3 for 4MHz bus_clk,7 for 8MHz bus_clk*/
        while (0 == FCLKDIV_FDIVLD);
        FSTAT = 0x30;  /*clear FACCERR and PVIOL flag*/
    }
}


/*
 * Function:flash_send_cmd
 * Param<uint16_t p_cmd[]>:
 * Param<uint8_t cmd_length>:
 * Return<bool>:
 * REQ IDs:TPMS_SW_BASE_0157,TPMS_SW_BASE_0158
 * Discription:
 * Note:
*/
bool flash_send_cmd(uint16_t p_cmd[], uint8_t cmd_length)
{
    uint8_t cmd_index = 0;
    if (1 == FSTAT_CCIF) /*Step2 check last command is complete or not*/
    {
        /*Step3 clear Access Error and Protection Violation and set page*/
        PPAGE = routine_ppage;
        FSTAT = (FSTAT_FPVIOL_MASK | FSTAT_ACCERR_MASK); /*Must used =, don't used |= operate*/
        DisableInterrupts;
        for (cmd_index = 0; cmd_index < cmd_length; cmd_index++)/*Step4 set command array*/
        {
            FCCOBIX = cmd_index; /*set CMD*/
            FCCOB   = p_cmd[cmd_index];
        }
        EnableInterrupts;
        fcmd_start_and_wait();

        if (FLASH_CMD_COMPLETE == (0xFC & FSTAT)) /*Note this*/
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

/*
 * Function:get_phy_flash_address
 * Param<uint32_t addr>:
 * Return<uint32_t>:
 * REQ IDs:TPMS_SW_BASE_0159
 * Discription:
 * Note:
*/
uint32_t get_phy_flash_address(uint32_t addr)
{
    if (addr >= FLASH_START_ADDR && addr <= FLASH_END_ADDR)
    {
        /* Is physical address already */
    }
    else
    {
        if (   (addr >= 0x4000 && addr <= 0x7FFF)
                || (addr >= 0xC000 && addr <= 0xFFFF)
           )
        {
            addr = addr | 0x30000ul;
        }
        else if (   ((addr & P_PAGE_ADDR_MASK) >= P_PAGE_START_ADDR)
                    && ((addr & P_PAGE_ADDR_MASK) <= P_PAGE_END_ADDR)
                )/*is page mode addr*/
        {
            /*allways treat by ppage*/
            addr = ((addr >> 16) << P_FLASH_PAGE_SHIFT_BIT) + ((addr & 0x0000FFFFu) - P_PAGE_START_ADDR);
        }
    }
    return addr;
}

/*
 * Function:phy_to_ppage_address
 * Param<uint32_t phy_addr>:
 * Return<uint16_t>:
 * REQ IDs:TPMS_SW_BASE_0160
 * Discription:
 * Note:
*/
uint16_t phy_to_ppage_address(uint32_t phy_addr)
{
    uint16_t ppage_addr = 0;
    int16_t  block_index = 0;

    if (phy_addr >= FLASH_START_ADDR && phy_addr <= FLASH_END_ADDR)
    {
        phy_addr = phy_addr - FLASH_START_ADDR;
        ppage_addr = (uint16_t)((phy_addr % P_PAGE_SIZE) + P_PAGE_START_ADDR);
    }
    return ppage_addr;
}

/*
 * Function:get_routine_ppage
 * Param<uint32_t phy_addr>:
 * Return<uint8_t>:
 * REQ IDs:TPMS_SW_BASE_0161
 * Discription:
 * Note:
*/
uint8_t get_routine_ppage(uint32_t phy_addr)
{
    uint8_t addr_ppage = 0xFE;
    addr_ppage = (uint8_t)(phy_addr >> P_FLASH_PAGE_SHIFT_BIT);
    return addr_ppage;
}

/*
 * Function:flash_erase
 * Param<uint32_t start_addr>:
 * Param<uint32_t length>:
 * Return<bool>:
 * REQ IDs:TPMS_SW_BASE_0162,TPMS_SW_BASE_0163
 * Discription:
 * Note:
*/
bool flash_erase(uint32_t start_addr,uint32_t length)
{
    uint32_t phy_start_addr;
    uint32_t phy_end_addr;
    uint16_t cmd_array[2];
    int16_t  block_index = 0;

    phy_start_addr = get_phy_flash_address(start_addr); /*Not check start address*/
    phy_end_addr = phy_start_addr + length - 1;

    if (phy_start_addr < FLASH_START_ADDR || phy_end_addr > FLASH_END_ADDR)
    {
        return false;
    }

    /*erase address available*/
    while (phy_start_addr <= phy_end_addr)
    {
        cmd_array[0] = FCMD_ERASE_P_SECTOR | ((phy_start_addr >> 16) & 0x00FF);
        cmd_array[1] = phy_start_addr & 0x0000FFFF;
        routine_ppage = get_routine_ppage(phy_start_addr);
        if (false == flash_send_cmd(cmd_array,2))
        {
            return false;
        }
        phy_start_addr += P_FLASH_SECTOR_SIZE;
    }
    return true;
}

/*
 * Function:ineeprom_write
 * Param<uint8_t *p_data_buffer>:
 * Param<uint16_t phy_start_addr>:
 * Param<uint16_t length>:
 * Return<bool>:
 * REQ IDs:TPMS_SW_BASE_0164,TPMS_SW_BASE_0165,TPMS_SW_BASE_0166,TPMS_SW_BASE_0167,TPMS_SW_BASE_0168
 * Discription:
 * Note:
*/
bool ineeprom_write(uint8_t *p_data_buffer, uint16_t phy_start_addr, uint16_t length)
{
    uint16_t cmd_array[6];
    uint16_t phy_base_addr;
    int16_t  i_index = 0;
    int16_t  data_index = 0;
    uint16_t *cur_prog_data_addr;
    uint8_t  sector_prog[INEEPROM_SECTOR_SIZE] = {0}; /*4 bytes*/

    if (0 == p_data_buffer || 0 == length)
    {
        return false;
    }
    else if (phy_start_addr < INEEPROM_START_ADDR || (phy_start_addr + length - 1) > INEEPROM_END_ADDR)
    {
        return false;
    }
    else
    {
        /*Address and length correct.*/
    }
    /*program address available*/
    while (length > 0) /*1 sector program a loop*/
    {
        /*Step 1. combination program data sector*/
        phy_base_addr = phy_start_addr & INEEPROM_BASE_ADDR_MASK;
        for (i_index = 0, data_index = 0; i_index < INEEPROM_SECTOR_SIZE; i_index++)
        {
            if (length > 0 && phy_base_addr >= phy_start_addr)
            {
                sector_prog[i_index] = p_data_buffer[data_index];
                length--;
                data_index++;
            }
            else
            {
                sector_prog[i_index] =  *(uint8_t *)(phy_base_addr);
            }
            phy_base_addr++;
        }

        /*Step 2. Erase sector*/
        phy_base_addr = phy_start_addr & INEEPROM_BASE_ADDR_MASK;
        phy_start_addr = phy_start_addr + data_index; /* Point to next data area */
        p_data_buffer = p_data_buffer + data_index;
        cmd_array[0] = FCMD_ERASE_D_SECTOR; /* inEEPROM GlobalAddr[17:16] is 00B */
        cmd_array[1] = phy_base_addr;
        if (false == flash_send_cmd(cmd_array,2))
        {
            return false;
        }

        /*Step 3. Program new sector*/

        cur_prog_data_addr = (uint16_t *)sector_prog;
        cmd_array[0] = FCMD_PROG_D_FLASH;
        cmd_array[1] = phy_base_addr;
        cmd_array[2] = cur_prog_data_addr[0];
        cmd_array[3] = cur_prog_data_addr[1];

        if (false == flash_send_cmd(cmd_array,4))
        {
            return false;
        }

        /*Check data*/
        if ((*(uint16_t *)(phy_base_addr) != cur_prog_data_addr[data_index])
                || (*(uint16_t *)(phy_base_addr + 2) != cur_prog_data_addr[data_index]))
        {
            return false;
        }
    }
    return true;
}

/*
 * Function:ineeprom_read
 * Param<uint8_t *p_data_buffer>:
 * Param<uint16_t phy_start_addr>:
 * Param<uint16_t length>:
 * Return<bool>:
 * REQ IDs:TPMS_SW_BASE_0169,TPMS_SW_BASE_0170,TPMS_SW_BASE_0171
 * Discription:
 * Note:
*/
bool ineeprom_read(uint8_t *p_data_buffer, uint16_t phy_start_addr, uint16_t length)
{
    uint16_t data_index = 0;
    if (0 == p_data_buffer || 0 == length)
    {
        return false;
    }
    else if (phy_start_addr < INEEPROM_START_ADDR || (phy_start_addr + length - 1) > INEEPROM_END_ADDR)
    {
        return false;
    }
    else
    {
        for (data_index = 0; data_index < length; data_index++)
        {
            p_data_buffer[data_index] = *(uint8_t *)(phy_start_addr);
            phy_start_addr++;
        }
        return true;
    }
}

void WriteCarPara()
{
    uint8 buf[CAR_PARA_EEPROM_SIZE],i,origiData[4];
    
    memset(buf,0,CAR_PARA_EEPROM_SIZE);
    memcpy(buf,(uint8*)&gCarPara,sizeof(gCarPara));
    for(i=0;i<CAR_PARA_EEPROM_SIZE;) 
    {
      //防止数据多次重写导致的eeprom损坏，先读出要写的区域的内容，比较后，如果不同则写入
      ineeprom_read(origiData,CAR_PARA_START_ADDRESS+i,4);
      if(MemEqual(origiData,&buf[i],4)==FALSE) 
      {        
        ineeprom_write(&buf[i],CAR_PARA_START_ADDRESS+i,4);
      }
      i+=4;
    }
}
void ReadCarPara()
{
    uint8 buf[CAR_PARA_EEPROM_SIZE],i,flag=0;
    
    memset(buf,0,CAR_PARA_EEPROM_SIZE);
    
    for(i=0;i<CAR_PARA_EEPROM_SIZE;) 
    {
      ineeprom_read(&buf[i],CAR_PARA_START_ADDRESS+i,4);
      i+=4;
    }
    memcpy((uint8*)&gCarPara,buf,sizeof(gCarPara));
    
    
    //配置默认值
    if(gCarPara.motorPower ==0||gCarPara.motorPower==0xFFFF) 
    {
        gCarPara.motorPower = 650;
        flag = 1;    
    }
    
    if(gCarPara.hallSensorPara==0||gCarPara.hallSensorPara==0xFF) 
    {
       gCarPara.hallSensorPara=4;
       flag = 1;    
    }
    
    if(gCarPara.softwareVersion==0x00 || gCarPara.softwareVersion==0xFFFF)  
    {      
        gCarPara.softwareVersion=SOFTWARE_VERSION;
        flag = 1;     
    } 
    
    if(gCarPara.tireDiameter==0||gCarPara.tireDiameter==0xFFFF) 
    {      
        gCarPara.tireDiameter=420; 
        flag = 1; 
    }
    
    if(gCarPara.name[0]==0xFF) 
    {
        memset(gCarPara.name,'0',sizeof(gCarPara.name));                    
        flag = 1;
    }
    if(gCarPara.VCUID[0]==0xFF) 
    {
        memset(gCarPara.VCUID,0,sizeof(gCarPara.VCUID));          
        flag = 1;
    }
    if(gCarPara.VIN[0]==0xFF) 
    {
        memset(gCarPara.VIN,0,sizeof(gCarPara.VIN));
        flag = 1;
    }
    
    if(gCarPara.hardwareVersion==0x00 || gCarPara.hardwareVersion==0xFFFF) 
    {
        gCarPara.hardwareVersion = HARDWARE_VERSION; 
        flag = 1;       
    }
    
    if(gCarPara.softwareUpdateDate==0x00 || gCarPara.softwareUpdateDate==0xFFFF) 
    {
        gCarPara.softwareUpdateDate = SOFTWARE_UPDATE_DATE;
        flag = 1;
    }
    
    if(gCarPara.mcuRefVoltStand>10 || gCarPara.mcuRefVoltStand<3) 
    {
        gCarPara.mcuRefVoltStand = 5.12;
        flag = 1;
    }
    
    
    if(gCarPara.batVoltReviseCoef>1 || gCarPara.batVoltReviseCoef<0.001) 
    {
        gCarPara.batVoltReviseCoef = 0.08;        
        flag = 1;
    }
    
    if(gCarPara.batCurrReviseCoef>1 || gCarPara.batCurrReviseCoef<0.001) 
    {
        gCarPara.batCurrReviseCoef = 0.05;
        flag = 1;
    }
    
    if(gCarPara.batzeroCurrVolt>5 || gCarPara.batzeroCurrVolt<1) 
    {
        gCarPara.batzeroCurrVolt = 2.5;
        flag = 1;
    }
    
    if(gCarPara.XAccelBase==0xFFFF || gCarPara.YAccelBase==0xFFFF || gCarPara.ZAccelBase==0xFFFF ) 
    {
        gCarPara.XAccelBase=328;           
        gCarPara.YAccelBase=350;           
        gCarPara.ZAccelBase=448; 
        flag = 1;
    }
    
    if(gCarPara.minAccelPedal==0||gCarPara.minAccelPedal==0xFFFF) 
    {
        gCarPara.minAccelPedal=240;
        flag = 1;
    }
    
    if(gCarPara.maxAccelPedal==0||gCarPara.maxAccelPedal==0xFFFF) 
    {
        gCarPara.maxAccelPedal=850;
        flag = 1;
    }
    
    if(gCarPara.midAccelPedal==0||gCarPara.midAccelPedal==0xFFFF) 
    {
        gCarPara.midAccelPedal=800;
        flag = 1;
    }
    
    if(gCarPara.gearRatio<1|| gCarPara.gearRatio>50) 
    {        
      gCarPara.gearRatio=8;  
      flag =1;
    }  
    
    if(flag==1)    
    {
      gCarParaEepromUpdateFlag=1;               
    }
    
    //取VIN码的后三个字节作为车辆的唯一标示ID    
    memcpy((uint8*)&gCarInfor.carid,&gCarPara.VIN[4],3);
    gCarInfor.carid = gCarInfor.carid>>8;
    
    #if DEBUG_MODE==1
    gCarInfor.carid = CarID;
    gCarPara.hardwareVersion=HARDWARE_VERSION;  
    #endif
    
}

void WriteCarThreshold()
{
    uint8 buf[CAR_THRESHOLD_EEPROM_SIZE],i,origiData[4];
    
    memset(buf,0,CAR_THRESHOLD_EEPROM_SIZE);
    memcpy(buf,(uint8*)&gCarThreshold,sizeof(gCarThreshold));
    for(i=0;i<CAR_THRESHOLD_EEPROM_SIZE;) 
    {
      //防止数据多次重写导致的eeprom损坏，先读出要写的区域的内容，比较后，如果不同则写入
      ineeprom_read(origiData,CAR_THRESHOLD_START_ADDRESS+i,4);
      if(MemEqual(origiData,&buf[i],4)==FALSE) 
      {        
        ineeprom_write(&buf[i],CAR_THRESHOLD_START_ADDRESS+i,4);
      }
      
      i+=4;
    }
}
void ReadCarThreshold()
{
    uint8 buf[CAR_THRESHOLD_EEPROM_SIZE],i,flag=0;
    
    memset(buf,0,CAR_THRESHOLD_EEPROM_SIZE);
    
    for(i=0;i<CAR_THRESHOLD_EEPROM_SIZE;) 
    {
      ineeprom_read(&buf[i],CAR_THRESHOLD_START_ADDRESS+i,4);
      i+=4;
    }
    memcpy((uint8*)&gCarThreshold,buf,sizeof(gCarThreshold)); 
    
      //配置默认值
    if(gCarThreshold.batLowLevel==0||gCarThreshold.batLowLevel==0xFF) 
    {   
        gCarThreshold.batLowLevel     = 10;            //低电量警告阈值0~100       
           
        flag = 1;       
    }
    if(gCarThreshold.batTempLevel==0||gCarThreshold.batTempLevel==0xFF) 
    {        
        gCarThreshold.batTempLevel    = 100;           //电池温度超标警告阈值0~255      
        flag = 1;       
    }
    
    if(gCarThreshold.operationMode==0||gCarThreshold.operationMode==0xFF) 
    {    
        gCarThreshold.operationMode   = COIN_AND_MOBILE_OPERATION;         //车辆运营模式      
        flag = 1;       
    }
    
    if(gCarThreshold.perCoinMetre==0||gCarThreshold.perCoinMetre==0xFFFF) 
    {     
        gCarThreshold.perCoinMetre    = 1000;          // 每硬币多少米      
        flag = 1;       
    }
    
    if(gCarThreshold.perCoinTime==0||gCarThreshold.perCoinTime==0xFFFF) 
    {   
        gCarThreshold.perCoinTime     = 5   ;          // 每硬币可占用小车最多时间 单位 秒              
        flag = 1;       
    }
    
    if(gCarThreshold.shortHeartTime==0||gCarThreshold.shortHeartTime==0xFF) 
    {  
        gCarThreshold.shortHeartTime  = 15;             //短心跳秒数      
        flag = 1;       
    }
    
    if(gCarThreshold.longHeartTime==0||gCarThreshold.longHeartTime==0xFF) 
    {    
        gCarThreshold.longHeartTime   = 30;            //长心跳秒数           
        flag = 1;               
    }
    
    if(gCarThreshold.maxSpeed==0||gCarThreshold.maxSpeed==0xFF) 
    {   
        gCarThreshold.maxSpeed        = 5;            //用户能够操作的最高车速         
        flag = 1;             
    }
    
    if(gCarThreshold.userLevel==0xFF) 
    {   
        gCarThreshold.userLevel  = 0;                  //用户等级                        
        flag = 1;       
    }
    
    if(gCarThreshold.LTLatitude1==0xFFFF ||gCarThreshold.LTLongitude1==0xFFFF || gCarThreshold.RBLatitude1==0xFFFF || gCarThreshold.RBLongitude1==0xFFFF
    || gCarThreshold.LTLatitude2==0xFFFFFFFF ||gCarThreshold.LTLongitude2==0xFFFFFFFF || gCarThreshold.RBLatitude2==0xFFFFFFFF || gCarThreshold.RBLongitude2==0xFFFFFFFF) 
    {   
        gCarThreshold.LTLatitude1  = 0;                //车辆左上角经纬度
        gCarThreshold.LTLatitude2  = 0;    
        gCarThreshold.LTLongitude1  = 0;   
        gCarThreshold.LTLongitude2  = 0; 
        gCarThreshold.RBLatitude1 = 0;                 //车辆右下角经纬度  
        gCarThreshold.RBLatitude2  = 0; 
        gCarThreshold.RBLongitude1  = 0;   
        gCarThreshold.RBLongitude2  = 0; 
      
        flag = 1;       
    }
    
    
    if(gCarThreshold.stopTimeout==0 || gCarThreshold.stopTimeout==0xFF) 
    {
        gCarThreshold.stopTimeout     = 10;            //租车条件下，停车超时时间，单位为分钟       
        flag = 1;                 
    }
    
    if(gCarThreshold.errCode==0xFFFF) 
    {     
        gCarThreshold.errCode         = 0;             //车辆错误码          
        flag = 1;              
    }
    
    if(gCarThreshold.totalMileage==0xFFFFFFFF) 
    {   
        gCarThreshold.totalMileage    = 0;             //总里程               
        flag = 1;             
    }
    
    if(gCarThreshold.GPRS_Noconnect_Coinnum==0xFFFF) 
    {   
        gCarThreshold.GPRS_Noconnect_Coinnum= 2;             //网络断开时投币个数               
        flag = 1;             
    }  
    if(gCarThreshold.brakeActiveCounter>10000000)
    {       
        
        gCarThreshold.brakeActiveCounter  = 0;        
        flag = 1;       
    }
    
    if(gCarThreshold.maxCoin==0 || gCarThreshold.maxCoin == 0xFF) 
    {
        gCarThreshold.maxCoin = 20;                      //最大投币数
   
        flag = 1;
    }
    
    if(gCarThreshold.maxConnectTimeOut==0 || gCarThreshold.maxConnectTimeOut == 0xFFFF) 
    {
         gCarThreshold.maxConnectTimeOut = 180;           //默认180秒     
        flag = 1;
    } 
    
    if(gCarThreshold.gprsInitFlag!=0 && gCarThreshold.gprsInitFlag!=1) 
    {
        gCarThreshold.gprsInitFlag = 0;
        flag = 1;
    }
    
    if(gCarThreshold.severMode!=DEBUG_SERVER && gCarThreshold.severMode!=RELEASE_SERVER) 
    {
        gCarThreshold.severMode = DEBUG_SERVER;
        flag = 1;
    }
    
    if(gCarThreshold.gprsMode!=UDP_MODE && gCarThreshold.gprsMode!=TCP_MODE) 
    {
        gCarThreshold.gprsMode = UDP_MODE;
        flag = 1;
    }
    
    
    if(flag==1)    
    {
      gCarThresholdEepromUpdateFlag=1;
    }
   
}


/*
*********************************************************************************
* 函数名：MemEqual
* 说  明：比较两个内存块内容是否相同
* 参  数：uint8* buf1 内存块1 ,uint8* buf2 内存块1 uint8 length 内存块长度
* 返回值：TRUE  相同      FALSE 不相同
* 其  他：
*********************************************************************************
*/
uint8 MemEqual(uint8* buf1,uint8* buf2,uint8 length) 
{
  uint8 i;
  
  for(i=0;i<length;i++) 
  {
    if(buf1[i]==buf2[i]) 
    {
      ;
    } 
    else 
    {
      return FALSE;
    }
  }
  
  return TRUE;
}


//清空数据区域，为测试准备
void EEPROM_INITIAL() 
{
  uint8 i=0;
  uint8 buf[4]={0xFF,0xFF,0xFF,0xFF};
  
  for(i=0;i<CAR_PARA_EEPROM_SIZE+CAR_THRESHOLD_EEPROM_SIZE;i+=4) 
  {
    ineeprom_write(buf,CAR_PARA_START_ADDRESS+i,4);
  }
}

void WriteBootFlag(uint8 mode)
{
   uint8 CompleteAppFlag[4]={0,0,0,0},i;
    
   for(i=0;i<3;i++) 
   {    
    CompleteAppFlag[i] = mode;
    if(true == ineeprom_write(CompleteAppFlag,APP_VALID_FLAG_ADDR,4)) 
    {
      break;
    }
   }
}



/*********************************************************************
 * @fn      smbSend
 * @brief   Sends buffer contents to SM-Bus device
 * @param   buffer - ptr to buffered data to send
 * @param   len - number of bytes in buffer
 * @param   sendStart - whether or not to send start condition.
 * @param   sendStop - whether or not to send stop condition.
 * @return  void
 */
void smbSend( uint8 *buffer, uint16 len, uint8 sendStart, uint8 sendStop )
{
  uint16 i;

  if (!len)  {
    return;
  }

  if (sendStart == SEND_START)  {
    smbStart();
  }

  for ( i = 0; i < len; i++ )
  {
    while ( !smbSendByte( buffer[i] ) );  // takes care of ack polling
  }

  if (sendStop == SEND_STOP) {
    smbStop();
  }
}

/*********************************************************************
 * @fn      smbSendByte
 * @brief   Serialize and send one byte to SM-Bus device
 * @param   dByte - data byte to send
 * @return  ACK status - 0=none, 1=received
 */
BOOL smbSendByte( uint8 dByte )
{
  uint8 i;

  for ( i = 0; i < 8; i++ )
  {
    // Send the MSB
    smbWrite( dByte & 0x80 );
    // Next bit into MSB
    dByte <<= 1;
  }
  // need clock low so if the SDA transitions on the next statement the
  // slave doesn't stop. Also give opportunity for slave to set SDA
  smbClock( 0 );
  OCM_DATA_HIGH(); // set to input to receive ack...
  smbClock( 1 );
  smbWait(1);

  return !OCM_SDA;  // Return ACK status
}

/*********************************************************************
 * @fn      smbWrite
 * @brief   Send one bit to SM-Bus device
 * @param   dBit - data bit to clock onto SM-Bus
 * @return  void
 */
void smbWrite( BOOL dBit )
{
  smbClock( 0 );
  smbWait(1);
  if ( dBit )
  {
    OCM_DATA_HIGH();
  }
  else
  {
    OCM_DATA_LOW();
  }

  smbClock( 1 );
  smbWait(1);
}

/*********************************************************************
 * @fn      smbClock
 * @brief   Clocks the SM-Bus. If a negative edge is going out, the
 *          I/O pin is set as an output and driven low. If a positive
 *          edge is going out, the pin is set as an input and the pin
 *          pull-up drives the line high. This way, the slave device
 *          can hold the node low if longer setup time is desired.
 * @param   dir - clock line direction
 * @return  void
 */
void smbClock( bool dir )
{
  if ( dir )
  {
    OCM_SCL_HIGH();
  }
  else
  {
    OCM_SCL_LOW();
    OCM_SCL = 0;
  }
  smbWait(1);
}

/*********************************************************************
 * @fn      smbStart
 * @brief   Initiates SM-Bus communication. Makes sure that both the
 *          clock and data lines of the SM-Bus are high. Then the data
 *          line is set high and clock line is set low to start I/O.
 * @param   void
 * @return  void
 */
void smbStart( void )
{
  // wait for slave to release clock line...
  // set SCL to input but with pull-up. if slave is pulling down it will stay down.
  smbClock(1);
  while (!OCM_SCL) ; // wait until the line is high...

  // SCL low to set SDA high so the transition will be correct.
  smbClock(0);
  OCM_DATA_HIGH();  // SDA high
  smbClock(1);      // set up for transition
  smbWait(1);
  OCM_DATA_LOW();   // start

  smbWait(1);
  smbClock( 0 );
}

/*********************************************************************
 * @fn      smbStop
 * @brief   Terminates SM-Bus communication. Waits unitl the data line
 *          is low and the clock line is high. Then sets the data line
 *          high, keeping the clock line high to stop I/O.
 * @param   void
 * @return  void
 */
void smbStop( void )
{
  // Wait for clock high and data low
  smbClock(0);
  OCM_DATA_LOW();  // force low with SCL low
  smbWait(1);

  smbClock( 1 );
  OCM_DATA_HIGH(); // stop condition
  smbWait(1);

}

/*********************************************************************
 * @fn      smbWait
 * @brief   Wastes a an amount of time.
 * @param   count: down count in busy-wait
 * @return  void
 */
void smbWait( uint8 count )
{
  while ( count-- );
}

/*********************************************************************
 * @fn      smbReceiveByte
 * @brief   Read the 8 data bits.
 * @param   void
 * @return  character read
 */
uint8 smbReceiveByte()
{
  sint8 i, rval = 0;

  for (i=7; i>=0; --i)  {
    if (smbRead())  {
      rval |= 1<<i;
    }
  }

  return rval;
}
/**************************************************************************************************
**************************************************************************************************/
/*********************************************************************
 * @fn      smbReceive
 * @brief   reads data into a buffer
 * @param   address: linear address on part from which to read
 * @param   buffer: target array for read characters
 * @param   len: max number of characters to read
 * @return  void
 */
void smbReceive( uint32 address, uint8 *buffer, uint16 len )
{
  uint8  ch;
  uint16 i;

  if (!len)  {
    return;
  }

  smbSendDeviceAddress(address);

  ch = OCM_ADDRESS_BYTE(0, OCM_READ);
  smbSend(&ch, 1, SEND_START, NOSEND_STOP);

  for ( i = 0; i < len-1; i++ )
  {
    // SCL may be high. set SCL low. If SDA goes high when input
    // mode is set the slave won't see a STOP
    smbClock(0);
    OCM_DATA_HIGH();

    buffer[i] = smbReceiveByte();
    smbWrite(SMB_ACK);           // write leaves SCL high
  }

  // condition SDA one more time...
  smbClock(0);
  OCM_DATA_HIGH();
  buffer[i] = smbReceiveByte();
  smbWrite(SMB_NAK);

  smbStop();
}

/*********************************************************************
 * @fn      smbRead
 * @brief   Toggle the clock line to let the slave set the data line.
 *          Then read the data line.
 * @param   void
 * @return  TRUE if bit read is 1 else FALSE
 */
BOOL smbRead( void )
{
  // SCL low to let slave set SDA. SCL high for SDA
  // valid and then get bit
  smbClock( 0 );
  smbWait(1);
  smbClock( 1 );
  smbWait(1);

  return OCM_SDA;
}

/*********************************************************************
 * @fn      smbSendDeviceAddress
 * @brief   Send onlythe device address. Do ack polling
 *
 * @param   void
 * @return  none
 */
void smbSendDeviceAddress(uint32 address)
{
  uint8 addr[2];

  addr[0] = (address>>8) & 0xFF;
  addr[1] = address & 0xFF;

  // do ack polling...
  do  {
    smbStart();
  } while (!smbSendByte(OCM_ADDRESS_BYTE(address, OCM_WRITE)));

  // OK. send memory address
  smbSend(addr, 2, NOSEND_START, NOSEND_STOP);
}



sint8 SMBReceive(uint32 address, uint8 *buf, uint16 len)
{
  smbReceive(address, buf, len);

  return 0;
}

sint8 SMBSend(uint32 address, uint8 *buf, uint16 len)
{
  uint16 rollover = (uint16)address;
  uint16  locLen;

  // The AT24C1024 writes 256 byte 'pages'. If the starting address
  // plus the number of bytes to write exceed the 256 byte page boundary
  // the address will wrap and overwrite from the beginning of the page.
  // Presumably this is not what is intended under this scenario. We
  // have to deal with rollover explicitly. If it occurs we need to
  // re-address the target byte so we need to stop and re-start.
  do  {
    if (((rollover+len-1)&0xFF00) != (rollover&0xFF00))  {
      // We're going to exceed the current page. Write only
      // enough bytes to fill the current page.
      locLen = 0x100 - (address & 0xFF);
    }
    else  {
      locLen = len;
    }
    // begin the write sequence with the address byte
    smbSendDeviceAddress(address);
    smbSend(buf, locLen, NOSEND_START, SEND_STOP);
    len      -= locLen;
    address  += locLen;
    rollover += locLen;
    buf      += locLen;
  } while (len);

  return 0;
}





