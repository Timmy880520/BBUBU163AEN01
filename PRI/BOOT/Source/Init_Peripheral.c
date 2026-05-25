/**************************************************************************************************
File Name: Init_Peripheral.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    IER                                     CPU interrupt enable register
    IFR                                     CPU interrupt flag register
    GpioCtrlRegs                            f28004x_globalvariabledefs.c
    AnalogSubsysRegs                        f28004x_globalvariabledefs.c
    CpuSysRegs                              f28004x_globalvariabledefs.c
    EPwm3Regs                               f28004x_globalvariabledefs.c
    EPwm5Regs                               f28004x_globalvariabledefs.c
    EPwm7Regs                               f28004x_globalvariabledefs.c
    EPwmXbarRegs                            f28004x_globalvariabledefs.c
    WdRegs                                  f28004x_globalvariabledefs.c
    PieVectTable                            f28004x_globalvariabledefs.c
    PieCtrlRegs                             f28004x_globalvariabledefs.c
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    InitSysCtrl                             f28004x_piectrl.c
    InitPieCtrl                             f28004x_piectrl.c
    InitPieVectTable                        f28004x_pievect.c
    InitGpio                                f28004x_gpio.c
    Device_cal                              F28004x Support Library
    ServiceDog                              f28004x_sysctrl.c
    DINT                                    External compiler intrinsic prototypes
    EINT                                    External compiler intrinsic prototypes
    ERTM                                    External compiler intrinsic prototypes
    EALLOW                                  External compiler intrinsic prototypes
    EDIS                                    External compiler intrinsic prototypes
    CPU_Timer1_ISR                          main.c
Description:
    MCU peripheral initialization program for IAP.

===================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    01/06/2020 Watch Lee        1. version 1.0.

**************************************************************************************************/

/* Includes */
#include "F28x_Project.h"
#include "Global_Var.h"

/* Local function prototypes */
void Init_GPIO(void);
void Init_Analog_Subsystem(void);
void Setup_Capture(void);
void Setup_Dac(void);
void Init_DMA(void);
void Init_EPWM(void);
void Init_Cpu_Timer(void);
void Init_Watchdog(void);
void Init_Interrupt(void);

/* Data definitions */

/**************************************************************************************************
Function Name:
    void Init_Peripheral(void)
Input:
    None.
Output:
    None.
Comment:
    Peripheral initialization function.
**************************************************************************************************/
void Init_Peripheral(void)
{
    InitSysCtrl();              // Initialize device clock and peripherals

    Init_Interrupt();

    Init_GPIO();

    Setup_Capture();

    Setup_Dac();
//    Init_Analog_Subsystem();

    Init_DMA();
//    // ePWM6 - For pseudo position
//    EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
//    EPwm6Regs.TBCTL.bit.PHSEN = TB_DISABLE;
//    EPwm6Regs.TBCTL.bit.PRDLD = TB_SHADOW;
//    EPwm6Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
//    EPwm6Regs.TBCTL.bit.HSPCLKDIV = 7;
//    EPwm6Regs.TBCTL.bit.CLKDIV = 7;
//    EPwm6Regs.TBPHS.bit.TBPHS = 0;
//    EPwm6Regs.TBPRD = 55803 / 1000;
//    EPwm6Regs.TBCTR = 0;
//
//    EPwm6Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
//    EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
//    EPwm6Regs.CMPA.bit.CMPA = EPwm6Regs.TBPRD >> 1;
//
//    EPwm6Regs.AQCTLA.bit.ZRO = AQ_SET;
//    EPwm6Regs.AQCTLA.bit.CAU = AQ_CLEAR;
//    Init_EPWM();

    Init_Cpu_Timer();

    Init_Watchdog();
}
/**************************************************************************************************
Function Name:
    void Deinit_Periphral(void)
Input:
    None.
Output:
    None.
Comment:
    Peripheral reset function.
**************************************************************************************************/
void Deinit_Periphral(void)
{
    EALLOW;
    DevCfgRegs.SOFTPRES0.all = 0xFFFFFFFF;
    DevCfgRegs.SOFTPRES2.all = 0xFFFFFFFF;
    DevCfgRegs.SOFTPRES3.all = 0xFFFFFFFF;
    DevCfgRegs.SOFTPRES4.all = 0xFFFFFFFF;
    DevCfgRegs.SOFTPRES6.all = 0xFFFFFFFF;
    DevCfgRegs.SOFTPRES7.all = 0xFFFFFFFF;
    DevCfgRegs.SOFTPRES8.all = 0xFFFFFFFF;
    DevCfgRegs.SOFTPRES9.all = 0xFFFFFFFF;
    DevCfgRegs.SOFTPRES10.all = 0xFFFFFFFF;
    DevCfgRegs.SOFTPRES13.all = 0xFFFFFFFF;
    DevCfgRegs.SOFTPRES14.all = 0xFFFFFFFF;
    DevCfgRegs.SOFTPRES16.all = 0xFFFFFFFF;
    DevCfgRegs.SOFTPRES17.all = 0xFFFFFFFF;
    DevCfgRegs.SOFTPRES18.all = 0xFFFFFFFF;
    DevCfgRegs.SOFTPRES19.all = 0xFFFFFFFF;
    DevCfgRegs.SOFTPRES20.all = 0xFFFFFFFF;
    DevCfgRegs.SOFTPRES21.all = 0xFFFFFFFF;
    DevCfgRegs.SOFTPRES25.all = 0xFFFFFFFF;
    DevCfgRegs.SOFTPRES26.all = 0xFFFFFFFF;
    DevCfgRegs.SOFTPRES27.all = 0xFFFFFFFF;

    asm(" NOP");

    DevCfgRegs.SOFTPRES0.all = 0;
    DevCfgRegs.SOFTPRES2.all = 0;
    DevCfgRegs.SOFTPRES3.all = 0;
    DevCfgRegs.SOFTPRES4.all = 0;
    DevCfgRegs.SOFTPRES6.all = 0;
    DevCfgRegs.SOFTPRES7.all = 0;
    DevCfgRegs.SOFTPRES8.all = 0;
    DevCfgRegs.SOFTPRES9.all = 0;
    DevCfgRegs.SOFTPRES10.all = 0;
    DevCfgRegs.SOFTPRES13.all = 0;
    DevCfgRegs.SOFTPRES14.all = 0;
    DevCfgRegs.SOFTPRES16.all = 0;
    DevCfgRegs.SOFTPRES17.all = 0;
    DevCfgRegs.SOFTPRES18.all = 0;
    DevCfgRegs.SOFTPRES19.all = 0;
    DevCfgRegs.SOFTPRES20.all = 0;
    DevCfgRegs.SOFTPRES21.all = 0;
    DevCfgRegs.SOFTPRES25.all = 0;
    DevCfgRegs.SOFTPRES26.all = 0;
    DevCfgRegs.SOFTPRES27.all = 0;
    EDIS;
}
/**************************************************************************************************
Function Name:
    void Init_GPIO(void)
Input:
    None.
Output:
    None.
Comment:
    GPIO initialization function.
**************************************************************************************************/
void Init_GPIO(void)
{
    InitGpio();                             // Initialize GPIO

    EALLOW;
    GpioCtrlRegs.GPACTRL.all = 0x00000000;
    GpioCtrlRegs.GPBCTRL.all = 0x00000000;
    // MUX setting
    // ePWM = GPIO 0~7
    // Fan  = GPIO 10
    // CAN  = GPIO 16,17
    // SCI  = GPIO 28,29
    // I2C  = GPIO 26,27
    // JTAG = GPIO 35,37
    // Qualification setting
    // CAN  = GPIO 16,17
    // SCI  = GPIO 28,29
    // I2C  = GPIO 26,27
    // O/P_OVP -> TRIP1
    //      = GPIO 31
    // VBUS_UNB -> TRIP2
    //      = GPIO 14
    // FAN  = GPIO 9, 39
/*
                            //    15141312111009080706050403020100
    GpioCtrlRegs.GPAMUX1.all  = 0b00000000000100000101010101010101;
    GpioCtrlRegs.GPAGMUX1.all = 0b00000000000000000000000000000000;
    GpioCtrlRegs.GPAQSEL1.all = 0b00100000000010000000000000000000; // fan, trip, communication
                            //    31302928272625242322212019181716
    GpioCtrlRegs.GPAMUX2.all  = 0b00001010110000000000000000001010;
    GpioCtrlRegs.GPAGMUX2.all = 0b00001010100000000000000000000000;
    GpioCtrlRegs.GPAQSEL2.all = 0b10001111110000000000000000001111;
                            //    47464544434241403938373635343332
    GpioCtrlRegs.GPBMUX1.all  = 0b00000000000000000000110011000000;
    GpioCtrlRegs.GPBGMUX1.all = 0b00000000000000000000110011000000;
    GpioCtrlRegs.GPBQSEL1.all = 0b00000000000000001000000000000000;
                            //    63626160595857565554535251504948
    GpioCtrlRegs.GPBMUX2.all  = 0b00000000000000000000000000000000;
    GpioCtrlRegs.GPBGMUX2.all = 0b00000000000000000000000000000000;
    GpioCtrlRegs.GPBQSEL2.all = 0b00000000000000000000000000000000;
*/
    // Analog mode
    GpioCtrlRegs.GPAAMSEL.all = 0x00000000;
    GpioCtrlRegs.GPHAMSEL.all = 0x00000000;
/*
                              //  33222222222211111111110000000000
                              //  10987654321098765432109876543210
    GpioDataRegs.GPADAT.all   = 0b01000000000000001010100000000000;
    GpioCtrlRegs.GPADIR.all   = 0b01000100000000001010100000000000;
    GpioCtrlRegs.GPAPUD.all   = 0b00111100000000110000011011111111;
    GpioCtrlRegs.GPACSEL1.all = 0b00000000000000000000000000000000; // CLA control core select

                              //  66665555555555444444444433333333
                              //  32109876543210987654321098765432
    GpioDataRegs.GPBDAT.all   = 0b00000100000000000000000000000000;
    GpioCtrlRegs.GPBDIR.all   = 0b00000101000000000000000000000110;
    GpioCtrlRegs.GPBPUD.all   = 0b00001000000000000000000010000000;
    GpioCtrlRegs.GPACSEL1.all = 0b00000000000000000000000000000000; // CLA control core select
*/
/*
    // communication
    // GPIO28 for LINA_TX
    GpioCtrlRegs.GPAGMUX2.bit.GPIO28 = 2;   // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 2;
    GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;     // Enable internal pull-up
    //GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;
    // GPIO29 for LINA_RX
    GpioCtrlRegs.GPAGMUX2.bit.GPIO29 = 2;   // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 2;
    GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;     // Enable internal pull-up
    GpioCtrlRegs.GPAQSEL2.bit.GPIO29 = 3;
*/
    //GPIO48 for SCIA_TX
    GpioCtrlRegs.GPBGMUX2.bit.GPIO48 = 1;   // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 2;
    GpioCtrlRegs.GPBPUD.bit.GPIO48 = 0;     // Enable internal pull-up
    //GpioCtrlRegs.GPBQSEL2.bit.GPIO48 = 3;

    //GPIO49 for SCIA_RX
    GpioCtrlRegs.GPBGMUX2.bit.GPIO49 = 1;   // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 2;
    GpioCtrlRegs.GPBPUD.bit.GPIO49 = 0;     // Enable internal pull-up
    GpioCtrlRegs.GPBQSEL2.bit.GPIO49 = 3;

    // GPIO51 for DO (GPIO_RLY_CTRL_BATT)
    GpioDataRegs.GPBCLEAR.bit.GPIO51 = 1;    // Default low
    GpioCtrlRegs.GPBGMUX2.bit.GPIO51 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO51 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO51 = 1;
    EDIS;
}
/**************************************************************************************************
Function Name:
    void Init_Analog_Subsystem(void)
Input:
    None.
Output:
    None.
Comment:
    Analog subsystem initialization function.
**************************************************************************************************/
void Init_Analog_Subsystem(void)
{
    EALLOW;
    AnalogSubsysRegs.ANAREFCTL.bit.ANAREFSEL = 0;       // Analog reference uses internal reference mode.

    AnalogSubsysRegs.CMPHPMXSEL.bit.CMP1HPMXSEL = 1;    // C0 for CMP1HP
    AnalogSubsysRegs.CMPHPMXSEL.bit.CMP3HPMXSEL = 1;    // C2 for CMP3HP
//    AnalogSubsysRegs.CMPHPMXSEL.bit.CMP5HPMXSEL = 1;    // C4 for CMP5HP

    AnalogSubsysRegs.CMPLPMXSEL.bit.CMP1LPMXSEL = 1;    // C0 for CMP1LP
    AnalogSubsysRegs.CMPLPMXSEL.bit.CMP3LPMXSEL = 1;    // C2 for CMP3LP
//    AnalogSubsysRegs.CMPLPMXSEL.bit.CMP5LPMXSEL = 1;    // C4 for CMP5LP
    EDIS;

    (*Device_cal)();                                    // Update analog trim registers.
}
/***************************************************************************************************
Function Name:
    void Setup_Capture(void)
Input:
    NULL
Output:
    NULL
Comment:
    Setup capture for fan tacho detect
***************************************************************************************************/
void Setup_Capture(void)
{
/*
    // Ecap1
    EALLOW;
    // GPIO59 -> InputXbar7 -> eCap1
    InputXbarRegs.INPUT7SELECT = 59;
    ECap1Regs.ECCTL0.bit.INPUTSEL = 6;

    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0;     // Stop eCap counter
    ECap1Regs.ECCTL2.bit.CAP_APWM = 0;      // Capture mode
    ECap1Regs.ECCTL2.bit.CONT_ONESHT = 0;   // Continuous mode
    ECap1Regs.ECCTL2.bit.STOP_WRAP = 0x3;
    ECap1Regs.ECCTL2.bit.DMAEVTSEL = 0x0;   // DMA triggered at event 4
    // Capture event trigger edge
    ECap1Regs.ECCTL1.bit.CAP1POL = 1;
    ECap1Regs.ECCTL1.bit.CAP2POL = 1;
    ECap1Regs.ECCTL1.bit.CAP3POL = 1;
    ECap1Regs.ECCTL1.bit.CAP4POL = 1;
    // Counter reset at capture event4
    ECap1Regs.ECCTL1.bit.CTRRST1 = 1;
    ECap1Regs.ECCTL1.bit.CTRRST2 = 1;
    ECap1Regs.ECCTL1.bit.CTRRST3 = 1;
    ECap1Regs.ECCTL1.bit.CTRRST4 = 1;
    // Disable all interrupt
    ECap1Regs.ECEINT.all = 0x00;
    ECap1Regs.ECCLR.all = 0xFF;

    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;
    ECap1Regs.ECCTL1.bit.CAPLDEN = 1;
    ECap1Regs.ECCTL2.bit.REARM = 1;
    EDIS;
*/
}
/***************************************************************************************************
Function Name:
    void Setup_Dac(void)
Input:
    NULL
Output:
    NULL
Comment:
    Setup DAC for debug
***************************************************************************************************/
void Setup_Dac(void)
{
    // DAC A setting, output maximum = 3.3V
//    EALLOW;
//    DacaRegs.DACCTL.bit.DACREFSEL = 1;
//    DacaRegs.DACCTL.bit.MODE = 1;
//    DacaRegs.DACOUTEN.bit.DACOUTEN = 1;
//    DacaRegs.DACVALS.all = 2500;
//    DELAY_US(10);
//    EDIS;
}
/**************************************************************************************************
Function Name:
    void Init_DMA(void)
Input:
    None.
Output:
    None.
Comment:
    DMA initialization function.
**************************************************************************************************/
void Init_DMA(void)
{
    EALLOW;
    DmaRegs.DMACTRL.bit.HARDRESET = 1;
    asm(" NOP");
    EDIS;
}
/**************************************************************************************************
Function Name:
    void Init_EPWM(void)
Input:
    None.
Output:
    None.
Comment:
    EPWM initialization function. The relationship of each phase are as follows:

      |   R-phase   |   S-phase   |   T-phase   |
-------------------------------------------------
iADC  |   C4        |   C0        |   C2        |
EPWM  |   EPWM7     |   EPWM5     |   EPWM3     |
CMPSS |   CMPSS5    |   CMPSS1    |   CMPSS3    |

    EPWMs are configured as follows:

           /\      /\
CMPA  __ _/__\_ __/__\_ _
         /:  :\  /:  :\
CMPC  __/_:__:_\/_:__:_\/
        __:  :__:_:  :___
EPWMxA    |__|  : |__|
                :
                ^-Trigger EPWM7SOCA

**************************************************************************************************/
void Init_EPWM(void)
{
    // Disable TBCLK SYNC
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;
/*
    // Set EPWM3 (EPWM_PFC_GT)
    EPwm3Regs.TBCTL.bit.HSPCLKDIV = 0;
    EPwm3Regs.TBCTL.bit.PRDLD = 0;
    EPwm3Regs.TBCTL.bit.CTRMODE = 2;

    EPwm3Regs.CMPCTL.bit.SHDWAMODE = 0;             // Default shadow mode
    EPwm3Regs.CMPA.bit.CMPA = 0;                    // Duty = 0%
    EPwm3Regs.TBPRD = 1000;

    EPwm3Regs.AQCTLA.bit.CAD = 2;                   // High for down-count match
    EPwm3Regs.AQCTLA.bit.CAU = 1;                   // Low for up-count match

    EPwm3Regs.DBCTL.bit.IN_MODE = 2;
    EPwm3Regs.DBCTL.bit.POLSEL = 0;
    EPwm3Regs.DBCTL.bit.OUT_MODE = 3;
    EPwm3Regs.DBCTL.bit.OUTSWAP = 0;

    // Set EPWM5 (EPWM_PFC_GS)
    EPwm5Regs.TBCTL.bit.HSPCLKDIV = 0;
    EPwm5Regs.TBCTL.bit.PRDLD = 0;
    EPwm5Regs.TBCTL.bit.CTRMODE = 2;

    EPwm5Regs.CMPCTL.bit.SHDWAMODE = 0;             // Default shadow mode
    EPwm5Regs.CMPA.bit.CMPA = 0;                    // Duty = 0%
    EPwm5Regs.TBPRD = 1000;

    EPwm5Regs.AQCTLA.bit.CAD = 2;                   // High for down-count match
    EPwm5Regs.AQCTLA.bit.CAU = 1;                   // Low for up-count match

    EPwm5Regs.DBCTL.bit.IN_MODE = 2;
    EPwm5Regs.DBCTL.bit.POLSEL = 0;
    EPwm5Regs.DBCTL.bit.OUT_MODE = 3;
    EPwm5Regs.DBCTL.bit.OUTSWAP = 0;

    // Set EPWM7 (EPWM_PFC_GR)
    EPwm7Regs.TBCTL.bit.HSPCLKDIV = 0;
    EPwm7Regs.TBCTL.bit.PRDLD = 0;
    EPwm7Regs.TBCTL.bit.CTRMODE = 2;

    EPwm7Regs.CMPCTL.bit.SHDWAMODE = 0;             // Default shadow mode
    EPwm7Regs.CMPA.bit.CMPA = 0;                    // Duty = 0%
    EPwm7Regs.CMPC = 5;                             // Set SOCA at incrementing point
    EPwm7Regs.TBPRD = 1000;

    EPwm7Regs.AQCTLA.bit.CAD = 2;                   // High for down-count match
    EPwm7Regs.AQCTLA.bit.CAU = 1;                   // Low for up-count match

    EPwm7Regs.DBCTL.bit.IN_MODE = 2;
    EPwm7Regs.DBCTL.bit.POLSEL = 0;
    EPwm7Regs.DBCTL.bit.OUT_MODE = 3;
    EPwm7Regs.DBCTL.bit.OUTSWAP = 0;

    EPwm7Regs.ETPS.bit.SOCAPRD = 1;                 // Generate the EPWMxSOCA pulse on the first event
    EPwm7Regs.ETSEL.bit.SOCASELCMP = 1;             // Select CMPC/CMPD to control SOCA
    EPwm7Regs.ETSEL.bit.SOCASEL = 4;                // Select CMPA/CMPC to trigger SOCA when the timer is incrementing
    EPwm7Regs.ETSEL.bit.SOCAEN = 1;

    // Set EPWM TZ mode
    EALLOW;
    EPwm3Regs.DCTRIPSEL.bit.DCAHCOMPSEL = 3;        // TRIPIN4
    EPwm3Regs.TZSEL.bit.DCAEVT2 = 1;                // Enable DCAEVT2 as a CBC trip source
    EPwm3Regs.TZDCSEL.bit.DCAEVT2 = 2;
    EPwm3Regs.TZCTL.bit.DCAEVT2 = 2;
    EPwm3Regs.TZCTL.bit.TZA = 2;
    EPwm3Regs.TZCLR.bit.CBCPULSE = 1;               // PRD pulse clears CBC trip latch

    EPwmXbarRegs.TRIP4MUX0TO15CFG.bit.MUX4 = 1;     // MUX4 for CMPSS3.CTRIPH or CTRIPL
    EPwmXbarRegs.TRIP4MUXENABLE.bit.MUX4 = 1;

    EPwm5Regs.DCTRIPSEL.bit.DCAHCOMPSEL = 4;        // TRIPIN5
    EPwm5Regs.TZSEL.bit.DCAEVT2 = 1;                // Enable DCAEVT2 as a CBC trip source
    EPwm5Regs.TZDCSEL.bit.DCAEVT2 = 2;
    EPwm5Regs.TZCTL.bit.DCAEVT2 = 2;
    EPwm5Regs.TZCTL.bit.TZA = 2;
    EPwm5Regs.TZCLR.bit.CBCPULSE = 1;               // PRD pulse clears CBC trip latch

    EPwmXbarRegs.TRIP5MUX0TO15CFG.bit.MUX0 = 1;     // MUX0 for CMPSS1.CTRIPH or CTRIPL
    EPwmXbarRegs.TRIP5MUXENABLE.bit.MUX0 = 1;

    EPwm7Regs.DCTRIPSEL.bit.DCAHCOMPSEL = 6;        // TRIPIN7
    EPwm7Regs.TZSEL.bit.DCAEVT2 = 1;                // Enable DCAEVT2 as a CBC trip source
    EPwm7Regs.TZDCSEL.bit.DCAEVT2 = 2;
    EPwm7Regs.TZCTL.bit.DCAEVT2 = 2;
    EPwm7Regs.TZCTL.bit.TZA = 2;
    EPwm7Regs.TZCLR.bit.CBCPULSE = 1;               // PRD pulse clears CBC trip latch

    EPwmXbarRegs.TRIP7MUX0TO15CFG.bit.MUX8 = 1;     // MUX8 for CMPSS5.CTRIPH or CTRIPL
    EPwmXbarRegs.TRIP7MUXENABLE.bit.MUX8 = 1;
*/
    // Enable TBCLK SYNC
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
}
/**************************************************************************************************
Function Name:
    void Init_Cpu_Timer(void)
Input:
    None.
Output:
    None.
Comment:
    CPU timer initialization function.
**************************************************************************************************/
void Init_Cpu_Timer(void)
{
    CpuTimer1Regs.TPR.bit.TDDR = 0;     // Timer period = (TDDR + 1) * LSW / 120MHz
    CpuTimer1Regs.PRD.all = 120000;     // (0 + 1) * 120000 / 120MHz = 1ms
    CpuTimer1Regs.TCR.bit.TIE = 1;
}
/**************************************************************************************************
Function Name:
    void Init_Watchdog(void)
Input:
    None.
Output:
    None.
Comment:
    Watchdog initialization function.
**************************************************************************************************/
void Init_Watchdog(void)
{
    ServiceDog();

    union WDCR_REG wdTemp;

    wdTemp.bit.WDPRECLKDIV = 1;
    wdTemp.bit.WDDIS = 0;
    wdTemp.bit.WDCHK = 5;
    wdTemp.bit.WDPS = 5;        // 256 * 16 * 1024 / 10MHz = 419.43ms

    EALLOW;
    WdRegs.WDCR.all = wdTemp.all;
    EDIS;
}
/**************************************************************************************************
Function Name:
    void Init_Interrupt(void)
Input:
    None.
Output:
    None.
Comment:
    Interrupt initialization function.
**************************************************************************************************/
void Init_Interrupt(void)
{
    // Disable CPU interrupts
    DINT;
/*
    Initialize the PIE control registers to their default state.
    The default state is all PIE interrupts disabled and flags are cleared.
*/
    InitPieCtrl();

    // Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;

    // Initialize the PIE vector table with pointers to the shell Interrupt Service Routines (ISR).
    InitPieVectTable();

    // Map ISR functions
    EALLOW;
    PieVectTable.TIMER1_INT = &CPU_Timer1_ISR;
    EDIS;

    // Enable global Interrupts and higher priority real-time debug events:
    IER |= M_INT13;                // Enable group 1 and 10 interrupts

    PieCtrlRegs.PIEACK.all = 0xFFFF;
    asm(" NOP");

    EINT;                                   // Enable Global interrupt INTM
    ERTM;                                   // Enable Global realtime interrupt DBGM
}
//---------------- END LINE -----------------------------------------------------------------------
