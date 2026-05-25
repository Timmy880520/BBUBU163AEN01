/**************************************************************************************************
File Name: COM_UART.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------

External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    Write_Boot_Activate_Code                f28004x_boot.c
    Boot_Reset_MCU                          f28004x_boot.c
Description:
    Internal UART communication program.

===================================================================================================
History:
    Date        Author          Description of Change
    ----------  --------------- ------------------------------------------------------------------
    04/15/2020  Watch Lee       1. version 1.0.
    03/09/2021  Fred Huang      1. Add pfcMonitor buffer
                                2. Add pfcLog buffer
                                2. Add 3-phase input voltage calculation
    03/11/2021  Fred Huang      Add PFC input data calibration calculation
    03/12/2021  Fred Huang      Get the primary firmware version at begin
    05/06/2021  Watch Lee       1. Add Boot_UART_Data_Process function.

**************************************************************************************************/

/* Includes */
#include "APP.h"
#include "COM_UART.h"

/* Local function prototypes */
unsigned char ISP_State_Machine(unsigned char shutdown);

/* Data definitions */
//#pragma SET_DATA_SECTION(".ramgs0")
//Create_LIN_RX_DMA_Object(dmaLinaRxObj, 64, DmaRegs.CH2);
//Create_LIN_TX_DMA_Object(dmaLinaTxObj, 64, DmaRegs.CH4);
//#pragma SET_DATA_SECTION()
//
//Create_LIN_Object(linaObj, LinaRegs, CLK_120MHz, 38400);
//Create_INT_Protocol_Object(intProtocol, 16);

//#pragma SET_DATA_SECTION(".ramgs0")
Create_SCI_Object(sciaObj, SciaRegs, LSPCLK_30MHz, 38400);
Create_INT_Protocol_Object(sciProtocol, 16);
Create_Boot_Internal_Object(bootSec, Bin_Char4(Function_ID), 32);
//#pragma SET_DATA_SECTION()

Create_Timer(waitGetState, 2000);   // 2000ms
Create_Timer(delayResetMCU, 100);   // 100ms

BootDataType bootSecData = {0, 0, 0, 0, 0, 0};
AccessKeyType clearFault = {0};

DcdcStateType dcdcState = {0};
DcdcAlarmType dcdcAlarm = {0};
ComDataType comData = {0};
BmsDataType bmsData = {0};
BbuStateType bbuState = {0};
BbuAlarmType bbuAlarm = {0};
BbuFaultType bbuFault = {0};
LearningStopReasonType learningCycle = {0};
unsigned long pmiTimes = 0;
BmsFetControlType bmsFetControl = {0};
unsigned long overWrietTimer = 0;
unsigned char protectRelease = 0;
unsigned char learningMode = 0;
unsigned char checkCommandAck = BOOT_NONE_COMMAND;
unsigned char startFwUpgrade = 0;

//calibration
unsigned short uartCalibrationMode;
unsigned char  uartSaveCalibration;
unsigned short uartEngineerMode;

Create_Boot_Internal_Object(bootUart, Bin_Char4(Function_ID), 16);

/***********************************************************************************************************
Function Name:
    void UART_Routine(void)
Input:
    None.
Output:
    None.
Comment:
    UART routine function. This function includes the transmission and reception of internal protocol.
***********************************************************************************************************/
void UART_Routine(void)
{
    sciProtocol.func.transmitData(&sciProtocol, &sciaObj, sciaObj.func.transmit);
    sciProtocol.func.receiveData(&sciProtocol, &sciaObj, sciaObj.func.receive);
    sciaObj.func.clearState(&sciaObj);
}
/***********************************************************************************************************
Function Name:
    void Process_UART_Data(void)
Input:
    None.
Output:
    None.
Comment:
    Process UART data function.
***********************************************************************************************************/
void Process_UART_Data(void)
{
    //TER and SEC internal communication ------------------------

    if (sciProtocol.func.getReloadFlag(&sciProtocol, 0xD0))
    {
        sciProtocol.func.clearReloadFlag(&sciProtocol, 0xD0);
        if (uartCalibrationMode == 0x55AA) // enter calibration mode, reset all calibration data
        {
            sciProtocol.func.packTxData(&sciProtocol, 0xD0, INTERNAL_ACK, LOW_PRIORITY);

            calibrationData.arg.dchgCal1[0].arg.offset = 0;
            calibrationData.arg.dchgCal1[0].arg.gain = 1000;
            calibrationData.arg.dchgCal2[0].arg.offset = 0;
            calibrationData.arg.dchgCal2[0].arg.gain = 1000;
            calibrationData.arg.dchgCal3[0].arg.offset = 0;
            calibrationData.arg.dchgCal3[0].arg.gain = 1000;
            calibrationData.arg.dchgVcmd.arg.offset = 0x2112; //New add 0x2112 0d8466 //0x1501 0d5377
            calibrationData.arg.dchgVcmd.arg.gain = 0;
            calibrationData.arg.dchgCal4[0].arg.offset = 0;
            calibrationData.arg.dchgCal4[0].arg.gain = 1000;
            calibrationData.arg.dchgCal5[0].arg.offset = 0;
            calibrationData.arg.dchgCal5[0].arg.gain = 1000;


            calibrationData.arg.chgCal1[0].arg.offset = 0;
            calibrationData.arg.chgCal1[0].arg.gain = 1000;
            calibrationData.arg.chgCal2[0].arg.offset = 0;
            calibrationData.arg.chgCal2[0].arg.gain = 1000;
            calibrationData.arg.chgIcmd.arg.offset = 0x3C;    //0d60
            calibrationData.arg.chgIcmd.arg.gain = 0;
            calibrationData.arg.chgVcmd.arg.offset = 0x1FC2;  //0d8130
            calibrationData.arg.chgVcmd.arg.gain = 0;

            uartCalibrationMode = 0;
        }
    }

    if (sciProtocol.func.getReloadFlag(&sciProtocol, 0xD4))
    {
        sciProtocol.func.clearReloadFlag(&sciProtocol, 0xD4);
        if (uartEngineerMode == 0xBBAA)
        {
            sciProtocol.func.packTxData(&sciProtocol, 0xD4, INTERNAL_ACK, LOW_PRIORITY);
            warningCode.status2.bit.Engineer_OK = 1;
            cpuLlcState.bit.sotcEngineerFlag = 1;
            uartEngineerMode = 0;
        }
        else
        {
            sciProtocol.func.packTxData(&sciProtocol, 0xD4, INTERNAL_ACK, LOW_PRIORITY);
            warningCode.status2.bit.Engineer_OK = 0;
            cpuLlcState.bit.sotcEngineerFlag = 0;
            uartEngineerMode = 0;
        }
    }

    if (sciProtocol.func.getReloadFlag(&sciProtocol, 0xC7))
    {
        sciProtocol.func.clearReloadFlag(&sciProtocol, 0xC7);
        sciProtocol.func.packTxData(&sciProtocol, 0xC7, INTERNAL_ACK, LOW_PRIORITY);

        dischargerOperationTimer.reg.bits.cnt = dischargerOperationTimer.cmp - overWrietTimer;
    }

//    unpack data
    sciProtocol.func.unpackRxData(&sciProtocol);

//    Transfer item data
    if (warningCode.status2.bit.Engineer_OK)
    {
        bbuItem.flag.cFet = 1;
        bbuItem.flag.dFet = 1;
    }
    else
    {
        bbuItem.flag.cFet = bbuState.bit.chargeFET;
        bbuItem.flag.dFet = bbuState.bit.dischargeFET;
    }

//    Transfer com data
    warningCode.flag.bit.AMB_OTP = comData.comState.bit.ambOTP;
    warningCode.flag.bit.AMB_OTW = comData.comState.bit.ambOTW;
    warningCode.flag.bit.fan1FrontFault = comData.comState.bit.faultFan1Front;
    warningCode.flag.bit.fan1RearFault = comData.comState.bit.faultFan1Rear;
    warningCode.flag.bit.fan2FrontFault = comData.comState.bit.faultFan2Front;
    warningCode.flag.bit.fan2RearFault = comData.comState.bit.faultFan2Rear;
    warningCode.flag.bit.BMSComFault = comData.comState.bit.bmsComFault;
    warningCode.flag.bit.DDComFault = comData.comState.bit.dcdcComFault;

    warningCode.flag.bit.srOTP = comData.comState.bit.srTempFault;
    warningCode.flag.bit.srOTW = comData.comState.bit.srTempWarning;
    warningCode.flag.bit.oringOTP = comData.comState.bit.oringTempFault;
    warningCode.flag.bit.oringOTW = comData.comState.bit.oringTempWarning;
    warningCode.flag.bit.chgOTP = comData.comState.bit.chgDDTempFault;
    warningCode.flag.bit.chgOTW = comData.comState.bit.chgDDTempWarning;
    warningCode.flag.bit.dchgOTP = comData.comState.bit.dchgDDTempFault;
    warningCode.flag.bit.dchgOTW = comData.comState.bit.dchgDDTempWarning;

    Reload_Ext_Calibration_Data();

    if (Boot_Check_Backup_Data(Boot_Activate, Boot_Activate_Key, sizeof(Boot_Activate_Key)))
    {
        if (Call_Timer(delayResetMCU))
        {
            Set_Llc_Mode(LLC_OFF_MODE);
            Llc_Sr_Pwm_Off();
            LLC_Driver_Disable();
            SR_Driver_Disable();
            Oring_Off();
            IBUS_Disable();

            Set_Ahb_Mode(AHB_OFF_MODE);
            Chg_Pwm_Off();
            CHG_Driver_Disable();
//            dmaLinaTxObj.member.reg->CONTROL.bit.SOFTRESET = 1; //Disable Uart DMA
            Boot_Reset_MCU();
        }
    }
    else
        Timer_Reset(delayResetMCU);

/*
    static unsigned char shutdownFlag = 0;

    if (ISP_State_Machine(shutdownFlag))
    {
        // disable all converter functions and ISR
        Set_Llc_Mode(LLC_OFF_MODE);
        LLC_Driver_Disable();
        SR_Driver_Disable();
        Oring_Off();
        IBUS_Disable();
        shutdownFlag = 1;
        //dmaLinaTxObj.member.reg->CONTROL.bit.SOFTRESET = 1; //Disable Uart DMA
    }
*/
}
/**************************************************************************************************
Function Name:
    void Warning_Storage(void)
Input:
    None.
Output:
    None.
Comment:
    Storage the warning flag and update the DCDC state.
**************************************************************************************************/
void Warning_Storage(void)
{
    dcdcState.bit.bbukill        |= warningCode.status2.bit.bbuKill;
    dcdcState.bit.Estop1         |= warningCode.status2.bit.ESTOP1;
    dcdcState.bit.Estop2         |= warningCode.status2.bit.ESTOP2;

    dcdcAlarm.bit.IoutOcpChgSw   |= warningCode.flag.bit.iChargeOcFault;
    dcdcAlarm.bit.OvpBusSw       |= warningCode.status.bit.OVP_BUS_SW;
    dcdcAlarm.bit.OvpBusChgIo    |= warningCode.status.bit.OVP_BUS_CHG_IO;
    dcdcAlarm.bit.UvpBusSw       |= warningCode.status.bit.UVP_BUS_SW;
    dcdcAlarm.bit.OvpChgBattSw   |= warningCode.flag.bit.vChargeOvFault;
    dcdcAlarm.bit.OvpBattIo      |= warningCode.status.bit.OVP_BATT_IO;
    dcdcAlarm.bit.OvpChgBattIo   |= warningCode.status.bit.OVP_CHG_BATT_IO;

    dcdcAlarm.bit.OcpDischgSw    |= warningCode.status.bit.OCP_DISCHG_SW;
    dcdcAlarm.bit.OcpDischgIo    |= warningCode.status.bit.OCP_DISCHG_IO;
    dcdcAlarm.bit.PriOcpDischgIo |= warningCode.status.bit.PRI_OCP_DISCHG_IO;
    dcdcAlarm.bit.OvpBattIo      |= warningCode.status.bit.OVP_BATT_IO;
    dcdcAlarm.bit.VinUvpDischgSw  = warningCode.flag.bit.vBattUvFault;
    dcdcAlarm.bit.OvpBusSw       |= warningCode.status.bit.OVP_BUS_SW;
    dcdcAlarm.bit.OvpBusIo       |= warningCode.status.bit.OVP_BUS_IO;

    if (warningCode.status2.bit.FAILOUT)
    {
        dcdcState.bit.faultShutdown = 1;
    }
}
/**************************************************************************************************
Function Name:
    void Live_Update_State_Machine(void)
Input:
    None.
Output:
    None.
Comment:
    Live update state machine function. This function will check boot commands and execute the
bootloader process.
**************************************************************************************************/
void Live_Update_State_Machine(void)
{
    // SCI routine
    sciProtocol.func.unpackRxData(&sciProtocol);

    if (sciProtocol.func.getReloadFlag(&sciProtocol, BOOT_DOWNLOAD))
    {
        bootSec.member.message.reloaded = 1;
        sciProtocol.func.clearReloadFlag(&sciProtocol, BOOT_DOWNLOAD);
    }

    // State machine of the live update process
    switch (bootSec.func.stateMachine(&bootSec, 0))
    {
        case BOOT_INIT:
            bootSec.func.init(&bootSec, 1, 0, 0);
            if (sohOutFlag == 1)
            {
                startFwUpgrade = 1;
                learningMode = 0;
            }
            // bootPri.func.transportInit(&bootPri);
            break;
        case BOOT_DOWNLOAD:
            sciProtocol.func.packTxData(&sciProtocol, BOOT_DOWNLOAD, INTERNAL_ACK, 0);
            if (sohOutFlag == 1)
            {
                startFwUpgrade = 1;
                learningMode = 0;
            }
            break;
        case BOOT_COMPLETED:
            sciProtocol.func.packTxData(&sciProtocol, BOOT_COMPLETED, INTERNAL_ACK, 0);
            sciProtocol.func.clearReloadFlag(&sciProtocol, BOOT_COMPLETED);
            if (sohOutFlag == 1)
            {
                startFwUpgrade = 1;
                learningMode = 0;
            }
            break;
        case BOOT_FINISHED:
            if (Check_Bank1_App_Image())
            {
                Write_Boot_Activate_Code(Boot_Activate_Key, sizeof(Boot_Activate_Key));
                bootSec.member.message.protectKey = BOOT_LOCK_KEY;  // Self Lock
            }
            startFwUpgrade = 0;
            break;
        case BOOT_RECOVERY:
            // Unsupported feature
            bootSec.func.init(&bootSec, 1, 0, 0);
            break;
    }
}

/**************************************************************************************************
Function Name:
    unsigned char Check_Bank1_App_Image(void)
Input:
    None.
Output:
    return      - result. 1 = app image is correct. 0 = app image is incorrect.
Comment:
    Check bank1 app image function. This function will check the app image of flash bank1.
**************************************************************************************************/
unsigned char Check_Bank1_App_Image(void)
{
    ServiceDog();

    EnumBootState state = bootSec.func.init(&bootSec, 0, Bank1_App_ROM_Start, Bank1_App_ROM_Size);

    ServiceDog();
    return (state == BOOT_LOCKED);
}

/**************************************************************************************************
Function Name:
    unsigned char ISP_State_Machine(unsigned char shutdown)
Input:
    shutdown - Shutdown flag. 1 = converter has disabled and ready to reset,
                              0 = converter is working.
Output:
    return   - Boot Init flag. 1 = received BootInit command, ready to reset MCU,
                               0 = ISP process is idling.
Comment:
    ISP state machine function. This function will check boot commands and reset MCU if the ISP
command procedure is correct.

Reset MCU process:
1. check the return value of ISP_State_Machine, ISP_State_Machin, applications shall execute the
shutdown process if the return value is 1.
2. set the shutdown argument if the shutdown process is completed.
3. reset MCU to enter the boot mode.

**************************************************************************************************/
unsigned char ISP_State_Machine(unsigned char shutdown)
{
    static unsigned char setBootInit = 0;

    switch (bootSecData.state.byte.bootState)
    {
        case BOOT_LOCKED:
            if (bootSecData.protectKey == BOOT_UNLOCK_KEY)
            {
                bootSecData.state.stateCode = (BOOT_UNLOCKED << 8) + PREPARING;
                bootSecData.protectKey = 0;
            }
            bootSecData.initKey = 0;
            break;
        case BOOT_UNLOCKED:
            if (bootSecData.protectKey == BOOT_LOCK_KEY)
            {
                bootSecData.state.stateCode = BOOT_LOCKED << 8;
                bootSecData.protectKey = 0;
            }
            else if (bootSecData.state.byte.subState == PREPARING)
            {
                if (bootSecData.initKey == BOOT_INIT_KEY)
                {
                    bootSecData.state.byte.subState = READY;
                    bootSecData.initKey = 0;
                    setBootInit = 1;
                }
            }
            break;
    }

    if (setBootInit && shutdown /*&& (dmaLinaTxObj.member.reg->CONTROL.bit.RUNSTS == 0)*/)//Wait UART dma transmit done
    {
        Write_Boot_Activate_Code(Boot_Activate_Key, sizeof(Boot_Activate_Key));
        Boot_Reset_MCU();
    }
    return setBootInit;
}
//---------------- END LINE -----------------------------------------------------------------------
