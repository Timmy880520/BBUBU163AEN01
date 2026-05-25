/**************************************************************************************************
File Name: main.c
IDE Info: Code Composer Studio v10.2.0.00009
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    bootCommand                             INT_Protocol_Table.c
    canaObj
    dmaLinaRxObj
    dmaLinaTxObj
    linaObj
    bootProtocol
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    Init_Peripheral                         Init_Peripheral.c
    Init_Flash_API                          f28004x_flash_access.c
    ServiceDog                              f28004x_sysctrl.c
    Call_Timer                              Univ_Lib.h
    Timer_Reset                             Univ_Lib.h
    Check_Boot_Activate_Code                f28004x_boot.c
    Write_Boot_Activate_Code                f28004x_boot.c
    Boot_Reset_MCU                          f28004x_boot.c
    Boot_UART_Routine                       Boot_UART.c
    Boot_UART_Data_Process                  Boot_UART.c
    CAN_Download_Routine                    CAN_Download.c
    Boot_CANRX_ISR_Task                     Boot_COM_CAN.c
    Boot_Process_CAN_Data                   Boot_COM_CAN.c
	CAN_Download_Tick                       CAN_Download.c
Description:
    Main task of BOOT code for Secondary(external CANBUS communication).

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    10/07/2020 Watch Lee        1. version 1.0.
    05/06/2021 Watch Lee        1. version 1.1.
                                2. modify Boot_State_Verification to support LFU.
                                3. insert CAN_Download_Tick to CPU_Timer1_ISR.
                                4. modify f28004x_boot_flash_lnk.cmd.

**************************************************************************************************/

/* Includes */
#include "F28x_Project.h"
#include "Global_Var.h"

/* Function prototypes */
void Boot_State_Verification(unsigned char waiveBoot);
void Backup_Image_Recovery(void);

/* Data definitions */
#pragma SET_DATA_SECTION(".bootfunctionid")
const WordType FUNCTION_ID = {Bin_Char4(Function_ID)};
#pragma SET_DATA_SECTION()

#pragma SET_DATA_SECTION(".bootversion")
const CodeVersionType BOOT_VERSION = Version_List;
#pragma SET_DATA_SECTION()

// Delay entry APP for the debug mode
Create_Timer(delayEntryApp, Count_1s);

// Delay entry APP for the debug mode
Create_Timer(delayLedBlinking, Count_100ms);

// Address detection counter
Create_Counter(checkGpioA0, Count_100ms);
Create_Counter(checkGpioA1, Count_100ms);
Create_Counter(checkGpioA2, Count_100ms);

// App code start address
unsigned long beginApp = App_Code_Start;
unsigned char debugCheck = 0;
unsigned char sendEvent = 0;

/**************************************************************************************************
Function Name:
    void main(void)
Input:
    None.
Output:
    None.
Comment:
    Main routine of the project.
**************************************************************************************************/
void main(void)
{
    Init_Peripheral();

    Init_Flash_API(FLASH_BANK0_ACTIVE);

    dmaLinaRxObj.func.init(&dmaLinaRxObj, (long)&LinaRegs.SCIRD);
    dmaLinaTxObj.func.init(&dmaLinaTxObj, (long)&LinaRegs.SCITD);

    linaObj.func.init(&linaObj, DMA_LIN_ENABLE);
    sciaObj.func.init(&sciaObj, SCI_FIFO_ENABLE);

    intpPri.func.init(&intpPri, &intpPriCommand);
    intpSec.func.init(&intpSec, &intpSecCommand);

    Boot_State_Verification(0);

    for (;;)
    {
        // Free loop
        ServiceDog();

        if (!debugCheck && Timer_Flag(delayEntryApp))
        {
            if (*(unsigned long*)App_ROM_Start == Bin_Char4("DBG"))
                Boot_State_Verification(1);
            else
                Timer_Reset(delayEntryApp);
            debugCheck = 1;
        }

        if(Timer_Flag(delayLedBlinking))
        {
            _toggleGPIO_DEBUG;

            Timer_Reset(delayLedBlinking);
        }

        if (sendEvent)
        {
            sendEvent = 0;
            intpPri.func.packTxData(&intpPri, BOOT_PROTECTION, INTERNAL_SET, LOW_PRIORITY);
        }
    }
}
/**************************************************************************************************
Function Name:
    __interrupt void CPU_Timer1_ISR(void)
Input:
    None.
Output:
    None.
Comment:
    C28x CPU Timer1 ISR function.
**************************************************************************************************/
__interrupt void CPU_Timer1_ISR(void)
{
//    _setGPIO_DEBUG;
    Call_Timer(delayEntryApp);
    Call_Timer(delayLedBlinking);

    Boot_UART_Routine();

    Boot_UART_Data_Process();

//    _clearGPIO_DEBUG;
}
/**************************************************************************************************
Function Name:
    void Boot_State_Verification(unsigned char waiveBoot)
Input:
    waiveBoot   - Flag of waiving boot. 1 = waive boot verification. 0 = execute boot verification.
Output:
    None.
Comment:
    Check bootloader state function. This function will check the boot init data that stored in
the backup register and the boot state.

The behavior of boot mode are detailed as follows:
BOOT_LOCKED:
    Normal operation. MCU will exit the boot mode and jump to app.
BOOT_UNLOCKED:
    Received BOOT_INIT. MCU will reply BOOT_INIT and ready to receive image data.
BOOT_ERROR:
    Detected error(s). MCU waits to receive BOOT_INIT.
**************************************************************************************************/
void Boot_State_Verification(unsigned char waiveBoot)
{
    EnumBootState stateBank0 = bootSec.func.init(&bootSec, 0, App_ROM_Start, App_ROM_Size);
    EnumBootState stateBank1 = bootSec.func.init(&bootSec, 0, Bank1_App_ROM_Start, Bank1_App_ROM_Size);

    if (Check_Boot_Activate_Code(Boot_Activate_Key, sizeof(Boot_Activate_Key)))
    {
        if (stateBank1 == BOOT_LOCKED)
        {
            // Copy app image of Bank1 to Bank0
            Backup_Image_Recovery();
            stateBank0 = bootSec.func.init(&bootSec, 0, App_ROM_Start, App_ROM_Size);
        }
    }
    else
    {
        stateBank0 = bootSec.func.init(&bootSec, 0, App_ROM_Start, App_ROM_Size);
        if (stateBank0 != BOOT_LOCKED && stateBank1 == BOOT_LOCKED)
        {
            // Copy app image of Bank1 to Bank0
            Backup_Image_Recovery();
            stateBank0 = bootSec.func.init(&bootSec, 0, App_ROM_Start, App_ROM_Size);
        }
    }

    // Reset boot command record
    Write_Boot_Activate_Code(0, 0);

    if (stateBank0 == BOOT_LOCKED || waiveBoot)
    {
        // Reset peripheral
        Deinit_Periphral();

        // Disable CPU interrupts
        DINT;

        // Jump to app
        asm(" .global _beginApp");
        asm(" MOVL XAR2, #_beginApp");
        asm(" MOVL XAR7, *XAR2");
        asm(" LB *XAR7");
//        ((void (*)())App_Code_Start)(); not support C
    }
//    else if (stateBank0 == BOOT_UNLOCKED)
//    {
//        GpioDataRegs.GPBSET.bit.GPIO56 = 1;    // Default HIGH, OFF
//        bootSec.member.message.initKey = BOOT_INIT_KEY;
//        intpSec.func.packTxData(&intpSec, BOOT_INIT, INTERNAL_ACK, LOW_PRIORITY);
//    }
}
/**************************************************************************************************
Function Name:
    void Backup_Image_Recovery(void)
Input:
    None.
Output:
    None.
Comment:
    Backup image recovery process. This function will use the backup image to overwrite the app
field if the backup image exists.
**************************************************************************************************/
void Backup_Image_Recovery(void)
{
    unsigned long appSizeBank1 = *((unsigned long*)Bank1_App_ROM_Start + 12) >> 1;
    if (appSizeBank1 < App_ROM_Size)
    {
        // erase app field
        Flash_Sector_Erase(App_ROM_Start, App_ROM_Size);
        // copy backup image to app field
        Flash_Word_Program(App_ROM_Start, appSizeBank1, (unsigned int*)Bank1_App_ROM_Start);
    }
}
//---------------- END LINE -----------------------------------------------------------------------
