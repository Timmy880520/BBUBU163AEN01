/**************************************************************************************************
File Name: f28003x_app_flash_lnk.cmd
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
	None
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
	None
Description:
    TMS320F28003x memory configuration file.

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    02/23/2023 Watch Lee        1. create file.

**************************************************************************************************/

MEMORY
{
    BOOT_RSVD           : origin = 0x00000002, length = 0x000000F3     /* Part of M0, BOOT rom will use this for stack */
    RAMM0               : origin = 0x000000F5, length = 0x0000030B
    RAMM1               : origin = 0x00000400, length = 0x000003F8     /* on-chip RAM block M1 */
//   RAMM1_RSVD      : origin = 0x0007F8, length = 0x000008     /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */

    CLADATARAM          : origin = 0x00008000, length = 0x00000800
    CLAPROGRAM          : origin = 0x00008800, length = 0x00000800
    RAMLS2              : origin = 0x00009000, length = 0x00001000
//    RAMLS3              : origin = 0x00009800, length = 0x00000800
    RAMLS45             : origin = 0x0000A000, length = 0x00001000
    RAMLS6              : origin = 0x0000B000, length = 0x00000800
    RAMLS7              : origin = 0x0000B800, length = 0x00000800

    BOOT_ACT_KEY        : origin = 0x0000C000, length = 0x00000010 /* Boot activate key */
    RAMGS0              : origin = 0x0000C010, length = 0x00000FF0

    RAMGS12             : origin = 0x0000D000, length = 0x00002000
    RAMGS3              : origin = 0x0000F000, length = 0x00000FF8
//   RAMGS3_RSVD : origin = 0x00FFF8, length = 0x000008     /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */

    BOOTROM             : origin = 0x003F8000, length = 0x00007FC0
    SECURE_ROM          : origin = 0x003F2000, length = 0x00006000

    RESET               : origin = 0x003FFFC0, length = 0x00000002

    /* Flash sectors */
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
    /* BANK 1 */
    FLASH_BANK1_SEC0    : origin = 0x090000, length = 0x001000 /* on-chip Flash */
    FLASH_BANK1_SEC1    : origin = 0x091000, length = 0x001000 /* on-chip Flash */
    FLASH_BANK1_SEC2    : origin = 0x092000, length = 0x001000 /* on-chip Flash */
    FLASH_BANK1_SEC3    : origin = 0x093000, length = 0x001000 /* on-chip Flash */
    FLASH_BANK1_APP     : origin = 0x094000, length = 0x00A000 /* Bank1 app ROM field */
    FLASH_BANK1_SEC14   : origin = 0x09E000, length = 0x001000
    FLASH_BANK1_SEC15   : origin = 0x09F000, length = 0x000FF0
//   FLASH_BANK1_SEC15_RSVD : origin = 0x09FFF0, length = 0x000010  /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */

    /* BANK 2 */
    FLASH_BANK2_SEC0    : origin = 0x000A0000, length = 0x00001000
    FLASH_BANK2_SEC1    : origin = 0x000A1000, length = 0x00001000
    FLASH_BANK2_SEC2    : origin = 0x000A2000, length = 0x00001000
    FLASH_BANK2_SEC3    : origin = 0x000A3000, length = 0x00001000
    FLASH_BANK2_SEC4    : origin = 0x000A4000, length = 0x00001000
    FLASH_BANK2_SEC5    : origin = 0x000A5000, length = 0x00001000
    FLASH_BANK2_SEC6    : origin = 0x000A6000, length = 0x00001000
    FLASH_BANK2_SEC7    : origin = 0x000A7000, length = 0x00001000
    FLASH_BANK2_SEC8    : origin = 0x000A8000, length = 0x00001000
    FLASH_BANK2_SEC9    : origin = 0x000A9000, length = 0x00001000
    FLASH_BANK2_SEC10   : origin = 0x000AA000, length = 0x00001000
    FLASH_BANK2_SEC11   : origin = 0x000AB000, length = 0x00001000
    FLASH_BANK2_SEC12   : origin = 0x000AC000, length = 0x00001000
    FLASH_BANK2_SEC13   : origin = 0x000AD000, length = 0x00001000
    FLASH_BANK2_SEC14   : origin = 0x000AE000, length = 0x00001000
    FLASH_BANK2_SEC15   : origin = 0x000AF000, length = 0x00000FF0

    /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */
//  FLASH_BANK2_RSVD  : origin = 0x0AFFF0, length = 0x000010

    CLA1_MSGRAMLOW      : origin = 0x00001480, length = 0x00000080
    CLA1_MSGRAMHIGH     : origin = 0x00001500, length = 0x00000080
}

SECTIONS
{
    codestart           : > ENTRY,          ALIGN(8)
    .cinit              : > FLASH_APP_CODE, ALIGN(8)
    .pinit              : > FLASH_APP_CODE, ALIGN(8)
    .ovly               : > FLASH_APP_CODE, ALIGN(8)
    .switch             : > FLASH_APP_CODE, ALIGN(8)
    .text               : > FLASH_APP_CODE, ALIGN(8)
    .econst             : > FLASH_APP_CODE, ALIGN(8)
    .llctable           : > FLASH_APP_CODE, ALIGN(8)

    .calibration        : > FLASH_CALIBRATION,
                            START(_start_of_calibration),
                            END(_end_of_calibration)
                            {
                                . += SIZE(FLASH_CALIBRATION);
                            }

    .TI.ramfunc         : { -l FAPI_F28003x_COFF_v1.58.10.lib }
                          LOAD = FLASH_APP_CODE,
                          RUN = RAMLS2,
                          LOAD_START(_RamfuncsLoadStart),
                          LOAD_SIZE(_RamfuncsLoadSize),
                          LOAD_END(_RamfuncsLoadEnd),
                          RUN_START(_RamfuncsRunStart),
                          RUN_SIZE(_RamfuncsRunSize),
                          RUN_END(_RamfuncsRunEnd),
                          ALIGN(8)

    cla1ToCpuMsgRAM     : > CLA1_MSGRAMLOW
    cpuToCla1MsgRAM     : > CLA1_MSGRAMHIGH

    .scratchpad         : > RAMLS45
    .bss_cla            : > RAMLS6

    Cla1Prog            : LOAD = FLASH_APP_CODE,
                          RUN = RAMLS45,
                          LOAD_START(_ClaFuncsLoadStart),
                          LOAD_SIZE(_ClaFuncsLoadSize),
                          RUN_START(_ClaFuncsRunStart),
                          ALIGN(8)

    .const_cla          : LOAD = FLASH_APP_CODE,
                          RUN = RAMLS6,
                          LOAD_START(_ClaConstLoadStart),
                          LOAD_SIZE(_ClaConstLoadSize),
                          RUN_START(_ClaConstRunStart),
                          ALIGN(8)

    .signature          : > FLASH_SIGNATURE,
                          START(_start_of_approm)

    .appheader          : > FLASH_APP_HEADER
    .compliantaddress   : > COMPLIANT_ADDRESS
    .headerreserved     : > FLASH_HEADER_RES

    .complianttable     : > COMPLIANT_TABLE,
                          START(_start_of_complianttable)

    .approm             : > FLASH_APP_CODE,
                          START(_start_of_appcode),
                          END(_end_of_approm),
                          ALIGN(8)

    .appbank1           : START(_start_of_appbank1),
                          END(_end_of_appbank1)
                          {
                              . += SIZE(FLASH_BANK1_APP);
                          } > FLASH_BANK1_APP,
                          ALIGN(8)

    .reset              : > RESET, TYPE = DSECT /* not used, */
    .stack              : > RAMM1
    .ebss               : >> RAMGS12 | RAMGS3
    .esysmem            : > RAMLS7
    .cio                : > RAMLS7
    .ramgs0             : > RAMGS0
    .ramgs12            : > RAMGS12

    .boot_activate      : > BOOT_ACT_KEY,
                          START(_boot_activate),
						  TYPE = NOINIT

    DataBufferSection : > RAMM1, ALIGN(8)
}
//---------------- END LINE -----------------------------------------------------------------------
