/**************************************************************************************************
File Name: EventLog_Table.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    lastChar                                Global_Var.h
    accumulateData                          Global_Var.h
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None
Description: 
    Table of event log contents, the event log and accumulated data table should be modified 
by different projects.

===================================================================================================
History:
    Date       Author           Description Of Change
    ---------- ---------------- -----------------------------------------------------------------
    03/06/2018 Watch Lee        1. version 1.0.

**************************************************************************************************/

/* Includes */ 
#include "App.h"
#include "../../EventLog/Include/EventLog.h"

/* Data declarations */
/*
The event log task will sequentially save data that data pointers point to. 
Each data MUST correctly fill in the data size of the data pointer. 
*/
EventLogType eventLogTable[Log_Table_Size] = 
{
//  data pointer                     data size
    {&bmsData.battOverallVolt,       2},
    {&bmsData.battCellTemp,          2},
    {&bmsData.battChargeCurr,        2},
    {&bmsData.battDischargeCurr,     2},
    {&comData.speedFan1,             2},
    //10
    {&comData.speedFan2,             2},
    {&comData.speedFan3,             2},
    {&comData.speedFan4,             2},
    {&comData.fanPWM,                2},
    //Event log data0 26
    {&comData.fanInletTemp,          2},
    //20
    {&comData.chargeTemp,            2},
    {&comData.dischargeFetTemp,      2},
    {&avgBattVolt.val,               2}, // pending as discharger input voltage
    {&avgOutputVolt.val,             2},
    {&bmsData.battDischargeCurr,     2}, // left for discharger input current
    //30
    {&avgOutputCurr.val,             2},
    {&avgChargePower.val,            2}, // left for discharger input power
    {&avgOutputPower.val,            2},
    {&bmsData.stateOfCharge,         2},
    {&bmsData.stateOfHealth,         2},
    //40
    {&bbuState.data[0],              1},
    {&bbuState.data[1],              1},
    {&dcdcState.bytes.high,          2},
    //Event log data1 26
    {&bbuState.data[4],              1},
    {&bbuState.data[5],              1},
    {&bbuState.data[6],              1},
    {&bbuState.data[7],              1},
    {&bbuAlarm.data[0],              1},
    {&bbuAlarm.data[1],              1},
    //50
    {&dcdcAlarm.all,                 4},
    {&bbuAlarm.data[6],              1},
    {&bbuAlarm.data[7],              1},
    {&bbuAlarm.data[8],              1},
    {&bbuAlarm.data[9],              1},
    {&bbuFault.all,                  2},
    //60
    {&countdownDischargeTimes,       4},
    {&pmiTimes,                      4}, // UPtimes
    {&bmsData.remainingBattCapcity,  2},
    //Event log data2 26
    {&avgCurrShareVolt.val,          2},
    {&bmsData.packFanTach,           2},
    //Event log data3 4
    //74
};

/*
The init event log data function will update the last data which are the corresponding items of the eventLogTable 
with target eventlog items to the specified variables that accumulated data pointers point to.
Each data MUST correctly fill in the data size of the data pointer. 
*/
AccumulateDataType accumulateDataTable[Accumulate_Data_Table_Size] = 
{
// target eventlog item         accumulated data pointer                data size
   0x24,                        &pmiTimes,                              4,
};
//---------------- END LINE -----------------------------------------------------------------------
