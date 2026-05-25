/**************************************************************************************************
File Name: f28004x_i2c_driver.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None.
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    EALLOW                                  F28004x_device.h
    EDIS                                    F28004x_device.h
Description:
    EEPROM I2C bus transmission and reception driver. The driver shall be created and filled in
the required data in the parameters of the driver before executing any function in the driver.

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    09/15/2020 Fred Huang       1. Ver. 1.0


**************************************************************************************************/

/* Includes */
#include "../../EventLog/Include/f28004x_i2c_driver.h"

#include "Io.h"
//------------------------------------------------------------------------------
// Local function prototypes
//------------------------------------------------------------------------------
unsigned char I2C_Master_Start_Packet(mI2cObj *obj, I2cTransmitMode trx);
unsigned char I2C_Master_Stop_Packet(mI2cObj *obj);
unsigned char I2C_Master_Transmit(mI2cObj *obj);
unsigned char I2C_Master_Receive(mI2cObj *obj);
unsigned char I2C_Master_TXD_Packet(mI2cObj *obj);
unsigned char I2C_Master_RXD_Packet(mI2cObj *obj);
void I2C_Master_Parameter_Reset(mI2cObj *obj);
//------------------------------------------------------------------------------
// Data definitions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------
/**************************************************************************************************
Function Name:
    unsigned char I2C_Master_Send_Start(mI2cObj *obj)
Input:
    *obj    - Pointer of driver object.
    trx     - Transfer mode select bit.
                1 = Transmitter mode(R/W = 0).
                0 = Receiver mode(R/W = 1).
Output:
    return  - Result. 1 = completed, 0 = not complete.
Comment:
    I2C master send start packet included slave address and R/W bit. The parameters of state
    machine shall be specified before executing this function(address, rxdPtr, txdPtr, rxSize
    , and txSize).
**************************************************************************************************/
unsigned char I2C_Master_Start_Packet(mI2cObj *obj, I2cTransmitMode trx)
{
    if(obj->member.reg->I2CSTR.bit.ARBL)
    {
        obj->member.reg->I2CSTR.bit.ARBL = 1; //Write a 1 to it to clear it.
        if(trx == I2C_TRANSMITTER_MODE)
        {
            obj->member.txStep = 0;
            obj->member.txSubStep = 0;
        }
        else
        {
            obj->member.rxStep = 0;
        }
    }
    else if(obj && trx == I2C_TRANSMITTER_MODE)
    {
        if(obj->member.txStep == 0)
        {
            if(!obj->member.reg->I2CSTR.bit.BB && obj->member.reg->I2CSTR.bit.XRDY)
            {
                // Slave addressing mask changes with addressing mode
                if(obj->member.reg->I2CMDR.bit.XA == I2C_7bit_ADDRESS)
                {
                    obj->member.reg->I2CSAR.bit.SAR = (obj->member.slaveAddress >> 1) & 0x007F;
                }
                else
                {
                    obj->member.reg->I2CSAR.bit.SAR = (obj->member.slaveAddress >> 1) & 0x03FF;
                }
                // Send slave address and read/write bit.
                obj->member.reg->I2CMDR.bit.RM = 1;
                // Master transmitter mode
                obj->member.reg->I2CMDR.bit.MST = 1;
                obj->member.reg->I2CMDR.bit.TRX = I2C_TRANSMITTER_MODE;
                // Send start condition
                obj->member.reg->I2CMDR.bit.STT = 1;
                obj->member.txStep++;
            }
            obj->member.occupied = 1;

        }
        return (obj->member.txStep > 0);
    }
    else
    {
        if(obj->member.rxStep == 0)
        {
            if(obj->member.reg->I2CSTR.bit.XRDY)
            {
                // Slave addressing mask changes with addressing mode
                if(obj->member.reg->I2CMDR.bit.XA == I2C_7bit_ADDRESS)
                {
                    obj->member.reg->I2CSAR.bit.SAR = (obj->member.slaveAddress >> 1) & 0x007F;
                }
                else
                {
                    obj->member.reg->I2CSAR.bit.SAR = (obj->member.slaveAddress >> 1) & 0x03FF;
                }
                obj->member.reg->I2CMDR.bit.RM = 1;
                // Master transmitter mode
                obj->member.reg->I2CMDR.bit.MST = 1;
                obj->member.reg->I2CMDR.bit.TRX = I2C_RECEIVER_MODE;                // Send slave address and read/write bit.
                // Send start condition
                obj->member.reg->I2CMDR.bit.STT = 1;

//                obj->member.reg->I2CCNT = obj->member.rxSize;
//                obj->member.reg->I2CMDR.bit.RM = 0;
                obj->member.rxStep++;
            }
            obj->member.occupied = 1;
        }
        return (obj->member.rxStep > 0);
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char I2C_Master_Stop_Packet(mI2cObj *obj)
Input:
    *obj            - Pointer of driver object.
Output:
    N/A
Comment:
    I2C master send stop condition.
**************************************************************************************************/
unsigned char I2C_Master_Stop_Packet(mI2cObj *obj)
{
    unsigned char dataTrash;
    if(obj->member.reg->I2CSTR.bit.ARBL)
    {
        obj->member.reg->I2CSTR.bit.ARBL = 1; //Write a 1 to it to clear it.
        obj->member.txStep = 0;
        obj->member.txSubStep = 0;
        obj->member.rxStep = 0;
    }
    else if(obj->member.reg->I2CSTR.bit.SCD)     //Stop condition detect
    {
        obj->member.reg->I2CSTR.bit.SCD = 1;// To clear this bit, write a 1 to it.
        // Reset parameters  of I2C state machine
        obj->member.txStep = 0;
        obj->member.txSubStep = 0;
        obj->member.rxStep = 0;
        obj->member.occupied = 0;
        obj->member.transmitting = 0;
        obj->member.receiving = 0;
        //        I2C_SW_Reset(obj);
        return 1;

    }
    else if (obj->member.reg->I2CSTR.bit.ARDY == 1 || obj->member.reg->I2CMDR.bit.TRX == I2C_RECEIVER_MODE)
    {

        // Send stop condition
        obj->member.reg->I2CMDR.bit.STP = 1;
        if(obj->member.reg->I2CSTR.bit.RRDY)
        {
            dataTrash = obj->member.reg->I2CDRR.bit.DATA;
            dataTrash = dataTrash ? 0 : dataTrash;
        }

    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char I2C_Master_TXD_Packet(mI2cObj *obj)
Input:
    *obj     - Object of I2C peripheral and parameters of state machine for I2C driver.
Output:
    return   - Result. 1 = completed, 0 = not complete.
Comment:
    I2C master mode transmit data function, the total number of bytes shall be less than
254 bytes.
**************************************************************************************************/
unsigned char I2C_Master_TXD_Packet(mI2cObj *obj)
{
    if(obj->member.txStep == 255)
        return 1;

    if(obj->member.txStep > 0)
    {
        if(obj->member.reg->I2CSTR.bit.XRDY)
        {
            obj->member.occupied = 0;

            if(obj->member.txStep < (obj->member.txSize + 1))
            {
                obj->member.reg->I2CDXR.bit.DATA = *(obj->member.txdPtr + obj->member.txStep - 1);
                obj->member.reg->I2CSTR.bit.ARDY = 1;   //Clear ARDY manually when write new data to I2CDXR
                if(obj->member.txStep < obj->member.txSize)
                {
                    obj->member.txStep++;
                }
                else
                {
                    obj->member.txStep = 255;
                    return 1;
                }
            }
            else
            {
                obj->member.txStep = 255;
                return 1;
            }
        }
        else
        {
            if(obj->member.reg->I2CSTR.bit.NACK)
            {
                //LED_TOGGLE();
                //obj->member.txStep = 0;
                //obj->member.txSubStep = 0;
                obj->member.reg->I2CSTR.bit.NACK = 1;
                //obj->member.reg->I2CMDR.bit.STP = 1;
                I2C_SW_Reset(obj);
            }
            obj->member.occupied = 1;
        }
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char I2C_Master_RXD_Packet(mI2cObj *obj)
Input:
    *obj     - Object of I2C peripheral and parameters of state machine for I2C driver.
Output:
    return   - Result. 1 = completed, 0 = not complete.
Comment:
    I2C master mode receive data function, the total number of bytes shall be less than
254 bytes.
**************************************************************************************************/
unsigned char I2C_Master_RXD_Packet(mI2cObj *obj)
{
    unsigned char dataTrash;

    if(obj->member.rxStep == 255)
    {
        return 1;
    }

    if(obj->member.rxStep > 0)
    {
        if(obj->member.reg->I2CSTR.bit.RRDY)
        {

            obj->member.occupied = 0;

            if(obj->member.rxStep < (obj->member.rxSize + 1))
            {
                *(obj->member.rxdPtr + obj->member.rxStep - 1) = obj->member.reg->I2CDRR.bit.DATA;
//                obj->member.rxStep++;
                if(obj->member.rxStep < obj->member.rxSize)
                {
                    if(++obj->member.rxStep >= obj->member.rxSize)
                    {
                        obj->member.reg->I2CMDR.bit.NACKMOD = 1;
                    }
                }
                else
                {
                    obj->member.rxStep = 255;
                    return 1;
                }
            }
            else
            {
                dataTrash = obj->member.reg->I2CDRR.bit.DATA;
                dataTrash = dataTrash ? 0 : dataTrash;
                obj->member.rxStep = 255;
                return 1;
            }
        }
        else
        {
            obj->member.occupied = 1;
        }
        if(obj->member.rxSize == 0)
        {
            obj->member.rxStep = 255;
            return 1;
        }

/*        if(obj->member.rxStep >= obj->member.rxSize + 1)
        {
            obj->member.reg->I2CMDR.bit.NACKMOD = 1;
            obj->member.rxStep = 255;
            return 1;
        }*/
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char I2C_Master_Transmit(mI2cObj *obj)
Input:
    *obj     - Object of I2C peripheral and parameters of state machine for I2C driver.
Output:
    return   - Result. 1 = completed, 0 = not complete.
Comment:
    I2C master mode sequence transmit function. The parameters of state machine shall be specified
before executing this function(address, rxdPtr, txdPtr, rxSize, and txSize).
**************************************************************************************************/
unsigned char I2C_Master_Transmit(mI2cObj *obj)
{
    if(I2C_Master_Start_Packet(obj,I2C_TRANSMITTER_MODE))
    {
        if(I2C_Master_TXD_Packet(obj))
        {
            return I2C_Master_Stop_Packet(obj);
        }
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char I2C_Master_Receive(mI2cObj *obj)
Input:
    *obj     - Object of I2C peripheral and parameters of state machine for I2C driver.
Output:
    return   - Result. 1 = completed, 0 = not complete.
Comment:
    I2C master mode sequence receive function. The parameters of state machine shall be specified
before executing this function(address, rxdPtr, txdPtr, rxSize, and txSize).
**************************************************************************************************/
unsigned char I2C_Master_Receive(mI2cObj *obj)
{
    if(I2C_Master_Start_Packet(obj, I2C_RECEIVER_MODE))
    {
        if(I2C_Master_RXD_Packet(obj))
        {

            return I2C_Master_Stop_Packet(obj);
        }
    }
    return 0;
}
//------------------------------------------------------------------------------
// Global functions
//------------------------------------------------------------------------------
/**************************************************************************************************
Function Name:
    void I2C_Master_Init(mI2cObj *obj, I2cAddressMode addressMode)
Input:
    *obj        - Pointer of driver object.
    addressMode - I2C addressing mode. 0 = 7-bit, 1 = 10-bit.
Output:
    None.
Comment:
    I2C initialization function. This function will set the mantissa and fraction of baud rate
register and control registers.
**************************************************************************************************/
void I2C_Master_Init(mI2cObj *obj, I2cAddressMode addressMode)
{
    static unsigned short dValue;
    static unsigned short clkTemp;

    // Must put I2C into reset before configuring it
    obj->member.reg->I2CMDR.bit.IRS = 0;
    // I2C module clock = 100MHz / (9 + 1) = 10MHz
    obj->member.reg->I2CPSC.bit.IPSC = obj->member.ipscValue;
    // dValue is an adjustment factor based on the IPSC
    switch(obj->member.ipscValue)
    {
        case 0U:
            dValue = 7U;
            break;

        case 1U:
            dValue = 6U;
            break;

        default:
            dValue = 5U;
            break;
    }
    clkTemp = obj->member.periodValue - (2 * dValue);
    obj->member.reg->I2CCLKH = clkTemp >> 1;
    obj->member.reg->I2CCLKL = clkTemp - obj->member.reg->I2CCLKH;

    obj->member.reg->I2CMDR.bit.XA = addressMode;
    obj->member.reg->I2CMDR.bit.BC = 0;         // 8 bits per data byte
//    obj->member.reg->I2CSAR.bit.SAR = 0xA0;     // Initial device ID
    // Enable FIFO
//    obj->member.reg->I2CFFTX.bit.I2CFFEN = 1;
//    obj->member.reg->I2CFFTX.bit.TXFFST = 1;
//    obj->member.reg->I2CFFRX.bit.RXFFST = 1;
    // Enable I2C module
    obj->member.reg->I2CMDR.bit.IRS = 1;        // I2C module is enabled.
}
/**************************************************************************************************
Function Name:
    unsigned char I2C_Master_Process_Transmit(void *obj, unsigned char id,
                                              void *aPtr, unsigned char aSize,
                                              void *dPtr, unsigned char dSize)
Input:
    *obj     - Object of I2C peripheral and parameters of state machine for I2C driver.
    id       - Slave address.
    *aPtr    - Pointer of address part, the function will sequentially send the specified number
               of bytes from the pointer.
    aSize    - Size of address part, the specified number of bytes for address part.
    *dPtr    - Pointer of data part, the function will sequentially send the specified number of
               bytes from the pointer after address part has been transferred.
    dSize    - Size of data part, the specified number of bytes for data part.
Output:
    return   - Result. 1 = completed, 0 = not complete.
Comment:
    I2C master mode process transmit function. The parameters of state machine shall be specified
before executing this function(address, rxdPtr, txdPtr, rxSize, and txSize).
**************************************************************************************************/
unsigned char I2C_Master_Process_Transmit(void *obj, unsigned char id,
                                          void *aPtr, unsigned char aSize,
                                          void *dPtr, unsigned char dSize)
{
    if(obj && ((mI2cObj*)obj)->member.receiving == 0)
    {
        ((mI2cObj*)obj)->member.transmitting = 1;
        ((mI2cObj*)obj)->member.slaveAddress = id;
        if(!((mI2cObj*)obj)->member.txSubStep)
        {
            ((mI2cObj*)obj)->member.txSize = aSize;
            ((mI2cObj*)obj)->member.txdPtr = aPtr;

            if(I2C_Master_Start_Packet(((mI2cObj*)obj), I2C_TRANSMITTER_MODE) && I2C_Master_TXD_Packet(((mI2cObj*)obj)))
            {
                ((mI2cObj*)obj)->member.txSubStep = 1;
                ((mI2cObj*)obj)->member.txStep = 1;
            }
        }
        else
        {
            ((mI2cObj*)obj)->member.txSize = dSize;
            ((mI2cObj*)obj)->member.txdPtr = dPtr;

            if(I2C_Master_TXD_Packet(((mI2cObj*)obj)))
            {
                return I2C_Master_Stop_Packet(((mI2cObj*)obj));
            }
        }

    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char I2C_Master_Process_Receive(I2cRegStr *reg,
                                             void *aPtr, unsigned char aSize,
                                             void *dPtr, unsigned char dSize)
Input:
    *reg     - Registers of I2C peripheral and parameters of state machine for I2C driver.
    *aPtr    - Pointer of address part, the function will sequentially send the specified number
               of bytes from the pointer.
    aSize    - Size of address part, the specified number of bytes for address part.
    *dPtr    - Pointer of data part, the function will sequentially receive the specified number
               of bytes to the pointer after address part has been transferred.
    dSize    - Size of data part, the specified number of bytes for data part.
Output:
    return   - Result. 1 = completed, 0 = not complete.
Comment:
    I2C master mode process receive function. The parameters of state machine shall be specified
before executing this function(address, rxdPtr, txdPtr, rxSize, and txSize).
**************************************************************************************************/
unsigned char I2C_Master_Process_Receive(void *obj, unsigned char id,
                                         void *aPtr, unsigned char aSize,
                                         void *dPtr, unsigned char dSize)
{
    if(obj && ((mI2cObj*)obj)->member.transmitting == 0)
    {
        ((mI2cObj*)obj)->member.receiving = 1;
        ((mI2cObj*)obj)->member.slaveAddress = id;
        ((mI2cObj*)obj)->member.txdPtr = aPtr;
        ((mI2cObj*)obj)->member.rxdPtr = dPtr;
        ((mI2cObj*)obj)->member.txSize = aSize;
        ((mI2cObj*)obj)->member.rxSize = dSize;

        if(!aSize)
            return I2C_Master_Receive((mI2cObj*)obj);
        else
        {
            if(((mI2cObj*)obj)->member.txSubStep == 0)
            {
                if (I2C_Master_Start_Packet((mI2cObj*)obj, I2C_TRANSMITTER_MODE))
                {
                    if(I2C_Master_TXD_Packet((mI2cObj*)obj))
                        ((mI2cObj*)obj)->member.txSubStep = 1;
                }
            }
            else
            {
                return I2C_Master_Receive((mI2cObj*)obj);
            }
        }
    }
    return 0;
}

/**************************************************************************************************
Function Name:
    unsigned char I2C_Master_Parameter_Reset(I2cMasterObj *obj)
Input:
    *obj     - Object pointer of master I2C driver.
Output:
    None.
Comment:
    I2C master mode state machine reset function.
**************************************************************************************************/
void I2C_Master_Parameter_Reset(mI2cObj *obj)
{
    obj->member.slaveAddress = 0;
    obj->member.txSize = 0;
    obj->member.rxSize = 0;
    obj->member.occupied = 0;
    obj->member.txStep = 0;
    obj->member.txSubStep = 0;
    obj->member.rxStep = 0;
    obj->member.txdPtr = 0;
    obj->member.rxdPtr = 0;
    obj->member.transmitting = 0;
    obj->member.receiving = 0;
}
/**************************************************************************************************
Function Name:
    unsigned char I2C_SW_Reset(I2cMasterObj *obj)
Input:
    *obj     - Object pointer of master I2C driver.
Output:
    None.
Comment:
    I2C software reset function, it is able to reset the I2C module.
**************************************************************************************************/
void I2C_SW_Reset(mI2cObj *obj)
{
    obj->member.reg->I2CMDR.bit.IRS = 0;

    while (obj->member.reg->I2CMDR.bit.IRS)
        asm(" NOP");                                 // Wait for SW Reset completed

    obj->member.reg->I2CMDR.bit.IRS = 1;

    I2C_Master_Parameter_Reset(obj);
}
/**************************************************************************************************
Function Name:
    unsigned char I2C_Master_Write_FIFO(void *obj, unsigned short *buffer, unsigned short dataCount)
Input:
    *obj            - Pointer of driver object.
    *buffer         - Pointer of the data array.
    dataCount       - Data count.
    slaveAddress    - Slave address.
Output:
    return          - 0 means No data is written.
                      i = Buffer Index, where buffer has been transfered.
Comment:
    I2C transmission function.
**************************************************************************************************/
unsigned char I2C_Master_Write_FIFO(void *obj, unsigned short *buffer, unsigned short dataCount)
{
    static short bufferIndex = 0;
    unsigned short i;

    if(obj && !((mI2cObj*)obj)->member.reg->I2CMDR.bit.STP && ((mI2cObj*)obj)->member.reg->I2CSTR.bit.ARDY)
    {
        // Whole packet data count, include slave address and byte index
        ((mI2cObj*)obj)->member.reg->I2CCNT = dataCount;
        // Put buffer into transmit data FIFO
        if(((mI2cObj*)obj)->member.reg->I2CFFTX.bit.TXFFINT)
        {
            if(bufferIndex <= (short)(dataCount - 16))
            {
                for(i = bufferIndex; i < bufferIndex + 16; i++)
                {
                    ((mI2cObj*)obj)->member.reg->I2CDXR.bit.DATA = buffer[i];
                }
                bufferIndex = i;
            }
            else
            {
                for(i = bufferIndex; i < dataCount; i++)
                {
                    ((mI2cObj*)obj)->member.reg->I2CDXR.bit.DATA = buffer[i];
                }
                bufferIndex = 0;
            }
        }
        // Transmitter mode
        ((mI2cObj*)obj)->member.reg->I2CMDR.bit.TRX = 1;

        return i;
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char I2C_Master_Read_FIFO(void *obj, unsigned short *buffer, unsigned short dataCount)

Input:
    *obj            - Pointer of driver object.
    *buffer         - Pointer of the data array.
    dataCount       - Data count.
    slaveAddress    - Slave address.
Output:
    return          - Result. 1 = completed. 0 = does not complete.
Comment:
    I2C reception function.
**************************************************************************************************/
unsigned char I2C_Master_Read_FIFO(void *obj, unsigned short *buffer, unsigned short dataCount)
{
    static short bufferIndex = 0;
    unsigned short i;

    if(obj && !((mI2cObj*)obj)->member.reg->I2CMDR.bit.STP)
    {
        // Whole packet data count, include slave address and byte index
        ((mI2cObj*)obj)->member.reg->I2CCNT = dataCount;
        // Put buffer into transmit data FIFO(16-deep)
        if(bufferIndex <= (short)(dataCount - 16))
        {
            for(i = bufferIndex; i < bufferIndex + 16; i++)
            {
                buffer[i] = ((mI2cObj*)obj)->member.reg->I2CDRR.bit.DATA;
            }
            bufferIndex = i;
        }
        else
        {
            for(i = bufferIndex; i < dataCount; i++)
            {
                buffer[i] = ((mI2cObj*)obj)->member.reg->I2CDRR.bit.DATA;
            }
            bufferIndex = 0;
        }

        return 1;
    }
    return 0;
}
//---------------- END LINE -----------------------------------------------------------------------
