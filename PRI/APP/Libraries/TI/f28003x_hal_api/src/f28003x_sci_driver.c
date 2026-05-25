/**************************************************************************************************
File Name: f28003x_sci_driver.c
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
    Universal SCI transmission and reception driver. The driver shall be created and filled in
the required data in the parameters of the driver before executing any function in the driver.

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    02/23/2023 Watch Lee        1. version 1.0.

**************************************************************************************************/

/* Includes */
#include "f28003x_sci_driver.h"

/**************************************************************************************************
Function Name:
    void SCI_Init(SciObj *obj, unsigned long feature)
Input:
    *obj      - Pointer of driver object.
    feature   - Enumeration variable for configuring SCI features.
Output:
    None.
Comment:
    SCI initialization function. This function will set baud registers and control registers.
**************************************************************************************************/
void SCI_Init(SciObj *obj, unsigned long feature)
{
    obj->member.reg->SCIFFTX.bit.SCIRST = 0;
    obj->member.reg->SCIFFTX.bit.SCIRST = 1;

    obj->member.reg->SCICTL1.bit.SWRESET = 0;
    obj->member.reg->SCICTL1.bit.RXENA = 0;
    obj->member.reg->SCICTL1.bit.TXENA = 0;
    obj->member.reg->SCIFFTX.bit.SCIFFENA = 0;

    obj->member.reg->SCICCR.bit.SCICHAR = 7;        // 8 bits character length
    obj->member.reg->SCICCR.bit.PARITYENA = 0;      // No parity
    obj->member.reg->SCICCR.bit.STOPBITS = 0;       // 1 stop bit
    obj->member.reg->SCICCR.bit.ADDRIDLE_MODE = ((feature & 2) == SCI_9_BIT) ? 1 : 0;

    obj->member.reg->SCILBAUD.bit.BAUD = obj->member.brrValue & 0xFF;
    obj->member.reg->SCIHBAUD.bit.BAUD = (obj->member.brrValue >> 8) & 0xFF;

    obj->member.reg->SCIFFTX.bit.SCIFFENA = ((feature & 1) == SCI_FIFO_ENABLE) ? 1 : 0;
    obj->member.reg->SCICTL1.bit.RXENA = 1;
    obj->member.reg->SCICTL1.bit.TXENA = 1;
    obj->member.reg->SCICTL1.bit.SWRESET = 1;
}
/**************************************************************************************************
Function Name:
    unsigned char SCI_Transmit(void *obj, void *buffer, unsigned char size)
Input:
    *obj      - Pointer of driver object.
    *buffer   - Pointer of the data array.
    size      - Size of the data array.
Output:
    return    - Result. 1 = completed. 0 = does not complete.
Comment:
    SCI transmission function.
**************************************************************************************************/
unsigned char SCI_Transmit(void *obj, void *buffer, unsigned char size)
{
    SciObj *txObj = obj;

    if (txObj)
    {
        unsigned short *word = buffer;
        unsigned short addressBit = txObj->member.reg->SCICCR.bit.ADDRIDLE_MODE;

        if (txObj->member.reg->SCIFFTX.bit.SCIFFENA && txObj->member.reg->SCIFFTX.bit.TXFFST < 16)
        {
            unsigned short fifoSize = 16 - txObj->member.reg->SCIFFTX.bit.TXFFST;
            unsigned short remain = size - txObj->member.txDataCount;

            remain = (fifoSize > remain) ? remain : fifoSize;

            for (int i = 0; i < remain; i++)
            {
                addressBit &= (word[txObj->member.txDataCount] >> 8) & 1;
                txObj->member.reg->SCICTL1.bit.TXWAKE = addressBit;
                txObj->member.reg->SCITXBUF.bit.TXDT = word[txObj->member.txDataCount];
                txObj->member.txDataCount++;
            }

            if (txObj->member.txDataCount >= size)
            {
                txObj->member.txDataCount = 0;
                return 1;
            }
        }
        else if (txObj->member.reg->SCICTL2.bit.TXRDY)
        {
            if (txObj->member.txDataCount < size)
            {
                addressBit &= (word[txObj->member.txDataCount] >> 8) & 1;
                txObj->member.reg->SCICTL1.bit.TXWAKE = addressBit;
                txObj->member.reg->SCITXBUF.bit.TXDT = word[txObj->member.txDataCount];
                txObj->member.txDataCount++;
            }
            else
            {
                txObj->member.txDataCount = 0;
                return 1;
            }
        }
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char SCI_Receive(void *obj, void **buffer, unsigned char *size)
Input:
    *obj      - Pointer of driver object.
    **buffer  - Pointer of the address of the data array.
    *size     - Pointer of size variable of the data array.
Output:
    return    - Result. 1 = completed. 0 = does not complete.
Comment:
    SCI reception function. This function will transfer the received data to the array which the
first parameter points to and update the data size to the size variable which the second parameter
points to.
**************************************************************************************************/
unsigned char SCI_Receive(void *obj, void **buffer, unsigned char *size)
{
    static unsigned short rxData[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    SciObj *rxObj = obj;

    if (rxObj)
    {
        unsigned short addressBit = (rxObj->member.reg->SCIRXST.bit.RXWAKE &&
                                     rxObj->member.reg->SCICCR.bit.ADDRIDLE_MODE);

        if (rxObj->member.reg->SCIFFTX.bit.SCIFFENA && rxObj->member.reg->SCIFFRX.bit.RXFFST)
        {
            // FIFO mode
            unsigned short count = rxObj->member.reg->SCIFFRX.bit.RXFFST;

            for (int i = 0; i < count; i++)
                rxData[i] = rxObj->member.reg->SCIRXBUF.bit.SAR;
            rxData[0] |= addressBit << 8;
            *buffer = rxData;
            *size = count;
            return 1;
        }
        else if (rxObj->member.reg->SCIRXST.bit.RXRDY)
        {
            // Standard mode
            rxData[0] = (unsigned short)rxObj->member.reg->SCIRXBUF.bit.SAR | (addressBit << 8);
            *buffer = rxData;
            *size = 1;
            return 1;
        }
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    void SCI_Clear_State_Register(SciObj *obj)
Input:
    *obj      - Pointer of driver object.
Output:
    return    - Result. 1 = completed. 0 = does not complete.
Comment:
    SCI clear state function. This function will execute software reset if RXERROR flag is set.
**************************************************************************************************/
void SCI_Clear_State_Register(SciObj *obj)
{
    if (obj->member.reg->SCIRXST.bit.RXERROR)
    {
        obj->member.reg->SCICTL1.bit.SWRESET = 0;
        obj->member.reg->SCICTL1.bit.SWRESET = 1;
    }
}
//---------------- END LINE -----------------------------------------------------------------------
