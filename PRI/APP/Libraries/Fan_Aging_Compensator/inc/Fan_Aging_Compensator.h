/**************************************************************************************************
File Name: Fan_Aging_Compensator.h
Global Data:
    Name                 Type               Description
    -------------------- ------------------ -----------------------------------------------------
    FAN_FG_TABLE         const long array   Converts fan FG signal to the speed value.
    FAN1_DUTY_TABLE      const long array   Converts PWM duty to the speed value.
Description: 
    Header file of FanAging_Compensator.c

==================================================================================================
History:
     Date       Author           Description of Change
     ---------- ---------------- -----------------------------------------------------------------
     10/23/2017 Watch Lee        1. Ver.1.0.
     11/13/2017 Watch Lee        1. Ver.1.1.
                                 2. Add limitations of the target fan speed for fan fault judge.
     02/12/2019 Watch Lee        1. Ver.1.2.
                                 2. Change target RPM calculation to table look-up.

**************************************************************************************************/

#ifndef __FANAGING_COMPENSATOR_H__
#define __FANAGING_COMPENSATOR_H__

/* Macro definitions */
// Fan1 aging compensation parameters
#define Fan1_maxFanRPM              23000       // RPM
#define Fan1_maxFanPwmCount         4000        // PWM output compare value of 100% duty
#define Fan1_slideValue             5           // Range: 0 - 100 (100 for no damping)
#define Fan1_maxCompensateGain      10          // 10%
#define Fan1_errorThreshold         5           // 5% of maximum fan speed
#define Fan1_operateRange           50          // 50% of target fan speed

// Default parameters for the FanAgingStr 
#define FanAging1_Defaults          { \
                                        { \
                                            { \
                                                Fan1_maxFanRPM,\
                                                Fan1_maxFanPwmCount,\
                                                Fan1_slideValue,\
                                                32768 * Fan1_maxCompensateGain / 100,\
                                                (long)Fan1_errorThreshold * Fan1_maxFanRPM / 100,\
                                                32768 * Fan1_maxFanPwmCount / 100,\
                                                32768 * Fan1_maxFanPwmCount / Fan1_maxFanRPM,\
                                                32768 * Fan1_operateRange / 100\
                                            },\
                                            FAN1_DUTY_TABLE,\
                                            0, 0, 0, 0, 0, 0, 0 \
                                        },\
                                        Fan_Aging_Compensator\
                                    }

/* Type definitions */
// Struct & union
typedef struct FanAgingStr FanAgingObj;

struct FanAgingStr
{
    struct
    {    
        struct
        {
            long maxFanRPM;
            long maxFanPwmCount;
            long slideValue;
            long maxCompensateGain;
            long errorThreshold;
            long fanPwmCountGain;
            long compensateGain;
            long operateLimitGain;
        }par;

        const unsigned short *dutyTable;
        unsigned long targetRPM;
        unsigned long lowLimitRPM;
        unsigned long upLimitRPM;
        unsigned long maxCompensateRPM;
        unsigned long compensateRPM;
        unsigned long compensateCount;
        unsigned long dampingDuty;
    }member;
    
	unsigned short (*compensator)(FanAgingObj*, unsigned short, unsigned short, unsigned char);
};

/* Global function prototypes */
unsigned short Fan_Aging_Compensator(FanAgingObj *obj, unsigned short duty, unsigned short rpm, unsigned char enable);

/* Global data declarations */
extern const unsigned short FAN_FG_TABLE[1024], FAN1_DUTY_TABLE[101];

#endif
