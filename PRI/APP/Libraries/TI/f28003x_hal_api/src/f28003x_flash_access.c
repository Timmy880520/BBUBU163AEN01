/**************************************************************************************************
File Name: f28003x_flash_access.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None.
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    Fapi_initializeAPI                      F28003x Support Library
    Fapi_setActiveFlashBank                 F28003x Support Library
    Fapi_issueAsyncCommandWithAddress       F28003x Support Library
    Fapi_checkFsmForReady                   F28003x Support Library
    Fapi_issueProgrammingCommand            F28003x Support Library
    ServiceDog                              f28003x_sysctrl.c
    EALLOW                                  Extern compiler intrinsic prototypes
    EDIS                                    Extern compiler intrinsic prototypes
Description:
    Universal flash access function. This file uses FAPI_F28003x_EABI/COFF_v1.58.10.lib to
implement the internal flash access.

    This file does NOT include DCSM OTP programming/erase operations.

===================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    02/22/2023 Watch Lee        1. version 1.0.

**************************************************************************************************/

/* Includes */
#include "f28003x_flash_access.h"

/**************************************************************************************************
Function Name:
    void Init_Flash_API(unsigned char active)
Input:
    active       - Symbol Flash bank, uses FlashInitMode Enumeration.
Output:
    None.
Comment:
    Init flash function. It does not matter which bank is passed as the parameter to initialize.
Both Banks and FMC get initialized with one function call unlike F2837xS. Hence, there is no need
to execute Fapi_setActiveFlashBank() for each bank. i.e., only need to active FLASH_BANK0_ACTIVE
for using Flash_API library.
**************************************************************************************************/
void Init_Flash_API(unsigned char active)
{
    EALLOW;
    Fapi_initializeAPI(F021_CPU0_BASE_ADDRESS, CPU_Clock_MHz);

    if (active | FLASH_BANK0_ACTIVE)
        Fapi_setActiveFlashBank(Fapi_FlashBank0);
    else if (active | FLASH_BANK1_ACTIVE)
        Fapi_setActiveFlashBank(Fapi_FlashBank1);
    else if (active | FLASH_BANK2_ACTIVE)
        Fapi_setActiveFlashBank(Fapi_FlashBank2);
    EDIS;
}
/**************************************************************************************************
Function Name:
    unsigned char Flash_Sector_Erase(unsigned long startAddress, unsigned long romSize)
Input:
    startAddress - Page start address.
    romSize      - Available ROM size.
Output:
    return       - Result. 1 = completed, 0 = failed(Sector_Size and romSize are not math).
Comment:
    Flash sector erase function. This function will erase all Available ROM size.
**************************************************************************************************/
unsigned char Flash_Sector_Erase(unsigned long startAddress, unsigned long romSize)
{
    long i;

    if (romSize % Flash_Sector_Size)
        return 0;

    EALLOW;

    for (i = 0; i < romSize; i += Flash_Sector_Size)
    {
        Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, (uint32*)(startAddress + i));

        while (Fapi_checkFsmForReady() != Fapi_Status_FsmReady);

        ServiceDog();
    }
    EDIS;
    return 1;
}
/**************************************************************************************************
Function Name:
    unsigned char Flash_Word_Program(unsigned long startAddress, unsigned long length, unsigned int *data)
Input:
    startAddress - Start address of flash.
    length       - Data size, uses TiByte(16-bits) and must be a multiple of 4.
    *data        - Pointer of data.
Output:
    return       - Result. 1 = completed, 0 = failed(Data size is incorrect).
Comment:
    Flash API data program function.
**************************************************************************************************/
unsigned char Flash_Word_Program(unsigned long startAddress, unsigned long length, unsigned int *data)
{
    long i;

    if (length & 0x03)
        return 0;                   // Flash API only supports the minimum number data programming of 64-bits

    EALLOW;

    for (i = 0; i < (length >> 2); i++)
    {
        Fapi_issueProgrammingCommand((uint32*)startAddress + 2 * i, &data[4 * i], 4, 0, 0, Fapi_AutoEccGeneration);

        while (Fapi_checkFsmForReady() != Fapi_Status_FsmReady);

        ServiceDog();
    }
    EDIS;
    return 1;
}
//---------------- END LINE -----------------------------------------------------------------------
