/**
 * @file        DPST6000EBA_SEC_App.cmd
 * @copyright   Delta Electronics Corp.
 */

/*******************************************************************************
 * Memory
 ******************************************************************************/
MEMORY
{
/* Program Memory */
PAGE 0:
    RAMM0               : origin = 0x000000F5, length = 0x0000030B
    RAMM1               : origin = 0x00000400, length = 0x000003F8     /* on-chip RAM block M1 */
//   RAMM1_RSVD      : origin = 0x0007F8, length = 0x000008     /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */
    CLAPROGRAM          : origin = 0x00008800, length = 0x00000800
    RAMLS2              : origin = 0x00009000, length = 0x00001000
//    RAMLS3              : origin = 0x00009800, length = 0x00000800
    RAMLS45             : origin = 0x0000A000, length = 0x00001000
    //RAMLS67             : origin = 0x0000B000, length = 0x00001000
    RAMLS6              : origin = 0x0000B000, length = 0x00000800
    RAMLS7              : origin = 0x0000B800, length = 0x00000800
    BOOT_ACT_KEY        : origin = 0x0000C000, length = 0x00000010 /* Boot activate key */
    RAMGS0              : origin = 0x0000C010, length = 0x00001FF0
//    RAMGS1              : origin = 0x0000E000, length = 0x00002000
//    RAMGS2              : origin = 0x00010000, length = 0x00002000
    RAMGS12             : origin = 0x0000E000, length = 0x00004000
    RAMGS3              : origin = 0x00012000, length = 0x00001FF8
//   RAMGS3_RSVD : origin = 0x013FF8, length = 0x000008     /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */
    /* Flash sectors */
    /* BANK 0 */
//    FLASH_BOOT_CODE     : origin = 0x00080002, length = 0x00004FEC /* IAP boot code       */
//    BOOT_FUNCTION_ID    : origin = 0x00084FEE, length = 0x00000002 /* Function ID record  */
//    BOOT_VERSION        : origin = 0x00084FF0, length = 0x00000010 /* Boot version record */
    FLASH_SIGNATURE     : origin = 0x00085000, length = 0x00000002 /* Signature of app header      */
    FLASH_APP_HEADER    : origin = 0x00085006, length = 0x0000001A /* App header field             */
    COMPLIANT_ADDRESS   : origin = 0x00085020, length = 0x00000002 /* Address of compliant table   */
    FLASH_HEADER_RES    : origin = 0x00085022, length = 0x000000DE /* Reserved field of app header */
    COMPLIANT_TABLE     : origin = 0x00085100, length = 0x00000100 /* Compliant table field        */
    FLASH_APP_CODE      : origin = 0x00085202, length = 0x00009DFE /* App code field    */
    FLASH_CALIBRATION   : origin = 0x0008F000, length = 0x00001000 /* on-chip Flash     */
    ENTRY               : origin = 0x00085200, length = 0x00000002

//    FLASH_BANK0_SEC5    : origin = 0x00085202, length = 0x00000DFE /* on-chip Flash */
//    FLASH_BANK0_SEC6    : origin = 0x00086000, length = 0x00001000 /* on-chip Flash */
//    FLASH_BANK0_SEC7    : origin = 0x00087000, length = 0x00001000 /* on-chip Flash */
//    FLASH_BANK0_SEC8    : origin = 0x00088000, length = 0x00001000 /* on-chip Flash */
//    FLASH_BANK0_SEC9    : origin = 0x00089000, length = 0x00001000 /* on-chip Flash */
//    FLASH_BANK0_SEC10   : origin = 0x0008A000, length = 0x00001000 /* on-chip Flash */
//    FLASH_BANK0_SEC11   : origin = 0x0008B000, length = 0x00001000 /* on-chip Flash */
//    FLASH_BANK0_SEC12   : origin = 0x0008C000, length = 0x00001000 /* on-chip Flash */
//    FLASH_BANK0_SEC13   : origin = 0x0008D000, length = 0x00001000 /* on-chip Flash */

    /* BANK 1 */
    FLASH_BANK1_SEC0    : origin = 0x00090000, length = 0x00001000 /* on-chip Flash */
    FLASH_BANK1_SEC1    : origin = 0x00091000, length = 0x00001000 /* on-chip Flash */
    FLASH_BANK1_SEC2    : origin = 0x00092000, length = 0x00001000 /* on-chip Flash */
    FLASH_BANK1_SEC3    : origin = 0x00093000, length = 0x00001000 /* on-chip Flash */
    FLASH_BANK1_SEC4    : origin = 0x00094000, length = 0x00001000 /* on-chip Flash */
    FLASH_BANK1_APP     : origin = 0x00095000, length = 0x0000A000 /* Bank1 app ROM field */
    FLASH_BANK1_SEC15   : origin = 0x0009F000, length = 0x00000FF0    /* on-chip Flash */
//   FLASH_BANK1_SEC15_RSVD : origin = 0x09FFF0, length = 0x000010  /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */
    RESET               : origin = 0x003FFFC0, length = 0x00000002

/* Data Memory */
PAGE 1:
    BOOT_RSVD           : origin = 0x00000002, length = 0x000000F3     /* Part of M0, BOOT rom will use this for stack */
    RAMM0               : origin = 0x000000F5, length = 0x0000030B
    RAMM1               : origin = 0x00000400, length = 0x000003F8     /* on-chip RAM block M1 */
//   RAMM1_RSVD      : origin = 0x0007F8, length = 0x000008     /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */
//    CLA2CPUMSGRAM       : origin = 0x00001480, length = 0x00000080
//    CPU2CLAMSGRAM       : origin = 0x00001500, length = 0x00000080
    CLA1_MSGRAMLOW      : origin = 0x00001480, length = 0x00000080
    CLA1_MSGRAMHIGH     : origin = 0x00001500, length = 0x00000080
    CLADATARAM          : origin = 0x00008000, length = 0x00000800
    RAMLS2              : origin = 0x00009000, length = 0x00000800
    RAMLS3              : origin = 0x00009800, length = 0x00000800
    RAMLS45              : origin = 0x0000A000, length = 0x00001000
//    RAMLS67              : origin = 0x0000B000, length = 0x00001000
//    RAMLS4              : origin = 0x0000A000, length = 0x00000800
//    RAMLS5              : origin = 0x0000A800, length = 0x00000800
    RAMLS6              : origin = 0x0000B000, length = 0x00000800
    RAMLS7              : origin = 0x0000B800, length = 0x00000800
    BOOT_ACT_KEY        : origin = 0x0000C000, length = 0x00000010 /* Boot activate key */
    RAMGS0              : origin = 0x0000C010, length = 0x00001FF0
    RAMGS1              : origin = 0x0000E000, length = 0x00002000
    RAMGS2              : origin = 0x00010000, length = 0x00002000
    RAMGS3              : origin = 0x00012000, length = 0x00001FF8
//   RAMGS3_RSVD : origin = 0x013FF8, length = 0x000008     /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */

    CLA1_DATAROM        : origin = 0x01001000, length = 0x00001000

/* Peripharal */

PAGE 1:
    ADCA_RESULT         : origin = 0x00000B00, length = 0x00000020
    ADCB_RESULT         : origin = 0x00000B20, length = 0x00000020
    ADCC_RESULT         : origin = 0x00000B40, length = 0x00000020
    ADCA                : origin = 0x00007400, length = 0x00000080
    ADCB                : origin = 0x00007480, length = 0x00000080
    ADCC                : origin = 0x00007500, length = 0x00000080

    ANALOG_SUBSYS       : origin = 0x0005D700, length = 0x00000100

    CANA                : origin = 0x00048000, length = 0x00000800
    CANB                : origin = 0x0004A000, length = 0x00000800

    CLA1                : origin = 0x00001400, length = 0x00000080

    CLAPROMCRC          : origin = 0x000061C0, length = 0x00000020

    CLB_XBAR            : origin = 0x00007A40, length = 0x00000040

    CMPSS1              : origin = 0x00005C80, length = 0x00000020
    CMPSS2              : origin = 0x00005CA0, length = 0x00000020
    CMPSS3              : origin = 0x00005CC0, length = 0x00000020
    CMPSS4              : origin = 0x00005CE0, length = 0x00000020
    CMPSS5              : origin = 0x00005D00, length = 0x00000020
    CMPSS6              : origin = 0x00005D20, length = 0x00000020
    CMPSS7              : origin = 0x00005D40, length = 0x00000020

    CPU_TIMER0          : origin = 0x00000C00, length = 0x00000008
    CPU_TIMER1          : origin = 0x00000C08, length = 0x00000008
    CPU_TIMER2          : origin = 0x00000C10, length = 0x00000008

    DACA                : origin = 0x00005C00, length = 0x00000010
    DACB                : origin = 0x00005C10, length = 0x00000010

    DCC0                : origin = 0x0005E700, length = 0x00000040

    DCSM_BANK0_Z1       : origin = 0x0005F000, length = 0x00000030
    DCSM_BANK0_Z2       : origin = 0x0005F040, length = 0x00000030
    DCSM_BANK1_Z1       : origin = 0x0005F100, length = 0x00000030
    DCSM_BANK1_Z2       : origin = 0x0005F140, length = 0x00000030
    DCSM_COMMON         : origin = 0x0005F070, length = 0x00000010     /* Common Dual code security module registers */

    DMA                 : origin = 0x00001000, length = 0x00000200

    ECAP1               : origin = 0x00005200, length = 0x00000040     /* Enhanced Capture 1 registers */
    ECAP2               : origin = 0x00005240, length = 0x00000040     /* Enhanced Capture 2 registers */
    ECAP3               : origin = 0x00005280, length = 0x00000040     /* Enhanced Capture 3 registers */
    ECAP4               : origin = 0x000052C0, length = 0x00000040     /* Enhanced Capture 4 registers */
    ECAP5               : origin = 0x00005300, length = 0x00000040     /* Enhanced Capture 5 registers */
    ECAP6               : origin = 0x00005340, length = 0x00000040     /* Enhanced Capture 6 registers */
    ECAP7               : origin = 0x00005380, length = 0x00000040     /* Enhanced Capture 7 registers */

    PGA1                : origin = 0x00005B00, length = 0x00000010
    PGA2                : origin = 0x00005B10, length = 0x00000010
    PGA3                : origin = 0x00005B20, length = 0x00000010
    PGA4                : origin = 0x00005B30, length = 0x00000010
    PGA5                : origin = 0x00005B40, length = 0x00000010
    PGA6                : origin = 0x00005B50, length = 0x00000010
    PGA7                : origin = 0x00005B60, length = 0x00000010

    EPWM1               : origin = 0x00004000, length = 0x00000100     /* Enhanced PWM 1 registers */
    EPWM2               : origin = 0x00004100, length = 0x00000100     /* Enhanced PWM 2 registers */
    EPWM3               : origin = 0x00004200, length = 0x00000100     /* Enhanced PWM 3 registers */
    EPWM4               : origin = 0x00004300, length = 0x00000100     /* Enhanced PWM 4 registers */
    EPWM5               : origin = 0x00004400, length = 0x00000100     /* Enhanced PWM 5 registers */
    EPWM6               : origin = 0x00004500, length = 0x00000100     /* Enhanced PWM 6 registers */
    EPWM7               : origin = 0x00004600, length = 0x00000100     /* Enhanced PWM 7 registers */
    EPWM8               : origin = 0x00004700, length = 0x00000100     /* Enhanced PWM 8 registers */

    EPWM_XBAR           : origin = 0x00007A00, length = 0x00000040

    EQEP1               : origin = 0x00005100, length = 0x00000040     /* Enhanced QEP 1 registers */
    EQEP2               : origin = 0x00005140, length = 0x00000040     /* Enhanced QEP 2 registers */

    FLASH0_CTRL         : origin = 0x0005F800, length = 0x00000300
    FLASH0_ECC          : origin = 0x0005FB00, length = 0x00000040

    FSITXA              : origin = 0x00006600, length = 0x00000080
    FSIRXA              : origin = 0x00006680, length = 0x00000080

    GPIOCTRL            : origin = 0x00007C00, length = 0x00000200     /* GPIO control registers */
    GPIODAT             : origin = 0x00007F00, length = 0x00000040     /* GPIO data registers */

    I2CA                : origin = 0x00007300, length = 0x00000040     /* I2C-A registers */

    INPUT_XBAR          : origin = 0x00007900, length = 0x00000020

    LINA                : origin = 0x00006A00, length = 0x00000100
    LINB                : origin = 0x00006B00, length = 0x00000100

    MEMCFG              : origin = 0x0005F400, length = 0x00000080     /* Mem Config registers */
    ACCESSPROTECTION    : origin = 0x0005F4C0, length = 0x00000040     /* Access Protection registers */
    MEMORYERROR         : origin = 0x0005F500, length = 0x00000040     /* Access Protection registers */

    NMIINTRUPT          : origin = 0x00007060, length = 0x00000010     /* NMI Watchdog Interrupt Registers */

    OUTPUT_XBAR         : origin = 0x00007A80, length = 0x00000040

    PIE_CTRL            : origin = 0x00000CE0, length = 0x00000020     /* PIE control registers */

    PIE_VECT            : origin = 0x00000D00, length = 0x00000200     /* PIE Vector Table */

    PMBUSA              : origin = 0x00006400, length = 0x00000020

    SCIA                : origin = 0x00007200, length = 0x00000010     /* SCI-A registers */
    SCIB                : origin = 0x00007210, length = 0x00000010     /* SCI-B registers */

    SDFM1               : origin = 0x00005E00, length = 0x00000080     /* Sigma delta 1 registers */

    SPIA                : origin = 0x00006100, length = 0x00000010
    SPIB                : origin = 0x00006110, length = 0x00000010

    WD                  : origin = 0x00007000, length = 0x00000040
    DMACLASRCSEL        : origin = 0x00007980, length = 0x00000040
    DEV_CFG             : origin = 0x0005D000, length = 0x00000180
    CLK_CFG             : origin = 0x0005D200, length = 0x00000100
    CPU_SYS             : origin = 0x0005D300, length = 0x00000100
    PERIPH_AC           : origin = 0x0005D500, length = 0x00000200

    ERAD_GLOBAL         : origin = 0x0005E800, length = 0x00000013
    ERAD_HWBP1          : origin = 0x0005E900, length = 0x00000008
    ERAD_HWBP2          : origin = 0x0005E908, length = 0x00000008
    ERAD_HWBP3          : origin = 0x0005E910, length = 0x00000008
    ERAD_HWBP4          : origin = 0x0005E918, length = 0x00000008
    ERAD_HWBP5          : origin = 0x0005E920, length = 0x00000008
    ERAD_HWBP6          : origin = 0x0005E928, length = 0x00000008
    ERAD_HWBP7          : origin = 0x0005E930, length = 0x00000008
    ERAD_HWBP8          : origin = 0x0005E938, length = 0x00000008
    ERAD_CTR1           : origin = 0x0005E980, length = 0x00000010
    ERAD_CTR2           : origin = 0x0005E990, length = 0x00000010
    ERAD_CTR3           : origin = 0x0005E9A0, length = 0x00000010
    ERAD_CTR4           : origin = 0x0005E9B0, length = 0x00000010

    XBAR                : origin = 0x00007920, length = 0x00000020
    SYNC_SOC            : origin = 0x00007940, length = 0x00000010

    XINT                : origin = 0x00007070, length = 0x00000010
}

/*******************************************************************************
 * Program Sections
 ******************************************************************************/
SECTIONS
{
    codestart           : > ENTRY,              PAGE = 0

    .cinit              : > FLASH_APP_CODE,     PAGE = 0

    .pinit              : > FLASH_APP_CODE,     PAGE = 0

    .ovly               : > FLASH_APP_CODE,     PAGE = 0

    .switch             : > FLASH_APP_CODE,     PAGE = 0

    .text               : > FLASH_APP_CODE,     PAGE = 0

    .econst             : > FLASH_APP_CODE,     PAGE = 0

    .llctable           : > FLASH_APP_CODE,     PAGE = 0

    .calibration        : > FLASH_CALIBRATION,
                            START(_start_of_calibration),
                            END(_end_of_calibration)
                            {
                                . += SIZE(FLASH_CALIBRATION);
                            },
                            PAGE = 0

    .TI.ramfunc         : { -l F021_API_F28004x_FPU32.lib }
                          LOAD = FLASH_APP_CODE,
                          RUN = RAMLS2,
                          LOAD_START(_RamfuncsLoadStart),
                          LOAD_SIZE(_RamfuncsLoadSize),
                          LOAD_END(_RamfuncsLoadEnd),
                          RUN_START(_RamfuncsRunStart),
                          RUN_SIZE(_RamfuncsRunSize),
                          RUN_END(_RamfuncsRunEnd),
                          PAGE = 0, ALIGN(4)
/*
    Cla1Prog            : LOAD = FLASH_APP_CODE,
                          RUN = CLAPROGRAM,
                          LOAD_START(_Cla1ProgLoadStart),
                          RUN_START(_Cla1ProgRunStart),
                          LOAD_SIZE(_Cla1ProgLoadSize),
                          PAGE = 0
*/



    cla1ToCpuMsgRAM   : > CLA1_MSGRAMLOW,       PAGE = 1
    cpuToCla1MsgRAM   : > CLA1_MSGRAMHIGH,      PAGE = 1

    .scratchpad       : > RAMLS45,             PAGE = 0
    .bss_cla          : > RAMLS6,             PAGE = 0

    Cla1Prog          : LOAD = FLASH_APP_CODE,
                        RUN = RAMLS45,
                        LOAD_START(_ClaFuncsLoadStart),
                        LOAD_SIZE(_ClaFuncsLoadSize),
                        RUN_START(_ClaFuncsRunStart),
                        PAGE = 0, ALIGN(4)

    .const_cla        : LOAD = FLASH_APP_CODE,
                        RUN = RAMLS6,
                        LOAD_START(_ClaConstLoadStart),
                        LOAD_SIZE(_ClaConstLoadSize),
                        RUN_START(_ClaConstRunStart),
                        PAGE = 0, ALIGN(4)



    .signature          : > FLASH_SIGNATURE,
                          START(_start_of_approm),
                          PAGE = 0

    .appheader          : > FLASH_APP_HEADER,   PAGE = 0
    .compliantaddress   : > COMPLIANT_ADDRESS,  PAGE = 0
    .headerreserved     : > FLASH_HEADER_RES,   PAGE = 0

    .complianttable     : > COMPLIANT_TABLE,
                          START(_start_of_complianttable),
                          PAGE = 0

    .approm             : > FLASH_APP_CODE,
                          START(_start_of_appcode),
                          END(_end_of_approm),
                          PAGE = 0

    .appbank1           : START(_start_of_appbank1),
                          END(_end_of_appbank1)
                          {
                              . += SIZE(FLASH_BANK1_APP);
                          } > FLASH_BANK1_APP,    PAGE = 0

    .reset              : > RESET,             PAGE = 0, TYPE = DSECT /* not used, */
}

/*******************************************************************************
 * Data Sections
 ******************************************************************************/
SECTIONS
{
    /* CLA C compiler sections */

    .stack              : > RAMM1,              PAGE = 1

    .ebss               : >> RAMGS1 | RAMGS2 ,  PAGE = 1
                        //: >> RAMLS3 | RAMLS4 | RAMLS5 | RAMLS6 , PAGE = 1
    .esysmem            : > RAMLS7,         PAGE = 0
    .cio                : > RAMLS7,         PAGE = 1

    .ramgs0             : > RAMGS0,             PAGE = 1
    .ramgs1             : > RAMGS1,             PAGE = 1
    .ramgs2             : > RAMGS2,             PAGE = 1
    .ramgs3             : > RAMGS3,             PAGE = 1

    .boot_activate      : > BOOT_ACT_KEY,
                          START(_boot_activate),
                          PAGE = 1
}

/*******************************************************************************
 * Peripharal Sections
 ******************************************************************************/
SECTIONS
{
/*** PIE Vect Table and Boot ROM Variables Structures ***/
    UNION run = PIE_VECT, PAGE = 1
    {
        PieVectTableFile
        GROUP
        {
            EmuKeyVar
            EmuBModeVar
            FlashCallbackVar
            FlashScalingVar
        }
    }

    AdcaResultFile              : > ADCA_RESULT,        PAGE = 1
    AdcbResultFile              : > ADCB_RESULT,        PAGE = 1
    AdccResultFile              : > ADCC_RESULT,        PAGE = 1

    AdcaRegsFile                : > ADCA,               PAGE = 1
    AdcbRegsFile                : > ADCB,               PAGE = 1
    AdccRegsFile                : > ADCC,               PAGE = 1

    AnalogSubsysRegsFile        : > ANALOG_SUBSYS,      PAGE = 1

    CanaRegsFile                : > CANA,               PAGE = 1
    CanbRegsFile                : > CANB,               PAGE = 1

    Cla1RegsFile                : > CLA1,               PAGE = 1
    Cla1SoftIntRegsFile         : > PIE_CTRL,           PAGE = 1, type=DSECT

    ClaPromCrc0RegsFile         : > CLAPROMCRC,         PAGE = 1

    ClbXbarRegsFile             : > CLB_XBAR,           PAGE = 1

    Cmpss1RegsFile              : > CMPSS1,             PAGE = 1
    Cmpss2RegsFile              : > CMPSS2,             PAGE = 1
    Cmpss3RegsFile              : > CMPSS3,             PAGE = 1
    Cmpss4RegsFile              : > CMPSS4,             PAGE = 1
    Cmpss5RegsFile              : > CMPSS5,             PAGE = 1
    Cmpss6RegsFile              : > CMPSS6,             PAGE = 1
    Cmpss7RegsFile              : > CMPSS7,             PAGE = 1

    CpuTimer0RegsFile           : > CPU_TIMER0,         PAGE = 1
    CpuTimer1RegsFile           : > CPU_TIMER1,         PAGE = 1
    CpuTimer2RegsFile           : > CPU_TIMER2,         PAGE = 1

    DacaRegsFile                : > DACA                PAGE = 1
    DacbRegsFile                : > DACB                PAGE = 1

    Dcc0RegsFile                : > DCC0                PAGE = 1

    DcsmBank0Z1RegsFile         : > DCSM_BANK0_Z1,      PAGE = 1
    DcsmBank0Z2RegsFile         : > DCSM_BANK0_Z2,      PAGE = 1
    DcsmBank1Z1RegsFile         : > DCSM_BANK1_Z1,      PAGE = 1
    DcsmBank1Z2RegsFile         : > DCSM_BANK1_Z2,      PAGE = 1
    DcsmCommonRegsFile          : > DCSM_COMMON,        PAGE = 1

    DmaRegsFile                 : > DMA,                PAGE = 1

    ECap1RegsFile               : > ECAP1,              PAGE = 1
    ECap2RegsFile               : > ECAP2,              PAGE = 1
    ECap3RegsFile               : > ECAP3,              PAGE = 1
    ECap4RegsFile               : > ECAP4,              PAGE = 1
    ECap5RegsFile               : > ECAP5,              PAGE = 1
    ECap6RegsFile               : > ECAP6,              PAGE = 1
    ECap7RegsFile               : > ECAP7,              PAGE = 1

    Pga1RegsFile                : > PGA1,               PAGE = 1
    Pga2RegsFile                : > PGA2,               PAGE = 1
    Pga3RegsFile                : > PGA3,               PAGE = 1
    Pga4RegsFile                : > PGA4,               PAGE = 1
    Pga5RegsFile                : > PGA5,               PAGE = 1
    Pga6RegsFile                : > PGA6,               PAGE = 1
    Pga7RegsFile                : > PGA7,               PAGE = 1

    EPwm1RegsFile               : > EPWM1,              PAGE = 1
    EPwm2RegsFile               : > EPWM2,              PAGE = 1
    EPwm3RegsFile               : > EPWM3,              PAGE = 1
    EPwm4RegsFile               : > EPWM4,              PAGE = 1
    EPwm5RegsFile               : > EPWM5,              PAGE = 1
    EPwm6RegsFile               : > EPWM6,              PAGE = 1
    EPwm7RegsFile               : > EPWM7,              PAGE = 1
    EPwm8RegsFile               : > EPWM8,              PAGE = 1

    EPwmXbarRegsFile            : > EPWM_XBAR           PAGE = 1

    EQep1RegsFile               : > EQEP1,              PAGE = 1
    EQep2RegsFile               : > EQEP2,              PAGE = 1

    EradGlobalRegsFile          : > ERAD_GLOBAL,        PAGE = 1
    EradHWBP1RegsFile           : > ERAD_HWBP1,         PAGE = 1
    EradHWBP2RegsFile           : > ERAD_HWBP2,         PAGE = 1
    EradHWBP3RegsFile           : > ERAD_HWBP3,         PAGE = 1
    EradHWBP4RegsFile           : > ERAD_HWBP4,         PAGE = 1
    EradHWBP5RegsFile           : > ERAD_HWBP5,         PAGE = 1
    EradHWBP6RegsFile           : > ERAD_HWBP6,         PAGE = 1
    EradHWBP7RegsFile           : > ERAD_HWBP7,         PAGE = 1
    EradHWBP8RegsFile           : > ERAD_HWBP8,         PAGE = 1
    EradCounter1RegsFile        : > ERAD_CTR1,          PAGE = 1
    EradCounter2RegsFile        : > ERAD_CTR2,          PAGE = 1
    EradCounter3RegsFile        : > ERAD_CTR3,          PAGE = 1
    EradCounter4RegsFile        : > ERAD_CTR4,          PAGE = 1

    Flash0CtrlRegsFile          : > FLASH0_CTRL         PAGE = 1
    Flash0EccRegsFile           : > FLASH0_ECC          PAGE = 1

    FsiTxaRegsFile              : > FSITXA              PAGE = 1
    FsiRxaRegsFile              : > FSIRXA              PAGE = 1

    GpioCtrlRegsFile            : > GPIOCTRL,           PAGE = 1
    GpioDataRegsFile            : > GPIODAT,            PAGE = 1

    I2caRegsFile                : > I2CA,               PAGE = 1

    InputXbarRegsFile           : > INPUT_XBAR          PAGE = 1
    XbarRegsFile                : > XBAR                PAGE = 1

    LinaRegsFile                : > LINA,               PAGE = 1
    LinbRegsFile                : > LINB,               PAGE = 1

    MemCfgRegsFile              : > MEMCFG,             PAGE = 1
    AccessProtectionRegsFile    : > ACCESSPROTECTION,   PAGE = 1
    MemoryErrorRegsFile         : > MEMORYERROR,        PAGE = 1

    NmiIntruptRegsFile          : > NMIINTRUPT,         PAGE = 1

    OutputXbarRegsFile          : > OUTPUT_XBAR,        PAGE = 1

    PieCtrlRegsFile             : > PIE_CTRL,           PAGE = 1

    PmbusaRegsFile              : > PMBUSA,             PAGE = 1

    SciaRegsFile                : > SCIA,               PAGE = 1
    ScibRegsFile                : > SCIB,               PAGE = 1

    Sdfm1RegsFile               : > SDFM1,              PAGE = 1

    SpiaRegsFile                : > SPIA,               PAGE = 1
    SpibRegsFile                : > SPIB,               PAGE = 1

    WdRegsFile                  : > WD,                 PAGE = 1
    DmaClaSrcSelRegsFile        : > DMACLASRCSEL        PAGE = 1
    DevCfgRegsFile              : > DEV_CFG,            PAGE = 1
    ClkCfgRegsFile              : > CLK_CFG,            PAGE = 1
    CpuSysRegsFile              : > CPU_SYS,            PAGE = 1
    SysPeriphAcRegsFile         : > PERIPH_AC,          PAGE = 1

    SyncSocRegsFile             : > SYNC_SOC,           PAGE = 1

    XintRegsFile                : > XINT,               PAGE = 1
}
