/**************************************************************************************************
File Name: SLIP.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None
Description: 
    Data locating, encoding, and decoding program for SLIP protocol. 

===================================================================================================
History:
     Date       Author           Description of Change
     ---------- ---------------- -----------------------------------------------------------------
     05/23/2018 Watch Lee        1. version 1.0.
     10/26/2020 Watch Lee        1. version 1.1.
                                 2. add Clear_SLIP_Flag function.

**************************************************************************************************/

/* Includes */ 
#include "SLIP.h"

/**************************************************************************************************
Function Name:
    unsigned char SLIP_Encoder(SlipBufferStr *buffer, SlipDataStr data)
Input:
    *buffer     - SLIP packet buffer.
    data        - Data structure, it includes a data pointer and size of the data pinter.
Output:
    return      - Result. 1 = data has encoded and updated to buffer, 0 = error occured.
Comment:
    Data encode to SLIP packet function. 
**************************************************************************************************/
unsigned char SLIP_Encoder(SlipBufferStr *buffer, SlipDataStr data)
{
    int i, j = 1;

    if (data.size > 0 && data.size < MAX_SLIP_MESSAGE_SIZE)
    {  
        buffer->code[0] = SLIP_END;
        
        for (i = 0; i < data.size; i++)
        {
            if (*((unsigned char*)data.ptr + i) == SLIP_END)
            {
                buffer->code[j] = SLIP_ESC;
                buffer->code[j + 1] = SLIP_ESC_END;
                ++j;
            }
            else if (*((unsigned char*)data.ptr + i) == SLIP_ESC)
            {
                buffer->code[j] = SLIP_ESC;
                buffer->code[j + 1] = SLIP_ESC_ESC;
                ++j;        
            }
            else
                buffer->code[j] = *((unsigned char*)data.ptr + i);
            ++j;
        }
        buffer->code[j] = SLIP_END;
        buffer->size = j + 1;
        return 1;
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char SLIP_Decoder(SlipBufferStr *buffer, SlipDataStr *data)
Input:
    *buffer     - SLIP packet buffer.
    *data       - Data structure, it includes a data pointer and size of the data pinter.
Output:
    return      - Result. 1 = SLIP packet has decoded and updated to the data space that is the 
                  data pointer points to, 0 = error occured.
Comment:
    SLIP packet decoding function. 
**************************************************************************************************/
unsigned char SLIP_Decoder(SlipBufferStr *buffer, SlipDataStr *data)
{
    int i = 1, j = 0, escape = 0;

    if (buffer->start && buffer->stop && buffer->size > 0 && !buffer->error)
    {
        while (i < (buffer->size - 1))
        {
            if (buffer->code[i] == SLIP_ESC)
                escape = 1;
            else if (escape)
            {
                if (buffer->code[i] == SLIP_ESC_END)
                    *((unsigned char*)data->ptr + j) = SLIP_END;
                else
                    *((unsigned char*)data->ptr + j) = SLIP_ESC;
                escape = 0;
                ++j;
            }
            else
            {    
                *((unsigned char*)data->ptr + j) = buffer->code[i];
                ++j;
            }
            ++i;
        }
        data->size = j;
        Clear_SLIP_Flag(buffer);
        return 1;
    }
    Clear_SLIP_Flag(buffer);
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char Locate_SLIP_Packet(SlipBufferStr *buffer, void *ptr, unsigned char size)
Input:
    *buffer     - SLIP packet buffer.
    data        - Data structure, it includes a data pointer and size of the data pinter.
Output:
    return      - Result. 1 = SLIP packet has located, 0 = Locating or stopped.
Comment:
    This function is used to locate a SLIP packet. This function will skip the locating if the 
buffer has filled in a SLIP packet.
**************************************************************************************************/
unsigned char Locate_SLIP_Packet(SlipBufferStr *buffer, SlipDataStr data)
{
    int i = buffer->offset;
    
    while (!buffer->stop && i < data.size)
    {
        if (!buffer->error)
        {    
            if (*((unsigned char*)data.ptr + i) == SLIP_END)
            {    
                if (!buffer->size)
                    buffer->start = 1;
                else if (!buffer->stop)
                    buffer->stop = 1;
                else
                    buffer->error = 1;
            }         
            
            if (buffer->size < SLIP_Packet_Size)
            {    
                buffer->code[buffer->size] = *((unsigned char*)data.ptr + i);
                ++buffer->size;
            }
            else
                buffer->error = 1;
        }
        ++i;
        ++buffer->offset;
    }
       
    if (buffer->stop || buffer->error)
    {
        if (!buffer->start || buffer->size < 3)
            buffer->error = 1;
        return 1;
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    void Clear_SLIP_Flag(SlipBufferStr *buffer)
Input:
    *buffer     - SLIP packet buffer.
Output:
    None.
Comment:
    This function is used to clear SLIP buffer data except include code array.
**************************************************************************************************/
void Clear_SLIP_Flag(SlipBufferStr *buffer)
{
    buffer->start = 0;
    buffer->stop = 0;
    buffer->error = 0;
    buffer->size = 0;
    buffer->offset = 0;
}
//---------------- END LINE -----------------------------------------------------------------------
