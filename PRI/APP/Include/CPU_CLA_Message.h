
#ifndef __CPU_CLA_MESSAGE_H__
#define __CPU_CLA_MESSAGE_H__

/* Includes */
#include "App.h"

// Default parameters
// High stop filter of type III Compensator     1k/20000   1k/15000   850
#define Default_CLA_Type_III_HS_B0     0.177f   //0.507f   //0.420f   //0.177f
#define Default_CLA_Type_III_HS_B1     0.039f   //0.045f   //0.038f   //0.039f
#define Default_CLA_Type_III_HS_B2     -0.137f  //-0.461f  //-0.383f  //-0.137f
#define Default_CLA_Type_III_HS_A1     -1.030f  //-1.110f  //-1.241f  //-1.030f
#define Default_CLA_Type_III_HS_A2     0.109f   //0.201f   //0.317f   //0.109f

// PI controller of type III Compensator          1k/15000   850
#define Default_CLA_Type_III_PI_KP     0.0002*0.941f //0.924f*2 //0.941f*0.2
#define Default_CLA_Type_III_PI_KI     0.0002*0.059f //0.075f*2 //0.059f*0.2
#define Default_CLA_Type_III_PI_KC     0.0063f     //0.081f*2 //0.063f
#define Default_CLA_Type_III_PI_KA     10.957f    //8.053f/2 //10.957f
#define Default_CLA_Type_III_PI_Limit  0.091f*2   //0.124f*4 //0.091f*2

/*  Old
// Default parameters
// High stop filter of type III Compensator
#define Default_CLA_Type_III_HS_B0     0.131f   //0.031f   //0.085f   //0.078f   //0.131f   //0.061f
#define Default_CLA_Type_III_HS_B1     0.023f   //0.007f   //0.019f   //0.016f   //0.023f   //0.014f
#define Default_CLA_Type_III_HS_B2     -0.109f  //-0.024f  //-0.066f  //-0.062f  //-0.109f   //-0.047f
#define Default_CLA_Type_III_HS_A1     -0.741f  //-1.105f  //-0.747f  //-0.752f  //-0.741f   //-1.089f
#define Default_CLA_Type_III_HS_A2     -0.214f  //0.118f   //-0.215f  //-0.216f  //-0.214f   //0.116f

// PI controller of type III Compensator
#define Default_CLA_Type_III_PI_KP     0.930f*2
#define Default_CLA_Type_III_PI_KI     0.070f*2
#define Default_CLA_Type_III_PI_KC     0.075f*2
#define Default_CLA_Type_III_PI_KA     2.151f/2
#define Default_CLA_Type_III_PI_Limit  0.465f*4
// PI controller of type III Compensator

#define Default_CLA_Type_III_PI_KP     0.929f   //0.941f   //0.940f   //0.929f   //0.929f   //0.941f
#define Default_CLA_Type_III_PI_KI     0.070f   //0.059f   //0.059f   //0.070f   //0.070f   //0.059f
#define Default_CLA_Type_III_PI_KC     0.075f   //0.063f   //0.063f   //0.075f   //0.075f   //0.063f
#define Default_CLA_Type_III_PI_KA     5.981f   //9.883f   //9.456f   //7.306f   //5.981f//750Hz    //4.376f//580Hz    //4.103f//550Hz  //3.884f//525Hz //3.666f//500Hz     //3.233f//400Hz    //4.947f
#define Default_CLA_Type_III_PI_Limit  0.167f   //0.101f   //0.106f   //0.137f   //0.167f           //0.219f           //0.228f            //0.244f        //0.258f        //0.273f            //0.309f           //0.201f
*/

#define AHB_PERIOD_MAX          240
#define AHB_PERIOD_MIN          40

#define AHB_PERIOD_STEP_UP      1
#define AHB_PERIOD_STEP_DOWN    1
// Low pass filter of VCS
#define Default_CLA_VCS_LPF_B0         0.7153f   //0.3858f
#define Default_CLA_VCS_LPF_B1         0.7153f   //0.0f
#define Default_CLA_VCS_LPF_A1         0.4307f   //-0.6141f
// PR Controller
#define Default_CLA_PR_B0         1.00126f      //
#define Default_CLA_PR_B1         -1.99744f     //
#define Default_CLA_PR_B2         0.99623f      //
#define Default_CLA_PR_A1         -1.99744f     //
#define Default_CLA_PR_A2         0.99749f      //

// Vea Table
#if(1)
#define Default_CLA_VEA_F0   250.00f //kHz
#define Default_CLA_VEA_F1   143.00f //160 //143
#define Default_CLA_VEA_F2   125.00f //132 //124
#define Default_CLA_VEA_F3   106.00f
#define Default_CLA_VEA_F4   106.00f

#define Default_CLA_VEA_P0   0.1f
#define Default_CLA_VEA_P1   0.4f
#define Default_CLA_VEA_P2   1.0f
#define Default_CLA_VEA_P3   1.6f
#define Default_CLA_VEA_P4   2.0f

#define Default_CLA_VEA_S0   (float)(120000.0f / Default_CLA_VEA_F0 / 2.0f)
#define Default_CLA_VEA_S1   (float)(120000.0f / Default_CLA_VEA_F1 / 2.0f)
#define Default_CLA_VEA_S2   (float)(120000.0f / Default_CLA_VEA_F2 / 2.0f)
#define Default_CLA_VEA_S3   (float)(120000.0f / Default_CLA_VEA_F3 / 2.0f)
#define Default_CLA_VEA_S4   (float)(120000.0f / Default_CLA_VEA_F4 / 2.0f)

#define Default_CLA_VEA_M0   0.0f
#define Default_CLA_VEA_M1   598.60f  //450.0f  //598.60f
#define Default_CLA_VEA_M2   100.70f  //132.58f //107.15f
#define Default_CLA_VEA_M3   143.40f  //203.96f //155.09f
#define Default_CLA_VEA_M4   0.0f

#define Default_CLA_VEA_B0   240.00f
#define Default_CLA_VEA_B1   180.14f  //195.0f  //180.14f
#define Default_CLA_VEA_B2   379.30f  //321.97f //376.72f
#define Default_CLA_VEA_B3   336.60f  //250.58f //328.78f
#define Default_CLA_VEA_B4   566.04f

#define Default_CLA_BURST_K  (float)(90.0f / Default_CLA_VEA_P0)
#define Default_CLA_iYn1     (float)(Default_CLA_VEA_P0 * Default_CLA_Type_III_PI_Limit)

#else

#define Default_CLA_VEA_F0   300.00f
#define Default_CLA_VEA_F1   125.00f
#define Default_CLA_VEA_F2   103.00f
#define Default_CLA_VEA_F3   100.00f

#define Default_CLA_VEA_P0   0.1f
#define Default_CLA_VEA_P1   0.6f
#define Default_CLA_VEA_P2   0.85f
#define Default_CLA_VEA_P3   1.00f

#define Default_CLA_VEA_S0   (float)(120000.0f / Default_CLA_VEA_F0 / 2.0f)
#define Default_CLA_VEA_S1   (float)(120000.0f / Default_CLA_VEA_F1 / 2.0f)
#define Default_CLA_VEA_S2   (float)(120000.0f / Default_CLA_VEA_F2 / 2.0f)
#define Default_CLA_VEA_S3   (float)(120000.0f / Default_CLA_VEA_F3 / 2.0f)

#define Default_CLA_VEA_M0   0.0f
#define Default_CLA_VEA_M1   678.26f
#define Default_CLA_VEA_M2   327.27f
#define Default_CLA_VEA_M3   160.43f

#define Default_CLA_VEA_B0   260.87f
#define Default_CLA_VEA_B1   193.04f
#define Default_CLA_VEA_B2   403.63f
#define Default_CLA_VEA_B3   545.45f


#define Default_CLA_BURST_K  (float)(18.0f / Default_CLA_VEA_P0)
#define Default_CLA_iYn1     (float)(Default_CLA_VEA_P0 * Default_CLA_Type_III_PI_Limit)
#endif

#define Default_AHB_V_PI_KP     0.01*0.0941f
#define Default_AHB_V_PI_KI     0.01*0.059f
#define Default_AHB_V_PI_KC     (Default_AHB_V_PI_KI/Default_AHB_V_PI_KP) //(Ki/Kp)
#define Default_AHB_V_PI_KA     1.0f
#define Default_AHB_V_PI_Limit  400.0f*0.6 //TBPRD = 600

#define Default_AHB_I_PI_KP     0.01*0.00541f
#define Default_AHB_I_PI_KI     0.01*0.0001f
#define Default_AHB_I_PI_KC     (Default_AHB_I_PI_KI/Default_AHB_I_PI_KP) //(Ki/Kp)
#define Default_AHB_I_PI_KA     1.0f
#define Default_AHB_I_PI_Limit  400.0f*0.6 //TBPRD = 600

/* Type definitions */
// Structure & union
typedef struct
{
    long period;
    long priDutyLead;
    long priDutyLag;
    long priDeadband;
    long priDutyMid;
    long srDutyLead;
    long srDutyLag;
    long srDeadband;
    long srDutyMid;

    float iYn1Buff;
    float sotcFlag;
}EpwmCmpValueType;

typedef struct
{
    float m0;
    float m1;
    float m2;
    float m3;
    float m4;
    float b0;
    float b1;
    float b2;
    float b3;
    float b4;

    float P0;
    float P1;
    float P2;
    float P3;
    float P4;

    float S0;
    float S1;
    float S2;
    float S3;
    float S4;

    float m5;
    float b5;

    float BurstK;
    float iYn1;
}ClaFloatVeaType;

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
}ClaFloatHighStopFilterType;

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
}ClaFloatPiControlType;

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
}ClaFloatPRControlType;

typedef struct
{
    float b0;
    float b1;
    float a1;
    float un;
    float un1;
    float yn;
    float yn1;
}ClaFloatLowPassFilterType;

//typedef struct ClaCompensatorType ClaCompensatorObj;

typedef struct
{
    struct
    {
        ClaFloatHighStopFilterType hsFilter;
        ClaFloatPiControlType piControl;
        ClaFloatLowPassFilterType lpFilter;
        ClaFloatPRControlType prControl;
        long reference;
        long feedback;
        long error;
        long output;
        long maximum;
        long minimum;
        long qNotation;
    }member;
}ClaCompensatorType;

/* Global data declarations */

/* Macro definitions */
// CPU to CLA message index

// Unsigned long type messages
#define CPU_Vout_Ref            ulongCpuToClaMsg[0]
#define CPU_Burst_Mode          ulongCpuToClaMsg[1]
#define CPU_LLC_State           ulongCpuToClaMsg[2]
#define CPU_Vout_Ref_CONSTAT    ulongCpuToClaMsg[3]
#define CPU_VCharge_Ref         ulongCpuToClaMsg[4]
#define CPU_ICharge_Ref         ulongCpuToClaMsg[5]
#define CPU_AHB_State           ulongCpuToClaMsg[6]
#define CPU_LLC_Vout            ulongCpuToClaMsg[7]
#define CPU_AHB_Vout            ulongCpuToClaMsg[8]
#define CPU_AHB_Iout            ulongCpuToClaMsg[9]
// Signed long type messages
#define CPU_IL_R_Offset         longCpuToClaMsg[0]


// CLA to CPU message index
// Float type messages
#define CLA_Iin_Alpha           floatClaToCpuMsg[0]

// Signed long type messages
#define CLA_LLC_VOUT            longClaToCpuMsg[0]
#define CLA_LLC_IOUT            longClaToCpuMsg[1]
#define CLA_LLC_OFF             longClaToCpuMsg[2]
#define CLA_AHB_VOUT            longClaToCpuMsg[3]
#define CLA_AHB_IOUT            longClaToCpuMsg[4]
#define CLA_AHB_OFF             longClaToCpuMsg[5]
/* Global function prototypes */
void CLA_ADC_A1_ISR(void);

/* Global data declarations */
extern long ulongCpuToClaMsg[16];
extern long longCpuToClaMsg[16];
extern uint16_t ahbTestPeriod;

#ifndef CLA_File
__attribute__((noblocked))
#endif
extern float floatClaToCpuMsg[32];

#ifndef CLA_File
__attribute__((noblocked))
#endif
extern long ulongClaToCpuMsg[16];

#ifndef CLA_File
__attribute__((noblocked))
#endif
extern long longClaToCpuMsg[16];

#endif /* INCLUDE_CPU_CLA_MESSAGE_H_ */
