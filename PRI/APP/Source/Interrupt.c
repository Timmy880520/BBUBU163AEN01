/***************************************************************************************************
File Name: Interrupt.c
External Data:
    Name                                    Source
    --------------------------------------- --------------------------------------------------------

External Functions:
    Name                                    Source
    --------------------------------------- --------------------------------------------------------
    Main_Task                               main.c
Description:

====================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- --------------------------------------------------------------------
    10/17/2019  Fred Huang      Create file
    03/19/2021  Fred Huang      1. Remove comment
                                2. Remove fan content
    05/06/2021  Watch Lee       1. add Timer2_ISR function.
                                1. mask systemTimer and delayResetMCU.

***************************************************************************************************/

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "App.h"

//------------------------------------------------------------------------------
// Local function prototypes

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Data definitions
#pragma SET_DATA_SECTION("cpuToCla1MsgRAM")
long ulongCpuToClaMsg[16];
long longCpuToClaMsg[16];
#pragma SET_DATA_SECTION()

#pragma DATA_SECTION(claMode, "cpuToCla1MsgRAM")
volatile STATE_MODE claMode = MODE_DEFAULT;
//------------------------------------------------------------------------------
//----- Global -----
// ADC result
// ADC - A
volatile unsigned short adcCurrShareVolt;
volatile unsigned short adcOutputCurr;
volatile unsigned short adcOutputCurrPK;
// ADC - B
volatile unsigned short adcOutputVolt;
volatile unsigned short adcOutputCurrSec;
volatile unsigned short adcBattVolt;
// ADC - C
volatile unsigned short adcChargeVolt;
volatile unsigned short adcInnerVolt;
volatile unsigned short adcChargeCurr;
// Raw Power = ADC_Voltage * ADC_Current
volatile unsigned short rawOutputPower;
volatile unsigned short rawStandOutputPower;
//Ray add
volatile unsigned short rawChargePower;

unsigned long cpuTimer1Countdown = 0, cpuTimer2Countdown = 0;
//----- Local -----
static Create_Timer(systemTimer, 50);
static Create_Timer(delayResetMCU, 50e3);

static short countChgOcp = 0;
static short countChgScp = 0;

static long countACLossRecover = 0;
static long countACLoss = 0;
static long countSyncStartAct = 0;
static long countSyncStartInAct = 0;

unsigned short usTest = 0;
unsigned short usTest2 = 0;
unsigned char Timercount = 0;
//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------

/**************************************************************************************************
Function Name:
    __interrupt void Timer2_ISR(void)
Input:
    None.
Output:
    None.
Comment:
    CPU timer 2 ISR function. This function is used for operating the main task.
**************************************************************************************************/
__interrupt void Timer2_ISR(void)
{
    EINT;                       // Clear INTM to enable Nested ISR

    State_Machine();
    if (++Timercount >= 2)
    {
        Main_Task();
        Timercount = 0;
    }
    cpuTimer2Countdown = CpuTimer2Regs.TIM.all;
    CpuTimer2Regs.TCR.bit.TIF = 1;

    DINT;                       // Set INTM to close Nested ISR
}
/**************************************************************************************************
Function Name:
    __interrupt void Timer1_ISR(void)
Input:
    None.
Output:
    None.
Comment:
    CPU timer 1 ISR function. This function is used for operating the LCC control task.
**************************************************************************************************/

/**************************************************************************************************
Function Name:
    long Filter_CMA_Calculation(FilterCMA *obj)
Input:
    *obj    - Object pointer of CMA filter.
    newData - New data insert CMA filter.
Output:
    result  - Filter result.
Comment:
    CMA filter calculation.
    - Difference equation
        -> y[n] = (a0 * y[n-1] + u[n]) / 2^n, a0 = 2^n - 1, n(exponent) < 16
    - Maximum input data = 4095
    - Coefficient Q format = 15
    - Sample frequency = 1kHz
    -----------------------------
    | Shift | Cut-off frequency |
    |----------------------------
    |   1   |      115 Hz       |
    |   2   |       46 Hz       |
    |   3   |       21 Hz       |
    |   4   |       10 Hz       |
    |   5   |        5 Hz       |
    |   6   |      2.5 Hz       |
    -----------------------------
    - Sample frequency = 50kHz
    -----------------------------
    | Shift | Cut-off frequency |
    |----------------------------
    |   1   |     5737 Hz       |
    |   2   |     2300 Hz       |
    |   3   |     1062 Hz       |
    |   4   |      512 Hz       |
    |   5   |      252 Hz       |
    |   6   |      125 Hz       |
    |   7   |       62 Hz       |
    |   8   |       31 Hz       |
    |   9   |       15 Hz       |
    |  10   |        8 Hz       |
    -----------------------------
**************************************************************************************************/
__attribute__((ramfunc))
__interrupt void Timer1_ISR(void)
{
    adcOutputCurr    = ADC_RESULT(a,0); // ADCA9 ADC_IO
    adcCurrShareVolt = ADC_RESULT(a,1); // ADCA4 ADC_VCS
    adcOutputCurrPK  = ADC_RESULT(a,2); // ADCA6 ADC_I_PRI_PEAK

    adcOutputCurrSec = ADC_RESULT(b,0); // ADCB6 ADC_IO_READING
    adcOutputVolt    = ADC_RESULT(b,1); // ADCB0 ADC_VO_COMP
    adcBattVolt      = ADC_RESULT(b,2); // ADCB3 ADC_VBATT

    adcInnerVolt     = ADC_RESULT(c,0); // ADCC3 ADC_VO_ORING
    adcChargeVolt    = ADC_RESULT(c,1); // ADCC1 ADC_VBATT_CHG
    adcChargeCurr    = ADC_RESULT(c,2); // ADCC6 ADC_ICHG

    if (adcChargeCurr >= CURR_ADC2_OFFSET)
        adcChargeCurr = adcChargeCurr - CURR_ADC2_OFFSET;
    else
        adcChargeCurr = 0;

    if (adcOutputCurr >= CURR_ADC2_OFFSET)
        adcOutputCurr = adcOutputCurr - CURR_ADC2_OFFSET;
    else
        adcOutputCurr = 0;

    if (adcCurrShareVolt >= CURR_ADC2_OFFSET)
        adcCurrShareVolt = adcCurrShareVolt - CURR_ADC2_OFFSET;
    else
        adcCurrShareVolt = 0;

    rawOutputPower = __IQmpy(adcOutputCurr, adcOutputVolt, 12);

    // ADC value convert
    avgInnerVolt.sum      = avgInnerVolt.sum + adcInnerVolt - avgInnerVolt.cma;
    avgInnerVolt.cma      = avgInnerVolt.sum >> avgInnerVolt.shift;
    avgInnerVolt.sum1     = avgInnerVolt.sum1 + adcInnerVolt - avgInnerVolt.cma1;
    avgInnerVolt.cma1     = avgInnerVolt.sum1 >> avgInnerVolt.shift1;

    avgOutputVolt.sum     = avgOutputVolt.sum + adcOutputVolt - avgOutputVolt.cma;
    avgOutputVolt.cma     = avgOutputVolt.sum >> avgOutputVolt.shift;
    avgOutputVolt.sum1    = avgOutputVolt.sum1 + adcOutputVolt - avgOutputVolt.cma1;
    avgOutputVolt.cma1    = avgOutputVolt.sum1 >> avgOutputVolt.shift1;

    avgOutputCurr.sum     = avgOutputCurr.sum + adcOutputCurr - avgOutputCurr.cma;
    avgOutputCurr.cma     = avgOutputCurr.sum >> avgOutputCurr.shift;
    avgOutputCurr.sum1    = avgOutputCurr.sum1 + adcOutputCurr - avgOutputCurr.cma1;
    avgOutputCurr.cma1    = avgOutputCurr.sum1 >> avgOutputCurr.shift1;

    avgOutputCurrSec.sum  = avgOutputCurrSec.sum + adcOutputCurrSec - avgOutputCurrSec.cma;
    avgOutputCurrSec.cma  = avgOutputCurrSec.sum >> avgOutputCurrSec.shift;
    avgOutputCurrSec.sum1 = avgOutputCurrSec.sum + adcOutputCurrSec - avgOutputCurrSec.cma1;
    avgOutputCurrSec.cma1 = avgOutputCurrSec.sum1 >> avgOutputCurrSec.shift1;

    avgCurrShareVolt.sum  = avgCurrShareVolt.sum + adcCurrShareVolt - avgCurrShareVolt.cma;
    avgCurrShareVolt.cma  = avgCurrShareVolt.sum >> avgCurrShareVolt.shift;
    avgCurrShareVolt.sum1 = avgCurrShareVolt.sum1 + adcCurrShareVolt - avgCurrShareVolt.cma1;
    avgCurrShareVolt.cma1 = avgCurrShareVolt.sum1 >> avgCurrShareVolt.shift1;

    avgChargeVolt.sum     = avgChargeVolt.sum + adcChargeVolt - avgChargeVolt.cma;
    avgChargeVolt.cma     = avgChargeVolt.sum >> avgChargeVolt.shift;
    avgChargeVolt.sum1    = avgChargeVolt.sum1 + adcChargeVolt - avgChargeVolt.cma1;
    avgChargeVolt.cma1    = avgChargeVolt.sum1 >> avgChargeVolt.shift1;

    avgBattVolt.sum       = avgBattVolt.sum + adcBattVolt - avgBattVolt.cma;
    avgBattVolt.cma       = avgBattVolt.sum >> avgBattVolt.shift;
    avgBattVolt.sum1      = avgBattVolt.sum1 + adcBattVolt - avgBattVolt.cma1;
    avgBattVolt.cma1      = avgBattVolt.sum1 >> avgBattVolt.shift1;

    avgChargeCurr.sum     = avgChargeCurr.sum + adcChargeCurr - avgChargeCurr.cma;
    avgChargeCurr.cma     = avgChargeCurr.sum >> avgChargeCurr.shift;
    avgChargeCurr.sum1    = avgChargeCurr.sum1 + adcChargeCurr - avgChargeCurr.cma1;
    avgChargeCurr.cma1    = avgChargeCurr.sum1 >> avgChargeCurr.shift1;

    avgInnerVolt.val      = Real_Value_Calculation(&avgInnerVolt);
    avgOutputVolt.val     = Real_Value_Calculation(&avgOutputVolt);
    avgOutputCurr.val     = Real_Value_Calculation(&avgOutputCurr);
    avgCurrShareVolt.val  = Real_Value_Calculation(&avgCurrShareVolt);
    avgOutputCurrSec.val  = Real_Value_Calculation(&avgOutputCurrSec);

    avgChargeCurr.val     = Real_Value_Calculation(&avgChargeCurr);
    avgBattVolt.val       = Real_Value_Calculation(&avgBattVolt);
    avgChargeVolt.val     = Real_Value_Calculation(&avgChargeVolt);

    LlcController();
    AhbController();

    CPU_UpdateMode();

//    Move Discharger soft start from main task to 50kHz ISR
    if (softstartFlag)
        softstartDone = Soft_Start_Routine();
    if (chargerSoftstartFlag)
        CurrsoftstartDone = Soft_Start_Routine_Curr();
    //====================== CHG UVP==========================
//    if(adcOutputVolt > 2470 || adcInnerVolt > 2470)// 2470 = bus 600V
//        chargerSoftstartFlag = 1;
//    if (chargerSoftstartFlag == 1 && adcChargeVolt < 2388) // 2388 = batt 217V
//    {
//        Set_Ahb_Mode(AHB_OFF_MODE);
//        CHG_Driver_Disable();
//    }
//    //====================== CHG OCP==========================
//    if(avgChargeCurr.cma > 745) //4A*150m*(4095/3.3) = 744.54
//    {
//        if(++countChgScp > COUNT_100us_IN_50kHz)
//        {
//            countChgScp = 0;
//            Set_Ahb_Mode(AHB_OFF_MODE);
//            CHG_Driver_Disable();
//            warningCode.flag.bit.iChargeOcFault = 1;
//        }
//    }
//    else
//    {
//        countChgScp = 0;
//    }
//
//    if(avgChargeCurr.cma > 372) //2A*150m*(4095/3.3) = 372.2
//    {
//        if(++countChgOcp > COUNT_1ms_IN_50kHz)
//        {
//            countChgOcp = 0;
//            Set_Ahb_Mode(AHB_OFF_MODE);
//            CHG_Driver_Disable();
//            warningCode.flag.bit.iChargeOcFault = 1;
//        }
//    }
//    else
//    {
//        countChgOcp = 0;
//    }

    if(AC_Loss())
    {
        countACLossRecover = 0;

        if(++countACLoss >= COUNT_200us_IN_50kHz)
        {
            SYNC_Start_Out_Active();
            if(countACLoss >= COUNT_500us_IN_50kHz)
            {
                dcdcState.bit.acLoss = true;
            }
        }
    }
    else if(NON_AC_Loss())
    {
        countACLoss = 0;

        if(++countACLossRecover >= COUNT_200us_IN_50kHz)
        {
            SYNC_Start_Out_Inactive();
            dcdcState.bit.acLoss = false;
        }
    }

    if (SYNC_Start())
    {
        countSyncStartInAct = 0;

        if (++countSyncStartAct >= COUNT_300us_IN_50kHz)
        {
            warningCode.status2.bit.SYNC_START = true;
        }
    }
    else if (NON_SYNC_Start())
    {
        countSyncStartAct = 0;

        if (++countSyncStartInAct >= COUNT_300us_IN_50kHz)
        {
            warningCode.status2.bit.SYNC_START = false;
        }
    }

    eventList.func.timer(&eventList);
    cpuTimer1Countdown = CpuTimer1Regs.TIM.all;
    CpuTimer1Regs.TCR.bit.TIF = 1;
}

__interrupt void Default_ISR(void)
{
    while(1);
}
/**************************************************************************************************
Function Name:
    void CPU_UpdateMode(void)
Input:

Output:

Comment:

**************************************************************************************************/
void CPU_UpdateMode(void)
{
    if (cpuLlcState.bit.enableLlc)
    {
        claMode = MODE_DISCHARGE;
    }
    else if (cpuAhbState.bit.enableAhb)
    {
        claMode = MODE_CHARGE;
    }
    else
        claMode = MODE_DEFAULT;
}
/**************************************************************************************************
Function Name:
    unsigned short Get_Timer_Tick(void)
Input:
    TmrPkg    - The pointer points to a timer structure which is counted up by this
                function to detect a timeout condition.
Output:
    return    - Result. 1 = a timeout condition is detected, 0 = no timeout condition.
Comment:
    Get software timer flag.
**************************************************************************************************/
unsigned short Get_Timer_Tick(void)
{
    unsigned short flag;

    flag = systemTimer.reg.bits.flg;

    return flag;
}
/**************************************************************************************************
Function Name:
    void Set_Timer_Tick(void)
Input:
    TmrPkg    - The pointer points to a timer structure which is counted up by this
                function to detect a timeout condition.
Output:

Comment:
    Set software timer flag.
**************************************************************************************************/
void Set_Timer_Tick(void)
{
    systemTimer.reg.bits.flg = false;
}
/**************************************************************************************************
Function Name:
    unsigned short Get_ResetMCU_Tick(void)
Input:
    TmrPkg    - The pointer points to a timer structure which is counted up by this
                function to detect a timeout condition.
Output:
    return    - Result. 1 = a timeout condition is detected, 0 = no timeout condition.
Comment:
    Get ResetMCU timer flag.
**************************************************************************************************/
unsigned short Get_ResetMCU_Tick(void)
{
    unsigned short flag;

    flag = delayResetMCU.reg.bits.flg;

    return flag;
}
/**************************************************************************************************
Function Name:
    void Set_ResetMCU_Tick(void)
Input:
    TmrPkg    - The pointer points to a timer structure which is counted up by this
                function to detect a timeout condition.
Output:

Comment:
    Set ResetMCU timer flag.
**************************************************************************************************/
void Set_ResetMCU_Tick(void)
{
    delayResetMCU.reg.bits.flg = false;
}
/**************************************************************************************************
Function Name:
    __interrupt void PMBusA_ISR(void)
Input:

Output:

Comment:

**************************************************************************************************/
/*
__interrupt void PMBusA_ISR(void)
{
    Interrupt_Sevice_Slave_PMBus(&pmbusRegObj);
    PieCtrlRegs.PIEACK.bit.ACK8 = 1;
}
*/
