/***************************************************************************************************
File Name: Vector.c
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
    ---------- ---------------- --------------------------------------------------------------------
    10/17/2019 Fred             Create file
    05/06/2021 Watch Lee        Update TIMER2_INT.

***************************************************************************************************/

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "App.h"

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------
const struct PIE_VECT_TABLE VectTable =
{
    .PIE1_RESERVED_INT = Default_ISR,
    .PIE2_RESERVED_INT = Default_ISR,
    .PIE3_RESERVED_INT = Default_ISR,
    .PIE4_RESERVED_INT = Default_ISR,
    .PIE5_RESERVED_INT = Default_ISR,
    .PIE6_RESERVED_INT = Default_ISR,
    .PIE7_RESERVED_INT = Default_ISR,
    .PIE8_RESERVED_INT = Default_ISR,
    .PIE9_RESERVED_INT = Default_ISR,
    .PIE10_RESERVED_INT = Default_ISR,
    .PIE11_RESERVED_INT = Default_ISR,
    .PIE12_RESERVED_INT = Default_ISR,
    .PIE13_RESERVED_INT = Default_ISR,

    // Non-Peripheral Interrupts
    .TIMER1_INT = Timer1_ISR,
    .TIMER2_INT = Timer2_ISR,
    .DATALOG_INT = Default_ISR,
    .RTOS_INT = Default_ISR,
    .EMU_INT = Default_ISR,
    .NMI_INT = Default_ISR,
    .ILLEGAL_INT = Default_ISR,
    .USER1_INT = Default_ISR,
    .USER2_INT = Default_ISR,
    .USER3_INT = Default_ISR,
    .USER4_INT = Default_ISR,
    .USER5_INT = Default_ISR,
    .USER6_INT = Default_ISR,
    .USER7_INT = Default_ISR,
    .USER8_INT = Default_ISR,
    .USER9_INT = Default_ISR,
    .USER10_INT = Default_ISR,
    .USER11_INT = Default_ISR,
    .USER12_INT = Default_ISR,

    // Group 1
    .ADCA1_INT = Default_ISR,
    .ADCB1_INT = Default_ISR,
    .ADCC1_INT = Default_ISR,
    .XINT1_INT = Default_ISR,
    .XINT2_INT = Default_ISR,
    .PIE14_RESERVED_INT = Default_ISR,
    .TIMER0_INT = Default_ISR,
    .WAKE_INT = Default_ISR,
    .PIE37_RESERVED_INT = Default_ISR,
    .PIE38_RESERVED_INT = Default_ISR,
    .PIE39_RESERVED_INT = Default_ISR,
    .PIE40_RESERVED_INT = Default_ISR,
    .PIE41_RESERVED_INT = Default_ISR,
    .PIE42_RESERVED_INT = Default_ISR,
    .PIE43_RESERVED_INT = Default_ISR,
    .PIE44_RESERVED_INT = Default_ISR,

    // Group 2
    .EPWM1_TZ_INT = Default_ISR,
    .EPWM2_TZ_INT = Default_ISR,
    .EPWM3_TZ_INT = Default_ISR,
    .EPWM4_TZ_INT = Default_ISR,
    .EPWM5_TZ_INT = Default_ISR,
    .EPWM6_TZ_INT = Default_ISR,
    .EPWM7_TZ_INT = Default_ISR,
    .EPWM8_TZ_INT = Default_ISR,
    .PIE45_RESERVED_INT = Default_ISR,
    .PIE46_RESERVED_INT = Default_ISR,
    .PIE47_RESERVED_INT = Default_ISR,
    .PIE48_RESERVED_INT = Default_ISR,
    .PIE49_RESERVED_INT = Default_ISR,
    .PIE50_RESERVED_INT = Default_ISR,
    .PIE51_RESERVED_INT = Default_ISR,
    .PIE52_RESERVED_INT = Default_ISR,

    // Group 3
    .EPWM1_INT = Default_ISR,
    .EPWM2_INT = Default_ISR,
    .EPWM3_INT = Default_ISR,
    .EPWM4_INT = Default_ISR,
    .EPWM5_INT = Default_ISR,
    .EPWM6_INT = Default_ISR,
    .EPWM7_INT = Default_ISR,
    .EPWM8_INT = Default_ISR,
    .PIE53_RESERVED_INT = Default_ISR,
    .PIE54_RESERVED_INT = Default_ISR,
    .PIE55_RESERVED_INT = Default_ISR,
    .PIE56_RESERVED_INT = Default_ISR,
    .PIE57_RESERVED_INT = Default_ISR,
    .PIE58_RESERVED_INT = Default_ISR,
    .PIE59_RESERVED_INT = Default_ISR,
    .PIE60_RESERVED_INT = Default_ISR,

    // Group 4
    .ECAP1_INT = Default_ISR,
    .ECAP2_INT = Default_ISR,
    .ECAP3_INT = Default_ISR,
    .ECAP4_INT = Default_ISR,
    .ECAP5_INT = Default_ISR,
    .ECAP6_INT = Default_ISR,
    .ECAP7_INT = Default_ISR,
    .PIE15_RESERVED_INT = Default_ISR,
    .PIE61_RESERVED_INT = Default_ISR,
    .PIE62_RESERVED_INT = Default_ISR,
    .PIE63_RESERVED_INT = Default_ISR,
    .PIE64_RESERVED_INT = Default_ISR,
    .PIE65_RESERVED_INT = Default_ISR,
    .ECAP6_2_INT = Default_ISR,
    .ECAP7_2_INT = Default_ISR,
    .PIE66_RESERVED_INT = Default_ISR,

    // Group 5
    .EQEP1_INT = Default_ISR,
    .EQEP2_INT = Default_ISR,
    .PIE16_RESERVED_INT = Default_ISR,
    .PIE17_RESERVED_INT = Default_ISR,
    .PIE18_RESERVED_INT = Default_ISR,
    .PIE19_RESERVED_INT = Default_ISR,
    .PIE20_RESERVED_INT = Default_ISR,
    .PIE21_RESERVED_INT = Default_ISR,
    .SD1_INT = Default_ISR,
    .PIE67_RESERVED_INT = Default_ISR,
    .PIE68_RESERVED_INT = Default_ISR,
    .PIE69_RESERVED_INT = Default_ISR,
    .SD1DR1_INT = Default_ISR,
    .SD1DR2_INT = Default_ISR,
    .SD1DR3_INT = Default_ISR,
    .SD1DR4_INT = Default_ISR,

    // Group 6
    .SPIA_RX_INT = Default_ISR,
    .SPIA_TX_INT = Default_ISR,
    .SPIB_RX_INT = Default_ISR,
    .SPIB_TX_INT = Default_ISR,
    .PIE22_RESERVED_INT = Default_ISR,
    .PIE23_RESERVED_INT = Default_ISR,
    .PIE24_RESERVED_INT = Default_ISR,
    .PIE25_RESERVED_INT = Default_ISR,
    .PIE70_RESERVED_INT = Default_ISR,
    .PIE71_RESERVED_INT = Default_ISR,
    .PIE72_RESERVED_INT = Default_ISR,
    .PIE73_RESERVED_INT = Default_ISR,
    .PIE74_RESERVED_INT = Default_ISR,
    .PIE75_RESERVED_INT = Default_ISR,
    .PIE76_RESERVED_INT = Default_ISR,
    .PIE77_RESERVED_INT = Default_ISR,

    // Group 7
    .DMA_CH1_INT = Default_ISR,
    .DMA_CH2_INT = Default_ISR,
    .DMA_CH3_INT = Default_ISR,
    .DMA_CH4_INT = Default_ISR,
    .DMA_CH5_INT = Default_ISR,
    .DMA_CH6_INT = Default_ISR,
    .PIE26_RESERVED_INT = Default_ISR,
    .PIE27_RESERVED_INT = Default_ISR,
    .PIE78_RESERVED_INT = Default_ISR,
    .PIE79_RESERVED_INT = Default_ISR,
    .PIE80_RESERVED_INT = Default_ISR,
    .PIE81_RESERVED_INT = Default_ISR,
    .PIE82_RESERVED_INT = Default_ISR,
    .PIE83_RESERVED_INT = Default_ISR,
    .CLA1PROMCRC_INT = Default_ISR,
    .PIE84_RESERVED_INT = Default_ISR,

    // Group 8
    .I2CA_INT = Default_ISR,
    .I2CA_FIFO_INT = Default_ISR,
    .PIE28_RESERVED_INT = Default_ISR,
    .PIE29_RESERVED_INT = Default_ISR,
    .PIE30_RESERVED_INT = Default_ISR,
    .PIE31_RESERVED_INT = Default_ISR,
    .PIE32_RESERVED_INT = Default_ISR,
    .PIE33_RESERVED_INT = Default_ISR,
    .LINA_0_INT = Default_ISR,
    .LINA_1_INT = Default_ISR,
    .PIE85_RESERVED_INT = Default_ISR,
    .PIE86_RESERVED_INT = Default_ISR,
    .PMBUSA_INT = Default_ISR,
    .PIE87_RESERVED_INT = Default_ISR,
    .PIE88_RESERVED_INT = Default_ISR,
    .PIE89_RESERVED_INT = Default_ISR,

    // Group 9
    .SCIA_RX_INT = Default_ISR,
    .SCIA_TX_INT = Default_ISR,
    .SCIB_RX_INT = Default_ISR,
    .SCIB_TX_INT = Default_ISR,
    .CANA0_INT = Default_ISR,
    .CANA1_INT = Default_ISR,
    .CANB0_INT = Default_ISR,
    .CANB1_INT = Default_ISR,
    .PIE90_RESERVED_INT = Default_ISR,
    .PIE91_RESERVED_INT = Default_ISR,
    .PIE92_RESERVED_INT = Default_ISR,
    .PIE93_RESERVED_INT = Default_ISR,
    .PIE94_RESERVED_INT = Default_ISR,
    .PIE95_RESERVED_INT = Default_ISR,
    .PIE96_RESERVED_INT = Default_ISR,
    .PIE97_RESERVED_INT = Default_ISR,

    // Group 10
    .ADCA_EVT_INT = Default_ISR,
    .ADCA2_INT = Default_ISR,
    .ADCA3_INT = Default_ISR,
    .ADCA4_INT = Default_ISR,
    .ADCB_EVT_INT = Default_ISR,
    .ADCB2_INT = Default_ISR,
    .ADCB3_INT = Default_ISR,
    .ADCB4_INT = Default_ISR,
    .ADCC_EVT_INT = Default_ISR,
    .ADCC2_INT = Default_ISR,
    .ADCC3_INT = Default_ISR,
    .ADCC4_INT = Default_ISR,
    .PIE98_RESERVED_INT = Default_ISR,
    .PIE99_RESERVED_INT = Default_ISR,
    .PIE100_RESERVED_INT = Default_ISR,
    .PIE101_RESERVED_INT = Default_ISR,

    // Group 11
    .CLA1_1_INT = Default_ISR,
    .CLA1_2_INT = Default_ISR,
    .CLA1_3_INT = Default_ISR,
    .CLA1_4_INT = Default_ISR,
    .CLA1_5_INT = Default_ISR,
    .CLA1_6_INT = Default_ISR,
    .CLA1_7_INT = Default_ISR,
    .CLA1_8_INT = Default_ISR,
    .PIE102_RESERVED_INT = Default_ISR,
    .PIE103_RESERVED_INT = Default_ISR,
    .PIE104_RESERVED_INT = Default_ISR,
    .PIE105_RESERVED_INT = Default_ISR,
    .PIE106_RESERVED_INT = Default_ISR,
    .PIE107_RESERVED_INT = Default_ISR,
    .PIE108_RESERVED_INT = Default_ISR,
    .PIE109_RESERVED_INT = Default_ISR,

    // Group 12
    .XINT3_INT = Default_ISR,
    .XINT4_INT = Default_ISR,
    .XINT5_INT = Default_ISR,
    .PIE34_RESERVED_INT = Default_ISR,
    .PIE35_RESERVED_INT = Default_ISR,
    .PIE36_RESERVED_INT = Default_ISR,
    .FPU_OVERFLOW_INT = Default_ISR,
    .FPU_UNDERFLOW_INT = Default_ISR,
    .PIE110_RESERVED_INT = Default_ISR,
    .RAM_CORRECTABLE_ERROR_INT = Default_ISR,
    .FLASH_CORRECTABLE_ERROR_INT = Default_ISR,
    .RAM_ACCESS_VIOLATION_INT = Default_ISR,
    .SYS_PLL_SLIP_INT = Default_ISR,
    .PIE111_RESERVED_INT = Default_ISR,
    .CLA_OVERFLOW_INT = Default_ISR,
    .CLA_UNDERFLOW_INT = Default_ISR,
};

const void *ClaTaskTable[] =
{
    [0] = CLA_ADC_C1_ISR,
    [1] = claDefaultTask,
    [2] = claDefaultTask,
    [3] = claDefaultTask,
    [4] = claDefaultTask,
    [5] = claDefaultTask,
    [6] = claDefaultTask,
    [7] = claDefaultTask,
};
