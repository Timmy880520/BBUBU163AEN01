/***********************************************************************************************************************
File Name: Univ_Lib.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ ----------------------------------------------------------------------------
    None
Description: 
    Header file of Univ_Lib.c

========================================================================================================================
History:
    Date        Author          Description Of Change
    ----------- --------------- ----------------------------------------------------------------------------------------
    07/22/2013  Watch Lee       Ver.1.0
    06/02/2017  Watch Lee       Update to Ver.1.1 for DLPF_Calculation.
    06/21/2017  Watch Lee       Update to Ver.1.2 for signal and event state function.
    02/22/2018  Watch Lee       Update to Ver.2.0 for standard coding style.
    03/25/2020  Fred Huang      Add CMA_Calculation and In_Range_Check
    03/07/2021  Fred Huang      Modify CMAtoRealPkt.gain and CMAtoRealPkt.offset from variable to pointer
    03/11/2021  Fred Huang      Add Calibration_Calculation function
    03/19/2021  Fred Huang      1. Modify ProtectionPkt, separate three condition counter
                                2. Remove Over_Range_Check, Under_Range_Check and In_Range_Check
                                3. Add Range_Check

***********************************************************************************************************************/

#ifndef __UNIV_LIB_H__
#define __UNIV_LIB_H__

/* Macro definitions */
#define X10                                     10
#define X100                                    100
#define NTC_Table_Size                          176
#define Create_ADC_Value(Name)                  AdcVal Name = {0, 0, 0, 0, 0, &Name.hexVal}
#define Create_ADC_Calibration(Name)            AdcCalib Name##_Calib = {32768, 0, 4096, 0, &Name}
#define ADC_Calibration(Name)                   ADC_Data_Calibration(&Name##_Calib)
#define Create_Table_Package(Name, Tab)         TabPkg Name = {Tab, sizeof(Tab) >> (sizeof(Tab[0]) >> 1)}
#define Look_Table(Data, Name)                  Data_Lookup(Data, Name.tab, Name.size)
#define Create_Timer(Name, Cmp)                 TmrPkg Name = {0, Cmp}
#define Call_Timer(Name)                        Software_Timer(&Name)
#define Timer_Flag(Name)                        Name.reg.bits.flg
#define Timer_Count(Name)                       Name.reg.bits.cnt
#define Timer_Reset(Name)                       Name.reg.val = 0
#define Timer_Countdown(Name)                   Name.cmp - Timer_Count(Name)
#define Create_Counter(Name, Cmp)               TmrPkg Name = {0, Cmp}
#define Call_Counter(Name, Val)                 Software_Counter(&Name, Val)
#define Counter_Reset(Name)                     Name.reg.val = 0

#define Default_2nd_A0                          16028	     // Sample frequency = 2kHz, cut-off frequency = 5Hz
#define Default_2nd_A1                          32408	     // Sample frequency = 2kHz, cut-off frequency = 5Hz
#define Default_2nd_B2                          1            // Sample frequency = 2kHz, cut-off frequency = 5Hz
#define Create_2nd_DLPF(Name, AdcStr)           Equation_2ndPar Name =                  \
                                                {                                       \
                                                    0, 0, 0, 0, 0, 0,                   \
                                                    Default_2nd_A0, Default_2nd_A1,     \
                                                    Default_2nd_B2, &AdcStr             \
                                                }

#define Create_DLPF_Real_Value(Name, AdcStr,                                            \
                               Gain, Offset,                                            \
                               Scale)           DLPFtoRealPkt Name =                    \
                                                {                                       \
                                                    0, 0, 0, 0, 0, 0,                   \
                                                    Gain,                               \
                                                    Offset,                             \
                                                    Scale,                              \
                                                    &AdcStr                             \
                                                }

#define Create_CMA_Real_Value(Name,             \
                              AdcVal,           \
                              Shift,            \
                              Shift1,           \
                              Gain,             \
                              Offset,           \
                              Scale)            CMAtoRealPkt Name =     \
                                                {                       \
                                                    0, 0, 0, 0, Shift,  \
                                                    Shift1,             \
                                                    &Gain,              \
                                                    &Offset,            \
                                                    Scale,              \
                                                    &AdcVal,            \
                                                    0,0,                \
                                                }

#define Create_Protection_Pkg(Name,             \
                              Value,            \
                              Lowerlv,          \
                              Upperlv,          \
                              underLatency,     \
                              betweenLatency,   \
                              overLatency)      ProtectionPkt Name =    \
                                                {                       \
                                                    &Value,             \
                                                    Lowerlv,            \
                                                    Upperlv,            \
                                                    underLatency,       \
                                                    betweenLatency,     \
                                                    overLatency,        \
                                                    0, 0, 0, 0          \
                                                }

#define Create_Event_Counter(Name, Tcmp, Fcmp)       EventTmrPkg Name = {{0, Tcmp}, {0, Fcmp}}
#define Update_Signal_State(Event, Tmr, Tcond)       Event = Check_Signal_State(Event, &Tmr, Tcond)
#define Update_Event_State(Event, Tmr, Tcond, Fcond) Event = Check_Event_State(Event, &Tmr, Tcond, Fcond)
#define Create_Decimal_Calibration(Name)             DecCalib Name##_Calib = {1000, 0, 4000, 4000, &Name}

/* Type definitions */
// Structure & union
enum
{
    UNDER_RANGE = 0,
    IN_RANGE,
    OVER_RANGE
};

typedef struct
{
    long sum;
    long sum1;
    long cma;
    long cma1;
    long shift;
    long shift1;
    short *const gain;
    short *const offset;
    long scale;
    volatile unsigned short *const raw;
    long val;
    long val1;
} CMAtoRealPkt;

typedef struct
{
    long *const val;
    long lowerlevel;
    long upperlevel;
    long underLatency;
    long betweenLatency;
    long overLatency;
    long underCount;
    long betweenCount;
    long overCount;
    unsigned short flag;
} ProtectionPkt;

typedef struct
{
    short decVal;                      // ADC decimal value
    short decInt;                      // ADC decimal integer 
    long hexVal;                       // ADC hexadecimal value
    long calibHex;                     // ADC calibrated hexadecimal value
    long filt;                         // ADC buffer for digital filtering
    long *valPtr;                      // ADC hexadecimal value pointer
}AdcVal;                               // ADC data value structure

typedef struct      
{         
    long gain;          
    long offset;        
    long limMax;                       // Maximum limitation of ADC calibrated data 
    long limMin;                       // Minimum limitation of ADC calibrated data
    AdcVal *data;                      // ADC data pointer
}AdcCalib;                             // ADC data calibration structure

typedef struct
{
    long gain;
    long offset;
    long maxGain;                      // Maximum limitation of calibration gain 
    long maxOffset;                    // Maximum limitation of ADC calibration offset 
    AdcVal *data;                      // ADC data pointer
}DecCalib;                             // Decimal data calibration structure

typedef struct
{
    struct
    {
        long un;
        long un1;
        long un2;
        long yn;
        long yn1;
        long yn2;
        long a0;
        long a1;
        long b2;
    }par;
    AdcVal *val;                       // ADC data pointer
}Equation_2ndPar;                      // 2nd DLPF equation structure

typedef struct
{
    unsigned short *tab;               // Lookup table pointer
    unsigned short size;          
}TabPkg;                               // Lookup table data package

typedef struct
{
    union
    {
        unsigned long val;             // Software timer register value
        struct
        {
            unsigned long cnt:31;      // Counter of software timer
            unsigned long flg:1;       // Timeout flag of software timer
        }bits;
    }reg;
    unsigned long cmp;                 // Compare value of software timer, maximum value = 2^31 - 2
}TmrPkg;                               // Software timer register structure

typedef struct
{
    TmrPkg trueCount;                  // True event counter
    TmrPkg falseCount;                 // False event counter
}EventTmrPkg;                          // Timer package of event detection

typedef struct 
{
    long un;
    long un1;
    long un2;
    long yn;
    long yn1;
    long yn2;
    long gain;
    long offset;
    long scale;	
    AdcVal *val;	
}DLPFtoRealPkt;

/* Global function prototypes */
void ADC_Data_Conversion(AdcVal *data, long stepSize, unsigned long accuracy);
long Sqrt_Calculation(long data);
char Software_Counter(TmrPkg *pkg, char flag);
unsigned char Software_Timer(TmrPkg *pkg);
short Data_Lookup(short data, unsigned short *tab, unsigned short size);
void ADC_Data_Calibration(AdcCalib *par);
long DLPF_Calculation(Equation_2ndPar *data);
unsigned char Check_Signal_State(unsigned char state, TmrPkg *count, unsigned char signal);
unsigned char Check_Event_State(unsigned char state, EventTmrPkg *count,
                                unsigned char trueEvent, unsigned char falseEvent);
void Decimal_Data_Calibration(DecCalib *par, unsigned long accuracy);
short NTC_Lookup(short data, unsigned short const *ptr);
long Average_Calculation(DLPFtoRealPkt *data);
long CMA_Calculation(CMAtoRealPkt *data);
long Real_Value_Calculation(CMAtoRealPkt *data);
long Real_Value_Calculation1(CMAtoRealPkt *data);
long Calibration_Calculation(long data,short gain, short offset);
void Range_Check(ProtectionPkt *data);
long Data_Saturation(long data, long max, long min);

#endif
