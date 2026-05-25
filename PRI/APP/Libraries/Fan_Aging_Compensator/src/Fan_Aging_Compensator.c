/**************************************************************************************************
File Name: Fan_Aging_Compensator.c
External Data:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None
External Functions:
    Name                                    Source
    --------------------------------------- -----------------------------------------------------
    None
Description:  
    Universal fan aging compensation program, the program includes the IDC fan aging compensation 
and damping duty for softly increase the fan speed.

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

/* Includes */
#include "Fan_Aging_Compensator.h"

/**************************************************************************************************
Function Name:
	unsigned short Fan_Aging_Compensator(FanAgingStr *obj,
                                         unsigned short duty, unsigned short rpm, unsigned char enable)
Input:
    *obj      - Object pointer of fan aging compensator.
    duty      - Target duty(%).
    rpm       - Fan speed reading data(RPM).
    enable    - 1/0 for enable/disable fan aging compensation.
Output:
	return    - PWM output compare value.
Comment:
	Fan aging compensation function. This function contains the fan aging compensation and soft 
start of PWM duty. 	
**************************************************************************************************/
unsigned short Fan_Aging_Compensator(FanAgingObj *obj,
                                     unsigned short duty, unsigned short rpm, unsigned char enable)		
{			
        // Control duty to ramp up and immediately down for soft start of PWM duty
	duty = (duty > 100) ? 100 : duty;
	
	if (obj->member.dampingDuty != duty)
	{
		if ((obj->member.dampingDuty + obj->member.par.slideValue) < duty) 
			obj->member.dampingDuty += obj->member.par.slideValue;
		else 
			obj->member.dampingDuty = duty;
		enable = 0;
	}

	// Calculate target RPM, lower limitation of RPM, upper limitation of RPM, and maximum compensation value
        obj->member.targetRPM = obj->member.dutyTable[duty];
    
	obj->member.lowLimitRPM = obj->member.targetRPM - 
                              ((obj->member.targetRPM * obj->member.par.operateLimitGain) >> 15);
    
	obj->member.upLimitRPM = obj->member.targetRPM + 
                             ((obj->member.targetRPM * obj->member.par.operateLimitGain) >> 15);
    
	obj->member.maxCompensateRPM = (obj->member.targetRPM * obj->member.par.maxCompensateGain) >> 15;

	// Calculate compensation value
	if (enable)
	{	
		if (rpm < obj->member.targetRPM && obj->member.compensateRPM < obj->member.maxCompensateRPM) 
			obj->member.compensateRPM++;
		else if (rpm > (obj->member.targetRPM + obj->member.par.errorThreshold) && obj->member.compensateRPM > 0) 
			obj->member.compensateRPM--;
	}
	else 
		obj->member.compensateRPM = 0;
	
	obj->member.compensateCount = ((obj->member.par.fanPwmCountGain * duty) >> 15) + 
                                  ((obj->member.compensateRPM * obj->member.par.compensateGain) >> 15);
	
	if (obj->member.compensateCount > obj->member.par.maxFanPwmCount) 
		return obj->member.par.maxFanPwmCount;
	else 
		return obj->member.compensateCount;
}
//---------------- END LINE -----------------------------------------------------------------------
