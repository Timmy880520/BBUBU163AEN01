/***********************************************************************************************************************
File Name: EXT_Calibration.h
Global Data:
    Name                 Type                Description
    -------------------- ------------------- --------------------------------------------------------
    calibrationSet       CalibrationSetType  Calibration mode control parameters.
    calibrationData      CalibrationDataType Calibration data group.
    extCalibration       ExtCommandArrayType Calibration command list.
    start_of_calibration CCS cmd export      Constant of calibration rom start address.
    end_of_calibration   CCS cmd export      Constant of calibration rom end address.
Description:
    Header file of EXT_Calibration.c
========================================================================================================================
History:
    Date        Author          Description of Change
    ----------- --------------- ----------------------------------------------------------------------------------------
    03/05/2021  Watch Lee       Create file.
    03/11/2021  Fred Huang      Add reload calibration data to buffer

***********************************************************************************************************************/

#ifndef _EXT_CALIBRATION_H_
#define _EXT_CALIBRATION_H_

/* Includes */
#include "INT_Protocol.h"

/* Macro definitions */
// Statement Replacement
// Calibration ROM address
#define Calibration_ROM_Start       (unsigned long)(&start_of_calibration)
#define Calibration_ROM_End         (unsigned long)(&end_of_calibration)
#define Calibration_ROM_Size        ((unsigned long)&end_of_calibration - (unsigned long)&start_of_calibration)

#define Ichg_Cmd_Default            0x0000003C  // Gain = 0, Offset = 60
#define Vdischg_Cmd_Default         0x00002112  // Gain = 0, Offset = 8466
#define Vchg_Cmd_Default            0x00001FC2  // Gain = 0, Offset = 8130
#define Calibration_Default         0x03E80000  // Gain = 1000, Offset = 0

/* Type definitions */
// Enumeration
typedef enum
{
    NORMAL_MODE         = 0,
    ENTRY_CALIBRATION   = 1,
    CALIBRATION_MODE    = 2
}EnumCalibrationStep;

// Struct & Union
typedef union
{
    unsigned long all;

    struct
    {
        unsigned short mode;
        unsigned short save;
    }arg;
}CalibrationSetType;


typedef union
{
    unsigned long all;

    struct
    {
        short offset;
        short gain;
    }arg;
}CalibrationArgType;

typedef struct
{
    unsigned long crc;

    struct
    {
        CalibrationArgType chgIcmd;
        CalibrationArgType dchgVcmd;
        CalibrationArgType chgVcmd;

        CalibrationArgType dchgCal1[1];
        CalibrationArgType dchgCal2[1];
        CalibrationArgType dchgCal3[1];
        CalibrationArgType dchgCal4[1];
        CalibrationArgType dchgCal5[1];

        CalibrationArgType chgCal1[1];
        CalibrationArgType chgCal2[1];


    }arg;

}CalibrationDataType;

/* Global function prototypes */
void Load_Ext_Calibration_Data(void);
void Process_Ext_Calibration_Data(void);
void Reload_Ext_Calibration_Data(void);

unsigned long Get_Package_CRC16(unsigned long crcValue, void *dataPtr, unsigned long size);

/* Global data declarations */
extern CalibrationSetType calibrationSet;
extern CalibrationDataType calibrationData;
extern CalibrationArgType calibrationDataDefault;

extern unsigned long start_of_calibration, end_of_calibration;

#endif
