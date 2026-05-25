/**************************************************************************************************
File Name: f28003x_can_driver.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None.
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    EALLOW                                  F28003x_device.h
    EDIS                                    F28003x_device.h
Description:
    Universal CANBUS transmission and reception driver. The driver shall be created and filled in
the required data in the parameters of the driver before executing any function in the driver.

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    02/23/2023 Watch Lee        1. version 1.0.

**************************************************************************************************/

/* Includes */
#include "f28003x_can_driver.h"

/**************************************************************************************************
Function Name:
    void CAN_Init(CanObj *obj, unsigned long feature)
Input:
    *obj      - Pointer of driver object.
    feature   - DMA mode enable/disable.
Output:
    None.
Comment:
    CAN initialization function. This function will set bit timing register and control registers.
**************************************************************************************************/
void CAN_Init(CanObj *obj, unsigned long feature)
{
    EALLOW;
    obj->member.reg->CAN_CTL.bit.Init = 1;          // Enable initialization mode

    obj->member.reg->CAN_RAM_INIT.all = 0x1A;       // Enable initialization of CAN message RAM

    while (obj->member.reg->CAN_RAM_INIT.all != 0x25);

    obj->member.reg->CAN_CTL.bit.SWR = 1;           // Enable software reset

    while (obj->member.reg->CAN_CTL.bit.SWR);

    obj->member.reg->CAN_CTL.bit.CCE = 1;           // Enable configuration of the bit timing
    obj->member.reg->CAN_CTL.bit.ABO = 1;           // Enable auto Bus On
    obj->member.reg->CAN_CTL.bit.Test = 0;          // Disable test mode
    obj->member.reg->CAN_TEST.bit.EXL = 0;          // External loop test mode
    obj->member.reg->CAN_CTL.bit.DE3 = ((feature & DMA_CANRX_ENABLE) == DMA_CANRX_ENABLE) ? 1 : 0;
    obj->member.reg->CAN_BTR.all = obj->member.bitTimingValue;

    obj->member.reg->CAN_CTL.bit.Init = 0;          // Disable initialization mode
    obj->member.reg->CAN_CTL.bit.CCE = 0;           // Disable configuration of the bit timing
    EDIS;

    if ((feature & DMA_CANRX_ENABLE) == DMA_CANRX_ENABLE)
    {
        // IF3 Configuration(for DMA reception, using mailbox 1-31)
        obj->member.reg->CAN_IF3OBS.bit.Mask = 0;       // Disable mask section observation
        obj->member.reg->CAN_IF3OBS.bit.Arb = 1;        // Enable arbitration section observation
        obj->member.reg->CAN_IF3OBS.bit.Ctrl = 1;       // Enable ctrl section observation
        obj->member.reg->CAN_IF3OBS.bit.Data_A = 1;     // Enable Data A section observation
        obj->member.reg->CAN_IF3OBS.bit.Data_B = 1;     // Enable Data B section observation
        obj->member.reg->CAN_IF3UPD = 0x7FFFFFFF;       // Bit0 is for mailbox1, Bit1 is for mailbox2, ...etc.
    }

    // IF2 Configuration(for reception, using mailbox 1-31)
    while (obj->member.reg->CAN_IF2CMD.bit.Busy);

    obj->member.reg->CAN_IF2CMD.bit.DIR = 1;
    obj->member.reg->CAN_IF2CMD.bit.Arb = 1;
    obj->member.reg->CAN_IF2CMD.bit.Control = 1;
    obj->member.reg->CAN_IF2CMD.bit.Mask = 1;
    obj->member.reg->CAN_IF2CMD.bit.DATA_A = 1;
    obj->member.reg->CAN_IF2CMD.bit.DATA_B = 1;
    obj->member.reg->CAN_IF2MSK.bit.MXtd = 1;
    obj->member.reg->CAN_IF2MSK.bit.MDir = 1;

    obj->member.reg->CAN_IF2MCTL.bit.EoB = 1;       // Setup end of FIFO
    obj->member.reg->CAN_IF2MCTL.bit.UMask = 1;

    obj->member.reg->CAN_IF2ARB.bit.MsgVal = 0;     // Disabled for default
    obj->member.reg->CAN_IF2ARB.bit.Xtd = 1;
    obj->member.reg->CAN_IF2ARB.bit.Dir = 0;
    obj->member.reg->CAN_IF2CMD.bit.MSG_NUM = 31;   // Setup the last reception message RAM

    // IF1 Configuration(for Transmission, using mailbox 32)
    while (obj->member.reg->CAN_IF1CMD.bit.Busy);

    obj->member.reg->CAN_IF1CMD.bit.DIR = 1;        // Write IF registers to mailbox
    obj->member.reg->CAN_IF1CMD.bit.Arb = 1;
    obj->member.reg->CAN_IF1CMD.bit.Control = 1;    // Enable access control
    obj->member.reg->CAN_IF1CMD.bit.Mask = 0;
    obj->member.reg->CAN_IF1MSK.bit.MXtd = 0;
    obj->member.reg->CAN_IF1MCTL.bit.EoB = 1;
    obj->member.reg->CAN_IF1MCTL.bit.UMask = 0;

    obj->member.reg->CAN_IF1ARB.bit.MsgVal = 1;
    obj->member.reg->CAN_IF1ARB.bit.Xtd = 1;
    obj->member.reg->CAN_IF1ARB.bit.Dir = 1;
    obj->member.reg->CAN_IF1CMD.bit.DATA_A = 1;
    obj->member.reg->CAN_IF1CMD.bit.DATA_B = 1;
    obj->member.reg->CAN_IF1CMD.bit.MSG_NUM = 32;    // Setup mailbox32
}
/**************************************************************************************************
Function Name:
    unsigned char CAN_Set_Rx_Mailbox(CanObj *obj, unsigned char number, unsigned long id, unsigned long mask)
Input:
    *obj      - Pointer of driver object.
    number    - Mailbox number.
    id        - CAN identifier.
    mask      - CAN mask for identifier filter.(bit definition: 1 = must match, 0 = don't care)
Output:
    return    - Result. 1 = completed. 0 = does not complete.
Comment:
    CANBUS setup reception mailbox function. This function shall be executed before receiving
CANBUS data.
**************************************************************************************************/
unsigned char CAN_Set_Rx_Mailbox(CanObj *obj, unsigned char number, unsigned long id, unsigned long mask)
{
    unsigned short timeout = 0;

    if (number < 32)
    {
        while (obj->member.reg->CAN_IF2CMD.bit.Busy);

        obj->member.reg->CAN_IF2ARB.bit.MsgVal = 0;
        obj->member.reg->CAN_IF2CMD.bit.MSG_NUM = number;

        while (obj->member.reg->CAN_MVAL_21 & (1 << (number - 1)) && timeout < 256)
            timeout++;

        timeout = (timeout < 256) ? 0 : 256;

        if (number == 31)
            obj->member.reg->CAN_IF2MCTL.bit.EoB = 1;
        else
            obj->member.reg->CAN_IF2MCTL.bit.EoB = 0;
        obj->member.reg->CAN_IF2ARB.bit.MsgVal = 1;
        obj->member.reg->CAN_IF2ARB.bit.ID = id;
        obj->member.reg->CAN_IF2MSK.bit.Msk = mask;
        obj->member.reg->CAN_IF2CMD.bit.DIR = 1;
        obj->member.reg->CAN_IF2CMD.bit.MSG_NUM = number;

        while (!obj->member.reg->CAN_MVAL_21 & (1 << (number - 1)) && timeout < 256)
            timeout++;
        return (timeout < 256);
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char CAN_Get_Rx_ID_Filter(CanObj *obj, unsigned char number, unsigned long *id, unsigned long *mask)
Input:
    *obj      - Pointer of driver object.
    number    - Mailbox number.
    *id       - CAN identifier.
    *mask     - CAN mask for identifier filter.(bit definition: 1 = must match, 0 = don't care)
Output:
    return    - Result. 1 = completed. 0 = does not complete.
Comment:
    Reading CANBUS reception identifier and mask function.
**************************************************************************************************/
unsigned char CAN_Get_Rx_ID_Filter(CanObj *obj, unsigned char number, unsigned long *id, unsigned long *mask)
{
    unsigned short timeout = 0;

    if (number < 32)
    {
        while (obj->member.reg->CAN_IF2CMD.bit.Busy);

        obj->member.reg->CAN_IF2CMD.bit.DIR = 0;
        obj->member.reg->CAN_IF2CMD.bit.Control = 1;
        obj->member.reg->CAN_IF2CMD.bit.MSG_NUM = number;

        while (++timeout < 256);

        *id = obj->member.reg->CAN_IF2ARB.bit.ID;
        *mask = obj->member.reg->CAN_IF2MSK.bit.Msk;
        return 1;
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char CAN_Transmit(void *obj, void *packet, unsigned char size)
Input:
    *obj      - Pointer of driver object.
    *packet   - Pointer of CANBUS data packet.
    size      - Size of CANBUS data packet.
Output:
    return    - Result. 1 = completed. 0 = does not complete.
Comment:
    CANBUS transmission function.
**************************************************************************************************/
unsigned char CAN_Transmit(void *obj, void *packet, unsigned char size)
{
    CanObj *txObj = obj;
    CanPacketType txPacket = {.arb.all = 0, .mctrl.all = 0, .datA.all = 0, .datB.all = 0};
    unsigned char *buffer = packet;

    if (size >= 4 && size <= 12)
    {
/*        for (int i = 0; i < 4; i++)
        {
            txPacket.arb.all |= (((unsigned long)buffer[i + 0] & 0xFF) << (i * 8));
            txPacket.datA.all |= (((unsigned long)buffer[i + 4] & 0xFF) << (i * 8));
            txPacket.datB.all |= (((unsigned long)buffer[i + 8] & 0xFF) << (i * 8));
        }
*/
        txPacket.arb.all = (((unsigned long)buffer[0] & 0xFF) << 0) + (((unsigned long)buffer[1] & 0xFF) << 8) +
                           (((unsigned long)buffer[2] & 0xFF) << 16) + (((unsigned long)buffer[3] & 0xFF) << 24);

        txPacket.datA.all = (((unsigned long)buffer[4] & 0xFF) << 0) + (((unsigned long)buffer[5] & 0xFF) << 8) +
                            (((unsigned long)buffer[6] & 0xFF) << 16) + (((unsigned long)buffer[7] & 0xFF) << 24);

        txPacket.datB.all = (((unsigned long)buffer[8] & 0xFF) << 0) + (((unsigned long)buffer[9] & 0xFF) << 8) +
                            (((unsigned long)buffer[10] & 0xFF) << 16) + (((unsigned long)buffer[11] & 0xFF) << 24);

        txPacket.mctrl.bit.dlc = size - 4;
    }

    if (txObj && !(txObj->member.reg->CAN_TXRQ_21 & 0x80000000))
    {
        txObj->member.reg->CAN_IF1ARB.bit.ID = txPacket.arb.bit.id;
        txObj->member.reg->CAN_IF1MCTL.bit.DLC = txPacket.mctrl.bit.dlc;
        txObj->member.reg->CAN_IF1DATA.all = txPacket.datA.all;
        txObj->member.reg->CAN_IF1DATB.all = txPacket.datB.all;
        txObj->member.reg->CAN_IF1CMD.bit.TXRQST = 1;

        unsigned short timeout = 0;

        while ((txObj->member.reg->CAN_TXRQ_21 & 0x80000000) && timeout < 32767)
            timeout++;
        return (timeout < 32767);
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char CAN_Receive(void *obj, void **packet, unsigned char *size)
Input:
    *obj      - Pointer of driver object.
    **packet  - Pointer of the address of CANBUS data packet.
    *size     - Pointer of size variable of CANBUS data packet.
Output:
    return    - Result. 1 = completed. 0 = does not complete.
Comment:
    CANBUS reception function. This function will transfer the received data packet address to
buffer.
**************************************************************************************************/
unsigned char CAN_Receive(void *obj, void **packet, unsigned char *size)
{
    CanPacketType rxPacket = {.arb.all = 0, .mctrl.all = 0, .datA.all = 0, .datB.all = 0};
    static unsigned char rxBuffer[12];
    CanObj *rxObj = obj;

    if (rxObj && (rxObj->member.reg->CAN_NDAT_21 & 0x7FFFFFFF))
    {
        unsigned long i = 1, rxFlag = rxObj->member.reg->CAN_NDAT_21;

        while (!(rxFlag & 1) && i < 31)
        {
            rxFlag >>= 1;
            i++;
        }

        while (rxObj->member.reg->CAN_IF2CMD.bit.Busy);

        rxObj->member.reg->CAN_IF2CMD.bit.DIR = 0;
        rxObj->member.reg->CAN_IF2CMD.bit.Control = 1;
        rxObj->member.reg->CAN_IF2CMD.bit.TxRqst = 1;
        rxObj->member.reg->CAN_IF2CMD.bit.MSG_NUM = i;

        unsigned short timeout = 0;

        while (rxObj->member.reg->CAN_NDAT_21 & (1 << (i - 1)) && timeout < 256)
            timeout++;

        rxPacket.arb.all = rxObj->member.reg->CAN_IF2ARB.all;
        rxPacket.mctrl.all = rxObj->member.reg->CAN_IF2MCTL.all;

        if (rxPacket.mctrl.bit.dlc > 4)
        {
            rxPacket.datA.all = rxObj->member.reg->CAN_IF2DATA.all;
            rxPacket.datB.all = rxObj->member.reg->CAN_IF2DATB.all;
        }
        else if (rxPacket.mctrl.bit.dlc > 0)
        {
            rxPacket.datA.all = rxObj->member.reg->CAN_IF2DATA.all;
            rxPacket.datB.all = 0;
        }
        else
        {
            rxPacket.datA.all = 0;
            rxPacket.datB.all = 0;
        }

        for (int i = 0; i < 4; i++)
        {
            rxBuffer[0 + i] = (rxPacket.arb.all >> (i * 8)) & 0xFF;
            rxBuffer[4 + i] = (rxPacket.datA.all >> (i * 8)) & 0xFF;
            rxBuffer[8 + i] = (rxPacket.datB.all >> (i * 8)) & 0xFF;
        }
        *packet = rxBuffer;
        *size = rxPacket.mctrl.bit.dlc + 4;
        return (timeout < 256);
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char CAN_DMA_Receive(void *obj, void **packet, unsigned char *size)
Input:
    *obj      - Pointer of DMA peripheral driver object.
    **packet  - Pointer of the address of CANBUS data packet.
    *size     - Pointer of size variable of CANBUS data packet.
Output:
    return    - Result. 1 = completed. 0 = does not complete.
Comment:
    CANBUS DMA reception function. This function will transfer the address of received CANBUS
packet to buffer.
**************************************************************************************************/
unsigned char CAN_DMA_Receive(void *obj, void **packet, unsigned char *size)
{
    static unsigned char rxBuffer[12];

    if (obj && ((DmaCanObj*)obj)->func.accessDmaData(obj))
    {
        CanPacketType *rxPacket = (CanPacketType*)(((DmaCanObj*)obj)->member.buffer.ptr);

        for (int i = 0; i < 4; i++)
        {
            rxBuffer[0 + i] = (rxPacket->arb.all >> (i * 8)) & 0xFF;
            rxBuffer[4 + i] = (rxPacket->datA.all >> (i * 8)) & 0xFF;
            rxBuffer[8 + i] = (rxPacket->datB.all >> (i * 8)) & 0xFF;
        }
        *packet = rxBuffer;
        *size = rxPacket->mctrl.bit.dlc + 4;
        return 1;
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    void CAN_SW_Reset(CanObj *obj)
Input:
    *obj      - Pointer of driver object.
Output:
    None.
Comment:
    CANBUS software reset function. This function will reset all CAN registers to default.
**************************************************************************************************/
void CAN_SW_Reset(CanObj *obj)
{
    EALLOW;

    if ((unsigned long)obj->member.reg == DMA_CANA_REGS_BASE)
    {
        DevCfgRegs.SOFTPRES10.bit.CAN_A = 1;
        DevCfgRegs.SOFTPRES10.bit.CAN_A = 0;
    }
    EDIS;
}
/**************************************************************************************************
Function Name:
    unsigned char CAN_Get_Error_Status(CanObj *obj)
Input:
    *obj      - Pointer of driver object.
Output:
    return    - Result. 1 = CANBus error occurred. 0 = No CANBus error.
Comment:
    Get CANBus error status function. Error flags are defined below:
    EPVF flag  = Error passive
    EWGF flag  = Warning status
    BOFF flag  = Bus_Off status
**************************************************************************************************/
unsigned char CAN_Get_Error_Status(CanObj *obj)
{
    return (obj->member.reg->CAN_ES.bit.EPass ||
            obj->member.reg->CAN_ES.bit.EWarn ||
            obj->member.reg->CAN_ES.bit.PER ||
            obj->member.reg->CAN_ES.bit.BOff);
}
//---------------- END LINE -----------------------------------------------------------------------
