/**************************************************************************************************
File Name: INT_Protocol_Table.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    bootPri                                 COM_UART.c
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None
Description:  
    Command list and packet data of internal protocol, they should be modified by different 
projects.

===================================================================================================
History:
    Date        Author          Description of Change
    ----------- --------------- -----------------------------------------------------------------
    12/26/2019  Watch Lee       1. version 1.0.
    03/07/2021  Fred Huang      1. Add CalibrationDataPacket
                                2. Add 0x04 and 0x05 command
    03/10/2021  Fred Huang      Modify CalibrationDataPacket
    03/18/2021  Fred Huang      Modify 0x20, 0x21 data pointer name
    05/06/2021  Watch Lee       1. Add Boot commands.

**************************************************************************************************/

/* Includes */ 
#include "INT_Protocol_Table.h"
#include "App.h"

/* Data definitions */
// Command packet data

DataPackStr dcdcDataPacket[] =
{
    {&avgBattVolt.val1,                     2},
    {&avgOutputVolt.val1,                   2},
    {&avgCurrShareVolt.val1,                2},
//    {&powerOnStateCheck,                    2},
    {&avgOutputCurr.val1,                   2},
    {&avgInnerVolt.val1,                    2},
    {&avgOutputPower.val1,                  2},
    {&countdownDischargeTimes,              4},
    {&avgChargeVolt.val1,                   2},
    {&avgChargeCurr.val1,                   2},
    {&avgChargePower.val1,                  2},
};

DataPackStr logData0Packet[] =
{
    {&eventList.member.logPage[0].index.state[0],      1},
    {&eventList.member.logPage[0].index.state[1],      1},
    {&eventList.member.logPage[0].index.crc8,          1},
    {&eventList.member.logPage[0].index.cycleCount,    1},
    {&eventList.member.logPage[0].index.writeCycle[0], 1},
    {&eventList.member.logPage[0].index.writeCycle[1], 1},
    {&eventList.member.logPage[0].index.block,         1},
    {&eventList.member.logPage[0].index.line,          1},
    {&eventList.member.logPage[0].data[0],  1},
    {&eventList.member.logPage[0].data[1],  1},
    {&eventList.member.logPage[0].data[2],  1},
    {&eventList.member.logPage[0].data[3],  1},
    {&eventList.member.logPage[0].data[4],  1},
    {&eventList.member.logPage[0].data[5],  1},
    {&eventList.member.logPage[0].data[6],  1},
    {&eventList.member.logPage[0].data[7],  1},
    {&eventList.member.logPage[0].data[8],  1},
    {&eventList.member.logPage[0].data[9],  1},
    {&eventList.member.logPage[0].data[10], 1},
    {&eventList.member.logPage[0].data[11], 1},
    {&eventList.member.logPage[0].data[12], 1},
    {&eventList.member.logPage[0].data[13], 1},
    {&eventList.member.logPage[0].data[14], 1},
    {&eventList.member.logPage[0].data[15], 1},
    {&eventList.member.logPage[0].data[16], 1},
    {&eventList.member.logPage[0].data[17], 1},
};

DataPackStr logData1Packet[] =
{
    {&eventList.member.logPage[0].data[18], 1},
    {&eventList.member.logPage[0].data[19], 1},
    {&eventList.member.logPage[0].data[20], 1},
    {&eventList.member.logPage[0].data[21], 1},
    {&eventList.member.logPage[0].data[22], 1},
    {&eventList.member.logPage[0].data[23], 1},
    {&eventList.member.logPage[0].data[24], 1},
    {&eventList.member.logPage[0].data[25], 1},
    {&eventList.member.logPage[0].data[26], 1},
    {&eventList.member.logPage[0].data[27], 1},
    {&eventList.member.logPage[0].data[28], 1},
    {&eventList.member.logPage[0].data[29], 1},
    {&eventList.member.logPage[0].data[30], 1},
    {&eventList.member.logPage[0].data[31], 1},
    {&eventList.member.logPage[0].data[32], 1},
    {&eventList.member.logPage[0].data[33], 1},
    {&eventList.member.logPage[0].data[34], 1},
    {&eventList.member.logPage[0].data[35], 1},
    {&eventList.member.logPage[0].data[36], 1},
    {&eventList.member.logPage[0].data[37], 1},
    {&eventList.member.logPage[0].data[38], 1},
    {&eventList.member.logPage[0].data[39], 1},
    {&eventList.member.logPage[0].data[40], 1},
    {&eventList.member.logPage[0].data[41], 1},
    {&eventList.member.logPage[0].data[42], 1},
    {&eventList.member.logPage[0].data[43], 1},

};

DataPackStr logData2Packet[] =
{
    {&eventList.member.logPage[0].data[44], 1},
    {&eventList.member.logPage[0].data[45], 1},
    {&eventList.member.logPage[0].data[46], 1},
    {&eventList.member.logPage[0].data[47], 1},
    {&eventList.member.logPage[0].data[48], 1},
    {&eventList.member.logPage[0].data[49], 1},
    {&eventList.member.logPage[0].data[50], 1},
    {&eventList.member.logPage[0].data[51], 1},
    {&eventList.member.logPage[0].data[52], 1},
    {&eventList.member.logPage[0].data[53], 1},
    {&eventList.member.logPage[0].data[54], 1},
    {&eventList.member.logPage[0].data[55], 1},
    {&eventList.member.logPage[0].data[56], 1},
    {&eventList.member.logPage[0].data[57], 1},
    {&eventList.member.logPage[0].data[58], 1},
    {&eventList.member.logPage[0].data[59], 1},
    {&eventList.member.logPage[0].data[60], 1},
    {&eventList.member.logPage[0].data[61], 1},
    {&eventList.member.logPage[0].data[62], 1},
    {&eventList.member.logPage[0].data[63], 1},
    {&eventList.member.logPage[0].data[64], 1},
    {&eventList.member.logPage[0].data[65], 1},
    {&eventList.member.logPage[0].data[66], 1},
    {&eventList.member.logPage[0].data[67], 1},
    {&eventList.member.logPage[0].data[68], 1},
    {&eventList.member.logPage[0].data[69], 1},
};

DataPackStr logData3Packet[] =
{
    {&eventList.member.logPage[0].data[70], 1},
    {&eventList.member.logPage[0].data[71], 1},
    {&eventList.member.logPage[0].data[72], 1},
    {&eventList.member.logPage[0].data[73], 1},
};

DataPackStr calibrationModePacket[] =
{
    {&uartCalibrationMode,           2},
};


DataPackStr saveCalibrationPacket[] =
{
    {&uartSaveCalibration,           1},
};

DataPackStr engineerModePacket[] =
{
    {&uartEngineerMode,              2},
};

DataPackStr calidataDchgPacket[] =
{
    {&calibrationData.arg.dchgCal1[0].arg.offset,           2},
    {&calibrationData.arg.dchgCal1[0].arg.gain,             2},
    {&calibrationData.arg.dchgCal2[0].arg.offset,           2},
    {&calibrationData.arg.dchgCal2[0].arg.gain,             2},
    {&calibrationData.arg.dchgCal3[0].arg.offset,           2},
    {&calibrationData.arg.dchgCal3[0].arg.gain,             2},
    {&calibrationData.arg.dchgVcmd.arg.offset,              2},
    {&calibrationData.arg.dchgVcmd.arg.gain,                2},
    {&calibrationData.arg.dchgCal4[0].arg.offset,           2},
    {&calibrationData.arg.dchgCal4[0].arg.gain,             2},
    {&calibrationData.arg.dchgCal5[0].arg.offset,           2},
    {&calibrationData.arg.dchgCal5[0].arg.gain,             2},
};

DataPackStr calidataChgPacket[] =
{
    {&calibrationData.arg.chgCal1[0].arg.offset,            2},
    {&calibrationData.arg.chgCal1[0].arg.gain,              2},
    {&calibrationData.arg.chgCal2[0].arg.offset,            2},
    {&calibrationData.arg.chgCal2[0].arg.gain,              2},
    {&calibrationData.arg.chgIcmd.arg.offset,               2},
    {&calibrationData.arg.chgIcmd.arg.gain,                 2},
    {&calibrationData.arg.chgVcmd.arg.offset,               2},
    {&calibrationData.arg.chgVcmd.arg.gain,                 2},
};

DataPackStr dcdcStatusPacket[] =
{
    {&dcdcState.all,                 4},
    {&dcdcAlarm.all,                 4},
};

DataPackStr comDataPacket[] =
{
    {&comData.comState.all,          2},
    {&comData.fanInletTemp,          2},
    {&comData.chargeTemp,            2},
    {&comData.dischargeFetTemp,      2},
    {&comData.speedFan1,             2},
    {&comData.speedFan2,             2},
    {&comData.speedFan3,             2},
    {&comData.speedFan4,             2},
    {&comData.fanPWM,                2},
    {&comData.srTemp,                2},
    {&comData.oringTemp,             2},
};

DataPackStr bmsDataPacket[] =
{
    {&bmsData.battOverallVolt,       2},
    {&bmsData.battCellTemp,          2},
    {&bmsData.battChargeCurr,        2},
    {&bmsData.battDischargeCurr,     2},
    {&bmsData.stateOfCharge,         2},
    {&bmsData.stateOfHealth,         2},
    {&bmsData.remainingBattCapcity,  2},
//    {&bmsData.packFanTach,           2},
};

DataPackStr bbuStatusPacket[] =
{
    {&bbuState.data[0],              1},
    {&bbuState.data[1],              1},
    {&bbuState.data[2],              1},
    {&bbuState.data[3],              1},
    {&bbuState.data[4],              1},
    {&bbuState.data[5],              1},
    {&bbuState.data[6],              1},
    {&bbuState.data[7],              1},
    {&bbuAlarm.data[0],              1},
    {&bbuAlarm.data[1],              1},
    {&bbuAlarm.data[2],              1},
    {&bbuAlarm.data[3],              1},
    {&bbuAlarm.data[4],              1},
    {&bbuAlarm.data[5],              1},
    {&bbuAlarm.data[6],              1},
    {&bbuAlarm.data[7],              1},
    {&bbuAlarm.data[8],              1},
    {&bbuAlarm.data[9],              1},
    {&bbuFault.all,                  2},
};

DataPackStr pmiTimesSyncPacket[] =
{
    {&pmiTimes,                      4},
};
DataPackStr fetControlPacket[] =
{
    {&bmsFetControl.all,             2},
};
DataPackStr overWrietTimerPacket[] =
{
    {&overWrietTimer,                4},
};
DataPackStr LearningModePacket[] =
{
    {&learningCycle.all,    2},
};
// Command list
Create_INT_Command(intPage0Command, 32, 256) =
{
//  flag,        code,              data type,     data pointer,                            data size(Byte)
    {GETTABLE,   0xC0,              DATA_PACKET,   dcdcDataPacket,                          sizeof(dcdcDataPacket)},
    {GETTABLE,   0xC1,              DATA_PACKET,   dcdcStatusPacket,                        sizeof(dcdcStatusPacket)},
    {SETTABLE,   0xC2,              DATA_PACKET,   comDataPacket,                           sizeof(comDataPacket)},
    {SETTABLE,   0xC3,              DATA_PACKET,   bmsDataPacket,                           sizeof(bmsDataPacket)},
    {SETTABLE,   0xC4,              DATA_PACKET,   bbuStatusPacket,                         sizeof(bbuStatusPacket)},
    {SETTABLE,   0xC5,              NOT_PACKET,    pmiTimesSyncPacket,                      sizeof(pmiTimesSyncPacket)},
    {NACK,       0xC6,              DATA_PACKET,   fetControlPacket,                        sizeof(fetControlPacket)},
    {NACK,       0xC7,              NOT_PACKET,    &overWrietTimerPacket,                   sizeof(overWrietTimerPacket)},
    {SETTABLE,   0xC8,              NOT_PACKET,    &protectRelease,                         1},
    {SETTABLE,   0xC9,              NOT_PACKET,    &forceDischarge,                         1},
    {GETTABLE,   0xCA,              NOT_PACKET,    (char*)bootVersion,                      8},
    {GETTABLE,   0xCB,              NOT_PACKET,    (char*)TEXT_HEADER_ROM.programVersion,   8},
    {SETTABLE,   0xCC,              NOT_PACKET,    &resetButton,                            1},

    {NACK,       0xE0,              NOT_PACKET,    &eventReadPointer,                       sizeof(eventReadPointer)},
    {GETTABLE,   0xE1,              DATA_PACKET,   logData0Packet,                          sizeof(logData0Packet)},
    {GETTABLE,   0xE2,              DATA_PACKET,   logData1Packet,                          sizeof(logData1Packet)},
    {GETTABLE,   0xE3,              DATA_PACKET,   logData2Packet,                          sizeof(logData2Packet)},
    {GETTABLE,   0xE4,              DATA_PACKET,   logData3Packet,                          sizeof(logData3Packet)},
    {SETTABLE,   0xE5,              NOT_PACKET,    &eraseTrig,                              1},
    {SETTABLE,   0xE6,              NOT_PACKET,    &eventTrig,                              1},
    {SETTABLE,   0xEA,              NOT_PACKET,    &learningMode,                           1},
    {GETTABLE,   0xEB,              DATA_PACKET,   LearningModePacket,                      sizeof(LearningModePacket)},

    {NACK,       0xD0,              DATA_PACKET,   calibrationModePacket,                   sizeof(calibrationModePacket)},
    {SETTABLE,   0xD1,              DATA_PACKET,   saveCalibrationPacket,                   sizeof(saveCalibrationPacket)},
    {SETTABLE,   0xD2,              DATA_PACKET,   calidataDchgPacket,                      sizeof(calidataDchgPacket)},
    {SETTABLE,   0xD3,              DATA_PACKET,   calidataChgPacket,                       sizeof(calidataChgPacket)},
    {NACK,       0xD4,              DATA_PACKET,   engineerModePacket,                      sizeof(engineerModePacket)},

    {SETTABLE,   BOOT_PROTECTION,   NOT_PACKET,    &bootSec.member.message.protectKey,      2},
    {SETTABLE,   BOOT_INIT,         NOT_PACKET,    &bootSec.member.message.initKey,         2},
    {NACK,       BOOT_DOWNLOAD,     POINTER_DATA,  &bootSec.member.message.download,        32},
    {NACK,       BOOT_COMPLETED,    NOT_PACKET,    &bootSec.member.message.completeKey,     2},
    {GETTABLE,   BOOT_STATE,        NOT_PACKET,    &bootSec.member.message.state,           2}
};

