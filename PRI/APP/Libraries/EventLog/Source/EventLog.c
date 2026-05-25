/**************************************************************************************************
File Name: EventLog.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    eventLogTable                           EventLog_Table.c
    accumulateDataTable                     EventLog_Table.c
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    Read_External_EEPROM                    ExtEEPROM_M24xxx.c
    Write_External_EEPROM                   ExtEEPROM_M24xxx.c
    Circular_Buffer_Enqueue                 CircularBuffer.c
    Circular_Buffer_Dequeue                 CircularBuffer.c
    CalcuCRC8                               CalcuCRC8.c
Description: 
    Universal event log program.

Example record format:

1 block = 1024 bytes = 16 lines

1 line = 64 bytes = Index(8 bytes) + log data(56 bytes)

Index = 8 bytes = State code(2 bytes)
                + CRC8(1 byte)
                + Cycle count(1 byte)
                + Write cycle(2 bytes)
                + Block number(1 byte)
                + Line number(1 byte)

Log data = 56 bytes, the details are in the eventLogTable.

Definition of state code: 
Line data is correct = 0xFFFF, incorrect = 0x4545(ASCII:EE) <- Reserved

===================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    12/28/2017 Watch Lee        1. version 1.0.
    07/23/2018 Watch Lee        1. version 2.0.
                                2. replace circular buffer function.
                                3. add the member structure to object. 
    08/15/2018 Watch Lee        1. version 3.0.
                                2. add the member structure to object.
                                3. remove direct peripheral module access.
    11/20/2018 Watch Lee        1. version 3.1.
                                2. modify Format_Event_Log_Space to accept the host can format 
                                   EEPROM over once when MCU powered.
                                3. modify Init_Event_Log_Buffer to pass the accumulate data 
                                   updating when log error.
    09/17/2021 S.Y Lee          1. version 3.2
                                2. Support TI unsigned char length is 16 bits problem.
                                   Modify all data put in high byte of short variable.
**************************************************************************************************/

/* Includes */ 
#include "../../EventLog/Include/EventLog.h"

/* Local function prototypes */
void Check_Event_Index(EventListObj *obj, EventIndexType *index);
unsigned char Reset_Driver_Task(EventListObj *obj);
unsigned char Check_Event_CRC_Error(LogLineType *log);
unsigned char Update_Event_Log_Data(LogLineType *log);
unsigned char Push_Data_to_Log(EventLogType table, unsigned char *buffer, unsigned char *offset);
unsigned char Pop_Last_Log_Data(void *ptr, EventListObj obj, unsigned char item);
unsigned char Calculate_Event_CRC(unsigned char *linePtr);

/* Data definitions */
Create_EEPROM_Object(extEeprom, Default_Device_ID, Default_Address_Byte, Default_Page_Write);

/**************************************************************************************************
Function Name:
void Init_Event_Log_Buffer(EventListObj *obj, void *driverObj,
                           unsigned char (*driverRead)(void *obj, unsigned char id, 
                                                       void *aPtr, unsigned char aSize, 
                                                       void *dPtr, unsigned char dSize),  
                           unsigned char (*driverWrite)(void *obj, unsigned char id,
                                                        void *aPtr, unsigned char aSize, 
                                                        void *dPtr, unsigned char dSize), 
                           void (*driverReset)())
Input:
    *obj         - Object pointer of event list.
    *driverObj   - Object pointer of driver.
    *driverRead  - Function pointer of driver reading.
    *driverWrite - Function pointer of driver writing.
    *driverReset - Function pointer of driver reset.
Output:
    None.
Comment:
    Init event log data function. The function will transfer the event log data in an non-volatile 
storage to internal RAM of MCU. It shall be executed after init the peripheral function and before 
main loop function.

The driver reading and driver writing function shall include six parameters and a return value. 
The first argument is an address of the peripheral object. 
The second argument is the device ID.
The third argument pass to parameters of the reception function is an address of the offset. 
The fourth argument is a number of byte of the offset. 
The fifth arqument is an address of the data. 
The sixth argument is a number of byte of the offset.
The return value shall mean state of the read/write operation. 1 = completed, 0 = do not complete. 
**************************************************************************************************/
void Init_Event_Log_Buffer(EventListObj *obj, void *driverObj,
                           unsigned char (*driverRead)(void *obj, unsigned char id, 
                                                       void *aPtr, unsigned char aSize, 
                                                       void *dPtr, unsigned char dSize),  
                           unsigned char (*driverWrite)(void *obj, unsigned char id,
                                                        void *aPtr, unsigned char aSize, 
                                                        void *dPtr, unsigned char dSize), 
                           void (*driverReset)())
{
    int i, offset, error;
    EventIndexType index = {0, 0, 0, 0, 0, 0};

    // Update driver information
    obj->member.storageObj->member.driverObj = driverObj;
    obj->member.storageObj->method.driverRead = driverRead;
    obj->member.storageObj->method.driverWrite = driverWrite;
    obj->member.storageObj->method.driverReset = driverReset;
    obj->member.storageObj->method.disableWrite(obj->member.storageObj);
    
    // Search the location of the latest event data
    Check_Event_Index(obj, &index);
    offset = (index.block * Log_Block_Size + index.line * Log_Line_Size - (Log_Buffer_Size - 1) * Log_Line_Size) & 
             (Log_Block_Length * Log_Block_Size - 1);
    
    // Load the latest event data to RAM
    obj->member.pointer = 0;
       
    EepromPackType data = {offset, Log_Line_Size, 0};
    
    for (i = 0; i < Log_Buffer_Size; i++)
    {        
        data.ptr = &obj->member.buffer[i];

        while (!obj->member.storageObj->func.read(obj->member.storageObj, data))
            Kick_WatchDog();
        data.offset = (data.offset + Log_Line_Size) & (Log_Block_Length * Log_Block_Size - 1); 
        error = Reset_Driver_Task(obj);
         
        if (error || obj->member.buffer[i].index.crc8 != Calculate_Event_CRC((unsigned char*)&obj->member.buffer[i]))
        {    
            obj->member.accessError += ((obj->member.accessError + 1) >> 8) ^ 0x01;
            obj->member.buffer[i].index.state[0] = Log_Error_Code;
            obj->member.buffer[i].index.state[1] = Log_Error_Code;
        }
        obj->member.pointer = (int)(obj->member.pointer + 1) & (Log_Buffer_Size - 1);
    } 
    
    // Update accumulating data
    for (i = 0; i < Accumulate_Data_Table_Size; i++)
    {
        if (accumulateDataTable[i].ptr && accumulateDataTable[i].size > 0)
            Pop_Last_Log_Data(accumulateDataTable[i].ptr, *obj, accumulateDataTable[i].targetItem);
    }
}
/**************************************************************************************************
Function Name:
    void Check_Event_Index(EventListObj *obj, EventIndexType *index)
Input:
    *obj     - Object pointer of event list.
    *index   - index pointer, the pointer points to a event log index.
Output:
    None.
Comment:
    Check event index function, the function will find the last event log position and update the 
line and block of the last event log to the index variable which the index pointer points to.    
**************************************************************************************************/
void Check_Event_Index(EventListObj *obj, EventIndexType *index)
{
    LogLineType checkIndex;
    long i, j, logCount = 0, maxCount = 0;
    EepromPackType data = {0, 8, 0};
    
    for (i = 0; i < Log_Block_Length; i++)
    {
        for (j = 0; j < Log_Line_Length; j++)
        {
            data.offset = Log_Block_Size * i + Log_Line_Size * j;
            data.ptr = &checkIndex.index;
            while (!obj->member.storageObj->func.read(obj->member.storageObj, data))
                Kick_WatchDog();
            
            if (Reset_Driver_Task(obj))
                obj->member.accessError += ((obj->member.accessError + 1) >> 8) ^ 0x01;
            
            logCount = (((long)checkIndex.index.cycleCount & 0x00FF) << 16)
                       + (((long)checkIndex.index.writeCycle[0] & 0x00FF) << 8)
                       + (((long)checkIndex.index.writeCycle[1] & 0x00FF));
            
            if (logCount > maxCount && logCount != 0xFFFFFF && 
                checkIndex.index.block == i && checkIndex.index.line == j)
            {
                index->line = checkIndex.index.line;
                index->block = checkIndex.index.block;
                index->cycleCount = checkIndex.index.cycleCount;
                index->writeCycle[0] = checkIndex.index.writeCycle[0];
                index->writeCycle[1] = checkIndex.index.writeCycle[1];
                maxCount = logCount;
            }
        }
    }
}
/**************************************************************************************************
Function Name:
    void Event_Log_Routine_Task(EventListObj *obj)
Input:
    *obj     - Object pointer of event list.
Output:
    None.
Comment:
    Event log routine task function. The function will transfer the event log data in internal 
RAM of MCU to an external EEPROM when the queue of event log is not empty.
**************************************************************************************************/
void Event_Log_Routine_Task(EventListObj *obj)
{
    QueueDataStr dataPtr;
    unsigned int offset, block, line;
    
    if (Common_Buffer_Dequeue(&obj->member.queue, &dataPtr))
    {
        obj->member.queue.freeze = 1;
        
        block = ((LogLineType*)dataPtr.ptr)->index.block;
        line = ((LogLineType*)dataPtr.ptr)->index.line;
        offset = block * Log_Block_Size + line * Log_Line_Size;
        
        EepromPackType data = {offset, dataPtr.size, dataPtr.ptr};
        
        if (dataPtr.direction)
        {
            if (obj->member.storageObj->func.read(obj->member.storageObj, data))
            {
                Reset_Driver_Task(obj);
                obj->member.queue.freeze = 0;
                
                if (Check_Event_CRC_Error((LogLineType*)dataPtr.ptr))
                    obj->member.accessError += ((obj->member.accessError + 1) >> 8) ^ 0x01;
            }
        }
        else 
        {             
            if (obj->member.storageObj->func.write(obj->member.storageObj, data))
            {
                Reset_Driver_Task(obj);
                obj->member.queue.freeze = 0;
            }
        }
    }
}
/**************************************************************************************************
Function Name:
    unsigned char Reset_Driver_Task(EventListObj *obj)
Input:
    *obj      - Object pointer of event list.
Output:
    resetFlag - Result. 1 = driver reset, 0 = only clear result parameter of state machine 
                in the driver.
Comment:
    Reset driver function. The function will reset driver when the result parameter of 
state machine is 0x80. 
**************************************************************************************************/
unsigned char Reset_Driver_Task(EventListObj *obj)
{
    unsigned char resetFlag = 0;
         
    if (obj->member.storageObj->member.result == 0x80)
    {
        if (obj->member.storageObj->method.driverReset)
            obj->member.storageObj->method.driverReset(obj->member.storageObj->member.driverObj);
        
        obj->member.accessError += ((obj->member.accessError + 1) >> 8) ^ 0x01;
        resetFlag = 1;
    }
    obj->member.storageObj->member.result = 0;
    return resetFlag;
}
/**************************************************************************************************
Function Name:
    unsigned char Check_Event_CRC_Error(LogLineType *log) 
Input:
    *log     - Log pointer, the pointer points to a event log line.
Output:
    return   - Result. 1 = CRC error happened, 0 = CRC check is ok. 
Comment:
    Event log CRC detection, the data will be cleared if CRC result is incorrect.
**************************************************************************************************/
unsigned char Check_Event_CRC_Error(LogLineType *log)
{
    if (log->index.crc8 != Calculate_Event_CRC((unsigned char*)log))
    {
        log->index.block = 0;
        log->index.line = 0;
        log->index.cycleCount = 0;
        log->index.writeCycle[0] = 0;
        log->index.writeCycle[1] = 0;
        log->index.state[0] = Log_Error_Code;
        log->index.state[1] = Log_Error_Code;
        for (int j = 0; j < Log_Data_Size; j++)
            log->data[j] = 0;
        return 1;
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    void Load_Event_Log(EventListObj *obj)
Input:
    *obj     - Object pointer of event list.
Output:
    None.
Comment:
    Load event log data function. The function will enqueue the read request to update the log
page of event list. The request is accessed any addressed location in the external EEPROM via 
the page offset variable of event list.
**************************************************************************************************/
void Load_Event_Log(EventListObj *obj)
{
    unsigned short prePointer = (obj->member.pointer - 1) & (Log_Buffer_Size - 1);
    int i = 0, j = Log_Line_Length, line, block;
    
    while ((j >> ++i));
    --i;
    
    line = (int)obj->member.buffer[prePointer].index.line - 
           (obj->member.pageOffset - (obj->member.pageOffset >> i) * Log_Line_Length);
    
    block = (int)obj->member.buffer[prePointer].index.block - (obj->member.pageOffset >> i);
    
    if (line < 0)
        --block;
         
    for (i = 0; i < Log_Page_Size; i++)
    {
        obj->member.logPage[Log_Page_Size - 1 - i].index.line = (line - i) & (Log_Line_Length - 1);
        
//        if (obj->member.logPage[Log_Page_Size - 1 - i].index.line == (Log_Line_Length - 1))
//            --block;

        obj->member.logPage[Log_Page_Size - 1 - i].index.block = block & (Log_Block_Length - 1); 

        if (line == 0)
            --block;
        QueueDataStr dataPtr = {&obj->member.logPage[Log_Page_Size - 1 - i], Log_Line_Size, Read_Queue};
        Common_Buffer_Enqueue(&obj->member.queue, dataPtr);
    }
}
/**************************************************************************************************
Function Name:
    void Record_Event_Log(EventListObj *obj)
Input:
    *obj     - Object pointer of event list.
Output:
    None.
Comment:
    Record event log function. The function will process information of event log, enqueue 
the data, and update the pointer in the event log.
**************************************************************************************************/
void Record_Event_Log(EventListObj *obj)
{
    unsigned short i = 0, prePointer  = (obj->member.pointer - 1) & (Log_Buffer_Size - 1);
    while (i < Log_Buffer_Size && obj->member.buffer[prePointer].index.state[0] == Log_Error_Code
            && obj->member.buffer[prePointer].index.state[1] == Log_Error_Code)
    {
        prePointer = (prePointer - 1) & ( Log_Buffer_Size - 1);
        i++;
    }
    unsigned long writeCyclesTmp = ((obj->member.buffer[prePointer].index.writeCycle[0] & 0x00FF) << 8)
                                 + (obj->member.buffer[prePointer].index.writeCycle[1] & 0x00FF) + 1;
    
    if (obj->member.buffer[prePointer].index.cycleCount < Max_Cycle_Count)
    {
        // Update write cycles
        obj->member.buffer[obj->member.pointer].index.cycleCount = 
            obj->member.buffer[prePointer].index.cycleCount + (writeCyclesTmp >> 16);
        
        obj->member.buffer[obj->member.pointer].index.writeCycle[0] = (unsigned short)writeCyclesTmp >> 8;
        obj->member.buffer[obj->member.pointer].index.writeCycle[1] = (unsigned short)writeCyclesTmp & 0x00FF;
        
        // Update line and block count
        unsigned char tmp = obj->member.buffer[prePointer].index.line + 1;
        unsigned char line = tmp & (Log_Line_Length - 1);
        unsigned char block = (obj->member.buffer[prePointer].index.block + ((tmp & Log_Line_Length) > 0)) & 
                              (Log_Block_Length - 1);
        
        obj->member.buffer[obj->member.pointer].index.line = line;
        obj->member.buffer[obj->member.pointer].index.block = block;
        
        // Update event log data
        Update_Event_Log_Data(&obj->member.buffer[obj->member.pointer]);
        obj->member.buffer[obj->member.pointer].index.state[0] = 0xFFFF;
        obj->member.buffer[obj->member.pointer].index.state[1] = 0xFFFF;
          
        // Update CRC of event log
        obj->member.buffer[obj->member.pointer].index.crc8 = 
            Calculate_Event_CRC((unsigned char*)&obj->member.buffer[obj->member.pointer]);
        
        // Save event data to storage(enqueue)
        QueueDataStr dataPtr = {&obj->member.buffer[obj->member.pointer], Log_Line_Size, Write_Queue};
        Common_Buffer_Enqueue(&obj->member.queue, dataPtr);
        
        // Update event log pointer
        obj->member.pointer = (obj->member.pointer + 1) & (Log_Buffer_Size - 1);
    }
}
/**************************************************************************************************
Function Name:
    unsigned char Update_Event_Log_Data(LogLineType *log)
Input:
    *log     - Log pointer, the pointer points to a event log line.
Output:
    return   - Result. 1 = completed, 0 = failed. 
Comment:
    Update event log to buffer function. The function will update event log line to a log buffer.
**************************************************************************************************/
unsigned char Update_Event_Log_Data(LogLineType *log)
{
    unsigned char i, offset = 0;

    for (i = 0; i < Log_Table_Size; i++)
    {
        if (!Push_Data_to_Log(eventLogTable[i], log->data, &offset))
            break;
    }
    while(offset < Log_Data_Size)
    {
        *(log->data + offset++) = 0;
    }
    return 1;
}
/**************************************************************************************************
Function Name:
    unsigned char Push_Data_to_Log(EventLogType table, unsigned char *buffer, unsigned char *offset)
Input:
    table    - Table of event log in EventLog.h. The table shows the details of the event log 
               data. 
    *buffer  - Buffer pointer, the pointer points to a log buffer.
    *offset  - The offset to the desired starting position in a log buffer.
Output:
    return   - Result. 1 = completed, 0 = buffer is full. 
Comment:
    Push data to event log function. The function will push the event log data in the table to 
a log buffer.
**************************************************************************************************/
unsigned char Push_Data_to_Log(EventLogType table, unsigned char *buffer, unsigned char *offset)
{
    if (table.ptr && ((table.size + *offset) <= Log_Data_Size))
    {
        for (int i = 0; i * 2 < table.size; i++)
        {
            *(buffer + *offset + i * 2) = *((unsigned char*)table.ptr + i) & 0x00FF;
            if(i * 2 + 1 < table.size)
                *(buffer + *offset + i * 2 + 1) = (*((unsigned char*)table.ptr + i) & 0xFF00) >> 8;
        }
        *offset += table.size;
        return 1;
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char Pop_Last_Log_Data(void *ptr, EventListObj obj, unsigned char item)
Input:
    *ptr     - Data pointer, the function will transfer the specified data in last log to the 
               address which the pointer points to.
    obj      - Object of event list.
    item     - The item to the desired position in last log.
Output:
    return   - Result. 1 = completed, 0 = invalid access. 
Comment:
    Pop the data in last log to the specified address function.
**************************************************************************************************/
unsigned char Pop_Last_Log_Data(void *ptr, EventListObj obj, unsigned char item)
{
    int i = 0, j = 0;
    short lastPointer = (obj.member.pointer - 1) & (Log_Buffer_Size - 1);
    
    while (i < Log_Buffer_Size && obj.member.buffer[lastPointer].index.state[0] == Log_Error_Code
            && obj.member.buffer[lastPointer].index.state[1] == Log_Error_Code)
    {
        lastPointer = (lastPointer - 1) & (Log_Buffer_Size - 1);
        i++;
    }
        
    if (eventLogTable[item].ptr && eventLogTable[item].size > 0 && i < Log_Buffer_Size)
    {
        for (i = 0; i < item; i++)
            j += eventLogTable[i].size;
    
        for (i = 0; i * 2 < eventLogTable[item].size; i++)
        {
            *((unsigned char*)ptr + i) = obj.member.buffer[lastPointer].data[j + i * 2];
            if(i + 1 < eventLogTable[item].size)
                *((unsigned char*)ptr + i ) |= (obj.member.buffer[lastPointer].data[j + i * 2 + 1] & 0x00FF) << 8;
        }
        return 1;
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char Calculate_Event_CRC(unsigned char *linePtr)
Input:
    *linePtr - The pointer points to a log buffer. 
Output:
    return   - CRC value. 
Comment:
    Calculate log error checking function.
**************************************************************************************************/
unsigned char Calculate_Event_CRC(unsigned char *linePtr)
{
    unsigned char eventCRC = 0;
    
    for (int i = 3; i < Log_Line_Size; i++)
    {
        eventCRC = Calculate_CRC8(eventCRC, *(linePtr + i));
    }
    return eventCRC;
}
/**************************************************************************************************
Function Name:
    void Format_Event_Log_Space(EventListObj *obj)
Input:
    *obj     - Object pointer of event list.
Output:
    None. 
Comment:
    Format event log space function. The function will erase log data and format index.
**************************************************************************************************/
unsigned char Format_Event_Log_Space(EventListObj *obj)
{
    int i, j, k = Log_Line_Length, shift = 0, init, offset;
    
    while ((k >> ++shift));
    --shift;
    
    if (obj->member.queue.empty && obj->member.formatCount != Log_Line_Length * Log_Block_Length)
    {     
        init = obj->member.formatCount;
        
        // Block level 
        for (j = (init >> shift); j < Log_Block_Length; j++)
        {
            // Line level
            for (i = init - (init >> shift) * Log_Line_Length; i < Log_Line_Length; i++)
            {
                offset = Log_Line_Length * j + i - init;
                
                if (offset < Log_Buffer_Size)
                {
                    // Byte level
                    for (k = 0; k < Log_Line_Size; k++)
                        *((unsigned char*)&obj->member.buffer[offset] + k) = 0;
                    obj->member.buffer[offset].index.block = j;
                    obj->member.buffer[offset].index.line = i;
                    obj->member.buffer[offset].index.state[0] = 0xFFFF;
                    obj->member.buffer[offset].index.state[1] = 0xFFFF;
                    obj->member.buffer[offset].index.crc8 = 
                        Calculate_Event_CRC((unsigned char*)&obj->member.buffer[offset]);
                    
                    obj->member.formatCount++;
                }
            }
        }     
        QueueDataStr dataPtr = {obj->member.buffer, Log_Line_Size, Write_Queue};
        
        for (i = 0; i < Log_Buffer_Size; i++)
        {
            dataPtr.ptr = &obj->member.buffer[i];
            Common_Buffer_Enqueue(&obj->member.queue, dataPtr);
        }
        obj->member.pointer = 0;
        return 0;
    }
    else 
    {     
        if (obj->member.formatCount == Log_Line_Length * Log_Block_Length)
        {
            obj->member.formatCount = 0;
            return 1;
        }
        else
            return 0;
    }
}
/**************************************************************************************************
Function Name:
    void Event_Log_Timer(void)
Input:
    None.  
Output:
    None.  
Comment:
    Event log timer function for timeout checking, the function shall plug in a cycling ISR to 
enable the I2C timeout function.
**************************************************************************************************/
void Event_Log_Timer(void)
{
    Call_Timer(storageTimeBase);
}

//---------------- END LINE -----------------------------------------------------------------------
