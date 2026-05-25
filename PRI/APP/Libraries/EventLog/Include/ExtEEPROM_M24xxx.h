/**************************************************************************************************
File Name: ExtEEPROM_M24xxx.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ -----------------------------------------------------
    storageTimeBase      TmrPkg             EEPROM access time base for timeout detection.
Description: 
    Header file of ExtEEPROM_M24xxx.c.

===================================================================================================
History:
    Date       Author           Description Of Change
    ---------- ---------------- -----------------------------------------------------------------
    03/12/2018 Watch Lee        1. support version 1.0. 
    08/15/2018 Watch Lee        1. version 2.0.
    09/17/2021 S.Y Lee          1. version 2.1
                                2. Support TI unsigned char length is 16 bits problem.
                                   Modify all data put in high byte of short variable.
**************************************************************************************************/

#ifndef __EXTEEPROM_M24XXX_H__
#define __EXTEEPROM_M24XXX_H__ 

/* Includes */
#include "Univ_Lib.h"
#include "Io.h"

#include "../../EventLog/Include/f28004x_i2c_driver.h"
/* Macro definitions */
#define Storage_Base          50                  // 1ms * 50kHz = 50
#define Storage_Count         250                 // 250ms
#define Default_Device_ID     0xA0                // Default device Select Code
#define Default_Page_Write    64                  // Default page write bytes, MUST be power of 2 (2^n)
#define Default_Address_Byte  TWO_ADDRESS_BYTE    // M24C01/02/04/08/16 = ONE_ADDRESS_BYTE, M24128 and M24256 = TWO_ADDRESS_BYTE

// Write protection pin definition
#define WRITE_PROTECT         Gpio_Pin(A,8)

#ifndef _Set_Default_WC
#define _Set_Default_WC       Set_Pin(WRITE_PROTECT)
#endif

#ifndef _Clear_Default_WC
#define _Clear_Default_WC     Clear_Pin(WRITE_PROTECT)
#endif

// Statement Replacement
#define I2C_Device_An(ID)                       ((0xFF - ID) & 0x6)

#ifndef Check_Power_2
#define Check_Power_2(Number)                   (Number * ((((unsigned)Number - 1) ^ Number) == ((Number << 1) - 1)))
#endif

// Group of declarations - Page_Write_Bytes MUST be power of 2 (2^n)
#define Create_EEPROM_Object(Name, \
                             ID, \
                             Address_Bytes, \
                             Page_Write_Bytes)  EepromObj Name = \
                                                {\
                                                    {\
                                                        ID, Address_Bytes, Check_Power_2(Page_Write_Bytes), \
                                                        0, 0, 0, 0, ENABLE_WRITE_CONTROL\
                                                    },\
                                                    {0, 0, 0, Pull_Low_Write_Control, Pull_High_Write_Control},\
                                                    {\
                                                        Read_External_EEPROM, \
                                                        Write_External_EEPROM, \
                                                        Reset_EEPROM_Object_Parameter\
                                                    }\
                                                }

//// Statement Replacement
//#define I2C_Device_An(ID)                       ((0xFF - ID) & 0x6)
//
//#ifndef Check_Power_2
//#define Check_Power_2(Number)                   (((Number - 1) ^ Number) == ((Number << 1) - 1))
//#endif

/* Type definitions */
// Enumeration
typedef enum
{
    ONE_ADDRESS_BYTE = 1,
    TWO_ADDRESS_BYTE = 2
}EnumAddressByte;

typedef enum
{
    DISABLE_WRITE_CONTROL = 0,
    ENABLE_WRITE_CONTROL  = 1     
}EnumWriteControl;

// Struct & Union
typedef struct
{
    unsigned short offset;
    unsigned char size;
    void *ptr;
}EepromPackType;

typedef struct EepromStr EepromObj;

struct EepromStr
{
    struct
    {
        unsigned char deviceId;         // Device Select Code
        EnumAddressByte addressBytes;   // Number of the address byte, M24C01/02/04/08/16 = 1, M24128 and M24256 = 2
        unsigned char pageWriteBytes;   // Maximum writable bytes in a single write cycle, MUST be power of 2 (2^n)
        unsigned char result;           // Result of EEPROM operation
        unsigned char txPageCount;      // Page write counter
        unsigned char busy;             // State of EEPROM operation
        void *driverObj;                // Pointer of the peripheral driver object
        EnumWriteControl writeControl;  // State of write control        
    }member;  
        
    struct 
    {
/*
        The driverRead and driverWrite function shall include six parameters and a return value. 
        The first argument is an address of the peripheral object. 
        The second argument is the device ID.
        The third argument pass to parameters of the reception function is an address of the offset. 
        The fourth argument is a number of byte of the offset. 
        The fifth arqument is an address of the data. 
        The sixth argument is a number of byte of the offset.
        The return value shall mean state of the read/write operation. 1 = completed, 0 = do not complete. 
*/        
        unsigned char (*driverRead)(void *obj, unsigned char id,    // Peripheral module operation function of reading  
                                    void *aPtr, unsigned char aSize, 
                                    void *dPtr, unsigned char dSize);  
        unsigned char (*driverWrite)(void *obj, unsigned char id,   // Peripheral module operation function of writing
                                     void *aPtr, unsigned char aSize, 
                                     void *dPtr, unsigned char dSize);
        void (*driverReset)();                                      // Peripheral module operation function of reset
        void (*enableWrite)();
        void (*disableWrite)();
    }method;
    
    struct 
    {
        unsigned char (*read)(EepromObj*, EepromPackType);
        unsigned char (*write)(EepromObj*, EepromPackType);        
        void (*resetParameter)(EepromObj*);         // Pointer of reset parameter function
    }func;
};

/* Global function prototypes */
unsigned char Read_External_EEPROM(EepromObj *obj, EepromPackType data);
unsigned char Write_External_EEPROM(EepromObj *obj, EepromPackType data);
void Reset_EEPROM_Object_Parameter(EepromObj *obj);
void Pull_Low_Write_Control(EepromObj *obj);
void Pull_High_Write_Control(EepromObj *obj);

/* Global data declarations */
extern TmrPkg storageTimeBase;

#endif
