/**************************************************************************************************
 File Name: Boot_UART.c
 External Data:
 Name                                    Source
 --------------------------------------- -----------------------------------------------------
 Call_Timer                              Univ_Lib.c
 External Functions:
 Name                                    Source
 --------------------------------------- -----------------------------------------------------
 None.
 Description:
 Boot UART function.

 ===================================================================================================
 History:
 Date       Author           Description of Change
 ---------- ---------------- -----------------------------------------------------------------
 10/08/2020 Watch Lee        1. version 1.0.
 05/06/2021 Watch Lee        1. version 1.1.
 2. modify Boot_UART_Data_Process to support LFU.

 **************************************************************************************************/

/* Includes */
#include "Global_Var.h"
#include "Boot_UART.h"

/* Local function prototypes */

/* Data definitions */
#pragma SET_DATA_SECTION("ramgs0")
Create_LIN_RX_DMA_Object(dmaLinaRxObj, SLIP_Packet_Size, DmaRegs.CH2);
Create_LIN_TX_DMA_Object(dmaLinaTxObj, SLIP_Packet_Size, DmaRegs.CH4);
#pragma SET_DATA_SECTION()

Create_Timer(waitGetState, Count_500ms);

Create_Boot_Internal_Object(bootPri, Bin_Char4(Function_ID), 32);
Create_Boot_Internal_Object(bootSec, Bin_Char4(Function_ID), 32);

Create_LIN_Object(linaObj, LinaRegs, CLK_120MHz, 38400);
Create_INT_Protocol_Object(intpPri, 16);
Create_SCI_Object(sciaObj, SciaRegs, LSPCLK_30MHz, 38400);
Create_INT_Protocol_Object(intpSec, 16);

// Delay reset MCU to restart app
Create_Timer(delayResetMCU, Count_100ms);

unsigned char checkCommandAck = BOOT_NONE_COMMAND;
unsigned short uartCalibrationMode;
unsigned short pfcCalibrationCtrl = 0;

/**************************************************************************************************
 Function Name:
 void Boot_UART_Routine(void)
 Input:
 None.
 Output:
 None.
 Comment:
 Boot-loader UART routine function. This function includes the transmission and reception of
 boot protocol.
 **************************************************************************************************/
void Boot_UART_Routine(void)
{
    intpPri.func.transmitData(&intpPri, &dmaLinaTxObj, linaObj.func.transmit);
    intpPri.func.receiveData(&intpPri, &dmaLinaRxObj, linaObj.func.receive);
    linaObj.func.clearState(&linaObj);

    intpSec.func.transmitData(&intpSec, &sciaObj, sciaObj.func.transmit);
    intpSec.func.receiveData(&intpSec, &sciaObj, sciaObj.func.receive);
    sciaObj.func.clearState(&sciaObj);
}
/**************************************************************************************************
 Function Name:
 void Boot_UART_Data_Process(void)
 Input:
 None.
 Output:
 None.
 Comment:
 Boot UART data process function. This function will unpack reception data and set reload flag of
 the boot internal object when received download data.
 **************************************************************************************************/
void Boot_UART_Data_Process(void)
{
    static short flagWaitAck = 0;
    // Secondary ISP process (program mode)
    intpSec.func.unpackRxData(&intpSec);

    if (intpSec.func.getReloadFlag(&intpSec, BOOT_DOWNLOAD))
    {
        bootSec.member.message.reloaded = 1;
        intpSec.func.clearReloadFlag(&intpSec, BOOT_DOWNLOAD);
    }

    if (intpSec.func.getReloadFlag(&intpSec, 0xD0))
    {
        intpSec.func.clearReloadFlag(&intpSec, 0xD0);

        if (uartCalibrationMode == 0x6699) //turn off primary side and sec
        {
            pfcCalibrationCtrl = 0x6699;
            intpPri.func.packTxData(&intpPri, 0x06, INTERNAL_SET, LOW_PRIORITY); //SET, CAL KEY
            flagWaitAck = 1;
        }
    }

    // RPI and SEC internal communication ------------------------
    if (flagWaitAck)
    {
        if (intpPri.func.getNoAckFlag(&intpPri, 0x06) == 0)
        {
            //intProtocol.func.clearReloadFlag(&intProtocol, 0x06);
            //intProtocol.func.packTxData(&intProtocol, 0x06, INTERNAL_SET, LOW_PRIORITY);

            //sciProtocol.func.clearReloadFlag(&sciProtocol, 0xD0);
            intpSec.func.packTxData(&intpSec, 0xD0, INTERNAL_ACK, LOW_PRIORITY);
            flagWaitAck = 0;
            GpioDataRegs.GPBSET.bit.GPIO56 = 1;    // Default HIGH, Standby power OFF
        }
    }

    switch (bootSec.func.stateMachine(&bootSec, &bootPri))
    {
    case BOOT_INIT:
//        Flash_Sector_Erase(App_ROM_Start, App_ROM_Size);
        bootSec.func.init(&bootSec, 1, 0, 0);
        bootPri.func.transportInit(&bootPri);
//        bootPri.member.message.reloaded = 0x03; //For execute bootUart protect and erase
        break;
    case BOOT_DOWNLOAD:
        intpSec.func.packTxData(&intpSec, BOOT_DOWNLOAD, INTERNAL_ACK, LOW_PRIORITY);
        break;
    case BOOT_COMPLETED:
        intpSec.func.packTxData(&intpSec, BOOT_COMPLETED, INTERNAL_ACK, LOW_PRIORITY);
        intpSec.func.clearReloadFlag(&intpSec, BOOT_COMPLETED);
        break;
    case BOOT_FINISHED:
        if (Call_Timer(delayResetMCU))
        {
            Write_Boot_Activate_Code(Boot_Activate_Key, sizeof(Boot_Activate_Key));
            Boot_Reset_MCU();
        }
        break;
    case BOOT_RECOVERY:
        // Unsupported feature
        bootSec.func.init(&bootSec, 1, 0, 0);
        break;
    }

    // Primary ISP process (transport mode)
    intpPri.func.unpackRxData(&intpPri);

    unsigned char ack = 0;

    if (checkCommandAck != BOOT_NONE_COMMAND)
    {
        if (checkCommandAck == BOOT_STATE)
        {
            if (Call_Timer(waitGetState))
            {
                Timer_Reset(waitGetState);
                ack = 1;
            }
        }
        else if(checkCommandAck == BOOT_PROTECTION)
        {
            ack = !intpPri.func.getNoAckFlag(&intpPri, checkCommandAck);
            if (Call_Timer(waitGetState))
            {
                Timer_Reset(waitGetState);
                intpPri.func.packTxData(&intpPri, BOOT_PROTECTION, INTERNAL_SET, LOW_PRIORITY);
            }
        }
        else
        {
            Timer_Reset(waitGetState);
            ack = !intpPri.func.getNoAckFlag(&intpPri, checkCommandAck);
        }
    }

    switch (bootPri.func.transportState(&bootPri, ack))
    {
    case BOOT_PROTECTION:
        intpPri.func.packTxData(&intpPri, BOOT_PROTECTION, INTERNAL_SET,
                                LOW_PRIORITY);
        checkCommandAck = BOOT_PROTECTION;
        break;
    case BOOT_INIT:
        intpPri.func.packTxData(&intpPri, BOOT_INIT, INTERNAL_SET,
                                LOW_PRIORITY);
        checkCommandAck = BOOT_INIT;
        break;
    case BOOT_DOWNLOAD:
        intpPri.func.packTxData(&intpPri, BOOT_DOWNLOAD, INTERNAL_SET,
                                LOW_PRIORITY);
        checkCommandAck = BOOT_DOWNLOAD;
        break;
    case BOOT_COMPLETED:
        intpPri.func.packTxData(&intpPri, BOOT_COMPLETED, INTERNAL_SET,
                                LOW_PRIORITY);
        checkCommandAck = BOOT_COMPLETED;
        break;
    case BOOT_STATE:
        intpPri.func.packTxData(&intpPri, BOOT_STATE, INTERNAL_GET,
                                LOW_PRIORITY);
        checkCommandAck = BOOT_STATE;
        break;
    }
}
//---------------- END LINE -----------------------------------------------------------------------
