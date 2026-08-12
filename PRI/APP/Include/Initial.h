/***********************************************************************************************************************
File Name: Initial.h
Global Data:
    Name                Type                Description
    ------------------- ------------------- --------------------------------------------------------
    None
Description:
    Header file of Initial.c
========================================================================================================================
History:
    Date        Author          Description of Change
    ----------- --------------- ----------------------------------------------------------------------------------------
    10/24/2019  Fred Huang      Create file

***********************************************************************************************************************/

#ifndef _INITIAL_H_
#define _INITIAL_H_

/* Macro definitions */
// Statement Replacement
#define OUTPUT_VOLT_SCALE               (9910.0f)   //New add 400V BBU 4725 //400V BBU 7439
#define DEFAULT_OUTPUT_VOLT_RATIO       (8779.0f)   //New add 400/472.478 = 0.8466 //400/743.985 = 0.537645
#define DEFAULT_OUTPUT_VOLT             (long)(OUTPUT_VOLT_SCALE * DEFAULT_OUTPUT_VOLT_RATIO / 1e4f)

#define CURR_ADC1_OFFSET                (unsigned short)2047//400V BBU
#define CURR_ADC2_OFFSET                (unsigned short)409//400V BBU

#define CHARGE_VOLT_SCALE               (3739.0f)   //300.15V BBU 3739
#define DEFAULT_CHARGE_VOLT_RATIO       (7890.0f)   //295.00/373.924 = 0.7889
#define DEFAULT_CHARGE_VOLT             (long)(CHARGE_VOLT_SCALE * DEFAULT_CHARGE_VOLT_RATIO / 1e4f)

#define CHARGE_CURR_SCALE               (2188.0f)  //21.88A
#define DEFAULT_CHARGE_CURR_RATIO1      (137.0f)   //0.6/21.88=0.0274
#define DEFAULT_CHARGE_CURR1            (long)(CHARGE_CURR_SCALE * DEFAULT_CHARGE_CURR_RATIO1 / 1e4f)

#define DEFAULT_CHARGE_CURR_RATIO2      (759.0f)   //1.66/21.88=0.0759
#define DEFAULT_CHARGE_CURR2            (long)(CHARGE_CURR_SCALE * DEFAULT_CHARGE_CURR_RATIO2 / 1e4f)

/* Type definitions */
// Structure & union

typedef struct
{
    short offsetIout;
    short gainIout;

    short offsetVout;
    short gainVout;

    short offsetVoutOring;
    short gainVoutOring;

    short offsetVcmd;
    short gainVcmd;

    short offsetIoutShare;
    short gainIoutShare;

    short offsetVin;
    short gainVin;

}DischargerCalibrationDataType;

typedef struct
{
    short offsetIout;
    short gainIout;

    short offsetVout;
    short gainVout;

    short offsetIcmd;
    short gainIcmd;

    short offsetVcmd;
    short gainVcmd;

}ChargerCalibrationDataType;

/* Global function prototypes */
void Initial(void);
void Disable_WatchDog(void);
void Enable_WatchDog(void);
void Kick_WatchDog(void);

/* Global data declarations */
extern DischargerCalibrationDataType llcCalibration;
extern ChargerCalibrationDataType ahbCalibration;
extern unsigned short outputVoltSetpoint;
extern short outputVoltSetpointOffset;
extern short chargeVoltSetpointOffset;
extern short chargeCurrSetpointOffset;
extern unsigned short waringCodeClearKey;

#endif /* _INITIAL_H_ */
