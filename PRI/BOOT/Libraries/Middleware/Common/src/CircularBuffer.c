/**************************************************************************************************
File Name: CircularBuffer.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None
Description: 
    Universal circular buffer library program. The circular buffer is a FIFO buffer for asynchronous
data transmission.

Block diagram:
               Memory space                   Queue data                  Queue register  
                ___________           __________________________          _______________
               |           |         |                          |        |       |       |
 Push data0--->|  [data0]  |-------->|Save data address and size|------->|       |       |
               |           |\        |__________________________|        |       |       |
               |-----------| \        __________________________         |Enqueue|Dequeue|
               |           |  \      |                          |        |       |       |
 Push data1--->|  [data1]  |---)---->|Save data address and size|------->|       |       |
               |___________|  |      |__________________________|        |_______|_______|
                ___________   |       __________________________                     |
               |           |<-*      |                          |                    |
               |  [data0]  |<--------|Load data address and size|<-------------------*
               |___________|Pop data0|__________________________|

Use case:
    char mSpace[4][8];
    QueueDataStr queueData[4];
    QueueRegStr queueReg = {0, 0, 0, 0, 0, queueData, 4};
    ...
    ..
    .
    void Push()
    {
        QueueDataStr enQueue = {&mSpace[queueReg.enqueue][0], 8, 0};
        
        // Push data to mSpace

        Common_Buffer_Enqueue(&queueReg, enQueue);
    }
    void Pop()
    {
        QueueDataStr deQueue;
        
        if (Common_Buffer_Dequeue(&queueReg, deQueue))
        {
            // Pop data form deQueue
        }
    }
===================================================================================================
History:
     Date       Author           Description of Change
     ---------- ---------------- -----------------------------------------------------------------
     12/28/2017 Watch Lee        1. version 1.0.
     05/25/2018 Watch Lee        1. version 2.0.
                                 2. rename the member "nWR" of the QueueDataStr to "direction".
                                 3. add common buffer enqueue/dequeue function.
                                 4. add priority buffer enqueue/dequeue function.
     08/17/2018 Watch Lee        1. version 2.1.
                                 2. modify priority buffer enqueue function to against data 
                                    corruption when the queue is full.
     03/24/2021 Watch Lee        1. modify Description.

**************************************************************************************************/

/* Includes */ 
#include "..\inc\CircularBuffer.h"

/**************************************************************************************************
Function Name:
    void Priority_Buffer_Enqueue(PriorQueueRegStr *reg, QueueDataStr data, unsigned char priority)
Input:
    *reg     - Registers of priority circular buffer.
    data     - Inserted data, the function will sequentially enqueue the data to a circular 
               buffer, the data contains an array size, a flag of transfer direction, and 
               a pointer which points an array.
    priority - Queue priority. 0 = lower priority, 1 = higher priority. 
Output:
    None.
Comment:
    Priority buffer enqueue function. This function has a priority variable to control the order 
of dequeuing.  
**************************************************************************************************/
void Priority_Buffer_Enqueue(PriorQueueRegStr *reg, QueueDataStr data, unsigned char priority)
{    
    unsigned short highUsed = ((reg->highPriority.enqueue < reg->highPriority.dequeue) * reg->highPriority.length + 
                               reg->highPriority.enqueue - reg->highPriority.dequeue) & (reg->highPriority.length - 1);

    unsigned short lowUsed = ((reg->lowPriority.enqueue < reg->lowPriority.dequeue) * reg->lowPriority.length + 
                              reg->lowPriority.enqueue - reg->lowPriority.dequeue) & (reg->lowPriority.length - 1);

    unsigned short totalUsed = lowUsed + highUsed;

    if (priority)
        Common_Buffer_Enqueue(&reg->highPriority, data);
    else
        Common_Buffer_Enqueue(&reg->lowPriority, data);

    if ((totalUsed + 1) >= reg->highPriority.length || (totalUsed + 1) >= reg->lowPriority.length)
    {
        reg->highPriority.full = 1;
        reg->lowPriority.full = 1;
    }        
}
/**************************************************************************************************
Function Name:
    void Common_Buffer_Enqueue(QueueRegStr *reg, QueueDataStr data)
Input:
    *reg     - Registers of common circular buffer.
    data     - Inserted data, the function will sequentially enqueue the data to a circular 
               buffer, the data contains an array size, a flag of transfer direction, and 
               a pointer which points an array.
Output:
    None.
Comment:
    Common circular buffer enqueue function.
**************************************************************************************************/
void Common_Buffer_Enqueue(QueueRegStr *reg, QueueDataStr data)
{
    unsigned short mapping = (reg->enqueue + 1) & ((reg->length << 1) - 1);
    unsigned short i = mapping & (reg->length - 1);
    
    if (i == reg->dequeue || reg->full)	
        reg->full = 1;
    else
    {	
        reg->data[i].direction = data.direction;	
        reg->data[i].ptr = data.ptr;
        reg->data[i].size = data.size;
        reg->enqueue = i;
    }
    reg->empty = 0;  
}
/**************************************************************************************************
Function Name:
    unsigned char Priority_Buffer_Dequeue(PriorQueueRegStr *reg, QueueDataStr *data)
Input:
    *reg     - Registers of priority circular buffer.
    *dataPtr - Data pointer, the function will dequeue an array size, a flag of transfer 
               direction, and an array pointer to the specified address which the pointer points 
               to.
Output:
    return   - Result. 1 = an inserted data dequeued, 0 = queue is empty.
Comment:
    Priority buffer dequeue function. This function will dequeue the higher priority queue first,
the lower priority queue operates when the higher priority queue is empty or continuously 
dequeuing over the priority flow limit. 
**************************************************************************************************/
unsigned char Priority_Buffer_Dequeue(PriorQueueRegStr *reg, QueueDataStr *data)
{
    if (!reg->highPriority.empty && !reg->lowPriority.freeze && 
        (reg->priorityFlow < Priority_Flow_Limit || reg->highPriority.freeze))
    {   
        if (!reg->highPriority.freeze)
            ++reg->priorityFlow;
        
        if (Common_Buffer_Dequeue(&reg->highPriority, data))
        {    
            reg->lowPriority.full = 0;
            return 1;
        }
    }
    else
    {
        reg->priorityFlow = 0;
        
        if (Common_Buffer_Dequeue(&reg->lowPriority, data))
        {    
            reg->highPriority.full = 0;
            return 1;
        }
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char Common_Buffer_Dequeue(QueueRegStr *reg, QueueDataStr *dataPtr)
Input:
    *reg     - Registers of common circular buffer.
    *dataPtr - Data pointer, the function will dequeue an array size, a flag of transfer 
               direction, and an array pointer to the specified address which the pointer points 
               to.
Output:
    None.
Comment:
    Common circular buffer enqueue function.
**************************************************************************************************/
unsigned char Common_Buffer_Dequeue(QueueRegStr *reg, QueueDataStr *dataPtr)
{
    unsigned short mapping = (reg->dequeue + 1) & ((reg->length << 1) - 1);
         
    if (reg->dequeue == reg->enqueue && !reg->freeze)
    {
        reg->empty = 1;
        return 0;
    }     
    else
    {
        if (!reg->freeze)
        {	
            reg->dequeue = mapping & (reg->length - 1);
            reg->full = 0;
        }
        dataPtr->direction = reg->data[reg->dequeue].direction;
        dataPtr->ptr = reg->data[reg->dequeue].ptr;
        dataPtr->size = reg->data[reg->dequeue].size;
        return 1;
    }	
}
//---------------- END LINE -----------------------------------------------------------------------
