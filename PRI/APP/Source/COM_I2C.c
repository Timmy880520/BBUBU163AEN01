/**************************************************************************************************
File Name: COM_I2C.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------

External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------

Description:
    EEPROM I2C communication program.

===================================================================================================
History:
    Date        Author          Description of Change
    ----------- --------------- -----------------------------------------------------------------
    09/16/2020  Fred Huang      1. version 1.0.

**************************************************************************************************/

/* Includes */
#include "COM_I2C.h"

/* Local function prototypes */

/* Data definitions */
Create_I2C_Object(masterI2cObj, I2caRegs, CLK_10MHz, 100e3);
unsigned char command[2], testData[16], slaveAddr = 0xA0;
unsigned char trigT = 0, trigR = 0, resetTrig = 0, commandLen = 1, traLen = 5, recLen = 0;
Create_Timer(recordTimer, 1000);        //1s
/***********************************************************************************************************
Function Name:
    void CAN_Routine(void)
Input:
    None.
Output:
    None.
Comment:
    UART routine function. This function includes the transmission and reception of internal protocol.
***********************************************************************************************************/
void I2C_Routine(void)
{
    if(trigT)
    {
        if(masterI2cObj.func.transmit(&masterI2cObj, slaveAddr, &command, commandLen, testData, traLen))
            trigT = 0;
    }
    if(trigR)
    {
        if(masterI2cObj.func.receive(&masterI2cObj, slaveAddr, &command, commandLen, testData, recLen))
            trigR = 0;
    }
    if(resetTrig || masterI2cObj.member.reg->I2CSTR.bit.ARBL)
    {
        masterI2cObj.func.resetModule(&masterI2cObj);
        resetTrig = 0;
    }

}
//---------------- END LINE -----------------------------------------------------------------------
