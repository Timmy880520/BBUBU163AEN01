/**************************************************************************************************
File Name: AC_Math_Lib.c
External Data:
    Name                                    Source
    --------------------------------------- ------------------------------------------------------
    DIVISION_TABLE                          Math_Table.c
    ATAN_TABLE                              Math_Table.c
External Functions:
    Name                                    Source
    --------------------------------------- ------------------------------------------------------
    DLPF_Calculation                        Univ_Lib.c
Description: 
    AC mathematical library.

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- ------------------------------------------------------------------
    06/13/2017 Watch Lee        1. version 1.0.
    08/27/2018 Watch Lee        1. version 2.0.
                                2. modify to fit standard coding style.
    02/27/2020 Watch Lee        1. version 2.1.
                                2. modify Detect_SYNC_Routine to support period count > 512.

**************************************************************************************************/

/* Includes */ 
#include "..\inc\AC_Math_Lib.h"

/**************************************************************************************************
Function Name:
    void Detect_SYNC_Routine(char signal, SyncType *sync, SineProcessObj *sineObj)
Input:
    signal   - Input synchronal signal for PLL.
    *sync    - Object pointer of synchronization. 
    *sineObj - Object pointer of sine process. 
Output:
    None. 
Comment:
    This function will detect the sync signal to synchronize the sine process function.
**************************************************************************************************/
void Detect_SYNC_Routine(char signal, SyncType *sync, SineProcessObj *sineObj)
{		
    unsigned long calcuTmp;
    
    if (signal)
    {
        if (!sync->level)
        {			
            sync->half = sync->count >> 1;
            
            if (sync->count > MIN_SYNC_PERIOD && sync->count < MAX_SYNC_PERIOD)
            {				 
                sync->state.bit.update = 1;         
                calcuTmp = (sync->delayParameter * DIVISION_TABLE[(sync->half << 1) & 0x3FF]) >> 13;
                sync->rcDelay = (sync->half * ATAN_TABLE[calcuTmp & 0x1FF]) >> 16;      // calcuTmp < 512
                
                if (sync->period > sync->half) 
                    sync->shift = (sync->period + sync->half) >> 1;				
                
                if (sync->shift > sync->rcDelay) 
                    sync->shift -= sync->rcDelay;
            }
            else 
                sync->state.bit.update = 0;          
            sync->count = 0;
        }
    
        sync->level = 1;
        
        if (sync->count == sync->shift && sync->state.bit.update)
        {
            sineObj->update(sineObj, sync->half);
        }
    
        if (sync->count < MAX_SYNC_HALF) 
            sync->count++;
        else 
            sync->state.bit.update = 0;
    }
    else if (sync->count > MIN_SYNC_HALF)
    {		
        if (sync->level) 
            sync->period = sync->count;
        sync->level = 0;		
            
        if (sync->count < MAX_SYNC_PERIOD) 
            sync->count++;
        else 
            sync->state.bit.update = 0;
    }
    else
    {		
        if (sync->count > MIN_SYNC_HALF) 
            sync->level = 0;
        sync->count++;		
    }
}
/************************************************************************************************
Function Name:
    unsigned char SYNC_Info_Update(SyncType *sync, unsigned short timeout)
Input:
    *sync    - Object pointer of synchronization. 
    timeout  - Timeout value.  
Output:
    return   - Fail flag of SYNC. 
Comment:
    SYNC fail info function. This function will set and return the fail flag if the counter is 
reached the timeout value. 
************************************************************************************************/
unsigned char SYNC_Info_Update(SyncType *sync, unsigned short timeout)
{		
	// Check SYNC Fail ---------------------------------------------------------------------- 
	if (!sync->state.bit.update) 
	{
		if (++sync->failCount >= timeout)
		{
			sync->state.bit.fail = 1;
			sync->failCount = 0;		
		}
	}
	else 
	{
		sync->state.bit.fail = 0;	
		sync->failCount = 0;
	}	
	return sync->state.bit.fail;
}
/**************************************************************************************************
Function Name:
    void SqSum_Calculation(RmsDataType *data, long feed)
Input:
    *data    - Object pointer of RMS data. 
    feed     - Input value of the calculation.   
Output:
    None. 
Comment:
    Square sum of period calculation function.
**************************************************************************************************/
void SqSum_Calculation(RmsDataType *data, long feed)
{
	// Check Pos/Neg edge
	if (data->edge.oldClock != data->edge.referenceClock)
	{
		data->edge.oldClock = data->edge.referenceClock;
        
		if (data->edge.referenceClock) 
            data->edge.state.byte.positive = 1;     // PosEdge
		else 
            data->edge.state.byte.negative = 1;     // NegEdge
	}
	else data->edge.state.all = 0;

	// Data square average calculate
	if (!data->edge.state.all)                      // Q12
	{
		if (data->positive.state.bit.trigger)
		{			
			if (!data->positive.state.bit.update && !data->positive.state.bit.fail)
			{
				data->positive.squareSum += (feed * feed) >> 2;		// SUM(Q11 * Q11) = SUM(Q22) ~ Q9 * Q22 = Q31
				data->positive.state.bit.count++;			
			}
		}
		else if (data->negative.state.bit.trigger)
		{			
			if (!data->negative.state.bit.update && !data->negative.state.bit.fail)
			{		
				data->negative.squareSum += (feed * feed) >> 2;		// SUM(Q11 * Q11) = SUM(Q22) ~ Q9 * Q22 = Q31
				data->negative.state.bit.count++;
			}
		}
	}
	else
	{
		data->positive.state.bit.trigger = data->edge.state.byte.positive;
		data->negative.state.bit.trigger = data->edge.state.byte.negative;
		
		if (data->positive.state.bit.trigger)
		{			
			if (data->negative.state.bit.count > MIN_EDGE_COUNT && data->negative.state.bit.count < MAX_EDGE_COUNT) 
                data->negative.state.bit.update = 1;
			else 
                data->negative.state.bit.fail = 1;
		}
		else if (data->negative.state.bit.trigger)
		{	
			if (data->positive.state.bit.count > MIN_EDGE_COUNT && data->positive.state.bit.count < MAX_EDGE_COUNT) 
                data->positive.state.bit.update = 1;
			else 
                data->positive.state.bit.fail = 1;
		}
	}	
}
/************************************************************************************************
Function Name:
    void RMS_Update(RmsDataType *data)
Input:
    *data    - Object pointer of RMS data. 
Output:
    None. 
Comment:
    RMS value calculation function.
************************************************************************************************/
void RMS_Update(RmsDataType *data)
{
    unsigned long tmp, inverseAmount = 0;
    
    if (data->positive.state.bit.fail || data->negative.state.bit.fail)
    {
        // Positive period fail
        if (data->positive.state.bit.fail)
        {		
            data->positive.state.bit.count = 0;
            data->positive.squareSum = 0;
            data->positive.state.bit.fail = 0;
        }
        
        // Negative period fail
        if (data->negative.state.bit.fail)
        {	
            data->negative.state.bit.count = 0;
            data->negative.squareSum = 0;		
            data->negative.state.bit.fail = 0;				
        }
    }
    else if (data->positive.state.bit.update)
    {
        // Calculate Square average value
        inverseAmount = DIVISION_TABLE[(data->positive.state.bit.count + 1) & 0x1FF] >> 2;                      // Q19
        tmp = (data->positive.squareSum >> 16) * inverseAmount;			
        data->positive.squareAvg = (tmp + (((data->positive.squareSum & 0xFFFF) * inverseAmount) >> 16)) >> 3;  // Q11   
        data->positive.state.bit.count = 0;
        data->positive.squareSum = 0;
        
        // Calculate Square root value		
        data->positive.rms = Sqrt_Calculation((long)data->positive.squareAvg);                      // Q11
        data->positive.rms = (((data->positive.rms) * data->gain) >> 15) + (data->offset >> 4);     // x(Gain / 2^15)
        data->positive.rms = (data->positive.rms > 0) ? data->positive.rms : 0;	       
        data->positive.state.bit.update = 0;
    }
    else if (data->negative.state.bit.update)
    {
        // Calculate Square average value
        inverseAmount = DIVISION_TABLE[(data->negative.state.bit.count + 1) & 0x1FF] >> 2;                      // Q19
        tmp = (data->negative.squareSum >> 16) * inverseAmount;			
        data->negative.squareAvg = (tmp + (((data->negative.squareSum & 0xFFFF) * inverseAmount) >> 16)) >> 3;  // Q11  
        data->negative.state.bit.count = 0;
        data->negative.squareSum = 0;
        
        // Calculate Square root value
        data->negative.rms = Sqrt_Calculation((long)data->negative.squareAvg);                      // Q11
        data->negative.rms = (((data->negative.rms) * data->gain) >> 15) + (data->offset >> 4);     // x(Gain / 2^15)
        data->negative.rms = (data->negative.rms > 0) ? data->negative.rms : 0;	
        
        // Calculate one cycle square root value
        data->rmsQ11 = Sqrt_Calculation((long)((data->negative.squareAvg + data->positive.squareAvg) >> 1));      
        data->periodRms = (data->scale * data->rmsQ11) >> 11;				
        data->negative.state.bit.update = 0;
    }
}
/************************************************************************************************
Function Name:
    long AVG_Calculation(AvgDataType *data, char mode)
Input:
    *data    - Object pointer of Average data. 
    mode     - Output mode.
Output:
    return   - Calculation result. RMS value (mode > 0) or average value (mode = 0).  
Comment:
    2nd DLPF for average/approximate RMS calculation. The default sampling frequency and cut-off 
frequency refers to Univ_Lib.h.
************************************************************************************************/
long AVG_Calculation(AvgDataType *data, char mode)
{
    long dataAvg, dataRms;
    
    // 2nd digital low pass filter for calculate VAC average value	
    DLPF_Calculation(&data->avg);
    
    // RMS value calculate (for sine wave approximate: RMS = (Pi / sqrt(2)) * AVG = 1.11 * Vavg)
    dataAvg = (((data->avg.par.yn >> 14) * data->gain) >> 15) + (data->offset >> 3);    // Q12
    dataAvg = (dataAvg > 0) ? dataAvg : 0;
    
    dataAvg = (dataAvg * data->scale) >> 12;                                            // Q0
    dataRms = (dataAvg * 1137) >> 10;
    
    if (mode > 0) 
        return dataRms;
    else 
        return dataAvg;
}
// ------------------------------------ End Line --------------------------------------------------
