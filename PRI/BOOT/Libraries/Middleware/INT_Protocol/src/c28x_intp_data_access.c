/**************************************************************************************************
File Name: c28x_intp_data_access.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    Update_INTP_Checksum                    INT_Protocol.c
Description:
        Expanded INTP data access function for TI C28x DSP.

===================================================================================================
History:
     Date       Author           Description of Change
     ---------- ---------------- -----------------------------------------------------------------
     12/30/2019 Watch Lee        1. version 1.0.

**************************************************************************************************/

/* Includes */
#include "..\inc\INT_Protocol.h"
#include "..\inc\c28x_intp_data_access.h"

/**************************************************************************************************
Function Name:
    unsigned char C28x_Read_INTP_Packet_Data(IntProtocolObj *obj, DataFrameStr data)
Input:
    *obj      - Object pointer of internal protocol.
    data      - Structure of data frame.
Output:
    byteCount - The total number of bytes of the internal protocol data frame.
Comment:
    C28x read packet data of internal protocol function. This function will extract the packet data
of the command list to the space is pointed to by the data pointer of the parameter "data".
**************************************************************************************************/
unsigned char C28x_Read_INTP_Packet_Data(IntProtocolObj *obj, DataFrameStr data)
{
    int i, j;
    unsigned char *buffer = (unsigned char*)data.pack.ptr, byteCount = 2;
    DataPackStr *infoDataPtr = (DataPackStr*)obj->member.listIndex[data.command]->ptr;

    if (!obj->member.checksum.position)
        byteCount += obj->member.checksum.size;

    // Update buffer and checksum
    unsigned long checksumValue = obj->method.calculateChecksum(obj, 0, buffer[0]);
    checksumValue = obj->method.calculateChecksum(obj, checksumValue, buffer[1]);

    for (i = 0; i < (obj->member.listIndex[data.command]->size >> 2); i++)
    {
        for (j = 0; j < ((DataPackStr*)obj->member.listIndex[data.command]->ptr + i)->size; j++)
        {
/*
            buffer[byteCount] = (j & 1) ? (*((unsigned char*)(infoDataPtr + i)->ptr + (j >> 1)) >> 8) & 0xFF :
                                          *((unsigned char*)(infoDataPtr + i)->ptr + (j >> 1)) & 0xFF;
*/
            buffer[byteCount] = __byte((int*)(infoDataPtr + i)->ptr, j);    // use c28x intrinsic
            checksumValue = obj->method.calculateChecksum(obj, checksumValue, buffer[byteCount]);

            if (++byteCount > data.pack.size)
                break;
        }
    }

    if (!obj->member.checksum.position)
        Update_INTP_Checksum(obj, &buffer[2], checksumValue);
    else
    {
        Update_INTP_Checksum(obj, &buffer[byteCount], checksumValue);
        byteCount += obj->member.checksum.size;
    }
    return byteCount;
}
/**************************************************************************************************
Function Name:
    unsigned char C28x_Read_INTP_Not_Packet_Data(IntProtocolObj *obj, DataFrameStr data)
Input:
    *obj      - Object pointer of internal protocol.
    data      - Structure of data frame.
Output:
    byteCount - The total number of bytes of the internal protocol data frame.
Comment:
    C28x read non-packed data of internal protocol function. This function will extract the
non-packed data of the command list to the space is pointed to by the data pointer of the
parameter "data".
**************************************************************************************************/
unsigned char C28x_Read_INTP_Not_Packet_Data(IntProtocolObj *obj, DataFrameStr data)
{
    int i;
    unsigned char *buffer = (unsigned char*)data.pack.ptr, byteCount = 2;

    if (!obj->member.checksum.position)
        byteCount += obj->member.checksum.size;

    // Update buffer and checksum
    unsigned long checksumValue = obj->method.calculateChecksum(obj, 0, buffer[0]);
    checksumValue = obj->method.calculateChecksum(obj, checksumValue, buffer[1]);

    unsigned char *ptrBase;

    if (obj->member.listIndex[data.command]->dataType == POINTER_DATA)
    {
        ptrBase = *(unsigned char**)obj->member.listIndex[data.command]->ptr;

        for (i = 0; i < obj->member.listIndex[data.command]->size; i++)
        {
            buffer[byteCount] = *(ptrBase + i) & 0xFF;
            checksumValue = obj->method.calculateChecksum(obj, checksumValue, buffer[byteCount]);

            if (++byteCount > data.pack.size)
                break;
        }
    }
    else
    {
        ptrBase = (unsigned char*)obj->member.listIndex[data.command]->ptr;

        for (i = 0; i < obj->member.listIndex[data.command]->size; i++)
        {
//            buffer[byteCount] = (i & 1) ? (*(ptrBase + (i >> 1)) >> 8) & 0xFF : *(ptrBase + (i >> 1)) & 0xFF;
            buffer[byteCount] = __byte((int*)ptrBase, i);    // use c28x intrinsic
            checksumValue = obj->method.calculateChecksum(obj, checksumValue, buffer[byteCount]);

            if (++byteCount > data.pack.size)
                break;
        }
    }

    if (!obj->member.checksum.position)
        Update_INTP_Checksum(obj, &buffer[2], checksumValue);
    else
    {
        Update_INTP_Checksum(obj, &buffer[byteCount], checksumValue);
        byteCount += obj->member.checksum.size;
    }
    return byteCount;
}
/**************************************************************************************************
Function Name:
    unsigned char C28x_Write_INTP_Packet_Data(IntProtocolObj *obj, SlipDataStr data)
Input:
    *obj      - Object pointer of internal protocol.
    data      - Structure of SLIP data packet. The structure includes both a data pointer and its
                size. The data pointer of the structure points to a decoded data.
Output:
    return    - Result. 0 = no data updated, 1 = data have updated.
Comment:
    C28x write packet data of internal protocol function. This function will update the packet
data of the command list.
**************************************************************************************************/
unsigned char C28x_Write_INTP_Packet_Data(IntProtocolObj *obj, DataPackStr data)
{
    int i, j;

    unsigned char *buffer = (unsigned char*)data.ptr, byteCount = 2;
    unsigned char command = buffer[0];
//    unsigned short word = 0;
    DataPackStr *infoDataPtr = (DataPackStr*)obj->member.listIndex[command]->ptr;

    if (!obj->member.checksum.position)
        byteCount += obj->member.checksum.size;

    // Check data size
    unsigned char packetSize = 0;

    for (i = 0; i < (obj->member.listIndex[command]->size >> 2); i++)
        packetSize += (infoDataPtr + i)->size;

    if (packetSize == (data.size - 2 - obj->member.checksum.size) &&
        (obj->member.listIndex[command]->flag == SETTABLE || obj->member.listIndex[command]->flag == NACK))
    {
        // Update data
        for (i = 0; i < (obj->member.listIndex[command]->size >> 2); i++)
        {
            for (j = 0; j < (infoDataPtr + i)->size; j++)
            {
/*                if (j & 1)
                {
                    word |= (buffer[byteCount] << 8) & 0xFF00;
                    *((unsigned char*)(infoDataPtr + i)->ptr + (j >> 1)) = word;
                }
                else
                    word = buffer[byteCount] & 0xFF;*/
                __byte((int*)(infoDataPtr + i)->ptr, j) = buffer[byteCount];    // use c28x intrinsic
                ++byteCount;

            }
        }
        return 1;
    }
    else
        return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char C28x_Write_INTP_Not_Packet_Data(IntProtocolObj *obj, SlipDataStr data)
Input:
    *obj      - Object pointer of internal protocol.
    data      - Structure of SLIP data packet. The structure includes both a data pointer and its
                size. The data pointer of the structure points to a decoded data.
Output:
    return    - Result. 0 = no data updated, 1 = data have updated.
Comment:
    C28x write non-packed data of internal protocol function. This function will update the
non-packed data of the command list.
**************************************************************************************************/
unsigned char C28x_Write_INTP_Not_Packet_Data(IntProtocolObj *obj, DataPackStr data)
{
    int i;
    unsigned char *buffer = (unsigned char*)data.ptr, byteCount = 2;
    unsigned char command = buffer[0];
//    unsigned short word = 0;

    if (!obj->member.checksum.position)
        byteCount += obj->member.checksum.size;

    // Check data size
    if (obj->member.listIndex[command]->size == (data.size - 2 - obj->member.checksum.size) &&
        (obj->member.listIndex[command]->flag == SETTABLE || obj->member.listIndex[command]->flag == NACK))
    {
        unsigned char *ptrBase;

        if (obj->member.listIndex[command]->dataType == POINTER_DATA)
        {
            ptrBase = *(unsigned char**)obj->member.listIndex[command]->ptr;

            for (i = 0; i < obj->member.listIndex[command]->size; i++)
            {
                *(ptrBase + i) = buffer[byteCount];
                ++byteCount;
            }
        }
        else
        {
            ptrBase = (unsigned char*)obj->member.listIndex[command]->ptr;

            for (i = 0; i < obj->member.listIndex[command]->size; i++)
            {
/*                if (i & 1)
                {
                    word |= (buffer[byteCount] << 8) & 0xFF00;
                    *(ptrBase + (i >> 1)) = word;
                }
                else
                    word = buffer[byteCount] & 0xFF;*/
                __byte((int*)ptrBase, i) = buffer[byteCount];    // use c28x intrinsic
                ++byteCount;
            }
        }
        return 1;
    }
    else
        return 0;
}
//---------------- END LINE -----------------------------------------------------------------------
