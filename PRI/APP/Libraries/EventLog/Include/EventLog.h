/**************************************************************************************************
File Name: EventLog.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ -----------------------------------------------------
    eventLogTable        EventLogType       Event log table.
    accumulateDataTable  AccumulateDataType Accumulated data table.
    extEeprom            EepromObj          EEPROM object.
Description: 
    Header file of universal event log program, the file has associated with EventLog.c, 
EventLog_Table.c, and CalcuCRC.c.

===================================================================================================
History:
    Date       Author           Description Of Change
    ---------- ---------------- -----------------------------------------------------------------
    12/28/2017 Watch Lee        1. support version 1.0.
    07/23/2018 Watch Lee        1. support version 2.0.
                                2. remove macro expression "EventList_Defaults".
                                3. add macro expression "Create_Event_List_Object".
                                4. add macro expression "Check_Power_2".
                                5. add the member structure to object. 
    08/15/2018 Watch Lee        1. support version 3.0.
    11/20/2018 Watch Lee        1. support version 3.1.
    09/17/2021 S.Y Lee          1. version 3.2
                                2. Support TI unsigned char length is 16 bits problem.
                                   Modify all data put in high byte of short variable.
    02/24/2022 S.Y Lee          1. version 3.3
                                2. Updated push/pop data to log method for support short or long format
                                   data can be used in EventLogType table.
**************************************************************************************************/

#ifndef __EVENTLOG_H__
#define __EVENTLOG_H__

/* Includes */
#include "F28x_Project.h"
#include "CircularBuffer.h"
#include "Initial.h"

#include "../../EventLog/Include/ExtEEPROM_M24xxx.h"
/* Macro definitions */
// Storage definition
#define Max_Cycle_Count            (long)0xFF
#define Log_Line_Size              (long)128                           // Number of bytes in a log line, MUST be power of 2 (2^n)
#define Log_Line_Length            (long)2                             // Number of lines in a log block, MUST be power of 2 (2^n)
#define Log_Block_Size             (Log_Line_Size * Log_Line_Length)   // Number of bytes in a block
#define Log_Block_Length           (long)64                            // Number of blocks in the storage space, MUST be power of 2 (2^n)

// Event log constant
#define Log_Buffer_Size            1                                   // Number of lines for MCU RAM access buffer, MUST be power of 2 (2^n)
#define Log_Page_Size              1                                   // Number of lines in a log page, MUST be power of 2 (2^n)
#define Log_Data_Size              (Log_Line_Size - 8)                 // Log_Line_Size - 8 (index size)
#define Log_Table_Size             (Log_Line_Size - 8)
#define Accumulate_Data_Table_Size 1
#define Log_Error_Code             0x45

// Statement Replacement
#ifndef Check_Power_2
#define Check_Power_2(Number)                   (Number * ((((unsigned)Number - 1) ^ Number) == ((Number << 1) - 1)))
#endif
// Group of declarations - Size MUST be power of 2 (2^n)
#define Create_Event_List_Object(Name, \
                                 Queue_Size)        QueueDataStr Name##Queue[Check_Power_2(Queue_Size)];\
                                                    EventListObj Name = \
                                                    {\
                                                        {\
                                                            0, 0, 0, 0, {0}, {0},\
                                                            {0, 0, 0, 0, 0, Name##Queue, Queue_Size},\
                                                            &extEeprom\
                                                        },\
                                                        {\
                                                            Init_Event_Log_Buffer,\
                                                            Event_Log_Routine_Task,\
                                                            Record_Event_Log,\
                                                            Load_Event_Log,\
                                                            Format_Event_Log_Space,\
                                                            Event_Log_Timer\
                                                        }\
                                                    }

// Statement Replacement
#ifndef Check_Power_2
#define Check_Power_2(Number)                   (((Number - 1) ^ Number) == ((Number << 1) - 1))
#endif

/* Type definitions */
// Enumeration

// Struct and union
typedef struct
{
    unsigned short state[2];
    unsigned char crc8;
    unsigned char cycleCount;
    unsigned short writeCycle[2];
    unsigned char block;
    unsigned char line;			
}EventIndexType;
     
typedef struct
{
    EventIndexType index;	
    unsigned char data[Log_Data_Size];	 
}LogLineType;

typedef struct
{
    void *ptr;
    unsigned long size;
}EventLogType;

typedef struct
{
    unsigned char targetItem;
    void *ptr;
    unsigned long size;
}AccumulateDataType;

typedef struct EventListStr EventListObj;

struct EventListStr
{
    struct 
    {
        short pointer;
        unsigned short pageOffset;
        unsigned short formatCount;
        unsigned short accessError;
        LogLineType buffer[Log_Buffer_Size];
        LogLineType logPage[Log_Page_Size];
        QueueRegStr queue;
        EepromObj *storageObj;
    }member;
    
    struct 
    {
        void (*init)(EventListObj*, void *,                             // Init event log data function     
                     unsigned char (*driverRead)(void*, unsigned char,  // Peripheral module reading function  
                                    void *, unsigned char, void *, unsigned char),  
                     unsigned char (*driverWrite)(void*, unsigned char, // Peripheral module writing function
                                    void *, unsigned char, void *, unsigned char),
                     void (*driverReset)());                            // Peripheral module reset function          
        void (*routine)(EventListObj*);                                 // Event log routine task function      
        void (*record)(EventListObj*);                                  // Record event log function
        void (*load)(EventListObj*);                                    // Load event log function
        unsigned char (*format)(EventListObj*);                         // Format event log space
        void (*timer)();                                                // Event log timer for timeout checking           
    }func;
};

/* Global function prototypes */
void Init_Event_Log_Buffer(EventListObj *obj, void *driverObj,        
                           unsigned char (*driverRead)(void *obj, unsigned char id, 
                                                       void *aPtr, unsigned char aSize, 
                                                       void *dPtr, unsigned char dSize),  
                           unsigned char (*driverWrite)(void *obj, unsigned char id,
                                                        void *aPtr, unsigned char aSize, 
                                                        void *dPtr, unsigned char dSize),
                           void (*driverReset)());
void Event_Log_Routine_Task(EventListObj *obj);
void Load_Event_Log(EventListObj *obj);
void Record_Event_Log(EventListObj *obj);
unsigned char Format_Event_Log_Space(EventListObj *obj);
void Event_Log_Timer(void);

unsigned long Calculate_CRC8(unsigned long crcValue, unsigned char data);

/* Global data declarations */
extern EventLogType eventLogTable[];
extern AccumulateDataType accumulateDataTable[];
extern EepromObj extEeprom;

#endif
