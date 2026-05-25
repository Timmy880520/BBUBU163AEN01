/**************************************************************************************************
File Name: Sine_Process.c
External Data:
    Name                                    Source
    --------------------------------------- ------------------------------------------------------
    SIN_TABLE                               Math_Table.c
    DIVISION_TABLE                          Math_Table.c
External Functions:
    Name                                    Source
    --------------------------------------- ------------------------------------------------------
    None
Description: 
    1pu sine wave with offset generation function.

==================================================================================================
History:
    Date       Author           Description of Change
    ---------- ---------------- ------------------------------------------------------------------
    03/23/2012 Watch Lee        1. version 1.0.
    04/18/2018 Watch Lee        1. version 2.0.
                                2. modify to fit standard coding style.
    09/10/2018 Watch Lee        1. version 2.1.
                                2. add the clockOut member. 

**************************************************************************************************/

/* Includes */ 
#include "..\inc\Sine_Process.h"

/**************************************************************************************************
Function Name:
    void Sine_Process(SineProcessObj *obj)
Input:
    *obj     - Object pointer of sine process. 
Output:
    None. 
Comment:
    Sine process function. This function generate the bilateral symmetry sine value when it is 
periodic called. The sine value is 1pu data with Q12 format.
**************************************************************************************************/
void Sine_Process(SineProcessObj *obj)
{	
    // Update step
    if (obj->count < obj->half)
    {
        if (obj->count <= obj->quarter) 
            obj->index = ((long)obj->count * DIVISION_TABLE[obj->quarter]) >> 11;
        else 
            obj->index = ((long)(obj->half - obj->count) * DIVISION_TABLE[obj->quarter]) >> 11;
    
        // Update sine value
        obj->sineValue = Sine_Offset + ((Sine_Gain * (SIN_TABLE[obj->index] >> 4)) >> 18);
    }
    else 
    { 
        if ((obj->count - obj->half) <= obj->quarter) 
            obj->index = ((long)(obj->count - obj->half) * DIVISION_TABLE[obj->quarter]) >> 11;
        else 
            obj->index = ((long)((obj->half << 1) - obj->count) * DIVISION_TABLE[obj->quarter]) >> 11;
        
        // Update sine value
        obj->sineValue = Sine_Offset - ((Sine_Gain * (SIN_TABLE[obj->index] >> 4)) >> 18);
    }
            
    // Check period
    if (obj->count < (obj->period - 1)) 
    {
        obj->count++;
        
        if (obj->count == obj->half)
        {
            obj->index = 0;
            obj->clockOut = 0;
        }
        else if (obj->count < obj->half)
            obj->clockOut = 1;
    }
    else 
    {
        Default_Update(obj, 0);
    }
}
/**************************************************************************************************
Function Name:
    void Default_Update(SineProcessObj *obj)
Input:
    *obj     - Object pointer of sine process.
    newHalf  - New half period, 0 for ignore. 
Output:
    None. 
Comment:
    Default sine half period update function.
**************************************************************************************************/
void Default_Update(SineProcessObj *obj, unsigned long newHalf)
{
    if (newHalf && obj->half != newHalf)
    {
        obj->half = newHalf;		
        obj->quarter = obj->half >> 1;
        obj->period = obj->half << 1;
    }
    obj->count = 0;
    obj->index = 0;
    obj->clockOut = 0;
}
//---------------- END LINE -----------------------------------------------------------------------
