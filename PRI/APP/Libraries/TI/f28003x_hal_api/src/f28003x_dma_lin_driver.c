/**************************************************************************************************
File Name: f28003x_dma_lin_driver.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    DmaClaSrcSelRegs                        f28003x_globalvariabledefs.c
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    EALLOW                                  F28003x_device.h
    EDIS                                    F28003x_device.h
Description:
    Universal DMA configuration driver for LINBUS transmission and reception. The driver shall be
created and filled in the required data in the parameters of the driver before executing any
function in the driver.

    This driver does not set DMACTRL register. Therefore, Please ensure HW_Reeset has executed
before using the driver.
==================================================================================================
History:
    Date       Author           Description Of Change
    ---------- ---------------- -----------------------------------------------------------------
    02/22/2023 Watch Lee        1. version 1.0.
                                2. only support SCI mode.

**************************************************************************************************/

/* Includes */
#include "f28003x_dma_lin_driver.h"

/**************************************************************************************************
Function Name:
    void DMA_LIN_SCI_Mode_RX_Init(DmaLinObj *obj, unsigned long dataRegAddress)
Input:
    *obj            - Pointer of DMA driver object.
    dataRegAddress  - Address of data register.
Output:
    None.
Comment:
    DMA initialization function. This function will configure the DMA's LINBUS reception request.
**************************************************************************************************/
void DMA_LIN_SCI_Mode_RX_Init(DmaLinObj *obj, unsigned long dataRegAddress)
{
    unsigned long shift = 0, dmaChannel = (((unsigned long)obj->member.reg - DMA_CHANNEL_BASE) >> 5) + 1;
    unsigned long linRegBase = dataRegAddress & 0xFFFFFF00;
    unsigned long dmaTriggerValue = (linRegBase == LINA_Register_BASE) ? DMA_LINA_RX_TRIGGER : DMA_LINB_RX_TRIGGER;

    EALLOW;

    if (dmaChannel > 0 && dmaChannel <= 4)
    {
        shift = (dmaChannel - 1) * 8;
        DmaClaSrcSelRegs.DMACHSRCSEL1.all &= ~(0xFFL << shift);
        DmaClaSrcSelRegs.DMACHSRCSEL1.all |= dmaTriggerValue << shift;
    }
    else if (dmaChannel > 4 && dmaChannel <= 6)
    {
        shift = (dmaChannel - 5) * 8;
        DmaClaSrcSelRegs.DMACHSRCSEL2.all &= ~(0xFFL << shift);
        DmaClaSrcSelRegs.DMACHSRCSEL2.all |= dmaTriggerValue << shift;
    }

    // Channel configuration
    obj->member.reg->CONTROL.bit.SOFTRESET = 1;
    obj->member.reg->BURST_SIZE.bit.BURSTSIZE = 0;
    obj->member.reg->SRC_BURST_STEP = 0;
    obj->member.reg->DST_BURST_STEP = 0;

    obj->member.reg->SRC_ADDR_SHADOW = dataRegAddress;
    obj->member.reg->DST_ADDR_SHADOW = (unsigned long)obj->member.dma.ptr;

    obj->member.reg->SRC_BEG_ADDR_SHADOW = dataRegAddress;
    obj->member.reg->DST_BEG_ADDR_SHADOW = (unsigned long)obj->member.dma.ptr;

    obj->member.reg->TRANSFER_SIZE = obj->member.dma.size - 1;  // Number of burst = array length - 1
    obj->member.reg->SRC_TRANSFER_STEP = 0;
    obj->member.reg->DST_TRANSFER_STEP = 1;                     // Common address offset = 16bit, 16bit / 16bit = 1

    obj->member.reg->MODE.bit.DATASIZE = 0;                     // Transfer size = 16bit
    obj->member.reg->MODE.bit.CONTINUOUS = 1;                   // Enable continuous mode
    obj->member.reg->MODE.bit.PERINTSEL = dmaChannel;
    obj->member.reg->MODE.bit.PERINTE = 1;
    obj->member.reg->CONTROL.bit.RUN = 1;
    EDIS;
}
/**************************************************************************************************
Function Name:
    void DMA_LIN_SCI_9_Bit_Mode_RX_Init(DmaLinObj *obj, unsigned long dataRegAddress)
Input:
    *obj            - Pointer of DMA driver object.
    dataRegAddress  - Address of data register.
Output:
    None.
Comment:
    DMA initialization function. This function will configure the DMA's LINBUS reception request.
**************************************************************************************************/
void DMA_LIN_SCI_9_Bit_Mode_RX_Init(DmaLinObj *obj, unsigned long dataRegAddress)
{
    unsigned long dmaChannel = (((unsigned long)obj->member.reg - DMA_CHANNEL_BASE) >> 5) + 1;
    unsigned long linRegBase = dataRegAddress & 0xFFFFFF00;
    unsigned long dmaTriggerValue = (linRegBase == LINA_Register_BASE) ? DMA_LINA_RX_TRIGGER : DMA_LINB_RX_TRIGGER;

    EALLOW;

    if (dmaChannel > 0 && dmaChannel <= 4)
    {
        unsigned long shift = (dmaChannel - 1) * 8;
        DmaClaSrcSelRegs.DMACHSRCSEL1.all &= ~(0xFFL << shift);
        DmaClaSrcSelRegs.DMACHSRCSEL1.all |= dmaTriggerValue << shift;
    }
    else if (dmaChannel > 4 && dmaChannel <= 6)
    {
        unsigned long shift = (dmaChannel - 5) * 8;
        DmaClaSrcSelRegs.DMACHSRCSEL2.all &= ~(0xFFL << shift);
        DmaClaSrcSelRegs.DMACHSRCSEL2.all |= dmaTriggerValue << shift;
    }

    // Channel configuration
    obj->member.reg->CONTROL.bit.SOFTRESET = 1;
    obj->member.reg->BURST_SIZE.bit.BURSTSIZE = 1;
    obj->member.reg->SRC_BURST_STEP = 24;
    obj->member.reg->DST_BURST_STEP = 1;

    obj->member.reg->SRC_ADDR_SHADOW = dataRegAddress - 24;     // Shift to SCIFLR
    obj->member.reg->DST_ADDR_SHADOW = (unsigned long)obj->member.dma.ptr;

    obj->member.reg->SRC_BEG_ADDR_SHADOW = dataRegAddress - 24; // Shift to SCIFLR
    obj->member.reg->DST_BEG_ADDR_SHADOW = (unsigned long)obj->member.dma.ptr;

    obj->member.reg->TRANSFER_SIZE = (obj->member.dma.size >> 1) - 1;  // Number of burst = (array length / 2) - 1
    obj->member.reg->SRC_TRANSFER_STEP = 0;
    obj->member.reg->DST_TRANSFER_STEP = 1;                     // Common address offset = 16bit, 16bit / 16bit = 1

    obj->member.reg->SRC_WRAP_SIZE = 0;
    obj->member.reg->SRC_WRAP_STEP = 0;

    obj->member.reg->MODE.bit.DATASIZE = 0;                     // Transfer size = 16bit
    obj->member.reg->MODE.bit.CONTINUOUS = 1;                   // Enable continuous mode
    obj->member.reg->MODE.bit.PERINTSEL = dmaChannel;
    obj->member.reg->MODE.bit.PERINTE = 1;
    obj->member.reg->CONTROL.bit.RUN = 1;
    EDIS;
}
/**************************************************************************************************
Function Name:
    void DMA_LIN_SCI_Mode_TX_Init(DmaLinObj *obj, unsigned long dataRegAddress)
Input:
    *obj            - Pointer of DMA driver object.
    dataRegAddress  - Address of data register.
Output:
    None.
Comment:
    DMA initialization function. This function will configure the DMA's LINBUS transmission
request.
**************************************************************************************************/
void DMA_LIN_SCI_Mode_TX_Init(DmaLinObj *obj, unsigned long dataRegAddress)
{
    unsigned long dmaChannel = (((unsigned long)obj->member.reg - DMA_CHANNEL_BASE) >> 5) + 1;
    unsigned long linRegBase = dataRegAddress & 0xFFFFFF00;
    unsigned long dmaTriggerValue = (linRegBase == LINA_Register_BASE) ? DMA_LINA_TX_TRIGGER : DMA_LINB_TX_TRIGGER;

    EALLOW;
    if (dmaChannel > 0 && dmaChannel <= 4)
    {
        unsigned long shift = (dmaChannel - 1) * 8;
        DmaClaSrcSelRegs.DMACHSRCSEL1.all &= ~(0xFFL << shift);
        DmaClaSrcSelRegs.DMACHSRCSEL1.all |= dmaTriggerValue << shift;
    }
    else if (dmaChannel > 4 && dmaChannel <= 6)
    {
        unsigned long shift = (dmaChannel - 5) * 8;
        DmaClaSrcSelRegs.DMACHSRCSEL2.all &= ~(0xFFL << shift);
        DmaClaSrcSelRegs.DMACHSRCSEL2.all |= dmaTriggerValue << shift;
    }

    // Channel configuration
    obj->member.reg->CONTROL.bit.SOFTRESET = 1;
    obj->member.reg->BURST_SIZE.bit.BURSTSIZE = 0;
    obj->member.reg->SRC_BURST_STEP = 0;
    obj->member.reg->DST_BURST_STEP = 0;

    obj->member.reg->SRC_ADDR_SHADOW = (unsigned long)obj->member.dma.ptr;
    obj->member.reg->DST_ADDR_SHADOW = dataRegAddress;

    obj->member.reg->SRC_BEG_ADDR_SHADOW = (unsigned long)obj->member.dma.ptr;
    obj->member.reg->DST_BEG_ADDR_SHADOW = dataRegAddress;

    obj->member.reg->TRANSFER_SIZE = obj->member.dma.size - 1;
    obj->member.reg->SRC_TRANSFER_STEP = 1;
    obj->member.reg->DST_TRANSFER_STEP = 0;

    obj->member.reg->MODE.bit.CONTINUOUS = 0;
    obj->member.reg->MODE.bit.PERINTSEL = dmaChannel;
    obj->member.reg->MODE.bit.PERINTE = 1;
    obj->member.reg->CONTROL.bit.RUN = 0;
    EDIS;
}
/**************************************************************************************************
Function Name:
    void DMA_LIN_SCI_Mode_TX_Init(DmaLinObj *obj, unsigned long dataRegAddress)
Input:
    *obj            - Pointer of DMA driver object.
    dataRegAddress  - Address of data register.
Output:
    None.
Comment:
    DMA initialization function. This function will configure the DMA's LINBUS transmission
request.
**************************************************************************************************/
void DMA_LIN_SCI_9_Bit_Mode_TX_Init(DmaLinObj *obj, unsigned long dataRegAddress)
{
    unsigned long shift = 0, dmaChannel = (((unsigned long)obj->member.reg - DMA_CHANNEL_BASE) >> 5) + 1;
    unsigned long linRegBase = dataRegAddress & 0xFFFFFF00;
    unsigned long dmaTriggerValue = (linRegBase == LINA_Register_BASE) ? DMA_LINA_TX_TRIGGER : DMA_LINB_TX_TRIGGER;

    EALLOW;
    if (dmaChannel > 0 && dmaChannel <= 4)
    {
        shift = (dmaChannel - 1) * 8;
        DmaClaSrcSelRegs.DMACHSRCSEL1.all &= ~(0xFFL << shift);
        DmaClaSrcSelRegs.DMACHSRCSEL1.all |= dmaTriggerValue << shift;
    }
    else if (dmaChannel > 4 && dmaChannel <= 6)
    {
        shift = (dmaChannel - 5) * 8;
        DmaClaSrcSelRegs.DMACHSRCSEL2.all &= ~(0xFFL << shift);
        DmaClaSrcSelRegs.DMACHSRCSEL2.all |= dmaTriggerValue << shift;
    }

    // Channel configuration
    obj->member.reg->CONTROL.bit.SOFTRESET = 1;
    obj->member.reg->BURST_SIZE.bit.BURSTSIZE = 1;
    obj->member.reg->SRC_BURST_STEP = 1;
    obj->member.reg->DST_BURST_STEP = 28;

    obj->member.reg->SRC_ADDR_SHADOW = (unsigned long)obj->member.dma.ptr;
    obj->member.reg->DST_ADDR_SHADOW = dataRegAddress - 28;     // Shift to SCIFLR

    obj->member.reg->SRC_BEG_ADDR_SHADOW = (unsigned long)obj->member.dma.ptr;
    obj->member.reg->DST_BEG_ADDR_SHADOW = dataRegAddress - 28; // Shift to SCIFLR

    obj->member.reg->TRANSFER_SIZE = obj->member.dma.size - 1;
    obj->member.reg->SRC_TRANSFER_STEP = 1;
    obj->member.reg->DST_TRANSFER_STEP = 0;

    obj->member.reg->DST_WRAP_SIZE = 0;
    obj->member.reg->DST_WRAP_STEP = 0;

    obj->member.reg->MODE.bit.CONTINUOUS = 0;
    obj->member.reg->MODE.bit.PERINTSEL = dmaChannel;
    obj->member.reg->MODE.bit.PERINTE = 1;
    obj->member.reg->CONTROL.bit.RUN = 0;
    EDIS;
}
/**************************************************************************************************
Function Name:
    unsigned char Write_DMA_Data(DmaUartObj *obj)
Input:
    *obj    - Pointer of DMA driver object.
Output:
    return  - Result. 1 = completed. 0 = does not complete.
Comment:
    Write memory of DMA function.
**************************************************************************************************/
unsigned char Write_DMA_Data(DmaLinObj *obj)
{
    if (!obj->member.reg->CONTROL.bit.RUNSTS)
    {
        int i;

        for (i = 0; i < obj->member.dataAmount; i++)
            *(obj->member.dma.ptr + i) = *(obj->member.buffer.ptr + i);

        EALLOW;
        obj->member.reg->TRANSFER_SIZE = i - 1;
        obj->member.reg->CONTROL.bit.RUN = 1;
        EDIS;
        return 1;
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char Write_DMA_LIN_SCI_9_Bit_Mode_Data(DmaLinObj *obj)
Input:
    *obj    - Pointer of DMA driver object.
Output:
    return  - Result. 1 = completed. 0 = does not complete.
Comment:
    Write memory of DMA function. This function is based on "Write_DMA_Data" function. The DMA
transfers RAM data to SCITD and SCIFLR because the ninth bit of transmit data is set by TXWAKE
bit of SCIFLR.
**************************************************************************************************/
unsigned char Write_DMA_LIN_SCI_9_Bit_Mode_Data(DmaLinObj *obj)
{
    if (!obj->member.reg->CONTROL.bit.RUNSTS)
    {
        int i;

        for (i = 0; i < obj->member.dataAmount; i++)
        {
            *(obj->member.dma.ptr + 2 * i) = (*(obj->member.buffer.ptr + i) & 0x100) << 2;
            *(obj->member.dma.ptr + 2 * i + 1) = *(obj->member.buffer.ptr + i);
        }

        EALLOW;
        obj->member.reg->TRANSFER_SIZE = i - 1;
        obj->member.reg->CONTROL.bit.RUN = 1;
        EDIS;
        return 1;
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char Read_Circular_DMA_Data(DmaLinObj *obj)
Input:
    *obj    - Pointer of DMA driver object.
Output:
    return  - Result. 1 = completed. 0 = does not complete.
Comment:
    Read memory of DMA function. This function will extract the unread data to the buffer of DMA
object. The driver uses DST_WRAP_COUNT rather than DST_TRANSFER_COUNT to detect the number of the
transfer because DST_TRANSFER_COUNT is zero in the beginning and ending address transfer when the
continuous mode is enabled.
**************************************************************************************************/
unsigned char Read_Circular_DMA_Data(DmaLinObj *obj)
{
    int stop = (0xFFFF - obj->member.reg->DST_WRAP_COUNT) & (obj->member.dma.size - 1);

    if (stop != obj->member.index && obj->member.reg->DST_WRAP_COUNT)
    {
        int byteCount = ((obj->member.index > stop) * obj->member.dma.size + stop - obj->member.index) &
                        (obj->member.dma.size - 1);

        if (obj->member.buffer.size >= byteCount)
        {
            int i;

            for(i = 0; i < byteCount; i++)
            {
                *(obj->member.buffer.ptr + i) = *(obj->member.dma.ptr + obj->member.index);
                obj->member.index = (obj->member.index + 1) & (obj->member.dma.size - 1);
            }
            obj->member.dataAmount = byteCount;
            return 1;
        }
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char Read_Circular_DMA_LIN_SCI_9_Bit_Mode_Data(DmaLinObj *obj)
Input:
    *obj    - Pointer of DMA driver object.
Output:
    return  - Result. 1 = completed. 0 = does not complete.
Comment:
    Read memory of DMA function for SCI 9bit mode DMA implementation. This function is based on
"Read_Circular_DMA_Data" function. The DMA transfers SCIRD and SCIFLR data to RAM because the
ninth bit of received data is present in RXWAKE bit of SCIFLR.
**************************************************************************************************/
unsigned char Read_Circular_DMA_LIN_SCI_9_Bit_Mode_Data(DmaLinObj *obj)
{
    int stop = (2 * (0xFFFF - obj->member.reg->DST_WRAP_COUNT)) & (obj->member.dma.size - 1);

    if (stop != obj->member.index && obj->member.reg->DST_WRAP_COUNT)
    {
        int byteCount = ((obj->member.index > stop) * obj->member.dma.size + stop - obj->member.index) &
                        (obj->member.dma.size - 1);

        if (obj->member.buffer.size >= byteCount)
        {
            int i;

            for(i = 0; i < (byteCount >> 1); i++)
            {
                *(obj->member.buffer.ptr + i) = (*(obj->member.dma.ptr + obj->member.index) & 0x1000) >> 4;
                obj->member.index = (obj->member.index + 1) & (obj->member.dma.size - 1);
                *(obj->member.buffer.ptr + i) |= *(obj->member.dma.ptr + obj->member.index);
                obj->member.index = (obj->member.index + 1) & (obj->member.dma.size - 1);
            }
            obj->member.dataAmount = byteCount >> 1;
            return 1;
        }
    }
    return 0;
}
//---------------- END LINE -----------------------------------------------------------------------
