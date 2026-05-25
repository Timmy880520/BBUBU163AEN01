/**************************************************************************************************
File Name: f28003x_lin_driver.c
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
    Universal LINBUS transmission and reception driver. The driver shall be created and filled in
the required data in the parameters of the driver before executing any function in the driver.

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    02/22/2023 Watch Lee        1. version 1.0.
                                2. only support SCI mode.

**************************************************************************************************/

/* Includes */
#include "f28003x_lin_driver.h"

/**************************************************************************************************
Function Name:
    void LIN_SCI_Mode_Init(LinObj *obj, unsigned long feature)
Input:
    *obj      - Pointer of driver object.
    feature   - Enumeration variable for configuring LIN_SCI features.
Output:
    None.
Comment:
    UART initialization function. This function will set the mantissa and fraction of baud rate
register and control registers.
**************************************************************************************************/
void LIN_SCI_Mode_Init(LinObj *obj, unsigned long feature)
{
    EALLOW;
    obj->member.reg->SCIGCR0.bit.RESET = 0;
    obj->member.reg->SCIGCR0.bit.RESET = 1;

    obj->member.reg->SCIGCR1.bit.SWnRST = 0;
    obj->member.reg->SCIGCR1.bit.TXENA = 1;
    obj->member.reg->SCIGCR1.bit.RXENA = 1;
    obj->member.reg->SCIGCR1.bit.CONT = 1;
    obj->member.reg->SCIGCR1.bit.CLK_MASTER = 1;
    obj->member.reg->SCIGCR1.bit.TIMINGMODE = 1;

    obj->member.reg->SCIFORMAT.bit.LENGTH = 0;
    obj->member.reg->SCIFORMAT.bit.CHAR = 7;

    obj->member.reg->SCIPIO0.bit.TXFUNC = 1;
    obj->member.reg->SCIPIO0.bit.RXFUNC = 1;

    obj->member.reg->BRSR.bit.SCI_LIN_PSH = obj->member.pValue >> 16;
    obj->member.reg->BRSR.bit.SCI_LIN_PSL = obj->member.pValue & 0xFFFF;
    obj->member.reg->BRSR.bit.M = obj->member.mValue;

    obj->member.reg->SCISETINT.bit.SET_TX_DMA = ((feature & 2) == DMA_LINTX_ENABLE) ? 1 : 0;
    obj->member.reg->SCISETINT.bit.SET_RX_DMA = ((feature & 1) == DMA_LINRX_ENABLE) ? 1 : 0;
    obj->member.reg->SCISETINT.bit.SET_RX_DMA_ALL = obj->member.reg->SCISETINT.bit.SET_RX_DMA;
    obj->member.reg->SCIGCR1.bit.COMMMODE = ((feature & 4) == DATA_9_BIT) ? 1 : 0;

    obj->member.reg->SCIGCR1.bit.SWnRST = 1;
    EDIS;
}
/**************************************************************************************************
Function Name:
    unsigned char LIN_SCI_Mode_Transmit(void *obj, void *buffer, unsigned char size)
Input:
    *obj      - Pointer of driver object.
    *buffer   - Pointer of the data array.
    size      - Size of the data array.
Output:
    return    - Result. 1 = completed. 0 = does not complete.
Comment:
    LINBUS SCI mode transmission function.
**************************************************************************************************/
unsigned char LIN_SCI_Mode_Transmit(void *obj, void *buffer, unsigned char size)
{
    if (obj && ((LinObj*)obj)->member.reg->SCIFLR.bit.TXRDY)
    {
        if (((LinObj*)obj)->member.txDataCount < size)
        {
            unsigned short *word = buffer;

            ((LinObj*)obj)->member.reg->SCIFLR.bit.TXWAKE = (word[((LinObj*)obj)->member.txDataCount] >> 8) & 1;
            ((LinObj*)obj)->member.reg->SCITD.all = word[((LinObj*)obj)->member.txDataCount];
            ((LinObj*)obj)->member.txDataCount++;
        }
        else
        {
            ((LinObj*)obj)->member.txDataCount = 0;
            return 1;
        }
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char LIN_SCI_Mode_Receive(void *obj, void **buffer, unsigned char *size)
Input:
    *obj      - Pointer of driver object.
    **buffer  - Pointer of the address of the data array.
    *size     - Pointer of size variable of the data array.
Output:
    return    - Result. 1 = completed. 0 = does not complete.
Comment:
    LINBUS SCI mode reception function. This function will transfer the received data to the array
which the first parameter points to and update the data size to the size variable which the second
parameter points to.
**************************************************************************************************/
unsigned char LIN_SCI_Mode_Receive(void *obj, void **buffer, unsigned char *size)
{
    static unsigned short rxData;

    if (obj && ((LinObj*)obj)->member.reg->SCIFLR.bit.RXRDY)
    {
        if (((LinObj*)obj)->member.reg->SCIGCR1.bit.COMMMODE)
        {
            rxData = ((LinObj*)obj)->member.reg->SCIFLR.bit.RXWAKE;
            rxData = (rxData << 8) | ((LinObj*)obj)->member.reg->SCIRD.all;
        }
        else
            rxData = ((LinObj*)obj)->member.reg->SCIRD.all;
        *buffer = &rxData;
        *size = 1;
        return 1;
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char LIN_SCI_Mode_DMA_Transmit(void *obj, void *buffer, unsigned char size)
Input:
    *obj      - Pointer of DMA peripheral driver object.
    *buffer   - Pointer of the data array.
    size      - Size of the data array.
Output:
    return    - Result. 1 = completed. 0 = does not complete.
Comment:
    LINBUS SCI mode DMA transmission function.
**************************************************************************************************/
unsigned char LIN_SCI_Mode_DMA_Transmit(void *obj, void *buffer, unsigned char size)
{
    unsigned short dmaSize = ((int)((DmaLinObj*)obj)->member.reg->SRC_BURST_STEP + 1) * size;

    if (obj && ((DmaLinObj*)obj)->member.dma.size >= dmaSize)
    {
        ((DmaLinObj*)obj)->member.buffer.ptr = buffer;
        ((DmaLinObj*)obj)->member.dataAmount = size;

        if(((DmaLinObj*)obj)->func.accessDmaData(obj))
            return 1;
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char LIN_SCI_Mode_DMA_Receive(void *obj, void **buffer, unsigned char *size)
Input:
    *obj      - Pointer of DMA peripheral driver object.
    **buffer  - Pointer of the address of the data array.
    *size     - Pointer of size variable of the data array.
Output:
    return    - Result. 1 = completed. 0 = does not complete.
Comment:
    LINBUS SCI mode DMA reception function. This function will transfer the received data to the
array which the first parameter points to and update the data size to the size variable which the
second parameter points to.
**************************************************************************************************/
unsigned char LIN_SCI_Mode_DMA_Receive(void *obj, void **buffer, unsigned char *size)
{
    if (obj && ((DmaLinObj*)obj)->func.accessDmaData(obj))
    {
        *buffer = ((DmaLinObj*)obj)->member.buffer.ptr;
        *size = ((DmaLinObj*)obj)->member.dataAmount;
        return 1;
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    void LIN_SCI_Mode_Clear_State_Register(LinObj *obj)
Input:
    *obj      - Pointer of driver object.
Output:
    return    - Result. 1 = completed. 0 = does not complete.
Comment:
    LINBUS SCI mode clear state function. This function will execute software reset if any error
flag is set.
**************************************************************************************************/
void LIN_SCI_Mode_Clear_State_Register(LinObj *obj)
{
    unsigned long errorFlags = obj->member.reg->SCIFLR.all;

    if ((errorFlags >> 24))
    {
        obj->member.reg->SCIGCR1.bit.SWnRST = 0;
        obj->member.reg->SCIGCR1.bit.SWnRST = 1;
    }
}
//---------------- END LINE -----------------------------------------------------------------------
