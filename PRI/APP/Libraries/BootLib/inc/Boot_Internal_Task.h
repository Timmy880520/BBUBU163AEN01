/**************************************************************************************************
File Name: Boot_Internal_Task.h
Global Data:
    Name                      Type               Description
    ------------------------- ----------------- --------------------------------------------------
    None
Description: 
    Header file of Boot_Internal_Task.c.

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    05/06/2019 Watch Lee        1. support version.1.0.
    11/08/2019 Watch Lee        1. support version.1.1.
    01/08/2020 Watch Lee        1. support c28x_Boot_Internal_Task.
    11/12/2020 Watch Lee        1. support version.1.2.
	04/29/2021 Watch Lee        1. support version.1.2.1.

**************************************************************************************************/

#ifndef __BOOT_INTERNAL_TASK_H__
#define __BOOT_INTERNAL_TASK_H__

/* Includes */ 
#include "c28x_data_structure.h"

/* Macro definitions */
/*
Boot buffer size definition - Size MUST be power of 2 (2^n) and cannot set over 128 byte.
Besides, If the size less than 64 byte, the boot task cannot support the transport mode
when received image without Mix Header.
*/
#define Boot_Buffer_Size        64                                // Byte

// Boot download message size MUST be power of 2 (2^n) and less than Boot_Buffer_Size
#define Boot_Download_Size      (long)(Boot_Buffer_Size >> 3)     // Byte

// Boot header block size = header size / buffer size
#define Boot_Header_Block_Size  (long)(sizeof(StdMixHeaderType) / Boot_Buffer_Size)

// Statement Replacement
#ifndef Package_Checksum_Type
#define Package_Checksum_Type               unsigned long(*)(unsigned long, void*, unsigned long)
#endif

#ifndef Check_Power_2
#define Check_Power_2(Number)               (((((unsigned)Number - 1) ^ Number) == (Number << 1) - 1) ? Number : -1)
#endif

#ifndef Check_Download
#define Check_Download(Number)              ((Check_Power_2(Number) <= Boot_Buffer_Size) ? Check_Power_2(Number) : -1)
#endif 

// Group of declarations definition, the size MUST be power of 2 (2^n) and less than Boot_Buffer_Size
#define Create_Boot_Internal_Object(ObjName, \
                                    FunctionId,\
                                    Size)           unsigned char ObjName##Download[Check_Download(Size)];\
                                                    BootInternalObj ObjName = \
                                                    {\
                                                        {\
                                                            {0, 0, 0, 0, 0, 0, 0, 0, 0, {0}},\
                                                            {0, 0, 0, 0, 0, {ObjName##Download, Size}}, \
                                                            0, \
                                                            {0}, \
                                                            {0}, \
                                                            0, \
                                                            {FunctionId}, \
                                                            {(Package_Checksum_Type)Get_Package_CRC32, 0xFFFFFFFF}\
                                                        }, \
                                                        {\
                                                            Boot_Transport, \
                                                            Boot_Program_Flash_Bank1\
                                                        }, \
                                                        {\
                                                            Boot_Internal_State_Init,\
                                                            Boot_Internal_State_Machine,\
                                                            Boot_Transport_Init,\
                                                            Boot_Transport_State_Machine\
                                                        }\
                                                    }

// Verification                                                   
#if ((((Boot_Buffer_Size - 1) ^ Boot_Buffer_Size) == ((Boot_Buffer_Size << 1) - 1))==0)
#error "Boot buffer size MUST be power of 2 (2^n)!"
#endif  

#if (Boot_Buffer_Size > 128)
#error "Boot buffer size CAN NOT set over 128 byte!"
#endif  

/* Type definitions */
// Enumeration
typedef enum
{
    RESET_BOOT_MODE,
    GET_MIX_HEADER,
    GET_APP_HEADER,
    PROGRAM_MODE,
    TRANSPORT_MODE
}EnumBootMode;

typedef enum
{
    BOOT_NONE_COMMAND,
    BOOT_FINISHED,
    BOOT_RECOVERY,
    BOOT_PROTECTION = 0xF0,
    BOOT_INIT       = 0xF1,
    BOOT_DOWNLOAD   = 0xF2,
    BOOT_COMPLETED  = 0xF3,
    BOOT_STATE      = 0xF4
}EnumBootCommand;

typedef enum
{
    BOOT_LOCK_KEY     = 0xFFFF,
    BOOT_UNLOCK_KEY   = 0x5555,
    BOOT_INIT_KEY     = 0x55AA,
    BOOT_COMPLETE_KEY = 0xAAAA,
    BOOT_RECOVER_KEY  = 0xCCCC
}EnumBootKey;

typedef enum
{
    BOOT_LOCKED,
    BOOT_UNLOCKED,
    BOOT_ERROR,
    BOOT_TRANSPORT_ERROR,
    BOOT_RECOVERING,
    BOOT_INITIATING
}EnumBootState;

typedef enum
{
    PREPARING,
    READY,
    PROCESSING,
    VERIFYING,
    VERIFIED    
}EnumSubState;

typedef enum
{
    HEADER_ERROR,
    CHECKSUM_ERROR,
    SIZE_ERROR,
    FLASH_ERROR,
    UNKNOWN_ERROR   = 0xFD,
    PENDING         = 0xFE,
    NO_ERROR        = 0xFF
}EnumErrorCode;

// Struct & Union
typedef struct
{
    unsigned char headerNumber;
    unsigned char mode;
    unsigned char transportEnable;
    unsigned char programEnable;
    unsigned long accumulation;
    unsigned long startBlock;
    unsigned long blockOffset;
    unsigned long byteCount;
    unsigned long blockCount;
    unsigned char buffer[Boot_Buffer_Size];
}BootArgumentType;

typedef union
{
    unsigned short stateCode;
    
    struct
    {
        unsigned short subState     :8;
        unsigned short bootState    :8;
    }byte;
}BootStateType;

typedef struct
{
    unsigned short protectKey;                  // Data of Boot Protection command
    unsigned short initKey;                     // Data of Boot Init command
    unsigned short reloaded;                    /* Reload flag. State machine will execute the download process when 
                                                   the flag is set by software setting. */
    unsigned short completeKey;                 // Data of Boot Completed command
    BootStateType state;                        // Data of Boot State command
    ArrayType download;                         // Data of Boot download command
}BootDataType;

typedef union
{
    unsigned char all;
    
    struct
    {
        unsigned char protect       :1;
        unsigned char init          :1;
        unsigned char download      :1;       
        unsigned char state         :1;
        unsigned char complete      :1;        
        unsigned char failed        :1;
        unsigned char recovery      :1;        
        unsigned char reserved      :1;
    }bit;
}BootTransportFlag;

typedef struct BootInternalStr BootInternalObj;  

struct BootInternalStr
{
    struct
    {
        BootArgumentType argument;          // Private variable for internal access
        BootDataType message;               // Command data for boot protocol 
        StdAppHeaderType *romHeaderPtr;     // Pointer of app header, update it to point app header address of ROM
        StdAppHeaderType appHeader;         // Private app header information for internal access  
        StdMixHeaderType mixHeader;         // Private mix header information for internal access 
        BootTransportFlag transportFlag;    // Transport mode state flag
        StringWordType ownFunctionId;       // Function code for image data identification
            
        struct
        {    
            unsigned long (*calculate)(unsigned long, void*, unsigned long);    // Checksum calculation function
            unsigned long seed;                                                 // Initial value
        }checksum;        
    }member;
    
    struct 
    {
        EnumErrorCode (*transport)(BootInternalObj*, ArrayType, unsigned char); // Transport download data method
        unsigned char (*program)(unsigned long, void*, unsigned long);          // Program download data method
    }method; 
    
    struct 
    {
        EnumBootState (*init)(BootInternalObj*, unsigned char,
                              unsigned long, unsigned long);                    // Initialization state machine
        EnumBootCommand (*stateMachine)(BootInternalObj*, BootInternalObj*);    // State machine function
        void (*transportInit)(BootInternalObj*);                                // Initialization transport state
        EnumBootCommand (*transportState)(BootInternalObj*, unsigned char);     // Transport state machine function
    }func; 
};

/* Global function prototypes */
EnumBootState Boot_Internal_State_Init(BootInternalObj *obj, unsigned char waiveHeader,
                                       unsigned long headerStart, unsigned long appRomSize);
EnumBootState Boot_Internal_Single_Step_Init(BootInternalObj *obj, unsigned char waiveHeader,
                                             unsigned long headerStart, unsigned long appRomSize);
EnumBootCommand Boot_Internal_State_Machine(BootInternalObj *obj, BootInternalObj *transportObj);
EnumErrorCode Boot_Transport(BootInternalObj *obj, ArrayType data, unsigned char finish);
void Boot_Transport_Init(BootInternalObj *obj);
EnumBootCommand Boot_Transport_State_Machine(BootInternalObj *obj, unsigned char ack);

unsigned long Get_Package_CRC16(unsigned long crcValue, void *dataPtr, unsigned long size);
unsigned long Get_Package_CRC32(unsigned long crcValue, void *dataPtr, unsigned long size);

/* Global data declarations */

#endif
