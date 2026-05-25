/***************************************************************************************************
File Name: HardwareSetup.c
External Data:
    Name                                    Source
    --------------------------------------- --------------------------------------------------------

External Functions:
    Name                                    Source
    --------------------------------------- --------------------------------------------------------

Description:

====================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    Date        Author          Description of Change
    ----------  --------------- --------------------------------------------------------------------
    10/24/2019  Fred Huang      Create file
    02/12/2020  Fred Huang      1. Replace part Setup_Can to CAN_Driver
                                2. Replace part Setup_Dma to CAN_Driver
    03/11/2021  Fred Huang      1. Disable I2C clock
                                2. Set VBUS_UNB to TRIP2 then trigger digital comparator
                                3. Add output XBAR to GPIO for debug
    05/06/2021  Watch Lee       1. Add CpuTimer2 configuration.
                                2. correct GPIO26 configuration.

***************************************************************************************************/

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "App.h"

//------------------------------------------------------------------------------
// Local function prototypes
//------------------------------------------------------------------------------

void Setup_Cla(void);
void Setup_Dma(void);
void Setup_Timer(void);
void Setup_Gpio(void);
void Setup_Adc(void);
void Setup_Dac(void);
void Setup_Comparator(void);
void Setup_Pwm(void);
void Setup_Capture(void);
void Setup_Sci(void);
void Setup_Lin(void);
void Setup_Can(void);
void Setup_Interrupt(void);

//------------------------------------------------------------------------------
// Data definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------
/***************************************************************************************************
Function Name:
    void Setup_System(void)
Input:
    NULL
Output:
    NULL
Comment:
    Setup system clock and flash
***************************************************************************************************/
/*void Setup_System(void)
{
    // Initialize ramfuncs
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);

    // Initialize OSC
    EALLOW;
#if             HAVE_EXT_OSC > 0                // 20MHz crystal
    ClkCfgRegs.XTALCR.bit.OSCOFF = 0;
    ClkCfgRegs.X1CNT.bit.CLR = 1;
    while(ClkCfgRegs.X1CNT.bit.X1CNT != 0x3ff);
    ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 0x1;
    while(ClkCfgRegs.MCDCR.bit.MCLKSTS)
    {
        ClkCfgRegs.MCDCR.bit.MCLKCLR = 1;
        ClkCfgRegs.X1CNT.bit.CLR = 1;
        while(ClkCfgRegs.X1CNT.bit.X1CNT != 0x3fff);
        ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 0x1;
    }
#else                                           // Internal RC 10MHz
    ClkCfgRegs.CLKSRCCTL1.bit.INTOSC2OFF = 0;
    ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 0x0;
    ClkCfgRegs.XTALCR.bit.OSCOFF = 1;
#endif
    EDIS;

    // Initialize PLL
    EALLOW;
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 0;
    asm(" RPT #60 || NOP");
    ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = 0;
#if             HAVE_EXT_OSC > 0                // 20MHz crystal
    ClkCfgRegs.SYSPLLMULT.all = 0x0000A;
#else                                           // Internal RC 10MHz
    ClkCfgRegs.SYSPLLMULT.all = 0x00014;
#endif
    ClkCfgRegs.SYSPLLCTL1.bit.PLLEN = 1;
    while(ClkCfgRegs.SYSPLLSTS.bit.LOCKS != 1);
    ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = 0x1;
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 1;
    EDIS;

    // Initialize Flash
    EALLOW;
    Flash0CtrlRegs.FPAC1.bit.PMPPWR = 0x1;
    Flash0CtrlRegs.FBFALLBACK.bit.BNKPWR0 = 0x3;
    Flash0CtrlRegs.FBFALLBACK.bit.BNKPWR1 = 0x3;
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.DATA_CACHE_EN = 0;
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.PREFETCH_EN = 0;
#if             HAVE_EXT_OSC > 0                // 20MHz crystal
    Flash0CtrlRegs.FRDCNTL.bit.RWAIT = 0x4;
#else                                           // Internal RC 10MHz
    Flash0CtrlRegs.FRDCNTL.bit.RWAIT = 0x5;
#endif
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.DATA_CACHE_EN = 1;
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.PREFETCH_EN = 1;
    Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0xA;
    EDIS;
    asm(" RPT #7 || NOP");

    // Initialize Peripheral Clocks
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.CLA1 = 1;
    CpuSysRegs.PCLKCR0.bit.DMA = 1;
    CpuSysRegs.PCLKCR0.bit.CPUTIMER0 = 0;
    CpuSysRegs.PCLKCR0.bit.CPUTIMER1 = 1;
    CpuSysRegs.PCLKCR0.bit.CPUTIMER2 = 1;
    CpuSysRegs.PCLKCR0.bit.HRPWM = 0;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;

    CpuSysRegs.PCLKCR2.bit.EPWM1 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM2 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM3 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM4 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM5 = 0;
    CpuSysRegs.PCLKCR2.bit.EPWM6 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM7 = 0;
    CpuSysRegs.PCLKCR2.bit.EPWM8 = 0;

    CpuSysRegs.PCLKCR3.bit.ECAP1 = 1;
    CpuSysRegs.PCLKCR3.bit.ECAP2 = 1;
    CpuSysRegs.PCLKCR3.bit.ECAP3 = 1;
    CpuSysRegs.PCLKCR3.bit.ECAP4 = 0;
    CpuSysRegs.PCLKCR3.bit.ECAP5 = 0;
    CpuSysRegs.PCLKCR3.bit.ECAP6 = 0;
    CpuSysRegs.PCLKCR3.bit.ECAP7 = 0;

    CpuSysRegs.PCLKCR4.bit.EQEP1 = 0;
    CpuSysRegs.PCLKCR4.bit.EQEP2 = 0;

    CpuSysRegs.PCLKCR6.bit.SD1 = 0;

    CpuSysRegs.PCLKCR7.bit.SCI_A = 1;
    CpuSysRegs.PCLKCR7.bit.SCI_B = 1;

    CpuSysRegs.PCLKCR8.bit.SPI_A = 0;
    CpuSysRegs.PCLKCR8.bit.SPI_B = 0;

    CpuSysRegs.PCLKCR9.bit.I2C_A = 1;

    CpuSysRegs.PCLKCR10.bit.CAN_A = 0;
    CpuSysRegs.PCLKCR10.bit.CAN_B = 1;

    CpuSysRegs.PCLKCR13.bit.ADC_A = 1;
    CpuSysRegs.PCLKCR13.bit.ADC_B = 1;
    CpuSysRegs.PCLKCR13.bit.ADC_C = 1;

    CpuSysRegs.PCLKCR14.bit.CMPSS1 = 1;
    CpuSysRegs.PCLKCR14.bit.CMPSS2 = 0;
    CpuSysRegs.PCLKCR14.bit.CMPSS3 = 1;
    CpuSysRegs.PCLKCR14.bit.CMPSS4 = 0;
    CpuSysRegs.PCLKCR14.bit.CMPSS5 = 1;
    CpuSysRegs.PCLKCR14.bit.CMPSS6 = 1;
    CpuSysRegs.PCLKCR14.bit.CMPSS7 = 0;

    CpuSysRegs.PCLKCR15.bit.PGA1 = 0;
    CpuSysRegs.PCLKCR15.bit.PGA2 = 0;
    CpuSysRegs.PCLKCR15.bit.PGA3 = 0;
    CpuSysRegs.PCLKCR15.bit.PGA4 = 0;
    CpuSysRegs.PCLKCR15.bit.PGA5 = 0;
    CpuSysRegs.PCLKCR15.bit.PGA6 = 0;
    CpuSysRegs.PCLKCR15.bit.PGA7 = 0;

    CpuSysRegs.PCLKCR16.bit.DAC_A = 1;
    CpuSysRegs.PCLKCR16.bit.DAC_B = 1;

    CpuSysRegs.PCLKCR19.bit.LIN_A = 1;

    CpuSysRegs.PCLKCR20.bit.PMBUS_A = 1;

    CpuSysRegs.PCLKCR21.bit.DCC_0 = 0;
    EDIS;
}*/
/***************************************************************************************************
Function Name:
    void Setup_Cla(void)
Input:
    NULL
Output:
    NULL
Comment:
    Setup CLA
***************************************************************************************************/
void Setup_Cla(void)
{
    memcpy(&ClaFuncsRunStart, &ClaFuncsLoadStart, (size_t)&ClaFuncsLoadSize);
    memcpy(&ClaConstRunStart, &ClaConstLoadStart, (size_t)&ClaConstLoadSize);

    EALLOW;

    MemCfgRegs.MSGxINIT.all = 6;

//    while (!MemCfgRegs.LSxINITDONE.bit.INITDONE_LS5);
    while (!MemCfgRegs.MSGxINITDONE.all);

    MemCfgRegs.LSxMSEL.bit.MSEL_LS4 = 1;
    MemCfgRegs.LSxMSEL.bit.MSEL_LS5 = 1;
    MemCfgRegs.LSxMSEL.bit.MSEL_LS6 = 1;
//    MemCfgRegs.LSxMSEL.bit.MSEL_LS7 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS4 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS5 = 1;

    unsigned long claIsr = (unsigned long)&CLA_ADC_A1_ISR;

    Cla1Regs.MVECT1 = (int)claIsr;
    DmaClaSrcSelRegs.CLA1TASKSRCSEL1.bit.TASK1 = 1;//11;

    Cla1Regs.MIER.bit.INT1 = 1;                         // Enable CLA task1
    EDIS;
}
/***************************************************************************************************
Function Name:
    void Setup_Dma(void)
Input:
    NULL
Output:
    NULL
Comment:
    Setup DMA
    Ch1 -
    Ch2 -
    Ch3 - CAN receive
    Ch4 - UART receive
    Ch5 - CAN transmit
    Ch6 - UART transmit
***************************************************************************************************/
void Setup_Dma(void)
{
    EALLOW;
    DmaRegs.DMACTRL.bit.HARDRESET = 1;
    asm(" NOP"); // one NOP required after HARDRESET
    DmaRegs.DEBUGCTRL.bit.FREE = 0;
    EDIS;
}
/***************************************************************************************************
Function Name:
    void Setup_Timer(void)
Input:
    NULL
Output:
    NULL
Comment:
    Setup CPU timer
    Timer1 - For fixed frequency control loop(50kHz)
***************************************************************************************************/
void Setup_Timer(void)
{
    CpuTimer1Regs.TIM.all = 0;
    CpuTimer1Regs.PRD.all = (unsigned long)(CPU_CLK / CONTROLLER_FREQ) - 1;
    CpuTimer1Regs.TPR.all = 0;
    CpuTimer1Regs.TPRH.all = 0;
    CpuTimer1Regs.TCR.all = 0xC000;

    CpuTimer2Regs.TPR.bit.TDDR = 0;     // Timer period = (TDDR + 1) * LSW / 100MHz
    CpuTimer2Regs.PRD.all = (unsigned long)(CPU_CLK / BACKGROUND_FREQ) - 1;     // = 0.5ms
    CpuTimer2Regs.TCR.bit.TIE = 1;
}
/***************************************************************************************************
Function Name:
    void Setup_Gpio(void)
Input:
    NULL
Output:
    NULL
Comment:
    Setup IO pins
***************************************************************************************************/
void Setup_Gpio(void)
{
    EALLOW;
    GpioCtrlRegs.GPACTRL.all = 0x00000000;
    GpioCtrlRegs.GPBCTRL.all = 0x00000000;

    // Analog mode
    GpioCtrlRegs.GPAAMSEL.all = 0x00000000;

    // EPWM
    // GPIO0 for EPWM1_A (PWM BH)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO0 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;      // Disable internal pull-up
    // GPIO1 for EPWM1_B (PWM BL)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO1 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;      // Disable internal pull-up
    // GPIO2 for EPWM2_A (SR CH)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO2 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;      // Disable internal pull-up
    // GPIO3 for EPWM2_B (SR CL)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO3 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO3 = 1;      // Disable internal pull-up
    // GPIO22 for EPWM4_A (PWM AH)
    GpioCtrlRegs.GPAGMUX2.bit.GPIO22 = 3;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 2;
    GpioCtrlRegs.GPAPUD.bit.GPIO22 = 1;      // Disable internal pull-up
    // GPIO7 for EPWM4_B (PWM AL)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO7 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO7 = 1;      // Disable internal pull-up
    // GPIO16 for EPWM5_A (SR BH)
    GpioCtrlRegs.GPAGMUX2.bit.GPIO16 = 1;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 1;      // Disable internal pull-up
    // GPIO17 for EPWM5_B (SR BL)
    GpioCtrlRegs.GPAGMUX2.bit.GPIO17 = 1;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO17 = 1;      // Disable internal pull-up
    // GPIO12 for EPWM7_A (SR AH)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO12 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO12 = 1;      // Disable internal pull-up
    // GPIO13 for EPWM7_B (SR AL)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO13 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO13 = 1;      // Disable internal pull-up
    // GPIO14 for EPWM8_A (PWM CH)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO14 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO14 = 1;      // Disable internal pull-up
    // GPIO15 for EPWM8_B (PWM CL)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO15 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO15 = 1;      // Disable internal pull-up
    // GPIO10 for EPWM6_A (Charger High)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO10 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO10 = 1;      // Disable internal pull-up
    GpioDataRegs.GPACLEAR.bit.GPIO10 = 1;    // Default low
    // GPIO11 for EPWM6_B (Charger Low)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO11 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO11 = 1;      // Disable internal pull-up
    GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;    // Default low

    // UART
    // GPIO48 for SCIA_TX
    GpioCtrlRegs.GPBGMUX2.bit.GPIO48 = 1;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 2;
    GpioCtrlRegs.GPBPUD.bit.GPIO48 = 0;      // Enable internal pull-up
    //GpioCtrlRegs.GPBQSEL2.bit.GPIO48 = 3;

    // GPIO49 for SCIA_RX
    GpioCtrlRegs.GPBGMUX2.bit.GPIO49 = 1;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 2;
    GpioCtrlRegs.GPBPUD.bit.GPIO49 = 0;      // Enable internal pull-up
    GpioCtrlRegs.GPBQSEL2.bit.GPIO49 = 3;

    // I2C
    // GPIO26 I2CA_SDA
    GpioCtrlRegs.GPAGMUX2.bit.GPIO26 = 2;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 3;
    GpioCtrlRegs.GPAPUD.bit.GPIO26 = 0;      // Enable internal pull-up
    GpioCtrlRegs.GPADIR.bit.GPIO26 = 1;
    GpioCtrlRegs.GPAQSEL2.bit.GPIO26 = 3;

    // GPIO27 I2CA_SCL
    GpioCtrlRegs.GPAGMUX2.bit.GPIO27 = 2;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 3;
    GpioCtrlRegs.GPAPUD.bit.GPIO27 = 0;      // Enable internal pull-up
    GpioCtrlRegs.GPADIR.bit.GPIO27 = 0;
    GpioCtrlRegs.GPAQSEL2.bit.GPIO27 = 3;

    // input
    // GPIO4 for DI (GPIO_STP_DISCH_OUT)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO4 = 0;     // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO4 = 1;       // Disable internal pull-up
    GpioCtrlRegs.GPADIR.bit.GPIO4 = 0;

    // GPIO6 for DI (GPIO_FAIL_OUT)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO6 = 0;     // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO6 = 1;       // Disable internal pull-up
    GpioCtrlRegs.GPADIR.bit.GPIO6 = 0;

    // GPIO9 for DI (GPIO_VBATT_CHG_OVP)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO9 = 0;     // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO9 = 1;       // Disable internal pull-up
    GpioCtrlRegs.GPADIR.bit.GPIO9 = 0;

    // GPIO20 for DI (GPIO_CHG_EN_OUT)
    GpioCtrlRegs.GPAGMUX2.bit.GPIO20 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO20 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPADIR.bit.GPIO20 = 0;

    // GPIO21 for DI (GPIO_SOH_OUT)
    GpioCtrlRegs.GPAGMUX2.bit.GPIO21 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO21 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPADIR.bit.GPIO21 = 0;

    // GPIO23 for DI (GPIO_E_STOP2_L)
    GpioCtrlRegs.GPAGMUX2.bit.GPIO23 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO23 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPADIR.bit.GPIO23 = 0;

    // GPIO25 for DI (GPIO_AC_LOSS_L)
    GpioCtrlRegs.GPAGMUX2.bit.GPIO25 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO25 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPADIR.bit.GPIO25 = 0;

    // GPIO29 for DI (GPIO_RESET)
    GpioCtrlRegs.GPAGMUX2.bit.GPIO29 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO29 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPADIR.bit.GPIO29 = 0;

    // GPIO31 for DI (GPIO_O/P_OVP)
    GpioCtrlRegs.GPAGMUX2.bit.GPIO31 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO31 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 0;
    GpioCtrlRegs.GPAQSEL2.bit.GPIO31 = 1;    // 3-sample qualification

    // GPIO41 for DI (GPIO_E_STOP1_L)
    GpioCtrlRegs.GPBGMUX1.bit.GPIO41 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO41 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO41 = 0;

    // GPIO47 for DI (GPIO_SYNC_START_DI)
    GpioCtrlRegs.GPBGMUX1.bit.GPIO47 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX1.bit.GPIO47 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO47 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO47 = 0;

    // GPIO55 for DI (GPIO_VBATT_OVP)
    GpioCtrlRegs.GPBGMUX2.bit.GPIO55 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO55 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO55 = 0;

    // GPIO60 for DI (GPIO_BBKILL)
    GpioCtrlRegs.GPBGMUX2.bit.GPIO60 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO60 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO60 = 0;

    // output
    // GPIO5 for DO (GPIO_FAULT_SHDN_L)
    GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;     // Default low
    GpioCtrlRegs.GPAGMUX1.bit.GPIO5 = 0;     // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO5 = 1;       // Disable internal pull-up
    GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;

    // GPIO8 for DO (GPIO_WC_PROTECT)
    GpioDataRegs.GPASET.bit.GPIO8 = 1;       // Default high
    GpioCtrlRegs.GPAGMUX1.bit.GPIO8 = 0;     // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO8 = 1;       // Disable internal pull-up
    GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;

    // GPIO28 for DO (GPIO_RLY_CTRL_PRECHG_1)
    GpioDataRegs.GPACLEAR.bit.GPIO28 = 1;    // Default low
    GpioCtrlRegs.GPAGMUX2.bit.GPIO28 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO28 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPADIR.bit.GPIO28 = 1;

    // GPIO30 for DO (GPIO_DD_FAULT_L)
    GpioDataRegs.GPACLEAR.bit.GPIO30 = 1;    // Default low
    GpioCtrlRegs.GPAGMUX2.bit.GPIO30 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO30 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPADIR.bit.GPIO30 = 1;

    // GPIO33 for DO (GPIO_OP_DISCH2)
    GpioDataRegs.GPBSET.bit.GPIO33 = 1;      // Default high
    GpioCtrlRegs.GPBGMUX1.bit.GPIO33 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO33 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO33 = 1;

    // GPIO34 for DO (GPIO_DISLAT_VO_OVP)
    GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;    // Default low
    GpioCtrlRegs.GPBGMUX1.bit.GPIO34 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO34 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;

    // GPIO40 for DO (GPIO_EN_CHG)
    GpioDataRegs.GPBSET.bit.GPIO40 = 1;      // Default high
    GpioCtrlRegs.GPBGMUX1.bit.GPIO40 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO40 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO40 = 1;

    // GPIO44 for DO (GPIO_EN_D2D)
    GpioDataRegs.GPBSET.bit.GPIO44 = 1;      // Default high
    GpioCtrlRegs.GPBGMUX1.bit.GPIO44 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX1.bit.GPIO44 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO44 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO44 = 1;

    // GPIO50 for DO (GPIO_SYNC_START_DO)
    GpioDataRegs.GPBCLEAR.bit.GPIO50 = 1;    // Default low
    GpioCtrlRegs.GPBGMUX2.bit.GPIO50 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX2.bit.GPIO50 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO50 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO50 = 1;

    // GPIO51 for DO (GPIO_RLY_CTRL_BATT)
    GpioDataRegs.GPBCLEAR.bit.GPIO51 = 1;    // Default low
    GpioCtrlRegs.GPBGMUX2.bit.GPIO51 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO51 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO51 = 1;

    // GPIO52 for DO (GPIO_RLY_CTRL_PRECHG_2)
    GpioDataRegs.GPBCLEAR.bit.GPIO52 = 1;    // Default low
    GpioCtrlRegs.GPBGMUX2.bit.GPIO52 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX2.bit.GPIO52 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO52 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO52 = 1;

    // GPIO53 for DO (GPIO_EN_ORING)
    GpioDataRegs.GPBSET.bit.GPIO53 = 1;      // Default high
    GpioCtrlRegs.GPBGMUX2.bit.GPIO53 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX2.bit.GPIO53 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO53 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO53 = 1;

    // GPIO54 for DO (GPIO_RLY_CTRL_AUX)
    GpioDataRegs.GPBCLEAR.bit.GPIO54 = 1;    // Default low
    GpioCtrlRegs.GPBGMUX2.bit.GPIO54 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO54 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO54 = 1;

    // GPIO56 for DO (GPIO_OP_DISCH1)
    GpioDataRegs.GPBSET.bit.GPIO56 = 1;      // Default high
    GpioCtrlRegs.GPBGMUX2.bit.GPIO56 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO56 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO56 = 1;

    // GPIO57 for DO (GPIO_EN_SR)
    GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1;    // Default low
    GpioCtrlRegs.GPBGMUX2.bit.GPIO57 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO57 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO57 = 1;

    // GPIO58 for DO (GPIO_DISABLE_IBUS)
    GpioDataRegs.GPBSET.bit.GPIO58 = 1;      // Default high
    GpioCtrlRegs.GPBGMUX2.bit.GPIO58 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO58 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO58 = 1;

    // GPIO59 for DO (GPIO_DISLAT_CHG_OVP)
    GpioDataRegs.GPBCLEAR.bit.GPIO59 = 1;    // Default low
    GpioCtrlRegs.GPBGMUX2.bit.GPIO59 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX2.bit.GPIO59 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO59 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO59 = 1;

    // GPIO61 for DO (GPIO_DISLAT_VBATT_OVP)
    GpioDataRegs.GPBCLEAR.bit.GPIO61 = 1;    // Default low
    GpioCtrlRegs.GPBGMUX2.bit.GPIO61 = 0;    // Muxed pin = GMUXy * 4 + MUXy
    GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO61 = 1;      // Disable internal pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO61 = 1;

    EDIS;
}
/***************************************************************************************************
Function Name:
    void Setup_Adc(void)
Input:
    NULL
Output:
    NULL
Comment:
    Setup ADC
***************************************************************************************************/
void Setup_Adc(void)
{
/*    unsigned short *offset;
    EALLOW;
    offset = (unsigned short *)(ADC_OFFSET_TRIM(0));
    AdcaRegs.ADCOFFTRIM.bit.OFFTRIM = (*offset) >> 8;
    AnalogSubsysRegs.ANAREFCTL.bit.ANAREFASEL = 0;
    AnalogSubsysRegs.ANAREFCTL.bit.ANAREFA2P5SEL = 0;

    offset = (unsigned short *)(ADC_OFFSET_TRIM(1));
    AdcbRegs.ADCOFFTRIM.bit.OFFTRIM = (*offset) >> 8;
    AnalogSubsysRegs.ANAREFCTL.bit.ANAREFBSEL = 0;
    AnalogSubsysRegs.ANAREFCTL.bit.ANAREFB2P5SEL = 0;

    offset = (unsigned short *)(ADC_OFFSET_TRIM(2));
    AdccRegs.ADCOFFTRIM.bit.OFFTRIM = (*offset) >> 8;
    AnalogSubsysRegs.ANAREFCTL.bit.ANAREFCSEL = 0;
    AnalogSubsysRegs.ANAREFCTL.bit.ANAREFC2P5SEL = 0;
    EDIS;
*/
    EALLOW;
    AnalogSubsysRegs.ANAREFCTL.bit.ANAREFSEL = 1;      // Analog reference A uses external reference mode.
    EDIS;

    EALLOW;
    // ADCCLK = SYSCLK / 2 = 60MHz
    AdcaRegs.ADCCTL2.bit.PRESCALE = 2;
    AdcbRegs.ADCCTL2.bit.PRESCALE = 2;
    AdccRegs.ADCCTL2.bit.PRESCALE = 2;
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdcbRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdccRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    AdcbRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    AdccRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    EDIS;
    DELAY_US(1000);

    //                         Trigger                      Channel                       ACQPS
    EALLOW;
    AdcaRegs.ADCSOC0CTL.all  = ((unsigned long)(9) << 20) | ((unsigned long)(9) << 15)  | 19; //ADC_IO
    AdcaRegs.ADCSOC1CTL.all  = ((unsigned long)(9) << 20) | ((unsigned long)(4) << 15)  | 19; //ADC_VCS
    AdcaRegs.ADCSOC2CTL.all  = ((unsigned long)(9) << 20) | ((unsigned long)(6) << 15)  | 19; //ADC_I_PRI_PEAK
    AdcaRegs.ADCSOC3CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(3) << 15)  | 9;
    AdcaRegs.ADCSOC4CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(5) << 15)  | 9;
    AdcaRegs.ADCSOC5CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(2) << 15)  | 9;
    AdcaRegs.ADCSOC6CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(0) << 15)  | 9;
    AdcaRegs.ADCSOC7CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(7) << 15)  | 9;
    AdcaRegs.ADCSOC8CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(1) << 15)  | 9;
    AdcaRegs.ADCSOC9CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(9) << 15)  | 9;
    AdcaRegs.ADCSOC10CTL.all = ((unsigned long)(0) << 20) | ((unsigned long)(10) << 15) | 9;
    AdcaRegs.ADCSOC11CTL.all = ((unsigned long)(0) << 20) | ((unsigned long)(11) << 15) | 9;
    AdcaRegs.ADCSOC12CTL.all = ((unsigned long)(0) << 20) | ((unsigned long)(12) << 15) | 9;
    AdcaRegs.ADCSOC13CTL.all = ((unsigned long)(0) << 20) | ((unsigned long)(13) << 15) | 9;
    AdcaRegs.ADCSOC14CTL.all = ((unsigned long)(0) << 20) | ((unsigned long)(14) << 15) | 9;
    AdcaRegs.ADCSOC15CTL.all = ((unsigned long)(0) << 20) | ((unsigned long)(15) << 15) | 9;
    EDIS;

    //                         Trigger                      Channel                       ACQPS
    EALLOW;
    AdcbRegs.ADCSOC0CTL.all  = ((unsigned long)(9) << 20) | ((unsigned long)(6) << 15)  | 19; //ADC_IO_READING
    AdcbRegs.ADCSOC1CTL.all  = ((unsigned long)(9) << 20) | ((unsigned long)(0) << 15)  | 19; //ADC_VO_COMP
    AdcbRegs.ADCSOC2CTL.all  = ((unsigned long)(9) << 20) | ((unsigned long)(3) << 15)  | 19; //ADC_VBATT
    AdcbRegs.ADCSOC3CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(4) << 15)  | 9;
    AdcbRegs.ADCSOC4CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(2) << 15)  | 9;
    AdcbRegs.ADCSOC5CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(5) << 15)  | 9;
    AdcbRegs.ADCSOC6CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(1) << 15)  | 9;
    AdcbRegs.ADCSOC7CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(7) << 15)  | 9;
    AdcbRegs.ADCSOC8CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(8) << 15)  | 9;
    AdcbRegs.ADCSOC9CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(9) << 15)  | 9;
    AdcbRegs.ADCSOC10CTL.all = ((unsigned long)(0) << 20) | ((unsigned long)(10) << 15) | 9;
    AdcbRegs.ADCSOC11CTL.all = ((unsigned long)(0) << 20) | ((unsigned long)(11) << 15) | 9;
    AdcbRegs.ADCSOC12CTL.all = ((unsigned long)(0) << 20) | ((unsigned long)(12) << 15) | 9;
    AdcbRegs.ADCSOC13CTL.all = ((unsigned long)(0) << 20) | ((unsigned long)(13) << 15) | 9;
    AdcbRegs.ADCSOC14CTL.all = ((unsigned long)(0) << 20) | ((unsigned long)(14) << 15) | 9;
    AdcbRegs.ADCSOC15CTL.all = ((unsigned long)(0) << 20) | ((unsigned long)(15) << 15) | 9;
    EDIS;

    //                         Trigger                      Channel                       ACQPS
    EALLOW;
    AdccRegs.ADCSOC0CTL.all  = ((unsigned long)(9) << 20) | ((unsigned long)(3) << 15)  | 19; //ADC_VO_ORING
    AdccRegs.ADCSOC1CTL.all  = ((unsigned long)(9) << 20) | ((unsigned long)(5) << 15)  | 19; //ADC_VBATT_CHG
    AdccRegs.ADCSOC2CTL.all  = ((unsigned long)(9) << 20) | ((unsigned long)(6) << 15)  | 19; //ADC_ICHG
    AdccRegs.ADCSOC3CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(0) << 15)  | 9;
    AdccRegs.ADCSOC4CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(4) << 15)  | 9;
    AdccRegs.ADCSOC5CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(2) << 15)  | 9;
    AdccRegs.ADCSOC6CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(1) << 15)  | 9;
    AdccRegs.ADCSOC7CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(7) << 15)  | 9;
    AdccRegs.ADCSOC8CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(8) << 15)  | 9;
    AdccRegs.ADCSOC9CTL.all  = ((unsigned long)(0) << 20) | ((unsigned long)(9) << 15)  | 9;
    AdccRegs.ADCSOC10CTL.all = ((unsigned long)(0) << 20) | ((unsigned long)(10) << 15) | 9;
    AdccRegs.ADCSOC11CTL.all = ((unsigned long)(0) << 20) | ((unsigned long)(11) << 15) | 9;
    AdccRegs.ADCSOC12CTL.all = ((unsigned long)(0) << 20) | ((unsigned long)(12) << 15) | 9;
    AdccRegs.ADCSOC13CTL.all = ((unsigned long)(0) << 20) | ((unsigned long)(13) << 15) | 9;
    AdccRegs.ADCSOC14CTL.all = ((unsigned long)(0) << 20) | ((unsigned long)(14) << 15) | 9;
    AdccRegs.ADCSOC15CTL.all = ((unsigned long)(0) << 20) | ((unsigned long)(15) << 15) | 9;
    EDIS;

    EALLOW;
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 2; // End of SOC0 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   // Enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; // Make sure INT1 flag is cleared
    EDIS;
}
/***************************************************************************************************
Function Name:
    void Setup_Comparator(void)
Input:
    NULL
Output:
    NULL
Comment:
    Setup Comparator
***************************************************************************************************/
void Setup_Comparator(void)
{
    // Comparator 2H  A9
    // positive = Output current, negative = DAC
    EALLOW;
    AnalogSubsysRegs.CMPHPMXSEL.bit.CMP2HPMXSEL = 2;    // Choose CMP2_HP2

    Cmpss2Regs.COMPCTL.bit.COMPDACE = 1;
    Cmpss2Regs.COMPCTL.bit.COMPHSOURCE = 0;             // NP = DAC
    Cmpss2Regs.DACHVALS.bit.DACVAL = 3570.84;           //4023.33 // load(96.5A) 96.5*26.4m*(4095/3.3)+409.5(offset) = 3570.84

    Cmpss2Regs.CTRIPHFILCLKCTL = 10;
    Cmpss2Regs.CTRIPHFILCTL.bit.SAMPWIN = 10;
    Cmpss2Regs.CTRIPHFILCTL.bit.THRESH = 6;
    Cmpss2Regs.CTRIPHFILCTL.bit.FILINIT = 1;

    Cmpss2Regs.COMPSTSCLR.all = 0x0202;
    // Select CMPSS output source
    Cmpss2Regs.COMPCTL.bit.CTRIPHSEL = 0x3;             // Select latched output*/
    EDIS;

    //ready set for ADC_I_PRI_PEAK(A6)
    EALLOW;
    AnalogSubsysRegs.CMPHPMXSEL.bit.CMP1HPMXSEL = 2;    // Choose CMP1_HP2

    Cmpss1Regs.COMPCTL.bit.COMPDACE = 1;
    Cmpss1Regs.COMPCTL.bit.COMPHSOURCE = 0;             // NP = DAC
    Cmpss1Regs.DACHVALS.bit.DACVAL = 3722;              //100A *0.01 * (4095/3.3) = 1241 //200A *0.01 * (4095/3.3) = 2481//300A*0.01*(4095/3.3) = 3722

    Cmpss1Regs.CTRIPHFILCLKCTL = 10;
    Cmpss1Regs.CTRIPHFILCTL.bit.SAMPWIN = 10;
    Cmpss1Regs.CTRIPHFILCTL.bit.THRESH = 6;
    Cmpss1Regs.CTRIPHFILCTL.bit.FILINIT = 1;

    Cmpss1Regs.COMPSTSCLR.all = 0x0202;
    // Select CMPSS output source
    Cmpss1Regs.COMPCTL.bit.CTRIPHSEL = 0x3;             // Select latched output*/
    EDIS;

    // Comparator 2L  B0
    // positive = ADC_VO_COMP, negative = DAC
    EALLOW;
    AnalogSubsysRegs.CMPLPMXSEL.bit.CMP2LPMXSEL = 4;    // Choose CMP2_LP2

    //Cmpss2Regs.COMPCTL.bit.COMPDACE = 1;
    Cmpss2Regs.COMPCTL.bit.COMPLSOURCE = 0;             // NP = DAC
    Cmpss2Regs.DACLVALS.bit.DACVAL = 3944;              // 455V*(2.794/400)*(4095/3.3) = 3944 // 455V*(1.774/400)*(4095/3.3) = 2504

    Cmpss2Regs.CTRIPLFILCLKCTL = 10;
    Cmpss2Regs.CTRIPLFILCTL.bit.SAMPWIN = 10;
    Cmpss2Regs.CTRIPLFILCTL.bit.THRESH = 6;
    Cmpss2Regs.CTRIPLFILCTL.bit.FILINIT = 1;

    Cmpss2Regs.COMPSTSCLR.all = 0x0202;
    // Select CMPSS output source
    Cmpss2Regs.COMPCTL.bit.CTRIPLSEL = 0x3;             // Select latched output*/
    EDIS;
}
/***************************************************************************************************
Function Name:
    void Setup_Pwm(void)
Input:
    NULL
Output:
    NULL
Comment:
    Setup PWM
***************************************************************************************************/
void Setup_Pwm(void)
{
    unsigned long sixPeriod = 0;
    sixPeriod = INIT_SWITCH_PERIOD;
    sixPeriod = (sixPeriod * 5461) >> 15;

    // PWM B - Primary side LLC switch
    EALLOW;
    EPwm1Regs.GLDCTL.bit.OSHTMODE = 1;
    EPwm1Regs.GLDCTL.bit.GLDMODE = CC_CTR_ZERO;
    EPwm1Regs.GLDCTL.bit.GLD = 1;
    EPwm1Regs.GLDCFG.all = 0x00E7;
    EPwm1Regs.EPWMSYNCINSEL.bit.SEL = 0;        // Disabled
    EDIS;

    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    EPwm1Regs.TBCTL.bit.PHSEN = 0;
    EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;
//    EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm1Regs.TBPHS.bit.TBPHS = 0;
    EPwm1Regs.TBPRD = INIT_SWITCH_PERIOD;
    EPwm1Regs.TBCTR = 0;
    EPwm1Regs.EPWMSYNCOUTEN.bit.ZEROEN = 1;     // BCTR = zero event to set SYNCO

    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm1Regs.CMPA.bit.CMPA = sixPeriod;//(INIT_SWITCH_PERIOD >> 1) + (INIT_SWITCH_PERIOD >> 2);
    EPwm1Regs.CMPB.bit.CMPB = INIT_SWITCH_PERIOD - sixPeriod;//INIT_SWITCH_PERIOD - (INIT_SWITCH_PERIOD >> 1) - (INIT_SWITCH_PERIOD >> 2);

    EPwm1Regs.AQCSFRC.bit.CSFA = AQ_CLEAR;
    EPwm1Regs.AQCSFRC.bit.CSFB = AQ_SET;
    EPwm1Regs.AQCTLA.bit.CBD = AQ_SET;
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm1Regs.AQCTLB.bit.CBD = AQ_SET;
    EPwm1Regs.AQCTLB.bit.CAU = AQ_CLEAR;

    EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;  // S1 & S0
    EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;       // S3 & S2
    EPwm1Regs.DBCTL.bit.IN_MODE = DBA_RED_DBB_FED;  // S5 & S4
    EPwm1Regs.DBCTL.bit.OUTSWAP = 0x0;              // S7 & S6
    EPwm1Regs.DBCTL.bit.DEDB_MODE = 0;              // S8
    EPwm1Regs.DBCTL.bit.LOADREDMODE = CC_CTR_ZERO;
    EPwm1Regs.DBCTL.bit.LOADFEDMODE = CC_CTR_ZERO;
    EPwm1Regs.DBCTL.bit.SHDWDBREDMODE = 1;
    EPwm1Regs.DBCTL.bit.SHDWDBFEDMODE = 1;
    EPwm1Regs.DBCTL.bit.HALFCYCLE = 0;
    EPwm1Regs.DBRED.bit.DBRED = INIT_PRI_SWITCH_DB;
    EPwm1Regs.DBFED.bit.DBFED = INIT_PRI_SWITCH_DB;

//    // Event trigger for SOCA
//    EPwm1Regs.ETSEL.bit.SOCASEL = ET_CTRU_CMPA;
//    EPwm1Regs.ETSEL.bit.SOCAEN = 1;
//    EPwm1Regs.ETPS.bit.SOCAPRD = ET_1ST;            // pulse on first event

    // PWM C - Primary side LLC switch
    EALLOW;
    EPwm8Regs.GLDCTL.bit.OSHTMODE = 1;
    EPwm8Regs.GLDCTL.bit.GLDMODE = CC_CTR_ZERO;
    EPwm8Regs.GLDCTL.bit.GLD = 1;
    EPwm8Regs.GLDCFG.all = 0x00E7;
    EPwm8Regs.EPWMSYNCINSEL.bit.SEL = 1;    // EPWM1.SYNCOUT for SYNCI
    EDIS;

    EPwm8Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    EPwm8Regs.TBCTL.bit.PHSEN = 1;
    EPwm8Regs.TBCTL.bit.PRDLD = TB_SHADOW;
//    EPwm8Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
    EPwm8Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm8Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm8Regs.TBPHS.bit.TBPHS = 2;
    EPwm8Regs.TBCTL.bit.PHSDIR = 1;         //count up after the synchronization event
    EPwm8Regs.TBPRD = INIT_SWITCH_PERIOD;
    EPwm8Regs.TBCTR = 0;
    EPwm8Regs.EPWMSYNCOUTEN.all = 0;        // Disabled

    EPwm8Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm8Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
    EPwm8Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm8Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm8Regs.CMPA.bit.CMPA = INIT_SWITCH_PERIOD - sixPeriod;//INIT_SWITCH_PERIOD >> 2;
    EPwm8Regs.CMPB.bit.CMPB = sixPeriod;//INIT_SWITCH_PERIOD - (INIT_SWITCH_PERIOD >> 2);

    EPwm8Regs.AQCSFRC.bit.CSFA = AQ_CLEAR;
    EPwm8Regs.AQCSFRC.bit.CSFB = AQ_SET;
    EPwm8Regs.AQCTLA.bit.CBD = AQ_SET;
    EPwm8Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm8Regs.AQCTLB.bit.CBD = AQ_SET;
    EPwm8Regs.AQCTLB.bit.CAU = AQ_CLEAR;

    EPwm8Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;  // S1 & S0
    EPwm8Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;       // S3 & S2
    EPwm8Regs.DBCTL.bit.IN_MODE = DBA_RED_DBB_FED;  // S5 & S4
    EPwm8Regs.DBCTL.bit.OUTSWAP = 0x0;              // S7 & S6
    EPwm8Regs.DBCTL.bit.DEDB_MODE = 0;              // S8
    EPwm8Regs.DBCTL.bit.LOADREDMODE = CC_CTR_ZERO;
    EPwm8Regs.DBCTL.bit.LOADFEDMODE = CC_CTR_ZERO;
    EPwm8Regs.DBCTL.bit.SHDWDBREDMODE = 1;
    EPwm8Regs.DBCTL.bit.SHDWDBFEDMODE = 1;
    EPwm8Regs.DBCTL.bit.HALFCYCLE = 0;
    EPwm8Regs.DBRED.bit.DBRED = INIT_PRI_SWITCH_DB;
    EPwm8Regs.DBFED.bit.DBFED = INIT_PRI_SWITCH_DB;
    // Link the GLDCTL2 together with PWM1
    EPwm8Regs.EPWMXLINK.bit.GLDCTL2LINK = 0x0;  //EPWM1
    EPwm8Regs.EPWMXLINK.bit.TBPRDLINK = 0x0;    //EPMW1

    // PWM A - Primary side LLC switch
    EALLOW;
    EPwm4Regs.GLDCTL.bit.OSHTMODE = 1;
    EPwm4Regs.GLDCTL.bit.GLDMODE = CC_CTR_ZERO;
    EPwm4Regs.GLDCTL.bit.GLD = 1;
    EPwm4Regs.GLDCFG.all = 0x00E7;
//    SyncSocRegs.SYNCSELECT.bit.EPWM4SYNCIN = 0;
    EPwm4Regs.EPWMSYNCINSEL.bit.SEL = 1;    // EPWM1.SYNCOUT for SYNCI
    EDIS;

    EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    EPwm4Regs.TBCTL.bit.PHSEN = 1;
    EPwm4Regs.TBCTL.bit.PRDLD = TB_SHADOW;
//    EPwm4Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
    EPwm4Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm4Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm4Regs.TBPHS.bit.TBPHS = 2;
    EPwm4Regs.TBCTL.bit.PHSDIR = 1;         //count up after the synchronization event
    EPwm4Regs.TBPRD = INIT_SWITCH_PERIOD;
    EPwm4Regs.TBCTR = 0;
    EPwm4Regs.EPWMSYNCOUTEN.all = 0;        // Disabled

    EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
    EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm4Regs.CMPA.bit.CMPA = INIT_SWITCH_PERIOD >> 1;//(INIT_SWITCH_PERIOD >> 1) + (INIT_SWITCH_PERIOD >> 2);
    EPwm4Regs.CMPB.bit.CMPB = INIT_SWITCH_PERIOD >> 1;//INIT_SWITCH_PERIOD - (INIT_SWITCH_PERIOD >> 1) - (INIT_SWITCH_PERIOD >> 2);

    EPwm4Regs.AQCSFRC.bit.CSFA = AQ_CLEAR;
    EPwm4Regs.AQCSFRC.bit.CSFB = AQ_SET;
    EPwm4Regs.AQCTLA.bit.CBD = AQ_CLEAR;//AQ_SET
    EPwm4Regs.AQCTLA.bit.CAU = AQ_SET;//AQ_CLEAR;
    EPwm4Regs.AQCTLB.bit.CBD = AQ_CLEAR;//AQ_SET;
    EPwm4Regs.AQCTLB.bit.CAU = AQ_SET;//AQ_CLEAR;

    EPwm4Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;  // S1 & S0
    EPwm4Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;       // S3 & S2
    EPwm4Regs.DBCTL.bit.IN_MODE = DBA_RED_DBB_FED;  // S5 & S4
    EPwm4Regs.DBCTL.bit.OUTSWAP = 0x0;              // S7 & S6
    EPwm4Regs.DBCTL.bit.DEDB_MODE = 0;              // S8
    EPwm4Regs.DBCTL.bit.LOADREDMODE = CC_CTR_ZERO;
    EPwm4Regs.DBCTL.bit.LOADFEDMODE = CC_CTR_ZERO;
    EPwm4Regs.DBCTL.bit.SHDWDBREDMODE = 1;
    EPwm4Regs.DBCTL.bit.SHDWDBFEDMODE = 1;
    EPwm4Regs.DBCTL.bit.HALFCYCLE = 0;
    EPwm4Regs.DBRED.bit.DBRED = INIT_PRI_SWITCH_DB;
    EPwm4Regs.DBFED.bit.DBFED = INIT_PRI_SWITCH_DB;
    // Link the GLDCTL2 together with PWM1
    EPwm4Regs.EPWMXLINK.bit.GLDCTL2LINK = 0x0;
    EPwm4Regs.EPWMXLINK.bit.TBPRDLINK = 0x0;
/////
    // SR B - Secondary side LLC switch
    EALLOW;
    EPwm2Regs.GLDCTL.bit.OSHTMODE = 1;
    EPwm2Regs.GLDCTL.bit.GLDMODE = CC_CTR_ZERO;
    EPwm2Regs.GLDCTL.bit.GLD = 1;
    EPwm2Regs.GLDCFG.all = 0x00E7;
    EPwm2Regs.EPWMSYNCINSEL.bit.SEL = 1;    // EPWM1.SYNCOUT for SYNCI
    EDIS;

    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    EPwm2Regs.TBCTL.bit.PHSEN = 1;
    EPwm2Regs.TBCTL.bit.PRDLD = TB_SHADOW;
//    EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm2Regs.TBPHS.bit.TBPHS = 2;
    EPwm2Regs.TBCTL.bit.PHSDIR = 1;         //count up after the synchronization event
    EPwm2Regs.TBPRD = INIT_SWITCH_PERIOD;
    EPwm2Regs.TBCTR = 0;
    EPwm2Regs.EPWMSYNCOUTEN.all = 0;        // Disabled

    EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
    EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm2Regs.CMPA.bit.CMPA = sixPeriod;//(INIT_SWITCH_PERIOD >> 1) + (INIT_SWITCH_PERIOD >> 2);
    EPwm2Regs.CMPB.bit.CMPB = INIT_SWITCH_PERIOD - sixPeriod;//INIT_SWITCH_PERIOD - (INIT_SWITCH_PERIOD >> 1) - (INIT_SWITCH_PERIOD >> 2);
    EPwm2Regs.AQCSFRC.bit.CSFA = AQ_CLEAR;
    EPwm2Regs.AQCSFRC.bit.CSFB = AQ_SET;
    EPwm2Regs.AQCTLA.bit.CBD = AQ_SET;
    EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm2Regs.AQCTLB.bit.CBD = AQ_SET;
    EPwm2Regs.AQCTLB.bit.CAU = AQ_CLEAR;

    EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;  // S1 & S0
    EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;       // S3 & S2
    EPwm2Regs.DBCTL.bit.IN_MODE = DBA_RED_DBB_FED;  // S5 & S4
    EPwm2Regs.DBCTL.bit.OUTSWAP = 0x0;              // S7 & S6
    EPwm2Regs.DBCTL.bit.DEDB_MODE = 0;              // S8
    EPwm2Regs.DBCTL.bit.LOADREDMODE = CC_CTR_ZERO;
    EPwm2Regs.DBCTL.bit.LOADFEDMODE = CC_CTR_ZERO;
    EPwm2Regs.DBCTL.bit.SHDWDBREDMODE = 1;
    EPwm2Regs.DBCTL.bit.SHDWDBFEDMODE = 1;
    EPwm2Regs.DBCTL.bit.HALFCYCLE = 0;
    EPwm2Regs.DBRED.bit.DBRED = INIT_PRI_SWITCH_DB;
    EPwm2Regs.DBFED.bit.DBFED = INIT_PRI_SWITCH_DB;
    // Link the GLDCTL2 together with PWM1
    EPwm2Regs.EPWMXLINK.bit.GLDCTL2LINK = 0x0;  //EPWM1
    EPwm2Regs.EPWMXLINK.bit.TBPRDLINK = 0x0;    //EPMW1

    // SR C - Secondary side LLC switch
    EALLOW;
    EPwm7Regs.GLDCTL.bit.OSHTMODE = 1;
    EPwm7Regs.GLDCTL.bit.GLDMODE = CC_CTR_ZERO;
    EPwm7Regs.GLDCTL.bit.GLD = 1;
    EPwm7Regs.GLDCFG.all = 0x00E7;
    EPwm7Regs.EPWMSYNCINSEL.bit.SEL = 1;    // EPWM1.SYNCOUT for SYNCI
    EDIS;

    EPwm7Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    EPwm7Regs.TBCTL.bit.PHSEN = 1;
    EPwm7Regs.TBCTL.bit.PRDLD = TB_SHADOW;
//    EPwm7Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
    EPwm7Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm7Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm7Regs.TBPHS.bit.TBPHS = 2;
    EPwm7Regs.TBCTL.bit.PHSDIR = 1;         //count up after the synchronization event
    EPwm7Regs.TBPRD = INIT_SWITCH_PERIOD;
    EPwm7Regs.TBCTR = 0;
    EPwm7Regs.EPWMSYNCOUTEN.all = 0;        // Disabled

    EPwm7Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm7Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
    EPwm7Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm7Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm7Regs.CMPA.bit.CMPA = INIT_SWITCH_PERIOD - sixPeriod;//INIT_SWITCH_PERIOD >> 2;
    EPwm7Regs.CMPB.bit.CMPB = sixPeriod;//INIT_SWITCH_PERIOD - (INIT_SWITCH_PERIOD >> 2);

    EPwm7Regs.AQCSFRC.bit.CSFA = AQ_CLEAR;
    EPwm7Regs.AQCSFRC.bit.CSFB = AQ_SET;
    EPwm7Regs.AQCTLA.bit.CBD = AQ_SET;
    EPwm7Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm7Regs.AQCTLB.bit.CBD = AQ_SET;
    EPwm7Regs.AQCTLB.bit.CAU = AQ_CLEAR;

    EPwm7Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;  // S1 & S0
    EPwm7Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;       // S3 & S2
    EPwm7Regs.DBCTL.bit.IN_MODE = DBA_RED_DBB_FED;  // S5 & S4
    EPwm7Regs.DBCTL.bit.OUTSWAP = 0x0;              // S7 & S6
    EPwm7Regs.DBCTL.bit.DEDB_MODE = 0;              // S8
    EPwm7Regs.DBCTL.bit.LOADREDMODE = CC_CTR_ZERO;
    EPwm7Regs.DBCTL.bit.LOADFEDMODE = CC_CTR_ZERO;
    EPwm7Regs.DBCTL.bit.SHDWDBREDMODE = 1;
    EPwm7Regs.DBCTL.bit.SHDWDBFEDMODE = 1;
    EPwm7Regs.DBCTL.bit.HALFCYCLE = 0;
    EPwm7Regs.DBRED.bit.DBRED = INIT_PRI_SWITCH_DB;
    EPwm7Regs.DBFED.bit.DBFED = INIT_PRI_SWITCH_DB;
    // Link the GLDCTL2 together with PWM1
    EPwm7Regs.EPWMXLINK.bit.GLDCTL2LINK = 0x0;  //EPWM1
    EPwm7Regs.EPWMXLINK.bit.TBPRDLINK = 0x0;    //EPMW1

    // SR A - Secondary side LLC switch
    EALLOW;
    EPwm5Regs.GLDCTL.bit.OSHTMODE = 1;
    EPwm5Regs.GLDCTL.bit.GLDMODE = CC_CTR_ZERO;
    EPwm5Regs.GLDCTL.bit.GLD = 1;
    EPwm5Regs.GLDCFG.all = 0x00E7;
//    SyncSocRegs.SYNCSELECT.bit.EPWM4SYNCIN = 0;
    EPwm5Regs.EPWMSYNCINSEL.bit.SEL = 1;    // EPWM1.SYNCOUT for SYNCI
    EDIS;

    EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    EPwm5Regs.TBCTL.bit.PHSEN = 1;
    EPwm5Regs.TBCTL.bit.PRDLD = TB_SHADOW;
//    EPwm5Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
    EPwm5Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm5Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm5Regs.TBPHS.bit.TBPHS = 2;
    EPwm5Regs.TBCTL.bit.PHSDIR = 1;         //count up after the synchronization event
    EPwm5Regs.TBPRD = INIT_SWITCH_PERIOD;
    EPwm5Regs.TBCTR = 0;
    EPwm5Regs.EPWMSYNCOUTEN.all = 0;        // Disabled

    EPwm5Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm5Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
    EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm5Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm5Regs.CMPA.bit.CMPA = INIT_SWITCH_PERIOD >> 1;//(INIT_SWITCH_PERIOD >> 1) + (INIT_SWITCH_PERIOD >> 2);
    EPwm5Regs.CMPB.bit.CMPB = INIT_SWITCH_PERIOD >> 1;//INIT_SWITCH_PERIOD - (INIT_SWITCH_PERIOD >> 1) - (INIT_SWITCH_PERIOD >> 2);

    EPwm5Regs.AQCSFRC.bit.CSFA = AQ_CLEAR;
    EPwm5Regs.AQCSFRC.bit.CSFB = AQ_SET;
    EPwm5Regs.AQCTLA.bit.CBD = AQ_CLEAR;//AQ_SET
    EPwm5Regs.AQCTLA.bit.CAU = AQ_SET;//AQ_CLEAR;
    EPwm5Regs.AQCTLB.bit.CBD = AQ_CLEAR;//AQ_SET;
    EPwm5Regs.AQCTLB.bit.CAU = AQ_SET;//AQ_CLEAR;

    EPwm5Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;  // S1 & S0
    EPwm5Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;       // S3 & S2
    EPwm5Regs.DBCTL.bit.IN_MODE = DBA_RED_DBB_FED;  // S5 & S4
    EPwm5Regs.DBCTL.bit.OUTSWAP = 0x0;              // S7 & S6
    EPwm5Regs.DBCTL.bit.DEDB_MODE = 0;              // S8
    EPwm5Regs.DBCTL.bit.LOADREDMODE = CC_CTR_ZERO;
    EPwm5Regs.DBCTL.bit.LOADFEDMODE = CC_CTR_ZERO;
    EPwm5Regs.DBCTL.bit.SHDWDBREDMODE = 1;
    EPwm5Regs.DBCTL.bit.SHDWDBFEDMODE = 1;
    EPwm5Regs.DBCTL.bit.HALFCYCLE = 0;
    EPwm5Regs.DBRED.bit.DBRED = INIT_PRI_SWITCH_DB;
    EPwm5Regs.DBFED.bit.DBFED = INIT_PRI_SWITCH_DB;
    // Link the GLDCTL2 together with PWM1
    EPwm5Regs.EPWMXLINK.bit.GLDCTL2LINK = 0x0;
    EPwm5Regs.EPWMXLINK.bit.TBPRDLINK = 0x0;
/////
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EPwm1Regs.GLDCTL2.bit.OSHTLD = 1;
    EPwm1Regs.GLDCFG.all = 0x04E7;
    EPwm8Regs.GLDCFG.all = 0x04E7;
    EPwm4Regs.GLDCFG.all = 0x04E7;
    EPwm2Regs.GLDCFG.all = 0x04E7;
    EPwm7Regs.GLDCFG.all = 0x04E7;
    EPwm5Regs.GLDCFG.all = 0x04E7;
    EDIS;

    // Protection
    EALLOW;
    // TZA & TZB pulls low, ignore rest TZ sources

    EPwm1Regs.TZCTL.all = 0x0FFF;
    EPwm8Regs.TZCTL.all = 0x0FFF;
    EPwm4Regs.TZCTL.all = 0x0FFF;

    EPwm2Regs.TZCTL.all = 0x0FFF;
    EPwm7Regs.TZCTL.all = 0x0FFF;
    EPwm5Regs.TZCTL.all = 0x0FFF;

    // Input X-BAR
    // Discharger input OVP
    // GPIO55 -> TRIP6 -> DCBL -> DCBEVT2 -> CBC
    InputXbarRegs.INPUT6SELECT = 55;
    EPwm1Regs.DCTRIPSEL.bit.DCBLCOMPSEL = DC_TRIPIN6;
    EPwm1Regs.TZDCSEL.bit.DCBEVT2 = TZ_DCBL_HI;
    EPwm1Regs.DCBCTL.bit.EVT2SRCSEL = DC_EVT2;
    EPwm1Regs.TZSEL.bit.DCBEVT2 = 1;
    EPwm8Regs.DCTRIPSEL.bit.DCBLCOMPSEL = DC_TRIPIN6;
    EPwm8Regs.TZDCSEL.bit.DCBEVT2 = TZ_DCBL_HI;
    EPwm8Regs.DCBCTL.bit.EVT2SRCSEL = DC_EVT2;
    EPwm8Regs.TZSEL.bit.DCBEVT2 = 1;
    EPwm4Regs.DCTRIPSEL.bit.DCBLCOMPSEL = DC_TRIPIN6;
    EPwm4Regs.TZDCSEL.bit.DCBEVT2 = TZ_DCBL_HI;
    EPwm4Regs.DCBCTL.bit.EVT2SRCSEL = DC_EVT2;
    EPwm4Regs.TZSEL.bit.DCBEVT2 = 1;

    EPwm2Regs.DCTRIPSEL.bit.DCBLCOMPSEL = DC_TRIPIN6;
    EPwm2Regs.TZDCSEL.bit.DCBEVT2 = TZ_DCBL_HI;
    EPwm2Regs.DCBCTL.bit.EVT2SRCSEL = DC_EVT2;
    EPwm2Regs.TZSEL.bit.DCBEVT2 = 1;
    EPwm7Regs.DCTRIPSEL.bit.DCBLCOMPSEL = DC_TRIPIN6;
    EPwm7Regs.TZDCSEL.bit.DCBEVT2 = TZ_DCBL_HI;
    EPwm7Regs.DCBCTL.bit.EVT2SRCSEL = DC_EVT2;
    EPwm7Regs.TZSEL.bit.DCBEVT2 = 1;
    EPwm5Regs.DCTRIPSEL.bit.DCBLCOMPSEL = DC_TRIPIN6;
    EPwm5Regs.TZDCSEL.bit.DCBEVT2 = TZ_DCBL_HI;
    EPwm5Regs.DCBCTL.bit.EVT2SRCSEL = DC_EVT2;
    EPwm5Regs.TZSEL.bit.DCBEVT2 = 1;

    // Input X-BAR
    // Discharger output OVP
    // GPIO31 -> TRIP1 -> DCAH -> DCAEVT1 -> OSHT
    InputXbarRegs.INPUT1SELECT = 31;
    EPwm1Regs.DCTRIPSEL.bit.DCAHCOMPSEL = DC_TRIPIN1;
    EPwm1Regs.TZDCSEL.bit.DCAEVT1 = TZ_DCAH_HI;
    EPwm1Regs.DCACTL.bit.EVT1SRCSEL = DC_EVT1;
    EPwm1Regs.TZSEL.bit.DCAEVT1 = 1;
    EPwm8Regs.DCTRIPSEL.bit.DCAHCOMPSEL = DC_TRIPIN1;
    EPwm8Regs.TZDCSEL.bit.DCAEVT1 = TZ_DCAH_HI;
    EPwm8Regs.DCACTL.bit.EVT1SRCSEL = DC_EVT1;
    EPwm8Regs.TZSEL.bit.DCAEVT1 = 1;
    EPwm4Regs.DCTRIPSEL.bit.DCAHCOMPSEL = DC_TRIPIN1;
    EPwm4Regs.TZDCSEL.bit.DCAEVT1 = TZ_DCAH_HI;
    EPwm4Regs.DCACTL.bit.EVT1SRCSEL = DC_EVT1;
    EPwm4Regs.TZSEL.bit.DCAEVT1 = 1;
    EPwm2Regs.DCTRIPSEL.bit.DCAHCOMPSEL = DC_TRIPIN1;
    EPwm2Regs.TZDCSEL.bit.DCAEVT1 = TZ_DCAH_HI;
    EPwm2Regs.DCACTL.bit.EVT1SRCSEL = DC_EVT1;
    EPwm2Regs.TZSEL.bit.DCAEVT1 = 1;
    EPwm7Regs.DCTRIPSEL.bit.DCAHCOMPSEL = DC_TRIPIN1;
    EPwm7Regs.TZDCSEL.bit.DCAEVT1 = TZ_DCAH_HI;
    EPwm7Regs.DCACTL.bit.EVT1SRCSEL = DC_EVT1;
    EPwm7Regs.TZSEL.bit.DCAEVT1 = 1;
    EPwm5Regs.DCTRIPSEL.bit.DCAHCOMPSEL = DC_TRIPIN1;
    EPwm5Regs.TZDCSEL.bit.DCAEVT1 = TZ_DCAH_HI;
    EPwm5Regs.DCACTL.bit.EVT1SRCSEL = DC_EVT1;
    EPwm5Regs.TZSEL.bit.DCAEVT1 = 1;

    //Discharger OCP
    // CMP2H -> TRIP4 -> DCAL -> DCAEVT2 -> CBC
    EPwmXbarRegs.TRIP4MUX0TO15CFG.bit.MUX2 = 0x0;
    EPwmXbarRegs.TRIP4MUXENABLE.bit.MUX2 = 1;
    EPwm1Regs.DCTRIPSEL.bit.DCALCOMPSEL = DC_TRIPIN4;
    EPwm1Regs.TZDCSEL.bit.DCAEVT2 = TZ_DCAL_HI;
    EPwm1Regs.DCACTL.bit.EVT2SRCSEL = DC_EVT2;
    EPwm1Regs.TZSEL.bit.DCAEVT2 = 1;
    EPwm8Regs.DCTRIPSEL.bit.DCALCOMPSEL = DC_TRIPIN4;
    EPwm8Regs.TZDCSEL.bit.DCAEVT2 = TZ_DCAL_HI;
    EPwm8Regs.DCACTL.bit.EVT2SRCSEL = DC_EVT2;
    EPwm8Regs.TZSEL.bit.DCAEVT2 = 1;
    EPwm4Regs.DCTRIPSEL.bit.DCALCOMPSEL = DC_TRIPIN4;
    EPwm4Regs.TZDCSEL.bit.DCAEVT2 = TZ_DCAL_HI;
    EPwm4Regs.DCACTL.bit.EVT2SRCSEL = DC_EVT2;
    EPwm4Regs.TZSEL.bit.DCAEVT2 = 1;

    EPwm2Regs.DCTRIPSEL.bit.DCALCOMPSEL = DC_TRIPIN4;
    EPwm2Regs.TZDCSEL.bit.DCAEVT2 = TZ_DCAL_HI;
    EPwm2Regs.DCACTL.bit.EVT2SRCSEL = DC_EVT2;
    EPwm2Regs.TZSEL.bit.DCAEVT2 = 1;
    EPwm7Regs.DCTRIPSEL.bit.DCALCOMPSEL = DC_TRIPIN4;
    EPwm7Regs.TZDCSEL.bit.DCAEVT2 = TZ_DCAL_HI;
    EPwm7Regs.DCACTL.bit.EVT2SRCSEL = DC_EVT2;
    EPwm7Regs.TZSEL.bit.DCAEVT2 = 1;
    EPwm5Regs.DCTRIPSEL.bit.DCALCOMPSEL = DC_TRIPIN4;
    EPwm5Regs.TZDCSEL.bit.DCAEVT2 = TZ_DCAL_HI;
    EPwm5Regs.DCACTL.bit.EVT2SRCSEL = DC_EVT2;
    EPwm5Regs.TZSEL.bit.DCAEVT2 = 1;

    //Discharger resonant OCP
    // CMP1H -> TRIP5 -> DCBH -> DCBEVT1 -> OSHT
    EPwmXbarRegs.TRIP5MUX0TO15CFG.bit.MUX0 = 0x0;
    EPwmXbarRegs.TRIP5MUXENABLE.bit.MUX0 = 1;
    EPwm1Regs.DCTRIPSEL.bit.DCBHCOMPSEL = DC_TRIPIN5;
    EPwm1Regs.TZDCSEL.bit.DCBEVT1 = TZ_DCBH_HI;
    EPwm1Regs.DCBCTL.bit.EVT1SRCSEL = DC_EVT1;
    EPwm1Regs.TZSEL.bit.DCBEVT1 = 1;
    EPwm8Regs.DCTRIPSEL.bit.DCBHCOMPSEL = DC_TRIPIN5;
    EPwm8Regs.TZDCSEL.bit.DCBEVT1 = TZ_DCBH_HI;
    EPwm8Regs.DCBCTL.bit.EVT1SRCSEL = DC_EVT1;
    EPwm8Regs.TZSEL.bit.DCBEVT1 = 1;
    EPwm4Regs.DCTRIPSEL.bit.DCBHCOMPSEL = DC_TRIPIN5;
    EPwm4Regs.TZDCSEL.bit.DCBEVT1 = TZ_DCBH_HI;
    EPwm4Regs.DCBCTL.bit.EVT1SRCSEL = DC_EVT1;
    EPwm4Regs.TZSEL.bit.DCBEVT1 = 1;

    EPwm2Regs.DCTRIPSEL.bit.DCBHCOMPSEL = DC_TRIPIN5;
    EPwm2Regs.TZDCSEL.bit.DCBEVT1 = TZ_DCBH_HI;
    EPwm2Regs.DCBCTL.bit.EVT1SRCSEL = DC_EVT1;
    EPwm2Regs.TZSEL.bit.DCBEVT1 = 1;
    EPwm7Regs.DCTRIPSEL.bit.DCBHCOMPSEL = DC_TRIPIN5;
    EPwm7Regs.TZDCSEL.bit.DCBEVT1 = TZ_DCBH_HI;
    EPwm7Regs.DCBCTL.bit.EVT1SRCSEL = DC_EVT1;
    EPwm7Regs.TZSEL.bit.DCBEVT1 = 1;
    EPwm5Regs.DCTRIPSEL.bit.DCBHCOMPSEL = DC_TRIPIN5;
    EPwm5Regs.TZDCSEL.bit.DCBEVT1 = TZ_DCBH_HI;
    EPwm5Regs.DCBCTL.bit.EVT1SRCSEL = DC_EVT1;
    EPwm5Regs.TZSEL.bit.DCBEVT1 = 1;

    // Clear all TZ flag
    EPwm1Regs.TZCLR.all = 0x007F;
    EPwm8Regs.TZCLR.all = 0x007F;
//    EPwm3Regs.TZCLR.all = 0x007F;
    EPwm4Regs.TZCLR.all = 0x007F;

    EPwm2Regs.TZCLR.all = 0x007F;
    EPwm7Regs.TZCLR.all = 0x007F;
    EPwm5Regs.TZCLR.all = 0x007F;
    EDIS;

    // Event trigger for SOCA
    //EPwm1Regs.ETSEL.bit.SOCASEL = ET_CTRU_CMPA;
    //EPwm1Regs.ETSEL.bit.SOCAEN = 1;
    //EPwm1Regs.ETPS.bit.SOCAPRD = ET_1ST;            // pulse on first event

    // ePWM6 - Fan control
//    EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
//    EPwm6Regs.TBCTL.bit.PHSEN = TB_DISABLE;
//    EPwm6Regs.TBCTL.bit.PRDLD = TB_SHADOW;
////    EPwm6Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
//    EPwm6Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
//    EPwm6Regs.TBCTL.bit.CLKDIV = TB_DIV1;
//    EPwm6Regs.TBPHS.bit.TBPHS = 0;
//    EPwm6Regs.TBPRD = (unsigned short)(CPU_CLK / 25e3f);
//    EPwm6Regs.TBCTR = 0;
//    EPwm6Regs.EPWMSYNCOUTEN.all = 0;        // Disabled
//
//    EPwm6Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
//    EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
//    EPwm6Regs.CMPA.bit.CMPA = 0;
//    // ePWM6B
//    EPwm6Regs.AQCTLB.bit.ZRO = AQ_SET;
//    EPwm6Regs.AQCTLB.bit.CAU = AQ_CLEAR;

    EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    EPwm6Regs.TBCTL.bit.PHSEN = TB_DISABLE;
    EPwm6Regs.TBCTL.bit.PRDLD = TB_SHADOW;
//    EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
    EPwm6Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm6Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm6Regs.TBPHS.bit.TBPHS = 0;
    EPwm6Regs.TBPRD = (uint16_t)(CPU_CLK / (2*100e3f));
    EPwm6Regs.TBCTR = 0;
    EPwm6Regs.EPWMSYNCOUTEN.all = 0;        // Disabled

    EALLOW;
    EPwm6Regs.EPWMSYNCINSEL.bit.SEL = 1;    // EPWM1.SYNCOUT for SYNCI
    EDIS;

    EPwm6Regs.AQCSFRC.bit.CSFA = AQ_CLEAR;
    EPwm6Regs.AQCSFRC.bit.CSFB = AQ_SET;
    EPwm6Regs.AQCTLA.bit.CAD = AQ_SET;
    EPwm6Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm6Regs.AQCTLB.bit.CAD = AQ_SET;
    EPwm6Regs.AQCTLB.bit.CAU = AQ_CLEAR;

    EPwm6Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;  // S1 & S0
    EPwm6Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;       // S3 & S2
    EPwm6Regs.DBCTL.bit.IN_MODE = DBA_RED_DBB_FED;  // S5 & S4
    EPwm6Regs.DBCTL.bit.OUTSWAP = 0x0;              // S7 & S6
    EPwm6Regs.DBCTL.bit.DEDB_MODE = 0;              // S8
    EPwm6Regs.DBCTL.bit.LOADREDMODE = CC_CTR_ZERO;
    EPwm6Regs.DBCTL.bit.LOADFEDMODE = CC_CTR_ZERO;
    EPwm6Regs.DBCTL.bit.SHDWDBREDMODE = 1;
    EPwm6Regs.DBCTL.bit.SHDWDBFEDMODE = 1;
    EPwm6Regs.DBCTL.bit.HALFCYCLE = 0;

    EPwm6Regs.DBRED.bit.DBRED = 20; //wait for determine DT
    EPwm6Regs.DBFED.bit.DBFED = 20;

    EALLOW;
    InputXbarRegs.INPUT2SELECT = 9;
    EPwm6Regs.DCTRIPSEL.bit.DCALCOMPSEL = DC_TRIPIN2;
    EPwm6Regs.TZDCSEL.bit.DCAEVT1 = TZ_DCAL_HI;
    EPwm6Regs.DCACTL.bit.EVT1SRCSEL = DC_EVT1;
    EPwm6Regs.TZSEL.bit.DCAEVT1 = 1;
    EPwm6Regs.TZCTL.bit.DCAEVT1 = 3;

    EPwmXbarRegs.TRIP7MUX0TO15CFG.bit.MUX3 = 0x0; //CMPSS2L
    EPwmXbarRegs.TRIP7MUXENABLE.bit.MUX3 = 1;
    EPwm6Regs.DCTRIPSEL.bit.DCAHCOMPSEL = DC_TRIPIN7;
    EPwm6Regs.TZDCSEL.bit.DCAEVT2 = TZ_DCAH_HI;
    EPwm6Regs.DCACTL.bit.EVT2SRCSEL = DC_EVT2;
    EPwm6Regs.TZSEL.bit.DCAEVT2 = 1;
    EPwm6Regs.TZCTL.bit.DCAEVT2 = 3;
    EDIS;

    EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;
    EPwm3Regs.TBCTL.bit.PRDLD = TB_SHADOW;
//    EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
    EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm3Regs.TBPHS.bit.TBPHS = 0;
    EPwm3Regs.TBPRD = (uint16_t)(CPU_CLK / (2*100e3f));
    EPwm3Regs.TBCTR = 0;
    EPwm3Regs.EPWMSYNCOUTEN.all = 0;        // Disabled

    EALLOW;
    EPwm3Regs.EPWMSYNCINSEL.bit.SEL = 1;    // EPWM1.SYNCOUT for SYNCI
    EDIS;

    EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm3Regs.CMPA.bit.CMPA = 0;  //(CPU_CLK / (100e3f*2));

    EPwm3Regs.AQCTLA.bit.CAD = AQ_SET;
    EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;

//    EPwm3Regs.ETSEL.bit.INTSEL = 1; //zero
//    EPwm3Regs.ETSEL.bit.INTEN = 1;
//    EPwm3Regs.ETPS.bit.INTPRD = 1;

    // Event trigger for SOCA
    EPwm3Regs.ETSEL.bit.SOCASEL = ET_CTRU_CMPA;
    EPwm3Regs.ETSEL.bit.SOCAEN = 1;
    EPwm3Regs.ETPS.bit.SOCAPRD = ET_1ST;            // pulse on first event

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
    // Ecap2
    EALLOW;
    // GPIO39 -> InputXbar8 -> eCap2
    InputXbarRegs.INPUT8SELECT = 39;
    ECap2Regs.ECCTL0.bit.INPUTSEL = 7;

    ECap2Regs.ECCTL2.bit.TSCTRSTOP = 0;     // Stop eCap counter
    ECap2Regs.ECCTL2.bit.CAP_APWM = 0;      // Capture mode
    ECap2Regs.ECCTL2.bit.CONT_ONESHT = 0;   // Continuous mode
    ECap2Regs.ECCTL2.bit.STOP_WRAP = 0x3;
    ECap2Regs.ECCTL2.bit.DMAEVTSEL = 0x0;   // DMA triggered at event 4
    // Capture event trigger edge
    ECap2Regs.ECCTL1.bit.CAP1POL = 1;
    ECap2Regs.ECCTL1.bit.CAP2POL = 1;
    ECap2Regs.ECCTL1.bit.CAP3POL = 1;
    ECap2Regs.ECCTL1.bit.CAP4POL = 1;
    // Counter reset at capture event4
    ECap2Regs.ECCTL1.bit.CTRRST1 = 1;
    ECap2Regs.ECCTL1.bit.CTRRST2 = 1;
    ECap2Regs.ECCTL1.bit.CTRRST3 = 1;
    ECap2Regs.ECCTL1.bit.CTRRST4 = 1;
    // Disable all interrupt
    ECap2Regs.ECEINT.all = 0x00;
    ECap2Regs.ECCLR.all = 0xFF;

    ECap2Regs.ECCTL2.bit.TSCTRSTOP = 1;
    ECap2Regs.ECCTL1.bit.CAPLDEN = 1;
    ECap2Regs.ECCTL2.bit.REARM = 1;
    EDIS;

    // Ecap3
    EALLOW;
    // GPIO40 -> InputXbar9 -> eCap3
    InputXbarRegs.INPUT9SELECT = 40;
    ECap3Regs.ECCTL0.bit.INPUTSEL = 8;

    ECap3Regs.ECCTL2.bit.TSCTRSTOP = 0;     // Stop eCap counter
    ECap3Regs.ECCTL2.bit.CAP_APWM = 0;      // Capture mode
    ECap3Regs.ECCTL2.bit.CONT_ONESHT = 0;   // Continuous mode
    ECap3Regs.ECCTL2.bit.STOP_WRAP = 0x3;
    ECap3Regs.ECCTL2.bit.DMAEVTSEL = 0x0;   // DMA triggered at event 4
    // Capture event trigger edge
    ECap3Regs.ECCTL1.bit.CAP1POL = 1;
    ECap3Regs.ECCTL1.bit.CAP2POL = 1;
    ECap3Regs.ECCTL1.bit.CAP3POL = 1;
    ECap3Regs.ECCTL1.bit.CAP4POL = 1;
    // Counter reset at capture event4
    ECap3Regs.ECCTL1.bit.CTRRST1 = 1;
    ECap3Regs.ECCTL1.bit.CTRRST2 = 1;
    ECap3Regs.ECCTL1.bit.CTRRST3 = 1;
    ECap3Regs.ECCTL1.bit.CTRRST4 = 1;
    // Disable all interrupt
    ECap3Regs.ECEINT.all = 0x00;
    ECap3Regs.ECCLR.all = 0xFF;

    ECap3Regs.ECCTL2.bit.TSCTRSTOP = 1;
    ECap3Regs.ECCTL1.bit.CAPLDEN = 1;
    ECap3Regs.ECCTL2.bit.REARM = 1;
    EDIS;
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
    EALLOW;
    DacaRegs.DACCTL.bit.DACREFSEL = 1;
    DacaRegs.DACCTL.bit.MODE = 0;
    DacaRegs.DACOUTEN.bit.DACOUTEN = 1;
    DacaRegs.DACVALS.all = 0;
    DELAY_US(10);
    EDIS;
    // DAC B setting, output maximum = 3.3V
    EALLOW;
    DacbRegs.DACCTL.bit.DACREFSEL = 1;
    DacbRegs.DACCTL.bit.MODE = 0;
    DacbRegs.DACOUTEN.bit.DACOUTEN = 1;
    DacbRegs.DACVALS.all = 0;
    DELAY_US(10);
    EDIS;
}
/***************************************************************************************************
Function Name:
    void Setup_Sci(void)
Input:
    NULL
Output:
    NULL
Comment:
    Setup SCI for internal communication
***************************************************************************************************/
void Setup_Sci(void)
{
    ScibRegs.SCIFFCT.all = 0x0000;
    ScibRegs.SCIFFTX.all = 0xE040;
    ScibRegs.SCIFFRX.all = 0x2040;

    ScibRegs.SCICCR.all = 0x0007;           // 1 stop bit, No parity, 8 char bits,
    ScibRegs.SCICTL1.all = 0x0003;
    ScibRegs.SCICTL2.all = 0x0000;          // Disable all SCI interrupt

    // SCIA at 9600 baud
    // LSPCLK = 25 MHz (100 MHz SYSCLK), Baud rate = (LSPCLK) / (BAUD * 16)
    ScibRegs.SCIHBAUD.all = 0x0000;
    ScibRegs.SCILBAUD.all = 0x00A3;

    ScibRegs.SCICTL1.bit.SWRESET = 1;


    SciaRegs.SCIFFCT.all = 0x0000;
    SciaRegs.SCIFFTX.all = 0xE040;
    SciaRegs.SCIFFRX.all = 0x2040;

    SciaRegs.SCICCR.all = 0x0007;           // 1 stop bit, No parity, 8 char bits,
    SciaRegs.SCICTL1.all = 0x0003;
    SciaRegs.SCICTL2.all = 0x0000;          // Disable all SCI interrupt

    // SCIA at 9600 baud
    // LSPCLK = 25 MHz (100 MHz SYSCLK), Baud rate = (LSPCLK) / (BAUD * 16)
    SciaRegs.SCIHBAUD.all = 0x0000;
    SciaRegs.SCILBAUD.all = 0x00A3;

    SciaRegs.SCICTL1.bit.SWRESET = 1;

}
/***************************************************************************************************
Function Name:
    void Setup_Lin(void)
Input:
    NULL
Output:
    NULL
Comment:
    Setup LIN as SCI for internal communication
    Notice that after initialize the peripheral, RX module take about 1.2ms to settle down
***************************************************************************************************/
void Setup_Lin(void)
{

}
/***************************************************************************************************
Function Name:
    void Setup_Can(void)
Input:
    NULL
Output:
    NULL
Comment:
    Setup CANBus for external system communication
***************************************************************************************************/
void Setup_Can(void)
{

}
/***************************************************************************************************
Function Name:
    void Setup_Interrupt(void)
Input:
    NULL
Output:
    NULL
Comment:
    Setup interrupt
    Timer0 ISR - For fixed frequency control loop(50kHz)
***************************************************************************************************/
void Setup_Interrupt(void)
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
    PieVectTable.TIMER2_INT = &Timer2_ISR;
    PieVectTable.TIMER1_INT = &Timer1_ISR;
    EDIS;

    // Enable global Interrupts and higher priority real-time debug events:
//    IER = M_INT8 | M_INT13 | M_INT14;       // Enable group 8, 13, and 14 interrupts

//    PieCtrlRegs.PIEIER3.bit.INTx3 = 1;      // Enable EPWM3 interrupt
//    PieCtrlRegs.PIEIER8.bit.INTx13 = 1;     // Enable PMBus interrupt
//    PieCtrlRegs.PIEIER3.bit.INTx3 = 1;     //Enable EPWM3 interrupt
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;     //Enable ADCAINT1 interrupt
    PieCtrlRegs.PIEIER8.bit.INTx13 = 1;     //Enable PMBus interrupt
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;

    IER = M_INT8 | M_INT13 | M_INT14;       // Enable group 13 and 14 interrupts

    PieCtrlRegs.PIEACK.all = 0xFFFF;
    asm(" NOP");

    EINT;                                   // Enable Global interrupt INTM
    ERTM;                                   // Enable Global realtime interrupt DBGM
}
//------------------------------------------------------------------------------
// Global functions
//------------------------------------------------------------------------------
/***************************************************************************************************
Function Name:
    void Hw_Setup(void)
Input:
    NULL
Output:
    NULL
Comment:
    MCU setup
***************************************************************************************************/
void Hw_Setup(void)
{
    DINT;

//    Setup_System();
    InitSysCtrl();              // Initialize device clock and peripherals
    Setup_Cla();
    Setup_Dma();
    Setup_Timer();
    Setup_Gpio();
    (*Device_cal)();
    Setup_Adc();
    Setup_Comparator();
    Setup_Pwm();
    //Setup_Capture();
    Setup_Dac();
    //Setup_Lin();
    Setup_Can();
    Setup_Interrupt();
}
