/**************************************************************************************************
File Name: Boot_UART.h
Global Data:
    Name                      Type               Description
    ------------------------- ----------------- --------------------------------------------------
    dmaLinaRxObj              DmaLinObj         Object of the LINBUS Tx DMA driver.
    dmaLinaTxObj              DmaLinObj         Object of the LINBUS Rx DMA driver.
    linaObj                   LinObj            Object of the LINBUS driver.
    bootProtocol              IntProtocolObj    Object of the internal protocol for the bootloader.
Description:
    Header file of Boot_UART.c.

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    10/08/2020 Watch Lee        1. version.1.0.

**************************************************************************************************/

#ifndef __BOOT_UART_H__
#define __BOOT_UART_H__

/* Includes */
#include "f28003x_dma_lin_driver.h"
#include "f28003x_lin_driver.h"
#include "f28003x_sci_driver.h"
#include "Middleware_Catalog.h"

/* Global function prototypes */
void Boot_UART_Routine(void);
void Boot_UART_Data_Process(void);

/* Global data declarations */
extern DmaLinObj dmaLinaRxObj, dmaLinaTxObj;
extern LinObj linaObj;
extern SciObj sciaObj;
extern IntProtocolObj intpPri, intpSec;
extern BootInternalObj bootPri, bootSec;
extern unsigned short uartCalibrationMode;
extern unsigned short pfcCalibrationCtrl;
#endif
