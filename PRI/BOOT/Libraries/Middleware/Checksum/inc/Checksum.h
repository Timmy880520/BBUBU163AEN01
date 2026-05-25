/**************************************************************************************************
File Name: Checksum.h
Global Data:
    Name                      Type               Description
    ------------------------- ----------------- --------------------------------------------------
    None
Description: 
    Header file of checksum algorithm files. this header file includes functions of c files below:
1. ZeroSum
2. CalcuCRC8
3. CalcuCRC16
4. CalcuCRC32

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    05/14/2021 Watch Lee        1. version.1.0.

**************************************************************************************************/

#ifndef __CHECKSUM_H__
#define __CHECKSUM_H__

/* Includes */

/* Global function prototypes */
// ZeroSum
unsigned long Calculate_Zero_Checksum(unsigned long checksum, unsigned char data);

// CalcuCRC8
unsigned long Get_Package_CRC8(unsigned long crcValue, void *dataPtr, unsigned long size);
unsigned long Calculate_CRC8(unsigned long crcValue, unsigned char data);

// CalcuCRC16
unsigned long Get_Package_CRC16(unsigned long crcValue, void *dataPtr, unsigned long size);
unsigned long Calculate_CRC16(unsigned long crcValue, unsigned char data);

// CalcuCRC32
unsigned long Get_Package_CRC32(unsigned long crcValue, void *dataPtr, unsigned long size);
unsigned long Calculate_CRC32(unsigned long crcValue, unsigned char data);

#endif
