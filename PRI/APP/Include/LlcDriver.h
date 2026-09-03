/***************************************************************************************************
File Name: LlcDriver.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ --------------------------------------------------------

Description:

====================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- --------------------------------------------------------------------
    11/26/2019   Fred            Create file

***************************************************************************************************/

#ifndef _LLCDRIVER_H_
#define _LLCDRIVER_H_

//------------------------------------------------------------------------------
// Condition definitions
//------------------------------------------------------------------------------
#define RIPPLE_FEEDFORWARD      0

//------------------------------------------------------------------------------
// Macro definitions
//------------------------------------------------------------------------------
#define MAX_DIS_POWER           (17.3e3f)
#define MAX_DIS_CURRENT         (43.70f)
#define MAX_SWITCH_FREQ         (250.0e3f)
#define MIN_SWITCH_FREQ         (106.0e3f)
#define MAX_SWITCH_PERIOD       (unsigned short)(CPU_CLK / MIN_SWITCH_FREQ / 2)
#define MIN_SWITCH_PERIOD       (unsigned short)(CPU_CLK / MAX_SWITCH_FREQ / 2)
#define MIN_SWITCH_DB           (unsigned short)(CPU_CLK * (0.3e-6f))
#define MIN_SR_DB               (unsigned short)(CPU_CLK * (0.7e-6f))

#define SWITCH_PERIOD(f)        (unsigned short)(CPU_CLK / f / 2)
#define SWITCH_DB(t)            (unsigned short)(CPU_CLK * t)

#define OUTPUT_VOLT(v)          (long)(((long)(v) * __IQ(1 / 743.985f, 24)) >> 10)  //743.985
#define OUTPUT_VOLT_BUS(v)      (long)(((long)(v) * __IQ(1 / 991.000f, 24)) >> 10)  //743.985 //new add 472.478
#define OUTPUT_VOLT_BATT(v)     (long)(((long)(v) * __IQ(1 / 373.924f, 24)) >> 10)  //743.985 //new add 373.924
#define OUTPUT_CURR(i)          (long)(((long)(i) * __IQ(1 / 11.11f, 24)) >> 10)    //100.0f  //new add 21.88
#define OUTPUTDIS_CURR(i)       (long)(((long)(i) * __IQ(1 / 101.2f, 24)) >> 10)    //200.0f  //new add 101.2

#define Default_SOH_I_PI_KP     0.01*0.001f
#define Default_SOH_I_PI_KI     0.01*0.00001f
#define Default_SOH_I_PI_KC     (Default_AHB_V_PI_KI/Default_AHB_V_PI_KP)
#define Default_SOH_I_PI_KA     1.0f
#define Default_SOH_I_PI_Limit  12.0f
//------------------------------------------------------------------------------
// Type definitions
//------------------------------------------------------------------------------
typedef enum
{
    AHB_OFF_MODE = 0,
    AHB_CV_MODE,
    AHB_CC_MODE,
    AHB_ON_MODE
} AhbCtrlMode;

typedef enum
{
    LLC_OFF_MODE = 0,
    LLC_ON_MODE
} LlcCtrlMode;

typedef enum
{
    LLC_HALT = 0,
    LLC_SOFTSTART,
    LLC_CV,
    LLC_CC,
    LLC_BURST,
    LLC_SHORT,
    LLC_OPENLOOP,
    LLC_SOTC
} LlcCtrlState;

typedef enum
{
    SOTC_DISABLE = 0,
    SOTC_LATCH,
    SOTC_STANDBY,
    SOTC_EXTEND,
    SOTC_STEP_UP,
    SOTC_STEP_DOWN
} SotcCtrlState;

typedef enum
{
    DEADZONE_DISABLE = 0,
    DEADZONE_STANDBY,
    DEADZONE_STEP_UP,
    DEADZONE_LATCH_UP,
    DEADZONE_STEP_DOWN,
    DEADZONE_LATCH_DOWN,
    DEADZONE_LATCH_BURST
} DeadzoneCtrlState;

typedef struct
{
    long reference;
    long feedback;
    long output;
    long maximum;
    long minimum;
    long b0;
    long b1;
    long b2;
    long b3;
    long a1;
    long a2;
    long a3;
    long En[4];
    long Yn[4];
} LlcCtrlParam;

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
}LlcFloatPiControlType;

typedef struct
{
   struct
   {
       long reference;
       long feedback;
       long maximum;
       long minimum;
   }curr;

   struct
   {
       long reference;
       long feedback;
       long maximum;
       long minimum;
   }volt;

} AhbCtrlParam;

//------------------------------------------------------------------------------
// Global function prototypes
//------------------------------------------------------------------------------
void AhbController(void);
void Set_Ahb_Mode(AhbCtrlMode mode);
void Llc_Sr_Pwm_Off(void);
void Chg_Pwm_On(void);
void Chg_Pwm_Off(void);

void LlcController(void);
void Set_Llc_Mode(LlcCtrlMode mode);
void Sr_Pwm_Off(void);
void Sr_Pwm_On(void);
unsigned short Get_Llc_State(void);

void Set_Voltage_Setpoint(unsigned long volt);
unsigned short Soft_Start_Routine(void);
void Set_Current_Setpoint(unsigned long curr);
unsigned short Soft_Start_Routine_Curr(void);

void Current_Sharing(void);
void Soh_Test_Voltage_Control(void);

void BMS_Current_Feedback(void);

void Voltage_Droop (void);

extern unsigned short ahbVoltSetpoint;
extern unsigned short ahbCurrSetpoint;
extern unsigned short ahbVoltReference;
extern unsigned short ahbCurrReference;
//------------------------------------------------------------------------------
// Global data declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#endif /* _LLCDRIVER_H_ */
