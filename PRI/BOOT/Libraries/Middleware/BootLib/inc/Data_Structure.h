/**************************************************************************************************
File Name: Data_Structure.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ -----------------------------------------------------
    None
Description: 
    Universal data structure definition file.

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    07/20/2018 Watch Lee        1. version.1.0.
    03/04/2019 Watch Lee        1. version.2.0.
                                2. add standard header type.
                                3. modify header type.
                                4. remove old types.
    05/06/2019 Watch Lee        1. version.2.1.
                                2. modify header type.
                                4. Add ArrayType.

**************************************************************************************************/

#ifndef __Data_Structure_H__
#define __Data_Structure_H__

/* Type definitions */
// Struct & Union
typedef union
{
    struct
    {
        unsigned short status;
        unsigned char busOffCnt;
        unsigned char rxOverrunCnt;
        unsigned char rxErrorStateCnt;
        unsigned char txErrorStateCnt;
        unsigned char rxQueueFullCnt;
        unsigned char txQueueFullCnt;
    };
    unsigned long word[2];
    unsigned char byte[8];
}ComStatusType;

typedef union
{
    struct
    {
        unsigned short max;
        unsigned short min;
    };
    unsigned long word;
}LimitType;

typedef struct
{
    unsigned long flag;
    unsigned long zero;
    unsigned short *dataPtr;
    LimitType *limitPtr;
}DataCheckType;

typedef union
{
    struct
    {
        short offset;
        short gain;
    };
    unsigned long word;
}ScaleType;

typedef union
{
    unsigned long word[2];
    unsigned short halfWord[4];
    unsigned char byte[8];	
}DoubleWordType;

typedef union
{	
    unsigned long word;
    unsigned short halfWord[2];
    unsigned char byte[4];
}WordType;

typedef union
{	
    unsigned char byte[4];
    unsigned long word;   
}StringWordType;

typedef union
{	
    unsigned short halfWord;
    unsigned char byte[2];
}HalfWordType;

typedef struct
{
    unsigned char length;
    WordType packet[2];
}DataPackType;

typedef union
{
    struct ArrayStr
    {
        unsigned char *array;
        unsigned long size;    
    }member;
    
    unsigned char *byte;    
}ArrayType;

typedef struct
{
    unsigned char *ramMap;
    unsigned char *romMap;
    unsigned short length;
}DataMapType;

typedef struct
{
    unsigned char moduleName[20];           // Byte[0:19]
    unsigned char programType[6];           // Byte[20:25]
    unsigned char customerCode[4];          // Byte[26:29]
    unsigned short swCompatibilityIndex;    // Byte[30:31]
    StringWordType programFunction;         // Byte[32:35]
    unsigned long programSize;              // Byte[36:39]
    unsigned short padSize;                 // Byte[40:41]
    unsigned char programVersion[10];       // Byte[42:51]
}StdHeaderMainType;

typedef struct
{
    StringWordType programFunction;         // Byte[0:3]
    unsigned long programSize;              // Byte[4:7]
    unsigned short padSize;                 // Byte[8:9]
    unsigned char programVersion[10];       // Byte[10:19]  
}StdHeaderSubType;

typedef struct
{
    struct
    {
        WordType text;
        unsigned long headerCrc;
        unsigned long imageCrc;
    }signature;
    
    StdHeaderMainType header; 
    unsigned long compliantTableAddress;
    unsigned char reserved[188];
}StdAppHeaderType;

typedef struct
{
    struct
    {
        WordType text;
        unsigned long headerCrc;
        unsigned long imageCrc;
        unsigned long imageSize;
    }signature;
    
    StdHeaderMainType firstIndex;
    StdHeaderSubType secondIndex;
    StdHeaderSubType thirdIndex;
    StdHeaderSubType fourthIndex;
    StdHeaderSubType fifthIndex;    
    unsigned char reserved[108];
}StdMixHeaderType;

typedef struct
{
    unsigned char main[9];
    unsigned char sub[3];
}CodeVersionType;

#endif
