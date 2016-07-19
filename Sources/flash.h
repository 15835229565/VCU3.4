#ifndef __FLASH_H__
#define __FLASH_H__

#include "typedefs.h"
#define OSC_FRQ_MHZ 16 /*[MHz]*/
#define BUS_FRQ_MHZ 24 /*[MHz]*/

#define P_PAGE_SIZE                 0x4000u /*16K Byte*/
#define P_PAGE_START_ADDR           0x8000u
#define P_PAGE_END_ADDR             0xBFFFu
#define P_PAGE_ADDR_MASK            0x0000FFFFul
#define P_PAGE_TYPE                 0u
#define P_FLASH_SECTOR_SIZE         512u
#define P_FLASH_PROG_ALINE          0xFFFFFFF8ul /*global address and program length [2..0] = 0*/
#define P_FLASH_PROG_BASE_WD_SIZE   4u      /*one phrase, 64 bits*/
#define P_FLASH_PAGE_SHIFT_BIT      14u

#define P_FLASH_BLOCK_0_START_ADDR  0x7F0000u
#define P_FLASH_BLOCK_0_END_ADDR    0x7FFFFFu
#define P_FLASH_BLOCK_1_START_ADDR  0x780000u
#define P_FLASH_BLOCK_1_END_ADDR    0x78FFFFu

#define FLASH_CLK_DIVIDER           (BUS_FRQ_MHZ - 1)
#define FLASH_SECURITY_ENABLED      2u
#define FLASH_SECURITY_UNSECURED    2u

#define FLASH_START_ADDR            0x00020000ul
#define FLASH_END_ADDR              0x0003FFFFul
#define FLASH_CONFIG_ADDR           0x0003FF00ul
#define FLASH_ACCESS_KEY_ADDR       FLASH_CONFIG_ADDR
#define FLASH_ACCESS_KEY_LEN        8
#define FLASH_PROTECT_ADDR          0x0003FF0Cul
#define EEPROM_PROTECT_ADDR         0x0003FF0Dul
#define OPT_PROTECT_ADDR            0x0003FF0Dul
#define SECURITY_PROTECT_ADDR       0x0003FF0Ful

#define INEEPROM_START_ADDR         0x0400u
#define INEEPROM_END_ADDR           0x13FFu /* 4 KByte */
#define INEEPROM_SECTOR_SIZE        4u      /* 4 Bytes */
#define INEEPROM_BASE_ADDR_MASK     0xFFFCu

#define FLASH_CMD_START             0x80u
#define FLASH_CMD_COMPLETE          0x80u

/*FCMD high byte used*/
#define FCMD_ERASE_VERIFY_ALL       0x0100u
#define FCMD_ERASE_VERIFY_BLOCK     0x0200u
#define FCMD_ERASE_VERIFY_P_SECTION 0x0300u
#define FCMD_READ_ONCE              0x0400u
#define FCMD_PROG_P_FLASH           0x0600u
#define FCMD_PROG_ONCE              0x0700u
#define FCMD_ERASE_ALL              0x0800u
#define FCMD_ERASE_P_BLOCK          0x0900u
#define FCMD_ERASE_P_SECTOR         0x0A00u
#define FCMD_UNSECURE_FLASH         0x0B00u
#define FCMD_VERIFY_ACCESS_KEY      0x0C00u
#define FCMD_SET_USER_MARGIN_LEVER  0x0D00u
#define FCMD_SET_FIELD_MARGIN_LEVER 0x0E00u
#define FCMD_FULL_PARTITION_D_FLASH 0x0F00u /* IN-EEPROM */
#define FCMD_ERASE_VERIFY_D_SECTION 0x1000u
#define FCMD_PROG_D_FLASH           0x1100u
#define FCMD_ERASE_D_SECTOR         0x1200u
#define INPUT_DATA_NUM              72
#define INPUT_CONTROL_NUM           64

#define APP_VALID_FLAG_ADDR             0x400        //bootloader标志位地址该位置如果改变需要同步改变bootloader程序里面的地址
#define CAR_PARA_START_ADDRESS          0x404                
#define CAR_PARA_EEPROM_SIZE            80               //80个字节空间由于EEPROM以4字节为一个操作单位，所以长度需要设置成4的倍数

#define CAR_THRESHOLD_START_ADDRESS     (CAR_PARA_START_ADDRESS+CAR_PARA_EEPROM_SIZE)               
#define CAR_THRESHOLD_EEPROM_SIZE       64              //64个字节空间 这个值过大有可能导致程序 stack溢出


/**************************/
extern uint8 gCarParaEepromUpdateFlag;   //车辆参数EEPROM更新标志位 1表示需要更新 0表示不用更新
extern uint8 gCarThresholdEepromUpdateFlag;   //车辆阈值EEPROM更新标志位 1表示需要更新 0表示不用更新




/*******function***********/
void flash_init(void);
bool ineeprom_write(uint8_t *p_data_buffer, uint16_t phy_start_addr, uint16_t length);
bool ineeprom_read(uint8_t *p_data_buffer, uint16_t phy_start_addr, uint16_t length);

void WriteCarPara();
void ReadCarPara();

void WriteCarThreshold();
void ReadCarThreshold();
void EEPROM_INITIAL(); 
uint8 MemEqual(uint8* buf1,uint8* buf2,uint8 length);    

void WriteBootFlag(uint8 mode);

/**************************外部EEPROM相关定义*************************/
#define OCM_ADDRESS  (0xA0)
#define OCM_READ     (0x01)
#define OCM_WRITE    (0x00)
#define SMB_ACK      (0)
#define SMB_NAK      (1)
#define SEND_STOP    (0)
#define NOSEND_STOP  (1)
#define SEND_START   (0)
#define NOSEND_START (1)
#define OCM_ADDRESS_BYTE(target,rw)  (OCM_ADDRESS | rw | ((target > 0xFFFF) ? 0x02 : 0x00))

#define OCM_SDA      EX_EE_SDA_IN
#define OCM_SCL      EX_EE_SCL_IN

#define OCM_DATA_HIGH()  EX_EE_SDA_INEN        //SDA设置为输入状态

#define OCM_DATA_LOW() {EX_EE_SDA_OUTEN; OCM_SDA = 0;} //SDA设置为输出状态，且输出低电平

#define OCM_SCL_HIGH()  EX_EE_SCL_INEN        //SCL设置为输入状态

#define OCM_SCL_LOW() {EX_EE_SCL_OUTEN; OCM_SCL = 0;} //SCL设置为输出状态，且输出低电平

void smbWait(uint8 count);
void   smbSend( uint8 *buffer, uint16 len, uint8 sendStart, uint8 sendStop );
BOOL  smbSendByte( uint8 dByte );
void   smbWrite( BOOL dBit );
void   smbClock( BOOL dir );
void   smbStart( void );
void   smbStop( void );
void   smbReceive( uint32 address, uint8 *buffer, uint16 len );
uint8  smbReceiveByte( void );
BOOL  smbRead( void );
void   smbSendDeviceAddress(uint32 address);

sint8 SMBReceive(uint32 address, uint8 *buf, uint16 len);
sint8 SMBSend(uint32 address, uint8 *buf, uint16 len);
#endif __FLASH_H__
