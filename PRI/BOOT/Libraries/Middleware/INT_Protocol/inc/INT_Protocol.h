/**************************************************************************************************
File Name: INT_Protocol.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ -----------------------------------------------------
    None
Description: 
    Header file of Internal_Protocol.c. 
    This file also includes variables of Internal_Protocol_Table.c and functions of CalcuCRC16.c, 
CalcuCRC8.c, and Checksum.c.

===================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    05/30/2018 Watch Lee        1. support version 1.0. 
    05/30/2018 Watch Lee        1. support version 1.1.
    09/12/2018 Watch Lee        1. support version 1.2.
                                2. add DataFrameStr structure.
    05/06/2019 Watch Lee        1. support version 2.0.
                                2. rename CheckNoAckBits to CheckAccessBits.
                                3. add member and function of the reload updating process.
                                4. Insert a parameter "*driverObj" to functions of the object 
                                   "transmitData" and "receiveData".
                                5. add NACK to enum InternalCommandFlag.
                                6. add POINTER_DATA to enum InternalDataType.
    05/14/2019 Watch Lee        1. support version 2.1.
                                2. group command list and index into the CommandArrayType.
                                3. Add "indexLength" member to object.
    10/23/2019 Watch Lee        1. support version 3.0.
                                2. decouple SLIP arguments.
                                3. extend driver arquments to support 2 bytes buffer.
    12/13/2019 Watch Lee        1. support version 3.0.
                                2. modify InternalCommandType for InvInterComm compatibility.
    10/26/2020 Watch Lee        1. support version 3.2.
                                2. remove InvInterComm data.

**************************************************************************************************/

#ifndef __INTERNAL_PROTOCOL_H__
#define __INTERNAL_PROTOCOL_H__

/* Includes */
#include "SLIP.h"
#include "..\..\Common\inc\CircularBuffer.h"

/* Macro definitions */
// Statement Replacement
#ifndef Checksum_Type
#define Checksum_Type                       unsigned long(*)(unsigned long, unsigned char)
#endif

#ifndef Check_Power_2
#define Check_Power_2(Number)               (((((unsigned)Number - 1) ^ Number) == (Number << 1) - 1) ? Number : -1)
#endif

#ifndef Check_Index
#define Check_Index(List_Length, Index_Length)  (Index_Length * (Index_Length >= List_Length))
#endif 

// Group of declarations - Command index length must greater than command list length  
#define Create_INT_Command(Name, \
                           List_Length, \
                           Index_Length)        extern CommandInfoStr *Name##Index[];\
                                                extern const CommandInfoStr Name##List[];\
                                                CommandArrayType Name = {\
                                                                            Name##List, List_Length,\
                                                                            Name##Index, Index_Length\
                                                                        };\
                                                CommandInfoStr *Name##Index[Check_Index(List_Length, Index_Length)];\
                                                const CommandInfoStr Name##List[List_Length]

// Group of declarations - Size MUST be power of 2 (2^n)
#define Create_INT_Protocol_Object(ObjName, \
                                   Size)        Create_INT_Protocol_Member(ObjName##Tx, ObjName##Rx, \
                                                                           Check_Power_2(Size));\
                                                IntProtocolObj ObjName = \
                                                {\
                                                    {\
                                                        &ObjName##TxLink, &ObjName##RxLink, \
                                                        &ObjName##TxQueue, &ObjName##RxQueue, 0, 0,\
                                                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
                                                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
                                                        {\
                                                            (Checksum_Type)Calculate_CRC16, 2, \
                                                            CHECKSUM_IN_HEAD, NO_INVERT\
                                                        }\
                                                    }, \
                                                    {\
                                                        Process_Internal_RXD, \
                                                        C28x_Read_INTP_Packet_Data, \
                                                        C28x_Read_INTP_Not_Packet_Data, \
                                                        C28x_Write_INTP_Packet_Data, \
                                                        C28x_Write_INTP_Not_Packet_Data, \
                                                        Calculate_INTP_Checksum, \
                                                        Update_INTP_Checksum\
                                                    }, \
                                                    {\
                                                        Init_INT_Protocol, \
                                                        Pack_Internal_TXD, \
                                                        Unpack_Internal_RXD, \
                                                        Transmit_Internal_Data, \
                                                        Receive_Internal_Data, \
                                                        Get_INTP_No_Ack_Flag, \
                                                        Get_INTP_Reload_Flag, \
                                                        Clear_INTP_Reload_Flag\
                                                    }\
                                                }

#define Create_INT_Protocol_Member(TxName, \
                                   RxName, \
                                   Size)        Create_SLIP_Register(TxName##Link, Size);\
                                                Create_SLIP_Register(RxName##Link, Size);\
                                                Create_Priority_Queue(TxName##Queue, Size, Size);\
                                                Create_Circular_Queue(RxName##Queue, Size)
                                                
#define Create_SLIP_Register(Name, Size)        SlipBufferStr Name##Buffer[Size];\
                                                DataRegStr Name = {\
                                                                     Name##Buffer,\
                                                                     sizeof(Name##Buffer) / sizeof(SlipBufferStr),\
                                                                     0\
                                                                  }

#define Create_Priority_Queue(Name, \
                              L_Size, H_Size)   QueueDataStr Name##LowData[L_Size], Name##HighData[H_Size];\
                                                PriorQueueRegStr Name = \
                                                {\
                                                   {0, 0, 0, 0, 0, Name##LowData, sizeof(Name##LowData) >> 3},\
                                                   {0, 0, 0, 0, 0, Name##HighData, sizeof(Name##HighData) >> 3},\
                                                   0\
                                                }
                                                
#define Create_Circular_Queue(Name, Size)       QueueDataStr Name##Data[Size];\
                                                QueueRegStr Name = \
                                                {0, 0, 0, 0, 0, Name##Data, sizeof(Name##Data) >> 3}
                                               
/* Type definitions */  
// Enumeration
typedef enum
{
    UNUSED           = 0,
    GETTABLE         = 1,
    SETTABLE         = 2,
    NACK             = 3
}InternalCommandFlag;

typedef enum
{
    NOT_PACKET       = 0,    
    DATA_PACKET      = 1,
    POINTER_DATA     = 2,
}InternalDataType;

typedef enum
{
    INTERNAL_GET     = 1,
    INTERNAL_SET     = 2,
    INTERNAL_REPLY   = 3,
    INTERNAL_ACK     = 4  
}InternalCommandType;

typedef enum
{
    CHECKSUM_IN_HEAD = 0,
    CHECKSUM_IN_END  = 1, 
    NO_INVERT        = 0,
    INVERT           = 1      
}ChecksumArgument;

// Struct & Union  
typedef struct  
{   
    void *ptr;
    unsigned long size; 
}DataPackStr;

typedef struct  
{   
    unsigned char command;
    unsigned char type;
    DataPackStr pack;
}DataFrameStr;

typedef struct
{
    unsigned char flag;
    unsigned char code;
    unsigned char dataType;
    void *ptr;
    unsigned long size;
}CommandInfoStr;

typedef struct
{
    const CommandInfoStr *list;
    unsigned long listLength;
    CommandInfoStr **index;     
    unsigned long indexLength; 
}CommandArrayType;

typedef union
{
    unsigned short all;
    
    struct
    {    
        unsigned short code0  :1;
        unsigned short code1  :1;
        unsigned short code2  :1;
        unsigned short code3  :1;
        unsigned short code4  :1;
        unsigned short code5  :1;
        unsigned short code6  :1;
        unsigned short code7  :1;

        unsigned short code8  :1;
        unsigned short code9  :1;
        unsigned short codeA  :1;
        unsigned short codeB  :1;
        unsigned short codeC  :1;
        unsigned short codeD  :1;
        unsigned short codeE  :1;
        unsigned short codeF  :1;
    }bit;
}CheckAccessBits;

typedef struct
{
    void *buffer;
    unsigned char length;
    unsigned char index;
}DataRegStr;

typedef struct IntProtocolStr IntProtocolObj;  

struct IntProtocolStr
{
    struct
    {
        DataRegStr *txData;             // Transmit data register pointer
        DataRegStr *rxData;             // Receive data register pointer
        PriorQueueRegStr *txQueue;      // Transmit pointer queue
        QueueRegStr *rxQueue;           // Receive pointer queue
        CommandInfoStr **listIndex;     // Pointer of the command table index
        unsigned long indexLength;      // Command table index length
        CheckAccessBits noAckBits[16];  // No ack flag of the commands
        CheckAccessBits reloadBits[16]; // Reload flag of the commands
        
        struct
        {    
            unsigned long (*algorithm)(unsigned long,   // Pointer of checksum calculation function
                                       unsigned char);
            unsigned char size;                         // Number of checksum bytes
            unsigned char position;                     // Position of checksum
            unsigned char invert;                       // Do/Don't invert checksum
        }checksum;
    }member;

    struct 
    {
        void (*rxDataProcess)(IntProtocolObj*, DataPackStr);                    // Process method of received data
        unsigned char (*readPacketData)(IntProtocolObj*, DataFrameStr);         // Method of read the packet data
        unsigned char (*readNotPacketData)(IntProtocolObj*, DataFrameStr);      // Method of read the non-packet data
        unsigned char (*writePacketData)(IntProtocolObj*, DataPackStr);         // Method of write the packet data
        unsigned char (*writeNotPacketData)(IntProtocolObj*, DataPackStr);      // Method of write the non-packet data  
        unsigned long (*calculateChecksum)(IntProtocolObj*,                     // Method of the checksum calculation
                                           unsigned long, unsigned char);
        void (*updateChecksum)(IntProtocolObj*, unsigned char*, unsigned long); // Method of update checksum to buffer
    }method; 

    struct 
    {
        void (*init)(IntProtocolObj*, CommandArrayType*);                       // Initialization of command index    
        void (*packTxData)(IntProtocolObj*,                                     // Pack transmit data of the command
                           unsigned char, unsigned char, unsigned char);
        void (*unpackRxData)(IntProtocolObj*);                                  // Unpack received data of all commands
        void (*transmitData)(IntProtocolObj *obj, void *driverObj,              // Transmit data routine
                             unsigned char (*driverFunc)(void*, void*, unsigned char)); 
        void (*receiveData)(IntProtocolObj *obj, void *driverObj,               // Receive data routine
                            unsigned char (*driverFunc)(void*, void**, unsigned char*));
        unsigned char (*getNoAckFlag)(IntProtocolObj*, unsigned char);          // Get no ack flag of the commad
        unsigned char (*getReloadFlag)(IntProtocolObj*, unsigned char);         // Get reload flag of the commad
        void (*clearReloadFlag)(IntProtocolObj*, unsigned char);                // Clear reload flag of the commad
    }func; 
};

/* Global function prototypes */
void Init_INT_Protocol(IntProtocolObj *obj, CommandArrayType *command);
void Init_Command_Index(IntProtocolObj *obj, CommandArrayType *command);
void Pack_Internal_TXD(IntProtocolObj *obj, unsigned char command, unsigned char type, unsigned char priority);
unsigned char Read_INTP_Packet_Data(IntProtocolObj *obj, DataFrameStr data);
unsigned char Read_INTP_Not_Packet_Data(IntProtocolObj *obj, DataFrameStr data);
void Unpack_Internal_RXD(IntProtocolObj *obj);
void Process_Internal_RXD(IntProtocolObj *obj, DataPackStr data);
unsigned char Write_INTP_Packet_Data(IntProtocolObj *obj, DataPackStr data);
unsigned char Write_INTP_Not_Packet_Data(IntProtocolObj *obj, DataPackStr data);
void Transmit_Internal_Data(IntProtocolObj *obj, void *driverObj,
                            unsigned char (*driverFunc)(void*, void*, unsigned char));
void Receive_Internal_Data(IntProtocolObj *obj, void *driverObj, 
                           unsigned char (*driverFunc)(void*, void**, unsigned char*));
unsigned long Calculate_INTP_Checksum(IntProtocolObj *obj, unsigned long init, unsigned char data);
void Update_INTP_Checksum(IntProtocolObj *obj, unsigned char *buffer, unsigned long checksum);
unsigned char Get_INTP_No_Ack_Flag(IntProtocolObj *obj, unsigned char command);
unsigned char Get_INTP_Reload_Flag(IntProtocolObj *obj, unsigned char command);
void Clear_INTP_Reload_Flag(IntProtocolObj *obj, unsigned char command);

unsigned long Calculate_CRC16(unsigned long crcValue, unsigned char Data);
unsigned long Calculate_CRC8(unsigned long crcValue, unsigned char data);
unsigned long Calculate_Checksum(unsigned long checksum, unsigned char data);

/* Global data declarations */

#endif
