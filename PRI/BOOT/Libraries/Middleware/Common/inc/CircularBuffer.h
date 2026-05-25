/**************************************************************************************************
File Name: CircularBuffer.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ -----------------------------------------------------
    None
Description: 
    Header file of CircularBuffer.c.

===================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    12/28/2017 Watch Lee        1. support version 1.0. 
    05/25/2018 Watch Lee        1. support version 2.0 and above. 

**************************************************************************************************/

#ifndef __CIRCULARBUFFER_H__
#define __CIRCULARBUFFER_H__

/* Macro definitions */
#define Priority_Flow_Limit   5 
#define Write_Queue           0     // Write direction flag
#define Read_Queue            1     // Read direction flag

/* Type definitions */  
// Enumeration  
typedef enum
{
    LOW_PRIORITY     = 0,    
    HIGH_PRIORITY    = 1   
}QueuePriorityType;

// Struct & Union   
typedef struct  
{   
    void *ptr;  
    unsigned short size;
    unsigned char direction;        // Queue direction flag, reserved for the half-duplex mode. 
}QueueDataStr;

typedef struct
{
    unsigned short enqueue;
    unsigned short dequeue;
    unsigned char full;
    unsigned char empty;
    unsigned char freeze;
    QueueDataStr *data;
    unsigned long length;           // MUST be power of 2 (2^n)
}QueueRegStr;                       // Structure of common queue register

typedef struct
{
    QueueRegStr lowPriority;
    QueueRegStr highPriority;
    unsigned char priorityFlow;
}PriorQueueRegStr;                  // Structure of priority queue register

/* Global function prototypes */
void Priority_Buffer_Enqueue(PriorQueueRegStr *reg, QueueDataStr data, unsigned char priority);
void Common_Buffer_Enqueue(QueueRegStr *reg, QueueDataStr data);
unsigned char Priority_Buffer_Dequeue(PriorQueueRegStr *reg, QueueDataStr *data);
unsigned char Common_Buffer_Dequeue(QueueRegStr *reg, QueueDataStr *dataPtr);

/* Global data declarations */

#endif
