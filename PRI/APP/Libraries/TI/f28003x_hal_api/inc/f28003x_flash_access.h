/**************************************************************************************************
File Name: f28003x_flash_access.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ -----------------------------------------------------
    None
Description:
    Header file of f28003x_flash_access.c.

===================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    02/22/2023 Watch Lee        1. support version 1.x.

**************************************************************************************************/

#ifndef __F28003X_FLASH_ACCESS_H__
#define __F28003X_FLASH_ACCESS_H__

/* Includes */
#include "F28x_Project.h"
#include "F021_F28003x_C28x.h"

/* Macro definitions */
// Statement Replacement
#ifndef CPU_Clock_MHz
#define CPU_Clock_MHz                   120     // 120 MHz System frequency
#endif

#ifndef Flash_Sector_Size
#define Flash_Sector_Size               (unsigned long)0x1000
#endif

/* Type definitions */
// Enumeration
typedef enum
{
    FLASH_BANK_INACTIVE   = 0,
    FLASH_BANK0_ACTIVE    = 1,
    FLASH_BANK1_ACTIVE    = 2,
    FLASH_BANK2_ACTIVE    = 3
}FlashInitMode;

/* Global function prototypes */
void Init_Flash_API(unsigned char active);
unsigned char Flash_Sector_Erase(unsigned long startAddress, unsigned long romSize);
unsigned char Flash_Word_Program(unsigned long startAddress, unsigned long length, unsigned int *data);

/* Global data declarations */

#endif
