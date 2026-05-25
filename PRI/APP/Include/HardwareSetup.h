/***************************************************************************************************
File Name: HardwareSetup.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ --------------------------------------------------------

Description:

====================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- --------------------------------------------------------------------
    10/24/2019   Fred            Create file

***************************************************************************************************/

#ifndef _HARDWARESETUP_H_
#define _HARDWARESETUP_H_

//------------------------------------------------------------------------------
// Condition definitions
//------------------------------------------------------------------------------
// System
#define HAVE_EXT_OSC            1
#define CPU_CLK                 (120.0e6f)
// ADC
#define ADC_OFFSET_TRIM(n)      (0x70594 + 6 * n)
// LLC
#define BACKGROUND_FREQ         (2.0e3f)
#define CONTROLLER_FREQ         (50.0e3f)
#define INIT_SWITCH_FREQ        (300.0e3f)

//------------------------------------------------------------------------------
// Macro definitions
//------------------------------------------------------------------------------
#define INIT_SWITCH_PERIOD      (unsigned short)(CPU_CLK / INIT_SWITCH_FREQ / 2)
#define INIT_PRI_SWITCH_DB      (unsigned short)(CPU_CLK * (0.5e-6f))
#define INIT_SEC_SWITCH_DB      (unsigned short)(CPU_CLK * (1.5e-6f))

// Task tick
#define CPU_Timer2_Base             120000UL                                         // refer to Setup_Timer function
#define CPU_Timer2_Tick_Q_Format    12
#define CPU_Timer2_Tick_Scale_Float ((float)(1 << CPU_Timer2_Tick_Q_Format) / CPU_Timer2_Base)
#define CPU_Timer2_Tick_Scale_Q15   (unsigned long)(CPU_Timer2_Tick_Scale_Float * 32768.0F)

#define CPU_Timer1_Base             ((unsigned long)(CPU_CLK / CONTROLLER_FREQ) - 1) // refer to Setup_Timer function
#define CPU_Timer1_Tick_Q_Format    12
#define CPU_Timer1_Tick_Scale_Float ((float)(1 << CPU_Timer1_Tick_Q_Format) / CPU_Timer1_Base)
#define CPU_Timer1_Tick_Scale_Q15   (unsigned long)(CPU_Timer1_Tick_Scale_Float * 32768.0F)

// Constant replacement
#define Float_Divisor_Q12           0.000244140625F     // 1 / 2^12

//------------------------------------------------------------------------------
// Type definitions
//------------------------------------------------------------------------------
// Structure & Union
typedef struct
{
    unsigned long usageQ12;
    float usage;
}CpuUsageType;

//------------------------------------------------------------------------------
// Global function prototypes
//------------------------------------------------------------------------------
void Hw_Setup(void);

//------------------------------------------------------------------------------
// Global data declarations
//------------------------------------------------------------------------------
extern unsigned short Cla1ProgLoadStart;
extern unsigned short Cla1ProgLoadSize;
extern unsigned short Cla1ProgRunStart;

//------------------------------------------------------------------------------
#endif /* _HARDWARESETUP_H_ */
