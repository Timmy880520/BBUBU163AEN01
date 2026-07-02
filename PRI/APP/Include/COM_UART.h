/**************************************************************************************************
File Name: COM_UART.h
Global Data:
    Name                     Type                Description
    ------------------------ ------------------- --------------------------------------------------
    bootPri                  BootDataType        Data of the boot code library.
    dmaLinaRxObj             DmaLinObj           Object of the RXD LIN BUS DMA driver.
    dmaLinaTxObj             DmaLinObj           Object of the TXD LIN BUS DMA driver.
    linaObj                  LinObj              Object of LIN BUS driver.
    intProtocol              IntProtocolObj      Internal protocol object.
Description:
    Header file of COM_UART.c.

===================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    04/15/2020 Watch Lee        1. version 1.0.
    03/09/2021  Fred Huang      Add pfcMonitorBuffer declaration
    03/10/2021  Fred Huang      Modify ORING_OVP to ORING_FAULT and SBYFLT to STB_FAULT

**************************************************************************************************/

#ifndef __COM_UART_H__
#define __COM_UART_H__

/* Includes */

#include "f28003x_dma_lin_driver.h"
#include "f28003x_lin_driver.h"
#include "INT_Protocol.h"
#include "c28x_intp_data_access.h"
#include "INT_Protocol_Table.h"
#include "Boot_Internal_Task.h"
#include "f28003x_boot.h"
#include "f28003x_sci_driver.h"
#include <stdint.h>

/* Macro definitions */

/* Type definitions */
// Structure & union

typedef union
{
    unsigned short word;
    struct
    {
        unsigned short low   :8;
        unsigned short high  :8;
    }byte;
}AccessKeyType;

typedef struct
{
    short offset;
    short gain;
}CalibrationData;

typedef union
{
    unsigned long all;
    struct
    {
        unsigned long reserved0             :8;

        unsigned long reserved1             :8;

        unsigned long prechargeOK           :1;
        unsigned long standbyOK             :1;
        unsigned long latchOK               :1;
        unsigned long reserved2             :2;
        unsigned long Estop1                :1;
        unsigned long Estop2                :1;
        unsigned long bbukill               :1;

        unsigned long dischargerNotReady    :1;
        unsigned long reserved3             :1;
        unsigned long datalogAlarm          :1;
        unsigned long faultShutdown         :1;
        unsigned long dcdcOverload          :1;
        unsigned long acLoss                :1;
        unsigned long chargerOK             :1;
        unsigned long dischargerOK          :1;
    }bit;

    struct
    {
        unsigned short low;
        unsigned short high;
    }bytes;
}DcdcStateType;

typedef union
{
    unsigned long all;
    struct
    {
        unsigned long reserved0             :1;
        unsigned long UvpBusSw              :1;
        unsigned long OvpBusChgIo           :1;//no temp sensor
        unsigned long OvpBusIo              :1;//no temp sensor
        unsigned long OvpBusSw              :1;
        unsigned long OvpBattIo             :1;
        unsigned long OvpChgBattIo          :1;
        unsigned long OvpChgBattSw          :1;

        unsigned long reserved1             :1;
        unsigned long reserved2             :1;
        unsigned long reserved3             :1;
        unsigned long IoutOcpChgSw          :1;
        unsigned long VinUvpDischgSw        :1;
        unsigned long PriOcpDischgIo        :1;
        unsigned long OcpDischgIo           :1;
        unsigned long OcpDischgSw           :1;

        unsigned long reserved4             :1;
        unsigned long reserved5             :1;
        unsigned long reserved6             :1;//no temp sensor
        unsigned long chargerVinUVW         :1;//no temp sensor
        unsigned long chargerVinOVW         :1;
        unsigned long chargerIoutOCW        :1;
        unsigned long chargerVoutUVW        :1;
        unsigned long chargerVoutOVW        :1;

        unsigned long reserved7             :1;
        unsigned long reserved8             :1;
        unsigned long reserved9             :1;
        unsigned long dischargerVinUVW      :1;
        unsigned long dischargerVinOVW      :1;
        unsigned long dischargerIoutOCW     :1;
        unsigned long dischargerVoutUVW     :1;
        unsigned long dischargerVoutOVW     :1;
    }bit;
}DcdcAlarmType;

typedef union
{
    unsigned short all;
    struct
    {
        unsigned short dcdcComFault          :1;
        unsigned short bmsComFault           :1;
        unsigned short faultFan2Rear         :1;
        unsigned short faultFan2Front        :1;
        unsigned short faultFan1Rear         :1;
        unsigned short faultFan1Front        :1;
        unsigned short ambOTW                :1;
        unsigned short ambOTP                :1;

        unsigned short dchgDDTempWarning     :1;
        unsigned short dchgDDTempFault       :1;
        unsigned short chgDDTempWarning      :1;
        unsigned short chgDDTempFault        :1;
        unsigned short oringTempWarning      :1;
        unsigned short oringTempFault        :1;
        unsigned short srTempWarning         :1;
        unsigned short srTempFault           :1;
    }bit;
}ComStateType;

typedef struct
{
    ComStateType   comState;
    unsigned short fanInletTemp;
    unsigned short chargeTemp;
    unsigned short dischargeFetTemp;
    unsigned short speedFan1;
    unsigned short speedFan2;
    unsigned short speedFan3;
    unsigned short speedFan4;
    unsigned short fanPWM;
    unsigned short srTemp;
    unsigned short oringTemp;
}ComDataType;

typedef struct
{
    unsigned short battOverallVolt;
    unsigned short battCellTemp;
    unsigned short battChargeCurr;
    unsigned short battDischargeCurr;
    unsigned short stateOfCharge;
    unsigned short stateOfHealth;
    unsigned short remainingBattCapcity;
//    unsigned short packFanTach;
}BmsDataType;

typedef union
{
    unsigned short data[8];
    struct
    {
        unsigned short reserved0                :8;
        unsigned short empty0                   :8;

        unsigned short reserved1                :3;
        unsigned short batteryHealthTestFail    :1;
        unsigned short batteryHealthTestSuccess :1;
        unsigned short batteryHealthTestRequired:1;
        unsigned short batteryHealthTestRunning :1;
        unsigned short batteryHealthTestReady   :1;
        unsigned short empty1                   :8;

        unsigned short prechargeOK              :1;
        unsigned short standbyOK                :1;
        unsigned short latchOK                  :1;
        unsigned short reserved2                :2;
        unsigned short Estop1                   :1;
        unsigned short Estop2                   :1;
        unsigned short bbukill                  :1;
        unsigned short empty2                   :8;

        unsigned short dischargerNotReady       :1;
        unsigned short reserved3                :1;
        unsigned short datalogAlarm             :1;
        unsigned short faultShutdown            :1;
        unsigned short dcdcOverload             :1;
        unsigned short acLoss                   :1;
        unsigned short chargerOK                :1;
        unsigned short dischargerOK             :1;
        unsigned short empty3                   :8;

        unsigned short chargeFET                :1;
        unsigned short dischargeFET             :1;
        unsigned short chargeEnOut              :1;
        unsigned short stopDischargeOut         :1;
        unsigned short failOut                  :1;
        unsigned short sohOut                   :1;
        unsigned short ddFault                  :1;
        unsigned short reserved4                :1;
        unsigned short empty4                   :8;

        unsigned short reserved5                :8;
        unsigned short empty5                   :8;

        unsigned short sohTest                  :1;
        unsigned short sohOcvRlxTime            :1;
        unsigned short cellBalance              :1;
        unsigned short reserved6                :1;
        unsigned short fullDischarge            :1;
        unsigned short fullCharge               :1;
        unsigned short discharging              :1;
        unsigned short initialized              :1;
        unsigned short empty6                   :8;

        unsigned short remainingTimeAlarm       :1;
        unsigned short remainingCapacityAlarm   :1;
        unsigned short reserved7                :1;
        unsigned short terminateDischargeAlarm  :1;
        unsigned short overTempAlarm            :1;
        unsigned short reserved8                :1;
        unsigned short terminateChargeAlarm     :1;
        unsigned short overChargeAlarm          :1;
        unsigned short empty7                   :8;
    }bit;
}BbuStateType;

typedef union
{
    unsigned short data[10];
    struct
    {
        unsigned short dcdcComFault             :1;
        unsigned short bmsComFault              :1;
        unsigned short fan2RearFault            :1;
        unsigned short fan2FrontFault           :1;
        unsigned short fan1RearFault            :1;
        unsigned short fan1FrontFault           :1;
        unsigned short ambOTW                   :1;
        unsigned short ambOTP                   :1;
        unsigned short empty0                   :8;

        unsigned short dischgD2DTempWarning     :1;
        unsigned short dischgD2DTempFault       :1;
        unsigned short chgD2DTempWarning        :1;
        unsigned short chgD2DTempFault          :1;
        unsigned short oringTempWarning         :1;
        unsigned short oringTempFault           :1;
        unsigned short srTempWarning            :1;
        unsigned short srTempFault              :1;
        unsigned short empty1                   :8;

        unsigned short reserved0                :1;
        unsigned short UvpBusSw                 :1;
        unsigned short OvpBusChgIo              :1;
        unsigned short OvpBusIo                 :1;
        unsigned short OvpBusSw                 :1;
        unsigned short OvpBattIo                :1;
        unsigned short OvpChgBattIo             :1;
        unsigned short OvpChgBattSw             :1;
        unsigned short empty2                   :8;

        unsigned short reserved1                :3;
        unsigned short IoutOcpChgSw             :1;
        unsigned short VinUvpDischgSw           :1;
        unsigned short PriOcpDischgIo           :1;
        unsigned short OcpDischgIo              :1;
        unsigned short OcpDischgSw              :1;
        unsigned short empty3                   :8;

        unsigned short reserved2                :3;
        unsigned short chargerVinUVW            :1;
        unsigned short chargerVinOVW            :1;
        unsigned short chargerIoutOCW           :1;
        unsigned short chargerVoutUVW           :1;
        unsigned short chargerVoutOVW           :1;
        unsigned short empty4                   :8;

        unsigned short reserved3                :3;
        unsigned short dischargerVinUVW         :1;
        unsigned short dischargerVinOVW         :1;
        unsigned short dischargerIoutOCW        :1;
        unsigned short dischargerVoutUVW        :1;
        unsigned short dischargerVoutOVW        :1;
        unsigned short empty5                   :8;

        unsigned short reserved4                :6;
        unsigned short adeComError              :1;
        unsigned short reserved5                :1;
        unsigned short empty6                   :8;

        unsigned short afeComError              :1;
        unsigned short calibDataError           :1;
        unsigned short fwChecksumError          :1;
        unsigned short reserved6                :2;
        unsigned short scpError                 :1;
        unsigned short lowVoltageErrorHw        :1;
        unsigned short reserved7                :1;
        unsigned short empty7                   :8;

        unsigned short cellOVP                  :1;
        unsigned short cellUVP                  :1;
        unsigned short swChargeOCP              :1;
        unsigned short swDischargeOCP           :1;
        unsigned short cellChargeOTP            :1;
        unsigned short cellChargeUTP            :1;
        unsigned short cellDischargeOTP         :1;
        unsigned short cellDischargeUTP         :1;
        unsigned short empty8                   :8;

        unsigned short fetOTP                   :1;
        unsigned short dcdcAlarm                :1;
        unsigned short sohAlarm                 :1;
        unsigned short sysAlarm                 :1;
        unsigned short reserved8                :4;
        unsigned short empty9                   :8;
    }bit;
}BbuAlarmType;

typedef union
{
    unsigned short all;
    struct
    {
        unsigned short swOVP                    :1;
        unsigned short hwOVP                    :1;
        unsigned short swUVP                    :1;
        unsigned short cellImbalanceError       :1;
        unsigned short dcdcFailure              :1;
        unsigned short cellOTPError             :1;
        unsigned short reserved0                :1;
        unsigned short thermistorError          :1;

        unsigned short reserved1                :3;
        unsigned short cFetError                :1;
        unsigned short dFetError                :1;
        unsigned short reserved2                :2;
        unsigned short epo                      :1;
    }bit;
}BbuFaultType;

typedef union
{
    unsigned short all;
    struct
    {
        unsigned short cFetEnable            :1;
        unsigned short dFetEnable            :1;
        unsigned short PredFetEnable         :1;
        unsigned short reserved1             :1;
        unsigned short reserved2             :1;
        unsigned short reserved3             :1;
        unsigned short reserved4             :1;
        unsigned short reserved5             :1;

        unsigned short reserved6             :8;
    }bit;
}BmsFetControlType;

typedef union
{
    unsigned short all;
    struct
    {
        unsigned short reserved0             :7;
        unsigned short other                 :1;

        unsigned short acLoss                :1;
        unsigned short canFail               :1;
        unsigned short reserved1             :1;
        unsigned short fwUpgrade             :1;
        unsigned short reserved2             :1;
        unsigned short notInstalled          :1;
        unsigned short fail                  :1;
        unsigned short lowDischargePower     :1;
    }bit;
}LearningStopReasonType;

/* Global function prototypes */
void UART_Routine(void);
void Process_UART_Data(void);
void Live_Update_State_Machine(void);
void Warning_Storage(void);
unsigned char Check_Bank1_App_Image(void);

/* Global data declarations */
extern BootInternalObj bootSec, bootPri;
extern BootDataType bootSecData;
extern DmaLinObj dmaLinaRxObj, dmaLinaTxObj;
extern LinObj linaObj;
extern IntProtocolObj intProtocol;

extern SciObj sciaObj;
extern IntProtocolObj sciProtocol;

extern unsigned short uartCalibrationMode;
extern unsigned char  uartSaveCalibration;
extern unsigned short uartEngineerMode;

extern DcdcStateType dcdcState;
extern DcdcAlarmType dcdcAlarm;
extern ComDataType comData;
extern BmsDataType bmsData;
extern BbuStateType bbuState;
extern BbuAlarmType bbuAlarm;
extern BbuFaultType bbuFault;
extern unsigned long pmiTimes;
extern BmsFetControlType bmsFetControl;
extern LearningStopReasonType learningCycle;
extern unsigned long overWrietTimer;

#endif
