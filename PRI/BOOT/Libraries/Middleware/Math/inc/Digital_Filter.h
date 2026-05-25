/**************************************************************************************************
File Name: Digital_Filter.h
Global Data:
    Name                     Type               Description
    ------------------------ ------------------ --------------------------------------------------
    None
Description:
    Header file of Digital_Filter.c.

===================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- -----------------------------------------------------------------
    04/08/2020 Watch Lee        1. version 1.0.
    11/10/2020 Watch Lee        1. support version 1.1.
    05/13/2021 Watch Lee        1. support version 1.2.

**************************************************************************************************/

#ifndef __DIGITAL_FILTER_H__
#define __DIGITAL_FILTER_H__

/* Macro definitions */
// Default 1st DLPF arguments, Q14 format, sample period = 1ms, cut-off frequency = 5Hz
#define A0_1st                              15878   // 0.969117 * 2^14
#define B0_1st                              253     // 0.015442 * 2^14
// Default 2nd DLPF arguments, Q14 format, sample period = 1ms, cut-off frequency = 5Hz
#define A0_2nd                              15679   // 0.956970 * 2^14
#define A1_2nd                              32047   // 1.955993 * 2^14
#define B0_2nd                              4       // 0.000244 * 2^14

// Group of digital filter declarations
#define Create_Simple_Filter(Name, Exp)     SimpleFilterObj Name = Simple_Filter_Set(Exp)

#define Create_DLPF_1st_Default(Name)       DlpFilterObj Name = DLPF_1st_Defaults
#define Create_DLPF_1st(Name, A0, B0)       DlpFilterObj Name = DLPF_1st_Set(A0, B0)

#define Create_DLPF_2nd_Default(Name)       DlpFilterObj Name = DLPF_2nd_Defaults
#define Create_DLPF_2nd(Name, A0, A1, B0)   DlpFilterObj Name = DLPF_2nd_Set(A0, A1, B0)

#define Create_Cascade3_DLPF_Default(Name)  CascadeFilterObj Name = Cascade3_DLPF_Defaults
#define Create_Cascade3_DLPF(Name, A0, B0)  CascadeFilterObj Name = Cascade3_DLPF_Set(A0, B0)

#define Create_Avg_Rect_Default(Name)       AvgRectObj Name = Avg_Rect_Defaults
#define Create_Avg_Rect(Name, \
                        Scale, \
                        Output)             AvgRectObj Name = Avg_Rect_Set(Scale, Output)

// Group of moving average declarations - Order MUST be power of 2 (2^n)
#define Create_Move_Avg(Name, Order)        long Name##Un[(Check_Power_2(Order) > 0) ? Check_Power_2(Order) : -1];\
                                            MoveAvgObj Name = \
                                            {\
                                                {{Name##Un, 0}, (float)Order / 65536, Order, 0, 0}, \
                                                {Move_Average_Calculation, Move_Average_Reset}\
                                            }

// Default parameters
#define Simple_Filter_Set(Exponent)         {\
                                                {{(1 << (Exponent & 0x7)) - 1, 0}, Exponent & 0x7, 0}, \
                                                {Simple_Filter_Calculation, Simple_Filter_Reset}\
                                            }

#define DLPF_1st_Defaults                   {\
                                                {{A0_1st, 0, B0_1st, 0, 0, 0, 0, 0, 0}, 0}, \
                                                {DLPF_1st_Calculation, DLPF_Reset}\
                                            }

#define DLPF_1st_Set(A0, B0)                {\
                                                {{A0, 0, B0, 0, 0, 0, 0, 0, 0}, 0}, \
                                                {DLPF_1st_Calculation, DLPF_Reset}\
                                            }

#define DLPF_2nd_Defaults                   {\
                                                {{A0_2nd, A1_2nd, B0_2nd, 0, 0, 0, 0, 0, 0}, 0}, \
                                                {DLPF_2nd_Calculation, DLPF_Reset}\
                                            }

#define DLPF_2nd_Set(A0, A1, B0)            {\
                                                {{A0, A1, B0, 0, 0, 0, 0, 0, 0}, 0}, \
                                                {DLPF_2nd_Calculation, DLPF_Reset}\
                                            }

#define Cascade3_DLPF_Defaults              {\
                                                {DLPF_1st_Defaults, DLPF_1st_Defaults, DLPF_1st_Defaults, 0}, \
                                                {Cascade3_DLPF_Calculation, Cascade3_DLPF_Reset}\
                                            }

#define Cascade3_DLPF_Set(A0, B0)           {\
                                                {DLPF_1st_Set(A0, B0), DLPF_1st_Set(A0, B0), DLPF_1st_Set(A0, B0), 0}, \
                                                {Cascade3_DLPF_Calculation, Cascade3_DLPF_Reset}\
                                            }

#define Avg_Rect_Defaults                   {\
                                                {DLPF_2nd_Defaults, 32768, 0, 4096, 0, 0, 0, 0, 0, 0, 0}, \
                                                {Sine_Average_Rectified_Calculation, Sine_Average_Rectified_Reset}\
                                            }

#define Avg_Rect_Set(Scale, Output)         {\
                                                {DLPF_2nd_Defaults, 32768, 0, Scale, 0, 0, 0, 0, 0, 0, Output}, \
                                                {Sine_Average_Rectified_Calculation, Sine_Average_Rectified_Reset}\
                                            }

// Statement Replacement
#ifndef Check_Power_2
#define Check_Power_2(Number)               (((((unsigned)Number - 1) ^ Number) == (Number << 1) - 1) ? Number : -1)
#endif

/* Type definitions */
// Enumeration
typedef enum
{
    AVG_RECT_SCALE_RMS,
    AVG_RECT_SCALE_MEAN,
    AVG_RECT_SCALE_PEAK,
    AVG_RECT_RAW_RMS,
    AVG_RECT_RAW_MEAN,
    AVG_RECT_RAW_PEAK
}EnumAvgRectOutput;

// Structure & union
typedef struct SimpleFilterType SimpleFilterObj;

struct SimpleFilterType
{
    struct
    {
        struct
        {
            long a0;
            long yn1;
        }coeff;

        unsigned short exponent;
        long result;
    }member;

    struct
    {
        long (*calculate)(SimpleFilterObj*, short);
        void (*reset)(SimpleFilterObj*);
    }func;
};

typedef struct MoveAvgType MoveAvgObj;

struct MoveAvgType
{
    struct
    {
        struct
        {
            long *un;
            long yn;
        }coeff;

        float eFactor;
        unsigned short order;
        unsigned short index;
        long result;
    }member;

    struct
    {
        long (*calculate)(MoveAvgObj*, short);
        void (*reset)(MoveAvgObj*);
    }func;
};

typedef struct DlpFilterType DlpFilterObj;

struct DlpFilterType
{
    struct
    {
        struct
        {
            long a0;
            long a1;
            long b0;
            long un;
            long un1;
            long un2;
            long yn;
            long yn1;
            long yn2;
        }coeff;

        long result;
    }member;

    struct
    {
        long (*calculate)(DlpFilterObj*, short);
        void (*reset)(DlpFilterObj*);
    }func;
};

typedef struct CascadeFilterType CascadeFilterObj;

struct CascadeFilterType
{
    struct
    {
        DlpFilterObj filterLv1;
        DlpFilterObj filterLv2;
        DlpFilterObj filterLv3;
        long result;
    }member;

    struct
    {
        long (*calculate)(CascadeFilterObj*, short);
        void (*reset)(CascadeFilterObj*);
    }func;
};

typedef struct AvgRectType AvgRectObj;

struct AvgRectType
{
    struct
    {
        DlpFilterObj filter;
        long gain;
        long offset;
        long scale;
        long rawMean;
        long rawRms;
        long rawPeak;
        long scaleMean;
        long scaleRms;
        long scalePeak;
        EnumAvgRectOutput outputType;
    }member;

    struct
    {
        long (*calculate)(AvgRectObj*, short);
        void (*reset)(AvgRectObj*);
    }func;
};

/* Global function prototypes */
long Simple_Filter_Calculation(SimpleFilterObj *obj, short input);
void Simple_Filter_Reset(SimpleFilterObj *obj);
long Move_Average_Calculation(MoveAvgObj *obj, short input);
void Move_Average_Reset(MoveAvgObj *obj);
long DLPF_1st_Calculation(DlpFilterObj *obj, short input);
void DLPF_Reset(DlpFilterObj *obj);
long DLPF_2nd_Calculation(DlpFilterObj *obj, short input);
long Cascade3_DLPF_Calculation(CascadeFilterObj *obj, short input);
void Cascade3_DLPF_Reset(CascadeFilterObj *obj);
long Sine_Average_Rectified_Calculation(AvgRectObj *obj, short input);
void Sine_Average_Rectified_Reset(AvgRectObj *obj);

/* Global data declarations */

#endif
