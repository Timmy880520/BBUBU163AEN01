/***************************************************************************************************
File Name: Interrupt.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ --------------------------------------------------------

Description:

====================================================================================================
History:
    Date        Author          Description of Change
    ----------- --------------- --------------------------------------------------------------------
    10/17/2019  Fred            Create file
    03/19/2021  Fred Huang      Remove fan content
    05/06/2021  Watch Lee       1. update global functions.

***************************************************************************************************/

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

//------------------------------------------------------------------------------
// Condition definitions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Macro definitions
//------------------------------------------------------------------------------
#define ADC_RESULT(m,n)         Adc##m##ResultRegs.ADCRESULT##n

// Time constant
#define ISR_CNT(s)              (unsigned long)(CONTROLLER_FREQ * (s))

//------------------------------------------------------------------------------
// Type definitions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Global function prototypes
//------------------------------------------------------------------------------
// Interrupt service routine
__interrupt void Timer1_ISR(void);
__interrupt void Timer2_ISR(void);
__interrupt void Default_ISR(void);
// CLA Task
//__interrupt void claBuckControl(void);
__interrupt void claDefaultTask(void);
// Timer tick
unsigned short Get_Timer_Tick(void);
void Set_Timer_Tick(void);
unsigned short Get_ResetMCU_Tick(void);
void Set_ResetMCU_Tick(void);
void CPU_UpdateMode(void);
//PMBus
__interrupt void PMBusA_ISR(void);
//------------------------------------------------------------------------------
// Global data declarations
//------------------------------------------------------------------------------
// ADC result
// ADC - A
extern volatile unsigned short adcOutputCurr;
extern volatile unsigned short adcCurrShareVolt;
extern volatile unsigned short adcOutputCurrPK;
//extern volatile unsigned short adcStandbyVolt;
// ADC - B
extern volatile unsigned short adcOutputCurrSec;
extern volatile unsigned short adcOutputVolt;
extern volatile unsigned short adcBattVolt;
//extern volatile unsigned short adcStandbyCurr;
//extern volatile unsigned short adcBulkVolt;
// ADC - C
extern volatile unsigned short adcChargeVolt;
extern volatile unsigned short adcInnerVolt;
extern volatile unsigned short adcChargeCurr;
//extern volatile unsigned short adcBattCurr;
// Raw Power = ADC_Voltage * ADC_Current
extern volatile unsigned short rawOutputPower;
extern volatile unsigned short rawStandOutputPower;

extern volatile unsigned short rawChargePower;

extern unsigned long cpuTimer2Countdown, cpuTimer1Countdown;

//------------------------------------------------------------------------------
#endif /* _INTERRUPT_H_ */
