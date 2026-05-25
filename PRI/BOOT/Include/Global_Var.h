/**************************************************************************************************
File Name: Global_Var.h
Global Data:
    Name                     Type               Description
    ------------------------ ------------------ --------------------------------------------------
    debugCheck               unsigned char      Debug code verification flag.
    deviceAddress            unsigned char      CANBus ID address.
Description: 
    Global variable header file for BOOT code.

===================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    10/07/2020 Watch Lee        1. version 1.0.
    05/06/2021 Watch Lee        1. version 1.1.
                                2. update global variables.

**************************************************************************************************/

#ifndef __GLOBAL_VAR_H__
#define __GLOBAL_VAR_H__

/* Includes */
#include "F28x_Project.h"
#include "f28003x_flash_access.h"
#include "f28003x_boot.h"
#include "Middleware_Catalog.h"
#include "Boot_UART.h"

/* Macro definitions */
// Function ID code
#ifndef Function_ID
#define Function_ID             "PRI"
#endif

// Bootloader Version
#ifndef Boot_Version
#define Boot_Version            "S2.02B01"
#endif
#ifndef Sub_Version
#define Sub_Version             "00"
#endif

// Version summary
#ifndef Version_List
#define Version_List            {Boot_Version, Sub_Version}
#endif

// Pin definition
#define _getGPIO_DEBUG          GpioDataRegs.GPADAT.bit.GPIO23
#define _setGPIO_DEBUG          GpioDataRegs.GPASET.bit.GPIO23 = 1
#define _clearGPIO_DEBUG        GpioDataRegs.GPACLEAR.bit.GPIO23 = 1
#define _toggleGPIO_DEBUG       GpioDataRegs.GPATOGGLE.bit.GPIO23 = 1

#define _getGPIO_A0             GpioDataRegs.GPADAT.bit.GPIO4
#define _getGPIO_A1             GpioDataRegs.GPADAT.bit.GPIO5
#define _getGPIO_A2             GpioDataRegs.GPBDAT.bit.GPIO39

// Global parameter
#define TRUE                    1
#define FALSE                   0

// Time base
#define Count_1ms               1
#define Count_10ms              10
#define Count_100ms             100
#define Count_500ms             500
#define Count_1s                1000
#define Count_2s                2000
#define Count_3s                3000
#define Count_5s                5000

/* Type definitions */
// Enumeration

// Structure & union
typedef union
{
    unsigned short all;

    struct
    {
        unsigned short a0           :1;
        unsigned short a1           :1;
        unsigned short a2           :1;
        unsigned short resverd      :13;
    }bit;
}addressDataType;

/* Global function prototypes */
void Init_Peripheral(void);
void Deinit_Periphral(void);
__interrupt void CPU_Timer1_ISR(void);

/* Global data declarations */
extern unsigned char debugCheck;
extern unsigned char deviceAddress;

#endif
