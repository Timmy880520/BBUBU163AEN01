/**************************************************************************************************
File Name: f28003x_dma_can_driver.c
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
    Universal DMA configuration driver for CANBUS reception. The driver shall be created and
filled in the required data in the parameters of the driver before executing any function in the
driver.

    This driver does not set DMACTRL register. Therefore, Please ensure HW_Reeset has executed
before using the driver.
==================================================================================================
History:
    Date       Author           Description Of Change
    ---------- ---------------- -----------------------------------------------------------------
    02/23/2023 Watch Lee        1. version 1.0.
                                2. only support CAN_RX.

**************************************************************************************************/

/* Includes */
#include "f28003x_dma_can_driver.h"

/**************************************************************************************************
Function Name:
    void DMA_CAN_RX_Init(DmaLinObj *obj, unsigned long dataRegAddress)
Input:
    *obj            - Pointer of DMA driver object.
    dataRegAddress  - Address of data register.
Output:
    None.
Comment:
    DMA CANRX initialization function. This function will configure the DMA's CANBUS reception
request.
**************************************************************************************************/
void DMA_CAN_RX_Init(DmaCanObj *obj, unsigned long dataRegAddress)
{
    unsigned long shift = 0, dmaChannel = (((unsigned long)obj->member.reg - DMA_CHANNEL_BASE) >> 5) + 1;
    struct CAN_REGS *canRegs = (struct CAN_REGS*)(dataRegAddress & 0xFFFFF000);

    EALLOW;

    if (dmaChannel > 0 && dmaChannel <= 4)
    {
        shift = (dmaChannel - 1) * 8;
        DmaClaSrcSelRegs.DMACHSRCSEL1.all &= ~(0xFFL << shift);

        if ((unsigned long)canRegs == DMA_CANA_REGS_BASE)
            DmaClaSrcSelRegs.DMACHSRCSEL1.all |= DMA_CANA_RX_TRIGGER << shift;
    }
    else if (dmaChannel > 4 && dmaChannel <= 6)
    {
        shift = (dmaChannel - 5) * 8;
        DmaClaSrcSelRegs.DMACHSRCSEL2.all &= ~(0xFFL << shift);

        if ((unsigned long)canRegs == DMA_CANA_REGS_BASE)
            DmaClaSrcSelRegs.DMACHSRCSEL2.all |= DMA_CANA_RX_TRIGGER << shift;
    }

    // Channel configuration
    obj->member.reg->BURST_SIZE.bit.BURSTSIZE = 7;       // 32bit * 4 / 16bit - 1 = 7
    obj->member.reg->SRC_BURST_STEP = 4;                 // CAN peripheral address offset = 8bit, 32bit / 8bit = 4
    obj->member.reg->DST_BURST_STEP = 2;                 // Common address offset = 16bit, 32bit / 16bit = 2

    obj->member.reg->SRC_ADDR_SHADOW = (unsigned long)&canRegs->CAN_IF3ARB;
    obj->member.reg->DST_ADDR_SHADOW = (unsigned long)obj->member.dma.ptr;

    obj->member.reg->SRC_BEG_ADDR_SHADOW = (unsigned long)&canRegs->CAN_IF3ARB;
    obj->member.reg->DST_BEG_ADDR_SHADOW = (unsigned long)obj->member.dma.ptr;

    obj->member.reg->TRANSFER_SIZE = obj->member.dma.size - 1;
    obj->member.reg->SRC_TRANSFER_STEP = 0;
    obj->member.reg->DST_TRANSFER_STEP = 2;              // Common address offset = 16bit, 32bit / 16bit = 2

    obj->member.reg->SRC_WRAP_SIZE = 0;                  // WRAP_SIZE = TRANSFER_STEP = 0 to disable source wrap
    obj->member.reg->SRC_WRAP_STEP = 0;
    obj->member.reg->DST_WRAP_SIZE = 0xFFFF;             // WRAP_SIZE > TRANSFER_STEP to disable source wrap
    obj->member.reg->DST_WRAP_STEP = 0;

    obj->member.reg->MODE.bit.DATASIZE = 1;              // Transfer size = 32bit
    obj->member.reg->MODE.bit.CONTINUOUS = 1;            // Enable continuous mode
    obj->member.reg->MODE.bit.PERINTSEL = dmaChannel;
    obj->member.reg->MODE.bit.PERINTE = 1;
    obj->member.reg->CONTROL.bit.RUN = 1;
    EDIS;
}
/**************************************************************************************************
Function Name:
    unsigned char Read_CAN_RX_DMA_Data(DmaCanObj *obj)
Input:
    *obj    - Pointer of DMA driver object.
Output:
    return  - Result. 1 = completed. 0 = does not complete.
Comment:
    Read memory of DMA function for CANRX. This function will extract the unread CANBUS packet to
the buffer of DMA object. The driver uses DST_WRAP_COUNT rather than DST_TRANSFER_COUNT to detect
the number of the transfer because DST_TRANSFER_COUNT is zero in the beginning and ending address
transfer when the continuous mode is enabled.
**************************************************************************************************/
unsigned char Read_CAN_RX_DMA_Data(DmaCanObj *obj)
{
    unsigned short stop = (0xFFFF - obj->member.reg->DST_WRAP_COUNT) & (obj->member.dma.size - 1);

    if (stop != obj->member.index && obj->member.reg->DST_WRAP_COUNT)
    {
        for (int i = 0; i < 4; i++)
            (*obj->member.buffer.ptr)[i] = (*(obj->member.dma.ptr + obj->member.index))[i];
        obj->member.index = (obj->member.index + 1) & (obj->member.dma.size - 1);
        return 1;
    }
    return 0;
}
//---------------- END LINE -----------------------------------------------------------------------
