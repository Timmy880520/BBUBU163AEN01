/**************************************************************************************************
File Name: Sine_Process.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ ------------------------------------------------------
    SIN_TABLE            long array         Q24 format sine table.
    DIVISION_TABLE       long array         Q21 division table.
    ATAN_TABLE           long array         Q16 arctangent table.
Description: 
    Header file of sine generator program, the file has associated with Sine_Process.c and 
Math_Table.c.

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- ------------------------------------------------------------------
    03/23/2012 Watch Lee        1. version 1.0.
    04/18/2018 Watch Lee        1. version 2.0.
                                2. modify to fit standard coding style.
    09/10/2018 Watch Lee        1. version 2.1.
                                2. add the clockOut member. 

**************************************************************************************************/

#ifndef __Sine_Process_H__
#define __Sine_Process_H__

/* Macro definitions */
#define Sine_Offset             0		    // Offset range should be 0~1pu
#define Sine_Gain               1024	    // Gain range should be 0~1(Q10)
#define Default_Period_Count    500         // 25kHz(polling frequency) / 50Hz(desired sine frequency) = 500

// Default parameters for the Sine Process 
#define Sine_Process_Defaults {\
                                Default_Period_Count / 2,\
                                Default_Period_Count / 4,\
                                Default_Period_Count,\
                                0,\
                                0,\
                                Default_Period_Count / 2,\
                                0,\
                                0,\
                                Sine_Process,\
                                Default_Update\
                              }

/* Type definitions */
// Struct and union
typedef struct
{
    unsigned long half;         // Input: Half period
    unsigned long quarter;      // Parameter: Quarter period
    unsigned long period;       // Parameter: Period
    unsigned long index;        // Parameter: Sine table index
    unsigned long count;        // Parameter: Period count
    unsigned long nextHalf;     // Input: Next half period
    long sineValue;             // Output: Sine value
    long clockOut;              // Output: Reference clock
    void (*proc)();             // Pointer to process function
    void (*update)();           // Pointer to period update function
}SineProcessObj;

typedef SineProcessObj *sineGroup;

/* Global function prototypes */
void Sine_Process(sineGroup);
void Default_Update(sineGroup, unsigned long);

/* Global data declarations */
extern const long SIN_TABLE[1025];
extern const long DIVISION_TABLE[1025];
extern const long SQRT_TABLE[1025];
extern const long ATAN_TABLE[512];

#endif
