/**************************************************************************************************
File Name: Univ_Lib.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ -----------------------------------------------------
    None
Description: 
    Header file of Univ_Lib.c

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    07/22/2013 Watch Lee        Ver.1.0
    06/02/2017 Watch Lee        Update to Ver.1.1 for DLPF_Calculation.
    06/21/2017 Watch Lee        Update to Ver.1.2 for signal and event state function.
    02/22/2018 Watch Lee        Update to Ver.2.0 for standard coding style.
    04/16/2020 Watch Lee        Modify to Ver.2.1 for modification of the table lookup algorithm.

**************************************************************************************************/

#ifndef __UNIV_LIB_H__
#define __UNIV_LIB_H__

/* Macro definitions */
#define X10                                          10
#define X100                                         100
#define Default_2nd_A0                               15679	     // Sample frequency = 1kHz, cut-off frequency = 5Hz
#define Default_2nd_A1                               32047	     // Sample frequency = 1kHz, cut-off frequency = 5Hz
#define Default_2nd_B2                               4		     // Sample frequency = 1kHz, cut-off frequency = 5Hz
#define Create_ADC_Value(Name)                       AdcVal Name = {0, 0, 0, 0, 0, &Name.hexVal}
#define Create_ADC_Calibration(Name)                 AdcCalib Name##Calib = {32768, 0, 4096, 0, &Name}
#define ADC_Calibration(Name)                        ADC_Data_Calibration(&Name##Calib)
#define Create_Table_Package(Name, Tab, Size)        TabPkg Name = {(unsigned short*)Tab, Size}
#define Look_Table(Data, Name)                       Data_Lookup(Data, Name.table, Name.size)
#define Create_Timer(Name, Cmp)                      TmrPkg Name = {0, Cmp}
#define Call_Timer(Name)                             Software_Timer(&Name)
#define Timer_Flag(Name)                             Name.reg.bits.flg
#define Timer_Count(Name)                            Name.reg.bits.cnt
#define Timer_Reset(Name)                            Name.reg.val = 0
#define Create_Counter(Name, Cmp)                    TmrPkg Name = {0, Cmp}
#define Call_Counter(Name, Val)                      Software_Counter(&Name, Val)
#define Counter_Reset(Name)                          Name.reg.val = 0
#define Create_2nd_DLPF(Name, AdcStr)                SecondOrderType Name = \
                                                     {\
                                                         0, 0, 0, 0, 0, 0,\
                                                         Default_2nd_A0, Default_2nd_A1,\
                                                         Default_2nd_B2, &AdcStr\
                                                     }
#define Create_Event_Counter(Name, Tcmp, Fcmp)       EventTmrPkg Name = {{0, Tcmp}, {0, Fcmp}}
#define Update_Signal_State(Event, Tmr, Tcond)       Event = Check_Signal_State(Event, &Tmr, Tcond)
#define Update_Event_State(Event, Tmr, Tcond, Fcond) Event = Check_Event_State(Event, &Tmr, Tcond, Fcond)
#define Create_Decimal_Calibration(Name)             DecCalib Name##_Calib = {1000, 0, 4000, 4000, &Name}

/* Type definitions */
// Structure & union
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
}SecondOrderType;                      // 2nd DLPF equation structure

typedef struct
{
    unsigned short *table;             // Lookup table pointer
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

/* Global function prototypes */
void ADC_Data_Conversion(AdcVal *data, long stepSize, unsigned long accuracy);
long Sqrt_Calculation(long data);
char Software_Counter(TmrPkg *pkg, char flag);
unsigned char Software_Timer(TmrPkg *pkg);
short Data_Lookup(short data, unsigned short *tab, unsigned short size);
void ADC_Data_Calibration(AdcCalib *par);
long DLPF_Calculation(SecondOrderType *data);
unsigned char Check_Signal_State(unsigned char state, TmrPkg *count, unsigned char signal);
unsigned char Check_Event_State(unsigned char state, EventTmrPkg *count,
                                unsigned char trueEvent, unsigned char falseEvent);
void Decimal_Data_Calibration(DecCalib *par, unsigned long accuracy);

#endif
