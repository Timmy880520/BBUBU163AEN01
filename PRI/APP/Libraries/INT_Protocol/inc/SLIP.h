/**************************************************************************************************
File Name: SLIP.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ -----------------------------------------------------
    None
Description: 
    Header file of SLIP.c.

===================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    05/23/2018 Watch Lee        1. support version 1.0.
    10/26/2020 Watch Lee        1. support version 1.1.

**************************************************************************************************/

#ifndef __SLIP_H__
#define __SLIP_H__

/* Macro definitions */
#define SLIP_Packet_Size    64      // MUST be power of 2 (2^n)

// SLIP special character codes
#define SLIP_END            0xC0
#define SLIP_ESC            0xDB
#define SLIP_ESC_END        0xDC
#define SLIP_ESC_ESC        0xDD

/* Type definitions */ 
// Enumeration 
enum SlipEnum
{
    MAX_SLIP_MESSAGE_SIZE = ((SLIP_Packet_Size >> 1) - 1)
};

// Struct & Union
typedef struct
{
    unsigned char start;
    unsigned char stop;
    unsigned char size;
    unsigned char error;
    unsigned long offset;
    unsigned char code[SLIP_Packet_Size];
}SlipBufferStr;

typedef struct
{
    SlipBufferStr *buffer;
    unsigned char length;
    unsigned char index;
}SlipRegStr;

typedef struct  
{   
    void *ptr;
    unsigned long size; 
}SlipDataStr;

/* Global function prototypes */
unsigned char SLIP_Encoder(SlipBufferStr *buffer, SlipDataStr data);
unsigned char SLIP_Decoder(SlipBufferStr *buffer, SlipDataStr *data);
unsigned char Locate_SLIP_Packet(SlipBufferStr *buffer, SlipDataStr data);
void Clear_SLIP_Flag(SlipBufferStr *buffer);

/* Global data declarations */

#endif
