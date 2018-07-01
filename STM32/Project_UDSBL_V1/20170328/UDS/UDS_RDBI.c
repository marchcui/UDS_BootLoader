/*******************************************************************************
File - UDS_RDBI.c
UDS read data by ident.
Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: WenHui Wu
Date: Mar. 20th 2017
Version: V1.0.0
*******************************************************************************/
#include "UDS_Driver.h"
#include "UDS_Config.h"
#include "UDS_Primitive.h"
#include "eeprom.h"
#include "UDS_Const.h"

#if USE_RD_DATA_BYID

/**
* StringToBytesConverter
*
* @param pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         RSP_TOOLONG - responseTooLong
* @date Mar. 23th 2017
*/
U8 StringToBytesConverter(U8 *pData, U16 *uwLen, const char *str)
{
  U16 i;
  U16 len = strlen(str);
  *uwLen = len+3;
  if(*uwLen>*(uwLen-1)) /* >RX:uwDataMax */
    return RSP_TOOLONG;
  for(i=0; i<len; i++)
  {
    pData[3+i] = *(str+i);
  }
  return POS_RSP;
}

/**
* Find_Rdbi
*
* @param RdbiIdent - Rdbi Identifier.
*        errCode - Pointer to errCode.
* @return Pointer of type <RDBI_T> pointing to found object, NULL if not found. 
* @date Jan. 18th 2017
*/
UDS_CONST_STORAGE RDBI_t *Find_Rdbi(U16 RdbiIdent, U8 *errCode)
{
  UDS_CONST_STORAGE RDBI_t *Rdbi = UDS_NULL;
  U16 i;
  for(i=0; i<RDBI_MAX; i++)
  {
    if(UDS_ROM.Rdbi[i].Ident == RdbiIdent)
    {
      Rdbi = &UDS_ROM.Rdbi[i];
      break;
    }
  }
  if(Rdbi)
  {
    *errCode = FIND_ERR_NO;
    /* Grant the right to access the Service */
    if(UDS_RAM.Session==DFLT_SESSN && (Rdbi->Access&ACCS_DFLT))
    {
    }
    else if(UDS_RAM.Session==ECU_PROG_SESSN && (Rdbi->Access&ACCS_PROG))
    {
    }
    else if(UDS_RAM.Session==ECU_EXT_DIAG_SESSN && (Rdbi->Access&ACCS_EXT_DIAG))
    {
    }
    else
    {
      /* conditionsNotCorrect */
      *errCode = FIND_ERR_COND;
      return UDS_NULL;
    }
  }
  else
  {
    *errCode = FIND_ERR_NULL;
  }
  return Rdbi;
}

/**
* RdbiFingerprint
*
* @param pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         RSP_TOOLONG - responseTooLong
* @date Mar. 20th 2017
*/
#if USE_RDBI_FINGERPRINT
U8 RdbiFingerprint(U8 *pData, U16 *uwLen)
{
  U8 i;
  *uwLen = 22;
  if(*uwLen>*(uwLen-1)) /* >RX:uwDataMax */
    return RSP_TOOLONG;
  /* Unlock the Flash Program Erase controller */
  __disable_irq();
  FLASH_Unlock();
  U16 uwData;
  /* #3 - programmingDate Year (BCD-coded) 
#4 - programmingDate Month (BCD-coded) 
#5 - programmingDate Date (BCD-coded) 
#6 - testerSerialNumber (Byte 1, ASCII)
#7 - testerSerialNumber (Byte 2, ASCII)
  ...
#21 - testerSerialNumber (Byte 16, ASCII) */
  for(i=0;i<10;i++)
  {
    EE_ReadVariable(FP_ADDR_START+i, &uwData);
    pData[i*2+3] = (U8)(uwData>>8);
    pData[i*2+4] = (U8)(uwData&0xFF);
  }
  FLASH_Lock();
  __enable_irq();
  return POS_RSP;
}
#endif /* USE_RDBI_FINGERPRINT */

/**
* RdbiEcuInfo
*
* @param pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         RSP_TOOLONG - responseTooLong
* @date Mar. 20th 2017
*/
#if USE_RDBI_ECUINFO
U8 RdbiEcuInfo(U8 *pData, U16 *uwLen)
{
  U8 i;
  *uwLen = 22;
  if(*uwLen>*(uwLen-1)) /* >RX:uwDataMax */
    return RSP_TOOLONG;
  for(i=0;i<4;i++)
  {
    pData[i+3] = i;
  }
  for(i=0;i<3;i++)
  {
    pData[i+7] = 0x99;
  }
  for(i=0;i<12;i++)
  {
    pData[i+10] = i;
  }
  return POS_RSP;
}
#endif /* USE_RDBI_ECUINFO */

/**
* RdbiEcuHW
*
* @param pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         RSP_TOOLONG - responseTooLong
* @date Mar. 20th 2017
*/
#if USE_RDBI_ECUHW
U8 RdbiEcuHW(U8 *pData, U16 *uwLen)
{
  return StringToBytesConverter(pData, uwLen, STR_MACRO(S_ECU_HW));
}
#endif /* USE_RDBI_ECUHW */

/**
* RdbiEcuSW
*
* @param pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         RSP_TOOLONG - responseTooLong
* @date Mar. 20th 2017
*/
#if USE_RDBI_ECUSW
U8 RdbiEcuSW(U8 *pData, U16 *uwLen)
{
  return StringToBytesConverter(pData, uwLen, STR_MACRO(S_ECU_SW));
}
#endif /* USE_RDBI_ECUSW */

#endif /* USE_RD_DATA_BYID */
