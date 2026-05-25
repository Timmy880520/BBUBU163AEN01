/**************************************************************************************************
File Name: f28003x_boot.h
Global Data:
    Name                     Type               Description
    ------------------------ ------------------ --------------------------------------------------
    start_of_appcode         CCS cmd export     Constant of app code start address.
    start_of_approm          CCS cmd export     Constant of app rom start address.
    end_of_approm            CCS cmd export     Constant of app rom end address.
    boot_activate            CCS cmd export     Constant of the boot activate code address.
    start_of_appbank1        CCS cmd export     Constant of bank1 app rom start address.
    end_of_appbank1          CCS cmd export     Constant of bank1 app rom end address.
Description:
    Header file of f28003x_boot.c.

===================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    02/22/2023 Watch Lee        1. support version 1.x.

**************************************************************************************************/

#ifndef __F28003X_BOOT_H__
#define __F28003X_BOOT_H__

/* Includes */

/* Macro definitions */
#define Set_Bank0_Symbol        1
#define Set_Bank1_Symbol        1
#define Set_Activate_Symbol     1

// Flash definition
#if Set_Bank0_Symbol==1
#define App_ROM_Start           (unsigned long)(&start_of_approm)
#define App_Code_Start          (unsigned long)(&start_of_appcode)      // Code offset = 0x200
#define App_Code_End            (unsigned long)(&end_of_approm)
#define App_ROM_Size            ((unsigned long)&end_of_approm - (unsigned long)&start_of_approm)       // TiByte
#else
#define App_ROM_Start           0x080000UL
#define App_Code_Start          0x080000UL
#define App_Code_End            0x08F000UL
#define App_ROM_Size            0x00F000UL
#endif

#if Set_Bank1_Symbol==1
#define Bank1_App_ROM_Start     (unsigned long)(&start_of_appbank1)
#define Bank1_App_ROM_End       (unsigned long)(&end_of_appbank1)
#define Bank1_App_ROM_Size      ((unsigned long)&end_of_appbank1 - (unsigned long)&start_of_appbank1)   // TiByte
#else
#define Bank1_App_ROM_Start     0x090000UL
#define Bank1_App_ROM_End       0x090000UL
#define Bank1_App_ROM_Size      0x09F000UL
#endif

#if Set_Activate_Symbol==1
#define Boot_Activate           (unsigned long)(&boot_activate)
#else
#define Boot_Activate           0x00FF00UL
#endif

// Boot activate key
#define Boot_Activate_Key       "ENTER_BOOT_MODE"

// Statement Replacement
#define Get_Char(Str, Num)      (sizeof(Str) > Num ? Str[Num] : 0)
#define Bin_Char4(Name)         ((long)Get_Char(Name, 3) << 24 | \
                                 (long)Get_Char(Name, 2) << 16 | \
                                 (long)Get_Char(Name, 1) << 8  | \
                                 (long)Get_Char(Name, 0))

/* Type definitions */
// Enumeration

/* Global function prototypes */
unsigned char Check_Boot_Activate_Code(void *dataPtr, unsigned long length);
unsigned char Boot_Check_Backup_Data(unsigned long address, void *dataPtr, unsigned long length);
void Write_Boot_Activate_Code(void *dataPtr, unsigned long length);
unsigned char Boot_Program_Flash(unsigned long offset, void *dataPtr, unsigned long size);
unsigned char Boot_Program_Flash_Bank1(unsigned long offset, void *dataPtr, unsigned long size);
void Boot_Reset_MCU(void);

/* Global data declarations */

// CCS CMD file exportation
extern unsigned long start_of_appcode, start_of_approm, end_of_approm, boot_activate;
extern unsigned long start_of_appbank1, end_of_appbank1;

#endif
