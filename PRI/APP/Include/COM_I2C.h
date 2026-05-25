/**************************************************************************************************
File Name: COM_I2C.h
Global Data:
    Name                        Type                Description
    --------------------------- ------------------- --------------------------------------------------
    i2caObj                     I2cObj              Object of CAN BUS driver.
Description:
    Header file of COM_I2C.c.

===================================================================================================
History:
    Date        Author          Description of Change
    ----------- --------------- -----------------------------------------------------------------
    09/16/2020  Fred Huang      1. version 1.0.

**************************************************************************************************/

#ifndef _COM_I2C_H_
#define _COM_I2C_H_

/* Includes */

#include "f28004x_i2c_driver.h"
#include "COM_UART.h"
#include "App.h"

#ifndef CLK_10MHz
#define CLK_10MHz                      10000000UL
#endif

/* Macro definitions */

/* Type definitions */

/* Global function prototypes */
void I2C_Routine(void);

/* Global data declarations */
extern mI2cObj masterI2cObj;
extern unsigned short eventTrig;
extern TmrPkg recordTimer;
//extern MeterDataType llcMonitor;

#endif /* _COM_I2C_H_ */
