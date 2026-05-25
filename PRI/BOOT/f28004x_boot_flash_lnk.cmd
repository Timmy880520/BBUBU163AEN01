
MEMORY
{
PAGE 0 :
    /* BEGIN is used for the "boot to Flash" bootloader mode   */

    BEGIN             : origin = 0x080000, length = 0x000002
    RAMM0             : origin = 0x0000F5, length = 0x00030B
    LS_RAM0           : origin = 0x008000, length = 0x000800 /* Local Shared RAM0 */
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
    FLASH_BANK1_SEC4  : origin = 0x094000, length = 0x001000 /* on-chip Flash */
    FLASH_BANK1_APP   : origin = 0x095000, length = 0x00A000 /* Bank1 app ROM field */
    FLASH_BANK1_SEC15 : origin = 0x09F000, length = 0x001000 /* on-chip Flash */

PAGE 1 :

    BOOT_RSVD         : origin = 0x000002, length = 0x0000F3 /* Part of M0, BOOT rom will use this for stack */
    RAMM1             : origin = 0x000400, length = 0x000400 /* on-chip RAM block M1 */
    LS_RAM1           : origin = 0x008800, length = 0x003800 /* Local Shared RAM1 */
    BOOT_ACT_KEY      : origin = 0x00C000, length = 0x000010 /* Boot activate key */
    GS_RAM            : origin = 0x00C010, length = 0x007FF0 /* Global Shared RAM */
}

SECTIONS
{
    codestart        : > BEGIN,                PAGE = 0, ALIGN(4)
    .text            : >>FLASH_BOOT_CODE,      PAGE = 0, ALIGN(4)

    .cinit           : > FLASH_BOOT_CODE,      PAGE = 0, ALIGN(4)
    .pinit           : > FLASH_BOOT_CODE,      PAGE = 0, ALIGN(4)
    .switch          : > FLASH_BOOT_CODE,      PAGE = 0, ALIGN(4)
    .reset           : > RESET,                PAGE = 0, TYPE = DSECT /* not used, */

    .cio             : > LS_RAM1,              PAGE = 1
    .stack           : > RAMM1,                PAGE = 1
    .ebss            : > LS_RAM1,              PAGE = 1
    .esysmem         : > LS_RAM1,              PAGE = 1
    .econst          : > FLASH_BOOT_CODE,      PAGE = 0, ALIGN(4)

    ramgs0           : > GS_RAM,               PAGE = 1
    .boot_activate   : START(_boot_activate)
                       {} > BOOT_ACT_KEY,      PAGE = 1

    .appromstart     : START(_start_of_approm)
                       {} > FLASH_APP_HEADER,  PAGE = 0

    .approm          : > FLASH_APP_CODE,
                         START(_start_of_appcode),
                         END(_end_of_approm)
                         {
                             . += SIZE(FLASH_APP_CODE);
                         },                    PAGE = 0

    .appbank1        : > FLASH_BANK1_APP,
                         START(_start_of_appbank1),
                         END(_end_of_appbank1)
                         {
                             . += SIZE(FLASH_BANK1_APP);
                         },                    PAGE = 0

    .bootfunctionid  : > BOOT_FUNCTION_ID,     PAGE = 0
    .bootversion     : > BOOT_VERSION,         PAGE = 0

    .TI.ramfunc      : { -l F021_API_F28004x_FPU32.lib }
                       LOAD = FLASH_BOOT_CODE,
                       RUN = LS_RAM0,
                       LOAD_START(_RamfuncsLoadStart),
                       LOAD_SIZE(_RamfuncsLoadSize),
                       LOAD_END(_RamfuncsLoadEnd),
                       RUN_START(_RamfuncsRunStart),
                       RUN_SIZE(_RamfuncsRunSize),
                       RUN_END(_RamfuncsRunEnd),
                       PAGE = 0, ALIGN(4)

    DataBufferSection: > RAMM1,                PAGE = 1, ALIGN(4)
}

/*
//===========================================================================
// End of file.
//===========================================================================
*/
