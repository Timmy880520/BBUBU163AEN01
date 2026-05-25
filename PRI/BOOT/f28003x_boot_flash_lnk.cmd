/**************************************************************************************************
File Name: f28003x_boot_flash_lnk.cmd
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
    02/20/2023 Watch Lee        1. create file.

**************************************************************************************************/

MEMORY
{
    /* BEGIN is used for the "boot to Flash" bootloader mode   */
    BEGIN             : origin = 0x080000, length = 0x000002
    BOOT_RSVD         : origin = 0x000002, length = 0x000126

    RAMM0             : origin = 0x000128, length = 0x0002D8
   	RAMM1             : origin = 0x000400, length = 0x000380 /* on-chip RAM block M1 */

    /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */
//  RAMM1_RSVD        : origin = 0x0007F8, length = 0x000008

    RAMLS0_1          : origin = 0x008000, length = 0x001000
    RAMLS2_7          : origin = 0x009000, length = 0x003000 /* Local Shared RAM1-7  */
/*
    RAMLS0            : origin = 0x008000, length = 0x000800
    RAMLS1            : origin = 0x008800, length = 0x000800
    RAMLS2            : origin = 0x009000, length = 0x000800
    RAMLS3            : origin = 0x009800, length = 0x000800
    RAMLS4            : origin = 0x00A000, length = 0x000800
    RAMLS5            : origin = 0x00A800, length = 0x000800
    RAMLS6            : origin = 0x00B000, length = 0x000800
    RAMLS7            : origin = 0x00B800, length = 0x000800
*/
    BOOT_ACT_KEY      : origin = 0x00C000, length = 0x000010 /* Boot activate key    */
    RAMGS0_3          : origin = 0x00C010, length = 0x003FE8 /* Global Shared RAM0-3 */
/*
    RAMGS0            : origin = 0x00C000, length = 0x001000
    RAMGS1            : origin = 0x00D000, length = 0x001000
    RAMGS2            : origin = 0x00E000, length = 0x001000
    RAMGS3            : origin = 0x00F000, length = 0x000FF8
*/
    /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */
//  RAMGS3_RSVD       : origin = 0x00FFF8, length = 0x000008

    BOOTROM           : origin = 0x3F8000, length = 0x007FC0
    SECURE_ROM        : origin = 0x3F2000, length = 0x006000

    RESET             : origin = 0x3FFFC0, length = 0x000002

    /* Flash sectors */
    /* BANK 0 */
    FLASH_BOOT_CODE   : origin = 0x080002, length = 0x004FEC /* IAP boot code       */
    BOOT_FUNCTION_ID  : origin = 0x084FEE, length = 0x000002 /* Function ID record  */
    BOOT_VERSION      : origin = 0x084FF0, length = 0x000010 /* Boot version record */
    FLASH_APP_HEADER  : origin = 0x085000, length = 0x000200 /* App header field    */
    FLASH_APP_CODE    : origin = 0x085200, length = 0x009E00 /* App code field      */
    FLASH_CALIBRATION : origin = 0x08F000, length = 0x001000 /* on-chip Flash       */

    /* BANK 1 */
    FLASH_BANK1_SEC0  : origin = 0x090000, length = 0x001000 /* on-chip Flash */
    FLASH_BANK1_SEC1  : origin = 0x091000, length = 0x001000 /* on-chip Flash */
    FLASH_BANK1_SEC2  : origin = 0x092000, length = 0x001000 /* on-chip Flash */
    FLASH_BANK1_SEC3  : origin = 0x093000, length = 0x001000 /* on-chip Flash */
    FLASH_BANK1_APP   : origin = 0x094000, length = 0x00A000 /* Bank1 app ROM field */
    FLASH_BANK1_SEC14 : origin = 0x09E000, length = 0x001000
    FLASH_BANK1_SEC15 : origin = 0x09F000, length = 0x000FF0
/*
    FLASH_BANK1_SEC0  : origin = 0x090000, length = 0x001000
    FLASH_BANK1_SEC1  : origin = 0x091000, length = 0x001000
    FLASH_BANK1_SEC2  : origin = 0x092000, length = 0x001000
    FLASH_BANK1_SEC3  : origin = 0x093000, length = 0x001000
    FLASH_BANK1_SEC4  : origin = 0x094000, length = 0x001000
    FLASH_BANK1_SEC5  : origin = 0x095000, length = 0x001000
    FLASH_BANK1_SEC6  : origin = 0x096000, length = 0x001000
    FLASH_BANK1_SEC7  : origin = 0x097000, length = 0x001000
    FLASH_BANK1_SEC8  : origin = 0x098000, length = 0x001000
    FLASH_BANK1_SEC9  : origin = 0x099000, length = 0x001000
    FLASH_BANK1_SEC10 : origin = 0x09A000, length = 0x001000
    FLASH_BANK1_SEC11 : origin = 0x09B000, length = 0x001000
    FLASH_BANK1_SEC12 : origin = 0x09C000, length = 0x001000
    FLASH_BANK1_SEC13 : origin = 0x09D000, length = 0x001000
    FLASH_BANK1_SEC14 : origin = 0x09E000, length = 0x001000
    FLASH_BANK1_SEC15 : origin = 0x09F000, length = 0x000FF0
*/
    /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */
//  FLASH_BANK1_RSVD  : origin = 0x09FFF0, length = 0x000010

    /* BANK 2 */
    FLASH_BANK2_SEC0  : origin = 0x0A0000, length = 0x001000
    FLASH_BANK2_SEC1  : origin = 0x0A1000, length = 0x001000
    FLASH_BANK2_SEC2  : origin = 0x0A2000, length = 0x001000
    FLASH_BANK2_SEC3  : origin = 0x0A3000, length = 0x001000
    FLASH_BANK2_SEC4  : origin = 0x0A4000, length = 0x001000
    FLASH_BANK2_SEC5  : origin = 0x0A5000, length = 0x001000
    FLASH_BANK2_SEC6  : origin = 0x0A6000, length = 0x001000
    FLASH_BANK2_SEC7  : origin = 0x0A7000, length = 0x001000
    FLASH_BANK2_SEC8  : origin = 0x0A8000, length = 0x001000
    FLASH_BANK2_SEC9  : origin = 0x0A9000, length = 0x001000
    FLASH_BANK2_SEC10 : origin = 0x0AA000, length = 0x001000
    FLASH_BANK2_SEC11 : origin = 0x0AB000, length = 0x001000
    FLASH_BANK2_SEC12 : origin = 0x0AC000, length = 0x001000
    FLASH_BANK2_SEC13 : origin = 0x0AD000, length = 0x001000
    FLASH_BANK2_SEC14 : origin = 0x0AE000, length = 0x001000
    FLASH_BANK2_SEC15 : origin = 0x0AF000, length = 0x000FF0

    /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */
//  FLASH_BANK2_RSVD  : origin = 0x0AFFF0, length = 0x000010
}

SECTIONS
{
    codestart         : > BEGIN, ALIGN(8)
    .text             : > FLASH_BOOT_CODE, ALIGN(8)
    .cinit            : > FLASH_BOOT_CODE, ALIGN(8)
    .switch           : > FLASH_BOOT_CODE, ALIGN(8)
    .reset            : > RESET, TYPE = DSECT /* not used */

    .stack            : > RAMM1

#if defined(__TI_EABI__)
    .bss              : > RAMLS2_7
    .bss:output       : > RAMLS2_7
    .init_array       : > FLASH_BOOT_CODE, ALIGN(8)
    .const            : > FLASH_BOOT_CODE, ALIGN(8)
    .data             : > RAMLS2_7
    .sysmem           : > RAMLS2_7
    .bss:cio          : > RAMLS2_7
#else
    .pinit            : > FLASH_BOOT_CODE, ALIGN(8)
    .econst           : > FLASH_BOOT_CODE, ALIGN(8)
    .ebss             : > RAMLS2_7
    .esysmem          : > RAMLS2_7
    .cio              : > RAMLS2_7
#endif

    ramgs0            : > RAMGS0_3
    .boot_activate    : START(_boot_activate)
                        {} > BOOT_ACT_KEY, TYPE = NOINIT

    .appromstart      : START(_start_of_approm)
                        {} > FLASH_APP_HEADER

    .approm           : > FLASH_APP_CODE,
                          START(_start_of_appcode),
                          END(_end_of_approm)
                          {
                              . += SIZE(FLASH_APP_CODE);
                          }

    .appbank1         : > FLASH_BANK1_APP,
                          START(_start_of_appbank1),
                          END(_end_of_appbank1)
                          {
                              . += SIZE(FLASH_BANK1_APP);
                          }

    .bootfunctionid   : > BOOT_FUNCTION_ID
    .bootversion      : > BOOT_VERSION

    .TI.ramfunc       : { -l FAPI_F28003x_COFF_v1.58.10.lib}
                        LOAD = FLASH_BOOT_CODE,
                        RUN = RAMLS0_1,
                        LOAD_START(_RamfuncsLoadStart),
                        LOAD_SIZE(_RamfuncsLoadSize),
                        LOAD_END(_RamfuncsLoadEnd),
                        RUN_START(_RamfuncsRunStart),
                        RUN_SIZE(_RamfuncsRunSize),
                        RUN_END(_RamfuncsRunEnd),
                        ALIGN(8)

    DataBufferSection : > RAMM1, ALIGN(8)
}
//---------------- END LINE -----------------------------------------------------------------------
