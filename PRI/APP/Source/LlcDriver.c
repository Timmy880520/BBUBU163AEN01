/***************************************************************************************************
File Name: LlcDriver.c
External Data:
    Name                                    Source
    --------------------------------------- --------------------------------------------------------
    EPwm1Regs                               f28004x_epwm.h
    EPwm2Regs                               f28004x_epwm.h
    EPwm3Regs                               f28004x_epwm.h
    EPwm4Regs                               f28004x_epwm.h
    adcOutputVolt                           Interrupt.c
    adcOutputCurr                           Interrupt.c
    adcOutput54vCurr                        Interrupt.c
External Functions:
    Name                                    Source
    --------------------------------------- --------------------------------------------------------

Description:

====================================================================================================
History:
    Date        Author          Description of Change
    ----------- --------------- --------------------------------------------------------------------
    11/26/2019  Fred            Create file
    12/11/2019  Fred            Rev0
                                - Voltage loop controller
                                - Voltage feedforward
                                - PWM mode control
    03/10/2021  Fred Huang      Add VBUS_UNB procedure
    03/11/2021  Fred Huang      Remove VBUS_UNB procedure

***************************************************************************************************/

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "App.h"
#include "COM_UART.h"
#include "..\Libraries\Math\inc\Digital_Compensator.h"

//------------------------------------------------------------------------------
// Local function prototypes
//------------------------------------------------------------------------------
unsigned short Switch_Llc_Mode(void);
void Sr_Pwm_Off(void);
void Sr_Pwm_On(void);
void Llc_Pwm_On(void);
void Llc_Sr_Pwm_On(void);
void Llc_Pwm_Update(void);
void Voltage_Feedforward(void);
void Transient_SOTC(void);
void SOTC(void);
void SOTC2(void);
void Transient_DeadZone(void);
void Float_Type_III(void);

//------------------------------------------------------------------------------
// Data definitions
//------------------------------------------------------------------------------
//----- Global -----

//----- Local -----
AhbCtrlParam ahbParam;
AhbCtrlMode ahbMode = AHB_OFF_MODE;

static unsigned short modifyLlcMode = false;
LlcCtrlMode llcMode = LLC_OFF_MODE;
LlcCtrlState llcState = LLC_HALT;
LlcCtrlParam llcVoltParam;
LlcCtrlParam llcCurrParam;
LlcCtrlParam llcCurrShareParam;
LlcCtrlParam llcVoltDroopParam;
LlcCtrlParam llcSohOutParam;
LlcFloatPiControlType sohCurrLoop;
Create_Type_III_Compensator(llcVoltLoop, 14);
Create_Type_III_Compensator(llcCurrLoop, 14);

// Voltage set point and soft start
static unsigned long voltSetpoint = 0;
static float voltDroopVoltage = 0;
static unsigned short voltSetpointModify;
static short voltSoftStartRef = 0;
static unsigned short voltSoftStartFlag;
static unsigned short voltSoftStartSetpoint1;
static unsigned short voltSoftStartStep1;
static unsigned short voltSoftStartSetpoint2;
static unsigned short voltSoftStartStep2;
static unsigned short openFlag = 0;
// SOH test
static short sohTestVolRefOffset = 0;
static float targetI = 0;
// Current share
static short csVoltRefOffset = 0;
// Voltage droop
static short droopVoltRefOffset = 0;
// Current set point and soft start
static unsigned long currSetpoint = 0;
static unsigned short currSetpointModify;
static unsigned short currSoftStartRef = 0;
static unsigned short currSoftStartFlag;
static unsigned short currSoftStartStep;
// PWM main variables
static long period, periodBuff;
static short priDutyLead;
static short priDutyLag;
static short priDeadband;
static short srDutyLead;
static short srDutyLag;
static short srDeadband;
// Voltage feed forward for output voltage ripple
#if RIPPLE_FEEDFORWARD
static long mafVoutSum;
static const long mafVoutShift = 5;
static long mafVoutAvgSum;
static const long mafVoutAvgShift = 10;
static long filterVout;
static long ripplePolarity;
static long rippleDelayCnt = 0;
static long rippleAntiStuckCnt = 0;
static long indexFeedforward = 0;
static const long indexFeedforwardMax = (CONTROLLER_FREQ / 200); // 100Hz AC input(Should be feed by PFC)
static long cntDecayFeedforward = ISR_CNT(5.0e-3f);
static long decayFeedforward = 0;
static short addendFeedforward = 0;
#endif
// SOTC - Current feed forward function for transient
long deltaPeriod = 0;
static long sotcCurrentLog[4];
static long sotcFiltCurrentLog[4];
static long sotcFiltError;
static unsigned char sotcState = 2;
static unsigned char sotcState2 = 2;
static short sotcLatchCnt;
static short sotcLatchCnt2;
// Burst mode
static long burstThreshold;
short testPeriod = 0;
unsigned short testPeriod2 = 0;
float fTest = 0.0f;
long lBuff = 0;
// SCP
static unsigned short SCP_Cnt=0;
static unsigned short PRI_OCP_Cnt=0;

unsigned char flagLatch = 0;
unsigned char punchUp = 0, punchDown = 0;
long sTemp = 0;

unsigned short ahbVoltSetpoint = 0;
unsigned short ahbCurrSetpoint = 0;
unsigned short ahbVoltReference = 0;
unsigned short ahbCurrReference = 0;
short bmsCurrentOffset = 0;

__attribute__((ramfunc))
void Float_Type_III(void)//_Compensation(CompensatorObj *obj, long reference, long feedback)
{
    llcVoltLoop.member.reference = llcVoltParam.reference;
    llcVoltLoop.member.feedback = llcVoltParam.feedback;
    llcVoltLoop.member.error = llcVoltLoop.member.reference - llcVoltLoop.member.feedback;
/*    if (llcVoltLoop.member.error > 340)
        llcVoltLoop.member.error = llcVoltLoop.member.error * 4;
    else if (llcVoltLoop.member.error < -340)
        llcVoltLoop.member.error = llcVoltLoop.member.error * 4;
*/

    // High stop filter process
    llcVoltLoop.member.hsFilter.un = (float)llcVoltLoop.member.error * Float_Qn_Divisor[llcVoltLoop.member.qNotation & 0x0F];
    llcVoltLoop.member.hsFilter.yn = llcVoltLoop.member.hsFilter.b0 * llcVoltLoop.member.hsFilter.un +
            llcVoltLoop.member.hsFilter.b1 * llcVoltLoop.member.hsFilter.un1 +
            llcVoltLoop.member.hsFilter.b2 * llcVoltLoop.member.hsFilter.un2 -
            llcVoltLoop.member.hsFilter.a1 * llcVoltLoop.member.hsFilter.yn1 -
            llcVoltLoop.member.hsFilter.a2 * llcVoltLoop.member.hsFilter.yn2;

    llcVoltLoop.member.hsFilter.un2 = llcVoltLoop.member.hsFilter.un1;
    llcVoltLoop.member.hsFilter.un1 = llcVoltLoop.member.hsFilter.un;
    llcVoltLoop.member.hsFilter.yn2 = llcVoltLoop.member.hsFilter.yn1;
    llcVoltLoop.member.hsFilter.yn1 = llcVoltLoop.member.hsFilter.yn;

    // PI compensation process
    llcVoltLoop.member.piControl.error = llcVoltLoop.member.hsFilter.yn;
    llcVoltLoop.member.piControl.pYn = llcVoltLoop.member.piControl.kp * llcVoltLoop.member.piControl.error;
    llcVoltLoop.member.piControl.iYn = llcVoltLoop.member.piControl.ki * llcVoltLoop.member.piControl.error + llcVoltLoop.member.piControl.iYn1;
    llcVoltLoop.member.piControl.iYn += llcVoltLoop.member.piControl.kc * llcVoltLoop.member.piControl.satError;
/*
    // Unnecessary if anti wind-up mechanism is executing correctly.
    if (obj->member.piControl.iYn > 1.0e4f)
        obj->member.piControl.iYn = 1.0e4f;
    else if (obj->member.piControl.iYn < -1.0e4f)
        obj->member.piControl.iYn = -1.0e4f;
*/
    llcVoltLoop.member.piControl.iYn1 = llcVoltLoop.member.piControl.iYn;
    llcVoltLoop.member.piControl.piYn = llcVoltLoop.member.piControl.pYn + llcVoltLoop.member.piControl.iYn;

    if (llcVoltLoop.member.piControl.piYn > llcVoltLoop.member.piControl.max)
        llcVoltLoop.member.piControl.output = llcVoltLoop.member.piControl.max;
    else if (llcVoltLoop.member.piControl.piYn < llcVoltLoop.member.piControl.min)
        llcVoltLoop.member.piControl.output = llcVoltLoop.member.piControl.min;
    else
        llcVoltLoop.member.piControl.output = llcVoltLoop.member.piControl.piYn;
    llcVoltLoop.member.piControl.satError = llcVoltLoop.member.piControl.output - llcVoltLoop.member.piControl.piYn;
    llcVoltLoop.member.piControl.output *= llcVoltLoop.member.piControl.ka;

    // Float to integer conversion
    llcVoltLoop.member.output = (long)(llcVoltLoop.member.piControl.output * (float)(1 << llcVoltLoop.member.qNotation));

    if (llcVoltLoop.member.output > llcVoltLoop.member.maximum)
        llcVoltLoop.member.output = llcVoltLoop.member.maximum;
    else if (llcVoltLoop.member.output < llcVoltLoop.member.minimum)
        llcVoltLoop.member.output = llcVoltLoop.member.minimum;
    //return llcVoltLoop.member.output;
}

//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------
/***************************************************************************************************
Function Name:
    unsigned short Switch_Llc_Mode(void)
Input:
    mode
Output:
    NULL
Comment:
    LLC mode change
***************************************************************************************************/
unsigned short Switch_Llc_Mode(void)
{
    if(modifyLlcMode)
    {
        // Initial OFF state
        Llc_Sr_Pwm_Off();

        llcVoltParam.reference = 0;
        llcVoltParam.feedback  = 0;
        llcVoltParam.output    = 0;
        llcVoltParam.maximum   = MAX_SWITCH_PERIOD;
        llcVoltParam.minimum   = MIN_SWITCH_PERIOD;

        // Integer parameters
        llcVoltLoop.member.reference = 0;
        llcVoltLoop.member.feedback  = 0;
        llcVoltLoop.member.error     = 0;
        llcVoltLoop.member.output    = 0;

        // High stop filter parameters
        llcVoltLoop.member.hsFilter.un  = 0.0f;
        llcVoltLoop.member.hsFilter.un1 = 0.0f;
        llcVoltLoop.member.hsFilter.un2 = 0.0f;
        llcVoltLoop.member.hsFilter.yn  = 0.0f;
        llcVoltLoop.member.hsFilter.yn1 = 0.0f;
        llcVoltLoop.member.hsFilter.yn2 = 0.0f;

        // PI compensation parameters
        llcVoltLoop.member.piControl.error    = 0.0f;
        llcVoltLoop.member.piControl.pYn      = 0.0f;
        llcVoltLoop.member.piControl.iYn      = 0.0f;
        llcVoltLoop.member.piControl.iYn1     = 0.0f;
        llcVoltLoop.member.piControl.piYn     = 0.0f;
        llcVoltLoop.member.piControl.satError = 0.0f;
        llcVoltLoop.member.piControl.output   = 0.0f;

        // Integer parameters
        llcCurrLoop.member.reference = 0;
        llcCurrLoop.member.feedback  = 0;
        llcCurrLoop.member.error     = 0;
        llcCurrLoop.member.output    = 0;

        // High stop filter parameters
        llcCurrLoop.member.hsFilter.un  = 0.0f;
        llcCurrLoop.member.hsFilter.un1 = 0.0f;
        llcCurrLoop.member.hsFilter.un2 = 0.0f;
        llcCurrLoop.member.hsFilter.yn  = 0.0f;
        llcCurrLoop.member.hsFilter.yn1 = 0.0f;
        llcCurrLoop.member.hsFilter.yn2 = 0.0f;

        // PI compensation parameters
        llcCurrLoop.member.piControl.error    = 0.0f;
        llcCurrLoop.member.piControl.pYn      = 0.0f;
        llcCurrLoop.member.piControl.iYn      = 0.0f;
        llcCurrLoop.member.piControl.iYn1     = 0.0f;
        llcCurrLoop.member.piControl.piYn     = 0.0f;
        llcCurrLoop.member.piControl.satError = 0.0f;
        llcCurrLoop.member.piControl.output   = 0.0f;

        // 1.5 4 3 12.5 5181
        llcVoltParam.b0 = (long)(1.313f * 16384.0f); //__IQ(0.115, 14);
        llcVoltParam.b1 = (long)(-0.559f * 16384.0f); //__IQ(-0.093, 14);
        llcVoltParam.b2 = (long)(-1.222f * 16384.0f); //__IQ(-0.113, 14);
        llcVoltParam.b3 = (long)(0.65f * 16384.0f); //__IQ(0.094, 14);
        llcVoltParam.a1 = (long)(-1.803f * 16384.0f); //__IQ(-2.845, 14);
        llcVoltParam.a2 = (long)(0.885f * 16384.0f); //__IQ(2.694, 14);
        llcVoltParam.a3 = (long)(-0.082f * 16384.0f); //__IQ(-0.849, 14);

        llcCurrParam.reference = 0;
        llcCurrParam.feedback  = 0;
        llcCurrParam.output    = 0;
        llcCurrParam.maximum   = MAX_SWITCH_PERIOD;
        llcCurrParam.minimum   = MIN_SWITCH_PERIOD;
        llcCurrParam.b0        = __IQ(0.04, 14);
        llcCurrParam.b1        = __IQ(0.0002972, 14);
        llcCurrParam.b2        = __IQ(-0.039, 14);
        llcCurrParam.b3        = __IQ(0, 14);
        llcCurrParam.a1        = __IQ(-1.882, 14);
        llcCurrParam.a2        = __IQ(0.882, 14);
        llcCurrParam.a3        = __IQ(0, 14);

        llcCurrShareParam.reference = 0;
        llcCurrShareParam.feedback  = 0;
        llcCurrShareParam.output    = 0;
        llcCurrShareParam.maximum   = (OUTPUT_VOLT_BUS(500) * __IQ(0.01, 14)) >> 14;
        llcCurrShareParam.minimum   = (OUTPUT_VOLT_BUS(-500) * __IQ(0.01, 14)) >> 14;
        llcCurrShareParam.b0        = __IQ(0.04, 14);
        llcCurrShareParam.b1        = __IQ(0.0002972, 14);
        llcCurrShareParam.b2        = __IQ(-0.039, 14);
        llcCurrShareParam.b3        = __IQ(0, 14);
        llcCurrShareParam.a1        = __IQ(-1.882, 14);
        llcCurrShareParam.a2        = __IQ(0.882, 14);
        llcCurrShareParam.a3        = __IQ(0, 14);

        llcVoltDroopParam.reference = 0;
        llcVoltDroopParam.feedback  = 0;
        llcVoltDroopParam.output    = 0;
        llcVoltDroopParam.maximum   = (OUTPUT_VOLT_BUS(600) * __IQ(0.01, 14)) >> 14;
        llcVoltDroopParam.minimum   = (OUTPUT_VOLT_BUS(-1200) * __IQ(0.01, 14)) >> 14;

        sohCurrLoop.kp       = Default_SOH_I_PI_KP;
        sohCurrLoop.ki       = Default_SOH_I_PI_KI;
        sohCurrLoop.kc       = Default_SOH_I_PI_KC;
        sohCurrLoop.ka       = Default_SOH_I_PI_KA;
        sohCurrLoop.max      = Default_SOH_I_PI_Limit;
        sohCurrLoop.min      = 0.0f;
        sohCurrLoop.error    = 0.0f;
        sohCurrLoop.pYn      = 0.0f;
        sohCurrLoop.iYn      = 0.0f;
        sohCurrLoop.iYn1     = 0.0f;
        sohCurrLoop.piYn     = 0.0f;
        sohCurrLoop.satError = 0.0f;
        sohCurrLoop.output   = 0.0f;

        llcSohOutParam.reference = 0;
        llcSohOutParam.feedback  = 0;
        llcSohOutParam.output    = 0;
        llcSohOutParam.maximum   = (OUTPUT_VOLT_BUS(600) * __IQ(0.01, 14)) >> 14;
        llcSohOutParam.minimum   = (OUTPUT_VOLT_BUS(-600) * __IQ(0.01, 14)) >> 14;

        // Initialize variables
        openFlag                      = 0;
        cpuLlcState.bit.enableDrive   = 0;
        cpuLlcState.bit.freqSoftStart = 0;
        voltSetpointModify            = true;
        voltSoftStartRef              = OUTPUT_VOLT_BUS(avgInnerVolt.val); //OUTPUT_VOLT_BUS(20.0);
        voltSoftStartFlag             = true;
        currSetpointModify            = false;
        currSoftStartRef              = 0;
        currSoftStartFlag             = false;
        period                        = MIN_SWITCH_PERIOD;
        priDutyLead                   = (period * 5461) >> 15;
        priDutyLag                    = period - priDutyLead;
        priDeadband                   = SWITCH_DB(0.25e-6f); //(0.3e-6f);
        srDutyLead                    = (period * 5461) >> 15;
        srDutyLag                     = period - srDutyLead;
        srDeadband                    = SWITCH_DB(0.3e-6f);   //(1.1e-6f);
        SCP_Cnt                       = 0;

        softstartFlag = 0;
        softstartDone = 1;

        Llc_Pwm_Update();

        deltaPeriod = 0;
        sotcState = SOTC_DISABLE;

        burstThreshold = (OUTPUT_VOLT_BUS(-330) * __IQ(0.01, 14)) >> 14;
        cpuLlcState.bit.burst = 0;

        switch(llcMode)
        {
        default:
        case LLC_OFF_MODE:
            Llc_Sr_Pwm_Off();
            break;
        case LLC_ON_MODE:
            Llc_Pwm_On();
            break;
        }
        modifyLlcMode = false;
    }
    return llcMode;
}
/***************************************************************************************************
Function Name:
    void Llc_Sr_Pwm_Off(void)
Input:
    NULL
Output:
    NULL
Comment:
    Force ePWM1, ePWM2, ePWM4, ePWM5, ePWM7 and ePWM8 module output low
***************************************************************************************************/
void Llc_Sr_Pwm_Off(void)
{
    cpuLlcState.bit.enableLlc = 0;
    EPwm1Regs.AQCSFRC.bit.CSFA = AQ_CLEAR;
    EPwm1Regs.AQCSFRC.bit.CSFB = AQ_SET;
    EPwm8Regs.AQCSFRC.bit.CSFA = AQ_CLEAR;
    EPwm8Regs.AQCSFRC.bit.CSFB = AQ_SET;
    EPwm4Regs.AQCSFRC.bit.CSFA = AQ_CLEAR;
    EPwm4Regs.AQCSFRC.bit.CSFB = AQ_SET;
    EPwm2Regs.AQCSFRC.bit.CSFA = AQ_CLEAR;
    EPwm2Regs.AQCSFRC.bit.CSFB = AQ_SET;
    EPwm7Regs.AQCSFRC.bit.CSFA = AQ_CLEAR;
    EPwm7Regs.AQCSFRC.bit.CSFB = AQ_SET;
    EPwm5Regs.AQCSFRC.bit.CSFA = AQ_CLEAR;
    EPwm5Regs.AQCSFRC.bit.CSFB = AQ_SET;
    EPwm1Regs.GLDCTL2.bit.OSHTLD = 1;
}
/***************************************************************************************************
Function Name:
    void Llc_Pwm_On(void)
Input:
    NULL
Output:
    NULL
Comment:
    Enable ePWM1, ePWM4 and ePWM8 module output
***************************************************************************************************/
void Llc_Pwm_On(void)
{
    cpuLlcState.bit.enableLlc = 1;
    EPwm1Regs.AQCSFRC.bit.CSFA = AQ_NO_ACTION;
    EPwm1Regs.AQCSFRC.bit.CSFB = AQ_NO_ACTION;
    EPwm8Regs.AQCSFRC.bit.CSFA = AQ_NO_ACTION;
    EPwm8Regs.AQCSFRC.bit.CSFB = AQ_NO_ACTION;
    EPwm4Regs.AQCSFRC.bit.CSFA = AQ_NO_ACTION;
    EPwm4Regs.AQCSFRC.bit.CSFB = AQ_NO_ACTION;
    EPwm1Regs.GLDCTL2.bit.OSHTLD = 1;
}
/***************************************************************************************************
Function Name:
    void Llc_Pwm_On(void)
Input:
    NULL
Output:
    NULL
Comment:
    Enable ePWM1, ePWM2, ePWM4, ePWM5, ePWM7 and ePWM8 module output
***************************************************************************************************/
void Llc_Sr_Pwm_On(void)
{
    cpuLlcState.bit.enableLlc = 1;
    EPwm1Regs.AQCSFRC.bit.CSFA = AQ_NO_ACTION;
    EPwm1Regs.AQCSFRC.bit.CSFB = AQ_NO_ACTION;
    EPwm8Regs.AQCSFRC.bit.CSFA = AQ_NO_ACTION;
    EPwm8Regs.AQCSFRC.bit.CSFB = AQ_NO_ACTION;
    EPwm4Regs.AQCSFRC.bit.CSFA = AQ_NO_ACTION;
    EPwm4Regs.AQCSFRC.bit.CSFB = AQ_NO_ACTION;
    EPwm2Regs.AQCSFRC.bit.CSFA = AQ_NO_ACTION;
    EPwm2Regs.AQCSFRC.bit.CSFB = AQ_NO_ACTION;
    EPwm7Regs.AQCSFRC.bit.CSFA = AQ_NO_ACTION;
    EPwm7Regs.AQCSFRC.bit.CSFB = AQ_NO_ACTION;
    EPwm5Regs.AQCSFRC.bit.CSFA = AQ_NO_ACTION;
    EPwm5Regs.AQCSFRC.bit.CSFB = AQ_NO_ACTION;
    EPwm1Regs.GLDCTL2.bit.OSHTLD = 1;
}
/***************************************************************************************************
Function Name:
    void Llc_Pwm_Update(void)
Input:
    period              - PWM period
    priDutyLead         - Interleaving high side primary falling edge position
    priDutyLag          - Interleaving low side primary falling edge position
    priDeadband         - Primary PWM dead band
    srDutyLead          - Interleaving high side SR falling edge position
    srDutyLag           - Interleaving low side SR falling edge position
    srDeadband          - SR :PWM dead band
Output:
    N/A
Comment:
    Update ePWM module SFR
***************************************************************************************************/
void Llc_Pwm_Update(void)
{
    EPwm1Regs.TBPRD = period;
    EPwm8Regs.TBPRD = period;
    EPwm4Regs.TBPRD = period;
    EPwm7Regs.TBPRD = period;
    EPwm5Regs.TBPRD = period;
    EPwm2Regs.TBPRD = period;

    EPwm1Regs.CMPA.bit.CMPA = priDutyLead;
    EPwm1Regs.CMPB.bit.CMPB = period - priDutyLead;
    EPwm8Regs.CMPA.bit.CMPA = priDutyLag;
    EPwm8Regs.CMPB.bit.CMPB = period - priDutyLag;
    EPwm4Regs.CMPA.bit.CMPA = period >> 1;
    EPwm4Regs.CMPB.bit.CMPB = period >> 1;
    EPwm2Regs.CMPA.bit.CMPA = srDutyLead;
    EPwm2Regs.CMPB.bit.CMPB = period - srDutyLead;
    EPwm7Regs.CMPA.bit.CMPA = srDutyLag;
    EPwm7Regs.CMPB.bit.CMPB = period - srDutyLag;
    EPwm5Regs.CMPA.bit.CMPA = period >> 1;
    EPwm5Regs.CMPB.bit.CMPB = period >> 1;

    EPwm1Regs.DBRED.bit.DBRED = priDeadband;
    EPwm1Regs.DBFED.bit.DBFED = priDeadband;
    EPwm8Regs.DBRED.bit.DBRED = priDeadband;
    EPwm8Regs.DBFED.bit.DBFED = priDeadband;
    EPwm4Regs.DBRED.bit.DBRED = priDeadband;
    EPwm4Regs.DBFED.bit.DBFED = priDeadband;
    EPwm2Regs.DBRED.bit.DBRED = srDeadband;
    EPwm2Regs.DBFED.bit.DBFED = srDeadband;
    EPwm7Regs.DBRED.bit.DBRED = srDeadband;
    EPwm7Regs.DBFED.bit.DBFED = srDeadband;
    EPwm5Regs.DBRED.bit.DBRED = srDeadband;
    EPwm5Regs.DBFED.bit.DBFED = srDeadband;

    EPwm1Regs.GLDCTL2.bit.OSHTLD = 1;
}
#if RIPPLE_FEEDFORWARD
/***************************************************************************************************
Function Name:
    void Voltage_Feedforward(void)
Input:
    adcOutputVolt       - Output voltage
Output:
    addendFeedforward   - Value add to controller
Comment:
    Output ripple cancellation function.
    Grab the output voltage and use a band-pass filter to get 100Hz wave.
    Use the wave as an index of inverse sine wave table.
    Detect the phase then plus into the controller output with the same phase.
***************************************************************************************************/
void Voltage_Feedforward(void)
{
    mafVoutSum = mafVoutSum - (mafVoutSum >> mafVoutShift) + adcOutputVolt;
    mafVoutAvgSum = mafVoutAvgSum - (mafVoutAvgSum >> mafVoutAvgShift) + adcOutputVolt;
    // Enable after soft start, TBD
    if(!voltSoftStartFlag)
    {
        filterVout = ((mafVoutSum >> mafVoutShift) - (mafVoutAvgSum >> mafVoutAvgShift));

        if(ripplePolarity < 2048)
        {
            if(indexFeedforward <= 0)
            {
                indexFeedforward = 0;
                rippleAntiStuckCnt += 1;
            }
            else
            {
                indexFeedforward -= 1;
            }

//            if(filterVout > 0 && indexFeedforward < ISR_CNT(0.3e-3f) || rippleAntiStuckCnt > ISR_CNT(0.5e-3f))
//            if(filterVout > 0 && ++rippleDelayCnt >= 40 && indexFeedforward < 15 || rippleAntiStuckCnt > 20)
            if(filterVout > 0 && ++rippleDelayCnt >= ISR_CNT(2e-3f) && indexFeedforward < 15 || rippleAntiStuckCnt > 20)
            {
                ripplePolarity = 3072;
                rippleDelayCnt = 0;
                indexFeedforward = 0;
                rippleAntiStuckCnt = 0;
            }
        }
        else if(ripplePolarity > 2048)
        {
            if(indexFeedforward >= indexFeedforwardMax)
            {
                indexFeedforward = indexFeedforwardMax;
                rippleAntiStuckCnt += 1;
            }
            else
            {
                indexFeedforward += 1;
            }

//            if(filterVout < 0 && indexFeedforward > (indexFeedforwardMax - ISR_CNT(0.3e-3f)) || rippleAntiStuckCnt > ISR_CNT(0.5e-3f))
//            if(filterVout < 0 && ++rippleDelayCnt >= 40 && indexFeedforward > (indexFeedforwardMax - 15) || rippleAntiStuckCnt > 20)
            if(filterVout < 0 && ++rippleDelayCnt >= ISR_CNT(2e-3f) && indexFeedforward > (indexFeedforwardMax - 15) || rippleAntiStuckCnt > 20)
            {
                ripplePolarity = 1024;
                rippleDelayCnt = 0;
                indexFeedforward = indexFeedforwardMax;
                rippleAntiStuckCnt = 0;
            }
        }
        // Enable condition, TBD
        if(llcVoltParam.En[0] < -57 || adcOutput54vCurr < 200)
        {
            addendFeedforward = 0;
            decayFeedforward = 0;
        }
        else if(adcOutput54vCurr > 250)    // 10A start feedforweard
        {
            addendFeedforward = (FEEDFORWARD_SIN_TABLE[indexFeedforward] * decayFeedforward) >> 10;
            if(--cntDecayFeedforward <= 0)
            {
                if (++decayFeedforward >= 1024)
                {
                    decayFeedforward = 1024;
                }
                cntDecayFeedforward = ISR_CNT(5.0e-3f);
            }
        }
    }
    else
    {
        rippleDelayCnt = 0;
        rippleAntiStuckCnt = 0;
        indexFeedforward = 0;
        decayFeedforward = 0;
        addendFeedforward = 0;
    }
}
#endif

__attribute__((ramfunc))
void SOTC3(void)
{
    static float fBuff;
    static unsigned long ulCount = 0, ulCount1 = 0, ulCount12 = 0;
    static unsigned char extendFlag = 0, extendFlag2 = 0;
    static unsigned long ulCount2 = 0;
    static unsigned char constFlag = 0, constFlag2 = 0;

    adcOutputCurr = adcCurrShareVolt;

    sotcCurrentLog[2] = sotcCurrentLog[1];
    sotcCurrentLog[1] = sotcCurrentLog[0];
    sotcCurrentLog[0] = adcOutputCurr >> 2; //Q10


    sotcFiltCurrentLog[2] = sotcFiltCurrentLog[1];
    sotcFiltCurrentLog[1] = sotcFiltCurrentLog[0];
    sotcFiltCurrentLog[0] = adcOutputCurr << 2;
    sotcFiltError = sotcFiltCurrentLog[0] - sotcFiltCurrentLog[2];
    //sotcFiltError = sotcFiltError + csErorr;

        if (ulCount1 > 60) //1200u
        {
            constFlag = 0;
        }
        else if (ulCount1 > 19)
        {
            constFlag = 1;
            ulCount1++;
        }
        else
            ulCount1++;

        if (ulCount12 > 60) //1200u
        {
            constFlag2 = 0;
        }
        else if (ulCount12 > 19)
        {
            constFlag2 = 1;
            ulCount12++;
        }
        else
            ulCount12++;

    periodBuff = adcOutputCurr;
    periodBuff = (periodBuff * 2118) >> 15;//2198) >> 15;

    if (sotcFiltError > 0) //up
    {
        if(constFlag == 2)
        {
            if (ulCount12 < 6)
                periodBuff = periodBuff + 265;
            else
                periodBuff = 400;
        }
        else if(constFlag == 1)
        {
            periodBuff = periodBuff + 280;
        }
        else
        {
            periodBuff = periodBuff + 265;
        }
    }
    else
    {
        if(constFlag2 == 2)
        {
            if (ulCount1 < 6)
            {
                periodBuff = periodBuff + 265;
            }
            else
            {
                periodBuff = 312; //periodBuff + 285;//+ 265;
            }
        }
        else if(constFlag2 == 1)
        {
            periodBuff = periodBuff + 275;//+ 265;
        }
        else
        {
            if (ulCount1 < 30)
            {
                periodBuff = periodBuff + 265;
            }
            else
            {
                periodBuff = periodBuff + 320;
            }
        }
    }

    if(sotcFiltError > OUTPUT_CURR(25.0))
        ;
    else if(sotcFiltError < OUTPUT_CURR(-25.0))
        ;
    else if(sotcFiltError > OUTPUT_CURR(12.0))
        periodBuff = periodBuff - 20;
    else if(sotcFiltError < OUTPUT_CURR(-12.0))
        ;


    if (adcOutputCurr < 268)//19A//278)//< 268) //18A
    {
        periodBuff = 180;//303; //165k
    }

    if (periodBuff > 417)
        periodBuff = 417;

    if (periodBuff > 341)
        fBuff = (float)periodBuff * 13.47 - 1522.52;
    else if (periodBuff > 289)
        fBuff = (float)periodBuff * 19.69 - 3643.0;
    else if (periodBuff > 250)
        fBuff = (float)periodBuff * 26.25 - 5540.0;
    else if (periodBuff > 192)
        fBuff = (float)periodBuff * 8.83 - 1182.89;
    else
    {
        fBuff = 256.0;
    }

    fBuff = fBuff * 4;

    if(dcdcState.bit.dischargerOK == 1 && llcState != LLC_CC )
    {
        if(sotcState == SOTC_LATCH || sotcState == SOTC_EXTEND)
        {
            deltaPeriod = 0;
            ulCount = 0;

            if (extendFlag < 1)
            {
                llcVoltParam.Yn[1] = __IQsat((long)fBuff, 16384, 0);
                fTest = (float)llcVoltParam.Yn[1];
                fTest = fTest * Default_Type_III_PI_Limit;
                llcVoltLoop.member.piControl.iYn1 = fTest * 6.1e-5;
                extendFlag++;
                constFlag = 2;
                punchUp = 0;
            }

            if (sotcFiltError > OUTPUT_CURR(12.0))
            {
                sotcLatchCnt = ISR_CNT(200e-6f);
                sotcState = SOTC_EXTEND;
            }
            else if(--sotcLatchCnt <= 0)
            {
                sotcState = SOTC_STANDBY;
                extendFlag = 0;
            }
        }
        else if(sotcFiltError > OUTPUT_CURR(12.0))
        {

            if((constFlag == 1) && (adcOutputCurr > UP_CURR) && (punchUp == 0))// 55A //1042)//70A 893)//60A
            {
                extendFlag = 1;
            }
            else
            {
                punchUp++;

                llcVoltParam.Yn[1] = __IQsat((long)fBuff, 16384, 0);
                fTest = (float)llcVoltParam.Yn[1];
                fTest = fTest * Default_Type_III_PI_Limit;
                llcVoltLoop.member.piControl.iYn1 = fTest * 6.1e-5;
            }

            sotcState = SOTC_STEP_UP;
            llcState = LLC_SOTC;
            ulCount++;

            ulCount1 = 0;
        }
        else if (ulCount > 0)
        {

            sotcState = SOTC_LATCH;
            sotcLatchCnt = ISR_CNT(200e-6f);
        }
        else
        {
            ulCount = 0;
            deltaPeriod = 0;
            extendFlag = 0;
            sotcState = SOTC_STANDBY;
            llcState = LLC_SOTC;
        }


        if (ulCount == 2)
        {
            sotcState = SOTC_LATCH;
            sotcLatchCnt = ISR_CNT(200e-6f);
        }

        if(sotcState2 == SOTC_LATCH || sotcState2 == SOTC_EXTEND)
        {
            deltaPeriod = 0;
            ulCount2 = 0;

            if (extendFlag2 < 1)
            {
                llcVoltParam.Yn[1] = __IQsat((long)fBuff, 16384, 0);
                fTest = (float)llcVoltParam.Yn[1];
                fTest = fTest * Default_Type_III_PI_Limit;
                llcVoltLoop.member.piControl.iYn1 = fTest * 6.1e-5;
                extendFlag2++;
                constFlag2 = 2;
                punchDown = 0;

            }

            if (sotcFiltError < OUTPUT_CURR(-12.0))
            {
                sotcLatchCnt2 = ISR_CNT(200e-6f);
                sotcState2 = SOTC_EXTEND;
            }
            else if(--sotcLatchCnt2 <= 0)
            {
                sotcState2 = SOTC_STANDBY;
                extendFlag2 = 0;
            }
        }
        else if(sotcFiltError < OUTPUT_CURR(-12.0))//&& skipFlag == 0)
        {

            if(constFlag2 == 1 && adcOutputCurr > 270)//450)//DOWN_CURR)//15A // 595) //40A
            {
                extendFlag2 = 1;
            }
            else
            {

                punchDown++;
                llcVoltParam.Yn[1] = __IQsat((long)fBuff, 16384, 0);
                fTest = (float)llcVoltParam.Yn[1];
                fTest = fTest * Default_Type_III_PI_Limit;
                llcVoltLoop.member.piControl.iYn1 = fTest * 6.1e-5;
            }

            sotcState2 = SOTC_STEP_DOWN;
            llcState = LLC_SOTC;
            ulCount2++;

            ulCount12 = 0;
        }
        else if (ulCount2 > 0)
        {

            sotcState2 = SOTC_LATCH;
            sotcLatchCnt2 = ISR_CNT(200e-6f);
        }
        else
        {
            ulCount2 = 0;
            deltaPeriod = 0;
            sotcState2 = SOTC_STANDBY;
            llcState = LLC_SOTC;
        }

        if (ulCount2 == 2)
        {
            sotcState2 = SOTC_LATCH;
            sotcLatchCnt2 = ISR_CNT(200e-6f);
        }

    }
    else
    {
        deltaPeriod = 0;
        ulCount = 0;
        ulCount2 = 0;
        sotcState = SOTC_STANDBY;
    }
}

//------------------------------------------------------------------------------
// Global functions
//------------------------------------------------------------------------------
/***************************************************************************************************
Function Name:
    void LlcController(void)
Input:
    adcOutputVolt       - Output voltage ADC value
    adcOutput54vCurr    - Output current ADC value
Output:
    NULL
Comment:
    LLC controller
***************************************************************************************************/
__attribute__((ramfunc))
void LlcController(void)
{
    cpuLlcState.bit.softStart = voltSoftStartFlag;
    // Update Messages from CLA
    CPU_LLC_State = cpuLlcState.all;

    switch(Switch_Llc_Mode())
    {
    default:
    case LLC_OFF_MODE:

        llcState = LLC_HALT;
        break;

    case LLC_ON_MODE:

        Voltage_Droop();

        if (sohOutFlag == 1)
        {
            Soh_Test_Voltage_Control();
        }

        if (!softstartDone && sohOutFlag == 1)
            sTemp = voltSetpoint;// + sohTestVolRefOffset;
        else if (!softstartDone)
            sTemp = voltSetpoint;// + droopVoltRefOffset + csVoltRefOffset;
        else
            sTemp = voltSoftStartRef;

        if (sTemp < 0)
            sTemp = 0;
        else if (sTemp > 13226) //New add 450V 15605 //800V 13226
            sTemp = 13226;

        llcVoltParam.reference = (long)sTemp;

        if(!softstartDone)
            llcVoltParam.feedback = (long)avgOutputVolt.cma << 2; //Control the bus voltage after oring on
        else
            llcVoltParam.feedback = (long)avgInnerVolt.cma << 2; //Control the bus voltage after oring on

        llcVoltParam.En[0] = llcVoltParam.reference - llcVoltParam.feedback;    // Q14

        // TO CLA
        CPU_Vout_Ref = llcVoltParam.reference;
        CPU_Vout_Ref_CONSTAT = (long)(voltSoftStartRef + 114);
        CPU_LLC_Vout = llcVoltParam.feedback;

        //FROM CLA
        if(llcVoltParam.feedback > OUTPUT_VOLT_BUS(0.0))    //OUTPUT_VOLT_BUS(20.0)
        {
            openFlag = 1;
            cpuLlcState.bit.enableDrive = 1;
            cpuLlcState.bit.freqSoftStart = 1;
        }

        if (highLimitFreq)
        {
            cpuLlcState.bit.highLimitFreq = 1;
            period = llcCurrParam.minimum;
        }
        else
            cpuLlcState.bit.highLimitFreq = 0;

        // Cmpss1Regs.COMPSTS.bit.COMPHLATCH -> Resonant Current
        if(Cmpss1Regs.COMPSTS.bit.COMPHLATCH)
        {
            if(!warningCode.status.bit.PRI_OCP_DISCHG_IO)
            {
                if(++PRI_OCP_Cnt >= 3)
                {
                    PRI_OCP_Cnt = 0;
                    Set_Llc_Mode(LLC_OFF_MODE);
                    LLC_Driver_Disable();
                    SR_Driver_Disable();
                    Oring_Off();
                    IBUS_Disable();
                    warningCode.status.bit.PRI_OCP_DISCHG_IO = 1;
                }
            }
            EALLOW;
            Cmpss1Regs.COMPSTSCLR.bit.HLATCHCLR = 1; // Clear the latch status
            EDIS;
        }
        else
        {
            PRI_OCP_Cnt = 0;
            EALLOW;
            Cmpss1Regs.COMPSTSCLR.bit.HLATCHCLR = 1;
            EDIS;
        }

        // Cmpss2Regs.COMPSTS.bit.COMPHLATCH -> Output Current
        if(Cmpss2Regs.COMPSTS.bit.COMPHLATCH)
        {
            if(!warningCode.flag.bit.iOutScFault4 && (llcVoltParam.feedback < OUTPUT_VOLT_BUS(40)))
            {
                if(++SCP_Cnt >= 2)
                {
                    SCP_Cnt = 0;
                    Set_Llc_Mode(LLC_OFF_MODE);
                    LLC_Driver_Disable();
                    SR_Driver_Disable();
                    Oring_Off();
                    IBUS_Disable();
                    warningCode.flag.bit.iOutScFault4 = 1;
                }
            }
            EALLOW;
            Cmpss2Regs.COMPSTSCLR.bit.HLATCHCLR = 1;
            EDIS;
        }
        else
        {
            SCP_Cnt = 0;
            EALLOW;
            Cmpss2Regs.COMPSTSCLR.bit.HLATCHCLR = 1;
            EDIS;
        }

        llcCurrParam.feedback = (long)avgOutputCurr.cma << 2;//avgOutput54vCurr.cma << 2;

        // Burst mode
        if(llcCurrParam.feedback > OUTPUTDIS_CURR(10.0))
        {
            burstThreshold = (OUTPUT_VOLT_BUS(-2000) * __IQ(0.01, 14)) >> 14;
        }
        else if(llcCurrParam.feedback < OUTPUTDIS_CURR(5.0))
        {
            burstThreshold = (OUTPUT_VOLT_BUS(-200) * __IQ(0.01, 14)) >> 14;
        }
        else
        {
            burstThreshold = (OUTPUT_VOLT_BUS(-1000) * __IQ(0.01, 14)) >> 14;
        }

        if(llcVoltParam.En[0] < burstThreshold)
        {
            cpuLlcState.bit.burst = 1;
        }
        else
            cpuLlcState.bit.burst = 0;

        break;
    }
}
/***************************************************************************************************
Function Name:
    void Set_Llc_Mode(LlcCtrlMode mode)
Input:
    mode
Output:
    NULL
Comment:
    LLC mode change
***************************************************************************************************/
void Set_Llc_Mode(LlcCtrlMode mode)
{
    if(llcMode != mode)
    {
        llcMode = mode;
        modifyLlcMode = true;
    }
}
/***************************************************************************************************
Function Name:
    void Sr_Pwm_Off(void)
Input:
    NULL
Output:
    NULL
Comment:
    Force SR PWM output low
***************************************************************************************************/
void Sr_Pwm_Off(void)
{
    EPwm2Regs.AQCSFRC.bit.CSFA = AQ_CLEAR;
    EPwm2Regs.AQCSFRC.bit.CSFB = AQ_SET;
    EPwm7Regs.AQCSFRC.bit.CSFA = AQ_CLEAR;
    EPwm7Regs.AQCSFRC.bit.CSFB = AQ_SET;
    EPwm5Regs.AQCSFRC.bit.CSFA = AQ_CLEAR;
    EPwm5Regs.AQCSFRC.bit.CSFB = AQ_SET;
    EPwm1Regs.GLDCTL2.bit.OSHTLD = 1;
}
/***************************************************************************************************
Function Name:
    void Sr_Pwm_On(void)
Input:
    NULL
Output:
    NULL
Comment:
    Enable SR PWM with initial statement
***************************************************************************************************/
void Sr_Pwm_On(void)
{
    srDeadband = 0xFFFF;//0x3FFF;
    EPwm2Regs.AQCSFRC.bit.CSFA = AQ_NO_ACTION;
    EPwm2Regs.AQCSFRC.bit.CSFB = AQ_NO_ACTION;
    EPwm7Regs.AQCSFRC.bit.CSFA = AQ_NO_ACTION;
    EPwm7Regs.AQCSFRC.bit.CSFB = AQ_NO_ACTION;
    EPwm5Regs.AQCSFRC.bit.CSFA = AQ_NO_ACTION;
    EPwm5Regs.AQCSFRC.bit.CSFB = AQ_NO_ACTION;
    EPwm1Regs.GLDCTL2.bit.OSHTLD = 1;
}
/***************************************************************************************************
Function Name:
    unsigned short Get_Llc_State(void)
Input:
    NULL
Output:
    NULL
Comment:
    Return current LLC status
***************************************************************************************************/
unsigned short Get_Llc_State(void)
{
    unsigned short state;

    state = llcState;

    return state;
}
/**************************************************************************************************
Function Name:
    void Set_Voltage_Setpoint(unsigned long volt)
Input:
    volt        - Output voltage setpoint x 100.
Output:

Comment:
    Set output voltage setpoint and calculate soft-start step.
    Whole function take about 800ns.
**************************************************************************************************/
void Set_Voltage_Setpoint(unsigned long volt)
{
    unsigned long dividend;
    unsigned long divisor;
    unsigned long remainder;

    volt = __IQsat(volt, 9910, 0);//991V
    voltSetpoint = ((OUTPUT_VOLT_BUS(volt)) * __IQ(0.1, 14)) >> 14;

    voltSoftStartSetpoint1 = ((voltSetpoint * __IQ(0.9, 14)) >> 14);
    voltSoftStartSetpoint2 = ((voltSetpoint * __IQ(0.1, 14)) >> 14);
    dividend = voltSoftStartSetpoint1;
    divisor = (long)(25); //0.5*50 //BBU 0.5ms in 50kHz ISR
    remainder = 0;
    voltSoftStartStep1 = __rpt_subcul(dividend, divisor, remainder, 31);
    voltSoftStartStep1 = __IQsat(voltSoftStartStep1, voltSoftStartStep1, 1);
    dividend = voltSoftStartSetpoint2;
    divisor = (long)(75); //1.5*50 //BBU 1.5ms in 50kHz ISR
    remainder = 0;
    voltSoftStartStep2 = __rpt_subcul(dividend, divisor, remainder, 31);
    voltSoftStartStep2 = __IQsat(voltSoftStartStep2, voltSoftStartStep2, 1);

    voltSetpointModify = true;
}
/***********************************************************************************************************************
Function Name:
    void Soft_Start_Routine(void)
Input:
    N/A
Output:
    Return      - Soft-start state.
                    1 = soft-start in progress
                    0 = soft-start complete
Comment:
    LLC soft start routine
***********************************************************************************************************************/
unsigned short Soft_Start_Routine(void)
{
    if (openFlag == 1)
    {
        if (voltSetpointModify)
        {
            if (voltSoftStartRef  == voltSetpoint)
            {
                voltSoftStartRef = voltSetpoint;
                voltSoftStartFlag = false;
                voltSetpointModify = false;
            }
            else if (voltSoftStartRef < voltSetpoint)
            {
                if(voltSoftStartRef < voltSoftStartSetpoint1)
                {
                    voltSoftStartRef += voltSoftStartStep1;
                    voltSoftStartFlag = true;
                }
                else
                {
                    voltSoftStartRef += voltSoftStartStep2;
                    voltSoftStartFlag = true;
                }
                if(voltSoftStartRef > voltSetpoint)
                {
                    voltSoftStartRef = voltSetpoint;
                    voltSoftStartFlag = false;
                    voltSetpointModify = false;
                }
            }
            else
            {
                if (voltSoftStartRef > voltSoftStartSetpoint1)
                {
                    voltSoftStartRef -= voltSoftStartStep2;
                    voltSoftStartFlag = true;
                }
                else
                {
                    voltSoftStartRef -= voltSoftStartStep1;
                    voltSoftStartFlag = true;
                }
                if (voltSoftStartRef < voltSetpoint)
                {
                    voltSoftStartRef = voltSetpoint;
                    voltSoftStartFlag = false;
                    voltSetpointModify = false;
                }
            }
        }
        else
        {
            voltSoftStartRef = voltSetpoint;
        }

}
    return voltSoftStartFlag;
}
/**************************************************************************************************
Function Name:
    void Set_Current_Setpoint(unsigned long curr)
Input:
    curr        - Output current setpoint x 100.
Output:

Comment:
    Set output current setpoint.
**************************************************************************************************/
void Set_Current_Setpoint(unsigned long curr)
{
    unsigned long dividend;
    unsigned long divisor;
    unsigned long remainder;

    curr = __IQsat(curr, 200, 0);
    currSetpoint = ((OUTPUT_CURR(curr)) * __IQ(0.01, 14)) >> 14;

    // 1ms increase 1%
    dividend = currSetpoint;
    divisor = 10;
    remainder = 0;
    currSoftStartStep = __rpt_subcul(dividend, divisor, remainder, 31);

    currSetpointModify = true;
}
/***********************************************************************************************************************
Function Name:
    void Soft_Start_Routine_Curr(void)
Input:
    N/A
Output:
    Return      - Soft-start state.
                    1 = soft-start in progress
                    0 = soft-start complete
Comment:
    LLC soft start routine
***********************************************************************************************************************/
unsigned short Soft_Start_Routine_Curr(void)
{
    if (currSetpointModify)
    {
        if(currSoftStartRef < currSetpoint)
        {
            currSoftStartRef += currSoftStartStep;
            currSoftStartFlag = true;

            if (currSoftStartRef > currSetpoint)
            {
                currSoftStartRef = currSetpoint;
                currSoftStartFlag = false;
                currSetpointModify = false;
            }
        }
        else
        {
            currSoftStartRef -= currSoftStartStep;
            currSoftStartFlag = true;

            if (currSoftStartRef < currSetpoint)
            {
                currSoftStartRef = currSetpoint;
                currSoftStartFlag = false;
                currSetpointModify = false;
            }
        }
    }
    else
    {
        currSoftStartRef = currSetpoint;
    }

    return currSoftStartFlag;
}
/***********************************************************************************************************************
Function Name:
    void Current_Sharing(void)
Input:
    avgCurrShareVolt - Vcs ADC value
    avgOutputCurr - Output current ADC value
Output:
    csVoltRefOffset - Value add to voltage reference
Comment:
    Use adcCurrShareVolt as the reference and avgOutputCurr as the feedback.
    Output a offset to change voltage reference.
***********************************************************************************************************************/
void Current_Sharing(void)
{
    llcCurrShareParam.reference = (long)avgCurrShareVolt.cma << 2;
    llcCurrShareParam.feedback = (long)avgOutputCurr.cma << 2;

    if (llcCurrShareParam.reference > (llcCurrShareParam.feedback + OUTPUTDIS_CURR(2.0)))
    {
        csVoltRefOffset++; // +1 = +0.02V
    }
    else if (llcCurrShareParam.reference < (llcCurrShareParam.feedback - OUTPUTDIS_CURR(2.0)))
    {
        csVoltRefOffset--;
    }

    csVoltRefOffset = __IQsat(csVoltRefOffset, llcCurrShareParam.maximum, llcCurrShareParam.minimum);
}
/***********************************************************************************************************************
Function Name:
    void Voltage_Droop(void)
Input:
    avgOutputCurr - Output current ADC value
Output:
    droopVoltRefOffset - Value add to voltage reference
Comment:
    N/A
***********************************************************************************************************************/
void Voltage_Droop(void)
{
    voltDroopVoltage = 0 - (float)avgOutputCurr.val * 0.0018f;
    droopVoltRefOffset = voltDroopVoltage * 34.68f; //old 22.02f

    droopVoltRefOffset = __IQsat(droopVoltRefOffset, llcVoltDroopParam.maximum, llcVoltDroopParam.minimum);
}
/***********************************************************************************************************************
Function Name:
    void Soh_Test_Voltage_Control(void)
Input:
    avgOutputVolt - Output voltage ADC value
    avgOutputCurr - Output current ADC value
Output:
    sohTestVolRefOffset - Value add to voltage reference
Comment:
    Use avgOutputCurr as the feedback.
    Output a offset to change voltage reference.
***********************************************************************************************************************/
void Soh_Test_Voltage_Control(void)
{
    targetI = MAX_DIS_POWER * 10 / (float)avgInnerVolt.val;
    if (targetI > MAX_DIS_CURRENT) targetI = MAX_DIS_CURRENT;

    sohCurrLoop.error = targetI * 100 - (float)avgOutputCurr.val;

    sohCurrLoop.pYn = sohCurrLoop.kp * sohCurrLoop.error;
    sohCurrLoop.iYn = sohCurrLoop.ki * sohCurrLoop.error;
    sohCurrLoop.iYn += sohCurrLoop.iYn1;

    sohCurrLoop.iYn += sohCurrLoop.kc * sohCurrLoop.satError;
    sohCurrLoop.iYn1 = sohCurrLoop.iYn;

    sohCurrLoop.piYn = sohCurrLoop.pYn + sohCurrLoop.iYn;

    if (sohCurrLoop.piYn > sohCurrLoop.max)
        sohCurrLoop.output = sohCurrLoop.max;
    else if (sohCurrLoop.piYn < sohCurrLoop.min)
        sohCurrLoop.output = sohCurrLoop.min;
    else
        sohCurrLoop.output = sohCurrLoop.piYn;

    sohCurrLoop.satError = sohCurrLoop.output - sohCurrLoop.piYn;

    sohTestVolRefOffset = OUTPUT_VOLT_BUS(sohCurrLoop.output);
}
/***************************************************************************************************
Function Name:
    void AhbLlcController(void)
Input:
    NULL
Output:
    NULL
Comment:
    AHB controller
***************************************************************************************************/
__attribute__((ramfunc))
void AhbController(void)
{
    CPU_AHB_State = cpuAhbState.all;

    ahbCurrReference = currSoftStartRef;

    if (ahbCurrChgDone)
        ahbParam.curr.reference = (long)currSetpoint;// + bmsCurrentOffset;
    else
        ahbParam.curr.reference = (long)ahbCurrReference;

    if (ahbVoltChgDone)
        ahbParam.volt.reference = (long)ahbVoltSetpoint;
    else
        ahbParam.volt.reference = (long)ahbVoltReference;


    ahbParam.volt.feedback = (long)avgChargeVolt.cma;//AdccResultRegs.ADCRESULT1;
    ahbParam.curr.feedback = (long)avgChargeCurr.cma;//AdccResultRegs.ADCRESULT2;

    CPU_ICharge_Ref = ahbParam.curr.reference; //Have set as Q14
    CPU_VCharge_Ref = ahbParam.volt.reference; //Have set as Q14
    CPU_AHB_Iout = ahbParam.curr.feedback << 2;
    CPU_AHB_Vout = ahbParam.volt.feedback << 2;

    switch(ahbMode)
    {
    default:
    case AHB_OFF_MODE:
        Chg_Pwm_Off();
        break;

    case AHB_CC_MODE:
        Chg_Pwm_On();
        cpuAhbState.bit.ccEnableDrive = 1;
        cpuAhbState.bit.cvEnableDrive = 0;
        break;

    case AHB_CV_MODE:
        Chg_Pwm_On();
        cpuAhbState.bit.ccEnableDrive = 0;
        cpuAhbState.bit.cvEnableDrive = 1;
        break;

    case AHB_ON_MODE:
        Chg_Pwm_On();
        cpuAhbState.bit.ccEnableDrive = 0;
        cpuAhbState.bit.cvEnableDrive = 0;
        break;
    }
}
/***********************************************************************************************************************
Function Name:
    void BMS_Current_Feedback(void)
Input:
    mode
Output:
    NULL
Comment:
    BMS Current Feedback
***********************************************************************************************************************/
void BMS_Current_Feedback(void)
{
    if (sohOutChgFlag)
    {
        if(bmsData.battChargeCurr > (DEFAULT_CHARGE_CURR2 + 4))
        {
            bmsCurrentOffset = bmsCurrentOffset - 9; // 37 = 0.05A
        }
        else if(bmsData.battChargeCurr < (DEFAULT_CHARGE_CURR2 - 16))
        {
            bmsCurrentOffset = bmsCurrentOffset + 9;
        }
    }
    else
    {
        if(bmsData.battChargeCurr > (DEFAULT_CHARGE_CURR1 + 4))
        {
            bmsCurrentOffset = bmsCurrentOffset - 9; // 37 = 0.05A
        }
        else if(bmsData.battChargeCurr < (DEFAULT_CHARGE_CURR1 - 16))
        {
            bmsCurrentOffset = bmsCurrentOffset + 9;
        }
    }

    bmsCurrentOffset = __IQsat(bmsCurrentOffset, 370, -740);
}
/***************************************************************************************************
Function Name:
    void Set_Ahb_Mode(AhbCtrlMode mode)
Input:
    mode
Output:
    NULL
Comment:
    AHB mode change
***************************************************************************************************/
void Set_Ahb_Mode(AhbCtrlMode mode)
{
    if(ahbMode != mode)
        ahbMode = mode;

    if(ahbMode == AHB_OFF_MODE)
    {
        ahbVoltReference = 0;
        ahbCurrReference = 0;
        ahbVoltChgDone = 0;
        ahbCurrChgDone = 0;
    }
}
/***************************************************************************************************
Function Name:
    void Ahb_Pwm_On(void)
Input:
    NULL
Output:
    NULL
Comment:
    Enable ePWM6 module output
***************************************************************************************************/
void Chg_Pwm_On(void)
{
    cpuAhbState.bit.enableAhb = 1;
    EPwm6Regs.AQCSFRC.bit.CSFA = AQ_NO_ACTION;
    EPwm6Regs.AQCSFRC.bit.CSFB = AQ_NO_ACTION;
//    EPwm6Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
//    EPwm6Regs.AQCTLB.bit.CAD = AQ_SET;
//    EPwm6Regs.AQCTLB.bit.CAU = AQ_CLEAR;
}
/***************************************************************************************************
Function Name:
    void Ahb_Pwm_Off(void)
Input:
    NULL
Output:
    NULL
Comment:
    Disable ePWM6 module output
***************************************************************************************************/
void Chg_Pwm_Off(void)
{
    cpuAhbState.bit.enableAhb = 0;
    EPwm6Regs.AQCSFRC.bit.CSFA = AQ_CLEAR;
    EPwm6Regs.AQCSFRC.bit.CSFB = AQ_SET;
//    EPwm6Regs.AQCTLB.bit.CAD = AQ_CLEAR;
//    EPwm6Regs.AQCTLB.bit.CAU = AQ_CLEAR;
}
//---------------- END LINE -----------------------------------------------------------------------
