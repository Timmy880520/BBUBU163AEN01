/**************************************************************************************************
File Name: EXT_Calibration.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    Calculate_CRC16                         CalcuCRC16.c
    Flash_Sector_Erase                      f28004x_flash_access.c
    Flash_Word_Program                      f28004x_flash_access.c
Description:
        Calibration protocol program.

===================================================================================================
History:
    Date        Author          Description of Change
    ----------- --------------- -----------------------------------------------------------------
    03/04/2021  Watch Lee       Create file.
    03/08/2021  Fred Huang      Include COM_UART for PFC calibration control
    03/09/2021  Fred Huang      1. Command CRC check from Save_Ext_Calibration_Data
                                2. Write crcValue back in Save_Ext_Calibration_Data
                                3. Add calibrationDataDefault for no calibration value
    03/10/2021  Fred Huang      Add 0x1C get CRC command
    03/11/2021  Fred Huang      Add reload calibration data to buffer

**************************************************************************************************/

/* Includes */
#include "App.h"
#include "EXT_Calibration.h"
#include "f28003x_flash_access.h"
#include "COM_UART.h"
#include "Initial.h"

/* Local function prototypes */
void Save_Ext_Calibration_Data(void);
void Reset_Ext_Calibration_Data(void);

/* Data definitions */
CalibrationSetType calibrationSet = {0};
CalibrationDataType calibrationData =
{
    0,
    {
        {Ichg_Cmd_Default},
        {Vdischg_Cmd_Default},
        {Vchg_Cmd_Default},

        {Calibration_Default}, //Dchg Iout
        {Calibration_Default}, //Dchg Vout
        {Calibration_Default}, //Dchg Vout oring
        {Calibration_Default}, //Dchg Current sharing
        {Calibration_Default}, //Dchg Vin

        {Calibration_Default}, //Chg Iout
        {Calibration_Default}, //Chg Vout

    }
};
CalibrationArgType calibrationDataDefault = {Calibration_Default};

/**************************************************************************************************
Function Name:
    void Load_Ext_Calibration_Data(void)
Input:
    None.
Output:
    None.
Comment:
    Load ROM data to calibrationData.
**************************************************************************************************/
void Load_Ext_Calibration_Data(void)
{
    CalibrationDataType *romCalibrationData = (CalibrationDataType*)Calibration_ROM_Start;
    unsigned long crcValue = 0;
    unsigned char buffer = 0;

    // Calculating checksum
    for (int i = 0; i < sizeof(calibrationData.arg); i++)
    {
        buffer = __byte((int*)&romCalibrationData->arg + i, 0) & 0xFF;
        crcValue = Calculate_CRC16(crcValue, buffer);
        buffer = __byte((int*)&romCalibrationData->arg + i, 1) & 0xFF;
        crcValue = Calculate_CRC16(crcValue, buffer);
    }

    if (crcValue == romCalibrationData->crc)
    {
        for (int i = 0; i < sizeof(calibrationData); i++)
            *((unsigned char*)&calibrationData + i) = *((unsigned char*)romCalibrationData + i);
    }
    else
        Reset_Ext_Calibration_Data();
}
/**************************************************************************************************
Function Name:
    void Reload_Ext_Calibration_Data(void)
Input:
    None.
Output:
    None.
Comment:
    Reload calibration data to buffer.
**************************************************************************************************/
void Reload_Ext_Calibration_Data(void)
{
    // Calibration buffer transfer
    long temp;

    llcCalibration.offsetIout      = calibrationData.arg.dchgCal1[0].arg.offset;
    llcCalibration.gainIout        = calibrationData.arg.dchgCal1[0].arg.gain;
    llcCalibration.offsetVout      = calibrationData.arg.dchgCal2[0].arg.offset;
    llcCalibration.gainVout        = calibrationData.arg.dchgCal2[0].arg.gain;
    llcCalibration.offsetVoutOring = calibrationData.arg.dchgCal3[0].arg.offset;
    llcCalibration.gainVoutOring   = calibrationData.arg.dchgCal3[0].arg.gain;
    llcCalibration.offsetVcmd      = calibrationData.arg.dchgVcmd.arg.offset;
    llcCalibration.gainVcmd        = calibrationData.arg.dchgVcmd.arg.gain;
    llcCalibration.offsetIoutShare = calibrationData.arg.dchgCal4[0].arg.offset;
    llcCalibration.gainIoutShare   = calibrationData.arg.dchgCal4[0].arg.gain;
    llcCalibration.offsetVin       = calibrationData.arg.dchgCal5[0].arg.offset;
    llcCalibration.gainVin         = calibrationData.arg.dchgCal5[0].arg.gain;

    ahbCalibration.offsetIout      = calibrationData.arg.chgCal1[0].arg.offset;
    ahbCalibration.gainIout        = calibrationData.arg.chgCal1[0].arg.gain;
    ahbCalibration.offsetVout      = calibrationData.arg.chgCal2[0].arg.offset;
    ahbCalibration.gainVout        = calibrationData.arg.chgCal2[0].arg.gain;
    ahbCalibration.offsetIcmd      = calibrationData.arg.chgIcmd.arg.offset;
    ahbCalibration.gainIcmd        = calibrationData.arg.chgIcmd.arg.gain;
    ahbCalibration.offsetVcmd      = calibrationData.arg.chgVcmd.arg.offset;
    ahbCalibration.gainVcmd        = calibrationData.arg.chgVcmd.arg.gain;

    //LLC voltage reference
    temp = ((float)llcCalibration.offsetVcmd * 0.0001) * OUTPUT_VOLT_SCALE; //8466 * 0.0001 * 4725 // 5377 * 0.0001 * 7439
    outputVoltSetpointOffset = Data_Saturation(temp - DEFAULT_OUTPUT_VOLT, 500, -500);
    if (sohOutFlag == 0)
        Set_Voltage_Setpoint(DEFAULT_OUTPUT_VOLT);// + outputVoltSetpointOffset); //404V
    else
        Set_Voltage_Setpoint(DEFAULT_OUTPUT_VOLT);// + outputVoltSetpointOffset - 100); //394V

    //AHB CV Mode voltage reference
    temp = ((float)ahbCalibration.offsetVcmd * 0.0001) * CHARGE_VOLT_SCALE;
    chargeVoltSetpointOffset = Data_Saturation(temp - DEFAULT_CHARGE_VOLT, 500, -500);
    ahbVoltSetpoint = ((OUTPUT_VOLT_BATT(DEFAULT_CHARGE_VOLT)) * __IQ(0.1, 14)) >> 14;// + chargeVoltSetpointOffset

    //AHB CC Mode current reference
    if (sohOutChgFlag)
    {
        temp = ((float)ahbCalibration.offsetIcmd * 0.000457) * CHARGE_CURR_SCALE;
        chargeCurrSetpointOffset = Data_Saturation(temp - DEFAULT_CHARGE_CURR2, 500, -500);
        Set_Current_Setpoint(DEFAULT_CHARGE_CURR2 + chargeCurrSetpointOffset);
    }
    else
    {
        temp = ((float)ahbCalibration.offsetIcmd * 0.000457) * CHARGE_CURR_SCALE;
        chargeCurrSetpointOffset = Data_Saturation(temp - DEFAULT_CHARGE_CURR1, 500, -500);
        Set_Current_Setpoint(DEFAULT_CHARGE_CURR1 + chargeCurrSetpointOffset);
    }
}
/**************************************************************************************************
Function Name:
    void Process_Ext_Calibration_Data(void)
Input:
    None.
Output:
    None.
Comment:
    State machine for processing calibration data.
**************************************************************************************************/
void Process_Ext_Calibration_Data(void)
{
    static EnumCalibrationStep step = NORMAL_MODE;

    switch (step)
    {
        case NORMAL_MODE:
            step = (calibrationSet.arg.mode == 1) ? ENTRY_CALIBRATION : NORMAL_MODE;
            break;
        case ENTRY_CALIBRATION:
            Reset_Ext_Calibration_Data();
            step = CALIBRATION_MODE;
            break;
        case CALIBRATION_MODE:
            if (calibrationSet.arg.mode == 1)
            {
                if (calibrationSet.arg.save == 1)
                {
                    Save_Ext_Calibration_Data();
                    //pfcCalibrationCtrl = 0x5555;
                    calibrationSet.all = 0;
                    step = NORMAL_MODE;
                }
            }
            else
            {
                Load_Ext_Calibration_Data();
                //pfcCalibrationCtrl = 0xAAAA;
                calibrationSet.all = 0;
                step = NORMAL_MODE;
            }
            break;
    }
}
/**************************************************************************************************
Function Name:
    void Save_Ext_Calibration_Data(void)
Input:
    None.
Output:
    None.
Comment:
    Save Calibration data to ROM.
**************************************************************************************************/
void Save_Ext_Calibration_Data(void)
{
    unsigned long crcValue = 0;
    unsigned char buffer = 0;

    // Calculating checksum
    for (int i = 0; i < sizeof(calibrationData.arg); i++)
    {
        buffer = __byte((int*)&calibrationData.arg + i, 0) & 0xFF;
        crcValue = Calculate_CRC16(crcValue, buffer);
        buffer = __byte((int*)&calibrationData.arg + i, 1) & 0xFF;
        crcValue = Calculate_CRC16(crcValue, buffer);
    }

    calibrationData.crc = crcValue;

    Flash_Sector_Erase(Calibration_ROM_Start, Calibration_ROM_Size);

    unsigned long dataSize = sizeof(calibrationData);

    dataSize = ((dataSize & 3) > 0) ? ((dataSize & 0xFFFFFFFC) + 4) : dataSize;

    for (int i = 0; i < dataSize; i++)
        Flash_Word_Program(Calibration_ROM_Start, dataSize, (unsigned int*)&calibrationData);
}
/**************************************************************************************************
Function Name:
    void Reset_Ext_Calibration_Data(void)
Input:
    None.
Output:
    None.
Comment:
    Reset calibration data.
**************************************************************************************************/
void Reset_Ext_Calibration_Data(void)
{
    calibrationData.crc = 0;
    calibrationData.arg.dchgVcmd.all = Vdischg_Cmd_Default;
    calibrationData.arg.chgVcmd.all = Vchg_Cmd_Default;
    calibrationData.arg.chgIcmd.all = Ichg_Cmd_Default;

    for (int i = 3; i < (sizeof(calibrationData.arg) >> 1); i++)
        *((unsigned long*)&calibrationData.arg + i) = Calibration_Default;
}
//---------------- END LINE -----------------------------------------------------------------------
