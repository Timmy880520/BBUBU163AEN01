/**************************************************************************************************
File Name: AC_Math_Lib.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ ------------------------------------------------------
    None
Description: 
    Header file of AC_Math_Lib.c

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- ------------------------------------------------------------------
    06/13/2017 Watch Lee        1. version 1.0.
    08/27/2018 Watch Lee        1. version 2.0.
                                2. modify to fit standard coding style.
    02/27/2020 Watch Lee        1. support version 2.1.

**************************************************************************************************/

#ifndef __AC_MATH_LIB_H__
#define __AC_MATH_LIB_H__

/* Includes */
#include "..\..\Math\inc\Sine_Process.h"
#include "Univ_Lib.h"

/* Macro definitions */
// PLL based frequency (Hz)
#define PLL_Based               25000L

// Default SYNC delay parameter 
/*
1st LPF phase delay = arctan(2pi * fo * RC) / 2pi * PLL_Based / fo, RC = 2.7m, fo = 47 ~ 63Hz
2pi * fo * RC = (2pi * RC * PLL_Based) / sync period count,
Ideal SYNC delay parameter = 2pi * RC * PLL_Based
The real delay parameter may be calibrated because the RC constant is an approximate value.
*/
#define Default_SYNC_Delay      418     // 2pi * RC * PLL_Based = 2 * 3.14 * 2.7m * 25k = 426 - 8(calibration value)
                                    
// RMS based frequency (Hz)
#define RMS_Based               8000

// Frequency lockable range (Hz)
#define High_SYNC_Frequency     70
#define Low_SYNC_Frequency      40

// Absolute frequency range (Hz)
#define Max_SYNC_Frequency      90
#define Min_SYNC_Frequency      30

#if High_SYNC_Frequency>Max_SYNC_Frequency          
#define High_SYNC_Frequency     Max_SYNC_Frequency
#elif High_SYNC_Frequency<Min_SYNC_Frequency            
#define High_SYNC_Frequency     Min_SYNC_Frequency
#endif          
            
#if Low_SYNC_Frequency>Max_SYNC_Frequency          
#define Low_SYNC_Frequency      Max_SYNC_Frequency
#elif Low_SYNC_Frequency<Min_SYNC_Frequency            
#define Low_SYNC_Frequency      Min_SYNC_Frequency
#endif

#if High_SYNC_Frequency<Low_SYNC_Frequency
#define Low_SYNC_Frequency      (High_SYNC_Frequency + 1)
#endif

// Macro command
#define Create_RMS_Data(Name, \
                        Gain, Offset, Scale)        RmsDataType Name = \
                                                    {\
                                                        {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0}, 0, 0,\
                                                        Gain, Offset, Scale\
                                                    }

#define Create_AVG_Data(Name, AdcStr, \
                        Gain, Offset, Scale)        AvgDataType Name = \
                                                    {\
                                                        {\
                                                            0, 0, 0, 0, 0, 0,\
                                                            Default_2nd_A0, Default_2nd_A1,\
                                                            Default_2nd_B2, &AdcStr\
                                                        },\
                                                        Gain, Offset, Scale\
                                                    }

#define Create_Sync_Data(Name, Delay_Parameter)     SyncType Name = {0, 0, 0, 0, 0, 0, 0, 0, 0, Delay_Parameter}

/* Type definitions */
// Enumeration
enum SyncLimitEnum
{
    MAX_SYNC_HALF   = (PLL_Based / (2 * Low_SYNC_Frequency)) * 13 / 10, // x1.3
    MAX_SYNC_PERIOD = MAX_SYNC_HALF * 2,
    MIN_SYNC_HALF   = PLL_Based / (2 * High_SYNC_Frequency),
    MIN_SYNC_PERIOD = MIN_SYNC_HALF * 2,
    MAX_EDGE_COUNT  = RMS_Based / (2 * Low_SYNC_Frequency),
    MIN_EDGE_COUNT  = RMS_Based / (2 * High_SYNC_Frequency)
};	

// Structure & union
union SyncUnion
{
	unsigned short all;
    
    struct
    {
        unsigned short update           :1;     // SYNC state update flag
        unsigned short fail             :1;     // SYNC failed flag
        unsigned short rcDelayUpdate    :1;     // Reserved for RC delay value asynchronous calculation
        unsigned short reserved1        :5;

        unsigned short reserved2        :8;
    }bit;
};

typedef struct
{
	union SyncUnion state;
	unsigned char level;                        // SYNC level
	unsigned short count;                       // SYNC count
	unsigned short failCount;                   // Fail count
	unsigned short half;                        // Half period of SYNC
	unsigned short shift;                       // Shift value of SYNC
	unsigned short period;                      // Period value of SYNC
	unsigned short rcDelayCount;                // Reserved for RC delay value asynchronous calculation
	unsigned long rcDelay;                      // RC delay value of SYNC
	unsigned long delayParameter;               // Delay parameter of SYNC 
}SyncType;

typedef struct
{
    union
    {
        unsigned short all;
        
        struct
        {
            unsigned short trigger      :1;
            unsigned short update       :1;
            unsigned short fail         :1;
            unsigned short count        :13;
        }bit;
    }state;
    
 	unsigned long squareSum;
    unsigned long squareAvg;
    long rms;
}RmsType;

typedef struct 
{
	union
	{
		unsigned short all;
        
		struct
		{
			unsigned char positive;
			unsigned char negative;
		}byte;	
	}state;
    
	unsigned char oldClock;
	unsigned char referenceClock;
}EdgeType;

typedef struct
{
    RmsType positive;
    RmsType negative;
    EdgeType edge;
    long periodRms;
    long rmsQ11;
	long gain;
	long offset;
	long scale;
}RmsDataType;

typedef struct
{
	SecondOrderType avg;
    long gain;
	long offset;
	long scale;
}AvgDataType;

/* Global function prototypes */
void Detect_SYNC_Routine(char signal, SyncType *sync, SineProcessObj *sineObj);
unsigned char SYNC_Info_Update(SyncType *sync, unsigned short timeout);
void SqSum_Calculation(RmsDataType *data, long feed);
void RMS_Update(RmsDataType *data);
long AVG_Calculation(AvgDataType *data, char mode);

/* Global data declarations */

#endif
