/***********************************************************************************************************************
File Name: LookupTable.h
Global Data:
    Name                    Type                        Description
    ----------------------- --------------------------- ----------------------------------------------------------------
    LLC_TABLE               const unsigned short array  LLC controller output to frequency table
    FEEDFORWARD_SIN_TABLE   const short array           Ripple cancellation feedforward table based on half sine wave
    NTC_Table1              const unsigned short        TSM2A103F3951RZ
    NTC_Table2              const unsigned short        TSM1A103F34D1RZ
    NTC_Table3              const unsigned short        TTC3A103F34D1FT1
Description:
    Header file of LookupTable.c.

========================================================================================================================
History:
    Date        Author          Description of Change
    ----------- --------------- ----------------------------------------------------------------------------------------
    03/25/2020  Fred Huang      Create file

***********************************************************************************************************************/

#ifndef _LOOKUPTABLE_H_
#define _LOOKUPTABLE_H_

//------------------------------------------------------------------------------
// Condition definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Macro definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Global function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Global data declarations
//------------------------------------------------------------------------------
extern const unsigned short LLC_TABLE[4096];
extern const short FEEDFORWARD_SIN_TABLE[251];
extern const unsigned short NTC_Table1[166];
extern const unsigned short NTC_Table2[176];
extern const unsigned short NTC_Table3[166];
extern const unsigned short T_STEP_UP[256];
extern const short T_STEP_DOWN[256];
extern const unsigned short SR_DEADBAND_TABLE[515];
extern const long SQRT_TABLE[1025];
extern const long DIVISION_TABLE[1025];
//------------------------------------------------------------------------------
#endif /* _LOOKUPTABLE_H_ */
