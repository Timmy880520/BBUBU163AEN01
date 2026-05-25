/***********************************************************************************************************************
File Name: Initial.c
External Data:
    Name                                    Source
    --------------------------------------- ----------------------------------------------------------------------------
    dmaLinaRxObj                            COM_UART.c
    dmaLinaTxObj                            COM_UART.c
    linaObj                                 COM_UART.c
    dmaCanbRxObj                            COM_CAN.c
    canbObj                                 COM_CAN.c
    i2caObj                                 COM_I2C.c
    intCommand                              INT_PROTOCOL_TABLE.c
    extCalibration                          EXT_Calibration.c
    extCommonCommand                        EXT_PROTOCOL_TABLE.c
    pfcCalibrationCtrl                      COM_UART.c
    waringCodeClearKey                      COM_CAN.c
    moduleUnlatchKey                        COM_CAN.c
    mfrAccessKey                            COM_CAN.c
    canTurnOffFlag                          main.c
    outputVoltSetpoint                      COM_CAN.c
    outputVoltSetpointPre                   COM_CAN.c
    outputVoltSetpointBuffer                COM_CAN.c
    outputVoltSetpointOffset                COM_CAN.c
    llcCalibration                          COM_CAN.c
External Functions:
    Name                                    Source
    --------------------------------------- ----------------------------------------------------------------------------
    Hw_Setup                                HardwareSetup.c
    Init_Flash_API                          f28004x_flash_access.c
    Load_Ext_Calibration_Data               EXT_Calibration.c
    Reload_Ext_Calibration_Data             EXT_Calibration.c
    CAN_DMA_Buffer_Reset                    COM_CAN.c
    Set_Voltage_Setpoint                    LlcDriver.c
    Set_Current_Setpoint                    LlcDriver.c
    Set_Llc_Mode                            LlcDriver.c
Description:
    Initial setup
========================================================================================================================
History:
    Date        Author          Description of Change
    ----------- --------------- ----------------------------------------------------------------------------------------
    10/24/2019  Fred Huang      Create file
    03/26/2020  Fred Huang      Add WD function
    03/05/2021  Watch Lee       1. Add Load_Ext_Calibration_Data to Initial for loading calibration data.
                                2. Add a initial of extProcotol for extCalibration.
    03/08/2021  Fred Huang      Add output voltage calibration offset calculation and outputVoltSetpoint initialize
    03/10/2021  Fred Huang      1. Add communication key reset
                                2. Modify watch dog time to 411ms
    03/11/2021  Fred Huang      1. Enable watch dog after all initial setup
                                2. Assign calibration data to buffer after load from flash
    03/18/2021  Fred Huang      Add canTurnOffFlag initialize
    05/06/2021  Watch Lee       1. modify Enable_WatchDog function.

***********************************************************************************************************************/

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "App.h"
#include "f28003x_flash_access.h"

//------------------------------------------------------------------------------
// Local function prototypes
//------------------------------------------------------------------------------
void Update_Boot_Version(void);

//------------------------------------------------------------------------------
// Data definitions
//------------------------------------------------------------------------------
//----- Global -----
DischargerCalibrationDataType llcCalibration = {0};
ChargerCalibrationDataType ahbCalibration = {0};
short outputVoltSetpointOffset = 0;
short chargeVoltSetpointOffset = 0;
short chargeCurrSetpointOffset = 0;
unsigned short waringCodeClearKey = 0x0;

//----- Local -----

//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------
/************************************************************************************************
Function Name:
    void Update_Boot_Version(void)
Input:
    None.
Output:
    None.
Comment:
    Update boot code version function.
************************************************************************************************/
void Update_Boot_Version(void)
{
    for (int i = 0; i < 4; i++)
    {
        bootVersion[i].low = *((unsigned char*)Boot_Version_Address + 2 * i);
        bootVersion[i].high = *((unsigned char*)Boot_Version_Address + 2 * i + 1);

        mfrID[i].low = Get_Char(Manufacturer, (2 * i));
        mfrID[i].high = Get_Char(Manufacturer, (2 * i + 1));
    }
}
//------------------------------------------------------------------------------
// Global functions
//------------------------------------------------------------------------------
/***************************************************************************************************
Function Name:
    void Disable_WatchDog(void)
Input:
    NULL
Output:
    NULL
Comment:
    Disable watchdog
***************************************************************************************************/
void Disable_WatchDog(void)
{
    EALLOW;
    WdRegs.WDCR.all = 0x0068;
    EDIS;
}
/***************************************************************************************************
Function Name:
    void Enable_WatchDog(void)
Input:
    NULL
Output:
    NULL
Comment:
    Watchdog initialization function. Watchdog timeout value is defined as follows:

    Timeout = 256 / WDCLK = 256 * 16 * 2048 / 10MHz = 838.86ms
***************************************************************************************************/
void Enable_WatchDog(void)
{
    ServiceDog();

    union WDCR_REG wdTemp;

    wdTemp.bit.WDPRECLKDIV = 2; // PREDIVCLK = INTOSC1 / 2048
    wdTemp.bit.WDDIS = 0;
    wdTemp.bit.WDCHK = 5;
    wdTemp.bit.WDPS = 5;        // WDCLK = PREDIVCLK / 16

    EALLOW;
    WdRegs.WDCR.all = wdTemp.all;
    EDIS;
}
/***************************************************************************************************
Function Name:
    void Kick_WatchDog(void)
Input:
    NULL
Output:
    NULL
Comment:
    Kick watchdog
***************************************************************************************************/
void Kick_WatchDog(void)
{
    EALLOW;
    WdRegs.WDKEY.bit.WDKEY = 0x55;
    WdRegs.WDKEY.bit.WDKEY = 0xAA;
    EDIS;
}
/***************************************************************************************************
Function Name:
    void Initial(void)
Input:
    NULL
Output:
    NULL
Comment:
    Startup setting
    1. Setup peripheral
    2. Set IO initial state
    3. Initial communication
    4. Initial LLC controller
***************************************************************************************************/
void Initial(void)
{
    Disable_WatchDog();

    Hw_Setup();

    Init_Flash_API(FLASH_BANK0_ACTIVE);

    Update_Boot_Version();

//    Load_Ext_Calibration_Data();
    Reload_Ext_Calibration_Data();

    // IO initial state
    LLC_Driver_Disable();
    SR_Driver_Disable();

    CHG_Driver_Disable();

    Oring_Off();
    IBUS_Disable();

    //Enable the OVP detection
    CHG_OVP_Unlatch_Active();
    DISCHG_Out_OVP_Unlatch_Active();
    DISCHG_In_OVP_Unlatch_Active();

    // Communication
    // Communication Key initial state
    //pfcCalibrationCtrl = 0;
    bootSecData.protectKey = 0;
    bootSecData.initKey = 0;
    bootSecData.state.stateCode = 0;
    warningCode.status2.bit.Engineer_OK = 0;

    // Internal - UART (communicate with primary)
    //dmaLinaRxObj.func.init(&dmaLinaRxObj, (long)&LinaRegs.SCIRD);
    //dmaLinaTxObj.func.init(&dmaLinaTxObj, (long)&LinaRegs.SCITD);
    //linaObj.func.init(&linaObj, DMA_LIN_ENABLE);

    // Internal - SCI (communicate with tertiary)
    sciaObj.func.init(&sciaObj, SCI_8_BIT | SCI_FIFO_ENABLE);

    // External - I2C
    masterI2cObj.func.init(&masterI2cObj, I2C_7bit_ADDRESS);

    // Command list initial
//    intProtocol.func.init(&intProtocol, &intCommand);
//    extProtocol.func.init(&extProtocol, &extCalibration);
//    extProtocol.func.init(&extProtocol, &extCommonCommand);

    //TER command list initial
    sciProtocol.func.init(&sciProtocol, &intPage0Command);

    Enable_WatchDog();

    Set_Llc_Mode(LLC_OFF_MODE);

    ahbTestPeriod = 216; //AHB duty 33%
#if(AHBTestMode)
    ahbVoltReference = 0;//wait for determine
    ahbCurrReference = 0;
#else
    ahbVoltReference = 0;//wait for determine
    ahbCurrReference = 0;
#endif

    Set_Ahb_Mode(AHB_OFF_MODE);

    //eventlog
    eventList.func.init(&eventList, &masterI2cObj,
                        masterI2cObj.func.receive,
                        masterI2cObj.func.transmit,
                        masterI2cObj.func.resetModule);

    if((eventList.member.buffer[eventList.member.pointer].index.block > Log_Block_Length) &&
       (eventList.member.buffer[eventList.member.pointer].index.line > Log_Line_Length) &&
       (eventList.member.buffer[eventList.member.pointer].index.cycleCount == 0xFF))
    {
        eraseTrig = 0x5A;
    }
}
//------------------------------------------------------------------------------
