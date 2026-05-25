/***********************************************************************************************************************
File Name: main.c
IDE Info: Code Composer Studio v10.2.0.00009
Compiler Info: Code Composer Studio v20.2.2.LTS
External Data:
    Name                                    Source
    --------------------------------------- ----------------------------------------------------------------------------
    EALLOW                                  F28004x_device.h
    EDIS                                    F28004x_device.h
    EPwm1Regs                               f28004x_epwm.h
    EPwm2Regs                               f28004x_epwm.h
    EPwm3Regs                               f28004x_epwm.h
    EPwm4Regs                               f28004x_epwm.h
    ECap1Regs                               f28004x_ecap.h
    adcOutputVolt                           Interrupt.c
    adcOringVolt                            Interrupt.c
    adcOutputCurr                           Interrupt.c
    adcOutput54vCurr                        Interrupt.c
    adcStandbyVolt                          Interrupt.c
    adcStandbyCurr                          Interrupt.c
    adcTempSr                               Interrupt.c
    adcTempOring                            Interrupt.c
    rawOutputPower                          Interrupt.c
    NTC_Table1                              LookupTable.c
    pfcLogBuffer                            COM_UART.c
    psuState                                COM_CAN.c

External Functions:
    Name                                    Source
    --------------------------------------- ----------------------------------------------------------------------------
    MAIN_CNT                                App.h
    BULK_FAULT                              Io.h
    BULK_OK                                 Io.h
    DRIVER_DISABLE                          Io.h
    DRIVER_ENABLE                           Io.h
    IBUS_DISABLE                            Io.h
    IBUS_ENABLE                             Io.h
    ORING_OFF                               Io.h
    ORING_ON                                Io.h
    OVP_UNLATCH_INACTIVE                    Io.h
    OVP_UNLATCH_ACTIVE                      Io.h
    VBUS_UNB                                Io.h
    VBUS_GOOD                               Io.h
    STANDBY_FAULT                           Io.h
    STANDBY_OK                              Io.h
    STANDBY_ENABLE                          Io.h
    STANDBY_DISABLE                         Io.h
    PSKILL                                  Io.h
    NON_PSKILL                              Io.h
    DACA_VAL                                Io.h
    DACB_VAL                                Io.h
    Initial                                 Initial.c
    Kick_WatchDog                           Initial.c
    Get_Timer_Tick                          Interrupt.c
    Set_Timer_Tick                          Interrupt.c
    Real_Value_Calculation                  Univ_Lib.c
    NTC_Lookup                              Univ_Lib.c
    CMA_Calculation                         Univ_Lib.c
    Range_Check                             Univ_Lib.c
    Soft_Start_Routine                      LlcDriver.c
    Sr_Pwm_On                               LlcDriver.c
    Sr_Pwm_Off                              LlcDriver.c
    Set_Voltage_Setpoint                    LlcDriver.c
    Set_Llc_Mode                            LlcDriver.c
    CAN_Routine                             COM_CAN.c
    Process_CAN_Data                        COM_CAN.c
    UART_Routine                            COM_UART.c
    Process_UART_Data                       COM_UART.c
    Current_Sharing                         LlcDriver.c
    CAN_Download_Routine                    CAN_Download.c
Description:
    DPST-6000 E BA PSU secondary main program.
    This is a interleaving LLC converter with standard 54.5V output voltage and 110A output current.
========================================================================================================================
History:
    Date        Author          Description of Change
    ----------- --------------- ----------------------------------------------------------------------------------------
    10/04/2019  Fred Huang      Create file
    04/08/2020  Fred Huang      Rev0 function
                                - State machine
                                - ADC convert
                                - Protection procedure
    03/07/2021  Fred Huang      1. Put calibration data into Create_CMA_Real_Value macro
                                2. Change offsetOutputVolt from variable to pointer
    03/08/2021  Fred Huang      1. Add 100ms routine for current sharing
                                2. Add offsetStandbyVolt
    03/10/2021  Fred Huang      1. Add cntCanbusEnable for enable CANBus after 5 seconds
                                2. Remove offsetOutputVolt and offsetStandbyVolt
                                3. Modify Fan fault latency to 5 seconds
                                4. Modify VBUS_UNB procedure
                                5. Write initial working state to POWERON_MODE
                                6. Remove all psuStatus reference
    03/11/2021  Fred Huang      1. Modify VBUS_UNB protection, refer to OVP_IO
                                2. Recovery operate under OVP_IO, OVP_ADC, OT, VBUS_UNB, rest will be latched
                                3. Remove all ddState reference
    03/17/2021  Fred Huang      1. Add Can_Address_Capture function
                                2. Put main routine at the front
                                3. Replace priAppVersion, priBootVersion with pfcAppVersion and pfcBootVersion
                                4. Replace Shut_Down function with global variable canTurnOffFlag
    03/19/2021  Fred Huang      1. Remove Fan_Gpio_Pulse_Count
                                2. Change all -- counter to ++ counter
                                3. OCP and UVP won't junp to recovery mode but directly to latch mode
                                4. Clean warning status when unlatch key is recieved
                                5. Remove front fan
                                6. Remove __IQ intrinsics function
                                7. Modify Create_Protection_Pkg to meet new ProtectionPkt
    05/06/2021  Watch Lee       1. Group all statements of 1 millisecond routine to Main_Task function.
                                2. Add CAN_Download_Routine to the while loop of main function.
                                3. Modify DPST6000EBA_SEC_App.cmd.

***********************************************************************************************************************/

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "App.h"

//------------------------------------------------------------------------------
// Local function prototypes
//------------------------------------------------------------------------------
//void State_Machine(void);
void State_Detection(void);
void Precharge_Procedure(void);
void Turn_On_Procedure(void);
void Turn_Off_Procedure(void);
void State_Check(void);
// Function contain register manipulate
void IO_Check(void);
void Resonant_Current_OCP_Check(void);
void Output_Current_OCP_Check(void);
void Clean_WarningCode_Status(void);
void Clean_TrigZone_Status(void);
void Clear_WarningCode_State(unsigned short key);
void Eventlog_Task(void);
//------------------------------------------------------------------------------
// Data definitions
//------------------------------------------------------------------------------
//----- Header -----
const WordType APP_SIGNATURE = {Signature_Code};
#pragma DATA_SECTION(APP_SIGNATURE, ".signature");
const BinHeaderMainType TEXT_HEADER_ROM = App_Header_List;
#pragma DATA_SECTION(TEXT_HEADER_ROM, ".appheader");
const unsigned long COMPLIANT_TABLE_ADDRESS = (unsigned long)COMPLIANT_TABLE;
#pragma DATA_SECTION(COMPLIANT_TABLE_ADDRESS, ".compliantaddress");
const CodeVersionType VERSION_ROM = Version_List;
#pragma DATA_SECTION(VERSION_ROM, ".headerreserved");
const unsigned char COMPLIANT_TABLE[10][20] = Compliant_Model;
#pragma DATA_SECTION(COMPLIANT_TABLE, ".complianttable");

// Primary APP code version
TwoByteType appVersion[4] = {0, 0, 0, 0, 0, 0, 0 ,0};//{9, 10, 0, 1, 2, 3, 4 ,5};

// Boot code version
TwoByteType bootVersion[4] = {{0}, {0}, {0}, {0}};
unsigned char energy;
// MFR list content
TwoByteType mfrID[4] = {0, 0, 0, 0};
//----- Local -----
static EnumWorkMode workingState = POWERON_MODE;
static long countPowerOn = 0;
static long countSleep = 0;

unsigned short flagPowerOn = false;
static long countLlcTurnOff = 0;
static long countLlcFaultOff = 0;
static long delaySrTurnOn = 0;
static long countSrTurnOn = 0;
static long countSrTurnOff = 0;
static unsigned short flagSrTurnOn = false;

static char flagBbuoff = 0;
static long countBbuoff = 0;
static long countBbuoffRecover = 0;
static long countFailOut = 0;
static long countFailOutRecover = 0;

static long countAhbTurnOff = 0;
static long countAhbTurnOff2 = 0;

static long countOvpBusIoFault = 0;
static long countOvpBattIoFault = 0;
static long countOvpChgBattIoFault = 0;
static long countOvpBusChgIoFault = 0;

static long countPriOcpDisIoFault = 0;
static long countOcpDisIoFault = 0;

static long countEStop1 = 0;
static long countEStop2 = 0;
static long countEStop1Recover = 0;
static long countEStop2Recover = 0;
static long countReset = 0;
static long countResetRecover = 0;
static long countChargeEnOut = 0;
static long countChargeEnOutRecover = 0;
static long countSohOut = 0;
static long countSohOutRecover = 0;
static long delayOringTurnOn = COUNT_50ms_IN_1kHz;
static long countOringTurnOn = 0;
static long countOringTurnOff = 0;
static long rampDelayCount = 0;

unsigned long countdownDischargeTimes = 0;
unsigned char prechargeDone = 0;
static unsigned short countDelayprecharge1 = 0;
static unsigned short countDelayprecharge2 = 0;
static unsigned short countDelayprecharge3 = 0;

unsigned short chargerSoftstart = 0;
unsigned short chargerSoftstartFlag = 0;
unsigned short ahbVoltChgDone = 0;
unsigned short ahbCurrChgDone = 0;
unsigned short softstartFlag = 0;
unsigned short softstartDone = 1;
unsigned short CurrsoftstartDone = 1;
unsigned short targetVolt = 0;

unsigned short eraseTrig = 0, eventTrig = 0, eventReadPointer = 0, readPageNotDone = 0;
unsigned short latchRecord = 1;
unsigned long secondCounter = 0;
unsigned char highLimitFreq = 0;
unsigned long countUnlatch = 0;
unsigned char forceDischarge = 0;
unsigned char sohOutFlag = 0;
unsigned char sohOutChgFlag = 0;
unsigned char resetButton = 0xAA;
static unsigned short countDischargeSecond = 0;
static unsigned short countSohOutLowPower = 0;
static Create_Protection_Pkg(protSrTurnOn, avgOutputCurr.val, 100, 500, COUNT_10ms_IN_1kHz, COUNT_10ms_IN_1kHz, \
                             COUNT_10ms_IN_1kHz);
static Create_Protection_Pkg(protOringCtrl, avgOutputCurr.val, 0, 300, COUNT_1ms_IN_1kHz, COUNT_1ms_IN_1kHz, \
                             COUNT_1ms_IN_1kHz);

// Event and protection
// LLC Vout
Create_Event_Counter(checkVoutOvf,  COUNT_40ms_IN_1kHz,    COUNT_40ms_IN_1kHz);
Create_Event_Counter(checkVoutUvf,  COUNT_40ms_IN_1kHz,    COUNT_40ms_IN_1kHz);
// LLC Iout
Create_Event_Counter(checkIoutOcf1, COUNT_200ms_IN_1kHz,   COUNT_200ms_IN_1kHz);
Create_Event_Counter(checkIoutOcf2, COUNT_100ms_IN_1kHz,   COUNT_100ms_IN_1kHz);
Create_Event_Counter(checkIoutOcf3, COUNT_60ms_IN_1kHz,    COUNT_100ms_IN_1kHz);
// AHB Vout Iout
Create_Event_Counter(checkVbattOvf, COUNT_40ms_IN_1kHz,    COUNT_40ms_IN_1kHz);
Create_Event_Counter(checkVbattUvf, COUNT_100ms_IN_1kHz,   COUNT_100ms_IN_1kHz);
Create_Event_Counter(checkIchargeOcf, COUNT_100ms_IN_1kHz, COUNT_100ms_IN_1kHz);

// Timer inside 1ms main routine
static Create_Timer(currentshareTimer, 100);
static Create_Timer(bmscurrentfeedbackTimer, 250);
Create_Timer(dischargerOperationTimer, 60); //60s in 1s routine
//----- Global -----
WarningCodePkt warningCode;
unsigned short canAddress = 0xFE;
I2cRegObj pmbusRegObj;
PrechargeState powerOnStateCheck = STATE_IDLE;

#pragma SET_DATA_SECTION(".ramgs0")
Create_Event_List_Object(eventList, 16);
#pragma SET_DATA_SECTION()
// ADC value to Real value package
// ADC Scale may change depend on the unit.
// ADV Voltage unit: 0.1V, current unit: 0.01A
Create_CMA_Real_Value(avgOutputVolt, adcOutputVolt, 5, 5, llcCalibration.gainVout, \
                      llcCalibration.offsetVout, 4725); //New add 4724.78->4725 //7439.85->7440

Create_CMA_Real_Value(avgOutputCurr, adcOutputCurr, 2, 5, llcCalibration.gainIout, \
                      llcCalibration.offsetIout, 11111); //11111 sens (26.4mV/A) //10120 sens (33mV/A) //13394 sens (25mV/A) //16667 sens (19.8mV/A)

Create_CMA_Real_Value(avgCurrShareVolt, adcCurrShareVolt, 2, 5, llcCalibration.gainIoutShare, \
                      llcCalibration.offsetIoutShare, 11111); //11111 sens (26.4mV/A) //10120 sens (33mV/A) //13394 sens (25mV/A) //16667 sens (19.8mV/A)

Create_CMA_Real_Value(avgOutputCurrSec, adcOutputCurrSec, 2, 5, llcCalibration.gainIout, \
                      llcCalibration.offsetIout, 11111); //11111 sens (26.4mV/A) //10120 sens (33mV/A) //13394 sens (25mV/A) //16667 sens (19.8mV/A)

Create_CMA_Real_Value(avgInnerVolt, adcInnerVolt, 2, 5, llcCalibration.gainVoutOring, \
                      llcCalibration.offsetVoutOring, 4725); //New add 4724.78->4725 //7439.85->7440

Create_CMA_Real_Value(avgOutputPower, rawOutputPower, 10, 5, calibrationDataDefault.arg.gain, \
                      calibrationDataDefault.arg.offset, 13848);

Create_CMA_Real_Value(avgChargePower, rawChargePower, 10, 5, calibrationDataDefault.arg.gain, \
                      calibrationDataDefault.arg.offset, 13848);

Create_CMA_Real_Value(avgBattVolt, adcBattVolt, 2, 5,llcCalibration.gainVin, \
                      llcCalibration.offsetVin , 3739); //New add 3739.24->3739 //7439.85->7440

Create_CMA_Real_Value(avgChargeCurr, adcChargeCurr, 2, 5, ahbCalibration.gainIout, \
                      ahbCalibration.offsetIout, 2188); //2188 sens (150mV/A) //10000 sens (33mV/A)

Create_CMA_Real_Value(avgChargeVolt, adcChargeVolt, 2, 5, ahbCalibration.gainVout, \
                      ahbCalibration.offsetVout, 3739); //New add 3739.24->3739 //7439.85->7440

CpuUsageType cpuTimer2Task = {0ul, 0.0f};
CpuUsageType cpuTimer1Task = {0ul, 0.0f};

LlcStateType cpuLlcState = {0};
AhbStateType cpuAhbState = {0};
BbuItemType bbuItem = {0};
/***************************************************************************************************
Function Name:
    int main(void)
Input:
    NULL
Output:
    return - Always 0
Comment:
    Background routine
***************************************************************************************************/
int main(void)
{
    Initial();      // ~700us
    Clean_WarningCode_Status();
    workingState = POWERON_MODE;//POWERON_MODE
    flagPowerOn = false;

    DACA_VAL(0);
    DACB_VAL(0);

    while(1)
    {
        Kick_WatchDog();
        // Background operating --------------------------------------------------------------

        eventList.func.routine(&eventList);

        static unsigned long cpuTimer2TickValue = 0, cpuTimer1TickValue = 0;

        cpuTimer2TickValue = CPU_Timer2_Base - cpuTimer2Countdown;
        cpuTimer2Task.usageQ12 = (cpuTimer2TickValue * CPU_Timer2_Tick_Scale_Q15) >> 15;
        cpuTimer2Task.usage = (float)cpuTimer2Task.usageQ12 * Float_Divisor_Q12;
        cpuTimer1TickValue = CPU_Timer1_Base - cpuTimer1Countdown;
        cpuTimer1Task.usageQ12 = (cpuTimer1TickValue * CPU_Timer1_Tick_Scale_Q15) >> 15;
        cpuTimer1Task.usage = (float)cpuTimer1Task.usageQ12 * Float_Divisor_Q12;
    }
}
//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------
/**************************************************************************************************
Function Name:
    void Main_Task(void)
Input:
    None.
Output:
    None.
Comment:
    LLC main routine function. This function is put into a 1ms timer ISR.
**************************************************************************************************/
void Main_Task(void)
{
    if(flagPowerOn)
    {
        IO_Check();
        State_Check();
        State_Detection();
    }
    Eventlog_Task();

    UART_Routine();
    Process_UART_Data();
    Live_Update_State_Machine();

    // 100ms routine
    // For current sharing usage
    if(Call_Timer(currentshareTimer))
    {
        Timer_Flag(currentshareTimer) = 0;

        if(workingState == DISCHARGER_MODE)
        {
            Current_Sharing();
        }
    }

    // 250ms routine
    // For bms current feedback usage
//    if(Call_Timer(bmscurrentfeedbackTimer) && (!warningCode.status2.bit.Engineer_OK))
//    {
//        Timer_Flag(bmscurrentfeedbackTimer) = 0;
//
//        if(workingState == CHARGER_MODE)
//        {
//            BMS_Current_Feedback();
//        }
//    }
}
/**************************************************************************************************
Function Name:
    State_Machine(void)
Input:
    NULL
Output:
    NULL
Comment:
    Run state machine in 2kHz background routine
**************************************************************************************************/
void State_Machine(void)
{
    dcdcState.all &= ~(0xC0070000); // clean the state of mode

    switch(workingState)
    {
    default:
    case POWERON_MODE:
        // Power on will do nothing till 8s is counted
        if (++countPowerOn >= COUNT_8s_IN_2kHz)
        {
            countPowerOn = 0;
            flagPowerOn = true;
            Bypass_AUX_RLY();
            workingState = SLEEP_MODE;
        }

        dcdcState.bit.bbukill = warningCode.status2.bit.bbuKill;
        break;

    case SLEEP_MODE:
        if ((avgOutputVolt.cma > (OUTPUT_VOLT_BUS(350) >> 2)) && (warningCode.status2.bit.ESTOP1 == 0) && (warningCode.status2.bit.ESTOP2 == 0) \
        && warningCode.status2.bit.bbuKill                    && ((1900 <= bmsData.battOverallVolt)    && (3100 >= bmsData.battOverallVolt)))
        {
            if (bbuItem.flag.dFet && ++countSleep >= COUNT_100ms_IN_2kHz)
            {
                countSleep = 0;
                workingState = STANDBY_MODE;
                powerOnStateCheck = STATE_CHECK_VOLTAGE;
            }
            else if (warningCode.status2.bit.chgEnOut && ++countSleep >= COUNT_100ms_IN_2kHz)
            {
                countSleep = 0;
                workingState = PRECHARGE_MODE;
                powerOnStateCheck = STATE_PRECHG_INIT;
            }
        }

        dcdcState.bit.bbukill = warningCode.status2.bit.bbuKill;
        break;

    case PRECHARGE_MODE:
        dcdcState.bit.prechargeOK = true;

        Precharge_Procedure();

        if (prechargeDone)
        {
            if (++countDelayprecharge3 >= COUNT_1s_IN_2kHz)
            {
                countDelayprecharge3 = 0;

                if (sciProtocol.func.getNoAckFlag(&sciProtocol, 0xC6) == 1)
                {
                    sciProtocol.func.packTxData(&sciProtocol, 0xC6, INTERNAL_SET, LOW_PRIORITY);
                }
                else if (bbuItem.flag.dFet)
                {
                    workingState = STANDBY_MODE;
                    prechargeDone = 0;
                    powerOnStateCheck = STATE_CHECK_VOLTAGE;
                }
            }
        }
        break;

    case STANDBY_MODE:
        dcdcState.bit.standbyOK = true;

        if (warningCode.status2.bit.FAILOUT)
        {
            BBU_Fault();
            workingState = LATCH_MODE;
        }
        else if (!warningCode.status2.bit.bbuKill || bbuItem.flag.dFet == 0)
        {
            workingState = SLEEP_MODE;
        }
        else if ((dcdcState.bit.acLoss   || warningCode.status2.bit.SYNC_START || (forceDischarge == 0xDC) || ((learningMode == 0x01) && warningCode.status2.bit.sohOut)) \
              && (warningCode.flag.bit.vBattUvFault == 0))
        {
            if ((learningMode == 0x01) && warningCode.status2.bit.sohOut)
            {
                sohOutFlag = 1;
                sohOutChgFlag = 1;
                learningCycle.all = 0;
                cpuLlcState.bit.sohOutFlag = 1;
                Set_Voltage_Setpoint(DEFAULT_OUTPUT_VOLT + outputVoltSetpointOffset - 100); //394V
            }

            Turn_On_Procedure();
        }
        else if ((warningCode.flag.bit.chgOTP == 0)         && (warningCode.flag.bit.chgOTW == 0)                && (warningCode.flag.bit.AMB_OTP == 0)        && (warningCode.flag.bit.AMB_OTW == 0) \
              && (warningCode.flag.bit.fan1FrontFault == 0) && (warningCode.flag.bit.fan1RearFault == 0)         && (warningCode.flag.bit.fan2FrontFault == 0) && (warningCode.flag.bit.fan2RearFault == 0) \
              && (dcdcState.bit.acLoss == 0)                && (avgOutputVolt.cma > (OUTPUT_VOLT_BUS(350) >> 2)) && warningCode.status2.bit.chgEnOut)
        {
            if (bbuItem.flag.cFet == 0)
            {
                Precharge_Procedure();
            }
            else
            {
                prechargeDone = 1;
            }

            if (prechargeDone)
            {
                if (++countDelayprecharge3 >= COUNT_1s_IN_2kHz)
                {
                    countDelayprecharge3 = 0;

                    if (sciProtocol.func.getNoAckFlag(&sciProtocol, 0xC6) == 1)
                    {
                        sciProtocol.func.packTxData(&sciProtocol, 0xC6, INTERNAL_SET, LOW_PRIORITY);
                    }
                    else if (bbuItem.flag.cFet == 1)
                    {
                        bmsCurrentOffset = 0;
                        ahbVoltSetpoint = ((OUTPUT_VOLT_BATT(DEFAULT_CHARGE_VOLT + chargeVoltSetpointOffset)) * __IQ(0.1, 14)) >> 14; // OUTPUT_VOLT_BATT(304) = 13320
                        ahbVoltReference = adcChargeVolt << 2;
                        chargerSoftstart = 1;
                        countAhbTurnOff = 0;
                        countAhbTurnOff2 = 0;
                        prechargeDone = 0;
                        workingState = CHARGER_MODE;
                    }
                }
            }
        }
        break;

    case DISCHARGER_SOFTSTART_MODE:
        Turn_Off_Procedure();
        LLC_Driver_Enable();
        softstartFlag = 1;

        // Soft-start
        if(!softstartDone)//Move Soft_Start_Routine(); to 50kHz ISR to raise voltage faster.
        {
            softstartFlag = 0;
            flagSrTurnOn = false;
            delaySrTurnOn = COUNT_100ms_IN_2kHz;
            workingState = DISCHARGER_MODE;
        }
        break;

    case DISCHARGER_MODE:
        if (sohOutFlag == 0)
            dcdcState.bit.dischargerOK = true;

        if (sohOutFlag == 1 && (dcdcState.bit.acLoss || warningCode.status2.bit.SYNC_START) && (warningCode.flag.bit.vBattUvFault == 0))
        {
            sohOutFlag = 0;
            learningMode = 0;
            learningCycle.bit.acLoss = 1;
            cpuLlcState.bit.sohOutFlag = 0;
        }

        if (sohOutFlag == 1)
        {
            if ((avgInnerVolt.val * avgOutputCurr.val < MAX_DIS_POWER * 1000) && (++countSohOutLowPower >= COUNT_10s_IN_2kHz))
            {
                learningMode = 0;
                learningCycle.bit.lowDischargePower = 1;
            }
            else if (avgInnerVolt.val * avgOutputCurr.val > MAX_DIS_POWER * 1000)
            {
                countSohOutLowPower = 0;
            }
        }
        else
        {
            countSohOutLowPower = 0;
        }

        // Count discharger operation times, maximum 60s
        if (++countDischargeSecond >= COUNT_1s_IN_2kHz && !warningCode.status2.bit.Engineer_OK && sohOutFlag == 0)
        {
            countDischargeSecond = 0;
            Call_Timer(dischargerOperationTimer);
            countdownDischargeTimes = Timer_Countdown(dischargerOperationTimer);
        }
        else if (warningCode.status2.bit.Engineer_OK)
        {
            Timer_Reset(dischargerOperationTimer);
        }

        Turn_Off_Procedure();

        // Oring control
        Range_Check(&protOringCtrl);
        if (protOringCtrl.flag == OVER_RANGE)   // Use output current to determine oring on-off
        {
            if(++countOringTurnOn >= delayOringTurnOn)
            {
                countOringTurnOn = 0;
                Oring_On();
            }
        }
        else if (protOringCtrl.flag == UNDER_RANGE)   // < 5Av & de-bounce 10ms, disable ORING
        {
            if (++countOringTurnOff >= COUNT_10ms_IN_1kHz)
            {
                countOringTurnOff = 0;
                Oring_Off();
            }
        }

        // SR control refer to output current
        Range_Check(&protSrTurnOn);
        if ((flagSrTurnOn == false) && protSrTurnOn.flag == OVER_RANGE)
        {
            if (++countSrTurnOn >= delaySrTurnOn)
            {
                flagSrTurnOn = true;
                countSrTurnOn = 0;
                delaySrTurnOn = COUNT_2ms_IN_2kHz;
                SR_Driver_Enable();
                Sr_Pwm_On();
            }
        }
        else if ((flagSrTurnOn == true) && protSrTurnOn.flag == UNDER_RANGE)
        {
            if (++countSrTurnOff >= COUNT_2ms_IN_2kHz)
            {
                flagSrTurnOn = false;
                countSrTurnOff = 0;
                SR_Driver_Disable();
                Sr_Pwm_Off();
            }
        }
        break;

    case CHARGER_MODE:
        dcdcState.bit.chargerOK = true;

        if (warningCode.status2.bit.ESTOP1   || warningCode.status2.bit.ESTOP2   || warningCode.flag.bit.vBattOvFault      || warningCode.flag.bit.iChargeOcFault \
        ||  warningCode.flag.bit.vOutUvFault || warningCode.flag.bit.vOutOvFault || (warningCode.status2.bit.bbuKill == 0) || warningCode.status2.bit.FAILOUT)
        {
            Warning_Storage();

            if (++countAhbTurnOff > COUNT_1ms_IN_2kHz)
            {
                countAhbTurnOff = 0;
                sohOutChgFlag = 0;
                Set_Ahb_Mode(AHB_OFF_MODE);
                CHG_Driver_Disable();
                workingState = LATCH_MODE;
                powerOnStateCheck = STATE_CHECK_VOLTAGE;
            }

            if (warningCode.status2.bit.FAILOUT)
                BBU_Fault();
        }
        else if (warningCode.flag.bit.chgOTP         || warningCode.flag.bit.chgOTW        || warningCode.flag.bit.AMB_OTP        || warningCode.flag.bit.AMB_OTW \
              || warningCode.flag.bit.fan1FrontFault || warningCode.flag.bit.fan1RearFault || warningCode.flag.bit.fan2FrontFault || warningCode.flag.bit.fan2RearFault \
              || (bbuItem.flag.cFet == 0)            || (bbuItem.flag.dFet == 0)           || (warningCode.status2.bit.chgEnOut == 0))
        {
            if (++countAhbTurnOff2 > COUNT_100ms_IN_2kHz)
            {
                countAhbTurnOff2 = 0;
                sohOutChgFlag = 0;
                Set_Ahb_Mode(AHB_OFF_MODE);
                CHG_Driver_Disable();
                workingState = STANDBY_MODE;
                powerOnStateCheck = STATE_CHECK_VOLTAGE;
            }
        }
        else if ((dcdcState.bit.acLoss || warningCode.status2.bit.SYNC_START || (forceDischarge == 0xDC)) && (warningCode.flag.bit.vBattUvFault == 0))
        {
            powerOnStateCheck = STATE_CHECK_VOLTAGE;
            Set_Ahb_Mode(AHB_OFF_MODE);
            CHG_Driver_Disable();
            Turn_On_Procedure();
            sohOutChgFlag = 0;
        }
        else
        {
            if (chargerSoftstart)
            {
                ahbCurrChgDone = 0;
                Set_Ahb_Mode(AHB_CC_MODE);
                CHG_Driver_Enable();
                chargerSoftstart = 0;
                chargerSoftstartFlag = 1;
            }

            if (!CurrsoftstartDone)
            {
                ahbCurrChgDone = 1;
                chargerSoftstartFlag = 0;
                Set_Ahb_Mode(AHB_ON_MODE);
                ahbVoltReference += 2;
                
                if (ahbVoltReference >= ahbVoltSetpoint)
                {
                    ahbVoltReference = ahbVoltSetpoint; // ahbVoltSetpoint = 13342
                    ahbVoltChgDone = 1;
                    CurrsoftstartDone = 1;
                }
            }
        }
        break;

    case LATCH_MODE:
        dcdcState.bit.latchOK = true;

        Warning_Storage();

        if (latchRecord)
        {
            eventList.func.record(&eventList);
            latchRecord = 0;
        }

        if (protectRelease == 0xAA || warningCode.status2.bit.RESET_BUTTON)
        {
            Clean_WarningCode_Status();
            sciProtocol.func.packTxData(&sciProtocol, 0xCC, INTERNAL_SET, LOW_PRIORITY);

            if (++countUnlatch >= COUNT_5ms_IN_2kHz)
            {
                workingState = STANDBY_MODE;
                protectRelease = 0;
                countUnlatch = 0;
                latchRecord = 1;
            }
        }
        break;

    case FAULT_MODE:
        break;
    case SOH_TEST_MODE:
        break;
    }
}
/**************************************************************************************************
Function Name:
    void Precharge_Procedure(void)
Input:
    NULL
Output:
    NULL
Comment:
    While insert BBU, charge DCDC capacitor then turn-on D-FET.
**************************************************************************************************/
void Precharge_Procedure(void)
{
    if (warningCode.status2.bit.ESTOP1   || warningCode.status2.bit.ESTOP2   || warningCode.flag.bit.vBattOvFault      || warningCode.flag.bit.iChargeOcFault \
    ||  warningCode.flag.bit.vOutUvFault || warningCode.flag.bit.vOutOvFault || (warningCode.status2.bit.bbuKill == 0) || warningCode.status2.bit.FAILOUT)
    {
        Warning_Storage();

        powerOnStateCheck = STATE_FAULT;

        if (++countAhbTurnOff > COUNT_200ms_IN_2kHz)
        {
            countAhbTurnOff = 0;
            CHG_Driver_Disable();
            Set_Ahb_Mode(AHB_OFF_MODE);
            workingState = LATCH_MODE;
        }

        if (warningCode.status2.bit.FAILOUT)
            BBU_Fault();
    }

    switch (powerOnStateCheck)
    {
        case STATE_PRECHG_INIT:
        {
            countDelayprecharge1 = 0;
            countDelayprecharge2 = 0;
            countDelayprecharge3 = 0;
            prechargeDone = 0;
            powerOnStateCheck = STATE_BYPASS_RLY2;
            break;
        }
        case STATE_BYPASS_RLY2:
        {
            if (++countDelayprecharge1 >= COUNT_700ms_IN_2kHz)
            {
                Bypass_PRECHG2_RLY();
                powerOnStateCheck = STATE_BYPASS_RLY1;
            }
            break;
        }
        case STATE_BYPASS_RLY1:
        {
            if (++countDelayprecharge1 >= COUNT_800ms_IN_2kHz)
            {
                Bypass_PRECHG1_RLY();
                targetVolt = OUTPUT_VOLT_BATT(0.1*(float)avgBattVolt.val);
                ahbVoltReference = OUTPUT_VOLT_BATT(0.1*(float)avgChargeVolt.val);
                powerOnStateCheck = STATE_PRECHG_BATT_CHG_CAP;
            }
            break;
        }
        case STATE_PRECHG_BATT_CHG_CAP: //4
        {
            long diff = ABS(avgChargeVolt.cma - avgBattVolt.cma);

            if (diff <= PRECHG_DIFF_THREDHOLD)
            {
                if (++countDelayprecharge2 >= COUNT_50ms_IN_2kHz)
                {
                    Bypass_BATT_RLY();
                    CHG_Driver_Disable();
                    Set_Ahb_Mode(AHB_OFF_MODE);
                    targetVolt = OUTPUT_VOLT_BATT(0.1*(float)bmsData.battOverallVolt);
                    powerOnStateCheck = STATE_PRECHG_CHG_OUTPUT;
                }
            }
            else if (avgChargeVolt.cma <= avgBattVolt.cma)
            {
                if (++rampDelayCount >= 5)
                {
                    rampDelayCount = 0;
                    ahbVoltReference += 1;
                }
                CONSTRAIN_MAX(ahbVoltReference, targetVolt);
                Set_Ahb_Mode(AHB_CV_MODE);
                CHG_Driver_Enable();
            }
            break;
        }
        case STATE_PRECHG_CHG_OUTPUT:
        {
            ahbVoltReference += 2;

            if (ahbVoltReference >= targetVolt)
            {
                ahbVoltReference = targetVolt;
                powerOnStateCheck = STATE_CHECK_FOR_D_FET;
            }

            Set_Ahb_Mode(AHB_CV_MODE);
            CHG_Driver_Enable();
            break;
        }
        case STATE_CHECK_FOR_D_FET:
        {
            if (avgBattVolt.val >= (bmsData.battOverallVolt - 50)) //Unit 0.1V
            {
                powerOnStateCheck = STATE_COMPLETED; //7
                Turnoff_BATT_RLY();
                Set_Ahb_Mode(AHB_OFF_MODE);
                CHG_Driver_Disable();
                bmsFetControl.bit.cFetEnable = 1;
                bmsFetControl.bit.dFetEnable = 1;
                sciProtocol.func.packTxData(&sciProtocol, 0xC6, INTERNAL_SET, LOW_PRIORITY);
                prechargeDone = 1;
            }
            break;
        }
        case STATE_CHECK_VOLTAGE: //8
        {
            ahbVoltReference = OUTPUT_VOLT_BATT(0.1*(float)avgChargeVolt.val);
            targetVolt = OUTPUT_VOLT_BATT(0.1*(float)bmsData.battOverallVolt);
            powerOnStateCheck = STATE_PRECHG_CHG_CAP;
            break;
        }
        case STATE_PRECHG_CHG_CAP: //9
        {
            ahbVoltReference += 2;
            if (ahbVoltReference >= targetVolt)
            {
                ahbVoltReference = targetVolt;
                powerOnStateCheck = STATE_CHECK_FOR_C_FET;
            }

            Set_Ahb_Mode(AHB_CV_MODE);
            CHG_Driver_Enable();
            break;
        }
        case STATE_CHECK_FOR_C_FET:
        {
            if (avgChargeVolt.val >= (bmsData.battOverallVolt - 50)) //Unit 0.1V
            {
                powerOnStateCheck = STATE_COMPLETED;
                Turnoff_BATT_RLY();
                Set_Ahb_Mode(AHB_OFF_MODE);
                CHG_Driver_Disable();
                bmsFetControl.bit.cFetEnable = 1;
                bmsFetControl.bit.dFetEnable = 1;
                sciProtocol.func.packTxData(&sciProtocol, 0xC6, INTERNAL_SET, LOW_PRIORITY);
                prechargeDone = 1;
            }
            break;
        }
    }
}
/**************************************************************************************************
Function Name:
    void Turn_On_Procedure(void)
Input:
    NULL
Output:
    NULL
Comment:
    While receive ACLOSS or STNC_START, LLC should be turn on.
**************************************************************************************************/
void Turn_On_Procedure(void)
{
#if (OpenLoop)
//    if(BULK_OK() && (flagPsKill == false))
    if(warningCode.status2.bit.bbuKill == 1)
#else
    if ((warningCode.status2.bit.bbuKill == 1)    && (warningCode.flag.bit.srOTP == 0)          && (warningCode.flag.bit.srOTW == 0)         && (warningCode.flag.bit.oringOTP == 0) \
    &&  (warningCode.flag.bit.oringOTW == 0)      && (warningCode.flag.bit.dchgOTP == 0)        && (warningCode.flag.bit.dchgOTW == 0)       && (warningCode.flag.bit.AMB_OTP == 0) \
    &&  (warningCode.flag.bit.AMB_OTW == 0)       && (warningCode.flag.bit.fan1FrontFault == 0) && (warningCode.flag.bit.fan1RearFault == 0) && (warningCode.flag.bit.fan2FrontFault == 0) \
    &&  (warningCode.flag.bit.fan2RearFault == 0) && (warningCode.status2.bit.ESTOP1 == 0)      && (warningCode.status2.bit.ESTOP2 == 0))
#endif
    {
        //reset turn off counter
        highLimitFreq = 0;
        countLlcTurnOff = 0;
        countLlcFaultOff = 0;
        Set_Llc_Mode(LLC_ON_MODE);
        IBUS_Enable();
        Clean_WarningCode_Status();
//        Clear_WarningCode_State(0x55AA);
        workingState = DISCHARGER_SOFTSTART_MODE;
    }
}
/**************************************************************************************************
Function Name:
    void Turn_Off_Procedure(void)
Input:
    NULL
Output:
    NULL
Comment:
    While receive BULK_FAULT or PSKILL for 10ms, LLC should be turn off.
**************************************************************************************************/
void Turn_Off_Procedure(void)
{
    if (warningCode.flag.bit.srOTP           || warningCode.flag.bit.srOTW               || warningCode.flag.bit.oringOTP        || warningCode.flag.bit.oringOTW \
    ||  warningCode.flag.bit.dchgOTP         || warningCode.flag.bit.dchgOTW             || warningCode.flag.bit.AMB_OTP         || warningCode.flag.bit.AMB_OTW \
    ||  warningCode.flag.bit.fan1FrontFault  || warningCode.flag.bit.fan1RearFault       || warningCode.flag.bit.fan2FrontFault  || warningCode.flag.bit.fan2RearFault \
    || (dcdcState.bit.acLoss == 0            && warningCode.status2.bit.SYNC_START == 0  && forceDischarge != 0xDC               && sohOutFlag == 0) \
    || warningCode.flag.bit.vBattUvFault     || (bbuItem.flag.dFet == 0) \
    || ((learningMode != 0x01                || warningCode.status2.bit.sohOut == 0)     && sohOutFlag == 1))
    {
        Warning_Storage();

        if (++countLlcTurnOff >= COUNT_10ms_IN_2kHz)
        {
            if (sohOutFlag == 1)
            {
                if (startFwUpgrade == 1)
                {
                    startFwUpgrade = 0;
                    learningCycle.bit.fwUpgrade = 1;
                }
                else if ((learningMode == 0x01) && warningCode.status2.bit.sohOut)
                {
                    learningCycle.bit.fail = 1;
                }

                learningMode = 0;
                sohOutFlag = 0;
                cpuLlcState.bit.sohOutFlag = 0;
            }

            highLimitFreq = 1;

            Timer_Reset(dischargerOperationTimer);
            countLlcTurnOff = 0;
            workingState = STANDBY_MODE;
            Set_Llc_Mode(LLC_OFF_MODE);
            LLC_Driver_Disable();
            SR_Driver_Disable();
            IBUS_Disable();
            Oring_Off();
        }
    }
    else
    {
        countLlcTurnOff = 0;
    }

    if (warningCode.status2.bit.ESTOP1 || warningCode.status2.bit.ESTOP2    || (warningCode.status2.bit.bbuKill == 0) || warningCode.flag.bit.vOutOvFault || Timer_Flag(dischargerOperationTimer) \
    || (countLlcFaultOff > 0)          || warningCode.flag.bit.iOutOcFault  || warningCode.flag.bit.vBattOvFault      || warningCode.status2.bit.FAILOUT)
    {
        Warning_Storage();

        if (++countLlcFaultOff >= COUNT_10ms_IN_2kHz)
        {
            if (sohOutFlag == 1)
            {
                if (warningCode.status2.bit.bbuKill == 0)
                {
                    learningCycle.bit.notInstalled = 1;
                }
                else if ((learningMode == 0x01) && warningCode.status2.bit.sohOut)
                {
                    learningCycle.bit.fail = 1;
                }

                learningMode = 0;
                sohOutFlag = 0;
                cpuLlcState.bit.sohOutFlag = 0;
            }

            highLimitFreq = 1;

            Timer_Reset(dischargerOperationTimer);
            countLlcFaultOff = 0;
            workingState = LATCH_MODE;
            Set_Llc_Mode(LLC_OFF_MODE);
            LLC_Driver_Disable();
            SR_Driver_Disable();
            IBUS_Disable();
            Oring_Off();

            if (warningCode.status2.bit.FAILOUT)
                BBU_Fault();
        }
    }
    else
    {
        countLlcFaultOff = 0;
    }
}

/***************************************************************************************************
Function Name:
    void State_Check(void)
Input:
    NULL
Output:
    NULL
Comment:
    Convert all ADC IQ data to real value, then check ADC protections
***************************************************************************************************/
void State_Check(void)
{
    static long ulTemp1 = 0, ulTemp2 = 0;

    // ADC value convert, move the low bit real value calculation to the 50kHz interrupt
    avgInnerVolt.val1     = Real_Value_Calculation1(&avgInnerVolt);
    avgOutputVolt.val1    = Real_Value_Calculation1(&avgOutputVolt);
    avgOutputCurr.val1    = Real_Value_Calculation1(&avgOutputCurr);
    avgCurrShareVolt.val1 = Real_Value_Calculation1(&avgCurrShareVolt);
    avgOutputCurrSec.val1 = Real_Value_Calculation1(&avgOutputCurrSec);

    avgChargeCurr.val1    = Real_Value_Calculation1(&avgChargeCurr);
    avgBattVolt.val1      = Real_Value_Calculation1(&avgBattVolt);
    avgChargeVolt.val1    = Real_Value_Calculation1(&avgChargeVolt);

    ulTemp1 = avgOutputCurr.val;
    ulTemp2 = avgInnerVolt.val;
    ulTemp1 = (ulTemp1 * ulTemp2) >> 7;
    ulTemp1 = (ulTemp1 * 66 ) >> 9;
    avgOutputPower.val = ulTemp1;//ulTemp1 * ulTemp2;//Real_Value_Calculation(&avgOutputPower);


    ulTemp1 = avgChargeVolt.val; //Use BMS current data to calculate input power(Unit: 0.01A)
    ulTemp2 = avgChargeCurr.val;
    ulTemp1 = (ulTemp1 * ulTemp2) >> 7;
    ulTemp1 = (ulTemp1 * 66 ) >> 9;
    avgChargePower.val = ulTemp1;
}

/***************************************************************************************************
Function Name:
    void IO_Check(void)
Input:
    NULL
Output:
    NULL
Comment:
    Check GPIO protection:
        1: Hardware OVP input
            GPIO31 -> TRIP1 -> DCAH -> DCAEVT1 -> OSHT
     x   2: Hardware bus voltage unbalance
            GPIO14 -> TRIP2 -> DCBH -> DCBEVT1 -> OSHT
     x   3: Hardware resonant current OCP
            CMP3H -> TRIP4 -> DCAL -> DCAEVT2 -> CBC
            CMP5H -> TRIP5 -> DCBL -> DCBEVT2 -> CBC
        4: In slot or not
            GPIO 30 -> PSKILL
        5: Hardware Standby fault
            GPIO 57 -> STB_FAULT
***************************************************************************************************/
void IO_Check(void)
{
    // GPIO31, Discharger Output OVP
    if (EPwm1Regs.TZFLG.bit.DCAEVT1 || EPwm8Regs.TZFLG.bit.DCAEVT1 || EPwm4Regs.TZFLG.bit.DCAEVT1 \
     || EPwm5Regs.TZFLG.bit.DCAEVT1 || EPwm2Regs.TZFLG.bit.DCAEVT1 || EPwm7Regs.TZFLG.bit.DCAEVT1)
    {
        if ((++countOvpBusIoFault >= COUNT_5ms_IN_1kHz) && (warningCode.status.bit.OVP_BUS_IO == false))
        {
            Clean_TrigZone_Status();
            warningCode.status.bit.OVP_BUS_IO = true;
        }
        Clean_TrigZone_Status();
    }
    else
    {
        countOvpBusIoFault = 0;
        warningCode.status.bit.OVP_BUS_IO = false;
    }

    // GPIO55, Discharger Input OVP
    if (EPwm1Regs.TZFLG.bit.DCBEVT2 || EPwm8Regs.TZFLG.bit.DCBEVT2 || EPwm4Regs.TZFLG.bit.DCBEVT2 \
     || EPwm5Regs.TZFLG.bit.DCBEVT2 || EPwm2Regs.TZFLG.bit.DCBEVT2 || EPwm7Regs.TZFLG.bit.DCBEVT2)
    {
        if ((++countOvpBattIoFault >= COUNT_5ms_IN_1kHz) && (warningCode.status.bit.OVP_BATT_IO == false))
        {
            Clean_TrigZone_Status();
            warningCode.status.bit.OVP_BATT_IO = true;
        }
        Clean_TrigZone_Status();
    }
    else
    {
        countOvpBattIoFault = 0;
        warningCode.status.bit.OVP_BATT_IO = false;
    }

    // Pri OCP
    Resonant_Current_OCP_Check();
    // Output OCP
    Output_Current_OCP_Check();

    // GPIO9, Charge OVP
    if (EPwm6Regs.TZFLG.bit.DCAEVT1)
    {
        if ((++countOvpChgBattIoFault >= COUNT_5ms_IN_1kHz) && (warningCode.status.bit.OVP_CHG_BATT_IO == false))
        {
            EALLOW;
            EPwm6Regs.TZCLR.bit.DCAEVT1 = 1;
            EDIS;
            warningCode.status.bit.OVP_CHG_BATT_IO = true;
        }
        EALLOW;
        EPwm6Regs.TZCLR.bit.DCAEVT1 = 1;
        EDIS;
    }
    else
    {
        countOvpChgBattIoFault = 0;
        warningCode.status.bit.OVP_CHG_BATT_IO = false;
    }

    if (EPwm6Regs.TZFLG.bit.DCAEVT2)
    {
        if ((++countOvpBusChgIoFault >= COUNT_5ms_IN_1kHz) && (warningCode.status.bit.OVP_BUS_CHG_IO == false))
        {
            EALLOW;
            EPwm6Regs.TZCLR.bit.DCAEVT2 = 1;
            EDIS;
            warningCode.status.bit.OVP_BUS_CHG_IO = true;
        }
        EALLOW;
        EPwm6Regs.TZCLR.bit.DCAEVT2 = 1;
        EDIS;
    }
    else
    {
        countOvpBusChgIoFault = 0;
        warningCode.status.bit.OVP_BUS_CHG_IO = false;
    }

    //GPIO 30, off to on ->> PSON >> PSKILL >> BBUKILL
    if (NON_BBU_Kill())
    {
        countBbuoffRecover = 0;

        if (++countBbuoff >= COUNT_5ms_IN_1kHz)
        {
            warningCode.status2.bit.bbuKill = false;
            DD_Fault();
            countBbuoff = 0;
            flagBbuoff = 1;
        }
    }
    else if (BBU_Kill())
    {
        countBbuoff = 0;
        if (++countBbuoffRecover >= COUNT_5ms_IN_1kHz)
        {
            warningCode.status2.bit.bbuKill = true;
            NON_DD_Fault();
            countBbuoffRecover = 0;
            if(flagBbuoff == 1)
            {
                flagBbuoff = 0;
                warningCode.status2.bit.OFF2ON = 1;
            }
        }
    }

    if (Fail_Out())
    {
        countFailOutRecover = 0;

        if (++countFailOut >= COUNT_5ms_IN_1kHz)
            warningCode.status2.bit.FAILOUT = true;

    }
    else if (NON_Fail_Out())
    {
        countFailOut = 0;

        if (++countFailOutRecover >= COUNT_5ms_IN_1kHz)
            warningCode.status2.bit.FAILOUT = false;
    }

    if (EM_Stop1())
    {
        countEStop1Recover = 0;

        if (++countEStop1 >= COUNT_5ms_IN_1kHz)
            warningCode.status2.bit.ESTOP1 = true;

    }
    else if (NON_EM_Stop1())
    {
        countEStop1 = 0;

        if (++countEStop1Recover >= COUNT_5ms_IN_1kHz)
            warningCode.status2.bit.ESTOP1 = false;
    }

    if (EM_Stop2())
    {
        countEStop2Recover = 0;

        if (++countEStop2 >= COUNT_5ms_IN_1kHz)
            warningCode.status2.bit.ESTOP2 = false;
    }
    else if (NON_EM_Stop2())
    {
        countEStop2 = 0;

        if (++countEStop2Recover >= COUNT_5ms_IN_1kHz)
            warningCode.status2.bit.ESTOP2 = false;
    }

    if (Reset())
    {
        countResetRecover = 0;

        if (++countReset >= COUNT_5ms_IN_1kHz)
            warningCode.status2.bit.RESET_BUTTON = true;
    }
    else if (NON_Reset())
    {
        countReset = 0;

        if (++countResetRecover >= COUNT_10ms_IN_1kHz)
            warningCode.status2.bit.RESET_BUTTON = false;
    }

    if (CHG_EN_Out())
    {
        countChargeEnOutRecover = 0;

        if (++countChargeEnOut >= COUNT_5ms_IN_1kHz)
            warningCode.status2.bit.chgEnOut = true;
    }
    else if (NON_CHG_EN_Out())
    {
        countChargeEnOut = 0;

        if (++countChargeEnOutRecover >= COUNT_10ms_IN_1kHz)
            warningCode.status2.bit.chgEnOut = false;
    }

    if (SOH_Test())
    {
        countSohOutRecover = 0;

        if (++countSohOut >= COUNT_5ms_IN_1kHz)
            warningCode.status2.bit.sohOut = true;
    }
    else if (NON_SOH_Test())
    {
        countSohOut = 0;

        if (++countSohOutRecover >= COUNT_10ms_IN_1kHz)
            warningCode.status2.bit.sohOut = false;
    }

}
/***************************************************************************************************
Function Name:
    void Resonant_Current_OCP_Check(void)
Input:
    NULL
Output:
    NULL
Comment:
    Check resonant current OCP to cycle-by-cycle function and count for latch
***************************************************************************************************/
void Resonant_Current_OCP_Check(void)
{
    if(EPwm1Regs.TZFLG.bit.DCBEVT1 || EPwm8Regs.TZFLG.bit.DCBEVT1 || EPwm4Regs.TZFLG.bit.DCBEVT1 \
    || EPwm5Regs.TZFLG.bit.DCBEVT1 || EPwm2Regs.TZFLG.bit.DCBEVT1 || EPwm7Regs.TZFLG.bit.DCBEVT1)
    {
        if ((++countPriOcpDisIoFault >= COUNT_5ms_IN_1kHz) && (warningCode.status.bit.PRI_OCP_DISCHG_IO == false))
        {
            Clean_TrigZone_Status();
            warningCode.status.bit.PRI_OCP_DISCHG_IO = true;
        }
        Clean_TrigZone_Status();
    }
    else
    {
        countPriOcpDisIoFault = 0;
        warningCode.status.bit.PRI_OCP_DISCHG_IO = false;
    }
}
/***************************************************************************************************
Function Name:
    void Output_Current_OCP_Check(void)
Input:
    NULL
Output:
    NULL
Comment:
    Check Output current OCP to cycle-by-cycle function and count for latch
***************************************************************************************************/
void Output_Current_OCP_Check(void)
{
    if(EPwm1Regs.TZFLG.bit.DCAEVT2 || EPwm8Regs.TZFLG.bit.DCAEVT2 || EPwm4Regs.TZFLG.bit.DCAEVT2 \
    || EPwm5Regs.TZFLG.bit.DCAEVT2 || EPwm2Regs.TZFLG.bit.DCAEVT2 || EPwm7Regs.TZFLG.bit.DCAEVT2)
    {
        if ((++countOcpDisIoFault >= COUNT_5ms_IN_1kHz) && (warningCode.status.bit.OCP_DISCHG_IO == false))
        {
            Clean_TrigZone_Status();
            warningCode.status.bit.OCP_DISCHG_IO = true;
        }
        Clean_TrigZone_Status();
    }
    else
    {
        countOcpDisIoFault = 0;
        warningCode.status.bit.OCP_DISCHG_IO = false;
    }
}
/**************************************************************************************************
Function Name:
    void Clean_TrigZone_Status(void)
Input:
    NULL
Output:
    NULL
Comment:
    Clean the EPWM trip zone flag.
    Status means the currently warning flag.
**************************************************************************************************/
void Clean_TrigZone_Status(void)
{
    EALLOW;
    EPwm1Regs.TZCLR.bit.DCAEVT1 = 1;
    EPwm8Regs.TZCLR.bit.DCAEVT1 = 1;
    EPwm4Regs.TZCLR.bit.DCAEVT1 = 1;
    EPwm5Regs.TZCLR.bit.DCAEVT1 = 1;
    EPwm2Regs.TZCLR.bit.DCAEVT1 = 1;
    EPwm7Regs.TZCLR.bit.DCAEVT1 = 1;

    EPwm1Regs.TZCLR.bit.DCAEVT2 = 1;
    EPwm8Regs.TZCLR.bit.DCAEVT2 = 1;
    EPwm4Regs.TZCLR.bit.DCAEVT2 = 1;
    EPwm5Regs.TZCLR.bit.DCAEVT2 = 1;
    EPwm2Regs.TZCLR.bit.DCAEVT2 = 1;
    EPwm7Regs.TZCLR.bit.DCAEVT2 = 1;

    EPwm1Regs.TZCLR.bit.DCBEVT1 = 1;
    EPwm8Regs.TZCLR.bit.DCBEVT1 = 1;
    EPwm4Regs.TZCLR.bit.DCBEVT1 = 1;
    EPwm5Regs.TZCLR.bit.DCBEVT1 = 1;
    EPwm2Regs.TZCLR.bit.DCBEVT1 = 1;
    EPwm7Regs.TZCLR.bit.DCBEVT1 = 1;

    EPwm1Regs.TZCLR.bit.DCBEVT2 = 1;
    EPwm8Regs.TZCLR.bit.DCBEVT2 = 1;
    EPwm4Regs.TZCLR.bit.DCBEVT2 = 1;
    EPwm5Regs.TZCLR.bit.DCBEVT2 = 1;
    EPwm2Regs.TZCLR.bit.DCBEVT2 = 1;
    EPwm7Regs.TZCLR.bit.DCBEVT2 = 1;
    EDIS;
}
/**************************************************************************************************
Function Name:
    void Clean_WarningCode_Status(void)
Input:
    NULL
Output:
    NULL
Comment:
    Clean all warning status and EPWM trip zone flag.
    Status means the currently warning flag.
**************************************************************************************************/
void Clean_WarningCode_Status(void)
{
    // Clear all trip zone flag
    EALLOW;
    EPwm1Regs.TZCLR.all = 0x007F;
    EPwm8Regs.TZCLR.all = 0x007F;
    EPwm4Regs.TZCLR.all = 0x007F;
    EPwm2Regs.TZCLR.all = 0x007F;
    EPwm7Regs.TZCLR.all = 0x007F;
    EPwm5Regs.TZCLR.all = 0x007F;
    EPwm6Regs.TZCLR.all = 0x007F;
    EDIS;

    learningCycle.all = 0;
    warningCode.status.all = 0;
    warningCode.flag.all = 0;
    dcdcState.all = 0;
    dcdcAlarm.all = 0;
}
/**************************************************************************************************
Function Name:
    void Clear_WarningCode_State(unsigned short key)
Input:
    key         - Unlock key for waring code clear.
Output:
    None.
Comment:
    Clear the state region of warning code.
    State means the warning flag once occurs then it will be record and latched.
**************************************************************************************************/
void Clear_WarningCode_State(unsigned short key)
{
    static int clearFault = 0;

    // PSON switch OFF to ON
    if (warningCode.status2.bit.OFF2ON == 1)
    {
        warningCode.status2.bit.OFF2ON = 0;
        clearFault = 1;
    }

    // clear all fault
    if(key == 0x55AA || clearFault == 1)
    {
        Clean_WarningCode_Status();
        Timer_Reset(recordTimer);

        if (clearFault)
        {
            workingState = SLEEP_MODE;
            clearFault = 0;
        }
        waringCodeClearKey = 0;
    }
}
//------------------------------------------------------------------------------
// Global functions
//------------------------------------------------------------------------------
/***********************************************************************************************************************
Function Name:
    void Eventlog_Task(void)
Input:
    N/A
Output:
    N/A
Comment:
    The eventlog task from I2C
***********************************************************************************************************************/
void Eventlog_Task(void)
{
//    I2C_Routine();
    // calculate the run time of the DD module
    if (++secondCounter >= COUNT_1s_IN_1kHz)   // 1000ms
    {
        pmiTimes++;
        secondCounter = 0;
    }
    // record the data to the black box
    if ((eventTrig == 0xAA) && (eraseTrig != 0x5A))
    {
        eventList.func.record(&eventList);
        eventTrig = 0;
    }
    // erase the data of the black box
    if (eraseTrig == 0x5A)
    {
        if (eventList.func.format(&eventList))
        {
            pmiTimes = 0;
            eraseTrig = 0;
        }
    }
    //Process read eventlog to logPage
    if (readPageNotDone && eventList.member.queue.empty)
    {
        readPageNotDone = 0;
        sciProtocol.func.packTxData(&sciProtocol, 0xE0, INTERNAL_ACK, LOW_PRIORITY);
    }

    if (sciProtocol.func.getReloadFlag(&sciProtocol, 0xE0))
    {
        eventList.member.pageOffset = eventReadPointer;
        eventList.func.load(&eventList);
        readPageNotDone = 1;
        sciProtocol.func.clearReloadFlag(&sciProtocol, 0xE0);
    }
}
/************************************************************************************************
Function Name:
    void State_Detection(void)
Input:
    None.
Output:
    None.
Comment:
    Detect PFC state function. This function will monitor the measurement data, received
commands, and GPIO signals to judge PFC state.
************************************************************************************************/
void State_Detection(void)
{
    char fault = 0, recovery = 0;

    //Detect battery UV only in discharger mode
    if ((dcdcState.bit.dischargerOK == 1) || (dcdcState.bit.standbyOK == 1) || (dcdcState.bit.chargerOK == 1) || (sohOutFlag == 1))
    {
        //Vbatt UV Fault
        fault = (avgBattVolt.val < VBATT_UV_FAULT_LIMIT);
        recovery = (avgBattVolt.val > VBATT_UV_FAULT_RECOVERY);
        Update_Event_State(warningCode.flag.bit.vBattUvFault, checkVbattUvf, fault, recovery);
    }

    // Vout UV
    fault = (avgOutputVolt.val < VOUT_UV_FAULT_LIMIT);
    recovery = (avgOutputVolt.val > VOUT_UV_FAULT_RECOVERY);
    Update_Event_State(warningCode.status.bit.UVP_BUS_SW, checkVoutUvf, fault, recovery);
    warningCode.flag.bit.vOutUvFault = warningCode.status.bit.UVP_BUS_SW;

    //Vout OV //OVP implement by HW
    fault = (avgInnerVolt.val > VOUT_OV_FAULT_LIMIT);
    recovery = (avgInnerVolt.val < VOUT_OV_FAULT_RECOVERY);
    Update_Event_State(warningCode.status.bit.OVP_BUS_SW, checkVoutOvf, fault, 0);
    warningCode.flag.bit.vOutOvFault = warningCode.status.bit.OVP_BUS_IO | warningCode.status.bit.OVP_BUS_SW | warningCode.status.bit.OVP_BUS_CHG_IO;

    //Iout OC1 Fault
    fault = (avgOutputCurr.val > IOUT_OC1_FAULT_LIMIT);
    recovery = (avgOutputCurr.val < IOUT_OC1_FAULT_RECOVERY);
    Update_Event_State(warningCode.flag.bit.iOutOcFault1, checkIoutOcf1, fault, 0);

    //Iout OC2 Fault
//    fault = (avgOutputCurr.val > IOUT_OC2_FAULT_LIMIT);
//    recovery = (avgOutputCurr.val < IOUT_OC2_FAULT_RECOVERY);
//    Update_Event_State(warningCode.flag.bit.iOutOcFault2, checkIoutOcf2, fault, 0);

    //Iout OC3 Fault
    fault = (avgOutputCurr.val > IOUT_OC3_FAULT_LIMIT);
    recovery = (avgOutputCurr.val < IOUT_OC3_FAULT_RECOVERY);
    Update_Event_State(warningCode.flag.bit.iOutOcFault3, checkIoutOcf3, fault, 0);

    warningCode.status.bit.OCP_DISCHG_SW = warningCode.flag.bit.iOutOcFault1 | warningCode.flag.bit.iOutOcFault2 \
                                         | warningCode.flag.bit.iOutOcFault3 | warningCode.flag.bit.iOutScFault4;

    warningCode.flag.bit.iOutOcFault = warningCode.status.bit.OCP_DISCHG_SW | warningCode.status.bit.OCP_DISCHG_IO \
                                     | warningCode.status.bit.PRI_OCP_DISCHG_IO;

    if ((dcdcState.bit.chargerOK == 1) || (dcdcState.bit.prechargeOK == 1))
    {
        //add Battery protection
        //VCharge OV Fault  //OVP implement by HW
        fault = (avgChargeVolt.val > VBATT_OV_FAULT_LIMIT);
        recovery = (avgChargeVolt.val < VBATT_OV_FAULT_RECOVERY);
        Update_Event_State(warningCode.flag.bit.vChargeOvFault, checkVbattOvf, fault, 0);
    }

    warningCode.flag.bit.vBattOvFault = warningCode.flag.bit.vChargeOvFault | warningCode.status.bit.OVP_CHG_BATT_IO | warningCode.status.bit.OVP_BATT_IO;
}
//------------------------------------------------------------------------------
