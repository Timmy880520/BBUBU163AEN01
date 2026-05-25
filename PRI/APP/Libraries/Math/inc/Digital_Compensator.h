/**************************************************************************************************
File Name: Digital_Compensator.h
Global Data:
    Name                     Type               Description
    ------------------------ ------------------ --------------------------------------------------
    None
Description:
    Header file of Digital_Compensator.c.

===================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    03/08/2022 Watch Lee        1. version 1.0.

**************************************************************************************************/

#ifndef __DIGITAL_COMPENSATOR_H__
#define __DIGITAL_COMPENSATOR_H__

/* Macro definitions */
// Group of digital filter declarations, Q shall be NOT more than 16
#define Create_Type_III_Compensator(Name, Q)     CompensatorObj Name = \
                                                 {\
                                                     {\
                                                         .hsFilter.b0 = Default_Type_III_HS_B0,\
                                                         .hsFilter.b1 = Default_Type_III_HS_B1,\
                                                         .hsFilter.b2 = Default_Type_III_HS_B2,\
                                                         .hsFilter.a1 = Default_Type_III_HS_A1,\
                                                         .hsFilter.a2 = Default_Type_III_HS_A2,\
                                                         .hsFilter.un = 0.0f,\
                                                         .hsFilter.un1 = 0.0f,\
                                                         .hsFilter.un2 = 0.0f,\
                                                         .hsFilter.yn = 0.0f,\
                                                         .hsFilter.yn1 = 0.0f,\
                                                         .hsFilter.yn2 = 0.0f,\
                                                         .piControl.kp = Default_Type_III_PI_KP,\
                                                         .piControl.ki = Default_Type_III_PI_KI,\
                                                         .piControl.kc = Default_Type_III_PI_KC,\
                                                         .piControl.ka = Default_Type_III_PI_KA,\
                                                         .piControl.max = Default_Type_III_PI_Limit,\
                                                         .piControl.min = 0.0f,\
                                                         .piControl.error = 0.0f,\
                                                         .piControl.pYn = 0.0f,\
                                                         .piControl.iYn = 0.0f,\
                                                         .piControl.iYn1 = 0.0f,\
                                                         .piControl.piYn = 0.0f,\
                                                         .piControl.satError = 0.0f,\
                                                         .piControl.output = 0.0f,\
                                                         .reference = 0l,\
                                                         .feedback = 0l,\
                                                         .error = 0l,\
                                                         .output = 0l,\
                                                         .maximum = (1l << Q),\
                                                         .minimum = 0l,\
                                                         .qNotation = Q\
                                                     },\
                                                     {\
                                                         .calculate = Float_Type_III_Compensation,\
                                                         .reset = Float_Type_III_Compensation_Reset\
                                                     },\
                                                 }

// Default parameters
// High stop filter of type III Compensator
#define Default_Type_III_HS_B0     0.31f//0.347f
#define Default_Type_III_HS_B1     0.098f//0.14f
#define Default_Type_III_HS_B2     -0.211f//-0.208f
#define Default_Type_III_HS_A1     -0.897f//-0.803f
#define Default_Type_III_HS_A2     0.093f//0.082f
// PI controller of type III Compensator
#define Default_Type_III_PI_KP     0.841f//0.841f
#define Default_Type_III_PI_KI     0.159f//0.159f
#define Default_Type_III_PI_KC     0.188f//0.188f
#define Default_Type_III_PI_KA     4.971f//2.883f//2.121f//4.106f
#define Default_Type_III_PI_Limit  0.201f//0.346f//0.472f//0.244f

/* Type definitions */
// Structure & union
typedef struct
{
    float b0;
    float b1;
    float b2;
    float a1;
    float a2;
    float un;
    float un1;
    float un2;
    float yn;
    float yn1;
    float yn2;
}FloatHighStopFilterType;

typedef struct
{
    float kp;
    float ki;
    float kc;
    float ka;
    float max;
    float min;
    float error;
    float pYn;
    float iYn;
    float iYn1;
    float piYn;
    float satError;
    float output;
}FloatPiControlType;

typedef struct CompensatorType CompensatorObj;

struct CompensatorType
{
    struct
    {
        FloatHighStopFilterType hsFilter;
        FloatPiControlType piControl;
        long reference;
        long feedback;
        long error;
        long output;
        long maximum;
        long minimum;
        long qNotation;
    }member;

    struct
    {
        long (*calculate)(CompensatorObj*, long, long);
        void (*reset)(CompensatorObj*);
    }func;
};

/* Global function prototypes */
long Float_Type_III_Compensation(CompensatorObj *obj, long reference, long feedback);
void Float_Type_III_Compensation_Reset(CompensatorObj *obj);

/* Global data declarations */
extern float Float_Qn_Divisor[16];

#endif
