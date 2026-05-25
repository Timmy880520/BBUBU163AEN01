/***************************************************************************************************
File Name: Io.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ --------------------------------------------------------

Description:

====================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- --------------------------------------------------------------------
    11/06/2019 Fred             Create file
    03/09/2026 Aiden            Modify to the coding style
***************************************************************************************************/

#ifndef _IO_H_
#define _IO_H_

//------------------------------------------------------------------------------
// Macros Pin Definitions
//------------------------------------------------------------------------------
//input
#define In_Sync_Start                       Gpio_Pin(B,15) // GPIO47
#define In_BattOVP                          Gpio_Pin(B,23) // GPIO55 not used
#define In_BBU_Kill                         Gpio_Pin(B,28) // GPIO60
#define In_Charge_EN                        Gpio_Pin(A,20)
#define In_AC_Loss                          Gpio_Pin(A,25)
#define In_EM_Stop2                         Gpio_Pin(A,23)
#define In_EM_Stop1                         Gpio_Pin(B,9)  // GPIO41
#define In_Batt_Charge_OVP                  Gpio_Pin(A,9)  // not used
#define In_Output_OVP                       Gpio_Pin(A,31) // not used
#define In_Fault_Out                        Gpio_Pin(A,6)
#define In_SOH_Out                          Gpio_Pin(A,21)
#define In_STP_Discharge                    Gpio_Pin(A,4)
#define In_Reset                            Gpio_Pin(A,29) // GPIO29

//output
#define Out_Sync_Start                      Gpio_Pin(B,18) // GPIO50 (50-32=18)
#define Out_Enable_Oring                    Gpio_Pin(B,21) // GPIO53
#define Out_Fault_Shutdown                  Gpio_Pin(A,5)
#define Out_Discharge2                      Gpio_Pin(B,1)  // GPIO33
#define Out_Discharge1                      Gpio_Pin(B,24) // GPIO56
#define Out_Enable_SR_Driver                Gpio_Pin(B,25) // GPIO57
#define Out_Disable_CSbus                   Gpio_Pin(B,26) // GPIO58
#define Out_WC_Protect                      Gpio_Pin(A,8)  // not used
#define Out_Enable_Charge                   Gpio_Pin(B,8)  // GPIO40
#define Out_Enable_LLC_Driver               Gpio_Pin(B,12) // GPIO44
#define Out_Relay_Precharge1                Gpio_Pin(A,28)
#define Out_Relay_Precharge2                Gpio_Pin(B,20) // GPIO52
#define Out_Relay_AUX                       Gpio_Pin(B,22) // GPIO54
#define Out_Relay_Ctrl_Batt                 Gpio_Pin(B,19) // GPIO51
#define Out_DD_Fault                        Gpio_Pin(A,30)
#define Out_Dislatch_Charge_OVP             Gpio_Pin(B,27) // GPIO59 not used
#define Out_Dislatch_Discharge_In_OVP       Gpio_Pin(B,29) // GPIO61 not used
#define Out_Dislatch_Discharge_Out_OVP      Gpio_Pin(B,2)  // GPIO34 not used

//------------------------------------------------------------------------------
// Macro Definitions
//------------------------------------------------------------------------------
#define Gpio_Pin(p,n)                       p,n
#define Port_Data_REG(p)                    GpioDataRegs.GP##p##DAT.all
#define Port_Set_REG(p)                     GpioDataRegs.GP##p##SET.all
#define Port_Clear_REG(p)                   GpioDataRegs.GP##p##CLEAR.all
#define Port_Toggle_REG(p)                  GpioDataRegs.GP##p##TOGGLE.all

#define _In_Pin(p,n)                        ((Port_Data_REG(p) >> n) & 0x0001)
#define In_Pin(pin)                         _In_Pin(pin)

#define _Out_Pin(p,n)                       ((Port_Data_REG(p) >> n) & 0x0001)
#define Out_Pin(pin)                        _Out_Pin(pin)

#define _Set_Pin(p,n)                       (Port_Set_REG(p) |= (1UL << n))
#define Set_Pin(pin)                        _Set_Pin(pin)

#define _Clear_Pin(p,n)                     (Port_Clear_REG(p) |= (1UL << n))
#define Clear_Pin(pin)                      _Clear_Pin(pin)

#define _Toggle_Pin(p,n)                    (Port_Toggle_REG(p) |= (1UL << n))
#define Toggle_Pin(pin)                     _Toggle_Pin(pin)

//------------------------------------------------------------------------------
// Macro Input Definitions
//------------------------------------------------------------------------------
#define AC_Loss_Status()                    (In_Pin(In_AC_Loss))
#define AC_Loss()                           (In_Pin(In_AC_Loss) == 0)
#define NON_AC_Loss()                       (In_Pin(In_AC_Loss) == 1)

#define BBU_Kill_Status()                   (In_Pin(In_BBU_Kill))
#define BBU_Kill()                          (In_Pin(In_BBU_Kill) == 0)
#define NON_BBU_Kill()                      (In_Pin(In_BBU_Kill) == 1)

#define EM_Stop1_Status()                   (In_Pin(In_EM_Stop1))
#define EM_Stop1()                          (In_Pin(In_EM_Stop1) == 0)
#define NON_EM_Stop1()                      (In_Pin(In_EM_Stop1) == 1)

#define EM_Stop2_Status()                   (In_Pin(In_EM_Stop2))
#define EM_Stop2()                          (In_Pin(In_EM_Stop2) == 1)
#define NON_EM_Stop2()                      (In_Pin(In_EM_Stop2) == 0)

#define SYNC_Start_Status()                 (In_Pin(In_Sync_Start))
#define SYNC_Start()                        (In_Pin(In_Sync_Start) == 0)
#define NON_SYNC_Start()                    (In_Pin(In_Sync_Start) == 1)

#define CHG_EN_Out_Status()                 (In_Pin(In_Charge_EN))
#define CHG_EN_Out()                        (In_Pin(In_Charge_EN) == 0)
#define NON_CHG_EN_Out()                    (In_Pin(In_Charge_EN) == 1)

#define SOH_Out_Status()                    (In_Pin(In_SOH_Out))
#define SOH_Test()                          (In_Pin(In_SOH_Out) == 0)
#define NON_SOH_Test()                      (In_Pin(In_SOH_Out) == 1)

#define STP_DISCHG_Out_Status()             (In_Pin(In_STP_Discharge_Out))
#define STP_DISCHG_Out()                    (In_Pin(In_STP_Discharge_Out) == 0)
#define NON_STP_DISCHG_Out()                (In_Pin(In_STP_Discharge_Out) == 1)

#define Fail_Out_Status()                   (In_Pin(In_Fault_Out))
#define Fail_Out()                          (In_Pin(In_Fault_Out) == 0)
#define NON_Fail_Out()                      (In_Pin(In_Fault_Out) == 1)

#define Reset_Status()                      (In_Pin(In_Reset))
#define Reset()                             (In_Pin(In_Reset) == 0)
#define NON_Reset()                         (In_Pin(In_Reset) == 1)

//------------------------------------------------------------------------------
// Macro Output Definitions
//------------------------------------------------------------------------------
#define Oring_Status()                      Out_Pin(Out_Enable_Oring)
#define Oring_Off()                         Set_Pin(Out_Enable_Oring)
#define Oring_On()                          Clear_Pin(Out_Enable_Oring)

#define IBUS_Status()                       Out_Pin(Out_Disable_CSbus)
#define IBUS_Disable()                      Set_Pin(Out_Disable_CSbus)
#define IBUS_Enable()                       Clear_Pin(Out_Disable_CSbus)

#define LLC_Driver_Disable()                Set_Pin(Out_Enable_LLC_Driver)
#define LLC_Driver_Enable()                 Clear_Pin(Out_Enable_LLC_Driver)

#define SR_Driver_Enable()                  Set_Pin(Out_Enable_SR_Driver)
#define SR_Driver_Disable()                 Clear_Pin(Out_Enable_SR_Driver)

#define Out_DISCH1_Disable()                Set_Pin(Out_Discharge1)
#define Out_DISCH1_Enable()                 Clear_Pin(Out_Discharge1)

#define Out_DISCH2_Disable()                Set_Pin(Out_Discharge2)
#define Out_DISCH2_Enable()                 Clear_Pin(Out_Discharge2)

#define BBU_Fault()                         Set_Pin(Out_Fault_Shutdown)
#define NON_BBU_Fault()                     Clear_Pin(Out_Fault_Shutdown)

#define NON_DD_Fault()                      Set_Pin(Out_DD_Fault)
#define DD_Fault()                          Clear_Pin(Out_DD_Fault)

#define CHG_Driver_Disable()                Set_Pin(Out_Enable_Charge)
#define CHG_Driver_Enable()                 Clear_Pin(Out_Enable_Charge)

#define SYNC_Start_Out_Active()             Set_Pin(Out_Sync_Start)
#define SYNC_Start_Out_Inactive()           Clear_Pin(Out_Sync_Start)

#define DISCHG_In_OVP_Unlatch_Inactive()    Set_Pin(Out_Dislatch_Discharge_Out_OVP)
#define DISCHG_In_OVP_Unlatch_Active()      Clear_Pin(Out_Dislatch_Discharge_Out_OVP)

#define DISCHG_Out_OVP_Unlatch_Inactive()   Set_Pin(Out_Dislatch_Discharge_In_OVP)
#define DISCHG_Out_OVP_Unlatch_Active()     Clear_Pin(Out_Dislatch_Discharge_In_OVP)

#define CHG_OVP_Unlatch_Inactive()          Set_Pin(Out_Dislatch_Charge_OVP)
#define CHG_OVP_Unlatch_Active()            Clear_Pin(Out_Dislatch_Charge_OVP)

#define Bypass_AUX_RLY()                    Set_Pin(Out_Relay_AUX)
#define Turnoff_AUX_RLY()                   Clear_Pin(Out_Relay_AUX)

#define Bypass_PRECHG1_RLY()                Set_Pin(Out_Relay_Precharge1)
#define Turnoff_PRECHG1_RLY()               Clear_Pin(Out_Relay_Precharge1)

#define Bypass_PRECHG2_RLY()                Set_Pin(Out_Relay_Precharge2)
#define TURNOFF_PRECHG2_RLY()               Clear_Pin(Out_Relay_Precharge2)

#define Bypass_BATT_RLY()                   Set_Pin(Out_Relay_Ctrl_Batt)
#define Turnoff_BATT_RLY()                  Clear_Pin(Out_Relay_Ctrl_Batt)

//------------------------------------------------------------------------------
// Macro DAC Definitions
//------------------------------------------------------------------------------
#define DACA_VAL(v)                         DacaRegs.DACVALS.all = (v)
#define DACB_VAL(v)                         DacbRegs.DACVALS.all = (v)

#endif
