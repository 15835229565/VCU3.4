/*
* Copyright (c) 2010,SHENZHEN HANGSHENG ELECTRONICS Co.,Ltd.
* All Rights Reserved.
* Dept.:ACEC
* File:typedefs.h
* Description: Common type define
* REQ IDs: TPMS_SW_COM_0002, TPMS_SW_COM_0003, TPMS_SW_COM_0004, TPMS_SW_COM_0005, TPMS_SW_BASE_0011
* History:
* 2011-10-8, 1007040, original
*/
 
#ifndef __TYPEDES_H__
#define __TYPEDES_H__

#include "stddef.h"
 
#ifdef __cplusplus
extern "C" {
#endif


typedef void (*PROC)(void);
/*!< Parameterless function pointer (Procedure variable) */

/*REQ IDs: TPMS_SW_COM_0002*/
#if defined(__CHAR_IS_8BIT__)
  typedef  unsigned char      uint8_t;
  typedef    signed char      int8_t;
#elif defined(__SHORT_IS_8BIT__)
  typedef  unsigned short     uint8_t;
  typedef    signed short     int8_t;
#elif defined(__INT_IS_8BIT__)
  typedef  uint16       uint8_t;
  typedef    signed int       int8_t;
#elif defined(__LONG_IS_8BIT__)
  typedef  unsigned long      uint8_t;
  typedef    signed long      int8_t;
#elif defined(__LONG_LONG_IS_8BIT__)
  typedef  unsigned long long uint8_t;
  typedef    signed long long int8_t;
#else /* default */
  typedef  uint8      uint8_t;
  typedef    signed char      int8_t;
#endif
/*! \typedef uint8 
    uint8 is a unsigned integral 8 bit type (typically uint8) */
/*! \typedef suint8  
    suint8 is a signed integral 8 bit type (typically signed char)  */


#if defined(__CHAR_IS_16BIT__)
  typedef  uint8      uint16_t;
  typedef    signed char      int16_t;
#elif defined(__SHORT_IS_16BIT__) && !defined(__INT_IS_16BIT__)
  typedef  unsigned short     uint16_t;
  typedef    signed short     int16_t;
#elif defined(__INT_IS_16BIT__)
  typedef  unsigned int       uint16_t;
  typedef    signed int       int16_t;
#elif defined(__LONG_IS_16BIT__)
  typedef  unsigned long      uint16_t;
  typedef    signed long      int16_t;
#elif defined(__LONG_LONG_IS_16BIT__)
  typedef  unsigned long long uint16_t;
  typedef    signed long long int16_t;
#else
  typedef  unsigned short     uint16_t;
  typedef    signed short     int16_t;
#endif
/*! \typedef Word 
    Word is a unsigned integral 16 bit type (typically unsigned short) */
/*! \typedef sWord  
    sWord is a signed integral 16 bit type (typically signed short)  */

#if defined(__CHAR_IS_32BIT__)
  typedef   unsigned char      uint32_t;
  typedef    signed char      int32_t;
#elif defined(__SHORT_IS_32BIT__)
  typedef  unsigned short     uint32_t;
  typedef    signed short     int32_t;
#elif defined(__INT_IS_32BIT__)
  typedef  unsigned int       uint32_t;
  typedef    signed int       int32_t;
#elif defined(__LONG_IS_32BIT__)
  typedef  unsigned long      uint32_t;
  typedef    signed long      int32_t;
#elif defined(__LONG_LONG_IS_32BIT__)
  typedef  unsigned long long uint32_t;
  typedef    signed long long int32_t;
#else /* default */
  typedef  unsigned long uint32_t;
  typedef    signed long int32_t;
#endif
/*! \typedef LWord 
    LWord is a unsigned integral 32 bit type (typically uint16 or unsigned long) */
/*! \typedef sLWord  
    sLWord is a signed integral 32 bit type (typically signed int or signed long)  */

typedef  unsigned char      uchar;
  /*!< Definition for an uint8. */
typedef  unsigned int       uint;
  /*!< Definition for an uint16. */
typedef  unsigned long      ulong;
  /*!< Definition for an unsigned long. */
#ifdef __CAN_HANDLE_LONG_LONG__
typedef  unsigned long long ullong;
  /*!< Definition for an unsigned long long. */
#endif

typedef  signed char        schar;
  /*!< Definition for an signed char. */
typedef  signed int         sint;
  /*!< Definition for an signed int. */
typedef  signed long        slong;
  /*!< Definition for an signed long. */
#ifdef __CAN_HANDLE_LONG_LONG__
typedef  signed long long   sllong;
  /*!< Definition for an signed long long. */
#endif

#ifndef bool
typedef unsigned char bool;
#endif

#ifndef BOOL
#define BOOL bool /*REQ IDs: TPMS_SW_COM_0003*/
#endif

#define true    (1u)
#define false   (0u)

/*REQ IDs: TPMS_SW_COM_0004*/
/*!< Definition for boolean type. */
#ifdef __MISRA__ /* MISRA rule #18 */
  #define TRUE  1u
    /*!< Definitioni for TRUE. */
  #define FALSE 0u
    /*!< Definition for FALSE. */
#else
  #define TRUE  1
    /*!< Definitioni for TRUE. */
  #define FALSE 0
    /*!< Definition for FALSE. */
#endif

#ifndef true
#define true    1
#endif
#ifndef false
#define false   0
#endif

#ifndef NULL
#define NULL 0 /*REQ IDs: TPMS_SW_COM_0005*/
#endif

#ifndef null
#define null 0
#endif

#define UINT8_T_MAX   (0xFF)
#define INT8_T_MAX    (0x7F)
#define UINT16_T_MAX  (0xFFFF)
#define INT16_T_MAX   (0x7FFF)
#define UINT32_T_MAX  (0xFFFFFFFF)
#define INT32_T_MAX   (0x7FFFFFFF)
#define CAL const volatile

typedef enum __IO_ERR_T__
{
  IO_ERR_OK = 0,
  IO_ERR_PARAMETER_ERROR,
  IO_ERR_BUSY,
  IO_ERR_RUN_ERROR,
  IO_ERR_OTHER_FAILURE  
}io_err_t; /*REQ IDs: TPMS_SW_BASE_0011*/

#ifdef __cplusplus
}
#endif

typedef unsigned int word;
typedef unsigned long dword;
typedef unsigned long dlong[2];
typedef signed char     sint8;
typedef unsigned char     uint8;
typedef signed int    sint16;
typedef unsigned int  uint16;
typedef signed long     sint32;
typedef unsigned long   uint32;    
typedef double    FP32;

#endif /*end if __TYPEDES_H__*/

