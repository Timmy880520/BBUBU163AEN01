/**************************************************************************************************
File Name: INTP_Table_Template.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    bootPri                                 main.c
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None
Description:  
    Command list and packet data of internal protocol, they should be modified by different 
projects.

===================================================================================================
History:
     Date       Author           Description of Change
     ---------- ---------------- -----------------------------------------------------------------
     12/26/2019 Watch Lee        1. version 1.0.

**************************************************************************************************/

/* Includes */ 
#include "..\inc\INTP_Table_Template.h"
#include "Global_Var.h"

/* Data definitions */
//calibration
// Command packet data
DataPackStr calibrationModePacket[] =
{
    {&uartCalibrationMode,           2},
};
// Command list
Create_INT_Command(intpPriCommand, 16, 256) =
{
     //  flag,              code,    data type,                             data pointer,                 data size
    {GETTABLE,              0x06,   NOT_PACKET,                      &pfcCalibrationCtrl,                        2},
    {GETTABLE,   BOOT_PROTECTION,   NOT_PACKET,       &bootPri.member.message.protectKey,                        2},
    {GETTABLE,         BOOT_INIT,   NOT_PACKET,          &bootPri.member.message.initKey,                        2},
    {GETTABLE,     BOOT_DOWNLOAD, POINTER_DATA,         &bootPri.member.message.download,                       32},
    {GETTABLE,    BOOT_COMPLETED,   NOT_PACKET,      &bootPri.member.message.completeKey,                        2},
    {SETTABLE,        BOOT_STATE,   NOT_PACKET,            &bootPri.member.message.state,                        2}
};

Create_INT_Command(intpSecCommand, 16, 256) =
{
    {    NACK,              0xD0,  DATA_PACKET,                    calibrationModePacket,                        sizeof(calibrationModePacket)},
    {SETTABLE,   BOOT_PROTECTION,   NOT_PACKET,       &bootSec.member.message.protectKey,                        2},
    {SETTABLE,         BOOT_INIT,   NOT_PACKET,          &bootSec.member.message.initKey,                        2},
    {    NACK,     BOOT_DOWNLOAD, POINTER_DATA,         &bootSec.member.message.download,                       32},
    {    NACK,    BOOT_COMPLETED,   NOT_PACKET,      &bootSec.member.message.completeKey,                        2},
    {GETTABLE,        BOOT_STATE,   NOT_PACKET,            &bootSec.member.message.state,                        2}
};
//---------------- END LINE -----------------------------------------------------------------------
