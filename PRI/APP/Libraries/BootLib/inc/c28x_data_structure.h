/**************************************************************************************************
File Name: c28x_data_structure.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ -----------------------------------------------------
    None
Description: 
    TI C28x DSP universal data structure definition file.

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    12/30/2019 Watch Lee        1. version.1.0.

**************************************************************************************************/

#ifndef __Data_Structure_H__
#define __Data_Structure_H__

/* Type definitions */
// Struct & Union
typedef struct
{
    unsigned char low   :8;
    unsigned char high  :8;
}TwoByteType;

typedef union
{	
    unsigned long word;
    unsigned short halfWord[2];
    TwoByteType byte[2];
}WordType;

typedef union
{	
    unsigned long long word;
    unsigned char byte[4];
}StringWordType;

typedef union
{
    unsigned long word;
    TwoByteType byte[2];
}BinStringWordType;

typedef union
{	
    unsigned short halfWord;
    TwoByteType byte;
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
    TwoByteType moduleName[10];                 // TiByte[0:9]     = 20
    TwoByteType programType[3];                 // TiByte[10:12]   = 6
    TwoByteType customerCode[2];                // TiByte[13:14]   = 4
    unsigned short swCompatibilityIndex;        // TiByte[15]      = 2
    BinStringWordType programFunction;          // TiByte[16:17]   = 4
    unsigned long programSize;                  // TiByte[18:19]   = 4
    unsigned short padSize;                     // TiByte[20]      = 2
    TwoByteType programVersion[5];              // TiByte[21:25]   = 10
}BinHeaderMainType;                             // Total 26 TiByte = 52 Byte

typedef struct
{
    BinStringWordType programFunction;          // TiByte[0:1]
    unsigned long programSize;                  // TiByte[2:3]
    unsigned short padSize;                     // TiByte[4]
    TwoByteType programVersion[5];              // TiByte[5:9]
}BinHeaderSubType;                              // Total 10 TiByte = 20 Byte

typedef struct
{
    struct
    {
        WordType text;                          // TiByte[0:1]
        unsigned long headerCrc;                // TiByte[2:3]
        unsigned long imageCrc;                 // TiByte[4:5]
    }signature;

    BinHeaderMainType header;                   // TiByte[6:31]
    unsigned long compliantTableAddress;        // TiByte[32:33]
    unsigned char reserved[94];                 // TiByte[34:127]
}BinAppHeaderType;                              // Total 128 TiByte = 256 Byte

typedef struct
{
    unsigned char moduleName[20];               // TiByte[0:19]
    unsigned char programType[6];               // TiByte[20:25]
    unsigned char customerCode[4];              // TiByte[26:29]
    unsigned long swCompatibilityIndex;         // TiByte[30:31]
    StringWordType programFunction;             // TiByte[32:35]
    unsigned long long programSize;             // TiByte[36:39]
    unsigned long padSize;                      // TiByte[40:41]
    unsigned char programVersion[10];           // TiByte[42:51]
}StdHeaderMainType;                             // Total 52 TiByte

typedef struct
{
    StringWordType programFunction;             // TiByte[0:3]
    unsigned long long programSize;             // TiByte[4:7]
    unsigned long padSize;                      // TiByte[8:9]
    unsigned char programVersion[10];           // TiByte[10:19]
}StdHeaderSubType;                              // Total 20 TiByte

typedef struct
{
    struct
    {
        StringWordType text;                    // TiByte[0:3]
        unsigned long long headerCrc;           // TiByte[4:7]
        unsigned long long imageCrc;            // TiByte[8:11]
    }signature;
    
    StdHeaderMainType header;                   // TiByte[12:63]
    unsigned long long compliantTableAddress;   // TiByte[64:57]
    unsigned char reserved[188];                // TiByte[68:255]
}StdAppHeaderType;                              // Total 256 TiByte

typedef struct
{
    struct
    {
        StringWordType text;                    // TiByte[0:3]
        unsigned long long headerCrc;           // TiByte[4:7]
        unsigned long long imageCrc;            // TiByte[8:11]
        unsigned long long imageSize;           // TiByte[12:15]
    }signature;
    
    StdHeaderMainType firstIndex;               // TiByte[16:67]
    StdHeaderSubType secondIndex;               // TiByte[68:87]
    StdHeaderSubType thirdIndex;                // TiByte[88:107]
    StdHeaderSubType fourthIndex;               // TiByte[108:127]
    StdHeaderSubType fifthIndex;                // TiByte[128:147]
    unsigned char reserved[108];                // TiByte[148:255]
}StdMixHeaderType;                              // Total 256 TiByte

typedef struct
{
    unsigned char main[9];
    unsigned char sub[3];
}CodeVersionType;

#endif
