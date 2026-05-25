/**************************************************************************************************
File Name: f28003x_boot.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    CpuSysRegs                              f28003x_globalvariabledefs.c
    WdRegs                                  f28003x_globalvariabledefs.c
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    EALLOW                                  External compiler intrinsic prototypes
    EDIS                                    External compiler intrinsic prototypes
    Flash_Sector_Erase                      f28003x_flash_access.c
    Flash_Word_Program                      f28003x_flash_access.c
Description:
    TI C28x DSP Boot function program.

    This file does NOT include the flash bank2 programming/erase operations.

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    02/22/2023 Watch Lee        1. version 1.0.

**************************************************************************************************/

/* Includes */
#include "F28x_Project.h"
#include "f28003x_flash_access.h"
#include "f28003x_boot.h"

/* Function prototypes */

/* Data definitions */

/**************************************************************************************************
Function Name:
    unsigned char Check_Boot_Activate_Code(void *dataPtr, unsigned long length)
Input:
    *dataPtr    - Pointer of input data.
    length      - Size of input data.
Output:
    return      - Result. 1 = Activate code is correct. 0 = Activate code is incorrect.
Comment:
    Check Boot activate code function. This function will check the boot activate code and return
1 if the code is correct.
**************************************************************************************************/
unsigned char Check_Boot_Activate_Code(void *dataPtr, unsigned long length)
{
    if (CpuSysRegs.RESC.bit.WDRSn)
    {
        CpuSysRegs.RESC.bit.WDRSn = 1;
        return Boot_Check_Backup_Data(Boot_Activate, dataPtr, length);
    }
    return 0;
}
/**************************************************************************************************
Function Name:
    unsigned char Boot_Check_Backup_Data(unsigned long address, void *dataPtr, unsigned long length)
Input:
    address     - Address of backup data.
    *dataPtr    - Pointer of input data.
    length      - Size of input data.
Output:
    return      - Result. 1 = Backup data is correct. 0 = Backup data is incorrect.
Comment:
    Boot check backup data function. This function will check backup data and return 1 if the data
is correct.
**************************************************************************************************/
unsigned char Boot_Check_Backup_Data(unsigned long address, void *dataPtr, unsigned long length)
{
    int i = 0;

    length = (length > 16) ? 16 : length;

    for (i = 0; i < length; i++)
    {
        if (*((unsigned char*)address + i) != *((unsigned char*)dataPtr + i))
            break;
    }
    return (i == length) ? 1 : 0;
}
/**************************************************************************************************
Function Name:
    void Write_Boot_Activate_Code(void *dataPtr, unsigned long length)
Input:
    *dataPtr    - Pointer of input data.
    length      - Size of input data.
Output:
    None.
Comment:
    Write boot activate code function. This function will write input data to the address of boot
activate code. The activate code will be written zero if the input argument(dataPtr) is zero.
**************************************************************************************************/
void Write_Boot_Activate_Code(void *dataPtr, unsigned long length)
{
    unsigned char *writeAddress = (unsigned char*)Boot_Activate;

    length = (!length || length > 16) ? 16 : length;

    for (int i = 0; i < length; i++)
        *(writeAddress + i) = (!dataPtr) ? 0 : *((unsigned char*)dataPtr + i);
}
/**************************************************************************************************
Function Name:
    unsigned char Boot_Program_Flash(unsigned long offset, void *dataPtr, unsigned long size)
Input:
    offset      - Offset byte from App ROM start address.
    *dataPtr    - Pointer of program data
    size        - Number of program data bytes.
Output:
    return      - Result. 1 = completed, 0 = failed(Data size is incorrect).
Comment:
    Boot program flash function. This function will write the program data to App ROM field. The
App ROM field will be erased if offset is zero.
**************************************************************************************************/
unsigned char Boot_Program_Flash(unsigned long offset, void *dataPtr, unsigned long size)
{
    unsigned long programAddress = App_ROM_Start + (offset >> 1);
    unsigned int buffer[4];

    if ((programAddress + (size >> 1)) > App_Code_End && (size & 0x7))
        return 0;
    else if (!offset)
        Flash_Sector_Erase(App_ROM_Start, App_ROM_Size);

    for (unsigned long i = 0; i < size; i += 8)
    {
/*        for (int j = 0; j < sizeof(buffer); j++)
        {
            buffer[j] = (*((uint16*)dataPtr + i + 2 * j) & 0xFF) |
                        ((*((uint16*)dataPtr + i + 2 * j + 1) << 8) & 0xFF00);
        }*/
        buffer[0] = (*((uint16*)dataPtr + i + 0) & 0xFF) | ((*((uint16*)dataPtr + i + 1) << 8) & 0xFF00);
        buffer[1] = (*((uint16*)dataPtr + i + 2) & 0xFF) | ((*((uint16*)dataPtr + i + 3) << 8) & 0xFF00);
        buffer[2] = (*((uint16*)dataPtr + i + 4) & 0xFF) | ((*((uint16*)dataPtr + i + 5) << 8) & 0xFF00);
        buffer[3] = (*((uint16*)dataPtr + i + 6) & 0xFF) | ((*((uint16*)dataPtr + i + 7) << 8) & 0xFF00);

        Flash_Word_Program(programAddress + (i >> 1), sizeof(buffer), buffer);
    }
    return 1;
}
/**************************************************************************************************
Function Name:
    unsigned char Boot_Program_Flash_Bank1(unsigned long offset, void *dataPtr, unsigned long size)
Input:
    offset      - Offset byte from bank1 app ROM start address.
    *dataPtr    - Pointer of program data
    size        - Size of program data.
Output:
    return      - Result. 1 = completed, 0 = failed(Data size is incorrect).
Comment:
    Boot program flash bank1 function. This function will write the program data to the bank1 app
ROM field. The bank1 app ROM field will be erased if offset is zero.
**************************************************************************************************/
unsigned char Boot_Program_Flash_Bank1(unsigned long offset, void *dataPtr, unsigned long size)
{
    unsigned long programAddress = Bank1_App_ROM_Start + (offset >> 1);
    unsigned int buffer[4];

    if ((programAddress + (size >> 1)) > Bank1_App_ROM_End && (size & 0x7))
        return 0;
    else if (!offset)
        Flash_Sector_Erase(Bank1_App_ROM_Start, Bank1_App_ROM_Size);

    for (unsigned long i = 0; i < size; i += 8)
    {
        buffer[0] = (*((uint16*)dataPtr + i + 0) & 0xFF) | ((*((uint16*)dataPtr + i + 1) << 8) & 0xFF00);
        buffer[1] = (*((uint16*)dataPtr + i + 2) & 0xFF) | ((*((uint16*)dataPtr + i + 3) << 8) & 0xFF00);
        buffer[2] = (*((uint16*)dataPtr + i + 4) & 0xFF) | ((*((uint16*)dataPtr + i + 5) << 8) & 0xFF00);
        buffer[3] = (*((uint16*)dataPtr + i + 6) & 0xFF) | ((*((uint16*)dataPtr + i + 7) << 8) & 0xFF00);

        Flash_Word_Program(programAddress + (i >> 1), sizeof(buffer), buffer);
    }
    return 1;
}
/**************************************************************************************************
Function Name:
    void Boot_Reset_MCU(void)
Input:
    None.
Output:
    None.
Comment:
    Reset MCU function.
**************************************************************************************************/
void Boot_Reset_MCU(void)
{
    EALLOW;
    WdRegs.WDCR.bit.WDCHK = 1;
    EDIS;
}
//---------------- END LINE -----------------------------------------------------------------------
